#include "smartfactory.h"
#include <QDebug>
#include <QProcess>
#include <QRegularExpression>

SmartFactory::SmartFactory(QObject *parent) : QObject(parent)
{
}

SmartFactory::~SmartFactory()
{
}

SmartData* SmartFactory::createSmartData(const QString &diskPath, QObject *parent)
{
    // 使用磁盘检测器来确定磁盘类型
    qDebug() << "---Factory调试---开始为磁盘创建SMART数据处理器，路径:" << diskPath;
    DiskType type = DiskDetector::detectDiskType(diskPath);
    qDebug() << "---Factory调试---检测到磁盘类型:" << (type == DiskType::SATA ? "SATA" : 
                                                  (type == DiskType::NVMe ? "NVMe" : "Unknown"));
    
    // 根据检测结果创建相应的处理器
    SmartData* processor = nullptr;
    switch (type) {
        case DiskType::SATA:
            qDebug() << "创建SATA设备SMART数据处理器，磁盘路径:" << diskPath;
            qDebug() << "---Factory调试---创建SATA设备处理器";
            processor = new SATAData(parent);
            break;
        
        case DiskType::NVMe:
            qDebug() << "创建NVMe设备SMART数据处理器，磁盘路径:" << diskPath;
            qDebug() << "---Factory调试---创建NVMe设备处理器";
            processor = new NVMeData(parent);
            break;
            
        default:
            // 如果无法确定类型，默认使用基础实现
            qDebug() << "无法确定磁盘类型，使用基础SMART数据处理器，磁盘路径:" << diskPath;
            qDebug() << "---Factory调试---无法确定磁盘类型，使用基础处理器";
            processor = new SmartData(parent);
            break;
    }
    
    qDebug() << "---Factory调试---SMART数据处理器创建完成";
    
    // 确保处理器有正确的磁盘类型信息
    if (processor) {
        qDebug() << "---Factory调试---调用detectDiskType方法确保设置正确的磁盘类型";
        processor->detectDiskType(diskPath);
        qDebug() << "---Factory调试---磁盘类型设置完成";
    }
    
    return processor;
}

QList<DiskSMARTInfo> SmartFactory::detectDisks()
{
    // 创建磁盘检测器并检测系统中的硬盘
    qDebug() << "---Factory调试---开始检测系统硬盘...";
    DiskDetector detector;
    QList<DiskSMARTInfo> disks = detector.detectDisks();
    qDebug() << "---Factory调试---检测到" << disks.size() << "个硬盘";
    for (const DiskSMARTInfo &disk : disks) {
        qDebug() << "---Factory调试---磁盘信息: 路径=" << disk.diskPath
                 << ", 类型=" << (disk.diskType == DiskType::SATA ? "SATA" : 
                                 (disk.diskType == DiskType::NVMe ? "NVMe" : "Unknown"))
                 << ", 型号=" << disk.model
                 << ", 序列号=" << disk.serialNumber;
    }
    return disks;
} 