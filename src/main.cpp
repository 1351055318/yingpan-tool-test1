#include <QApplication>
#include <QFile>
#include <QDir>
#include <QDebug>
#include <QTimer>
#include <QMessageBox>
#include <QStyleFactory>
#include <QSplashScreen>
#include <QPixmap>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    // 设置高DPI支持
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
    
    qDebug() << "程序启动...";
    qDebug() << "支持的样式:" << QStyleFactory::keys();
    
    QApplication app(argc, argv);
    
    // 设置应用程序信息
    app.setApplicationName("硬盘工具箱");
    app.setApplicationVersion("0.1");
    app.setOrganizationName("DiskToolbox");
    
    qDebug() << "尝试创建启动画面...";
    // 创建启动画面
    QSplashScreen *splash = nullptr;
    
    // 使用资源文件中的图片作为启动画面
    QPixmap pixmap(":/resources/images/splash.png");
    if (pixmap.isNull()) {
        // 如果无法加载图片，创建一个简单的替代启动画面
        pixmap = QPixmap(400, 250);
        pixmap.fill(Qt::darkGray);
    }
    
    splash = new QSplashScreen(pixmap);
    splash->show();
    app.processEvents();
    
    splash->showMessage("正在加载界面...", Qt::AlignCenter, Qt::white);
    qDebug() << "显示启动画面";
    
    // 应用深色主题样式表
    qDebug() << "尝试加载样式表...";
    QFile styleFile(":/resources/styles/dark.qss");
    if (styleFile.open(QFile::ReadOnly)) {
        QString styleSheet = QLatin1String(styleFile.readAll());
        app.setStyleSheet(styleSheet);
        styleFile.close();
        qDebug() << "样式表加载成功";
    } else {
        qDebug() << "无法加载样式表文件:" << styleFile.errorString();
    }
    
    splash->showMessage("创建主窗口...", Qt::AlignCenter, Qt::white);
    app.processEvents();
    
    // 创建主窗口但先不显示
    MainWindow *mainWindow = new MainWindow();
    
    // 延迟显示主窗口
    QTimer::singleShot(2000, [mainWindow, splash]() {
        if (splash) {
            splash->finish(mainWindow);
            delete splash;
        }
        mainWindow->show();
        mainWindow->raise();
        mainWindow->activateWindow();
        qDebug() << "主窗口显示完成";
    });
    
    int result = app.exec();
    qDebug() << "程序退出，代码:" << result;
    return result;
} 