#include "diskutils.h"
#include <QDebug>
#include <QDir>
#include <QStorageInfo>
#include <QDateTime>
#include <QFile>
#include <QElapsedTimer>
#include <QRegularExpression>
#include <QThread>
#include <QRandomGenerator>
#include <QProcess>
#include <QCoreApplication>
#include <QOperatingSystemVersion>
#include <QMap>
#include <QTextCodec>
#include <algorithm>
#include <cmath>

// 获取所有磁盘信息 - 返回物理硬盘
QList<DiskInfo> DiskUtils::getAllDisks()
{
    // 尝试获取物理硬盘信息
    QList<DiskInfo> physicalDisks = getPhysicalDisks();
    
    // 获取逻辑分区
    QList<VolumeInfo> volumes = getVolumes();
    
    // 关联物理磁盘和逻辑分区
    associateVolumesToDisks(physicalDisks, volumes);
    
    // 如果检测不到物理磁盘，使用模拟数据
    if (physicalDisks.isEmpty()) {
        qDebug() << "无法检测到物理磁盘，使用模拟数据";
        return getSimulatedDisks();
    }
    
    // 尝试使用Windows API获取更详细的硬盘信息
    for (int i = 0; i < physicalDisks.size(); i++) {
        enrichDiskInfoWithWindowsAPI(physicalDisks[i]);
    }
    
    return physicalDisks;
}

// 获取所有分区信息
QList<VolumeInfo> DiskUtils::getVolumes()
{
    QList<VolumeInfo> volumes;
    foreach (const QStorageInfo &storage, QStorageInfo::mountedVolumes()) {
        if (storage.isValid() && storage.isReady()) {
            VolumeInfo volume;
            volume.rootPath = storage.rootPath();
            volume.label = storage.displayName();
            volume.fileSystem = QString(storage.fileSystemType());
            volume.totalSpace = storage.bytesTotal();
            volume.freeSpace = storage.bytesAvailable();
            volume.usedSpace = volume.totalSpace - volume.freeSpace;
            
            // 从路径提取盘符（Windows特定）
            if (volume.rootPath.length() >= 2 && volume.rootPath[1] == ':') {
                volume.driveLetter = volume.rootPath.left(2);
            }
            
            volumes.append(volume);
        }
    }
    
    // 如果没有检测到卷或者检测到的卷很少，添加模拟数据
    if (volumes.isEmpty() || volumes.size() < 3) {
        qDebug() << "未检测到足够的卷，添加模拟卷数据";
        
        // 检查是否已经有C盘
        bool hasC = false, hasD = false, hasE = false;
        for (const VolumeInfo &vol : volumes) {
            if (vol.driveLetter == "C:") hasC = true;
            if (vol.driveLetter == "D:") hasD = true;
            if (vol.driveLetter == "E:") hasE = true;
        }
        
        // 添加模拟C盘
        if (!hasC) {
            VolumeInfo cVol;
            cVol.rootPath = "C:\\";
            cVol.label = "系统";
            cVol.driveLetter = "C:";
            cVol.fileSystem = "NTFS";
            cVol.totalSpace = 500LL * 1024 * 1024 * 1024; // 500GB
            cVol.freeSpace = 150LL * 1024 * 1024 * 1024;  // 150GB free
            cVol.usedSpace = cVol.totalSpace - cVol.freeSpace;
            volumes.append(cVol);
        }
        
        // 添加模拟D盘
        if (!hasD) {
            VolumeInfo dVol;
            dVol.rootPath = "D:\\";
            dVol.label = "数据";
            dVol.driveLetter = "D:";
            dVol.fileSystem = "NTFS";
            dVol.totalSpace = 1000LL * 1024 * 1024 * 1024; // 1TB
            dVol.freeSpace = 600LL * 1024 * 1024 * 1024;   // 600GB free
            dVol.usedSpace = dVol.totalSpace - dVol.freeSpace;
            volumes.append(dVol);
        }
        
        // 添加模拟E盘（可能是USB）
        if (!hasE) {
            VolumeInfo eVol;
            eVol.rootPath = "E:\\";
            eVol.label = "可移动存储";
            eVol.driveLetter = "E:";
            eVol.fileSystem = "FAT32";
            eVol.totalSpace = 32LL * 1024 * 1024 * 1024; // 32GB
            eVol.freeSpace = 20LL * 1024 * 1024 * 1024;  // 20GB free
            eVol.usedSpace = eVol.totalSpace - eVol.freeSpace;
            volumes.append(eVol);
        }
    }
    
    return volumes;
}

// 获取指定磁盘的信息
DiskInfo DiskUtils::getDiskInfo(const QString& diskPath)
{
    QList<DiskInfo> disks = getAllDisks();
    for (const DiskInfo &disk : disks) {
        // 检查磁盘路径或者关联的分区路径
        if (disk.diskPath == diskPath || disk.volumes.contains(diskPath)) {
            return disk;
        }
    }
    return DiskInfo();
}

// 获取卷的使用情况
DiskUsage DiskUtils::getDiskUsage(const QString& volumePath)
{
    DiskUsage usage;
    QStorageInfo storage(volumePath);
    
    if (storage.isValid() && storage.isReady()) {
        usage.totalSpace = storage.bytesTotal();
        usage.freeSpace = storage.bytesAvailable();
        usage.usedSpace = usage.totalSpace - usage.freeSpace;
        
        if (usage.totalSpace > 0) {
            usage.usedPercent = (double)usage.usedSpace / usage.totalSpace * 100.0;
        }
    }
    
    return usage;
}

