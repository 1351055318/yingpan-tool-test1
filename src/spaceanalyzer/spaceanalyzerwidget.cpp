#include "spaceanalyzerwidget.h"

#include <QDir>
#include <QFileInfo>
#include <QDateTime>
#include <QFileDialog>
#include <QMessageBox>
#include <QFileIconProvider>
#include <QStandardPaths>
#include <QHeaderView>
#include <QTimer>
#include <QtCharts/QPieSeries>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QValueAxis>
#include <QTextStream>
#include <QCoreApplication>

// 目录大小计算线程实现
DirSizeWorker::DirSizeWorker(QObject *parent) : QObject(parent), m_stopped(false) {
}

void DirSizeWorker::setDirectory(const QString &path) {
    m_rootPath = path;
    m_stopped = false;
}

void DirSizeWorker::stop() {
    m_stopped = true;
}

void DirSizeWorker::process() {
    if (m_rootPath.isEmpty()) {
        emit finished();
        return;
    }
    
    int fileCount = 0;
    int dirCount = 0;
    qint64 size = calculateDirSize(m_rootPath, 0, fileCount, dirCount);
    
    emit resultReady(m_rootPath, size, fileCount, dirCount);
    emit finished();
}

qint64 DirSizeWorker::calculateDirSize(const QString &path, int level, int &fileCount, int &dirCount) {
    if (m_stopped) {
        return 0;
    }
    
    QDir dir(path);
    qint64 size = 0;
    
    // 发送进度信号
    emit progress(path, level);
    
    // 处理文件
    QFileInfoList entries = dir.entryInfoList(QDir::NoDotAndDotDot | QDir::Files);
    fileCount += entries.size();
    
    for (const QFileInfo &fileInfo : entries) {
        if (m_stopped) {
            return 0;
        }
        
        // 文件大小
        size += fileInfo.size();
        
        // 处理事件，保持UI响应
        if (fileCount % 100 == 0) {
            QCoreApplication::processEvents();
        }
    }
    
    // 处理子目录
    entries = dir.entryInfoList(QDir::NoDotAndDotDot | QDir::Dirs);
    dirCount += entries.size();
    
    for (const QFileInfo &dirInfo : entries) {
        if (m_stopped) {
            return 0;
        }
        
        int subFileCount = 0;
        int subDirCount = 0;
        qint64 subSize = calculateDirSize(dirInfo.absoluteFilePath(), level + 1, subFileCount, subDirCount);
        
        size += subSize;
        fileCount += subFileCount;
        dirCount += subDirCount;
        
        emit resultReady(dirInfo.absoluteFilePath(), subSize, subFileCount, subDirCount);
    }
    
    return size;
}

// SpaceAnalyzerWidget实现
SpaceAnalyzerWidget::SpaceAnalyzerWidget(QWidget *parent) : QWidget(parent), 
    m_rootItem(nullptr), m_scanning(false), m_workerThread(nullptr), m_worker(nullptr),
    m_totalItems(0), m_processedItems(0) {
    
    setupUI();
    refreshVolumeList();
}

SpaceAnalyzerWidget::~SpaceAnalyzerWidget() {
    if (m_scanning) {
        onStopButtonClicked();
    }
    
    if (m_workerThread) {
        m_workerThread->quit();
        m_workerThread->wait();
        delete m_workerThread;
    }
    
    delete m_rootItem;
}

