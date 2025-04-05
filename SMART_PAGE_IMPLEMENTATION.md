# SMART 信息页面实现方法

## 概述

SMART (Self-Monitoring, Analysis and Reporting Technology) 页面负责显示硬盘的健康状态和详细属性信息。本文档详细介绍了该页面的实现方法、数据流程和关键技术点。

## 技术架构

SMART 信息页面基于 Electron + React 实现，采用以下技术栈：

- **前端框架**：React
- **UI 组件库**：Ant Design
- **数据可视化**：Progress 组件
- **状态管理**：React Hooks (useState, useEffect)
- **通信机制**：Electron IPC
- **系统信息获取**：systeminformation, smartctl

## 实现流程

### 1. 页面结构

```
DiskSmartInfo
├── 磁盘选择器
├── SMART 信息摘要
│   ├── 基本信息 (型号、序列号等)
│   └── 健康状态指示器
└── SMART 属性表格
    ├── 属性 ID
    ├── 属性名称
    ├── 当前值
    ├── 最差值
    ├── 阈值
    ├── 原始值
    └── 状态指示器
```

### 2. 数据流程

```
用户选择磁盘 → 请求 SMART 数据 → IPC 通信 → 
主进程获取数据 → 返回结果 → 更新 UI 显示
```

### 3. 前端实现 (DiskSmartInfo.js)

#### 3.1 状态管理

```javascript
// 状态定义
const [disks, setDisks] = useState([]);             // 磁盘列表
const [selectedDisk, setSelectedDisk] = useState(null);  // 当前选中磁盘
const [smartData, setSmartData] = useState(null);   // SMART 数据
const [loading, setLoading] = useState(true);       // 加载状态
const [lastUpdated, setLastUpdated] = useState(null); // 最后更新时间
const [language, setLanguage] = useState('zh');     // 显示语言
```

#### 3.2 磁盘列表获取

```javascript
// 获取磁盘列表
useEffect(() => {
  const fetchDisks = async () => {
    try {
      // 调用IPC获取磁盘信息
      const diskInfo = await ipcRenderer.invoke('get-disk-info');
      
      // 过滤有效磁盘
      const validDisks = diskInfo.filter(disk => 
        disk.mounted && 
        disk.physical && 
        disk.physical.name && 
        disk.blocks
      );
      
      setDisks(validDisks);
      
      // 默认选择第一个磁盘
      const firstDisk = validDisks[0];
      setSelectedDisk(firstDisk.mounted);
      
    } catch (error) {
      console.error('获取磁盘信息失败:', error);
    } finally {
      setLoading(false);
    }
  };
  
  fetchDisks();
}, []);
```

#### 3.3 SMART 数据获取

```javascript
// 获取SMART信息
useEffect(() => {
  if (!selectedDisk) return;
  
  const fetchSmartData = async () => {
    setLoading(true);
    try {
      // 调用IPC获取SMART数据，支持多语言
      const data = await ipcRenderer.invoke('get-smart-data', selectedDisk, language);
      
      if (data.error) {
        setSmartData({ error: data.error });
      } else {
        setSmartData(data);
      }
      setLastUpdated(new Date());
    } catch (error) {
      setSmartData({ error: error.message });
    } finally {
      setLoading(false);
    }
  };
  
  fetchSmartData();
  
  // 设置定时刷新 - 每30分钟刷新一次
  const refreshInterval = setInterval(fetchSmartData, 30 * 60 * 1000);
  
  return () => clearInterval(refreshInterval);
}, [selectedDisk, language]);
```

#### 3.4 健康状态评估

```javascript
// 评估硬盘健康状态
const evaluateHealth = (smartData) => {
  if (!smartData || !smartData.attributes) {
    return { status: 'unknown', criticalCount: 0, warningCount: 0 };
  }
  
  // 这里简化了健康状态评估逻辑，实际应用中应该根据各种SMART属性进行综合评估
  const criticalAttributes = smartData.attributes.filter(attr =>
    attr.status === 'critical' || (attr.threshold > 0 && attr.value <= attr.threshold)
  );
  
  const warningAttributes = smartData.attributes.filter(attr =>
    attr.status === 'warning' || (attr.threshold > 0 && attr.value <= attr.threshold * 1.2)
  );
  
  if (criticalAttributes.length > 0) {
    return { status: 'critical', criticalCount: criticalAttributes.length, warningCount: warningAttributes.length, criticalAttributes };
  }
  
  if (warningAttributes.length > 0) {
    return { status: 'warning', criticalCount: 0, warningCount: warningAttributes.length, warningAttributes };
  }
  
  return { status: 'good', criticalCount: 0, warningCount: 0 };
};
```

