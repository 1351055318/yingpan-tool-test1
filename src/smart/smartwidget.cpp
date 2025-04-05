#include "smartwidget.h"
#include "../core/smartattributedict.h"

#include <QDebug>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QComboBox>
#include <QPushButton>
#include <QTableWidget>
#include <QHeaderView>
#include <QGroupBox>
#include <QFileDialog>
#include <QMessageBox>
#include <QDateTime>
#include <QStandardPaths>
#include <QFile>
#include <QTextStream>
#include <QProgressBar>
#include <QTimer>
#include <QtCharts/QPieSeries>
#include <QtCharts/QChart>
#include <QtCharts/QPieSlice>
#include <QBrush>
#include <QtConcurrent>

QT_CHARTS_USE_NAMESPACE

#include "../core/diskutils.h"
#include "../core/smartfactory.h"

SmartWidget::SmartWidget(QWidget *parent) :
    QWidget(parent),
    m_currentDiskIndex(-1),
    m_smartData(nullptr),
    m_language(SmartAttributeLanguage::Chinese) // 默认使用中文
{
    setupUI();
    refreshDiskList();
    
    // 设置定时刷新
    m_refreshTimer = new QTimer(this);
    connect(m_refreshTimer, &QTimer::timeout, this, &SmartWidget::onTimerTimeout);
    m_refreshTimer->start(300000);  // 每5分钟刷新一次，避免频繁刷新导致卡顿
}

SmartWidget::~SmartWidget()
{
    if (m_refreshTimer) {
        m_refreshTimer->stop();
    }
    
    if (m_smartData) {
        delete m_smartData;
        m_smartData = nullptr;
    }
}