void SpaceAnalyzerWidget::setupUI() {
    m_mainLayout = new QVBoxLayout(this);
    
    // === 顶部控制区域 ===
    QGroupBox *controlGroupBox = new QGroupBox("卷选择和扫描控制", this);
    QVBoxLayout *controlLayout = new QVBoxLayout(controlGroupBox);
    
    // 卷选择和路径
    QHBoxLayout *volumeLayout = new QHBoxLayout();
    QLabel *volumeLabel = new QLabel("卷:", this);
    m_volumeComboBox = new QComboBox(this);
    m_pathEdit = new QLineEdit(this);
    m_refreshButton = new QPushButton("刷新", this);
    m_refreshButton->setToolTip("刷新卷列表");
    
    volumeLayout->addWidget(volumeLabel);
    volumeLayout->addWidget(m_volumeComboBox);
    volumeLayout->addWidget(m_pathEdit);
    volumeLayout->addWidget(m_refreshButton);
    
    // 扫描控制
    QHBoxLayout *scanLayout = new QHBoxLayout();
    m_scanButton = new QPushButton("开始扫描", this);
    m_stopButton = new QPushButton("停止扫描", this);
    m_stopButton->setEnabled(false);
    m_exportButton = new QPushButton("导出报告", this);
    m_exportButton->setEnabled(false);
    
    // 筛选设置
    QLabel *minSizeLabel = new QLabel("最小显示大小 (MB):", this);
    m_minSizeSpinBox = new QSpinBox(this);
    m_minSizeSpinBox->setRange(0, 10000);
    m_minSizeSpinBox->setValue(10);
    m_minSizeSpinBox->setSingleStep(10);
    
    m_showFilesCheckBox = new QCheckBox("显示文件", this);
    m_showFilesCheckBox->setChecked(false);
    
    QLabel *filterLabel = new QLabel("文件名过滤:", this);
    m_filterEdit = new QLineEdit(this);
    m_filterEdit->setPlaceholderText("输入文件名过滤条件");
    
    scanLayout->addWidget(m_scanButton);
    scanLayout->addWidget(m_stopButton);
    scanLayout->addWidget(m_exportButton);
    scanLayout->addStretch();
    scanLayout->addWidget(minSizeLabel);
    scanLayout->addWidget(m_minSizeSpinBox);
    scanLayout->addWidget(m_showFilesCheckBox);
    scanLayout->addWidget(filterLabel);
    scanLayout->addWidget(m_filterEdit);
    
    // 扫描状态
    QHBoxLayout *statusLayout = new QHBoxLayout();
    m_scanProgressBar = new QProgressBar(this);
    m_scanProgressBar->setRange(0, 100);
    m_scanProgressBar->setValue(0);
    m_scanStatusLabel = new QLabel("准备就绪", this);
    
    statusLayout->addWidget(m_scanProgressBar);
    statusLayout->addWidget(m_scanStatusLabel);
    
    controlLayout->addLayout(volumeLayout);
    controlLayout->addLayout(scanLayout);
    controlLayout->addLayout(statusLayout);
    
    // === 中间区域 - 目录树和图表 ===
    QHBoxLayout *middleLayout = new QHBoxLayout();
    
    // 目录树
    QGroupBox *dirTreeGroupBox = new QGroupBox("目录结构", this);
    QVBoxLayout *dirTreeLayout = new QVBoxLayout(dirTreeGroupBox);
    
    m_dirTreeWidget = new QTreeWidget(this);
    m_dirTreeWidget->setHeaderLabels({"名称", "大小", "文件数", "文件夹数", "%"});
    m_dirTreeWidget->setColumnWidth(0, 300);
    m_dirTreeWidget->setSortingEnabled(true);
    m_dirTreeWidget->sortByColumn(1, Qt::DescendingOrder);
    
    dirTreeLayout->addWidget(m_dirTreeWidget);
    
    // 图表
    QGroupBox *chartGroupBox = new QGroupBox("空间分布", this);
    QVBoxLayout *chartLayout = new QVBoxLayout(chartGroupBox);
    
    m_chartView = new QtCharts::QChartView(this);
    m_chartView->setRenderHint(QPainter::Antialiasing);
    m_chartView->setMinimumHeight(200);
    
    chartLayout->addWidget(m_chartView);
    
    middleLayout->addWidget(dirTreeGroupBox, 3);
    middleLayout->addWidget(chartGroupBox, 2);
    
    // === 底部区域 - 文件列表 ===
    QGroupBox *fileListGroupBox = new QGroupBox("文件列表", this);
    QVBoxLayout *fileListLayout = new QVBoxLayout(fileListGroupBox);
    
    m_fileListWidget = new QTreeWidget(this);
    m_fileListWidget->setHeaderLabels({"名称", "大小", "类型", "修改日期"});
    m_fileListWidget->setColumnWidth(0, 350);
    m_fileListWidget->setColumnWidth(1, 100);
    m_fileListWidget->setColumnWidth(2, 100);
    m_fileListWidget->setSortingEnabled(true);
    m_fileListWidget->sortByColumn(1, Qt::DescendingOrder);
    
    fileListLayout->addWidget(m_fileListWidget);
    
    // 添加到主布局
    m_mainLayout->addWidget(controlGroupBox);
    m_mainLayout->addLayout(middleLayout, 3);
    m_mainLayout->addWidget(fileListGroupBox, 2);
    
    // 信号连接
    connect(m_volumeComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &SpaceAnalyzerWidget::onVolumeSelectionChanged);
    connect(m_pathEdit, &QLineEdit::textChanged, this, &SpaceAnalyzerWidget::onPathChanged);
    connect(m_scanButton, &QPushButton::clicked, this, &SpaceAnalyzerWidget::onScanButtonClicked);
    connect(m_stopButton, &QPushButton::clicked, this, &SpaceAnalyzerWidget::onStopButtonClicked);
    connect(m_refreshButton, &QPushButton::clicked, this, &SpaceAnalyzerWidget::onRefreshButtonClicked);
    connect(m_exportButton, &QPushButton::clicked, this, &SpaceAnalyzerWidget::onExportButtonClicked);
    connect(m_dirTreeWidget, &QTreeWidget::itemClicked, this, &SpaceAnalyzerWidget::onTreeItemClicked);
    connect(m_dirTreeWidget, &QTreeWidget::itemExpanded, this, &SpaceAnalyzerWidget::onTreeItemExpanded);
    connect(m_minSizeSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &SpaceAnalyzerWidget::onFilterChanged);
    connect(m_showFilesCheckBox, &QCheckBox::stateChanged, this, &SpaceAnalyzerWidget::onFilterChanged);
    connect(m_filterEdit, &QLineEdit::textChanged, this, &SpaceAnalyzerWidget::onFilterChanged);
}