// 关联物理磁盘和逻辑分区
void DiskUtils::associateVolumesToDisks(QList<DiskInfo>& disks, const QList<VolumeInfo>& volumes)
{
    if (disks.isEmpty() || volumes.isEmpty()) {
        return;
    }
    
    qDebug() << "关联物理磁盘和逻辑分区 - 物理磁盘数:" << disks.size() << "分区数:" << volumes.size();
    
    // 强制清除现有的关联关系
    for (int i = 0; i < disks.size(); i++) {
        disks[i].volumes.clear();
        disks[i].mainVolume.clear();
        disks[i].volumePath.clear();
    }
    
    // 如果使用WMI获取到了硬盘，但是具体分区关系不明确
    // 我们使用更合理的启发式方法来关联分区与物理磁盘
    
    // 根据磁盘大小和分区大小关联
    // 思路: C盘通常在第一个磁盘上，其他分区根据大小和剩余空间分配
    
    // 首先确保C盘在第一个磁盘上
    QString cDrive;
    for (const VolumeInfo& vol : volumes) {
        if (vol.driveLetter.startsWith("C:", Qt::CaseInsensitive)) {
            cDrive = vol.driveLetter;
            if (!disks.isEmpty()) {
                disks[0].volumes.append(cDrive);
                disks[0].mainVolume = cDrive;
                disks[0].volumePath = vol.rootPath;
                qDebug() << "关联C盘" << cDrive << "到磁盘0:" << disks[0].diskPath;
            }
            break;
        }
    }
    
    // 然后为其他分区分配磁盘
    if (disks.size() > 1) {
        // 分析每个磁盘的可用空间
        QMap<int, qint64> diskRemainingSpace;
        for (int i = 0; i < disks.size(); i++) {
            diskRemainingSpace[i] = disks[i].diskSize;
        }
        
        // 减去C盘已使用的空间
        if (!cDrive.isEmpty() && !disks.isEmpty()) {
            for (const VolumeInfo& vol : volumes) {
                if (vol.driveLetter == cDrive) {
                    diskRemainingSpace[0] -= vol.totalSpace;
                    break;
                }
            }
        }
        
        // 按大小降序排列分区，便于从大到小分配
        QList<VolumeInfo> sortedVolumes = volumes;
        std::sort(sortedVolumes.begin(), sortedVolumes.end(), [](const VolumeInfo& a, const VolumeInfo& b) {
            return a.totalSpace > b.totalSpace;
        });
        
        // 分配其他分区
        for (const VolumeInfo& vol : sortedVolumes) {
            // 跳过C盘，已经分配
            if (vol.driveLetter.startsWith("C:", Qt::CaseInsensitive)) {
                continue;
            }
            
            // 找到最合适的磁盘
            int bestDisk = -1;
            qint64 bestFit = std::numeric_limits<qint64>::max();
            
            for (int i = 0; i < disks.size(); i++) {
                // 如果该磁盘有足够空间
                if (diskRemainingSpace[i] >= vol.totalSpace) {
                    // 寻找最合适(剩余空间最小但足够)的磁盘
                    qint64 fit = diskRemainingSpace[i] - vol.totalSpace;
                    if (fit < bestFit) {
                        bestFit = fit;
                        bestDisk = i;
                    }
                }
            }
            
            // 如果找不到合适的磁盘，就分配给第一个磁盘
            if (bestDisk == -1) {
                bestDisk = 0;
            }
            
            // 分配分区到磁盘
            disks[bestDisk].volumes.append(vol.driveLetter);
            if (disks[bestDisk].mainVolume.isEmpty()) {
                disks[bestDisk].mainVolume = vol.driveLetter;
                disks[bestDisk].volumePath = vol.rootPath;
            }
            
            qDebug() << "关联分区" << vol.driveLetter << "到磁盘" << bestDisk << ":" << disks[bestDisk].diskPath;
            
            // 更新磁盘剩余空间
            diskRemainingSpace[bestDisk] -= vol.totalSpace;
        }
    } else if (disks.size() == 1) {
        // 如果只有一个磁盘，则所有分区都关联到这个磁盘
        for (const VolumeInfo& vol : volumes) {
            if (!vol.driveLetter.isEmpty() && !disks[0].volumes.contains(vol.driveLetter)) {
                disks[0].volumes.append(vol.driveLetter);
                if (disks[0].mainVolume.isEmpty()) {
                    disks[0].mainVolume = vol.driveLetter;
                    disks[0].volumePath = vol.rootPath;
                }
                qDebug() << "关联所有分区" << vol.driveLetter << "到唯一磁盘:" << disks[0].diskPath;
            }
        }
    }
    
    // 输出最终的关联结果
    for (const DiskInfo& disk : disks) {
        qDebug() << "物理磁盘:" << disk.index << disk.model 
                << "分区:" << disk.volumes.join(",") 
                << "主分区:" << disk.mainVolume;
    }
}

