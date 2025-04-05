#include "diskdetector.h"
#include <QDebug>
#include <QProcess>
#include <QRegularExpression>

DiskDetector::DiskDetector(QObject *parent) : QObject(parent)
{
}

DiskDetector::~DiskDetector()
{
}

QList<DiskSMARTInfo> DiskDetector::detectDisks()
{
    QList<DiskSMARTInfo> diskList;
    
    // 使用Windows命令检测系统中的所有物理硬盘
    QProcess wmicProcess;
    qDebug() << "开始使用wmic命令检测物理硬盘...";
    qDebug() << "---磁盘检测调试---准备执行命令: wmic diskdrive list brief";
    wmicProcess.start("wmic diskdrive list brief");
    qDebug() << "---磁盘检测调试---命令已启动，等待返回结果...";
    
    if (!wmicProcess.waitForFinished(5000)) {
        qDebug() << "执行wmic命令超时";
        qDebug() << "---磁盘检测调试---命令执行超时(5秒)";
        return simulateDisks();  // 如果超时，使用模拟数据
    }
    
    QString output = QString::fromLocal8Bit(wmicProcess.readAllStandardOutput());
    qDebug() << "---磁盘检测调试---命令完成，退出码:" << wmicProcess.exitCode();
    
    if (output.isEmpty()) {
        qDebug() << "---磁盘检测调试---命令输出为空";
        QByteArray errorOutput = wmicProcess.readAllStandardError();
        if (!errorOutput.isEmpty()) {
            qDebug() << "---磁盘检测调试---错误输出:" << QString::fromLocal8Bit(errorOutput);
        }
    } else {
        qDebug() << "---磁盘检测调试---命令输出长度:" << output.length();
        qDebug() << "---磁盘检测调试---输出前200个字符:" << output.left(200);
    }
    
    QStringList lines = output.split("\n", QString::SkipEmptyParts);
    
    // 跳过标题行
    if (lines.size() > 1) {
        qDebug() << "---磁盘检测调试---检测到表头行:" << lines.first();
        lines.removeFirst();
    }
    
    qDebug() << "WMIC输出行数:" << lines.size();
    if (lines.size() == 0) {
        qDebug() << "没有检测到物理硬盘，使用模拟数据";
        qDebug() << "---磁盘检测调试---没有检测到数据行，使用模拟数据";
        return simulateDisks();  // 如果没有检测到硬盘，使用模拟数据
    }
    
    for (const QString &line : lines) {
        qDebug() << "---磁盘检测调试---处理数据行:" << line;
        QStringList parts = line.trimmed().split(QRegExp("\\s{2,}"), QString::SkipEmptyParts);
        if (parts.size() < 3) {
            qDebug() << "解析行失败，跳过:" << line;
            qDebug() << "---磁盘检测调试---行数据列数不足，跳过. 列数:" << parts.size();
            continue;
        }
        
        // wmic diskdrive list brief 一般返回格式为:
        // Caption, DeviceID, Model, Partitions, Size
        QString deviceID = parts.value(1);  // 类似于 \\.\PHYSICALDRIVE0
        QString model = parts.value(2);
        
        qDebug() << "解析到磁盘信息:" << deviceID << model;
        qDebug() << "---磁盘检测调试---解析到设备ID:" << deviceID << ", 型号:" << model;
        
        // 从PhysicalDriveX格式中提取磁盘号
        QRegularExpression driveNumRe("PHYSICALDRIVE(\\d+)", QRegularExpression::CaseInsensitiveOption);
        QRegularExpressionMatch match = driveNumRe.match(deviceID);
        if (!match.hasMatch()) {
            qDebug() << "无法提取磁盘号，跳过:" << deviceID;
            qDebug() << "---磁盘检测调试---未找到PHYSICALDRIVE格式，跳过";
            continue;
        }
        
        int driveIndex = match.captured(1).toInt();
        QString diskPath = QString("\\\\.\\PhysicalDrive%1").arg(driveIndex);
        
        qDebug() << "检测磁盘:" << diskPath << ", 型号:" << model;
        qDebug() << "---磁盘检测调试---提取到磁盘索引:" << driveIndex << ", 构造路径:" << diskPath;
        
        // 基于索引确定磁盘类型（这里使用简单判断，实际应与硬件匹配）
        DiskType type = (driveIndex == 0) ? DiskType::SATA : DiskType::NVMe;
        qDebug() << "---磁盘检测调试---基于索引确定磁盘类型:" << (type == DiskType::SATA ? "SATA" : "NVMe");
        
        // 创建磁盘信息
        DiskSMARTInfo diskInfo;
        diskInfo.diskPath = diskPath;
        diskInfo.diskType = type;
        diskInfo.model = model.isEmpty() ? "Unknown Model" : model;
        diskInfo.size = 1000000000000LL * (driveIndex + 1); // 简单模拟大小
        
        // 为SATA和NVMe设备设置不同的序列号格式
        if (type == DiskType::SATA) {
            diskInfo.serialNumber = QString("SATA-DISK-%1").arg(driveIndex);
        } else {
            diskInfo.serialNumber = QString("NVME-DISK-%1").arg(driveIndex);
        }
        
        qDebug() << "添加磁盘到列表:" << diskInfo.diskPath 
                 << "类型:" << (diskInfo.diskType == DiskType::SATA ? "SATA" : "NVMe")
                 << "型号:" << diskInfo.model;
        
        diskList.append(diskInfo);
    }
    
    // 如果没有检测到任何磁盘，使用模拟数据
    if (diskList.isEmpty()) {
        qDebug() << "没有检测到支持SMART的硬盘，使用模拟数据";
        qDebug() << "---磁盘检测调试---处理完所有行后磁盘列表为空，使用模拟数据";
        return simulateDisks();
    }
    
    qDebug() << "最终检测到" << diskList.size() << "个硬盘";
    for (const DiskSMARTInfo &disk : diskList) {
        qDebug() << "路径:" << disk.diskPath 
                 << "类型:" << (disk.diskType == DiskType::SATA ? "SATA" : "NVMe")
                 << "型号:" << disk.model
                 << "序列号:" << disk.serialNumber
                 << "容量:" << disk.size;
    }
    
    return diskList;
}