void SpaceAnalyzerWidget::refreshVolumeList() {
    m_volumeComboBox->clear();
    m_volumes = DiskUtils::getVolumes();
    
    for (const VolumeInfo &volume : m_volumes) {
        QString displayName = QString("%1 (%2) - %3").arg(volume.label.isEmpty() ? volume.rootPath : volume.label)
                                                   .arg(volume.rootPath)
                                                   .arg(DiskUtils::formatSize(volume.totalSpace));
        m_volumeComboBox->addItem(displayName);
    }
    
    if (m_volumeComboBox->count() > 0) {
        m_volumeComboBox->setCurrentIndex(0);
        onVolumeSelectionChanged(0);
    } else {
        m_pathEdit->clear();
    }
}

void SpaceAnalyzerWidget::onVolumeSelectionChanged(int index) {
    if (index >= 0 && index < m_volumes.size()) {
        m_selectedVolume = m_volumes[index];
        m_pathEdit->setText(m_selectedVolume.rootPath);
    }
}

void SpaceAnalyzerWidget::onPathChanged() {
    QString path = m_pathEdit->text();
    QFileInfo fileInfo(path);
    
    if (!fileInfo.exists() || !fileInfo.isDir()) {
        m_pathEdit->setStyleSheet("background-color: #FFDDDD;");
        m_scanButton->setEnabled(false);
    } else {
        m_pathEdit->setStyleSheet("");
        m_scanButton->setEnabled(true);
    }
}

void SpaceAnalyzerWidget::onScanButtonClicked() {
    QString path = m_pathEdit->text();
    QFileInfo fileInfo(path);
    
    if (!fileInfo.exists() || !fileInfo.isDir()) {
        QMessageBox::warning(this, "无效路径", "请选择一个有效的目录进行扫描");
        return;
    }
    
    // 清除之前的结果
    clearResults();
    
    // 创建根项
    m_rootItem = new FileItem();
    m_rootItem->path = path;
    m_rootItem->name = QDir(path).dirName();
    if (m_rootItem->name.isEmpty()) {
        m_rootItem->name = path;
    }
    
    // 更新UI状态
    m_scanning = true;
    m_scanButton->setEnabled(false);
    m_stopButton->setEnabled(true);
    m_refreshButton->setEnabled(false);
    m_exportButton->setEnabled(false);
    m_scanProgressBar->setValue(0);
    m_scanStatusLabel->setText("正在扫描...");
    
    // 创建工作线程
    m_workerThread = new QThread(this);
    m_worker = new DirSizeWorker();
    m_worker->moveToThread(m_workerThread);
    
    connect(m_workerThread, &QThread::started, m_worker, &DirSizeWorker::process);
    connect(m_worker, &DirSizeWorker::finished, this, &SpaceAnalyzerWidget::onScanFinished);
    connect(m_worker, &DirSizeWorker::finished, m_workerThread, &QThread::quit);
    connect(m_worker, &DirSizeWorker::resultReady, this, &SpaceAnalyzerWidget::onDirResultReady);
    connect(m_worker, &DirSizeWorker::progress, this, &SpaceAnalyzerWidget::onScanProgress);
    connect(m_workerThread, &QThread::finished, m_worker, &DirSizeWorker::deleteLater);
    connect(m_workerThread, &QThread::finished, [this]() {
        m_worker = nullptr;
    });
    
    // 设置扫描路径并启动线程
    m_worker->setDirectory(path);
    m_workerThread->start();
}

