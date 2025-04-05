#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTabWidget>
#include <QStatusBar>
#include <QMenuBar>
#include <QTimer>
#include <QSystemTrayIcon>
#include <QSettings>
#include <QCloseEvent>

#include "dashboard/dashboardwidget.h"
#include "diskinfo/diskinfowidget.h"
#include "speedtest/speedtestwidget.h"
#include "smart/smartwidget.h"
#include "spaceanalyzer/spaceanalyzerwidget.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    
protected:
    void closeEvent(QCloseEvent *event) override;
    
private slots:
    void onTabChanged(int index);
    void showAboutDialog();
    void showSettingsDialog();
    void refreshAllData();
    void onTrayIconActivated(QSystemTrayIcon::ActivationReason reason);
    void switchTheme(bool isDark);
    
private:
    void setupUI();
    void loadSettings();
    void saveSettings();
    void loadStyleSheet(const QString &fileName);
    void createMenus();
    void createTrayIcon();
    void checkForUpdates();
    
    QTabWidget *m_tabWidget;
    DashboardWidget *m_dashboardWidget;
    DiskInfoWidget *m_diskInfoWidget;
    SpeedTestWidget *m_speedTestWidget;
    SmartWidget *m_smartWidget;
    SpaceAnalyzerWidget *m_spaceAnalyzerWidget;
    
    QStatusBar *m_statusBar;
    QMenuBar *m_menuBar;
    QSystemTrayIcon *m_trayIcon;
    QTimer *m_refreshTimer;
    
    QSettings *m_settings;
    bool m_isDarkTheme;
};

#endif // MAINWINDOW_H 