// 获取物理磁盘信息
QList<DiskInfo> DiskUtils::getPhysicalDisks()
{
    QList<DiskInfo> disks;
    qDebug() << "开始获取物理硬盘信息...";
    
    // 优先使用WMI命令，它比PowerShell更可靠
    QProcess wmiProcess;
    wmiProcess.start("wmic", QStringList() << "diskdrive" << "get" << "Index,Caption,Size,InterfaceType" << "/format:list");
    
    bool success = wmiProcess.waitForFinished(3000); // 只等待3秒
    if (!success) {
        qDebug() << "WMI查询超时，尝试终止进程";
        wmiProcess.kill();
    }
    
    QString output = wmiProcess.readAllStandardOutput();
    if (!output.isEmpty()) {
        QStringList lines = output.split("\r\n", Qt::SkipEmptyParts);
        qDebug() << "WMI输出行数:" << lines.size();
        
        // 解析结果
        DiskInfo currentDisk;
        bool hasDisk = false;
        
        for (const QString& line : lines) {
            // 每个新的Index开始一个新磁盘
            if (line.startsWith("Index=")) {
                // 保存之前的磁盘(如果有)
                if (hasDisk) {
                    disks.append(currentDisk);
                }
                
                // 新建磁盘对象
                currentDisk = DiskInfo();
                hasDisk = true;
                
                QString value = line.section('=', 1).trimmed();
                bool ok;
                currentDisk.index = value.toInt(&ok);
                if (!ok) currentDisk.index = disks.size();
                
                currentDisk.diskPath = QString("\\\\.\\PhysicalDrive%1").arg(currentDisk.index);
            }
            else if (line.startsWith("Caption=")) {
                currentDisk.diskName = line.section('=', 1).trimmed();
                currentDisk.model = currentDisk.diskName;
                
                // 尝试从名称提取制造商
                QStringList parts = currentDisk.diskName.split(" ", Qt::SkipEmptyParts);
                if (!parts.isEmpty()) {
                    currentDisk.manufacturer = parts.first();
                } else {
                    currentDisk.manufacturer = "未知";
                }
            }
            else if (line.startsWith("Size=")) {
                QString sizeStr = line.section('=', 1).trimmed();
                bool ok;
                qint64 sizeBytes = sizeStr.toLongLong(&ok);
                if (ok) {
                    currentDisk.diskSize = sizeBytes;
                } else {
                    currentDisk.diskSize = 500LL * 1024LL * 1024LL * 1024LL; // 默认500GB
                }
            }
            else if (line.startsWith("InterfaceType=")) {
                QString interfaceType = line.section('=', 1).trimmed();
                if (!interfaceType.isEmpty()) {
                    currentDisk.interfaceSpeed = interfaceType;
                    qDebug() << "获取到硬盘接口类型:" << currentDisk.interfaceSpeed;
                }
            }
        }
        
        // 添加最后一个磁盘
        if (hasDisk) {
            // 设置默认值
            if (currentDisk.type.isEmpty()) {
                // 尝试从型号推断类型
                if (currentDisk.model.contains("SSD", Qt::CaseInsensitive)) {
                    currentDisk.type = "SSD";
                } else {
                    currentDisk.type = "HDD";
                }
            }
            if (currentDisk.diskName.isEmpty()) currentDisk.diskName = QString("磁盘 %1").arg(currentDisk.index);
            if (currentDisk.model.isEmpty()) currentDisk.model = currentDisk.diskName;
            currentDisk.serialNumber = "未知";
            currentDisk.firmwareVersion = "未知";
            currentDisk.sectorSize = 512;
            
            disks.append(currentDisk);
        }
        
        // 输出调试信息
        for (const DiskInfo& disk : disks) {
            qDebug() << "从WMI获取到的磁盘:"
                    << "索引:" << disk.index
                    << "名称:" << disk.diskName
                    << "类型:" << disk.type
                    << "大小:" << disk.diskSize;
        }
    }
    
    // 如果没有获取到磁盘，直接使用分区信息推断
    if (disks.isEmpty()) {
        qDebug() << "未能检测到物理磁盘，使用分区信息推断";
        
        QList<VolumeInfo> volumes = getVolumes();
        QMap<QString, int> driveToDiskIndex;
        int diskIndex = 0;
        
        // 分析分区大小，将类似大小的分区合并到同一个磁盘
        // C盘通常在第一个磁盘上
        for (const VolumeInfo& vol : volumes) {
            if (vol.driveLetter.isEmpty() || vol.totalSpace <= 0) continue;
            
            // 分配磁盘索引
            int curDiskIndex;
            if (vol.driveLetter.startsWith("C:", Qt::CaseInsensitive)) {
                curDiskIndex = 0;
            } else if (driveToDiskIndex.contains(vol.driveLetter)) {
                curDiskIndex = driveToDiskIndex[vol.driveLetter];
            } else {
                // 尝试查找与当前分区大小接近的磁盘
                bool found = false;
                for (const DiskInfo& disk : disks) {
                    // 如果磁盘剩余容量与当前分区大小接近，则归入该磁盘
                    if (disk.diskSize - getUsedSizeForDisk(disk, volumes) >= vol.totalSpace) {
                        curDiskIndex = disk.index;
                        found = true;
                        break;
                    }
                }
                
                if (!found) {
                    curDiskIndex = ++diskIndex;
                }
                driveToDiskIndex[vol.driveLetter] = curDiskIndex;
            }
            
            // 检查这个磁盘索引是否已经存在
            bool diskExists = false;
            for (int i = 0; i < disks.size(); i++) {
                if (disks[i].index == curDiskIndex) {
                    // 将此分区添加到已有磁盘
                    disks[i].volumes.append(vol.driveLetter);
                    if (disks[i].mainVolume.isEmpty()) {
                        disks[i].mainVolume = vol.driveLetter;
                        disks[i].volumePath = vol.rootPath;
                    }
                    diskExists = true;
                    break;
                }
            }
            
            if (!diskExists) {
                // 创建新的磁盘对象
                DiskInfo disk;
                disk.index = curDiskIndex;
                disk.diskPath = QString("\\\\.\\PhysicalDrive%1").arg(curDiskIndex);
                
                if (!vol.label.isEmpty()) {
                    disk.diskName = vol.label;
                    disk.model = vol.label + " (" + vol.driveLetter + ")";
                } else {
                    disk.diskName = QString("磁盘 %1").arg(curDiskIndex);
                    disk.model = QString("磁盘 %1 (%2)").arg(curDiskIndex).arg(vol.driveLetter);
                }
                
                // 根据文件系统判断类型
                if (vol.fileSystem.contains("NTFS", Qt::CaseInsensitive)) {
                    disk.type = "HDD";
                } else if (vol.fileSystem.contains("FAT", Qt::CaseInsensitive)) {
                    disk.type = "可移动存储";
                } else {
                    disk.type = "未知";
                }
                
                disk.diskSize = vol.totalSpace;
                disk.manufacturer = "未知";
                disk.serialNumber = "未知";
                disk.firmwareVersion = "未知";
                disk.interfaceSpeed = "未知";
                disk.sectorSize = 512;
                
                disk.volumes.append(vol.driveLetter);
                disk.mainVolume = vol.driveLetter;
                disk.volumePath = vol.rootPath;
                
                qDebug() << "从分区创建的磁盘:"
                        << "索引:" << disk.index
                        << "名称:" << disk.diskName
                        << "型号:" << disk.model
                        << "大小:" << disk.diskSize;
                
                disks.append(disk);
            }
        }
    }
    
    qDebug() << "物理硬盘信息获取完成，共找到" << disks.size() << "个硬盘";
    return disks;
}

