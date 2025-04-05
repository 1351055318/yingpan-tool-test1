#ifndef SATADATA_H
#define SATADATA_H

#include "smartdata.h"

class SATAData : public SmartData
{
    Q_OBJECT

public:
    explicit SATAData(QObject *parent = nullptr);
    virtual ~SATAData();

protected:
    // 检测SATA设备类型
    virtual bool detectDiskType(const QString &diskPath) override;
    
    // 加载SATA设备SMART数据
    virtual bool loadSmartData(const QString &diskPath) override;
    
    // 解析SATA设备输出
    bool parseSATAOutput(const QString &output);
    
    // 为SATA设备创建模拟数据
    virtual void createSimulatedData() override;
};

#endif // SATADATA_H 