void SpaceAnalyzerWidget::onStopButtonClicked() {
    if (m_worker) {
        m_worker->stop();
        m_scanStatusLabel->setText("正在停止扫描...");
    }
}

void SpaceAnalyzerWidget::onDirResultReady(const QString &path, qint64 size, int fileCount, int dirCount) {
    if (!m_rootItem) {
        return;
    }
    
    // 跳过根目录，因为它会在onScanFinished中处理
    if (path == m_rootItem->path) {
        m_rootItem->size = size;
        m_rootItem->fileCount = fileCount;
        m_rootItem->dirCount = dirCount;
        return;
    }
    
    // 创建或更新文件项
    QString relativePath = path.mid(m_rootItem->path.length());
    if (relativePath.startsWith('/') || relativePath.startsWith('\\')) {
        relativePath = relativePath.mid(1);
    }
    
    QStringList parts = relativePath.split(QDir::separator());
    FileItem *currentItem = m_rootItem;
    
    QString currentPath = m_rootItem->path;
    for (const QString &part : parts) {
        currentPath = QDir(currentPath).absoluteFilePath(part);
        
        if (!currentItem->children.contains(part)) {
            FileItem *newItem = new FileItem();
            newItem->path = currentPath;
            newItem->name = part;
            newItem->parent = currentItem;
            currentItem->children[part] = newItem;
        }
        
        currentItem = currentItem->children[part];
    }
    
    // 更新属性
    currentItem->size = size;
    currentItem->fileCount = fileCount;
    currentItem->dirCount = dirCount;
    
    // 增加已处理项数量
    m_processedItems++;
    
    // 更新进度
    if (m_totalItems > 0) {
        int progress = (m_processedItems * 100) / m_totalItems;
        m_scanProgressBar->setValue(progress);
    }
}

void SpaceAnalyzerWidget::onScanProgress(const QString &currentPath, int level) {
    m_scanStatusLabel->setText("正在扫描: " + formatPath(currentPath));
    m_totalItems++;
}

void SpaceAnalyzerWidget::onScanFinished() {
    // 更新UI状态
    m_scanning = false;
    m_scanButton->setEnabled(true);
    m_stopButton->setEnabled(false);
    m_refreshButton->setEnabled(true);
    
    // 扫描完成，显示结果
    if (m_rootItem) {
        m_scanProgressBar->setValue(100);
        m_scanStatusLabel->setText(QString("扫描完成: %1 文件夹, %2 文件, 总大小 %3")
                                  .arg(m_rootItem->dirCount)
                                  .arg(m_rootItem->fileCount)
                                  .arg(formatSize(m_rootItem->size)));
        
        // 更新目录树
        m_dirTreeWidget->clear();
        QTreeWidgetItem *rootTreeItem = new QTreeWidgetItem(m_dirTreeWidget);
        rootTreeItem->setText(0, m_rootItem->name);
        rootTreeItem->setText(1, formatSize(m_rootItem->size));
        rootTreeItem->setText(2, QString::number(m_rootItem->fileCount));
        rootTreeItem->setText(3, QString::number(m_rootItem->dirCount));
        rootTreeItem->setText(4, "100%");
        rootTreeItem->setData(0, Qt::UserRole, m_rootItem->path);
        
        // 添加一级子目录
        for (auto it = m_rootItem->children.begin(); it != m_rootItem->children.end(); ++it) {
            int minSize = m_minSizeSpinBox->value() * 1024 * 1024; // MB to bytes
            addItemToTree(rootTreeItem, it.value(), minSize);
        }
        
        // 展开根项
        m_dirTreeWidget->expandItem(rootTreeItem);
        
        // 更新图表
        updateChart(m_rootItem);
        
        // 更新文件列表
        updateFileList(m_rootItem);
        
        // 允许导出
        m_exportButton->setEnabled(true);
    } else {
        m_scanStatusLabel->setText("扫描已取消");
    }
    
    // 清理线程
    if (m_workerThread) {
        m_workerThread->deleteLater();
        m_workerThread = nullptr;
    }
}

