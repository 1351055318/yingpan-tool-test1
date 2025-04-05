#ifndef SMARTFACTORY_H
#define SMARTFACTORY_H

#include <QObject>
#include "smartdata.h"
#include "satadata.h"
#include "nvmedata.h"
#include "diskdetector.h"

class SmartFactory : public QObject
{
    Q_OBJECT

public:
    explicit SmartFactory(QObject *parent = nullptr);
    ~SmartFactory();

    // 创建适当的SMART数据处理器
    static SmartData* createSmartData(const QString &diskPath, QObject *parent = nullptr);
    
    // 检测系统中的所有硬盘
    static QList<DiskSMARTInfo> detectDisks();

private:
    // 检测磁盘类型
    static DiskType detectDiskType(const QString &diskPath);
};

#endif // SMARTFACTORY_H 