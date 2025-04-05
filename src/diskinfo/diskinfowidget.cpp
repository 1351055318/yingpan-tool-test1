#include "diskinfowidget.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QComboBox>
#include <QPushButton>
#include <QTableWidget>
#include <QHeaderView>
#include <QGroupBox>
#include <QFileDialog>
#include <QMessageBox>
#include <QDateTime>
#include <QStandardPaths>
#include <QFile>
#include <QTextStream>

#include "../core/diskutils.h"

DiskInfoWidget::DiskInfoWidget(QWidget *parent) :
    QWidget(parent),
    m_currentDiskIndex(-1)
{
    setupUI();
    refreshDiskList();
}

DiskInfoWidget::~DiskInfoWidget()
{
}

void DiskInfoWidget::setupUI()
{
    // 创建主布局
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(15, 15, 15, 15);
    m_mainLayout->setSpacing(15);
    
    // 创建控制区域
    m_controlLayout = new QHBoxLayout();
    m_diskLabel = new QLabel("选择硬盘:", this);
    m_diskComboBox = new QComboBox(this);
    m_refreshButton = new QPushButton("刷新", this);
    m_saveInfoButton = new QPushButton("保存信息", this);
    
    m_controlLayout->addWidget(m_diskLabel);
    m_controlLayout->addWidget(m_diskComboBox, 1);
    m_controlLayout->addWidget(m_refreshButton);
    m_controlLayout->addWidget(m_saveInfoButton);
    
    // 创建硬盘信息组
    m_diskInfoGroup = new QGroupBox("硬盘信息", this);
    m_diskInfoLayout = new QFormLayout(m_diskInfoGroup);
    
    m_modelValueLabel = new QLabel(this);
    m_serialValueLabel = new QLabel(this);
    m_firmwareValueLabel = new QLabel(this);
    m_interfaceValueLabel = new QLabel(this);
    m_typeValueLabel = new QLabel(this);
    m_sizeValueLabel = new QLabel(this);
    
    QFont valueFont = m_modelValueLabel->font();
    valueFont.setBold(true);
    m_modelValueLabel->setFont(valueFont);
    m_serialValueLabel->setFont(valueFont);
    m_firmwareValueLabel->setFont(valueFont);
    m_interfaceValueLabel->setFont(valueFont);
    m_typeValueLabel->setFont(valueFont);
    m_sizeValueLabel->setFont(valueFont);
    
    m_diskInfoLayout->addRow("型号:", m_modelValueLabel);
    m_diskInfoLayout->addRow("序列号:", m_serialValueLabel);
    m_diskInfoLayout->addRow("固件版本:", m_firmwareValueLabel);
    m_diskInfoLayout->addRow("接口类型:", m_interfaceValueLabel);
    m_diskInfoLayout->addRow("硬盘类型:", m_typeValueLabel);
    m_diskInfoLayout->addRow("硬盘容量:", m_sizeValueLabel);
    
    // 创建卷信息组
    m_volumeInfoGroup = new QGroupBox("卷信息", this);
    QVBoxLayout *volumeLayout = new QVBoxLayout(m_volumeInfoGroup);
    
    m_volumeTable = new QTableWidget(this);
    m_volumeTable->setColumnCount(6);
    m_volumeTable->setHorizontalHeaderLabels(QStringList() 
                                             << "盘符" 
                                             << "标签" 
                                             << "文件系统" 
                                             << "总容量" 
                                             << "已用空间" 
                                             << "可用空间");
    m_volumeTable->horizontalHeader()->setStretchLastSection(true);
    m_volumeTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_volumeTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_volumeTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_volumeTable->setAlternatingRowColors(true);
    
    volumeLayout->addWidget(m_volumeTable);
    
    // 添加所有元素到主布局
    m_mainLayout->addLayout(m_controlLayout);
    m_mainLayout->addWidget(m_diskInfoGroup);
    m_mainLayout->addWidget(m_volumeInfoGroup, 1);
    
    // 连接信号槽
    connect(m_diskComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &DiskInfoWidget::onDiskSelectionChanged);
    connect(m_refreshButton, &QPushButton::clicked, this, &DiskInfoWidget::onRefreshClicked);
    connect(m_saveInfoButton, &QPushButton::clicked, this, &DiskInfoWidget::onSaveInfoClicked);
}

