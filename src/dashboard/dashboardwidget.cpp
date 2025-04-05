#include "dashboardwidget.h"
#include <QDebug>
#include <QtMath>
#include <QtConcurrent>

DashboardWidget::DashboardWidget(QWidget *parent) :
    QWidget(parent),
    m_mainLayout(nullptr),
    m_controlLayout(nullptr),
    m_chartLayout(nullptr),
    m_infoLayout(nullptr),
    m_diskLabel(nullptr),
    m_diskComboBox(nullptr),
    m_refreshButton(nullptr),
    m_diskUsageChartView(nullptr),
    m_diskUsageChart(nullptr),
    m_diskUsageSeries(nullptr),
    m_temperatureChartView(nullptr),
    m_temperatureChart(nullptr),
    m_temperatureSeries(nullptr),
    m_temperatureAxisX(nullptr),
    m_temperatureAxisY(nullptr),
    m_healthChartView(nullptr),
    m_healthChart(nullptr),
    m_healthSeries(nullptr),
    m_modelLabel(nullptr),
    m_modelValueLabel(nullptr),
    m_sizeLabel(nullptr),
    m_sizeValueLabel(nullptr),
    m_interfaceLabel(nullptr),
    m_interfaceValueLabel(nullptr),
    m_typeLabel(nullptr),
    m_typeValueLabel(nullptr),
    m_healthLabel(nullptr),
    m_healthProgressBar(nullptr),
    m_powerOnHoursLabel(nullptr),
    m_powerOnHoursValueLabel(nullptr),
    m_temperatureLabel(nullptr),
    m_temperatureValueLabel(nullptr),
    m_startStopLabel(nullptr),
    m_startStopValueLabel(nullptr),
    m_warningLabel(nullptr),
    m_refreshTimer(nullptr),
    m_currentDiskIndex(-1),
    m_smartData(nullptr)
{
    qDebug() << "DashboardWidget构造函数开始初始化...";
    
    setupUI();
    
    // 使用单次定时器延迟加载磁盘列表，先让UI显示出来
    QTimer::singleShot(500, this, [this]() {
        qDebug() << "开始刷新磁盘列表...";
        refreshDiskList();
        qDebug() << "磁盘列表刷新完成，磁盘数量:" << m_diskList.size();
    });
    
    // 设置定时刷新
    m_refreshTimer = new QTimer(this);
    connect(m_refreshTimer, &QTimer::timeout, this, &DashboardWidget::onTimerTimeout);
    m_refreshTimer->start(5000); // 5秒刷新一次
    
    qDebug() << "DashboardWidget构造函数初始化完成";
}

DashboardWidget::~DashboardWidget()
{
    if (m_refreshTimer) {
        m_refreshTimer->stop();
    }
    
    if (m_smartData) {
        delete m_smartData;
        m_smartData = nullptr;
    }
}

