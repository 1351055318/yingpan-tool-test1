#ifndef DISKDETECTOR_H
#define DISKDETECTOR_H

#include <QObject>
#include <QList>
#include <QString>
#include "smartdata.h"

// 硬盘信息结构
struct DiskSMARTInfo {
    QString diskPath;        // 磁盘路径
    DiskType diskType;       // 磁盘类型
    QString model;           // 型号
    QString serialNumber;    // 序列号
    qint64 size;             // 容量（字节）
    
    DiskSMARTInfo() : diskType(DiskType::Unknown), size(0) {}
};

class DiskDetector : public QObject
{
    Q_OBJECT

public:
    explicit DiskDetector(QObject *parent = nullptr);
    ~DiskDetector();
    
    // 检测系统中的硬盘
    QList<DiskSMARTInfo> detectDisks();
    
    // 检测指定路径的硬盘类型
    static DiskType detectDiskType(const QString &diskPath);
    
    // 运行smartctl命令并处理输出
    static QString runSmartCtl(const QString &diskPath, const QString &params);
    
private:
    // 解析SATA设备信息
    void parseSATADeviceInfo(DiskSMARTInfo &diskInfo, const QString &output);
    
    // 解析NVMe设备信息
    void parseNVMeDeviceInfo(DiskSMARTInfo &diskInfo, const QString &output);
    
    // 生成模拟磁盘数据
    QList<DiskSMARTInfo> simulateDisks();
};

#endif // DISKDETECTOR_H 