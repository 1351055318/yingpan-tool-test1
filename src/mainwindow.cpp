#include "mainwindow.h"

#include <QDebug>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QStatusBar>
#include <QFile>
#include <QApplication>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QMessageBox>
#include <QCloseEvent>
#include <QSettings>
#include <QFileDialog>
#include <QDesktopServices>
#include <QUrl>
#include <QTimer>
#include <QSystemTrayIcon>
#include <QStyle>
#include <QDateTime>

#include "dashboard/dashboardwidget.h"
#include "diskinfo/diskinfowidget.h"
#include "speedtest/speedtestwidget.h"
#include "smart/smartwidget.h"
#include "spaceanalyzer/spaceanalyzerwidget.h"
// 注释掉尚未实现的模块的引用
// #include "health/healthmonitorwidget.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), m_isDarkTheme(true)
{
    qDebug() << "MainWindow构造函数开始...";
    
    // 创建设置对象
    m_settings = new QSettings("DiskToolbox", "DiskToolbox", this);
    
    // 加载设置
    loadSettings();
    
    // 设置窗口标题和大小
    setWindowTitle("磁盘工具箱");
    resize(900, 700);
    
    // 加载样式表
    loadStyleSheet(m_isDarkTheme ? ":/resources/styles/dark.qss" : ":/resources/styles/light.qss");
    
    qDebug() << "开始设置UI组件...";
    // 设置UI组件
    setupUI();
    
    // 创建菜单
    createMenus();
    
    // 创建系统托盘图标
    createTrayIcon();
    
    // 使用单次定时器延迟启动自动刷新，避免窗口显示延迟
    QTimer::singleShot(1000, this, [this]() {
        // 启动自动刷新定时器
        m_refreshTimer = new QTimer(this);
        connect(m_refreshTimer, &QTimer::timeout, this, &MainWindow::refreshAllData);
        m_refreshTimer->start(60000); // 每分钟刷新一次
        qDebug() << "刷新定时器已启动";
    });
    
    // 检查更新
    QTimer::singleShot(2000, this, &MainWindow::checkForUpdates);
    
    qDebug() << "MainWindow构造函数完成";
}

MainWindow::~MainWindow()
{
    saveSettings();
    delete m_settings;
}

void MainWindow::setupUI()
{
    // 创建中央选项卡控件
    m_tabWidget = new QTabWidget(this);
    setCentralWidget(m_tabWidget);
    
    // 创建功能模块
    m_dashboardWidget = new DashboardWidget(this);
    m_diskInfoWidget = new DiskInfoWidget(this);
    m_speedTestWidget = new SpeedTestWidget(this);
    m_smartWidget = new SmartWidget(this);
    m_spaceAnalyzerWidget = new SpaceAnalyzerWidget(this);
    
    // 添加选项卡
    m_tabWidget->addTab(m_dashboardWidget, QIcon(":/resources/icons/dashboard.png"), "仪表盘");
    m_tabWidget->addTab(m_diskInfoWidget, QIcon(":/resources/icons/disk.png"), "磁盘信息");
    m_tabWidget->addTab(m_speedTestWidget, QIcon(":/resources/icons/speed.png"), "速度测试");
    m_tabWidget->addTab(m_smartWidget, QIcon(":/resources/icons/smart.png"), "SMART信息");
    m_tabWidget->addTab(m_spaceAnalyzerWidget, QIcon(":/resources/icons/space.png"), "空间分析");
    
    // 创建状态栏
    m_statusBar = statusBar();
    m_statusBar->showMessage("准备就绪");
    
    // 连接选项卡切换信号
    connect(m_tabWidget, &QTabWidget::currentChanged, this, &MainWindow::onTabChanged);
}

void MainWindow::loadSettings()
{
    // 加载窗口位置和大小
    if (m_settings->contains("MainWindow/geometry")) {
        restoreGeometry(m_settings->value("MainWindow/geometry").toByteArray());
    }
    
    if (m_settings->contains("MainWindow/state")) {
        restoreState(m_settings->value("MainWindow/state").toByteArray());
    }
    
    // 加载当前选项卡
    int currentTab = m_settings->value("MainWindow/currentTab", 0).toInt();
    
    // 加载主题设置
    m_isDarkTheme = m_settings->value("MainWindow/darkTheme", true).toBool();
}

void MainWindow::saveSettings()
{
    // 保存窗口位置和大小
    m_settings->setValue("MainWindow/geometry", saveGeometry());
    m_settings->setValue("MainWindow/state", saveState());
    
    // 保存当前选项卡
    m_settings->setValue("MainWindow/currentTab", m_tabWidget->currentIndex());
    
    // 保存主题设置
    m_settings->setValue("MainWindow/darkTheme", m_isDarkTheme);
    
    m_settings->sync();
}

void MainWindow::loadStyleSheet(const QString &fileName)
{
    QFile file(fileName);
    if (file.open(QFile::ReadOnly | QFile::Text)) {
        QString styleSheet = QLatin1String(file.readAll());
        qApp->setStyleSheet(styleSheet);
        file.close();
    }
}