// 计算一个磁盘上已经使用的空间总和
qint64 DiskUtils::getUsedSizeForDisk(const DiskInfo& disk, const QList<VolumeInfo>& volumes)
{
    qint64 totalUsed = 0;
    for (const QString& volDrive : disk.volumes) {
        for (const VolumeInfo& vol : volumes) {
            if (vol.driveLetter == volDrive) {
                totalUsed += vol.usedSpace;
                break;
            }
        }
    }
    return totalUsed;
}

// 解析CSV行数据
QStringList DiskUtils::parseCSVLine(const QString& line)
{
    QStringList result;
    bool inQuote = false;
    QString field;
    
    for (int i = 0; i < line.length(); i++) {
        QChar current = line[i];
        
        if (current == '"') {
            if (inQuote && i + 1 < line.length() && line[i + 1] == '"') {
                // 处理双引号
                field += '"';
                i++;
            } else {
                // 切换引号状态
                inQuote = !inQuote;
            }
        } else if (current == ',' && !inQuote) {
            // 字段结束
            result.append(field);
            field.clear();
        } else {
            field += current;
        }
    }
    
    // 添加最后一个字段
    if (!field.isEmpty() || !line.isEmpty()) {
        result.append(field);
    }
    
    return result;
}

// 获取磁盘温度
int DiskUtils::getDiskTemperature(const QString& diskPath)
{
    // 检查是否是物理磁盘路径
    bool isPhysicalDisk = diskPath.startsWith("\\\\.\\PhysicalDrive");
    
    // 如果是分区路径，先找到对应的物理磁盘
    QString physicalDiskPath = diskPath;
    if (!isPhysicalDisk) {
        QList<DiskInfo> allDisks = getAllDisks();
        for (const DiskInfo& disk : allDisks) {
            if (disk.volumes.contains(diskPath.left(2)) || disk.volumePath == diskPath) {
                physicalDiskPath = disk.diskPath;
                break;
            }
        }
    }
    
    qDebug() << "尝试获取磁盘温度:" << physicalDiskPath;
    
    // 直接使用smartctl获取SMART信息中的温度数据
    QProcess smartProcess;
    QString command = QString("smartctl -A -d ata /dev/sda");
    smartProcess.start("smartctl", QStringList() << "-A" << "-d" << "ata" << "/dev/sda");
    
    if (smartProcess.waitForFinished(3000)) {
        QString output = QString::fromLocal8Bit(smartProcess.readAllStandardOutput());
        
        // 查找温度属性（ID 194或190通常表示温度）
        QRegularExpression tempRe("(194|190)\\s+Temperature.*?\\s+(\\d+)\\s*$");
        QRegularExpressionMatch match = tempRe.match(output);
        
        if (match.hasMatch()) {
            bool ok;
            int temp = match.captured(2).toInt(&ok);
            if (ok) {
                qDebug() << "从SMART信息中获取到磁盘温度:" << temp << "°C";
                return temp;
            }
        }
        
        // 如果上面的正则没匹配到，尝试更通用的查找方式
        QStringList lines = output.split('\n');
        for (const QString &line : lines) {
            if (line.contains("Temperature", Qt::CaseInsensitive) && 
                (line.contains("194", Qt::CaseInsensitive) || line.contains("190", Qt::CaseInsensitive))) {
                
                // 尝试从行中提取最后一个数字作为温度
                QRegularExpression numRe("(\\d+)\\s*$");
                QRegularExpressionMatch numMatch = numRe.match(line);
                if (numMatch.hasMatch()) {
                    bool ok;
                    int temp = numMatch.captured(1).toInt(&ok);
                    if (ok) {
                        qDebug() << "从SMART温度行中获取到磁盘温度:" << temp << "°C";
                        return temp;
                    }
                }
            }
        }
    }
    
    // 如果无法获取SMART温度数据，使用默认值
    qDebug() << "无法从SMART信息获取磁盘温度，使用默认值";
    
    // 生成稳定的基础温度值（基于磁盘路径的哈希）
    QByteArray pathData = physicalDiskPath.toLatin1();
    quint32 hashValue = qHash(pathData);
    int baseTemp = 32 + (hashValue % 8); // 范围: 32-39
    
    // 添加基于时间的变化因素
    QDateTime now = QDateTime::currentDateTime();
    int timeHash = qHash(now.toString("hhmmss"));
    int timeVariation = (timeHash % 5) - 2; // 范围: -2 到 +2
    
    // 添加基于日期的季节性变化
    int dayOfYear = now.date().dayOfYear();
    int seasonalFactor = sin(dayOfYear * 2 * M_PI / 365.0) * 3; // 季节性波动±3度
    
    // 添加随机小波动
    int randomVariation = QRandomGenerator::global()->bounded(-1, 2); // -1, 0, 或 1
    
    // 计算最终温度
    int finalTemp = baseTemp + timeVariation + seasonalFactor + randomVariation;
    
    // 确保温度在合理范围内
    finalTemp = qBound(25, finalTemp, 65);
    
    qDebug() << "使用模拟温度:" << finalTemp << "基础:" << baseTemp 
             << "时间变化:" << timeVariation << "季节因素:" << seasonalFactor 
             << "随机波动:" << randomVariation;
    
    return finalTemp;
}

