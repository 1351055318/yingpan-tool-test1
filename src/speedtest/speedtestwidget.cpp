#include "speedtestwidget.h"
#include "../core/diskutils.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QComboBox>
#include <QPushButton>
#include <QGroupBox>
#include <QTableWidget>
#include <QHeaderView>
#include <QTableWidgetItem>
#include <QProgressBar>
#include <QFileDialog>
#include <QMessageBox>
#include <QStandardPaths>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QThread>
#include <QElapsedTimer>
#include <QDebug>
#include <QTimer>
#include <QRandomGenerator>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>

// Windows API
#ifdef Q_OS_WIN
#define NOMINMAX
#include <windows.h>
#endif

// 声明图表相关变量
QtCharts::QChart *m_chart;
QtCharts::QBarSeries *m_barSeries;

// 速度测试线程类
class SpeedTester : public QObject {
    Q_OBJECT
    
public:
    SpeedTester(const QString& diskPath, const QString& testFilePath, int blockSizeKB, int fileSizeMB)
        : m_diskPath(diskPath), m_testFilePath(testFilePath), 
          m_blockSizeKB(blockSizeKB), m_fileSizeMB(fileSizeMB), m_canceled(false) {}
    
    void startTest() {
        double readSpeed = performReadTest();
        if (m_canceled) {
            emit testCompleted(0, 0);
            return;
        }
        
        double writeSpeed = performWriteTest();
        if (m_canceled) {
            emit testCompleted(0, 0);
            return;
        }
        
        emit testCompleted(readSpeed, writeSpeed);
    }
    
    void cancel() {
        m_canceled = true;
    }
    
signals:
    void progressUpdated(int percent, double currentSpeed, bool isRead);
    void testCompleted(double readSpeed, double writeSpeed);
    
private:
    double performReadTest() {
        QFile file(m_testFilePath);
        if (!file.open(QIODevice::ReadOnly)) {
            qDebug() << "无法打开测试文件进行读测试";
            return 0;
        }
        
        const int totalBytes = m_fileSizeMB * 1024 * 1024;
        const int blockSize = m_blockSizeKB * 1024;
        char* buffer = new char[blockSize];
        
        QElapsedTimer timer;
        timer.start();
        
        qint64 bytesRead = 0;
        int lastPercent = 0;
        
        while (bytesRead < totalBytes && !m_canceled) {
            qint64 read = file.read(buffer, blockSize);
            if (read <= 0) {
                file.seek(0);
                continue;
            }
            
            bytesRead += read;
            
            int percent = static_cast<int>((bytesRead * 100) / totalBytes);
            if (percent > lastPercent) {
                lastPercent = percent;
                double currentSpeed = (bytesRead / (1024.0 * 1024.0)) / (timer.elapsed() / 1000.0);
                emit progressUpdated(percent, currentSpeed, true);
                QThread::msleep(10);
            }
        }
        
        file.close();
        delete[] buffer;
        
        if (m_canceled) {
            return 0;
        }
        
        double elapsedSec = timer.elapsed() / 1000.0;
        double mbPerSec = (totalBytes / (1024.0 * 1024.0)) / elapsedSec;
        
        return mbPerSec;
    }
    