void SpaceAnalyzerWidget::onTreeItemClicked(QTreeWidgetItem *item, int column) {
    QString path = item->data(0, Qt::UserRole).toString();
    
    // 查找对应的FileItem
    FileItem *currentItem = m_rootItem;
    
    if (path != m_rootItem->path) {
        QString relativePath = path.mid(m_rootItem->path.length());
        if (relativePath.startsWith('/') || relativePath.startsWith('\\')) {
            relativePath = relativePath.mid(1);
        }
        
        QStringList parts = relativePath.split(QDir::separator());
        
        for (const QString &part : parts) {
            if (currentItem->children.contains(part)) {
                currentItem = currentItem->children[part];
            } else {
                return;
            }
        }
    }
    
    // 更新图表
    updateChart(currentItem);
    
    // 更新文件列表
    updateFileList(currentItem);
}

void SpaceAnalyzerWidget::onTreeItemExpanded(QTreeWidgetItem *item) {
    // 如果子项已经存在，不做任何事
    if (item->childCount() > 0) {
        return;
    }
    
    QString path = item->data(0, Qt::UserRole).toString();
    
    // 查找对应的FileItem
    FileItem *currentItem = m_rootItem;
    
    if (path != m_rootItem->path) {
        QString relativePath = path.mid(m_rootItem->path.length());
        if (relativePath.startsWith('/') || relativePath.startsWith('\\')) {
            relativePath = relativePath.mid(1);
        }
        
        QStringList parts = relativePath.split(QDir::separator());
        
        for (const QString &part : parts) {
            if (currentItem->children.contains(part)) {
                currentItem = currentItem->children[part];
            } else {
                return;
            }
        }
    }
    
    // 添加子项到树
    for (auto it = currentItem->children.begin(); it != currentItem->children.end(); ++it) {
        int minSize = m_minSizeSpinBox->value() * 1024 * 1024; // MB to bytes
        addItemToTree(item, it.value(), minSize);
    }
}

void SpaceAnalyzerWidget::onRefreshButtonClicked() {
    refreshVolumeList();
}

void SpaceAnalyzerWidget::onExportButtonClicked() {
    if (!m_rootItem) {
        QMessageBox::warning(this, "无数据", "没有可导出的数据，请先扫描一个目录");
        return;
    }
    
    QString filePath = QFileDialog::getSaveFileName(
        this,
        "导出空间分析报告",
        QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/空间分析报告.csv",
        "CSV文件 (*.csv);;所有文件 (*.*)"
    );
    
    if (filePath.isEmpty()) {
        return;
    }
    
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "错误", "无法打开文件进行写入: " + filePath);
        return;
    }
    
    QTextStream out(&file);
    out.setCodec("UTF-8");
    
    // 写入CSV头
    out << "路径,大小(字节),大小,文件数,文件夹数,占比(%)\n";
    
    // 递归导出目录结构
    std::function<void(FileItem*, const QString&, qint64)> exportItem = [&](FileItem *item, const QString &basePath, qint64 totalSize) {
        // 计算百分比
        double percent = totalSize > 0 ? (item->size * 100.0) / totalSize : 0.0;
        
        // 写入当前项
        out << "\"" << item->path << "\","
            << item->size << ","
            << "\"" << formatSize(item->size) << "\","
            << item->fileCount << ","
            << item->dirCount << ","
            << QString::number(percent, 'f', 2) << "\n";
        
        // 导出子项
        for (auto it = item->children.begin(); it != item->children.end(); ++it) {
            exportItem(it.value(), basePath, totalSize);
        }
    };
    
    // 从根目录开始导出
    exportItem(m_rootItem, m_rootItem->path, m_rootItem->size);
    
    file.close();
    
    QMessageBox::information(this, "成功", "空间分析报告已成功导出到: " + filePath);
}