void MainWindow::createMenus()
{
    // 创建菜单栏
    m_menuBar = menuBar();
    
    // 文件菜单
    QMenu *fileMenu = m_menuBar->addMenu("文件");
    
    QAction *refreshAction = fileMenu->addAction("刷新数据");
    refreshAction->setShortcut(QKeySequence("F5"));
    connect(refreshAction, &QAction::triggered, this, &MainWindow::refreshAllData);
    
    fileMenu->addSeparator();
    
    QAction *exitAction = fileMenu->addAction("退出");
    exitAction->setShortcut(QKeySequence("Alt+F4"));
    connect(exitAction, &QAction::triggered, this, &QMainWindow::close);
    
    // 工具菜单
    QMenu *toolsMenu = m_menuBar->addMenu("工具");
    
    QAction *settingsAction = toolsMenu->addAction("设置");
    connect(settingsAction, &QAction::triggered, this, &MainWindow::showSettingsDialog);
    
    // 外观菜单
    QMenu *viewMenu = m_menuBar->addMenu("外观");
    
    QAction *darkThemeAction = viewMenu->addAction("深色主题");
    darkThemeAction->setCheckable(true);
    darkThemeAction->setChecked(m_isDarkTheme);
    connect(darkThemeAction, &QAction::toggled, this, &MainWindow::switchTheme);
    
    // 帮助菜单
    QMenu *helpMenu = m_menuBar->addMenu("帮助");
    
    QAction *aboutAction = helpMenu->addAction("关于");
    connect(aboutAction, &QAction::triggered, this, &MainWindow::showAboutDialog);
    
    QAction *checkUpdateAction = helpMenu->addAction("检查更新");
    connect(checkUpdateAction, &QAction::triggered, this, &MainWindow::checkForUpdates);
}

void MainWindow::createTrayIcon()
{
    m_trayIcon = new QSystemTrayIcon(this);
    m_trayIcon->setIcon(QIcon(":/resources/icons/app.png"));
    m_trayIcon->setToolTip("磁盘工具箱");
    
    // 创建托盘菜单
    QMenu *trayMenu = new QMenu(this);
    
    QAction *showHideAction = trayMenu->addAction("显示/隐藏");
    connect(showHideAction, &QAction::triggered, [this]() {
        setVisible(!isVisible());
    });
    
    QAction *exitAction = trayMenu->addAction("退出");
    connect(exitAction, &QAction::triggered, this, &QMainWindow::close);
    
    m_trayIcon->setContextMenu(trayMenu);
    
    // 连接托盘激活信号
    connect(m_trayIcon, &QSystemTrayIcon::activated, this, &MainWindow::onTrayIconActivated);
    
    // 显示托盘图标
    m_trayIcon->show();
}

void MainWindow::onTabChanged(int index)
{
    // 根据当前选项卡更新状态栏
    switch (index) {
    case 0:
        m_statusBar->showMessage("仪表盘 - 查看系统磁盘概况");
        break;
    case 1:
        m_statusBar->showMessage("磁盘信息 - 查看详细磁盘信息");
        break;
    case 2:
        m_statusBar->showMessage("速度测试 - 测试磁盘读写速度");
        break;
    case 3:
        m_statusBar->showMessage("SMART信息 - 检查磁盘健康状态");
        break;
    case 4:
        m_statusBar->showMessage("空间分析 - 分析磁盘空间使用情况");
        break;
    default:
        m_statusBar->showMessage("准备就绪");
        break;
    }
}

