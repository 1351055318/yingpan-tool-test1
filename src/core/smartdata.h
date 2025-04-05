#ifndef SMARTDATA_H
#define SMARTDATA_H

#include <QObject>
#include <QList>
#include <QString>
#include <QStringList>

// 定义SMART属性结构
struct SmartAttribute {
    int id;                 // 属性ID
    QString name;           // 属性名称
    int current;            // 当前值
    int worst;              // 历史最差值
    int threshold;          // 阈值
    long long raw;          // 原始值
    QString status;         // 状态: "正常", "警告", "危险"
    QString description;    // 附加描述
};

// 定义硬盘类型枚举
enum class DiskType {
    Unknown,
    SATA,
    NVMe
};

class SmartData : public QObject
{
    Q_OBJECT

public:
    explicit SmartData(QObject *parent = nullptr);
    virtual ~SmartData();

    // 从磁盘加载SMART数据
    bool loadFromDisk(const QString &diskPath);

    // 获取属性列表
    QList<SmartAttribute> attributes() const;

    // 获取重要指标
    int overallHealth() const;
    int temperature() const;
    int powerOnHours() const;
    int powerCycleCount() const;

    // 获取硬盘类型
    DiskType diskType() const;

    // 获取警告信息
    QStringList warnings() const;
    
    // 检测硬盘类型并创建相应的实例
    virtual bool detectDiskType(const QString &diskPath);

protected:
    // 尝试加载SMART数据
    virtual bool loadSmartData(const QString &diskPath);

    // 创建模拟数据
    virtual void createSimulatedData();

    QList<SmartAttribute> m_attributes;   // SMART属性列表
    QStringList m_warnings;               // 警告信息
    int m_overallHealth;                  // 整体健康度 (0-100)
    int m_temperature;                    // 温度 (°C)
    int m_powerOnHours;                   // 通电时间 (小时)
    int m_powerCycleCount;                // 通电次数
    DiskType m_diskType;                  // 硬盘类型
};

#endif // SMARTDATA_H 