void DiskInfoWidget::refreshDiskList()
{
    // 保存当前选择
    QString currentDiskPath;
    if (m_currentDiskIndex >= 0 && m_currentDiskIndex < m_disks.size()) {
        currentDiskPath = m_disks[m_currentDiskIndex].diskPath;
    }
    
    // 清除旧数据
    m_diskComboBox->clear();
    m_disks.clear();
    
    // 获取硬盘列表
    m_disks = DiskUtils::getAllDisks();
    
    // 填充下拉框
    int index = 0;
    int newSelectedIndex = -1;
    
    for (const DiskInfo &disk : m_disks) {
        QString displayText = QString("%1 (%2) - %3")
                            .arg(disk.model)
                            .arg(disk.diskPath)
                            .arg(DiskUtils::formatSize(disk.diskSize));
        m_diskComboBox->addItem(displayText);
        
        // 检查是否为之前选择的磁盘
        if (disk.diskPath == currentDiskPath) {
            newSelectedIndex = index;
        }
        
        index++;
    }
    
    // 如果找到之前的选择，则还原
    if (newSelectedIndex >= 0) {
        m_diskComboBox->setCurrentIndex(newSelectedIndex);
    } else if (!m_disks.isEmpty()) {
        // 否则选择第一个
        m_diskComboBox->setCurrentIndex(0);
    }
}

void DiskInfoWidget::onDiskSelectionChanged(int index)
{
    displayDiskInfo(index);
}

void DiskInfoWidget::onRefreshClicked()
{
    refreshDiskList();
}

void DiskInfoWidget::onSaveInfoClicked()
{
    if (m_currentDiskIndex < 0 || m_currentDiskIndex >= m_disks.size()) {
        QMessageBox::warning(this, "警告", "请先选择一个磁盘");
        return;
    }
    
    // 获取保存路径
    QString filePath = QFileDialog::getSaveFileName(
        this,
        "保存磁盘信息",
        QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/磁盘信息.txt",
        "文本文件 (*.txt);;所有文件 (*.*)"
    );
    
    if (filePath.isEmpty()) {
        return;
    }
    
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "错误", "无法打开文件进行写入: " + filePath);
        return;
    }
    
    QTextStream out(&file);
    out.setCodec("UTF-8");
    
    const DiskInfo &disk = m_disks[m_currentDiskIndex];
    
    // 写入基本信息
    out << "===== 磁盘信息 =====" << "\n\n";
    out << "设备名称: " << disk.diskPath << "\n";
    out << "型号: " << disk.model << "\n";
    out << "序列号: " << disk.serialNumber << "\n";
    out << "固件版本: " << disk.firmwareVersion << "\n";
    out << "接口类型: " << disk.interfaceSpeed << "\n";
    out << "硬盘类型: " << disk.type << "\n";
    out << "硬盘容量: " << DiskUtils::formatSize(disk.diskSize) << " (" << disk.diskSize << " 字节)\n\n";
    
    // 写入卷信息
    out << "===== 卷信息 =====" << "\n\n";
    
    // 获取卷信息
    QList<VolumeInfo> volumes = DiskUtils::getVolumes();
    for (const VolumeInfo &volume : volumes) {
        // 只保存与当前磁盘相关的卷信息
        if (!disk.diskPath.isEmpty() && !volume.rootPath.startsWith(disk.diskPath)) {
            continue;
        }
        
        out << "盘符: " << volume.rootPath << "\n";
        out << "  标签: " << volume.label << "\n";
        out << "  文件系统: " << volume.fileSystem << "\n";
        out << "  总容量: " << DiskUtils::formatSize(volume.totalSpace) << "\n";
        out << "  已用空间: " << DiskUtils::formatSize(volume.usedSpace) << "\n";  
        out << "  可用空间: " << DiskUtils::formatSize(volume.freeSpace) << "\n\n";
    }
    
    // 写入时间戳
    out << "生成时间: " << QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss") << "\n";
    
    file.close();
    
    QMessageBox::information(this, "成功", "磁盘信息已保存到: " + filePath);
}

