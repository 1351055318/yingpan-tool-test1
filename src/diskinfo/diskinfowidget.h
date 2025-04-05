#ifndef DISKINFOWIDGET_H
#define DISKINFOWIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QGroupBox>
#include <QFormLayout>

#include "../core/diskutils.h"

class DiskInfoWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DiskInfoWidget(QWidget *parent = nullptr);
    ~DiskInfoWidget();

private slots:
    void onDiskSelectionChanged(int index);
    void onRefreshClicked();
    void onSaveInfoClicked();

private:
    void setupUI();
    void refreshDiskList();
    void displayDiskInfo(int index);
    void displayVolumeInfo(const DiskInfo &disk);
    QString getVolumeType(const QString &fileSystem);
    
private:
    QVBoxLayout *m_mainLayout;
    QHBoxLayout *m_controlLayout;
    
    QLabel *m_diskLabel;
    QComboBox *m_diskComboBox;
    QPushButton *m_refreshButton;
    QPushButton *m_saveInfoButton;
    
    QGroupBox *m_diskInfoGroup;
    QFormLayout *m_diskInfoLayout;
    QLabel *m_modelValueLabel;
    QLabel *m_serialValueLabel;
    QLabel *m_firmwareValueLabel;
    QLabel *m_interfaceValueLabel;
    QLabel *m_typeValueLabel;
    QLabel *m_sizeValueLabel;
    
    QGroupBox *m_volumeInfoGroup;
    QTableWidget *m_volumeTable;
    
    QList<DiskInfo> m_disks;
    int m_currentDiskIndex;
};

#endif // DISKINFOWIDGET_H 