void SmartWidget::setupUI()
{
    // 创建主布局
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(15, 15, 15, 15);
    m_mainLayout->setSpacing(15);
    
    // 创建控制区域
    m_controlLayout = new QHBoxLayout();
    m_diskLabel = new QLabel("选择硬盘:", this);
    m_diskComboBox = new QComboBox(this);
    m_refreshButton = new QPushButton("刷新", this);
    m_saveInfoButton = new QPushButton("保存信息", this);
    m_languageToggleButton = new QPushButton(this); // 新增语言切换按钮
    updateLanguageButtonText(); // 设置按钮文本
    
    m_controlLayout->addWidget(m_diskLabel);
    m_controlLayout->addWidget(m_diskComboBox, 1);
    m_controlLayout->addWidget(m_refreshButton);
    m_controlLayout->addWidget(m_saveInfoButton);
    m_controlLayout->addWidget(m_languageToggleButton);
    
    // 创建内容区域
    m_contentLayout = new QHBoxLayout();
    
    // 左侧 - SMART属性表格
    m_attributesGroup = new QGroupBox("SMART属性", this);
    QVBoxLayout *attributesLayout = new QVBoxLayout(m_attributesGroup);
    
    m_attributesTable = new QTableWidget(this);
    m_attributesTable->setColumnCount(7);
    m_attributesTable->setHorizontalHeaderLabels(QStringList() 
                                                << "ID" 
                                                << "属性名称" 
                                                << "当前值" 
                                                << "最差值" 
                                                << "阈值" 
                                                << "原始值" 
                                                << "状态");
    
    m_attributesTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_attributesTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_attributesTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_attributesTable->setAlternatingRowColors(true);
    
    attributesLayout->addWidget(m_attributesTable);
    
    // 右侧 - 摘要信息
    m_infoLayout = new QVBoxLayout();
    
    // 健康状态
    m_healthGroup = new QGroupBox("硬盘健康状态", this);
    m_healthLayout = new QVBoxLayout(m_healthGroup);
    
    m_healthScoreLabel = new QLabel("健康评分: N/A", this);
    QFont healthFont = m_healthScoreLabel->font();
    healthFont.setBold(true);
    healthFont.setPointSize(healthFont.pointSize() + 2);
    m_healthScoreLabel->setFont(healthFont);
    m_healthScoreLabel->setAlignment(Qt::AlignCenter);
    
    m_healthScoreBar = new QProgressBar(this);
    m_healthScoreBar->setRange(0, 100);
    m_healthScoreBar->setValue(0);
    m_healthScoreBar->setTextVisible(true);
    
    // 创建健康状态饼图
    QChart *healthChart = new QChart();
    healthChart->setTitle("健康状态分布");
    healthChart->setAnimationOptions(QChart::SeriesAnimations);
    healthChart->legend()->setVisible(true);
    healthChart->legend()->setAlignment(Qt::AlignBottom);
    
    QPieSeries *healthSeries = new QPieSeries();
    healthSeries->append("健康", 100);
    healthSeries->append("损耗", 0);
    
    // 设置颜色
    healthSeries->slices().at(0)->setBrush(QColor(50, 205, 50));  // 绿色
    healthSeries->slices().at(1)->setBrush(QColor(192, 192, 192)); // 灰色
    
    // 突出健康状态
    healthSeries->slices().at(0)->setExploded(true);
    healthSeries->slices().at(0)->setLabelVisible(true);
    
    healthChart->addSeries(healthSeries);
    
    m_healthChartView = new QChartView(healthChart);
    m_healthChartView->setRenderHint(QPainter::Antialiasing);
    m_healthChartView->setMinimumHeight(200);
    
    m_healthLayout->addWidget(m_healthScoreLabel);
    m_healthLayout->addWidget(m_healthScoreBar);
    m_healthLayout->addWidget(m_healthChartView);
    
    // 摘要信息
    m_summaryGroup = new QGroupBox("重要指标", this);
    m_summaryLayout = new QVBoxLayout(m_summaryGroup);
    
    m_deviceTypeLabel = new QLabel("设备类型: N/A", this);
    m_temperatureLabel = new QLabel("温度: N/A", this);
    m_powerOnLabel = new QLabel("通电时间: N/A", this);
    m_startStopLabel = new QLabel("启动/停止次数: N/A", this);
    m_warningLabel = new QLabel("警告: 无", this);
    
    QFont summaryFont = m_temperatureLabel->font();
    summaryFont.setBold(true);
    m_deviceTypeLabel->setFont(summaryFont);
    m_temperatureLabel->setFont(summaryFont);
    m_powerOnLabel->setFont(summaryFont);
    m_startStopLabel->setFont(summaryFont);
    
    // 警告使用不同颜色
    QPalette warningPalette = m_warningLabel->palette();
    warningPalette.setColor(QPalette::WindowText, QColor(255, 165, 0));  // 橙色
    m_warningLabel->setPalette(warningPalette);
    
    m_summaryLayout->addWidget(m_deviceTypeLabel);
    m_summaryLayout->addWidget(m_temperatureLabel);
    m_summaryLayout->addWidget(m_powerOnLabel);
    m_summaryLayout->addWidget(m_startStopLabel);
    m_summaryLayout->addWidget(m_warningLabel);
    m_summaryLayout->addStretch();
    
    // 添加到右侧布局
    m_infoLayout->addWidget(m_healthGroup);
    m_infoLayout->addWidget(m_summaryGroup);
    
    // 设置左右比例
    m_contentLayout->addWidget(m_attributesGroup, 2);
    m_contentLayout->addLayout(m_infoLayout, 1);
    
    // 添加所有元素到主布局
    m_mainLayout->addLayout(m_controlLayout);
    m_mainLayout->addLayout(m_contentLayout, 1);
    
    // 连接信号槽
    connect(m_diskComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &SmartWidget::onDiskSelectionChanged);
    connect(m_refreshButton, &QPushButton::clicked, this, &SmartWidget::onRefreshClicked);
    connect(m_saveInfoButton, &QPushButton::clicked, this, &SmartWidget::onSaveSmartInfoClicked);
    connect(m_languageToggleButton, &QPushButton::clicked, this, &SmartWidget::onLanguageToggleClicked);
}