#### 3.5 UI 渲染

```jsx
return (
  <StyledContainer>
    <StyledHeader>
      <Title level={2}>
        <HddOutlined /> 硬盘SMART信息
      </Title>
      <HeaderControls>
        {/* 磁盘选择器 */}
        <Select 
          value={selectedDisk} 
          onChange={setSelectedDisk}
          style={{ width: 300 }}
          loading={loading && !disks.length}
        >
          {disks.map(disk => (
            <Option key={disk.mounted} value={disk.mounted}>
              {formatDiskName(disk)}
            </Option>
          ))}
        </Select>
        
        {/* 刷新按钮 */}
        <Button 
          icon={<ReloadOutlined />} 
          onClick={refreshSmartData}
          loading={loading}
        >
          刷新
        </Button>
        
        {/* 语言切换 */}
        <Button onClick={toggleLanguage}>
          {language === 'zh' ? 'English' : '中文'}
        </Button>
      </HeaderControls>
    </StyledHeader>
    
    {/* 加载中状态 */}
    {loading && !smartData ? (
      <Spin size="large" tip="读取SMART信息中..." />
    ) : smartData ? (
      smartData.error ? (
        /* 错误信息显示 */
        <ErrorMessage>
          <WarningOutlined />
          {smartData.error}
        </ErrorMessage>
      ) : (
        /* SMART数据显示 */
        <StyledContent>
          {/* 基本信息 */}
          <StyledCard title="基本信息">
            <InfoItem>
              <span className="label">型号:</span>
              <span className="value">{smartData.model || '未知'}</span>
            </InfoItem>
            {/* 其他基本信息项 */}
          </StyledCard>
          
          {/* SMART属性表格 */}
          <StyledCard title="SMART详细属性">
            {smartData.attributes && smartData.attributes.length > 0 ? (
              <Table 
                columns={columns} 
                dataSource={smartData.attributes.map(attr => ({ ...attr, key: attr.id }))}
                pagination={false}
                size="middle"
              />
            ) : (
              <EmptyData>无SMART属性数据</EmptyData>
            )}
          </StyledCard>
        </StyledContent>
      )
    ) : (
      /* 无数据状态 */
      <NoData>
        {selectedDisk ? '无法获取SMART信息，请确保您有足够的权限' : '请选择一个磁盘'}
      </NoData>
    )}
  </StyledContainer>
);
```

### 4. 后端实现 (smartService.js)

#### 4.1 IPC 处理器

在 main.js 中注册 IPC 处理器：

```javascript
// SMART数据相关
ipcMain.handle('get-smart-data', smartService.getSmartData);
```

#### 4.2 获取 SMART 数据

```javascript
// 获取硬盘SMART信息
async function getSmartData(event, diskPath, language = 'zh') {
  try {
    // 获取磁盘布局信息
    const diskLayout = await si.diskLayout();
    
    // 获取文件系统信息
    const fsSize = await si.fsSize();
    
    // 查找对应的磁盘
    let disk = null;
    
    // 在Windows上，通过盘符匹配物理磁盘
    if (process.platform === 'win32') {
      const driveLetter = diskPath.charAt(0).toUpperCase();
      
      // 遍历所有物理磁盘进行匹配
      for (const physicalDisk of diskLayout) {
        // 匹配逻辑
        // ...
      }
    } else {
      // 在非Windows系统上使用挂载点匹配
      disk = diskLayout.find(d => d.mount === diskPath);
    }
    
    if (!disk) {
      return { error: '未找到对应的磁盘' };
    }
    
    // 获取SMART数据
    // 根据硬盘类型采用不同处理方式
    if (disk.interfaceType === 'NVMe') {
      // 使用smartctl命令获取NVMe SMART数据
      // ...
    } else if (disk.smartData && disk.smartData.ata_smart_attributes) {
      // 处理SATA硬盘的SMART数据
      // ...
    }
    
    // 返回处理后的数据
    return smartData;
  } catch (error) {
    return { error: error.message };
  }
}
```

#### 4.3 NVMe 硬盘 SMART 数据处理