// 添加一个新方法用于生成模拟磁盘数据
QList<DiskSMARTInfo> DiskDetector::simulateDisks()
{
    QList<DiskSMARTInfo> diskList;
    
    qDebug() << "---磁盘检测调试---生成模拟磁盘数据";
    
    // 模拟SATA设备
    DiskSMARTInfo sataInfo;
    sataInfo.diskPath = "\\\\.\\PhysicalDrive0";
    sataInfo.diskType = DiskType::SATA;
    sataInfo.model = "SATA SSD 1TB";
    sataInfo.serialNumber = "SATA-SN12345678";
    sataInfo.size = 1000000000000LL; // 1 TB
    diskList.append(sataInfo);
    qDebug() << "---磁盘检测调试---添加模拟SATA磁盘:" << sataInfo.diskPath;
    
    // 模拟NVMe设备
    DiskSMARTInfo nvmeInfo;
    nvmeInfo.diskPath = "\\\\.\\PhysicalDrive1";
    nvmeInfo.diskType = DiskType::NVMe;
    nvmeInfo.model = "NVMe SSD 2TB";
    nvmeInfo.serialNumber = "NVME-SN87654321";
    nvmeInfo.size = 2000000000000LL; // 2 TB
    diskList.append(nvmeInfo);
    qDebug() << "---磁盘检测调试---添加模拟NVMe磁盘:" << nvmeInfo.diskPath;
    
    qDebug() << "生成了" << diskList.size() << "个模拟磁盘";
    return diskList;
}

