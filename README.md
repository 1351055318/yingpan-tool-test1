# 硬盘工具箱

基于Qt C++开发的现代化硬盘工具箱，提供硬盘监控、性能测试和健康管理功能。

## 功能特点

- **科技感UI**: 采用深色主题，带有现代化科技感的界面设计
- **硬盘仪表盘**: 可视化监控硬盘使用率、温度、健康状态、性能等关键指标
- **硬盘信息**: 显示硬盘详细信息，包括型号、序列号、固件版本等
- **速度测试**: 提供读写速度测试功能，支持不同块大小和测试文件大小设置
- **SMART信息**: 显示硬盘SMART健康信息，监控潜在故障
- **空间分析**: 分析硬盘空间使用情况，帮助找出大文件和文件夹
- **健康监控**: 长期监控硬盘健康状态，提供趋势分析

## 技术细节

- **开发语言**: C++17
- **GUI框架**: Qt 5.15.2
- **图表库**: Qt Charts
- **系统支持**: Windows 10/11 (主要支持)

## 系统要求

- Windows 10或更高版本
- 最低1GB内存
- 100MB硬盘空间
- 管理员权限 (用于访问系统硬盘信息)

## 编译方法

### 准备工作

1. 安装Qt 5.15.2
2. 安装MinGW 8.1 64位编译器（推荐）或Visual Studio 2019或更高版本
3. 安装CMake 4.0.0或更高版本

### 编译步骤

#### 使用MinGW编译（推荐）

1. 克隆仓库
```
git clone https://github.com/yourusername/disk-toolbox.git
cd disk-toolbox
```

2. 创建构建目录
```
mkdir build
cd build
```

3. 配置项目（使用MinGW）
```
cmake .. -G "MinGW Makefiles"
```

4. 编译项目
```
mingw32-make
```

5. 运行应用程序
```
./DiskToolbox
```

#### 使用Visual Studio编译（备选）

1. 克隆仓库后，创建构建目录
```
mkdir build
cd build
```

2. 配置项目（使用MSVC）
```
cmake .. -G "Visual Studio 16 2019" -A x64
```

3. 编译项目
```
cmake --build . --config Release
```

4. 运行应用程序
```
.\Release\DiskToolbox.exe
```

## 使用说明

1. **仪表盘**: 主界面显示硬盘关键指标，直观展示硬盘状态。
2. **硬盘信息**: 查看所有硬盘的详细信息，包括物理和逻辑信息。
3. **速度测试**: 选择要测试的硬盘，设定测试参数，点击开始测试按钮。
4. **SMART信息**: 选择硬盘，查看SMART健康属性和健康状态评分。
5. **空间分析**: 选择卷，点击扫描，分析磁盘空间使用情况。
6. **健康监控**: 设置自动监控参数，监控硬盘健康状态变化。

## 许可证

本项目采用MIT许可证。详情请查看[LICENSE](LICENSE)文件。

## 联系方式

如有任何问题或建议，请通过以下方式联系:

- 邮箱: your.email@example.com
- GitHub: [https://github.com/yourusername](https://github.com/yourusername)

## 截图

(项目截图将放在这里) 