    double performWriteTest() {
        QFile file(m_testFilePath);
        if (!file.open(QIODevice::WriteOnly)) {
            qDebug() << "无法打开测试文件进行写测试";
            return 0;
        }
        
        const int totalBytes = m_fileSizeMB * 1024 * 1024;
        const int blockSize = m_blockSizeKB * 1024;
        char* buffer = new char[blockSize];
        
        for (int i = 0; i < blockSize; i++) {
            buffer[i] = static_cast<char>(QRandomGenerator::global()->bounded(256));
        }
        
        QElapsedTimer timer;
        timer.start();
        
        qint64 bytesWritten = 0;
        int lastPercent = 0;
        
        while (bytesWritten < totalBytes && !m_canceled) {
            qint64 written = file.write(buffer, blockSize);
            if (written <= 0) {
                qDebug() << "写入失败";
                break;
            }
            
            bytesWritten += written;
            
            int percent = static_cast<int>((bytesWritten * 100) / totalBytes);
            if (percent > lastPercent) {
                lastPercent = percent;
                double currentSpeed = (bytesWritten / (1024.0 * 1024.0)) / (timer.elapsed() / 1000.0);
                emit progressUpdated(percent, currentSpeed, false);
                QThread::msleep(10);
            }
        }
        
        file.close();
        file.remove();
        delete[] buffer;
        
        if (m_canceled) {
            return 0;
        }
        
        double elapsedSec = timer.elapsed() / 1000.0;
        double mbPerSec = (totalBytes / (1024.0 * 1024.0)) / elapsedSec;
        
        return mbPerSec;
    }
    
    QString m_diskPath;
    QString m_testFilePath;
    int m_blockSizeKB;
    int m_fileSizeMB;
    bool m_canceled;
};

SpeedTestWidget::SpeedTestWidget(QWidget *parent) : QWidget(parent) {
    setupUI();
    refreshDiskList();
    
    m_tester = nullptr;
    m_testerThread = nullptr;
    
    QTimer::singleShot(500, this, [this]() {
        // 初始化图表
        createChart();
    });
}

SpeedTestWidget::~SpeedTestWidget() {
    if (m_testerThread) {
        if (m_tester) {
            m_tester->cancel();
        }
        m_testerThread->quit();
        m_testerThread->wait();
    }
}