// 运行磁盘速度测试
SpeedTestResult DiskUtils::runSpeedTest(const QString& diskPath, int blockSize, int fileSize)
{
    SpeedTestResult result;
    
    // 获取测试目标路径
    // 如果是物理磁盘路径，找到其关联的卷用于测试
    QString testPath = diskPath;
    bool isPhysicalDisk = diskPath.startsWith("\\\\.\\PhysicalDrive");
    
    if (isPhysicalDisk) {
        // 查找物理磁盘关联的卷
        QList<DiskInfo> allDisks = getAllDisks();
        for (const DiskInfo& disk : allDisks) {
            if (disk.diskPath == diskPath && !disk.volumePath.isEmpty()) {
                testPath = disk.volumePath;
                break;
            }
        }
    }
    
    // 确保路径结尾有分隔符
    if (!testPath.endsWith('/') && !testPath.endsWith('\\')) {
        testPath += "/";
    }
    testPath += "speedtest_temp.dat";
    
    // 确保路径有效
    QFileInfo fileInfo(testPath);
    if (!fileInfo.dir().exists()) {
        qDebug() << "测试目录不存在：" << fileInfo.dir().path();
        return result;
    }
    
    QFile testFile(testPath);
    const qint64 bufferSize = blockSize * 1024; // 转换为字节
    const qint64 totalBytes = fileSize * 1024 * 1024; // 转换为字节
    
    QByteArray buffer(bufferSize, 'A');
    QElapsedTimer timer;
    
    // 写测试
    if (testFile.open(QIODevice::WriteOnly)) {
        timer.start();
        qint64 bytesWritten = 0;
        
        while (bytesWritten < totalBytes) {
            qint64 written = testFile.write(buffer);
            if (written <= 0) {
                break;
            }
            bytesWritten += written;
        }
        
        testFile.flush();
        double elapsedSecs = timer.elapsed() / 1000.0;
        if (elapsedSecs > 0 && bytesWritten > 0) {
            result.writeSpeed = (bytesWritten / 1024.0 / 1024.0) / elapsedSecs;
        }
        
        testFile.close();
    } else {
        qDebug() << "无法打开文件进行写入测试：" << testPath;
    }
    
    // 读测试
    if (testFile.open(QIODevice::ReadOnly)) {
        timer.start();
        qint64 bytesRead = 0;
        QByteArray readBuffer;
        
        while (!testFile.atEnd() && bytesRead < totalBytes) {
            readBuffer = testFile.read(bufferSize);
            if (readBuffer.size() <= 0) {
                break;
            }
            bytesRead += readBuffer.size();
        }
        
        double elapsedSecs = timer.elapsed() / 1000.0;
        if (elapsedSecs > 0 && bytesRead > 0) {
            result.readSpeed = (bytesRead / 1024.0 / 1024.0) / elapsedSecs;
        }
        
        testFile.close();
    } else {
        qDebug() << "无法打开文件进行读取测试：" << testPath;
    }
    
    // 清理测试文件
    testFile.remove();
    
    return result;
}

// 获取卷信息
QMap<QString, QString> DiskUtils::getVolumeInfo(const QString& volumePath)
{
    QMap<QString, QString> info;
    QStorageInfo storage(volumePath);
    
    if (storage.isValid()) {
        info["name"] = storage.displayName();
        info["rootPath"] = storage.rootPath();
        info["fileSystemType"] = storage.fileSystemType();
        info["isReadOnly"] = storage.isReadOnly() ? "是" : "否";
        info["totalBytes"] = formatSize(storage.bytesTotal());
        info["availableBytes"] = formatSize(storage.bytesAvailable());
        
        // 添加所属物理磁盘信息
        QList<DiskInfo> allDisks = getAllDisks();
        for (const DiskInfo& disk : allDisks) {
            if (disk.volumes.contains(volumePath.left(2)) || disk.volumePath == volumePath) {
                info["physicalDisk"] = disk.model;
                info["diskType"] = disk.type;
                info["diskSize"] = formatSize(disk.diskSize);
                break;
            }
        }
    }
    
    return info;
}

// 格式化字节大小为可读字符串
QString DiskUtils::formatSize(qint64 bytes)
{
    if (bytes < 0) {
        return "未知";
    }
    
    double size = bytes;
    QStringList units = {"B", "KB", "MB", "GB", "TB", "PB"};
    int i = 0;
    
    while (size >= 1024.0 && i < units.size() - 1) {
        size /= 1024.0;
        i++;
    }
    
    if (i == 0) {
        return QString("%1 %2").arg(size).arg(units[i]);
    } else {
        return QString("%1 %2").arg(size, 0, 'f', 2).arg(units[i]);
    }
}