void SpaceAnalyzerWidget::onFilterChanged() {
    // 如果没有扫描结果，不做任何事
    if (!m_rootItem) {
        return;
    }
    
    // 获取当前选择的项
    QTreeWidgetItem *currentItem = m_dirTreeWidget->currentItem();
    if (!currentItem) {
        return;
    }
    
    QString path = currentItem->data(0, Qt::UserRole).toString();
    
    // 查找对应的FileItem
    FileItem *currentFileItem = m_rootItem;
    
    if (path != m_rootItem->path) {
        QString relativePath = path.mid(m_rootItem->path.length());
        if (relativePath.startsWith('/') || relativePath.startsWith('\\')) {
            relativePath = relativePath.mid(1);
        }
        
        QStringList parts = relativePath.split(QDir::separator());
        
        for (const QString &part : parts) {
            if (currentFileItem->children.contains(part)) {
                currentFileItem = currentFileItem->children[part];
            } else {
                return;
            }
        }
    }
    
    // 重新加载文件列表
    updateFileList(currentFileItem);
    
    // 更新树视图
    m_dirTreeWidget->clear();
    QTreeWidgetItem *rootTreeItem = new QTreeWidgetItem(m_dirTreeWidget);
    rootTreeItem->setText(0, m_rootItem->name);
    rootTreeItem->setText(1, formatSize(m_rootItem->size));
    rootTreeItem->setText(2, QString::number(m_rootItem->fileCount));
    rootTreeItem->setText(3, QString::number(m_rootItem->dirCount));
    rootTreeItem->setText(4, "100%");
    rootTreeItem->setData(0, Qt::UserRole, m_rootItem->path);
    
    // 添加一级子目录
    int minSize = m_minSizeSpinBox->value() * 1024 * 1024; // MB to bytes
    for (auto it = m_rootItem->children.begin(); it != m_rootItem->children.end(); ++it) {
        addItemToTree(rootTreeItem, it.value(), minSize);
    }
    
    // 展开根项
    m_dirTreeWidget->expandItem(rootTreeItem);
}

void SpaceAnalyzerWidget::updateChart(FileItem *item) {
    if (!item) {
        return;
    }
    
    // 清除旧图表
    auto chart = new QtCharts::QChart();
    chart->setTitle(QString("目录: %1").arg(formatPath(item->path)));
    chart->setAnimationOptions(QtCharts::QChart::SeriesAnimations);
    
    // 创建饼图
    QtCharts::QPieSeries *pieSeries = new QtCharts::QPieSeries();
    
    // 对子项按大小排序
    QVector<FileItem*> sortedItems;
    for (auto it = item->children.begin(); it != item->children.end(); ++it) {
        sortedItems.append(it.value());
    }
    
    std::sort(sortedItems.begin(), sortedItems.end(), [](FileItem *a, FileItem *b) {
        return a->size > b->size;
    });
    
    // 添加前10个最大的子项
    qint64 otherSize = item->size;
    int count = 0;
    
    for (FileItem *childItem : sortedItems) {
        if (count < 10) {
            double percent = (childItem->size * 100.0) / item->size;
            if (percent >= 1.0) { // 只显示占比至少1%的项
                QString label = QString("%1 (%2, %3%)").arg(childItem->name)
                                                      .arg(formatSize(childItem->size))
                                                      .arg(percent, 0, 'f', 1);
                pieSeries->append(label, childItem->size);
                otherSize -= childItem->size;
                count++;
            }
        }
    }
    
    // 如果有其他没有显示的项，添加一个"其他"项
    if (otherSize > 0 && count > 0) {
        double percent = (otherSize * 100.0) / item->size;
        QString label = QString("其他 (%1, %2%)").arg(formatSize(otherSize))
                                             .arg(percent, 0, 'f', 1);
        pieSeries->append(label, otherSize);
    }
    
    // 如果是空目录，显示一个提示
    if (count == 0) {
        pieSeries->append("空目录", 1);
    }
    
    chart->addSeries(pieSeries);
    chart->legend()->setAlignment(Qt::AlignRight);
    
    m_chartView->setChart(chart);
}