void SpeedTestWidget::setupUI() {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    
    // === 磁盘选择区域 ===
    QGroupBox *diskGroupBox = new QGroupBox("磁盘选择", this);
    QHBoxLayout *diskLayout = new QHBoxLayout(diskGroupBox);
    
    QLabel *diskLabel = new QLabel("磁盘:", this);
    m_diskComboBox = new QComboBox(this);
    m_diskComboBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    
    QPushButton *refreshButton = new QPushButton("刷新", this);
    
    diskLayout->addWidget(diskLabel);
    diskLayout->addWidget(m_diskComboBox);
    diskLayout->addWidget(refreshButton);
    
    // === 测试设置区域 ===
    QGroupBox *settingsGroupBox = new QGroupBox("测试设置", this);
    QGridLayout *settingsLayout = new QGridLayout(settingsGroupBox);
    
    QLabel *blockSizeLabel = new QLabel("块大小:", this);
    m_blockSizeComboBox = new QComboBox(this);
    m_blockSizeComboBox->addItem("4 KB", 4);
    m_blockSizeComboBox->addItem("8 KB", 8);
    m_blockSizeComboBox->addItem("16 KB", 16);
    m_blockSizeComboBox->addItem("32 KB", 32);
    m_blockSizeComboBox->addItem("64 KB", 64);
    m_blockSizeComboBox->addItem("128 KB", 128);
    m_blockSizeComboBox->addItem("256 KB", 256);
    m_blockSizeComboBox->addItem("512 KB", 512);
    m_blockSizeComboBox->addItem("1 MB", 1024);
    m_blockSizeComboBox->setCurrentIndex(4); // 64KB默认
    
    QLabel *fileSizeLabel = new QLabel("文件大小:", this);
    m_fileSizeComboBox = new QComboBox(this);
    m_fileSizeComboBox->addItem("100 MB", 100);
    m_fileSizeComboBox->addItem("500 MB", 500);
    m_fileSizeComboBox->addItem("1 GB", 1024);
    m_fileSizeComboBox->addItem("2 GB", 2048);
    m_fileSizeComboBox->addItem("4 GB", 4096);
    m_fileSizeComboBox->setCurrentIndex(1); // 500MB默认
    
    settingsLayout->addWidget(blockSizeLabel, 0, 0);
    settingsLayout->addWidget(m_blockSizeComboBox, 0, 1);
    settingsLayout->addWidget(fileSizeLabel, 1, 0);
    settingsLayout->addWidget(m_fileSizeComboBox, 1, 1);
    
    // === 控制按钮区域 ===
    QHBoxLayout *controlLayout = new QHBoxLayout();
    
    m_startButton = new QPushButton("开始测试", this);
    m_cancelButton = new QPushButton("取消测试", this);
    m_cancelButton->setEnabled(false);
    m_exportButton = new QPushButton("导出结果", this);
    m_exportButton->setEnabled(false);
    
    controlLayout->addWidget(m_startButton);
    controlLayout->addWidget(m_cancelButton);
    controlLayout->addWidget(m_exportButton);
    
    // === 测试状态区域 ===
    QGroupBox *statusGroupBox = new QGroupBox("测试状态", this);
    QVBoxLayout *statusLayout = new QVBoxLayout(statusGroupBox);
    
    m_testStatusLabel = new QLabel("准备就绪", this);
    m_progressBar = new QProgressBar(this);
    m_progressBar->setRange(0, 100);
    m_progressBar->setValue(0);
    
    statusLayout->addWidget(m_testStatusLabel);
    statusLayout->addWidget(m_progressBar);
    
    // === 测试结果区域 ===
    QGroupBox *resultsGroupBox = new QGroupBox("测试结果", this);
    QVBoxLayout *resultsLayout = new QVBoxLayout(resultsGroupBox);
    
    m_chartView = new QtCharts::QChartView(this);
    m_chartView->setRenderHint(QPainter::Antialiasing);
    m_chartView->setMinimumHeight(200);
    
    QHBoxLayout *speedLayout = new QHBoxLayout();
    
    QLabel *readSpeedLabel = new QLabel("读取速度:", this);
    m_readSpeedLabel = new QLabel("0 MB/s", this);
    m_readSpeedLabel->setStyleSheet("font-weight: bold; color: #2196F3;");
    
    QLabel *writeSpeedLabel = new QLabel("写入速度:", this);
    m_writeSpeedLabel = new QLabel("0 MB/s", this);
    m_writeSpeedLabel->setStyleSheet("font-weight: bold; color: #4CAF50;");
    
    speedLayout->addWidget(readSpeedLabel);
    speedLayout->addWidget(m_readSpeedLabel);
    speedLayout->addSpacing(20);
    speedLayout->addWidget(writeSpeedLabel);
    speedLayout->addWidget(m_writeSpeedLabel);
    speedLayout->addStretch();
    
    resultsLayout->addWidget(m_chartView);
    resultsLayout->addLayout(speedLayout);
    
    // === 历史记录区域 ===
    QGroupBox *historyGroupBox = new QGroupBox("测试历史", this);
    QVBoxLayout *historyLayout = new QVBoxLayout(historyGroupBox);
    
    m_historyTable = new QTableWidget(0, 5, this);
    m_historyTable->setHorizontalHeaderLabels({"磁盘", "块大小", "文件大小", "读取速度", "写入速度"});
    m_historyTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_historyTable->setAlternatingRowColors(true);
    m_historyTable->setEditTriggers(QTableWidget::NoEditTriggers);
    
    historyLayout->addWidget(m_historyTable);
    
    // 添加所有区域到主布局
    mainLayout->addWidget(diskGroupBox);
    mainLayout->addWidget(settingsGroupBox);
    mainLayout->addLayout(controlLayout);
    mainLayout->addWidget(statusGroupBox);
    mainLayout->addWidget(resultsGroupBox);
    mainLayout->addWidget(historyGroupBox);
    
    // 信号连接
    connect(m_diskComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &SpeedTestWidget::onDiskSelectionChanged);
    connect(refreshButton, &QPushButton::clicked, this, &SpeedTestWidget::refreshDiskList);
    connect(m_startButton, &QPushButton::clicked, this, &SpeedTestWidget::onStartTestClicked);
    connect(m_cancelButton, &QPushButton::clicked, this, &SpeedTestWidget::onCancelTestClicked);
    connect(m_exportButton, &QPushButton::clicked, this, &SpeedTestWidget::onExportResultsClicked);
    connect(m_blockSizeComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &SpeedTestWidget::onBlockSizeChanged);
    connect(m_fileSizeComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &SpeedTestWidget::onFileSizeChanged);
}