void SmartWidget::refreshDiskList()
{
    // 保存当前选择
    QString currentDiskPath;
    if (m_currentDiskIndex >= 0 && m_currentDiskIndex < m_diskList.size()) {
        currentDiskPath = m_diskList[m_currentDiskIndex].diskPath;
    }
    
    // 清除旧数据
    m_diskComboBox->clear();
    m_diskList.clear();
    
    // 使用SmartFactory检测系统中的硬盘
    QList<DiskSMARTInfo> smartDisks = SmartFactory::detectDisks();
    
    // 转换为界面所需的DiskInfo格式
    for (const DiskSMARTInfo &smartDisk : smartDisks) {
        DiskInfo diskInfo;
        diskInfo.index = m_diskList.size();
        diskInfo.diskPath = smartDisk.diskPath;
        diskInfo.model = smartDisk.model;
        diskInfo.serialNumber = smartDisk.serialNumber;
        diskInfo.diskSize = smartDisk.size;
        
        // 根据类型设置接口类型
        if (smartDisk.diskType == DiskType::SATA) {
            diskInfo.type = "SATA";
            diskInfo.interfaceSpeed = "6.0 Gb/s";
        } else if (smartDisk.diskType == DiskType::NVMe) {
            diskInfo.type = "NVMe";
            diskInfo.interfaceSpeed = "PCIe 3.0 x4";
        } else {
            diskInfo.type = "Unknown";
            diskInfo.interfaceSpeed = "Unknown";
        }
        
        // 添加到列表
        m_diskList.append(diskInfo);
        
        // 添加到下拉框
        QString displayName = QString("%1: %2 (%3)")
                            .arg(diskInfo.diskPath)
                            .arg(diskInfo.model)
                            .arg(DiskUtils::formatSize(diskInfo.diskSize));
        m_diskComboBox->addItem(displayName);
    }
    
    // 如果没有找到任何磁盘，显示一个默认项
    if (m_diskList.isEmpty()) {
        m_diskComboBox->addItem("未找到磁盘");
        m_diskComboBox->setEnabled(false);
        m_currentDiskIndex = -1;
        return;
    }
    
    // 启用下拉框
    m_diskComboBox->setEnabled(true);
    
    // 尝试恢复先前选择
    int newIndex = -1;
    for (int i = 0; i < m_diskList.size(); i++) {
        if (m_diskList[i].diskPath == currentDiskPath) {
            newIndex = i;
            break;
        }
    }
    
    // 如果找不到先前选择，使用第一个项
    if (newIndex == -1) {
        newIndex = 0;
    }
    
    // 设置选中项但避免触发信号
    m_diskComboBox->blockSignals(true);
    m_diskComboBox->setCurrentIndex(newIndex);
    m_diskComboBox->blockSignals(false);
    
    // 手动更新当前选择
    m_currentDiskIndex = newIndex;
    
    // 如果界面可见，加载选中磁盘的SMART数据
    if (isVisible()) {
        loadSmartData(m_currentDiskIndex);
    }
}

void SmartWidget::onDiskSelectionChanged(int index)
{
    if (index >= 0 && index < m_diskList.size()) {
        m_currentDiskIndex = index;
        loadSmartData(m_currentDiskIndex);
    }
}

void SmartWidget::onRefreshClicked()
{
    // 禁用刷新按钮，防止重复点击
    m_refreshButton->setEnabled(false);
    
    // 显示正在刷新状态
    m_refreshButton->setText("刷新中...");
    
    // 创建一个短暂延迟，让UI能够更新
    QTimer::singleShot(100, this, [this]() {
        // 刷新磁盘列表
        refreshDiskList();
        
        // 如果当前有选中的磁盘，重新加载SMART数据
        if (m_currentDiskIndex >= 0 && m_currentDiskIndex < m_diskList.size()) {
            loadSmartData(m_currentDiskIndex);
        } else if (!m_diskList.isEmpty()) {
            // 否则选择第一个磁盘
            m_diskComboBox->setCurrentIndex(0);
        }
    });
}