void DashboardWidget::setupUI()
{
    // 创建主布局
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(15, 15, 15, 15);
    m_mainLayout->setSpacing(10);
    
    // 创建控制区域
    m_controlLayout = new QHBoxLayout();
    m_diskLabel = new QLabel("选择硬盘:", this);
    m_diskComboBox = new QComboBox(this);
    m_refreshButton = new QPushButton("刷新", this);
    
    m_controlLayout->addWidget(m_diskLabel);
    m_controlLayout->addWidget(m_diskComboBox, 1);
    m_controlLayout->addWidget(m_refreshButton);
    
    // 创建图表区域
    m_chartLayout = new QGridLayout();
    m_chartLayout->setSpacing(15);
    
    // 创建信息区域
    m_infoLayout = new QHBoxLayout();
    
    QGridLayout *infoGridLayout = new QGridLayout();
    m_modelLabel = new QLabel("型号:", this);
    m_modelValueLabel = new QLabel(this);
    m_sizeLabel = new QLabel("容量:", this);
    m_sizeValueLabel = new QLabel(this);
    m_interfaceLabel = new QLabel("接口:", this);
    m_interfaceValueLabel = new QLabel(this);
    m_typeLabel = new QLabel("类型:", this);
    m_typeValueLabel = new QLabel(this);
    m_healthLabel = new QLabel("健康状态:", this);
    m_healthProgressBar = new QProgressBar(this);
    m_healthProgressBar->setRange(0, 100);
    m_healthProgressBar->setTextVisible(true);
    
    // 添加SMART信息标签
    m_temperatureLabel = new QLabel("温度:", this);
    m_temperatureValueLabel = new QLabel("N/A", this);
    m_powerOnHoursLabel = new QLabel("通电时间:", this);
    m_powerOnHoursValueLabel = new QLabel("N/A", this);
    m_startStopLabel = new QLabel("启动次数:", this);
    m_startStopValueLabel = new QLabel("N/A", this);
    m_warningLabel = new QLabel("", this);
    m_warningLabel->setStyleSheet("color: #FFA500;"); // 橙色警告
    
    infoGridLayout->addWidget(m_modelLabel, 0, 0);
    infoGridLayout->addWidget(m_modelValueLabel, 0, 1);
    infoGridLayout->addWidget(m_sizeLabel, 0, 2);
    infoGridLayout->addWidget(m_sizeValueLabel, 0, 3);
    infoGridLayout->addWidget(m_interfaceLabel, 1, 0);
    infoGridLayout->addWidget(m_interfaceValueLabel, 1, 1);
    infoGridLayout->addWidget(m_typeLabel, 1, 2);
    infoGridLayout->addWidget(m_typeValueLabel, 1, 3);
    infoGridLayout->addWidget(m_temperatureLabel, 2, 0);
    infoGridLayout->addWidget(m_temperatureValueLabel, 2, 1);
    infoGridLayout->addWidget(m_powerOnHoursLabel, 2, 2);
    infoGridLayout->addWidget(m_powerOnHoursValueLabel, 2, 3);
    infoGridLayout->addWidget(m_startStopLabel, 3, 0);
    infoGridLayout->addWidget(m_startStopValueLabel, 3, 1);
    infoGridLayout->addWidget(m_healthLabel, 3, 2);
    infoGridLayout->addWidget(m_healthProgressBar, 3, 3);
    infoGridLayout->addWidget(m_warningLabel, 4, 0, 1, 4);
    
    infoGridLayout->setColumnStretch(1, 1);
    infoGridLayout->setColumnStretch(3, 1);
    
    m_infoLayout->addLayout(infoGridLayout);
    
    // 添加所有布局到主布局
    m_mainLayout->addLayout(m_controlLayout);
    m_mainLayout->addLayout(m_chartLayout, 1);
    m_mainLayout->addLayout(m_infoLayout);
    
    // 创建图表
    createDiskUsageChart();
    createTemperatureChart();
    createHealthChart();
    
    // 设置图表布局
    m_chartLayout->addWidget(m_diskUsageChartView, 0, 0);
    m_chartLayout->addWidget(m_temperatureChartView, 0, 1);
    m_chartLayout->addWidget(m_healthChartView, 1, 0, 1, 2);
    
    // 连接信号槽
    connect(m_diskComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &DashboardWidget::onDiskSelectionChanged);
    connect(m_refreshButton, &QPushButton::clicked, this, &DashboardWidget::onRefreshClicked);
}

void DashboardWidget::refreshDiskList()
{
    // 保存当前选择
    QString currentDiskPath;
    if (m_currentDiskIndex >= 0 && m_currentDiskIndex < m_diskList.size()) {
        currentDiskPath = m_diskList[m_currentDiskIndex].diskPath;
    }
    
    // 清除旧数据
    m_diskComboBox->clear();
    m_diskList.clear();
    
    // 获取硬盘列表
    m_diskList = DiskUtils::getAllDisks();
    
    // 填充下拉框
    int index = 0;
    int newSelectedIndex = -1;
    
    for (const DiskInfo &disk : m_diskList) {
        QString displayText = QString("%1 (%2) - %3")
                            .arg(disk.model)
                            .arg(disk.diskPath)
                            .arg(DiskUtils::formatSize(disk.diskSize));
        m_diskComboBox->addItem(displayText);
        
        // 检查是否为之前选择的磁盘
        if (disk.diskPath == currentDiskPath) {
            newSelectedIndex = index;
        }
        
        index++;
    }
    
    // 如果找到之前的选择，则还原
    if (newSelectedIndex >= 0) {
        m_diskComboBox->setCurrentIndex(newSelectedIndex);
    } else if (!m_diskList.isEmpty()) {
        // 否则选择第一个
        m_diskComboBox->setCurrentIndex(0);
    }
}

void DashboardWidget::onDiskSelectionChanged(int index)
{
    if (index >= 0 && index < m_diskList.size()) {
        m_currentDiskIndex = index;
        
        // 清除温度历史记录，确保切换硬盘后不显示上一个硬盘的温度历史
        m_temperatureHistory.clear();
        
        // 更新仪表盘
        updateDashboard();
    }
}

void DashboardWidget::onRefreshClicked()
{
    refreshDiskList();
    updateDashboard();
}