void SpeedTestWidget::refreshDiskList() {
    m_diskComboBox->clear();
    m_diskList = DiskUtils::getAllDisks();
    
    for (const DiskInfo &disk : m_diskList) {
        QString displayName = QString("%1 (%2)").arg(disk.diskName).arg(disk.model);
        m_diskComboBox->addItem(displayName);
    }
    
    if (m_diskComboBox->count() > 0) {
        m_diskComboBox->setCurrentIndex(0);
        onDiskSelectionChanged(0);
    }
}

void SpeedTestWidget::onDiskSelectionChanged(int index) {
    if (index >= 0 && index < m_diskList.size()) {
        m_selectedDisk = m_diskList[index];
        
        // 可以显示一些所选磁盘的信息
        QString diskInfo = QString("已选择: %1 (%2, %3)").arg(m_selectedDisk.diskName)
                                                      .arg(m_selectedDisk.model)
                                                      .arg(DiskUtils::formatSize(m_selectedDisk.diskSize));
        m_testStatusLabel->setText(diskInfo);
    }
}

void SpeedTestWidget::onStartTestClicked() {
    if (m_diskComboBox->currentIndex() < 0) {
        QMessageBox::warning(this, "警告", "请先选择一个磁盘");
        return;
    }
    
    // 获取测试参数
    int blockSizeKB = m_blockSizeComboBox->currentData().toInt();
    int fileSizeMB = m_fileSizeComboBox->currentData().toInt();
    
    // 创建临时文件路径
    QString tempPath = QStandardPaths::writableLocation(QStandardPaths::TempLocation);
    QString testFilePath = tempPath + "/disktoolbox_speedtest.dat";
    
    // 检查空间是否足够
    // 使用DiskUsage获取可用空间
    DiskUsage usage = DiskUtils::getDiskUsage(m_selectedDisk.diskPath);
    qint64 freeSpace = usage.freeSpace;
    
    if (freeSpace < (fileSizeMB * 1024 * 1024)) {
        QMessageBox::warning(this, "错误", "磁盘空间不足，无法进行测试");
        return;
    }
    
    // 更新UI状态
    m_startButton->setEnabled(false);
    m_cancelButton->setEnabled(true);
    m_diskComboBox->setEnabled(false);
    m_blockSizeComboBox->setEnabled(false);
    m_fileSizeComboBox->setEnabled(false);
    m_progressBar->setValue(0);
    m_testStatusLabel->setText("正在准备测试...");
    
    // 创建测试线程
    m_testerThread = new QThread(this);
    m_tester = new SpeedTester(m_selectedDisk.diskPath, testFilePath, blockSizeKB, fileSizeMB);
    m_tester->moveToThread(m_testerThread);
    
    connect(m_testerThread, &QThread::started, m_tester, &SpeedTester::startTest);
    connect(m_tester, &SpeedTester::progressUpdated, this, &SpeedTestWidget::updateProgress);
    connect(m_tester, &SpeedTester::testCompleted, this, &SpeedTestWidget::onTestCompleted);
    
    connect(m_testerThread, &QThread::finished, [this]() {
        m_tester->deleteLater();
        m_tester = nullptr;
        m_testerThread->deleteLater();
        m_testerThread = nullptr;
        
        // 恢复UI状态
        m_startButton->setEnabled(true);
        m_cancelButton->setEnabled(false);
        m_diskComboBox->setEnabled(true);
        m_blockSizeComboBox->setEnabled(true);
        m_fileSizeComboBox->setEnabled(true);
    });
    
    m_testerThread->start();
}

