#ifndef SMARTWIDGET_H
#define SMARTWIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QComboBox>
#include <QPushButton>
#include <QTableWidget>
#include <QGroupBox>
#include <QProgressBar>
#include <QTimer>
#include <QtCharts/QChartView>
#include <QtCharts/QPieSeries>
#include <QtCharts/QPieSlice>
#include <QtCharts/QChart>
#include "../core/diskutils.h"
#include "../core/smartdata.h"

QT_CHARTS_USE_NAMESPACE

// 定义SMART状态枚举
enum SmartStatus {
    Good,
    Warning,
    Critical,
    Unknown
};

// 定义属性名称语言枚举
enum SmartAttributeLanguage {
    English,
    Chinese
};

class SmartWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SmartWidget(QWidget *parent = nullptr);
    ~SmartWidget();

private slots:
    void onDiskSelectionChanged(int index);
    void onRefreshClicked();
    void onSaveSmartInfoClicked();
    void onTimerTimeout();
    void onLanguageToggleClicked();

private:
    void setupUI();
    void refreshDiskList();
    void loadSmartData(int diskIndex);
    void updateAttributesTable();
    void updateHealthChart();
    void updateSummaryInfo();
    QString getStatusText(SmartStatus status);
    void loadSmartData();
    void refreshData();
    QString translateAttributeName(const QString &name);
    void updateLanguageButtonText();

    QVBoxLayout *m_mainLayout;
    QHBoxLayout *m_controlLayout;
    QHBoxLayout *m_contentLayout;
    QVBoxLayout *m_infoLayout;
    
    QLabel *m_diskLabel;
    QComboBox *m_diskComboBox;
    QPushButton *m_refreshButton;
    QPushButton *m_saveInfoButton;
    QPushButton *m_languageToggleButton;  // 新增语言切换按钮
    
    QGroupBox *m_attributesGroup;
    QTableWidget *m_attributesTable;
    
    QGroupBox *m_healthGroup;
    QVBoxLayout *m_healthLayout;
    QProgressBar *m_healthScoreBar;
    QLabel *m_healthScoreLabel;
    QChartView *m_healthChartView;
    
    QGroupBox *m_summaryGroup;
    QVBoxLayout *m_summaryLayout;
    QLabel *m_deviceTypeLabel;
    QLabel *m_temperatureLabel;
    QLabel *m_powerOnLabel;
    QLabel *m_startStopLabel;
    QLabel *m_warningLabel;
    
    QList<DiskInfo> m_diskList;
    int m_currentDiskIndex;
    SmartData *m_smartData;
    QTimer *m_refreshTimer;
    SmartAttributeLanguage m_language;  // 新增语言设置
};

#endif // SMARTWIDGET_H 