void DiskInfoWidget::displayDiskInfo(int index)
{
    if (index < 0 || index >= m_disks.size()) {
        return;
    }
    
    const DiskInfo &disk = m_disks[index];
    
    // 更新标签
    m_modelValueLabel->setText(disk.model);
    m_serialValueLabel->setText(disk.serialNumber);
    m_firmwareValueLabel->setText(disk.firmwareVersion);
    m_interfaceValueLabel->setText(disk.interfaceSpeed);
    m_typeValueLabel->setText(disk.type);
    m_sizeValueLabel->setText(DiskUtils::formatSize(disk.diskSize));
    
    // 显示卷信息
    displayVolumeInfo(disk);
}

void DiskInfoWidget::displayVolumeInfo(const DiskInfo& disk)
{
    // 清空表格
    m_volumeTable->clearContents();
    m_volumeTable->setRowCount(0);
    
    // 获取卷信息
    QList<VolumeInfo> volumes = DiskUtils::getVolumes();
    
    // 填充表格
    int row = 0;
    for (const VolumeInfo &volume : volumes) {
        // 只显示与当前硬盘相关的分区
        // 检查卷的盘符是否在当前磁盘的volumes列表中
        if (!disk.volumes.contains(volume.driveLetter) && 
            // 或者检查卷的盘符是否是当前磁盘的主卷
            volume.driveLetter != disk.mainVolume &&
            // 如果磁盘路径以\\.\开头(原始物理磁盘格式)，则不进行路径开头匹配
            // 否则尝试匹配路径前缀(用于模拟数据或其他情况)
            (!disk.diskPath.startsWith("\\\\.\\") && 
             !disk.diskPath.isEmpty() && 
             !volume.rootPath.startsWith(disk.diskPath))) {
            // 跳过不属于当前磁盘的卷
            continue;
        }
        
        m_volumeTable->insertRow(row);
        
        // 盘符
        QTableWidgetItem *rootItem = new QTableWidgetItem(volume.rootPath);
        rootItem->setTextAlignment(Qt::AlignCenter);
        m_volumeTable->setItem(row, 0, rootItem);
        
        // 卷标
        QTableWidgetItem *labelItem = new QTableWidgetItem(volume.label);
        m_volumeTable->setItem(row, 1, labelItem);
        
        // 文件系统
        QTableWidgetItem *fsItem = new QTableWidgetItem(volume.fileSystem);
        fsItem->setTextAlignment(Qt::AlignCenter);
        m_volumeTable->setItem(row, 2, fsItem);
        
        // 总容量
        m_volumeTable->setItem(row, 3, new QTableWidgetItem(DiskUtils::formatSize(volume.totalSpace)));
        
        // 已用空间
        qint64 usedSpace = volume.usedSpace;
        m_volumeTable->setItem(row, 4, new QTableWidgetItem(DiskUtils::formatSize(usedSpace)));
        
        // 可用空间
        m_volumeTable->setItem(row, 5, new QTableWidgetItem(DiskUtils::formatSize(volume.freeSpace)));
        
        row++;
    }
    
    // 如果表格仍然为空，添加一条提示信息
    if (row == 0) {
        m_volumeTable->insertRow(0);
        QTableWidgetItem *noDataItem = new QTableWidgetItem("未找到与此磁盘关联的卷");
        noDataItem->setTextAlignment(Qt::AlignCenter);
        m_volumeTable->setItem(0, 0, noDataItem);
        m_volumeTable->setSpan(0, 0, 1, 6); // 合并单元格以显示提示
    }
}

QString DiskInfoWidget::getVolumeType(const QString &fileSystem)
{
    if (fileSystem.isEmpty()) {
        return "未知";
    }
    
    if (fileSystem.toLower() == "ntfs") {
        return "NTFS";
    } else if (fileSystem.toLower() == "fat32") {
        return "FAT32";
    } else if (fileSystem.toLower() == "exfat") {
        return "exFAT";
    } else if (fileSystem.toLower() == "fat") {
        return "FAT";
    } else if (fileSystem.toLower() == "refs") {
        return "ReFS";
    } else if (fileSystem.toLower() == "ext4") {
        return "Ext4";
    } else if (fileSystem.toLower() == "ext3") {
        return "Ext3";
    } else if (fileSystem.toLower() == "ext2") {
        return "Ext2";
    } else if (fileSystem.toLower() == "hfs+") {
        return "HFS+";
    } else if (fileSystem.toLower() == "apfs") {
        return "APFS";
    }
    
    return fileSystem;
} 