void DashboardWidget::updateDashboard()
{
    if (m_currentDiskIndex < 0 || m_currentDiskIndex >= m_diskList.size()) {
        return;
    }
    
    // 获取当前选择的硬盘
    const DiskInfo &disk = m_diskList[m_currentDiskIndex];
    
    // 加载SMART数据
    loadSmartData(disk.diskPath);
    
    // 更新各个图表
    updateDiskUsageChart();
    updateTemperatureChart();
    updateHealthChart();
    updateInfoLabels();
}

void DashboardWidget::onTimerTimeout()
{
    // 定时更新仪表盘
    updateDashboard();
}

void DashboardWidget::createDiskUsageChart()
{
    m_diskUsageChart = new QChart();
    m_diskUsageChart->setTitle("磁盘使用率");
    m_diskUsageChart->setAnimationOptions(QChart::SeriesAnimations);
    m_diskUsageChart->legend()->setVisible(true);
    m_diskUsageChart->legend()->setAlignment(Qt::AlignBottom);
    
    m_diskUsageSeries = new QPieSeries();
    m_diskUsageChart->addSeries(m_diskUsageSeries);
    
    m_diskUsageChartView = new QChartView(m_diskUsageChart);
    m_diskUsageChartView->setRenderHint(QPainter::Antialiasing);
}

void DashboardWidget::createTemperatureChart()
{
    m_temperatureChart = new QChart();
    m_temperatureChart->setTitle("温度历史");
    m_temperatureChart->setAnimationOptions(QChart::SeriesAnimations);
    
    m_temperatureSeries = new QLineSeries();
    m_temperatureSeries->setName("温度"); // 设置系列名称
    m_temperatureChart->addSeries(m_temperatureSeries);
    
    m_temperatureAxisX = new QValueAxis();
    m_temperatureAxisX->setRange(0, 20);
    m_temperatureAxisX->setLabelFormat("%d");
    m_temperatureAxisX->setTitleText("采样点");
    
    m_temperatureAxisY = new QValueAxis();
    m_temperatureAxisY->setRange(20, 60);
    m_temperatureAxisY->setLabelFormat("%d"); // 只显示数字
    m_temperatureAxisY->setTitleText("温度(C)"); // 在标题中显示单位
    
    m_temperatureChart->addAxis(m_temperatureAxisX, Qt::AlignBottom);
    m_temperatureChart->addAxis(m_temperatureAxisY, Qt::AlignLeft);
    m_temperatureSeries->attachAxis(m_temperatureAxisX);
    m_temperatureSeries->attachAxis(m_temperatureAxisY);
    
    m_temperatureChartView = new QChartView(m_temperatureChart);
    m_temperatureChartView->setRenderHint(QPainter::Antialiasing);
}

void DashboardWidget::createHealthChart()
{
    m_healthChart = new QChart();
    m_healthChart->setTitle("硬盘健康状态");
    m_healthChart->setAnimationOptions(QChart::SeriesAnimations);
    m_healthChart->legend()->setVisible(true);
    m_healthChart->legend()->setAlignment(Qt::AlignBottom);
    
    m_healthSeries = new QPieSeries();
    m_healthChart->addSeries(m_healthSeries);
    
    m_healthChartView = new QChartView(m_healthChart);
    m_healthChartView->setRenderHint(QPainter::Antialiasing);
}

void DashboardWidget::updateDiskUsageChart()
{
    if (m_currentDiskIndex < 0 || m_currentDiskIndex >= m_diskList.size()) {
        return;
    }
    
    const DiskInfo &disk = m_diskList[m_currentDiskIndex];
    
    // 计算使用率
    double usagePercent = DiskUtils::getDiskUsage(disk.diskPath).usedPercent;
    
    m_diskUsageSeries->clear();
    
    // 添加饼图数据
    m_diskUsageSeries->append("已用 " + QString::number(usagePercent, 'f', 1) + "%", usagePercent);
    m_diskUsageSeries->append("可用 " + QString::number(100 - usagePercent, 'f', 1) + "%", 100 - usagePercent);
    
    // 设置颜色
    m_diskUsageSeries->slices().at(0)->setBrush(QColor(65, 105, 225)); // 蓝色
    m_diskUsageSeries->slices().at(1)->setBrush(QColor(192, 192, 192)); // 灰色
    
    // 突出已用空间
    m_diskUsageSeries->slices().at(0)->setExploded(true);
    m_diskUsageSeries->slices().at(0)->setLabelVisible(true);
}

