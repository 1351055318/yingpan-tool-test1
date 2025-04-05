#include <QApplication>
#include <QLabel>
#include <QMainWindow>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    QMainWindow window;
    window.setWindowTitle("Qt测试");
    
    QLabel *label = new QLabel("Qt环境测试成功！");
    window.setCentralWidget(label);
    
    window.resize(300, 200);
    window.show();
    
    return app.exec();
} 