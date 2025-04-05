#ifndef DISKUTILS_H
#define DISKUTILS_H

#include <QString>
#include <QList>
#include <QMap>
#include <QStorageInfo>
#include <QStringList>

// 卷信息结构
struct VolumeInfo {
    QString rootPath;        // 根路径
    QString label;           // 卷标
    QString fileSystem;      // 文件系统类型
    QString driveLetter;     // 驱动器字母（如 C:）
    qint64 totalSpace = 0;   // 总空间
    qint64 usedSpace = 0;    // 已用空间
    qint64 freeSpace = 0;    // 可用空间
    
    VolumeInfo() : totalSpace(0), usedSpace(0), freeSpace(0) {}
};

// 磁盘信息结构
struct DiskInfo {
    int index = -1;          // 物理磁盘索引
    QString diskName;        // 磁盘名称/标识
    QString diskPath;        // 磁盘路径
    QString model;           // 型号
    QString manufacturer;    // 制造商
    QString serialNumber;    // 序列号
    QString firmwareVersion; // 固件版本
    QString type;            // 类型 (HDD, SSD, USB等)
    QString interfaceSpeed;  // 接口速度 (SATA III, USB 3.0等)
    
    qint64 diskSize = 0;     // 总大小(字节)
    int sectorSize = 0;      // 扇区大小(字节)
    qint64 cylinders = 0;    // 柱面数
    int tracksPerCylinder = 0; // 每柱面磁道数
    int sectorsPerTrack = 0; // 每磁道扇区数
    
    // 关联分区信息
    QStringList volumes;     // 关联的分区（驱动器字母列表）
    QString mainVolume;      // 主要关联分区（如C:）
    QString volumePath;      // 主要分区的完整路径
    
    DiskInfo() : index(-1), diskSize(0), sectorSize(0), cylinders(0), tracksPerCylinder(0), sectorsPerTrack(0) {}
};

// 磁盘使用情况结构
struct DiskUsage {
    qint64 totalSpace = 0;   // 总空间(字节)
    qint64 usedSpace = 0;    // 已用空间(字节)
    qint64 freeSpace = 0;    // 可用空间(字节)
    double usedPercent = 0;  // 使用百分比
    
    DiskUsage() : totalSpace(0), usedSpace(0), freeSpace(0), usedPercent(0.0) {}
};

// 速度测试结果结构
struct SpeedTestResult {
    double readSpeed = 0;    // 读取速度(MB/s)
    double writeSpeed = 0;   // 写入速度(MB/s)
    
    SpeedTestResult() : readSpeed(0.0), writeSpeed(0.0) {}
};

class DiskUtils {
public:
    // 获取所有物理磁盘信息
    static QList<DiskInfo> getAllDisks();
    
    // 获取所有分区信息
    static QList<VolumeInfo> getVolumes();
    
    // 获取指定磁盘的信息
    static DiskInfo getDiskInfo(const QString& diskPath);
    
    // 获取卷的使用情况
    static DiskUsage getDiskUsage(const QString& volumePath);
    
    // 获取磁盘温度(摄氏度)
    static int getDiskTemperature(const QString& diskPath);
    
    // 运行磁盘速度测试
    static SpeedTestResult runSpeedTest(const QString& diskPath, int blockSize, int fileSize);
    
    // 获取卷信息
    static QMap<QString, QString> getVolumeInfo(const QString& volumePath);
    
    // 格式化字节大小为可读字符串
    static QString formatSize(qint64 bytes);
    
    // 模拟数据方法（用于测试）
    static QList<DiskInfo> getSimulatedDisks();
    static DiskUsage getSimulatedDiskUsage(const QString& volumePath);
    static int getSimulatedTemperature();
    static SpeedTestResult getSimulatedSpeedTest(int blockSize);

    // 计算一个磁盘上已经使用的空间总和
    static qint64 getUsedSizeForDisk(const DiskInfo& disk, const QList<VolumeInfo>& volumes);

private:
    // 获取物理磁盘信息
    static QList<DiskInfo> getPhysicalDisks();
    
    // 关联物理磁盘和逻辑分区
    static void associateVolumesToDisks(QList<DiskInfo>& disks, const QList<VolumeInfo>& volumes);
    
    // 解析CSV格式数据行
    static QStringList parseCSVLine(const QString& line);
    
    // 通过Windows API获取硬盘序列号、型号和固件版本
    static void enrichDiskInfoWithWindowsAPI(DiskInfo& diskInfo);
};

#endif // DISKUTILS_H 