void SmartWidget::onSaveSmartInfoClicked()
{
    if (m_currentDiskIndex < 0 || m_currentDiskIndex >= m_diskList.size()) {
        QMessageBox::warning(this, "错误", "请先选择一个硬盘");
        return;
    }
    
    // 获取保存路径
    QString documentsPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    QString defaultFileName = QString("%1/SMART信息_%2_%3.txt")
                            .arg(documentsPath)
                            .arg(m_diskList[m_currentDiskIndex].model.replace(" ", "_"))
                            .arg(QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss"));
    
    QString filePath = QFileDialog::getSaveFileName(this, "保存SMART信息",
                                                  defaultFileName,
                                                  "文本文件 (*.txt)");
    
    if (filePath.isEmpty()) {
        return;
    }
    
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "保存失败", "无法创建文件: " + filePath);
        return;
    }
    
    QTextStream out(&file);
    out.setCodec("UTF-8");
    
    const DiskInfo &disk = m_diskList[m_currentDiskIndex];
    
    // 写入硬盘基本信息
    out << "===== 硬盘信息 =====" << "\n\n";
    out << "设备名称: " << disk.diskPath << "\n";
    out << "型号: " << disk.model << "\n";
    out << "序列号: " << disk.serialNumber << "\n";
    out << "固件版本: " << disk.firmwareVersion << "\n";
    out << "接口类型: " << disk.interfaceSpeed << "\n";
    out << "硬盘容量: " << DiskUtils::formatSize(disk.diskSize) << "\n";
    
    // 添加设备类型信息
    QString deviceType;
    switch (m_smartData->diskType()) {
        case DiskType::SATA:
            deviceType = "SATA/ATA设备";
            break;
        case DiskType::NVMe:
            deviceType = "NVMe设备";
            break;
        default:
            deviceType = "未知类型设备";
            break;
    }
    out << "设备类型: " << deviceType << "\n\n";
    
    // 写入SMART摘要信息
    out << "===== SMART摘要 =====" << "\n\n";
    out << "总体健康: " << m_smartData->overallHealth() << "/100\n";
    
    int temperature = m_smartData->temperature();
    if (temperature > 0) {
        out << "温度: " << temperature << "°C\n";
    }
    
    int powerOnHours = m_smartData->powerOnHours();
    if (powerOnHours >= 0) {
        out << "通电时间: " << powerOnHours << " 小时";
        if (powerOnHours > 24) {
            out << " (" << powerOnHours / 24 << " 天 " << powerOnHours % 24 << " 小时)";
        }
        out << "\n";
    }
    
    int startStopCount = m_smartData->powerCycleCount();
    if (startStopCount >= 0) {
        out << "启动/停止次数: " << startStopCount << " 次\n";
    }
    
    // 写入警告
    QStringList warnings = m_smartData->warnings();
    if (!warnings.isEmpty()) {
        out << "\n警告:\n";
        for (const QString &warning : warnings) {
            out << "- " << warning << "\n";
        }
    }
    
    // 写入SMART属性详情
    out << "\n===== SMART属性详情 =====" << "\n\n";
    out << QString("%1\t%2\t%3\t%4\t%5\t%6\t%7\t%8\n")
           .arg("ID", 4)
           .arg("属性名称", -25)
           .arg("当前值", 8)
           .arg("最差值", 8)
           .arg("阈值", 8)
           .arg("原始值", 12)
           .arg("状态", 8)
           .arg("描述", -20);
    
    out << QString("%1\n").arg("-", 100, '-');
    
    QList<SmartAttribute> attributes = m_smartData->attributes();
    for (const SmartAttribute &attr : attributes) {
        out << QString("%1\t%2\t%3\t%4\t%5\t%6\t%7\t%8\n")
               .arg(attr.id, 4)
               .arg(attr.name, -25)
               .arg(attr.current, 8)
               .arg(attr.worst, 8)
               .arg(attr.threshold, 8)
               .arg(QString::number(attr.raw), 12)
               .arg(attr.status, 8)
               .arg(attr.description, -20);
    }
    
    // 写入时间信息
    out << "\n生成时间: " << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") << "\n";
    
    file.close();
    
    QMessageBox::information(this, "保存成功", "SMART信息已保存到: " + filePath);
}

