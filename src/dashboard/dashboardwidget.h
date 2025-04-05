#ifndef DASHBOARDWIDGET_H
#define DASHBOARDWIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QComboBox>
#include <QPushButton>
#include <QProgressBar>
#include <QGroupBox>
#include <QTimer>
#include <QtCharts/QChartView>
#include <QtCharts/QPieSeries>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>
#include <QtCharts/QChart>

#include "../core/diskutils.h"
#include "../core/smartdata.h"
#include "../core/smartfactory.h"

QT_CHARTS_USE_NAMESPACE

class DashboardWidget : public QWidget
{
    Q_OBJECT
    
public:
    explicit DashboardWidget(QWidget *parent = nullptr);
    ~DashboardWidget();
    
    void refreshData();
    
private slots:
    void onDiskSelectionChanged(int index);
    void onRefreshClicked();
    void updateDashboard();
    void onTimerTimeout();
    
private:
    void setupUI();
    void refreshDiskList();
    void createDiskUsageChart();
    void createTemperatureChart();
    void createHealthChart();
    void updateDiskUsageChart();
    void updateTemperatureChart();
    void updateHealthChart();
    void updateInfoLabels();
    void loadSmartData(const QString &diskPath);
    
private:
    QVBoxLayout *m_mainLayout;
    QHBoxLayout *m_controlLayout;
    QGridLayout *m_chartLayout;
    QHBoxLayout *m_infoLayout;
    
    QLabel *m_diskLabel;
    QComboBox *m_diskComboBox;
    QPushButton *m_refreshButton;
    
    QChartView *m_diskUsageChartView;
    QChart *m_diskUsageChart;
    QPieSeries *m_diskUsageSeries;
    
    QChartView *m_temperatureChartView;
    QChart *m_temperatureChart;
    QLineSeries *m_temperatureSeries;
    QValueAxis *m_temperatureAxisX;
    QValueAxis *m_temperatureAxisY;
    QList<qreal> m_temperatureHistory;
    
    QChartView *m_healthChartView;
    QChart *m_healthChart;
    QPieSeries *m_healthSeries;
    
    QLabel *m_modelLabel;
    QLabel *m_modelValueLabel;
    QLabel *m_sizeLabel;
    QLabel *m_sizeValueLabel;
    QLabel *m_interfaceLabel;
    QLabel *m_interfaceValueLabel;
    QLabel *m_typeLabel;
    QLabel *m_typeValueLabel;
    QLabel *m_healthLabel;
    QProgressBar *m_healthProgressBar;
    
    QLabel *m_powerOnHoursLabel;
    QLabel *m_powerOnHoursValueLabel;
    QLabel *m_temperatureLabel;
    QLabel *m_temperatureValueLabel;
    QLabel *m_startStopLabel;
    QLabel *m_startStopValueLabel;
    QLabel *m_warningLabel;
    
    QTimer *m_refreshTimer;
    
    QList<DiskInfo> m_diskList;
    int m_currentDiskIndex;
    SmartData *m_smartData;
};

#endif // DASHBOARDWIDGET_H 