void SpeedTestWidget::onCancelTestClicked() {
    if (m_tester) {
        m_tester->cancel();
        m_testStatusLabel->setText("已取消测试");
    }
}

void SpeedTestWidget::onTestCompleted(double readSpeed, double writeSpeed) {
    if (readSpeed == 0 && writeSpeed == 0) {
        m_testStatusLabel->setText("测试已取消");
        return;
    }
    
    m_readSpeedLabel->setText(QString("%1 MB/s").arg(readSpeed, 0, 'f', 2));
    m_writeSpeedLabel->setText(QString("%1 MB/s").arg(writeSpeed, 0, 'f', 2));
    
    updateChart(readSpeed, writeSpeed);
    addResultToHistory(readSpeed, writeSpeed);
    
    m_testStatusLabel->setText(QString("测试完成！读取: %1 MB/s, 写入: %2 MB/s")
                       .arg(readSpeed, 0, 'f', 2)
                       .arg(writeSpeed, 0, 'f', 2));
    
    m_progressBar->setValue(100);
    m_exportButton->setEnabled(true);
}

void SpeedTestWidget::updateProgress(int percent, double currentSpeed, bool isRead) {
    m_progressBar->setValue(percent);
    
    QString speedText = QString("%1 MB/s").arg(currentSpeed, 0, 'f', 2);
    if (isRead) {
        m_testStatusLabel->setText(QString("正在进行读取测试: %1").arg(speedText));
        m_readSpeedLabel->setText(speedText);
    } else {
        m_testStatusLabel->setText(QString("正在进行写入测试: %1").arg(speedText));
        m_writeSpeedLabel->setText(speedText);
    }
}