void SmartWidget::onTimerTimeout()
{
    // 如果用户禁用了定时刷新，则直接返回
    if (!m_refreshButton->isEnabled()) {
        return;
    }
    
    // 定时刷新当前硬盘的SMART数据
    if (m_currentDiskIndex >= 0 && m_currentDiskIndex < m_diskList.size()) {
        // 只在界面可见时才进行定时刷新
        if (isVisible()) {
            loadSmartData(m_currentDiskIndex);
        }
    }
}

void SmartWidget::loadSmartData(int diskIndex)
{
    if (diskIndex < 0 || diskIndex >= m_diskList.size()) {
        qDebug() << "无效的磁盘索引:" << diskIndex;
        return;
    }
    
    m_currentDiskIndex = diskIndex;
    DiskInfo disk = m_diskList[diskIndex];
    
    qDebug() << "加载磁盘" << disk.index << disk.model << "的SMART数据, 路径:" << disk.diskPath;
    
    // 界面更新为加载状态
    m_attributesTable->clearContents();
    m_attributesTable->setRowCount(0);
    
    // 设置标题显示为"正在加载"状态
    m_attributesGroup->setTitle(QString("SMART属性 (正在加载...)"));
    m_healthGroup->setTitle(QString("硬盘健康状态 (正在加载...)"));
    m_summaryGroup->setTitle(QString("重要指标 (正在加载...)"));
    
    // 禁用刷新按钮，防止用户重复点击
    m_refreshButton->setEnabled(false);
    
    // 创建一个QFutureWatcher来监控异步任务
    QFutureWatcher<void> *watcher = new QFutureWatcher<void>(this);
    
    // 连接完成信号
    connect(watcher, &QFutureWatcher<void>::finished, this, [this, watcher, disk]() {
        // 重新启用刷新按钮
        m_refreshButton->setEnabled(true);
        m_refreshButton->setText("刷新"); // 恢复按钮文本
        
        // 恢复标题
        m_attributesGroup->setTitle(QString("SMART属性"));
        m_healthGroup->setTitle(QString("硬盘健康状态"));
        m_summaryGroup->setTitle(QString("重要指标"));
        
        // 更新界面
        updateAttributesTable();
        updateHealthChart();
        updateSummaryInfo();
        
        // 删除watcher
        watcher->deleteLater();
    });
    
    // 预先在主线程中创建适合磁盘类型的SmartData对象
    if (m_smartData) {
        delete m_smartData;
        m_smartData = nullptr;
    }
    
    // 使用工厂类创建适合的SMART数据处理器
    m_smartData = SmartFactory::createSmartData(disk.diskPath, this);
    
    // 复制要传给线程的数据，避免传递QObject
    QString diskPathCopy = disk.diskPath;
    
    // 使用QtConcurrent在单独线程中执行SMART数据加载
    QFuture<void> future = QtConcurrent::run([this, diskPathCopy]() {
        // 加载SMART数据
        bool success = m_smartData->loadFromDisk(diskPathCopy);
        
        if (success) {
            qDebug() << "硬盘类型:" << (m_smartData->diskType() == DiskType::NVMe ? "NVMe" : 
                       (m_smartData->diskType() == DiskType::SATA ? "SATA" : "未知"))
                     << "SMART健康度:" << m_smartData->overallHealth()
                     << "温度:" << m_smartData->temperature()
                     << "通电时间:" << m_smartData->powerOnHours() << "小时"
                     << "SMART属性数量:" << m_smartData->attributes().size();
        } else {
            qDebug() << "加载SMART数据失败，使用模拟数据";
        }
    });
    
    // 设置watcher以监控future
    watcher->setFuture(future);
}