void SpaceAnalyzerWidget::updateFileList(FileItem *item) {
    if (!item) {
        return;
    }
    
    m_fileListWidget->clear();
    
    // 获取子目录和文件
    QDir dir(item->path);
    QFileInfoList entries;
    
    int minSize = m_minSizeSpinBox->value() * 1024 * 1024; // MB to bytes
    bool showFiles = m_showFilesCheckBox->isChecked();
    QString filter = m_filterEdit->text().trimmed();
    
    if (showFiles) {
        if (filter.isEmpty()) {
            entries = dir.entryInfoList(QDir::NoDotAndDotDot | QDir::Files | QDir::Dirs, QDir::DirsFirst);
        } else {
            QStringList nameFilters;
            nameFilters << "*" + filter + "*";
            entries = dir.entryInfoList(nameFilters, QDir::NoDotAndDotDot | QDir::Files | QDir::Dirs, QDir::DirsFirst);
        }
    } else {
        entries = dir.entryInfoList(QDir::NoDotAndDotDot | QDir::Dirs, QDir::DirsFirst);
    }
    
    QFileIconProvider iconProvider;
    
    for (const QFileInfo &fileInfo : entries) {
        // 过滤小文件
        if (fileInfo.isFile() && fileInfo.size() < minSize) {
            continue;
        }
        
        QTreeWidgetItem *fileItem = new QTreeWidgetItem(m_fileListWidget);
        
        // 图标
        fileItem->setIcon(0, iconProvider.icon(fileInfo));
        
        // 名称
        fileItem->setText(0, fileInfo.fileName());
        
        // 大小
        if (fileInfo.isDir()) {
            // 查找对应的FileItem以获取准确大小
            QString childName = fileInfo.fileName();
            if (item->children.contains(childName)) {
                FileItem *childItem = item->children[childName];
                fileItem->setText(1, formatSize(childItem->size));
                fileItem->setData(1, Qt::UserRole, childItem->size);
            } else {
                fileItem->setText(1, "计算中...");
            }
        } else {
            fileItem->setText(1, formatSize(fileInfo.size()));
            fileItem->setData(1, Qt::UserRole, fileInfo.size());
        }
        
        // 类型
        fileItem->setText(2, fileInfo.isDir() ? "文件夹" : fileInfo.suffix().toUpper() + " 文件");
        
        // 修改日期
        fileItem->setText(3, fileInfo.lastModified().toString("yyyy-MM-dd HH:mm:ss"));
    }
    
    // 按大小排序
    m_fileListWidget->sortItems(1, Qt::DescendingOrder);
}

void SpaceAnalyzerWidget::addItemToTree(QTreeWidgetItem *parentItem, FileItem *fileItem, int minSize) {
    // 过滤小文件夹
    if (fileItem->size < minSize) {
        return;
    }
    
    QTreeWidgetItem *treeItem = new QTreeWidgetItem(parentItem);
    treeItem->setText(0, fileItem->name);
    treeItem->setText(1, formatSize(fileItem->size));
    treeItem->setText(2, QString::number(fileItem->fileCount));
    treeItem->setText(3, QString::number(fileItem->dirCount));
    
    // 计算百分比
    FileItem *parentFileItem = fileItem->parent;
    if (parentFileItem && parentFileItem->size > 0) {
        double percent = (fileItem->size * 100.0) / parentFileItem->size;
        treeItem->setText(4, QString::number(percent, 'f', 2) + "%");
    } else {
        treeItem->setText(4, "N/A");
    }
    
    treeItem->setData(0, Qt::UserRole, fileItem->path);
    
    // 添加一个空的子项，以允许展开
    if (!fileItem->children.isEmpty()) {
        new QTreeWidgetItem(treeItem);
    }
}

void SpaceAnalyzerWidget::clearResults() {
    // 清除目录树
    m_dirTreeWidget->clear();
    
    // 清除文件列表
    m_fileListWidget->clear();
    
    // 清除图表
    m_chartView->chart()->removeAllSeries();
    
    // 清除根项
    delete m_rootItem;
    m_rootItem = nullptr;
    
    // 重置计数器
    m_totalItems = 0;
    m_processedItems = 0;
}

QString SpaceAnalyzerWidget::formatSize(qint64 size) const {
    return DiskUtils::formatSize(size);
}

QString SpaceAnalyzerWidget::formatPath(const QString &path) const {
    if (path.length() > 50) {
        return "..." + path.right(47);
    }
    return path;
} 