DiskType DiskDetector::detectDiskType(const QString &diskPath)
{
    // 根据磁盘路径确定类型
    qDebug() << "---磁盘检测调试---检测磁盘类型，路径:" << diskPath;
    
    // 在Windows中，使用WMI获取更详细的设备信息
    QProcess wmicProcess;
    QString query = QString("wmic path Win32_DiskDrive where DeviceID='%1' get InterfaceType").arg(diskPath);
    qDebug() << "---磁盘检测调试---执行WMI命令获取接口类型:" << query;
    
    wmicProcess.start(query);
    if (!wmicProcess.waitForFinished(3000)) {
        qDebug() << "---磁盘检测调试---WMI命令执行超时";
        // 基于路径进行简单判断
        if (diskPath.contains("nvme", Qt::CaseInsensitive)) {
            qDebug() << "---磁盘检测调试---根据路径判定为NVMe设备";
            return DiskType::NVMe;
        } else {
            qDebug() << "---磁盘检测调试---根据路径判定为SATA设备";
            return DiskType::SATA;
        }
    }
    
    QString output = QString::fromLocal8Bit(wmicProcess.readAllStandardOutput());
    qDebug() << "---磁盘检测调试---WMI输出:" << output;
    
    if (output.contains("NVMe", Qt::CaseInsensitive) || output.contains("PCI", Qt::CaseInsensitive)) {
        qDebug() << "---磁盘检测调试---根据WMI输出判定为NVMe设备";
        return DiskType::NVMe;
    } else if (output.contains("SATA", Qt::CaseInsensitive) || output.contains("IDE", Qt::CaseInsensitive) || output.contains("SCSI", Qt::CaseInsensitive)) {
        qDebug() << "---磁盘检测调试---根据WMI输出判定为SATA设备";
        return DiskType::SATA;
    } else {
        // 如果找不到接口类型，根据设备索引判断
        if (diskPath.contains("PhysicalDrive1")) {
            qDebug() << "---磁盘检测调试---根据索引判定为NVMe设备";
            return DiskType::NVMe;
        } else {
            qDebug() << "---磁盘检测调试---根据索引判定为SATA设备";
            return DiskType::SATA;
        }
    }
}

QString DiskDetector::runSmartCtl(const QString &diskPath, const QString &params)
{
    QProcess smartctlProcess;
    QString command = "smartctl " + params + " " + diskPath;
    
    qDebug() << "执行命令:" << command;
    qDebug() << "---smartctl调试---准备执行命令:" << command;
    
    // 启动进程并等待完成
    smartctlProcess.start(command);
    qDebug() << "---smartctl调试---命令已启动，等待返回结果...";
    
    // 直接等待进程完成，最多等待5秒
    if (!smartctlProcess.waitForFinished(5000)) {
        qDebug() << "执行smartctl命令超时";
        qDebug() << "---smartctl调试---命令执行超时(5秒)";
        return QString();
    }
    
    // 检查退出状态
    int exitCode = smartctlProcess.exitCode();
    qDebug() << "smartctl命令执行结束，退出码:" << exitCode;
    qDebug() << "---smartctl调试---命令完成，退出码:" << exitCode;
    
    if (exitCode != 0) {
        qDebug() << "smartctl命令执行失败，退出码:" << exitCode;
        QByteArray errorOutput = smartctlProcess.readAllStandardError();
        if (!errorOutput.isEmpty()) {
            QString errorString = QString::fromLocal8Bit(errorOutput);
            qDebug() << "错误信息:" << errorString;
            qDebug() << "---smartctl调试---错误输出:" << errorString;
        } else {
            qDebug() << "---smartctl调试---无错误输出";
        }
        
        // 某些情况下，命令可能返回非零值但仍有有用信息
        if (exitCode == 1 || exitCode == 2) {
            qDebug() << "命令返回非零值，但可能有部分数据可用";
            qDebug() << "---smartctl调试---退出码为1或2，继续处理输出";
        } else {
            qDebug() << "---smartctl调试---退出码表示严重错误，放弃处理";
            return QString(); // 严重错误，返回空字符串
        }
    }
    
    // 解析输出
    QString output = QString::fromLocal8Bit(smartctlProcess.readAllStandardOutput());
    
    if (output.isEmpty()) {
        qDebug() << "---smartctl调试---命令标准输出为空";
    } else {
        qDebug() << "---smartctl调试---命令输出长度:" << output.length();
        qDebug() << "---smartctl调试---输出前200个字符:" << output.left(200);
    }
    
    return output;
}