void SmartWidget::updateAttributesTable()
{
    // 优化表格更新性能
    m_attributesTable->setUpdatesEnabled(false);
    
    // 先禁用排序，提高表格填充速度
    m_attributesTable->setSortingEnabled(false);
    
    // 清空表格但保留结构
    m_attributesTable->clearContents();
    
    // 设置列数
    m_attributesTable->setColumnCount(8);
    m_attributesTable->setHorizontalHeaderLabels(QStringList() 
                                             << "ID" 
                                             << "属性名称" 
                                             << "当前值" 
                                             << "最差值" 
                                             << "阈值" 
                                             << "原始值" 
                                             << "状态" 
                                             << "描述");
    
    // 如果没有SMART数据，则退出
    if (!m_smartData) {
        m_attributesTable->setUpdatesEnabled(true);
        return;
    }
    
    QList<SmartAttribute> attributes = m_smartData->attributes();
    m_attributesTable->setRowCount(attributes.size());
    
    for (int i = 0; i < attributes.size(); i++) {
        const SmartAttribute &attr = attributes[i];
        
        // 创建表格项目
        QTableWidgetItem *idItem = new QTableWidgetItem(QString::number(attr.id));
        
        // 这里使用翻译方法处理属性名称
        QTableWidgetItem *nameItem = new QTableWidgetItem(translateAttributeName(attr.name));
        
        QTableWidgetItem *currentItem = new QTableWidgetItem(QString::number(attr.current));
        QTableWidgetItem *worstItem = new QTableWidgetItem(QString::number(attr.worst));
        QTableWidgetItem *thresholdItem = new QTableWidgetItem(QString::number(attr.threshold));
        QTableWidgetItem *rawItem = new QTableWidgetItem(QString::number(attr.raw));
        
        // 设置状态
        SmartStatus status = SmartStatus::Good;
        if (attr.threshold > 0 && attr.current <= attr.threshold) {
            status = SmartStatus::Critical;
        } else if (attr.current < 70) {
            status = SmartStatus::Warning;
        }
        
        QTableWidgetItem *statusItem = new QTableWidgetItem(getStatusText(status));
        QTableWidgetItem *descItem = new QTableWidgetItem(attr.description);
        
        // 根据状态设置颜色
        if (status == SmartStatus::Critical) {
            statusItem->setForeground(QBrush(QColor(255, 0, 0)));  // 红色
        } else if (status == SmartStatus::Warning) {
            statusItem->setForeground(QBrush(QColor(255, 165, 0)));  // 橙色
        } else {
            statusItem->setForeground(QBrush(QColor(0, 128, 0)));  // 绿色
        }
        
        // 设置表格项目
        m_attributesTable->setItem(i, 0, idItem);
        m_attributesTable->setItem(i, 1, nameItem);
        m_attributesTable->setItem(i, 2, currentItem);
        m_attributesTable->setItem(i, 3, worstItem);
        m_attributesTable->setItem(i, 4, thresholdItem);
        m_attributesTable->setItem(i, 5, rawItem);
        m_attributesTable->setItem(i, 6, statusItem);
        m_attributesTable->setItem(i, 7, descItem);
        
        // 如果是关键属性，整行使用不同背景色标记
        if (status == SmartStatus::Critical) {
            QBrush bgBrush = QBrush(QColor(255, 230, 230));  // 浅红色背景
            for (int j = 0; j < 8; ++j) {
                m_attributesTable->item(i, j)->setBackground(bgBrush);
            }
        } else if (status == SmartStatus::Warning) {
            QBrush bgBrush = QBrush(QColor(255, 245, 220));  // 浅橙色背景
            for (int j = 0; j < 8; ++j) {
                m_attributesTable->item(i, j)->setBackground(bgBrush);
            }
        }
    }
    
    // 重新启用排序
    m_attributesTable->setSortingEnabled(true);
    
    // 自动调整列宽以适应内容
    m_attributesTable->resizeColumnsToContents();
    
    // 重新启用表格更新
    m_attributesTable->setUpdatesEnabled(true);
}

