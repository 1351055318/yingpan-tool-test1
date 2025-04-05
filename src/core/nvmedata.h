#ifndef NVMEDATA_H
#define NVMEDATA_H

#include "smartdata.h"

class NVMeData : public SmartData
{
    Q_OBJECT

public:
    explicit NVMeData(QObject *parent = nullptr);
    virtual ~NVMeData();

protected:
    // 检测NVMe设备类型
    virtual bool detectDiskType(const QString &diskPath) override;
    
    // 加载NVMe设备SMART数据
    virtual bool loadSmartData(const QString &diskPath) override;
    
    // 解析NVMe设备输出
    bool parseNVMeOutput(const QString &output);
    
    // 为NVMe设备创建模拟数据
    virtual void createSimulatedData() override;
    
    // NVMe特有属性
    long long m_availableSpare;           // 可用备用空间百分比
    long long m_availableSpareThreshold;  // 可用备用空间阈值
    long long m_percentageUsed;           // 已使用寿命百分比
    long long m_dataUnitsRead;            // 已读取数据量
    long long m_dataUnitsWritten;         // 已写入数据量
    long long m_hostReadCommands;         // 主机读取命令数
    long long m_hostWriteCommands;        // 主机写入命令数
    long long m_controllerBusyTime;       // 控制器忙碌时间
    long long m_unsafeShutdowns;          // 不安全关机次数
    long long m_mediaErrors;              // 媒体错误数
    long long m_errorLogEntries;          // 错误日志条目数
    QList<int> m_temperatures;            // 多个温度传感器值
};

#endif // NVMEDATA_H 