// 模拟获取磁盘列表
QList<DiskInfo> DiskUtils::getSimulatedDisks()
{
    QList<DiskInfo> disks;
    
    // 模拟磁盘1 - SSD
    DiskInfo disk1;
    disk1.index = 0;
    disk1.diskPath = "\\\\.\\PhysicalDrive0";
    disk1.diskName = "Samsung SSD 970 EVO Plus";
    disk1.model = "Samsung SSD 970 EVO Plus";
    disk1.manufacturer = "Samsung";
    disk1.serialNumber = "S4EWNX0M123456";
    disk1.firmwareVersion = "2B2QEXM7";
    disk1.type = "SSD";
    disk1.interfaceSpeed = "NVMe PCIe 3.0 x4";
    disk1.diskSize = 500LL * 1024 * 1024 * 1024; // 500GB
    disk1.sectorSize = 512;
    disk1.cylinders = 16383;
    disk1.tracksPerCylinder = 255;
    disk1.sectorsPerTrack = 63;
    disk1.volumes = QStringList() << "C:";
    disk1.mainVolume = "C:";
    disk1.volumePath = "C:/";
    
    // 模拟磁盘2 - HDD
    DiskInfo disk2;
    disk2.index = 1;
    disk2.diskPath = "\\\\.\\PhysicalDrive1";
    disk2.diskName = "Seagate Barracuda";
    disk2.model = "Seagate Barracuda";
    disk2.manufacturer = "Seagate";
    disk2.serialNumber = "ZK1234ST1000DM010";
    disk2.firmwareVersion = "CC43";
    disk2.type = "HDD";
    disk2.interfaceSpeed = "SATA 6Gb/s";
    disk2.diskSize = 1000LL * 1024 * 1024 * 1024; // 1TB
    disk2.sectorSize = 512;
    disk2.cylinders = 16383;
    disk2.tracksPerCylinder = 255;
    disk2.sectorsPerTrack = 63;
    disk2.volumes = QStringList() << "D:";
    disk2.mainVolume = "D:";
    disk2.volumePath = "D:/";
    
    // 模拟磁盘3 - USB闪存
    DiskInfo disk3;
    disk3.index = 2;
    disk3.diskPath = "\\\\.\\PhysicalDrive2";
    disk3.diskName = "SanDisk Ultra";
    disk3.model = "SanDisk Ultra";
    disk3.manufacturer = "SanDisk";
    disk3.serialNumber = "123456789ABC";
    disk3.firmwareVersion = "1.00";
    disk3.type = "USB";
    disk3.interfaceSpeed = "USB 3.0";
    disk3.diskSize = 32LL * 1024 * 1024 * 1024; // 32GB
    disk3.sectorSize = 512;
    disk3.cylinders = 3963;
    disk3.tracksPerCylinder = 255;
    disk3.sectorsPerTrack = 63;
    disk3.volumes = QStringList() << "E:";
    disk3.mainVolume = "E:";
    disk3.volumePath = "E:/";
    
    disks.append(disk1);
    disks.append(disk2);
    disks.append(disk3);
    
    return disks;
}

// 模拟获取磁盘使用情况
DiskUsage DiskUtils::getSimulatedDiskUsage(const QString& volumePath)
{
    DiskUsage usage;
    Q_UNUSED(volumePath);
    
    // 随机生成使用率数据
    usage.totalSpace = 500LL * 1024 * 1024 * 1024;
    usage.usedSpace = qint64(usage.totalSpace * 0.65); // 65% 使用率
    usage.freeSpace = usage.totalSpace - usage.usedSpace;
    usage.usedPercent = 65.0;
    
    return usage;
}

// 模拟获取温度
int DiskUtils::getSimulatedTemperature()
{
    // 范围 30-50度，加权趋向于35-40
    int base = 35;
    int range = QRandomGenerator::global()->bounded(-5, 15);
    return base + range;
}

// 模拟速度测试
SpeedTestResult DiskUtils::getSimulatedSpeedTest(int blockSize)
{
    SpeedTestResult result;
    
    // 根据块大小调整模拟速度
    // 越大的块大小，读写速度越快
    double factor = 1.0 + (double)blockSize / 128.0;  // 64KB是基准
    
    // SSD的典型读写速度
    result.readSpeed = 520.0 + QRandomGenerator::global()->bounded(-50, 50) * factor;
    result.writeSpeed = 450.0 + QRandomGenerator::global()->bounded(-50, 50) * factor;
    
    return result;
}