void SmartWidget::updateHealthChart()
{
    int healthScore = m_smartData->overallHealth();
    int remaining = 100 - healthScore;
    
    // 更新健康评分标签
    m_healthScoreLabel->setText(QString("健康评分: %1 / 100").arg(healthScore));
    
    // 更新进度条
    m_healthScoreBar->setValue(healthScore);
    
    // 设置进度条颜色
    QString styleSheet;
    if (healthScore >= 80) {
        styleSheet = "QProgressBar::chunk { background-color: #32CD32; }"; // 绿色
    } else if (healthScore >= 60) {
        styleSheet = "QProgressBar::chunk { background-color: #FFA500; }"; // 橙色
    } else {
        styleSheet = "QProgressBar::chunk { background-color: #FF4500; }"; // 红色
    }
    m_healthScoreBar->setStyleSheet(styleSheet);
    
    // 优化图表更新 - 避免不必要的重建
    QChart *chart = m_healthChartView->chart();
    
    // 检查是否需要更新图表
    QPieSeries *currentSeries = nullptr;
    if (chart && !chart->series().isEmpty()) {
        currentSeries = qobject_cast<QPieSeries*>(chart->series().first());
    }
    
    // 如果已有数据且与当前数据相同，不重新创建
    if (currentSeries && currentSeries->count() == 2 && 
        qAbs(currentSeries->slices().at(0)->value() - healthScore) < 0.1) {
        // 数据基本相同，只更新标签即可
        currentSeries->slices().at(0)->setLabel(QString("健康 %1%").arg(healthScore));
        currentSeries->slices().at(1)->setLabel(QString("损耗 %1%").arg(remaining));
        return;
    }
    
    // 需要重新创建图表
    QPieSeries *series = new QPieSeries();
    series->append(QString("健康 %1%").arg(healthScore), healthScore);
    series->append(QString("损耗 %1%").arg(remaining), remaining);
    
    // 设置颜色
    QColor healthColor;
    if (healthScore >= 80) {
        healthColor = QColor(50, 205, 50); // 绿色
    } else if (healthScore >= 60) {
        healthColor = QColor(255, 165, 0); // 橙色
    } else {
        healthColor = QColor(255, 69, 0);  // 红色
    }
    
    series->slices().at(0)->setBrush(healthColor);
    series->slices().at(1)->setBrush(QColor(192, 192, 192)); // 灰色
    
    // 突出健康状态
    series->slices().at(0)->setExploded(true);
    series->slices().at(0)->setLabelVisible(true);
    
    // 优化：复用现有图表对象
    if (chart) {
        // 移除当前系列
        chart->removeAllSeries();
        
        // 添加新系列
        chart->addSeries(series);
    } else {
        // 创建新图表
        chart = new QChart();
        chart->addSeries(series);
        chart->setTitle("健康状态分布");
        chart->legend()->setVisible(true);
        chart->legend()->setAlignment(Qt::AlignBottom);
        
        m_healthChartView->setChart(chart);
    }
    
    // 禁用动画以提高性能
    chart->setAnimationOptions(QChart::NoAnimation);
}

