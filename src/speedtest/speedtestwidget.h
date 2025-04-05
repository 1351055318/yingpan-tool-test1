#ifndef SPEEDTESTWIDGET_H
#define SPEEDTESTWIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QComboBox>
#include <QSpinBox>
#include <QPushButton>
#include <QTableWidget>
#include <QGroupBox>
#include <QProgressBar>
#include <QThread>
#include <QtCharts/QChartView>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QValueAxis>
#include <QtCharts/QBarCategoryAxis>
#include <QDateTime>
#include "../core/diskutils.h"

QT_CHARTS_USE_NAMESPACE

class SpeedTester;

// 重命名为SpeedTestHistoryItem避免冲突
struct SpeedTestHistoryItem {
    double readSpeed;
    double writeSpeed;
    QString diskName;
    int blockSizeKB;
    int fileSizeMB;
    QDateTime timestamp;
};

class SpeedTestWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SpeedTestWidget(QWidget *parent = nullptr);
    ~SpeedTestWidget();

private slots:
    void onDiskSelectionChanged(int index);
    void onStartTestClicked();
    void onCancelTestClicked();
    void onExportResultsClicked();
    void onBlockSizeChanged(int index);
    void onFileSizeChanged(int index);
    void onTestCompleted(double readSpeed, double writeSpeed);

private:
    void setupUI();
    void refreshDiskList();
    void createChart();
    void updateChart(double readSpeed, double writeSpeed);
    void addResultToHistory(double readSpeed, double writeSpeed);
    void updateProgress(int percent, double currentSpeed, bool isRead);

    QVBoxLayout *m_mainLayout;
    QLabel *m_diskLabel;
    QComboBox *m_diskComboBox;
    
    QLabel *m_blockSizeLabel;
    QComboBox *m_blockSizeComboBox;
    
    QLabel *m_fileSizeLabel;
    QComboBox *m_fileSizeComboBox;
    
    QPushButton *m_startButton;
    QPushButton *m_cancelButton;
    QPushButton *m_exportButton;

    QGroupBox *m_testStatusGroup;
    QLabel *m_testStatusLabel;
    QProgressBar *m_progressBar;

    QGroupBox *m_resultsGroup;
    QLabel *m_readSpeedLabel;
    QLabel *m_writeSpeedLabel;
    QChartView *m_chartView;

    QGroupBox *m_historyGroup;
    QTableWidget *m_historyTable;

    QList<DiskInfo> m_diskList;
    DiskInfo m_selectedDisk;
    QList<SpeedTestHistoryItem> m_testHistory;
    
    SpeedTester *m_tester;
    QThread *m_testerThread;
    bool m_testRunning;
};

#endif // SPEEDTESTWIDGET_H 