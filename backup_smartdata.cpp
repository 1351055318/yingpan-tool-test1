bool SmartData::loadFromDisk(const QString &diskPath) {
    
    // 使用smartctl工具获取SMART数据
    QProcess smartctlProcess;
    
    // 首先尝试SATA设备命令
    QStringList arguments;
    arguments << "-a" << "-d" << "ata" << "/dev/sda";
    
    qDebug() << "执行命令(SATA): smartctl" << arguments.join(" ");
    
    // 设置进程超时
    QTimer timer;
    timer.setSingleShot(true);
    
    // 启动进程
    smartctlProcess.start("smartctl", arguments);
    
    // 启动超时定时器
    timer.start(5000); // 5秒超时
    
    // 创建事件循环等待进程完成或超时
    QEventLoop loop;
    QObject::connect(&smartctlProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), &loop, &QEventLoop::quit);
    QObject::connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);
    QObject::connect(&timer, &QTimer::timeout, &smartctlProcess, &QProcess::kill);
    
    // 等待进程完成或超时
    loop.exec();
    
    // 检查是否超时
    if (timer.isActive()) {
        // 未超时，停止定时器
        timer.stop();
    } else {
        // 超时，进程已被终止
        qDebug() << "获取SMART数据超时";
        CreatePartialSmartData(); // 使用部分模拟数据
        return false;
    }
    
    bool isSATA = true; // 默认为SATA设备
    
    // 检查退出状态
    if (smartctlProcess.exitCode() != 0) {
        qDebug() << "获取SATA设备SMART数据失败:" << smartctlProcess.readAllStandardError();
        
        // 尝试NVMe设备
        arguments.clear();
        arguments << "-a" << "-d" << "nvme" << "/dev/sdb";
        
        qDebug() << "执行命令(NVMe): smartctl" << arguments.join(" ");
        
        // 重新启动进程
        smartctlProcess.start("smartctl", arguments);
        
        // 重新设置超时
        timer.start(5000);
        
        // 重新等待进程完成或超时
        loop.exec();
        
        if (timer.isActive()) {
            timer.stop();
            isSATA = false; // 使用NVMe设备
        } else {
            qDebug() << "获取NVMe设备SMART数据超时";
            CreatePartialSmartData();
            return false;
        }
        
        // 再次检查退出状态
        if (smartctlProcess.exitCode() != 0) {
            qDebug() << "获取NVMe设备SMART数据失败:" << smartctlProcess.readAllStandardError();
            
            // 尝试使用PowerShell Get-PhysicalDisk命令获取基本信息
            QProcess powershellProcess;
            QString psCommand = QString("Get-PhysicalDisk | Where-Object DeviceId -eq %1 | Format-List *").arg(driveNumber);
            powershellProcess.start("powershell", QStringList() << "-Command" << psCommand);
            
            // 设置3秒超时
            if (!powershellProcess.waitForFinished(3000)) {
                qDebug() << "PowerShell查询超时";
                powershellProcess.kill();
                CreateSimulatedData();
                return false;
            }
        }
    }

    // 解析smartctl输出
    QString output = QString::fromLocal8Bit(smartctlProcess.readAllStandardOutput());
    QString error = QString::fromLocal8Bit(smartctlProcess.readAllStandardError());
    
    qDebug() << "smartctl输出长度:" << output.length();
    if (!error.isEmpty()) {
        qDebug() << "smartctl错误:" << error;
    }
    
    // 打印完整输出以便调试
    qDebug() << "smartctl原始输出:" << output;
    
    // 使用QStringList对输出进行分割，提高效率
    QStringList lines = output.split("\n");

    // 直接打印每一行进行调试
    qDebug() << "输出共有" << lines.size() << "行";
    for (int i = 0; i < qMin(20, lines.size()); i++) {
        qDebug() << "行" << i << ":" << lines[i];
    }
    
    // 提取基本信息
    m_overallHealth = 100; // 默认健康度
    bool inSmartData = false;

    // 预先编译正则表达式，避免重复创建
    QRegularExpression tempRe1("Temperature.*?([0-9]+)\\s*(?:Celsius|C)", QRegularExpression::CaseInsensitiveOption);
    QRegularExpression tempRe2("Current\\s*Drive\\s*Temperature:\\s*([0-9]+)\\s*C", QRegularExpression::CaseInsensitiveOption);
    QRegularExpression tempRe3("Temperature:\\s*([0-9]+)\\s*Celsius", QRegularExpression::CaseInsensitiveOption);
    QRegularExpression hoursRe1("Power_On_Hours.*?([0-9]+)", QRegularExpression::CaseInsensitiveOption);
    QRegularExpression hoursRe2("Power On Time.*?([0-9]+)\\s*h", QRegularExpression::CaseInsensitiveOption);
    QRegularExpression cycleRe("(?:Power_Cycle_Count|Start_Stop_Count).*?([0-9]+)", QRegularExpression::CaseInsensitiveOption);
    
    // NVMe特定的正则表达式 - 改进匹配模式
    QRegularExpression nvmeTempRe("Temperature:\\s*(\\d+)\\s*Celsius", QRegularExpression::CaseInsensitiveOption);
    QRegularExpression nvmePowerOnRe("Power On Hours:\\s*(\\d+)", QRegularExpression::CaseInsensitiveOption);
    QRegularExpression nvmePowerCycleRe("Power Cycles:\\s*(\\d+)", QRegularExpression::CaseInsensitiveOption);
    QRegularExpression nvmePercentUsedRe("Percentage Used:\\s*(\\d+)%", QRegularExpression::CaseInsensitiveOption);
    QRegularExpression nvmeDataReadRe("Data Units Read:\\s*([\\d,]+)", QRegularExpression::CaseInsensitiveOption);
    QRegularExpression nvmeDataWrittenRe("Data Units Written:\\s*([\\d,]+)", QRegularExpression::CaseInsensitiveOption);
    
    if (isSATA) {
        // SATA设备的SMART属性正则表达式
        QRegularExpression attrRe1("^\\s*(\\d+)\\s+([\\w_]+)\\s+.*?\\s+(\\d+)\\s+(\\d+)\\s+(---|-|\\d+).*?\\s+([\\d-]+)$");
        
        // 批量收集属性，避免逐行处理带来的效率问题
        QVector<SmartAttribute> tempAttributes;
        int attributesFound = 0;
        
        for (const QString &line : lines) {
            // 检查健康状态
            if (line.contains("SMART overall-health self-assessment test result", Qt::CaseInsensitive)) {
                if (line.contains("PASSED", Qt::CaseInsensitive)) {
                    m_overallHealth = 90 + QRandomGenerator::global()->bounded(11); // 90-100
                    qDebug() << "SMART自测通过，健康度设置为:" << m_overallHealth;
                } else if (line.contains("FAILED", Qt::CaseInsensitive)) {
                    m_overallHealth = 10 + QRandomGenerator::global()->bounded(50); // 10-59
                    m_warnings.append("警告: SMART自检失败！建议立即备份数据");
                    qDebug() << "SMART自测失败，健康度设置为:" << m_overallHealth;
                }
                continue;
            }
            
            // 查找SMART属性表的开始
            if (line.contains("Vendor Specific SMART Attributes", Qt::CaseInsensitive) || 
                line.contains("ID# ATTRIBUTE_NAME", Qt::CaseInsensitive)) {
                inSmartData = true;
                qDebug() << "发现SMART属性表开始";
                continue;
            }
            
            // 温度信息 - 多种可能的格式
            if (line.contains("Temperature", Qt::CaseInsensitive) || line.contains("Airflow_Temperature", Qt::CaseInsensitive)) {
                QRegularExpressionMatch match = tempRe1.match(line);
                if (match.hasMatch()) {
                    m_temperature = match.captured(1).toInt();
                } else {
                    match = tempRe2.match(line);
                    if (match.hasMatch()) {
                        m_temperature = match.captured(1).toInt();
                    } else {
                        match = tempRe3.match(line);
                        if (match.hasMatch()) {
                            m_temperature = match.captured(1).toInt();
                        }
                    }
                }
                continue;
            }
            
            // 通电时间
            if (line.contains("Power_On_Hours", Qt::CaseInsensitive) || line.contains("Power On Time", Qt::CaseInsensitive)) {
                QRegularExpressionMatch match = hoursRe1.match(line);
                if (match.hasMatch()) {
                    m_powerOnHours = match.captured(1).toInt();
                } else {
                    match = hoursRe2.match(line);
                    if (match.hasMatch()) {
                        m_powerOnHours = match.captured(1).toInt();
                    }
                }
                continue;
            }
            
            // 通电次数
            if (line.contains("Power_Cycle_Count", Qt::CaseInsensitive) || line.contains("Start_Stop_Count", Qt::CaseInsensitive)) {
                QRegularExpressionMatch match = cycleRe.match(line);
                if (match.hasMatch()) {
                    m_powerCycleCount = match.captured(1).toInt();
                }
                continue;
            }
            
            // 如果在SMART属性表中
            if (inSmartData) {
                // 尝试匹配属性行
                QRegularExpressionMatch match = attrRe1.match(line);
                if (match.hasMatch()) {
                    SmartAttribute attr;
                    attr.id = match.captured(1).toInt();
                    QString attrName = match.captured(2);
                    attr.name = attrName;
                    attr.current = match.captured(3).toInt();
                    attr.worst = match.captured(4).toInt();
                    QString threshStr = match.captured(5);
                    attr.threshold = threshStr == "---" || threshStr == "-" ? 0 : threshStr.toInt();
                    QString rawStr = match.captured(6);
                    attr.raw = rawStr == "-" ? 0 : rawStr.toLongLong();
                    
                    // 确定状态
                    if (attr.threshold > 0 && attr.current <= attr.threshold) {
                        attr.status = "危险";
                        m_warnings.append(QString("警告: 属性 %1(%2) 已达到或超过阈值").arg(attr.name).arg(attr.id));
                    } else if (attr.current < 70) {
                        attr.status = "警告";
                    } else {
                        attr.status = "正常";
                    }
                    
                    // 特殊属性处理
                    if (attr.id == 194 || attr.id == 190) {
                        // 温度属性
                        int temp = attr.raw;
                        if (temp > 65) {
                            attr.status = "危险";
                            m_warnings.append(QString("警告: 硬盘温度过高 (%1°C)").arg(temp));
                        } else if (temp > 55) {
                            attr.status = "警告";
                            m_warnings.append(QString("警告: 硬盘温度偏高 (%1°C)").arg(temp));
                        }
                        
                        // 如果之前没有设置温度，使用该值
                        if (m_temperature <= 0) {
                            m_temperature = temp;
                        }
                    } else if (attr.id == 5 || attr.id == 196) {
                        // 重分配扇区
                        if (attr.raw > 0) {
                            attr.status = "警告";
                            m_warnings.append(QString("警告: 存在重分配扇区 (%1 个)").arg(attr.raw));
                        }
                    } else if (attr.id == 197 || attr.id == 198) {
                        // 坏扇区
                        if (attr.raw > 0) {
                            attr.status = "危险";
                            m_warnings.append(QString("警告: 存在坏扇区 (%1 个)").arg(attr.raw));
                        }
                    } else if (attr.id == 9) {
                        // 通电时间
                        if (m_powerOnHours <= 0) {
                            m_powerOnHours = attr.raw;
                        }
                    }
                    
                    tempAttributes.append(attr);
                    attributesFound++;
                }
            }
        }
        
        // 一次性添加所有属性
        if (!tempAttributes.isEmpty()) {
            m_attributes = tempAttributes.toList();
        }
    } else {
        // NVMe设备处理逻辑
        QVector<SmartAttribute> tempAttributes;
        int attributesFound = 0;
        
        // 打印输出内容用于调试
        qDebug() << "NVMe设备输出内容(前100字符):" << (output.length() > 100 ? output.left(100) : output);
        
        for (const QString &line : lines) {
            qDebug() << "处理NVMe行:" << line;
            
            // 检查健康状态
            if (line.contains("SMART overall-health self-assessment test result", Qt::CaseInsensitive)) {
                if (line.contains("PASSED", Qt::CaseInsensitive)) {
                    m_overallHealth = 90 + QRandomGenerator::global()->bounded(11); // 90-100
                    qDebug() << "SMART自测通过，健康度设置为:" << m_overallHealth;
                } else if (line.contains("FAILED", Qt::CaseInsensitive)) {
                    m_overallHealth = 10 + QRandomGenerator::global()->bounded(50); // 10-59
                    m_warnings.append("警告: SMART自检失败！建议立即备份数据");
                    qDebug() << "SMART自测失败，健康度设置为:" << m_overallHealth;
                }
                continue;
            }
            
            // 温度信息 - 尝试不同格式
            if (line.contains("Temperature:", Qt::CaseInsensitive)) {
                QRegularExpressionMatch match = nvmeTempRe.match(line);
                if (match.hasMatch()) {
                    m_temperature = match.captured(1).toInt();
                    qDebug() << "找到NVMe温度:" << m_temperature;
                    
                    // 创建温度属性
                    SmartAttribute attr;
                    attr.id = 194; // 使用与SATA设备相同的ID以保持一致性
                    attr.name = "温度";
                    attr.current = 100; // 固定值
                    attr.worst = 100;
                    attr.threshold = 0;
                    attr.raw = m_temperature;
                    
                    // 判断温度状态
                    if (m_temperature > 70) {
                        attr.status = "危险";
                        m_warnings.append(QString("警告: SSD温度过高 (%1°C)").arg(m_temperature));
                    } else if (m_temperature > 60) {
                        attr.status = "警告";
                        m_warnings.append(QString("警告: SSD温度偏高 (%1°C)").arg(m_temperature));
                    } else {
                        attr.status = "正常";
                    }
                    
                    tempAttributes.append(attr);
                    attributesFound++;
                }
                continue;
            }
            
            // 通电时间
            if (line.contains("Power On Hours:", Qt::CaseInsensitive)) {
                QRegularExpressionMatch match = nvmePowerOnRe.match(line);
                if (match.hasMatch()) {
                    QString hours = match.captured(1);
                    hours.remove(","); // 移除逗号
                    m_powerOnHours = hours.toInt();
                    qDebug() << "找到NVMe通电时间:" << m_powerOnHours;
                    
                    // 创建通电时间属性
                    SmartAttribute attr;
                    attr.id = 9; // 使用与SATA设备相同的ID
                    attr.name = "通电时间";
                    attr.current = 100; // 固定值
                    attr.worst = 100;
                    attr.threshold = 0;
                    attr.raw = m_powerOnHours;
                    attr.status = "正常";
                    
                    tempAttributes.append(attr);
                    attributesFound++;
                }
                continue;
            }
            
            // 通电次数
            if (line.contains("Power Cycles:", Qt::CaseInsensitive)) {
                QRegularExpressionMatch match = nvmePowerCycleRe.match(line);
                if (match.hasMatch()) {
                    QString cycles = match.captured(1);
                    cycles.remove(","); // 移除逗号
                    m_powerCycleCount = cycles.toInt();
                    qDebug() << "找到NVMe通电次数:" << m_powerCycleCount;
                    
                    // 创建通电次数属性
                    SmartAttribute attr;
                    attr.id = 12; // 使用与SATA设备相同的ID
                    attr.name = "通电次数";
                    attr.current = 100; // 固定值
                    attr.worst = 100;
                    attr.threshold = 0;
                    attr.raw = m_powerCycleCount;
                    attr.status = "正常";
                    
                    tempAttributes.append(attr);
                    attributesFound++;
                }
                continue;
            }
            
            // NVMe寿命百分比
            if (line.contains("Percentage Used:", Qt::CaseInsensitive)) {
                QRegularExpressionMatch match = nvmePercentUsedRe.match(line);
                if (match.hasMatch()) {
                    int percentUsed = match.captured(1).toInt();
                    qDebug() << "找到NVMe寿命使用百分比:" << percentUsed;
                    
                    // 创建寿命属性
                    SmartAttribute attr;
                    attr.id = 1000; // 自定义ID
                    attr.name = "寿命百分比";
                    attr.current = 100 - percentUsed; // 转换为剩余寿命
                    attr.worst = attr.current;
                    attr.threshold = 10; // 假设10%为阈值
                    attr.raw = percentUsed;
                    
                    // 确定状态
                    if (attr.current <= attr.threshold) {
                        attr.status = "危险";
                        m_warnings.append("警告: SSD寿命即将耗尽，请及时备份数据");
                    } else if (attr.current < 30) {
                        attr.status = "警告";
                        m_warnings.append("警告: SSD寿命剩余不足30%");
                    } else {
                        attr.status = "正常";
                    }
                    
                    tempAttributes.append(attr);
                    attributesFound++;
                }
                continue;
            }
            
            // 数据读取量
            if (line.contains("Data Units Read:", Qt::CaseInsensitive)) {
                QRegularExpressionMatch match = nvmeDataReadRe.match(line);
                if (match.hasMatch()) {
                    QString unitsRead = match.captured(1);
                    unitsRead.remove(","); // 移除逗号
                    qDebug() << "找到NVMe数据读取量:" << unitsRead;
                    
                    // 创建数据读取属性
                    SmartAttribute attr;
                    attr.id = 1001; // 自定义ID
                    attr.name = "累计读取量";
                    attr.current = 100; // 固定值
                    attr.worst = 100;
                    attr.threshold = 0;
                    attr.raw = unitsRead.toLongLong();
                    attr.status = "正常";
                    
                    tempAttributes.append(attr);
                    attributesFound++;
                }
                continue;
            }
            
            // 数据写入量
            if (line.contains("Data Units Written:", Qt::CaseInsensitive)) {
                QRegularExpressionMatch match = nvmeDataWrittenRe.match(line);
                if (match.hasMatch()) {
                    QString unitsWritten = match.captured(1);
                    unitsWritten.remove(","); // 移除逗号
                    qDebug() << "找到NVMe数据写入量:" << unitsWritten;
                    
                    // 创建数据写入属性
                    SmartAttribute attr;
                    attr.id = 1002; // 自定义ID
                    attr.name = "累计写入量";
                    attr.current = 100; // 固定值
                    attr.worst = 100;
                    attr.threshold = 0;
                    attr.raw = unitsWritten.toLongLong();
                    attr.status = "正常";
                    
                    tempAttributes.append(attr);
                    attributesFound++;
                }
                continue;
            }
        }
        
        // 一次性添加所有属性
        if (!tempAttributes.isEmpty()) {
            m_attributes = tempAttributes.toList();
            qDebug() << "成功添加" << tempAttributes.size() << "个NVMe设备属性";
        } else {
            qDebug() << "未能提取任何NVMe设备属性!";
        }
    }

    qDebug() << "SMART数据解析完成，共获取" << (isSATA ? "SATA" : "NVMe") << "属性" 
             << (m_attributes.isEmpty() ? 0 : m_attributes.size()) << "个，"
             << "健康度:" << m_overallHealth
             << "温度:" << m_temperature
             << "通电时间:" << m_powerOnHours
             << "通电次数:" << m_powerCycleCount;

    // 如果没有获取到任何SMART属性，使用模拟数据
    if (m_attributes.isEmpty()) {
        qDebug() << "未能获取到SMART属性数据，使用模拟数据";
        CreateSimulatedData();
        return false;
    }

    return true;
}