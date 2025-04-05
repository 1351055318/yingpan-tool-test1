#ifndef SMARTATTRIBUTEDICT_H
#define SMARTATTRIBUTEDICT_H

#include <QMap>
#include <QString>

class SmartAttributeDict
{
public:
    static QMap<QString, QString> getEnglishToChineseMap() {
        static QMap<QString, QString> dict = {
            // 常见SMART属性英文到中文的映射
            {"Raw_Read_Error_Rate", "原始读取错误率"},
            {"Throughput_Performance", "吞吐性能"},
            {"Spin_Up_Time", "启动时间"},
            {"Start_Stop_Count", "启停次数"},
            {"Reallocated_Sector_Ct", "重新分配扇区计数"},
            {"Read_Channel_Margin", "读取通道裕度"},
            {"Seek_Error_Rate", "寻道错误率"},
            {"Seek_Time_Performance", "寻道时间性能"},
            {"Power_On_Hours", "通电时间"},
            {"Spin_Retry_Count", "主轴重试次数"},
            {"Calibration_Retry_Count", "校准重试次数"},
            {"Power_Cycle_Count", "通电次数"},
            {"Power-Off_Retract_Count", "断电后收回计数"},
            {"Load_Cycle_Count", "加载周期计数"},
            {"Temperature_Celsius", "温度（摄氏度）"},
            {"Reallocated_Event_Count", "重新分配事件计数"},
            {"Current_Pending_Sector", "当前待处理扇区"},
            {"Offline_Uncorrectable", "离线无法修正的扇区数"},
            {"UDMA_CRC_Error_Count", "UDMA CRC错误计数"},
            {"Multi_Zone_Error_Rate", "多区域错误率"},
            {"Write_Error_Rate", "写入错误率"},
            {"TA_Increase_Count", "TA增加计数"},
            {"Reported_Uncorrect", "报告的不可修正错误"},
            {"Command_Timeout", "命令超时"},
            {"High_Fly_Writes", "高飞写入"},
            {"Airflow_Temperature_Cel", "气流温度"},
            {"G-Sense_Error_Rate", "G感应错误率"},
            {"Head_Stability", "磁头稳定性"},
            {"Induced_Op_Vibration_Detection", "感应操作振动检测"},
            {"Reported_Uncorrectable_Errors", "报告的不可修正错误"},
            {"Hardware_ECC_Recovered", "硬件ECC恢复"},
            {"Reallocation_Event_Count", "重新分配事件计数"},
            {"Current_Pending_Sector_Count", "当前待处理扇区计数"},
            {"Uncorrectable_Sector_Count", "不可修正扇区计数"},
            {"UltraDMA_CRC_Error_Count", "UltraDMA CRC错误计数"},
            {"Write_Error_Rate_Total", "总写入错误率"},
            {"Soft_Read_Error_Rate", "软读取错误率"},
            {"Flying_Height", "飞行高度"},
            {"Spin_High_Current", "主轴高电流"},
            {"Spin_Buzz", "主轴嗡嗡声"},
            {"Offline_Seek_Performance", "离线寻道性能"},
            {"Vibration_During_Write", "写入时振动"},
            {"Vibration_During_Read", "读取时振动"},
            {"Shock_During_Write", "写入时震动"},
            {"Disk_Shift", "磁盘偏移"},
            {"G-Sense_Error_Rate_Alt", "G感应错误率（备用）"},
            {"Loaded_Hours", "加载小时数"},
            {"Load_Retry_Count", "加载重试计数"},
            {"Load_Friction", "加载摩擦"},
            {"Load_Cycle_Count_Alt", "加载周期计数（备用）"},
            {"Load-in_Time", "加载时间"},
            {"Torque_Amplification_Count", "扭矩放大计数"},
            {"Power-Off_Retract_Cycle", "断电收回周期"},
            {"GMR_Head_Amplitude", "GMR磁头幅度"},
            {"Head_Loading_Time", "磁头加载时间"},
            {"Head_Depotion", "磁头去除"},
            {"Available_Reservd_Space", "可用保留空间"},
            {"Media_Wearout_Indicator", "介质磨损指示器"},
            {"SSD_Life_Left", "SSD剩余寿命"},
            {"Power_Loss_Cap_Test", "断电保护测试"},
            {"Total_LBAs_Written", "总LBA写入"},
            {"Total_LBAs_Read", "总LBA读取"},
            {"SSD_Wear_Leveling_Count", "SSD磨损均衡计数"},
            {"Unused_Rsvd_Blk_Cnt_Tot", "未使用保留块计数总计"},
            {"Program_Fail_Count_Total", "总程序失败计数"},
            {"Erase_Fail_Count_Total", "总擦除失败计数"},
            {"Average_Block-Erase_Count", "平均块擦除计数"},
            {"SATA_Downshift_Error_Count", "SATA降档错误计数"},
            {"End-to-End_Error", "端到端错误"},
            {"Head_Stability", "磁头稳定性"},
            {"Verified_Correct_on_Reads", "读取时已验证正确"},
            {"Shift_Amount_from_Warranty", "脱离保修期的程度"},
            {"Wear_Range_Delta", "磨损范围差异"},
            {"Used_Reserved_Block_Count", "已使用保留块计数"},
            {"Unused_Reserved_Block_Count", "未使用保留块计数"},
            {"Program_Fail_Count", "程序失败计数"},
            {"Erase_Fail_Count", "擦除失败计数"},
            {"Wear_Leveling_Count", "磨损均衡计数"},
            {"Used_Rsvd_Blk_Cnt_Chip", "芯片已使用保留块计数"},
            {"Used_Rsvd_Blk_Cnt_Tot", "总已使用保留块计数"},
            {"Program_Fail_Cnt_Total", "总程序失败计数"},
            {"Erase_Fail_Count_Total", "总擦除失败计数"},
            {"Runtime_Bad_Block", "运行时坏块"},
            {"Thermal_Throttle", "热节流"},
            {"Temperature", "温度"},
            {"Reallocated_Sectors_Count", "重新分配扇区计数"},
            {"Unknown_Attribute", "未知属性"},
            {"Data_Units_Read", "数据单元读取"},
            {"Data_Units_Written", "数据单元写入"},
            {"Host_Read_Commands", "主机读取命令"},
            {"Host_Write_Commands", "主机写入命令"},
            {"Controller_Busy_Time", "控制器忙碌时间"},
            {"Available_Spare", "可用备件"},
            {"Available_Spare_Threshold", "可用备件阈值"},
            {"Percentage_Used", "使用百分比"},
            {"Media_Errors", "介质错误"},
            {"Unsafe_Shutdowns", "不安全关机"}
        };
        return dict;
    }
    
    static QString translate(const QString &englishName) {
        QMap<QString, QString> dict = getEnglishToChineseMap();
        return dict.contains(englishName) ? dict[englishName] : englishName;
    }
};

#endif // SMARTATTRIBUTEDICT_H 