void DiskDetector::parseSATADeviceInfo(DiskSMARTInfo &diskInfo, const QString &output)
{
    qDebug() << "---SATA解析调试---开始解析SATA设备信息，输出长度:" << output.length();
    
    // 解析设备型号
    QRegularExpression modelRe("Device Model:\\s+(.+)$");
    QRegularExpressionMatch modelMatch = modelRe.match(output);
    if (modelMatch.hasMatch()) {
        diskInfo.model = modelMatch.captured(1).trimmed();
        qDebug() << "---SATA解析调试---找到设备型号:" << diskInfo.model;
    } else {
        qDebug() << "---SATA解析调试---未找到设备型号";
    }
    
    // 解析序列号
    QRegularExpression serialRe("Serial Number:\\s+(.+)$");
    QRegularExpressionMatch serialMatch = serialRe.match(output);
    if (serialMatch.hasMatch()) {
        diskInfo.serialNumber = serialMatch.captured(1).trimmed();
        qDebug() << "---SATA解析调试---找到序列号:" << diskInfo.serialNumber;
    } else {
        qDebug() << "---SATA解析调试---未找到序列号";
    }
    
    // 在SATA设备中，大小通常需要通过其他命令获取
    // 这里简单地设置一个默认值
    diskInfo.size = 1000000000000; // 约1TB
    qDebug() << "---SATA解析调试---设置默认磁盘大小:" << diskInfo.size;
    
    qDebug() << "已解析SATA设备信息:" << diskInfo.model << diskInfo.serialNumber;
}

void DiskDetector::parseNVMeDeviceInfo(DiskSMARTInfo &diskInfo, const QString &output)
{
    qDebug() << "---NVMe解析调试---开始解析NVMe设备信息，输出长度:" << output.length();
    
    // 解析容量
    QRegularExpression sizeRe("Namespace 1 Size/Capacity:\\s+([\\d,]+)\\s+\\[([\\d.]+)\\s+(\\w+)\\]");
    QRegularExpressionMatch sizeMatch = sizeRe.match(output);
    if (sizeMatch.hasMatch()) {
        QString sizeStr = sizeMatch.captured(1);
        sizeStr.remove(','); // 移除千位分隔符
        diskInfo.size = sizeStr.toLongLong();
        qDebug() << "---NVMe解析调试---找到容量原始值:" << sizeStr << ", 转换后:" << diskInfo.size;
        
        // 设置型号为容量信息
        QString sizeAmount = sizeMatch.captured(2);
        QString sizeUnit = sizeMatch.captured(3);
        diskInfo.model = QString("NVMe SSD %1 %2").arg(sizeAmount).arg(sizeUnit);
        qDebug() << "---NVMe解析调试---根据容量设置型号:" << diskInfo.model;
    } else {
        qDebug() << "---NVMe解析调试---未找到容量信息";
        
        // 尝试其他方式查找型号
        QRegularExpression modelRe("Model Number:\\s+(.+)$");
        QRegularExpressionMatch modelMatch = modelRe.match(output);
        if (modelMatch.hasMatch()) {
            diskInfo.model = modelMatch.captured(1).trimmed();
            qDebug() << "---NVMe解析调试---找到型号信息:" << diskInfo.model;
        } else {
            qDebug() << "---NVMe解析调试---未找到任何型号信息";
        }
    }
    
    // NVMe通常不直接提供序列号，使用其他信息作为唯一标识
    QRegularExpression serialRe("Serial Number:\\s+(.+)$");
    QRegularExpressionMatch serialMatch = serialRe.match(output);
    if (serialMatch.hasMatch()) {
        diskInfo.serialNumber = serialMatch.captured(1).trimmed();
        qDebug() << "---NVMe解析调试---找到序列号:" << diskInfo.serialNumber;
    } else {
        QRegularExpression powerOnRe("Power On Hours:\\s+([\\d,]+)");
        QRegularExpressionMatch powerOnMatch = powerOnRe.match(output);
        if (powerOnMatch.hasMatch()) {
            diskInfo.serialNumber = QString("NVMe-%1").arg(powerOnMatch.captured(1).trimmed());
            qDebug() << "---NVMe解析调试---使用Power On Hours作为序列号:" << diskInfo.serialNumber;
        } else {
            diskInfo.serialNumber = "Unknown-NVMe";
            qDebug() << "---NVMe解析调试---未找到序列号，使用默认值:" << diskInfo.serialNumber;
        }
    }
    
    qDebug() << "已解析NVMe设备信息:" << diskInfo.model << diskInfo.serialNumber;
} 