void DashboardWidget::updateTemperatureChart()
{
    if (m_currentDiskIndex < 0 || m_currentDiskIndex >= m_diskList.size() || !m_smartData) {
        return;
    }
    
    // 从SMART数据中获取温度
    int temperature = m_smartData->temperature();
    
    // 如果SMART数据中没有温度信息，尝试使用其他方法获取
    if (temperature <= 0) {
        const DiskInfo &disk = m_diskList[m_currentDiskIndex];
        temperature = DiskUtils::getDiskTemperature(disk.diskPath);
    }
    
    // 确保温度在合理范围内
    if (temperature <= 0) {
        temperature = 35; // 使用默认值
    }
    
    // 添加到历史记录
    m_temperatureHistory.append(temperature);
    
    // 保持最多20个点
    while (m_temperatureHistory.size() > 20) {
        m_temperatureHistory.removeFirst();
    }
    
    // 更新图表
    m_temperatureSeries->clear();
    for (int i = 0; i < m_temperatureHistory.size(); ++i) {
        m_temperatureSeries->append(i, m_temperatureHistory[i]);
    }
    
    // 调整Y轴范围
    if (!m_temperatureHistory.isEmpty()) {
        qreal minTemp = *std::min_element(m_temperatureHistory.begin(), m_temperatureHistory.end());
        qreal maxTemp = *std::max_element(m_temperatureHistory.begin(), m_temperatureHistory.end());
        
        // 确保范围至少有10个单位的跨度，避免显示问题
        if (maxTemp - minTemp < 10) {
            qreal mid = (maxTemp + minTemp) / 2;
            minTemp = mid - 5;
            maxTemp = mid + 5;
        }
        
        // 添加一点余量并确保范围在合理值内
        double floorValue = qFloor(minTemp - 5.0);
        minTemp = floorValue < 0.0 ? 0.0 : floorValue;
        
        double ceilValue = qCeil(maxTemp + 5.0);
        maxTemp = ceilValue > 100.0 ? 100.0 : ceilValue;
        
        // 确保刻度值是整数
        m_temperatureAxisY->setRange(minTemp, maxTemp);
        m_temperatureAxisY->setTickCount(5); // 设置刻度数量
        
        // 确保Y轴标签格式和标题是最新的
        m_temperatureAxisY->setLabelFormat("%d"); // 只显示数字
        m_temperatureAxisY->setTitleText("温度(C)"); // 在标题中显示单位
    }
    
    // 显示固定的温度警告线
    QLineSeries *warningLine = nullptr;
    
    // 移除旧的警告线
    QList<QAbstractSeries*> allSeries = m_temperatureChart->series();
    for (QAbstractSeries *series : allSeries) {
        if (series != m_temperatureSeries) {
            m_temperatureChart->removeSeries(series);
            delete series;
        }
    }
    
    // 添加新的警告线
    warningLine = new QLineSeries();
    warningLine->setName("温度警告");
    warningLine->setPen(QPen(QColor(255, 165, 0), 1, Qt::DashLine)); // 橙色虚线
    
    qreal minX = m_temperatureAxisX->min();
    qreal maxX = m_temperatureAxisX->max();
    warningLine->append(minX, 55); // 高温警告线
    warningLine->append(maxX, 55);
    
    m_temperatureChart->addSeries(warningLine);
    warningLine->attachAxis(m_temperatureAxisX);
    warningLine->attachAxis(m_temperatureAxisY);
}

void DashboardWidget::updateHealthChart()
{
    if (m_currentDiskIndex < 0 || m_currentDiskIndex >= m_diskList.size() || !m_smartData) {
        return;
    }
    
    // 获取硬盘健康状态
    int healthScore = m_smartData->overallHealth();
    int remaining = 100 - healthScore;
    
    m_healthSeries->clear();
    
    // 添加饼图数据
    m_healthSeries->append("健康 " + QString::number(healthScore) + "%", healthScore);
    m_healthSeries->append("损耗 " + QString::number(remaining) + "%", remaining);
    
    // 设置颜色
    QColor healthColor;
    if (healthScore >= 80) {
        healthColor = QColor(50, 205, 50); // 绿色
    } else if (healthScore >= 60) {
        healthColor = QColor(255, 165, 0); // 橙色
    } else {
        healthColor = QColor(255, 69, 0);  // 红色
    }
    
    m_healthSeries->slices().at(0)->setBrush(healthColor);
    m_healthSeries->slices().at(1)->setBrush(QColor(192, 192, 192)); // 灰色
    
    // 突出健康状态
    m_healthSeries->slices().at(0)->setExploded(true);
    m_healthSeries->slices().at(0)->setLabelVisible(true);
}