void SmartWidget::updateSummaryInfo()
{
    // 添加设备类型信息
    QString deviceType;
    switch (m_smartData->diskType()) {
        case DiskType::SATA:
            deviceType = "SATA/ATA设备";
            break;
        case DiskType::NVMe:
            deviceType = "NVMe设备";
            break;
        default:
            deviceType = "未知类型设备";
            break;
    }
    m_deviceTypeLabel->setText(QString("设备类型: %1").arg(deviceType));
    
    // 更新温度信息
    int temperature = m_smartData->temperature();
    if (temperature > 0) {
        QString tempText = QString("温度: %1 °C").arg(temperature);
        
        // 添加温度评估
        if (temperature < 35) {
            tempText += " (正常)";
        } else if (temperature < 45) {
            tempText += " (温暖)";
        } else if (temperature < 55) {
            tempText += " (偏热)";
        } else {
            tempText += " (过热!)";
        }
        
        m_temperatureLabel->setText(tempText);
    } else {
        m_temperatureLabel->setText("温度: N/A");
    }
    
    // 更新通电时间
    int powerOnHours = m_smartData->powerOnHours();
    if (powerOnHours >= 0) {
        QString hoursText = QString("通电时间: %1 小时").arg(powerOnHours);
        
        // 添加天数换算
        if (powerOnHours > 24) {
            hoursText += QString(" (%1 天 %2 小时)").arg(powerOnHours / 24).arg(powerOnHours % 24);
        }
        
        m_powerOnLabel->setText(hoursText);
    } else {
        m_powerOnLabel->setText("通电时间: N/A");
    }
    
    // 更新启动停止次数
    int startStopCount = m_smartData->powerCycleCount();
    if (startStopCount >= 0) {
        m_startStopLabel->setText(QString("启动/停止次数: %1 次").arg(startStopCount));
    } else {
        m_startStopLabel->setText("启动/停止次数: N/A");
    }
    
    // 更新警告信息
    QStringList warnings = m_smartData->warnings();
    if (!warnings.isEmpty()) {
        m_warningLabel->setText(QString("警告: %1 个问题").arg(warnings.size()));
        
        // 设置工具提示显示具体警告
        m_warningLabel->setToolTip(warnings.join("\n"));
    } else {
        m_warningLabel->setText("警告: 无");
        m_warningLabel->setToolTip("");
    }
}

// 辅助函数，将SmartStatus枚举转换为显示文本
QString SmartWidget::getStatusText(SmartStatus status) {
    switch (status) {
        case SmartStatus::Good:
            return "正常";
        case SmartStatus::Warning:
            return "警告";
        case SmartStatus::Critical:
            return "危险";
        default:
            return "未知";
    }
}

void SmartWidget::loadSmartData()
{
    if (m_currentDiskIndex >= 0 && m_currentDiskIndex < m_diskList.size()) {
        loadSmartData(m_currentDiskIndex);
    }
}

void SmartWidget::refreshData()
{
    // 刷新磁盘列表
    refreshDiskList();
    
    // 重新加载当前选中磁盘的SMART数据
    if (m_currentDiskIndex >= 0 && m_currentDiskIndex < m_diskList.size()) {
        loadSmartData(m_currentDiskIndex);
    }
}

// 新增语言切换方法
void SmartWidget::onLanguageToggleClicked()
{
    // 切换语言
    m_language = (m_language == SmartAttributeLanguage::English) ? 
                 SmartAttributeLanguage::Chinese : 
                 SmartAttributeLanguage::English;
    
    // 更新按钮文本
    updateLanguageButtonText();
    
    // 更新表格显示
    updateAttributesTable();
}

// 新增更新语言按钮文本的方法
void SmartWidget::updateLanguageButtonText()
{
    if (m_language == SmartAttributeLanguage::English) {
        m_languageToggleButton->setText("切换到中文");
        m_languageToggleButton->setToolTip("将SMART属性名称显示为中文");
    } else {
        m_languageToggleButton->setText("Switch to English");
        m_languageToggleButton->setToolTip("Display SMART attribute names in English");
    }
}

// 翻译SMART属性名称
QString SmartWidget::translateAttributeName(const QString &name)
{
    if (m_language == SmartAttributeLanguage::Chinese) {
        return SmartAttributeDict::translate(name);
    } else {
        return name;
    }
} 