```javascript
// 使用smartctl命令获取NVMe SMART信息
const smartctlCommand = `smartctl -a -d nvme /dev/sdb`;

return new Promise((resolve, reject) => {
  exec(smartctlCommand, (error, stdout, stderr) => {
    if (error) {
      // 处理错误
      return;
    }
    
    try {
      // 解析smartctl的输出
      const lines = stdout.split('\n');
      const healthInfo = {};
      
      // 解析各项指标
      lines.forEach(line => {
        // 解析各类属性信息
        // ...
      });
      
      // 构建标准化的SMART数据对象
      const smartData = {
        // 基本信息
        device: disk.device,
        model: disk.name,
        // ...
        
        // SMART属性列表
        attributes: [
          // 各种属性
          // ...
        ]
      };
      
      // 计算整体健康状态
      // ...
      
      resolve(smartData);
    } catch (parseError) {
      resolve({
        // 错误信息
        error: '解析smartctl输出失败: ' + parseError.message
      });
    }
  });
});
```

#### 4.4 SATA 硬盘 SMART 数据处理

```javascript
// 处理SATA硬盘的SMART数据
const attributes = disk.smartData.ata_smart_attributes.table.map(attr => {
  const name = attr.name || 'Unknown_Attribute';
  return {
    id: attr.id,
    name: name,
    name_zh: smartAttributesLocale[name] || name, // 多语言支持
    value: attr.value,
    worst: attr.worst,
    threshold: attr.thresh,
    raw: attr.raw.value,
    status: evaluateAttributeStatus(attr) // 评估属性状态
  };
});
smartData.attributes = attributes;
```

#### 4.5 属性状态评估

```javascript
// 评估SMART属性状态
function evaluateAttributeStatus(attr) {
  if (!attr || typeof attr.value !== 'number') {
    return 'unknown';
  }

  // 对于温度属性，使用特殊判断
  if (attr.name === 'Temperature_Celsius' || attr.name === 'Temperature') {
    const temp = parseInt(attr.raw.value);
    if (temp > 60) return 'critical';
    if (temp > 50) return 'warning';
    return 'ok';
  }

  // 对于其他属性，根据值和阈值判断
  if (attr.thresh !== undefined && attr.value <= attr.thresh) {
    return 'critical';
  }
  
  // 如果当前值比最差值低，说明有下降趋势
  if (attr.worst !== undefined && attr.value < attr.worst) {
    return 'warning';
  }
  
  // 对于特定属性使用特殊判断
  switch (attr.name) {
    case 'Raw_Read_Error_Rate':
    case 'Reallocated_Sector_Ct':
      if (parseInt(attr.raw.value) > 0) {
        return 'warning';
      }
      break;
    // 其他特殊属性处理...
  }
  
  return 'ok';
}
```

## 多语言支持

项目实现了对 SMART 属性名称的多语言支持：

```javascript
// SMART属性中英文映射
const smartAttributesLocale = {
  'Raw_Read_Error_Rate': '原始读取错误率',
  'Reallocated_Sector_Ct': '重分配扇区计数',
  'Power_On_Hours': '通电时间',
  'Power_Cycle_Count': '通电次数',
  // 其他属性...
};
```

前端可以通过语言切换按钮在中英文之间切换：

```javascript
// 切换语言
const toggleLanguage = async () => {
  const newLanguage = language === 'zh' ? 'en' : 'zh';
  setLanguage(newLanguage);
  await refreshSmartData();
};
```

## 定时刷新机制

页面实现了 SMART 数据的定时刷新功能，默认每 30 分钟自动刷新一次：

```javascript
// 设置定时刷新 - 每30分钟刷新一次
const refreshInterval = setInterval(fetchSmartData, 30 * 60 * 1000);

// 组件卸载时清除定时器
return () => clearInterval(refreshInterval);
```

同时也提供了手动刷新按钮，用户可以随时更新数据：

```javascript
// 手动刷新SMART信息
const refreshSmartData = async () => {
  if (!selectedDisk) return;
  
  setLoading(true);
  try {
    // 调用主进程获取最新数据
    const data = await ipcRenderer.invoke('get-smart-data', selectedDisk, language);
    // 处理数据...
  } catch (error) {
    // 处理错误...
  } finally {
    setLoading(false);
  }
};
```

## 优化与注意事项

1. **错误处理**：实现了完善的错误处理机制，确保在各种情况下都能给用户提供友好的反馈
2. **资源优化**：使用合理的刷新间隔，避免频繁请求导致性能问题
3. **权限检查**：对需要管理员权限的操作提供了友好提示
4. **兼容性**：针对不同类型的硬盘(SATA/NVMe)采用不同的处理方式
5. **用户体验**：加载状态显示、错误提示等都经过精心设计

## 参考资料

- [SMART 属性说明](https://en.wikipedia.org/wiki/S.M.A.R.T.)
- [NVMe 规范](https://nvmexpress.org/specifications/)
- [smartctl 使用手册](https://www.smartmontools.org/browser/trunk/smartmontools/smartctl.8.in)
- [systeminformation 文档](https://systeminformation.io/) 