void DashboardWidget::updateInfoLabels()
{
    if (m_currentDiskIndex < 0 || m_currentDiskIndex >= m_diskList.size()) {
        return;
    }
    
    const DiskInfo &disk = m_diskList[m_currentDiskIndex];
    
    // 更新基本信息标签
    m_modelValueLabel->setText(disk.model);
    m_sizeValueLabel->setText(DiskUtils::formatSize(disk.diskSize));
    m_interfaceValueLabel->setText(disk.interfaceSpeed);
    m_typeValueLabel->setText(disk.type);
    
    // 更新SMART信息标签
    if (m_smartData) {
        // 温度
        int temperature = m_smartData->temperature();
        if (temperature > 0) {
            QString tempStatus;
            if (temperature < 35) {
                tempStatus = " (正常)";
            } else if (temperature < 45) {
                tempStatus = " (温暖)";
            } else if (temperature < 55) {
                tempStatus = " (偏热)";
            } else {
                tempStatus = " (过热!)";
            }
            m_temperatureValueLabel->setText(QString("%1°C%2").arg(temperature).arg(tempStatus));
        } else {
            m_temperatureValueLabel->setText("N/A");
        }
        
        // 通电时间
        int powerOnHours = m_smartData->powerOnHours();
        if (powerOnHours > 0) {
            QString hoursText = QString("%1小时").arg(powerOnHours);
            if (powerOnHours > 24) {
                hoursText += QString(" (%1天%2小时)").arg(powerOnHours / 24).arg(powerOnHours % 24);
            }
            m_powerOnHoursValueLabel->setText(hoursText);
        } else {
            m_powerOnHoursValueLabel->setText("N/A");
        }
        
        // 启动/停止次数
        int powerCycleCount = m_smartData->powerCycleCount();
        if (powerCycleCount > 0) {
            m_startStopValueLabel->setText(QString("%1次").arg(powerCycleCount));
        } else {
            m_startStopValueLabel->setText("N/A");
        }
        
        // 健康状态进度条
        int healthScore = m_smartData->overallHealth();
        m_healthProgressBar->setValue(healthScore);
        
        // 根据健康状态设置进度条颜色
        QString styleSheet;
        if (healthScore >= 80) {
            styleSheet = "QProgressBar::chunk { background-color: #32CD32; }"; // 绿色
        } else if (healthScore >= 60) {
            styleSheet = "QProgressBar::chunk { background-color: #FFA500; }"; // 橙色
        } else {
            styleSheet = "QProgressBar::chunk { background-color: #FF4500; }"; // 红色
        }
        
        m_healthProgressBar->setStyleSheet(styleSheet);
        
        // 显示警告
        QStringList warnings = m_smartData->warnings();
        if (!warnings.isEmpty()) {
            m_warningLabel->setText("警告: " + warnings.first() + 
                                   (warnings.size() > 1 ? QString(" (+%1条)").arg(warnings.size() - 1) : ""));
            m_warningLabel->setVisible(true);
        } else {
            m_warningLabel->setVisible(false);
        }
    } else {
        // 如果没有SMART数据，显示默认值
        m_temperatureValueLabel->setText("N/A");
        m_powerOnHoursValueLabel->setText("N/A");
        m_startStopValueLabel->setText("N/A");
        m_healthProgressBar->setValue(0);
        m_warningLabel->setVisible(false);
    }
}

// 加载SMART数据的方法
void DashboardWidget::loadSmartData(const QString &diskPath)
{
    // 清理旧的SMART数据对象
    if (m_smartData) {
        delete m_smartData;
        m_smartData = nullptr;
    }
    
    // 使用工厂类创建适合的SMART数据处理器
    m_smartData = SmartFactory::createSmartData(diskPath, this);
    
    // 加载SMART数据
    QFuture<bool> future = QtConcurrent::run([this, diskPath]() {
        return m_smartData->loadFromDisk(diskPath);
    });
    
    // 等待加载完成
    bool success = future.result();
    
    if (!success) {
        qDebug() << "加载SMART数据失败，使用模拟数据";
    } else {
        qDebug() << "硬盘类型:" << (m_smartData->diskType() == DiskType::NVMe ? "NVMe" : 
                   (m_smartData->diskType() == DiskType::SATA ? "SATA" : "未知"))
                 << "SMART健康度:" << m_smartData->overallHealth()
                 << "温度:" << m_smartData->temperature()
                 << "通电时间:" << m_smartData->powerOnHours() << "小时"
                 << "SMART属性数量:" << m_smartData->attributes().size();
    }
}

void DashboardWidget::refreshData()
{
    // 刷新硬盘列表
    refreshDiskList();
    
    // 更新当前选中磁盘的信息
    updateDashboard();
} 