void SpeedTestWidget::onExportResultsClicked() {
    if (m_historyTable->rowCount() == 0) {
        QMessageBox::warning(this, "警告", "没有测试结果可以导出");
        return;
    }
    
    QString filePath = QFileDialog::getSaveFileName(
        this,
        "导出测试结果",
        QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/磁盘速度测试结果.csv",
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
    out << "磁盘,型号,块大小,文件大小,读取速度(MB/s),写入速度(MB/s),测试时间\n";
    
    // 写入所有历史记录
    for (int i = 0; i < m_historyTable->rowCount(); ++i) {
        QTableWidgetItem *diskItem = m_historyTable->item(i, 0);
        QTableWidgetItem *blockSizeItem = m_historyTable->item(i, 1);
        QTableWidgetItem *fileSizeItem = m_historyTable->item(i, 2);
        QTableWidgetItem *readSpeedItem = m_historyTable->item(i, 3);
        QTableWidgetItem *writeSpeedItem = m_historyTable->item(i, 4);
        
        if (diskItem && blockSizeItem && fileSizeItem && readSpeedItem && writeSpeedItem) {
            QString diskName = diskItem->text();
            QString diskModel;
            
            // 尝试提取型号信息
            int startPos = diskName.indexOf('(');
            int endPos = diskName.indexOf(')');
            if (startPos >= 0 && endPos > startPos) {
                diskModel = diskName.mid(startPos + 1, endPos - startPos - 1);
                diskName = diskName.left(startPos).trimmed();
            }
            
            out << diskName << ","
                << diskModel << ","
                << blockSizeItem->text() << ","
                << fileSizeItem->text() << ","
                << readSpeedItem->text().replace(" MB/s", "") << ","
                << writeSpeedItem->text().replace(" MB/s", "") << ","
                << QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss") << "\n";
        }
    }
    
    file.close();
    
    QMessageBox::information(this, "成功", "测试结果已成功导出到: " + filePath);
}

void SpeedTestWidget::createChart() {
    m_chart = new QtCharts::QChart();
    m_chart->setTitle("磁盘速度测试结果");
    m_chart->setAnimationOptions(QtCharts::QChart::SeriesAnimations);
    
    // 创建条形系列
    m_barSeries = new QtCharts::QBarSeries(m_chart);
    
    QtCharts::QBarSet *readSet = new QtCharts::QBarSet("读取速度");
    readSet->append(0);
    readSet->setColor(QColor("#2196F3"));
    
    QtCharts::QBarSet *writeSet = new QtCharts::QBarSet("写入速度");
    writeSet->append(0);
    writeSet->setColor(QColor("#4CAF50"));
    
    m_barSeries->append(readSet);
    m_barSeries->append(writeSet);
    m_chart->addSeries(m_barSeries);
    
    // 创建坐标轴
    QStringList categories;
    categories << "速度";
    
    QtCharts::QBarCategoryAxis *axisX = new QtCharts::QBarCategoryAxis();
    axisX->append(categories);
    m_chart->addAxis(axisX, Qt::AlignBottom);
    m_barSeries->attachAxis(axisX);
    
    QtCharts::QValueAxis *axisY = new QtCharts::QValueAxis();
    axisY->setRange(0, 1000);
    axisY->setTitleText("速度(MB/s)");
    m_chart->addAxis(axisY, Qt::AlignLeft);
    m_barSeries->attachAxis(axisY);
    
    m_chart->legend()->setVisible(true);
    m_chart->legend()->setAlignment(Qt::AlignBottom);
    
    m_chartView->setChart(m_chart);
}

void SpeedTestWidget::updateChart(double readSpeed, double writeSpeed) {
    if (!m_chart || !m_barSeries) {
        return;
    }
    
    // 更新条形数据
    if (m_barSeries->count() >= 2) {
        m_barSeries->barSets().at(0)->replace(0, readSpeed);
        m_barSeries->barSets().at(1)->replace(0, writeSpeed);
        
        // 调整Y轴范围
        double maxSpeed = qMax(readSpeed, writeSpeed) * 1.2;
        QtCharts::QValueAxis *axisY = qobject_cast<QtCharts::QValueAxis*>(m_chart->axes(Qt::Vertical).first());
        if (axisY) {
            axisY->setRange(0, maxSpeed);
        }
    }
}

void SpeedTestWidget::addResultToHistory(double readSpeed, double writeSpeed) {
    int row = m_historyTable->rowCount();
    m_historyTable->insertRow(row);
    
    // 设置单元格数据
    QTableWidgetItem *diskItem = new QTableWidgetItem(QString("%1 (%2)").arg(m_selectedDisk.diskName).arg(m_selectedDisk.model));
    QTableWidgetItem *blockSizeItem = new QTableWidgetItem(m_blockSizeComboBox->currentText());
    QTableWidgetItem *fileSizeItem = new QTableWidgetItem(m_fileSizeComboBox->currentText());
    QTableWidgetItem *readSpeedItem = new QTableWidgetItem(QString("%1 MB/s").arg(readSpeed, 0, 'f', 1));
    QTableWidgetItem *writeSpeedItem = new QTableWidgetItem(QString("%1 MB/s").arg(writeSpeed, 0, 'f', 1));
    
    m_historyTable->setItem(row, 0, diskItem);
    m_historyTable->setItem(row, 1, blockSizeItem);
    m_historyTable->setItem(row, 2, fileSizeItem);
    m_historyTable->setItem(row, 3, readSpeedItem);
    m_historyTable->setItem(row, 4, writeSpeedItem);
    
    // 滚动到新行
    m_historyTable->scrollToItem(diskItem);
}

void SpeedTestWidget::onBlockSizeChanged(int index) {
    Q_UNUSED(index);
    // 可以在这里添加一些逻辑来处理块大小变化
}

void SpeedTestWidget::onFileSizeChanged(int index) {
    Q_UNUSED(index);
    // 可以在这里添加一些逻辑来处理文件大小变化
}

#include "speedtestwidget.moc" 