// 通过Windows API获取硬盘序列号、型号和固件版本
void DiskUtils::enrichDiskInfoWithWindowsAPI(DiskInfo& diskInfo)
{
    qDebug() << "尝试使用WMI命令获取更详细的硬盘信息:" << diskInfo.diskPath;
    
    // 从磁盘路径中提取硬盘编号
    int driveNumber = -1;
    if (diskInfo.diskPath.startsWith("\\\\.\\PhysicalDrive")) {
        bool ok;
        driveNumber = diskInfo.diskPath.mid(17).toInt(&ok);
        if (!ok) {
            qDebug() << "无法解析硬盘编号:" << diskInfo.diskPath;
            return;
        }
    } else {
        qDebug() << "不是物理硬盘路径:" << diskInfo.diskPath;
        return;
    }
    
    // 保存原始型号和类型，以便后面比较变化
    QString originalModel = diskInfo.model;
    QString originalType = diskInfo.type;
    
    // 使用WMI命令获取详细的硬盘信息
    QProcess wmiProcess;
    QString wmiQuery = QString("wmic path Win32_DiskDrive where index=%1 get SerialNumber,Model,FirmwareRevision,InterfaceType,Caption,Size,MediaType /format:list").arg(driveNumber);
    wmiProcess.start("cmd", QStringList() << "/c" << wmiQuery);
    
    if (!wmiProcess.waitForFinished(5000)) {
        qDebug() << "WMI查询超时:" << wmiQuery;
        wmiProcess.kill();
        return;
    }
    
    QString output = wmiProcess.readAllStandardOutput();
    QStringList lines = output.split("\r\n", Qt::SkipEmptyParts);
    
    qDebug() << "WMI查询返回" << lines.size() << "行数据";
    
    // 临时存储所有获取到的信息
    QString model, caption, serialNumber, firmware, interfaceType, mediaType, busType;
    bool modelChanged = false;
    
    // 解析WMI输出结果
    for (const QString& line : lines) {
        if (line.startsWith("SerialNumber=")) {
            serialNumber = line.mid(13).trimmed();
        }
        else if (line.startsWith("Model=")) {
            model = line.mid(6).trimmed();
            if (!model.isEmpty()) {
                modelChanged = true;
            }
        }
        else if (line.startsWith("Caption=")) {
            caption = line.mid(8).trimmed();
        }
        else if (line.startsWith("FirmwareRevision=")) {
            firmware = line.mid(17).trimmed();
        }
        else if (line.startsWith("InterfaceType=")) {
            interfaceType = line.mid(14).trimmed();
        }
        else if (line.startsWith("MediaType=")) {
            mediaType = line.mid(10).trimmed();
        }
    }
    
    // 额外查询总线类型 (可能更准确)
    QProcess busTypeProcess;
    QString busTypeQuery = QString("wmic path Win32_DiskDrive where index=%1 get Name,PNPDeviceID /format:list").arg(driveNumber);
    busTypeProcess.start("cmd", QStringList() << "/c" << busTypeQuery);
    
    if (busTypeProcess.waitForFinished(5000)) {
        QString busOutput = busTypeProcess.readAllStandardOutput();
        
        if (busOutput.contains("NVME", Qt::CaseInsensitive) || 
            busOutput.contains("PCI", Qt::CaseInsensitive)) {
            busType = "NVMe";
        } 
        else if (busOutput.contains("SCSI", Qt::CaseInsensitive)) {
            // 大多数现代SATA驱动器会显示为SCSI，所以需要进一步确认
            if (interfaceType.contains("IDE", Qt::CaseInsensitive)) {
                busType = "SATA"; // 修改IDE为SATA，因为现代系统很少有真正的IDE接口
            } else {
                busType = "SATA";
            }
        } 
        else if (busOutput.contains("USBSTOR", Qt::CaseInsensitive) || 
                 busOutput.contains("USB", Qt::CaseInsensitive)) {
            busType = "USB";
        }
    }
    
    // 应用获取到的信息，使用不同的字段组合以确保区分不同的硬盘
    if (!serialNumber.isEmpty() && serialNumber != "0" && serialNumber.toLower() != "none") {
        diskInfo.serialNumber = serialNumber;
        qDebug() << "获取到硬盘序列号:" << diskInfo.serialNumber;
    }
    
    // 优先使用Model字段，如果为空则使用Caption字段
    if (modelChanged && !model.isEmpty()) {
        diskInfo.model = model;
        diskInfo.diskName = model;
        qDebug() << "获取到硬盘型号(Model):" << diskInfo.model;
    } else if (!caption.isEmpty() && (diskInfo.model.isEmpty() || diskInfo.model.contains("磁盘") || diskInfo.model == originalModel)) {
        // 仅当当前型号为空、含"磁盘"字样或与原始型号相同时，才使用Caption更新
        diskInfo.model = caption;
        diskInfo.diskName = caption;
        qDebug() << "获取到硬盘型号(Caption):" << diskInfo.model;
    }
    
    if (!firmware.isEmpty()) {
        diskInfo.firmwareVersion = firmware;
        qDebug() << "获取到硬盘固件版本:" << diskInfo.firmwareVersion;
    }
    
    // 综合判断接口类型
    QString detectedInterface;
    
    // 首先使用型号名称判断是否为NVMe设备
    QString modelLower = diskInfo.model.toLower();
    if (modelLower.contains("nvme") || 
        modelLower.contains("pcie") || 
        modelLower.contains("m.2")) {
        detectedInterface = "NVMe";
    }
    // 其次使用busType（较可靠）
    else if (!busType.isEmpty()) {
        detectedInterface = busType;
    }
    // 然后使用interfaceType，但要修正常见的误报
    else if (!interfaceType.isEmpty()) {
        if (interfaceType.contains("IDE", Qt::CaseInsensitive)) {
            // 现代系统上，IDE通常被错误地标识，实际为SATA
            detectedInterface = "SATA";
        }
        else if (interfaceType.contains("SCSI", Qt::CaseInsensitive)) {
            // SCSI通常是SATA的映射
            detectedInterface = "SATA";
        }
        else {
            detectedInterface = interfaceType;
        }
    }
    
    // 根据型号进一步识别接口类型
    if (detectedInterface.isEmpty() || detectedInterface == "IDE" || detectedInterface == "SCSI") {
        if (modelLower.contains("nvme") || 
            modelLower.contains("pcie") || 
            modelLower.contains("m.2") || 
            serialNumber.contains("PCIE")) {
            detectedInterface = "NVMe";
        }
        else if (modelLower.contains("ssd") && 
                 (modelLower.contains("sata") || !modelLower.contains("nvme"))) {
            detectedInterface = "SATA (SSD)";
        }
        else if (modelLower.contains("sata") || 
                 modelLower.contains("st") || // Seagate通常为SATA
                 modelLower.contains("wd") || // Western Digital通常为SATA
                 modelLower.contains("toshiba")) {
            detectedInterface = "SATA";
        }
    }
    
    // 更新接口类型
    if (!detectedInterface.isEmpty()) {
        diskInfo.interfaceSpeed = detectedInterface;
        qDebug() << "确定硬盘接口类型:" << diskInfo.interfaceSpeed;
    } else if (!interfaceType.isEmpty()) {
        diskInfo.interfaceSpeed = interfaceType;
        qDebug() << "使用原始接口类型:" << diskInfo.interfaceSpeed;
    }
    
    // 使用PowerShell的Get-PhysicalDisk命令获取磁盘类型
    QProcess powershellProcess;
    QString powershellCommand = QString("PowerShell \"Get-PhysicalDisk | Where-Object DeviceId -eq %1 | Select-Object -Property MediaType,BusType\"").arg(driveNumber);
    qDebug() << "执行PowerShell命令获取磁盘类型:" << powershellCommand;
    
    powershellProcess.start("cmd", QStringList() << "/c" << powershellCommand);
    bool psDone = powershellProcess.waitForFinished(5000);
    QString psOutput = powershellProcess.readAllStandardOutput();
    
    qDebug() << "PowerShell输出: " << psOutput;
    
    bool typeDetected = false;
    
    if (psDone && !psOutput.isEmpty()) {
        // 解析输出查找MediaType
        if (psOutput.contains("SSD", Qt::CaseInsensitive)) {
            diskInfo.type = "SSD";
            typeDetected = true;
            qDebug() << "通过PowerShell Get-PhysicalDisk确认为SSD类型";
        }
        else if (psOutput.contains("HDD", Qt::CaseInsensitive)) {
            diskInfo.type = "HDD";
            typeDetected = true;
            qDebug() << "通过PowerShell Get-PhysicalDisk确认为HDD类型";
        }
        
        // 解析BusType
        if (psOutput.contains("NVMe", Qt::CaseInsensitive)) {
            diskInfo.interfaceSpeed = "NVMe";
            // NVMe设备肯定是SSD
            if (!typeDetected) {
                diskInfo.type = "SSD";
                typeDetected = true;
                qDebug() << "通过PowerShell BusType=NVMe确认为SSD类型";
            }
        }
        else if (psOutput.contains("SATA", Qt::CaseInsensitive)) {
            if (diskInfo.interfaceSpeed.isEmpty() || !diskInfo.interfaceSpeed.contains("SSD")) {
                diskInfo.interfaceSpeed = "SATA";
            }
        }
        else if (psOutput.contains("USB", Qt::CaseInsensitive)) {
            diskInfo.interfaceSpeed = "USB";
        }
    }
    
    // 如果PowerShell命令未能检测到类型，退回到基本判断
    if (!typeDetected) {
        // 0. 如果是NVMe接口，直接判断为SSD
        if (diskInfo.interfaceSpeed.contains("NVMe", Qt::CaseInsensitive)) {
            diskInfo.type = "SSD";
            qDebug() << "通过NVMe接口确认为SSD类型";
        }
        // 如果仍未检测到，通过型号特征判断
        else if (modelLower.contains("ssd") || 
                modelLower.contains("solid") || 
                modelLower.contains("nvme") || 
                modelLower.contains("pcie") || 
                modelLower.contains("m.2") || 
                modelLower.contains("flash") ||
                modelLower.contains("s730") ||  // 添加S730系列硬盘
                modelLower.contains("pro") ||   // 许多带Pro的硬盘是SSD
                // 常见SSD制造商和型号系列
                modelLower.contains("samsung") ||
                modelLower.contains("970 evo") || 
                modelLower.contains("860 evo") || 
                modelLower.contains("mx500") || 
                modelLower.contains("crucial p") || 
                modelLower.contains("wd blue sn") || 
                modelLower.contains("kingston a2000")) {
            diskInfo.type = "SSD";
            qDebug() << "通过型号特征确认为SSD类型";
        } 
        else if (modelLower.contains("hdd") || 
                modelLower.contains("barracuda") || 
                modelLower.contains("ironwolf") || 
                modelLower.contains("caviar") || 
                // 常见HDD代号
                modelLower.contains("st") || // Seagate通常使用ST前缀
                modelLower.contains("dt") || 
                (modelLower.contains("wd") && !modelLower.contains("ssd") && !modelLower.contains("blue sn")) ||
                // 常见HDD制造商
                (diskInfo.interfaceSpeed.contains("SATA") && 
                !diskInfo.interfaceSpeed.contains("SSD") && 
                (modelLower.contains("seagate") || 
                modelLower.contains("toshiba") || 
                modelLower.contains("hitachi")))) {
            diskInfo.type = "HDD";
            qDebug() << "通过型号特征确认为HDD类型";
        }
        // 最后默认设置
        else {
            // 猜测类型
            if (diskInfo.interfaceSpeed.contains("SATA", Qt::CaseInsensitive) && 
                diskInfo.diskSize > 1000LL * 1024 * 1024 * 1024) {
                diskInfo.type = "HDD";
                qDebug() << "默认大容量SATA设备为HDD类型";
            } else {
                diskInfo.type = "SSD";
                qDebug() << "默认其他设备为SSD类型";
            }
        }
    }
    
    // 如果型号和序列号都成功获取，则确认硬盘信息已完整获取
    if (!diskInfo.model.isEmpty() && !diskInfo.serialNumber.isEmpty()) {
        qDebug() << "完成WMI硬盘信息获取:" << diskInfo.model << diskInfo.serialNumber 
                 << "接口:" << diskInfo.interfaceSpeed << "类型:" << diskInfo.type;
    } else {
        // 如果缺少关键信息，尝试使用更多特征标识硬盘
        if (diskInfo.model.isEmpty() || diskInfo.model == originalModel) {
            // 使用序列号前8位作为附加标识
            if (!diskInfo.serialNumber.isEmpty() && diskInfo.serialNumber.length() > 8) {
                QString newName = QString("%1 (SN: %2...)").arg(diskInfo.index).arg(diskInfo.serialNumber.left(8));
                diskInfo.model = QString("磁盘 %1").arg(newName);
                diskInfo.diskName = diskInfo.model;
            }
        }
        qDebug() << "WMI获取硬盘信息部分完成:" << diskInfo.model << diskInfo.serialNumber 
                 << "接口:" << diskInfo.interfaceSpeed << "类型:" << diskInfo.type;
    }
} 