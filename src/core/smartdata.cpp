#include "smartdata.h"
#include "satadata.h"
#include "nvmedata.h"
#include <QDebug>
#include <QProcess>
#include <QRegularExpression>
#include <QRandomGenerator>
#include <QStringList>

SmartData::SmartData(QObject *parent)
    : QObject(parent)
    , m_overallHealth(100)
    , m_temperature(0)
    , m_powerOnHours(0)
    , m_powerCycleCount(0)
    , m_diskType(DiskType::Unknown)
{
}

SmartData::~SmartData()
{
}

bool SmartData::loadFromDisk(const QString &diskPath)
{
    qDebug() << "开始获取SMART数据，磁盘路径:" << diskPath;
    qDebug() << "---基础SMART调试---开始加载磁盘SMART数据，路径:" << diskPath;
    
    // 初始化默认值
    m_overallHealth = 100;
    m_temperature = 0;
    m_powerOnHours = 0;
    m_powerCycleCount = 0;
    m_attributes.clear();
    m_warnings.clear();
    
    qDebug() << "---基础SMART调试---已初始化所有属性为默认值";
    
    // 检测硬盘类型
    bool success = detectDiskType(diskPath);
    qDebug() << "---基础SMART调试---硬盘类型检测" << (success ? "成功" : "失败") 
             << "，类型:" << (m_diskType == DiskType::SATA ? "SATA" : 
                            (m_diskType == DiskType::NVMe ? "NVMe" : "Unknown"));
    
    // 加载SMART数据
    if (success) {
        qDebug() << "---基础SMART调试---开始加载SMART属性数据";
        success = loadSmartData(diskPath);
        qDebug() << "---基础SMART调试---加载SMART属性数据" << (success ? "成功" : "失败")
                 << "，属性数量:" << m_attributes.size();
    } else {
        qDebug() << "---基础SMART调试---因硬盘类型检测失败，跳过SMART数据加载";
    }
    
    // 如果没有获取到任何SMART属性，使用模拟数据
    if (!success || m_attributes.isEmpty()) {
        qDebug() << "未能获取到SMART属性数据，使用模拟数据";
        qDebug() << "---基础SMART调试---SMART数据获取失败或属性为空，切换到模拟数据";
        createSimulatedData();
        qDebug() << "---基础SMART调试---已创建模拟数据，属性数量:" << m_attributes.size();
        return false;
    }
    
    qDebug() << "---基础SMART调试---SMART数据加载完成，总体健康度:" << m_overallHealth
             << "，温度:" << m_temperature
             << "，通电时间:" << m_powerOnHours
             << "，通电次数:" << m_powerCycleCount;
    
    return true;
}

bool SmartData::detectDiskType(const QString &diskPath)
{
    // 默认实现，实际逻辑由子类覆盖
    qDebug() << "---基础SMART调试---基类detectDiskType被调用，使用默认实现";
    m_diskType = DiskType::Unknown;
    return false;
}

bool SmartData::loadSmartData(const QString &diskPath)
{
    // 默认实现，实际逻辑由子类覆盖
    qDebug() << "---基础SMART调试---基类loadSmartData被调用，使用默认实现";
    return false;
}

void SmartData::createSimulatedData()
{
    // 默认实现，实际逻辑由子类覆盖
    qDebug() << "创建基础模拟SMART数据";
    qDebug() << "---基础SMART调试---创建基础模拟数据";
    
    // 创建一些模拟属性
    QList<SmartAttribute> simAttributes;
    
    // 健康度和温度
    m_overallHealth = 90 + QRandomGenerator::global()->bounded(11); // 90-100之间
    m_temperature = 35 + QRandomGenerator::global()->bounded(15);   // 35-49度之间
    m_powerOnHours = 5000 + QRandomGenerator::global()->bounded(10000); // 5000-14999小时
    m_powerCycleCount = 500 + QRandomGenerator::global()->bounded(1000); // 500-1499次
    
    qDebug() << "---基础SMART调试---设置模拟值: 健康度=" << m_overallHealth
             << "，温度=" << m_temperature
             << "，通电时间=" << m_powerOnHours
             << "，通电次数=" << m_powerCycleCount;
    
    // 添加一些通用属性
    // 温度
    SmartAttribute tempAttr;
    tempAttr.id = 194;
    tempAttr.name = "Temperature_Celsius";
    tempAttr.current = 100;
    tempAttr.worst = 100;
    tempAttr.threshold = 0;
    tempAttr.raw = m_temperature;
    tempAttr.status = "正常";
    tempAttr.description = QString("%1 °C").arg(m_temperature);
    simAttributes.append(tempAttr);
    qDebug() << "---基础SMART调试---添加模拟属性: 温度(194)";
    
    // 通电时间
    SmartAttribute hoursAttr;
    hoursAttr.id = 9;
    hoursAttr.name = "Power_On_Hours";
    hoursAttr.current = 100;
    hoursAttr.worst = 100;
    hoursAttr.threshold = 0;
    hoursAttr.raw = m_powerOnHours;
    hoursAttr.status = "正常";
    hoursAttr.description = QString("%1 小时").arg(m_powerOnHours);
    simAttributes.append(hoursAttr);
    qDebug() << "---基础SMART调试---添加模拟属性: 通电时间(9)";
    
    // 通电次数
    SmartAttribute cyclesAttr;
    cyclesAttr.id = 12;
    cyclesAttr.name = "Power_Cycle_Count";
    cyclesAttr.current = 100;
    cyclesAttr.worst = 100;
    cyclesAttr.threshold = 0;
    cyclesAttr.raw = m_powerCycleCount;
    cyclesAttr.status = "正常";
    cyclesAttr.description = QString("%1 次").arg(m_powerCycleCount);
    simAttributes.append(cyclesAttr);
    qDebug() << "---基础SMART调试---添加模拟属性: 通电次数(12)";
    
    // 设置模拟属性
    m_attributes = simAttributes;
    qDebug() << "创建了" << simAttributes.size() << "个基础模拟SMART属性";
    qDebug() << "---基础SMART调试---完成模拟属性创建，总属性数:" << m_attributes.size();
    
    for (const SmartAttribute &attr : m_attributes) {
        qDebug() << "---基础SMART调试---模拟属性详情: ID=" << attr.id 
                 << ", 名称=" << attr.name 
                 << ", 当前值=" << attr.current 
                 << ", 原始值=" << attr.raw
                 << ", 描述=" << attr.description;
    }
}

// 获取属性列表
QList<SmartAttribute> SmartData::attributes() const
{
    return m_attributes;
}

// 获取重要指标
int SmartData::overallHealth() const
{
    return m_overallHealth;
}

int SmartData::temperature() const
{
    return m_temperature;
}

int SmartData::powerOnHours() const
{
    return m_powerOnHours;
}

int SmartData::powerCycleCount() const
{
    return m_powerCycleCount;
}

DiskType SmartData::diskType() const
{
    return m_diskType;
}

// 获取警告信息
QStringList SmartData::warnings() const
{
    return m_warnings;
}