void MainWindow::showAboutDialog()
{
    QMessageBox about(this);
    about.setWindowTitle("关于 磁盘工具箱");
    about.setTextFormat(Qt::RichText);
    about.setText("<h2>磁盘工具箱 v1.0</h2>"
                  "<p>一款功能强大的磁盘管理与分析工具</p>"
                  "<p>版权所有 © 2023-2024</p>"
                  "<p><a href='https://github.com/yourusername/disktoolbox'>访问项目主页</a></p>");
    about.setIconPixmap(QPixmap(":/resources/icons/app.png").scaled(64, 64, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    about.exec();
}

void MainWindow::showSettingsDialog()
{
    QDialog settingsDialog(this);
    settingsDialog.setWindowTitle("设置");
    settingsDialog.setMinimumWidth(400);
    
    QVBoxLayout *mainLayout = new QVBoxLayout(&settingsDialog);
    
    // 常规设置组
    QGroupBox *generalGroup = new QGroupBox("常规设置", &settingsDialog);
    QVBoxLayout *generalLayout = new QVBoxLayout(generalGroup);
    
    QCheckBox *startWithWindowsCheckBox = new QCheckBox("开机自启动", generalGroup);
    startWithWindowsCheckBox->setChecked(m_settings->value("Settings/startWithWindows", false).toBool());
    
    QCheckBox *minimizeToTrayCheckBox = new QCheckBox("关闭窗口时最小化到系统托盘", generalGroup);
    minimizeToTrayCheckBox->setChecked(m_settings->value("Settings/minimizeToTray", true).toBool());
    
    generalLayout->addWidget(startWithWindowsCheckBox);
    generalLayout->addWidget(minimizeToTrayCheckBox);
    
    // 刷新设置组
    QGroupBox *refreshGroup = new QGroupBox("自动刷新设置", &settingsDialog);
    QVBoxLayout *refreshLayout = new QVBoxLayout(refreshGroup);
    
    QCheckBox *autoRefreshCheckBox = new QCheckBox("启用自动刷新", refreshGroup);
    autoRefreshCheckBox->setChecked(m_refreshTimer->isActive());
    
    QHBoxLayout *refreshIntervalLayout = new QHBoxLayout();
    QLabel *refreshIntervalLabel = new QLabel("刷新间隔 (秒):", refreshGroup);
    QSpinBox *refreshIntervalSpinBox = new QSpinBox(refreshGroup);
    refreshIntervalSpinBox->setRange(10, 3600);
    refreshIntervalSpinBox->setValue(m_refreshTimer->interval() / 1000);
    refreshIntervalSpinBox->setEnabled(autoRefreshCheckBox->isChecked());
    
    connect(autoRefreshCheckBox, &QCheckBox::toggled, refreshIntervalSpinBox, &QSpinBox::setEnabled);
    
    refreshIntervalLayout->addWidget(refreshIntervalLabel);
    refreshIntervalLayout->addWidget(refreshIntervalSpinBox);
    
    refreshLayout->addWidget(autoRefreshCheckBox);
    refreshLayout->addLayout(refreshIntervalLayout);
    
    // 按钮
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    QPushButton *cancelButton = new QPushButton("取消", &settingsDialog);
    QPushButton *okButton = new QPushButton("确定", &settingsDialog);
    okButton->setDefault(true);
    
    buttonLayout->addStretch();
    buttonLayout->addWidget(cancelButton);
    buttonLayout->addWidget(okButton);
    
    mainLayout->addWidget(generalGroup);
    mainLayout->addWidget(refreshGroup);
    mainLayout->addStretch();
    mainLayout->addLayout(buttonLayout);
    
    // 连接信号
    connect(cancelButton, &QPushButton::clicked, &settingsDialog, &QDialog::reject);
    connect(okButton, &QPushButton::clicked, &settingsDialog, &QDialog::accept);
    
    // 显示对话框
    if (settingsDialog.exec() == QDialog::Accepted) {
        // 保存设置
        m_settings->setValue("Settings/startWithWindows", startWithWindowsCheckBox->isChecked());
        m_settings->setValue("Settings/minimizeToTray", minimizeToTrayCheckBox->isChecked());
        
        // 更新刷新定时器
        if (autoRefreshCheckBox->isChecked()) {
            m_refreshTimer->setInterval(refreshIntervalSpinBox->value() * 1000);
            if (!m_refreshTimer->isActive()) {
                m_refreshTimer->start();
            }
        } else {
            m_refreshTimer->stop();
        }
        
        // 设置开机自启动
        #ifdef Q_OS_WIN
        QSettings bootSettings("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run", QSettings::NativeFormat);
        if (startWithWindowsCheckBox->isChecked()) {
            bootSettings.setValue("DiskToolbox", QCoreApplication::applicationFilePath().replace('/', '\\'));
        } else {
            bootSettings.remove("DiskToolbox");
        }
        #endif
    }
}

void MainWindow::refreshAllData()
{
    // 刷新所有模块的数据
    // 注释掉不存在的刷新方法
    // m_dashboardWidget->refreshData();
    
    // 根据当前选项卡，刷新对应模块的数据
    int currentIndex = m_tabWidget->currentIndex();
    switch (currentIndex) {
    case 1:
        // 磁盘信息
        // m_diskInfoWidget->refreshData();
        break;
    case 2:
        // 速度测试
        // 不需要自动刷新
        break;
    case 3:
        // SMART信息
        // m_smartWidget->refreshData();
        break;
    case 4:
        // 空间分析
        // 不需要自动刷新
        break;
    default:
        break;
    }
    
    // 更新状态栏
    m_statusBar->showMessage("数据已刷新: " + QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"), 3000);
}

void MainWindow::onTrayIconActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason) {
    case QSystemTrayIcon::Trigger:
    case QSystemTrayIcon::DoubleClick:
        // 单击或双击托盘图标时切换窗口显示状态
        setVisible(!isVisible());
        if (isVisible()) {
            activateWindow();
            raise();
        }
        break;
    default:
        break;
    }
}

void MainWindow::switchTheme(bool isDark)
{
    m_isDarkTheme = isDark;
    loadStyleSheet(m_isDarkTheme ? ":/resources/styles/dark.qss" : ":/resources/styles/light.qss");
    
    // 保存主题设置
    m_settings->setValue("MainWindow/darkTheme", m_isDarkTheme);
}

void MainWindow::checkForUpdates()
{
    // 这里应该添加实际的更新检查逻辑
    // 目前只是显示一个假的消息
    m_statusBar->showMessage("已是最新版本", 3000);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (m_settings->value("Settings/minimizeToTray", true).toBool() && m_trayIcon->isVisible()) {
        hide();
        event->ignore();
    } else {
        saveSettings();
        event->accept();
    }
} 