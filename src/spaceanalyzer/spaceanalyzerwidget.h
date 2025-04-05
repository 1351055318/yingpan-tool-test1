#ifndef SPACEANALYZERWIDGET_H
#define SPACEANALYZERWIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QComboBox>
#include <QPushButton>
#include <QTreeWidget>
#include <QGroupBox>
#include <QProgressBar>
#include <QLineEdit>
#include <QSpinBox>
#include <QCheckBox>
#include <QThread>
#include <QtCharts/QChartView>
#include <QtCharts/QPieSeries>
#include <QtCharts/QBarSeries>

#include "../core/diskutils.h"

// 目录大小计算线程
class DirSizeWorker : public QObject
{
    Q_OBJECT
    
public:
    explicit DirSizeWorker(QObject *parent = nullptr);
    void setDirectory(const QString &path);
    void stop();
    
public slots:
    void process();
    
signals:
    void resultReady(const QString &path, qint64 size, int fileCount, int dirCount);
    void progress(const QString &currentPath, int level);
    void finished();
    
private:
    qint64 calculateDirSize(const QString &path, int level, int &fileCount, int &dirCount);
    
    QString m_rootPath;
    bool m_stopped;
};

// 文件项结构
struct FileItem
{
    QString path;
    QString name;
    qint64 size;
    bool isDir;
    int fileCount;
    int dirCount;
    QMap<QString, FileItem*> children;
    FileItem *parent;
    
    FileItem() : size(0), isDir(true), fileCount(0), dirCount(0), parent(nullptr) {}
    ~FileItem() { qDeleteAll(children); }
};

class SpaceAnalyzerWidget : public QWidget
{
    Q_OBJECT
    
public:
    explicit SpaceAnalyzerWidget(QWidget *parent = nullptr);
    ~SpaceAnalyzerWidget();
    
private slots:
    void onVolumeSelectionChanged(int index);
    void onScanButtonClicked();
    void onStopButtonClicked();
    void onDirResultReady(const QString &path, qint64 size, int fileCount, int dirCount);
    void onScanProgress(const QString &currentPath, int level);
    void onScanFinished();
    void onTreeItemClicked(QTreeWidgetItem *item, int column);
    void onTreeItemExpanded(QTreeWidgetItem *item);
    void onPathChanged();
    void onRefreshButtonClicked();
    void onExportButtonClicked();
    void onFilterChanged();
    
private:
    void setupUI();
    void refreshVolumeList();
    void updateChart(FileItem *item);
    void updateFileList(FileItem *item);
    void addItemToTree(QTreeWidgetItem *parentItem, FileItem *fileItem, int minSize = 0);
    void clearResults();
    QString formatSize(qint64 size) const;
    QString formatPath(const QString &path) const;
    
    // UI元素
    QVBoxLayout *m_mainLayout;
    QComboBox *m_volumeComboBox;
    QLineEdit *m_pathEdit;
    QPushButton *m_scanButton;
    QPushButton *m_stopButton;
    QPushButton *m_refreshButton;
    QPushButton *m_exportButton;
    QTreeWidget *m_dirTreeWidget;
    QTreeWidget *m_fileListWidget;
    QProgressBar *m_scanProgressBar;
    QLabel *m_scanStatusLabel;
    QtCharts::QChartView *m_chartView;
    QSpinBox *m_minSizeSpinBox;
    QCheckBox *m_showFilesCheckBox;
    QLineEdit *m_filterEdit;
    
    // 数据
    QList<VolumeInfo> m_volumes;
    VolumeInfo m_selectedVolume;
    FileItem *m_rootItem;
    
    // 当前已扫描状态
    bool m_scanning;
    QThread *m_workerThread;
    DirSizeWorker *m_worker;
    int m_totalItems;
    int m_processedItems;
};

#endif // SPACEANALYZERWIDGET_H 