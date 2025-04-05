cmake_minimum_required(VERSION 3.5)

project(DiskToolbox VERSION 0.1 LANGUAGES CXX)

# 自动处理UI、MOC和资源文件
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# 设置C++标准为C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 设置Qt路径
set(QT_DIR "D:/Qt/5.15.2/mingw81_64")
set(CMAKE_PREFIX_PATH ${QT_DIR})

# 查找Qt组件
find_package(Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt5 COMPONENTS Charts REQUIRED)
find_package(Qt5 COMPONENTS Core REQUIRED)
find_package(Qt5 COMPONENTS Gui REQUIRED)
find_package(Qt5 COMPONENTS Network REQUIRED)
find_package(Qt5 COMPONENTS Xml REQUIRED)
find_package(Qt5 COMPONENTS Sql REQUIRED)
find_package(Qt5 COMPONENTS Concurrent REQUIRED)

# 源文件
set(PROJECT_SOURCES
    src/main.cpp
    src/mainwindow.cpp
    src/mainwindow.h
    src/dashboard/dashboardwidget.cpp
    src/dashboard/dashboardwidget.h
    src/diskinfo/diskinfowidget.cpp
    src/diskinfo/diskinfowidget.h
    src/speedtest/speedtestwidget.cpp
    src/speedtest/speedtestwidget.h
    src/smart/smartwidget.cpp
    src/smart/smartwidget.h
    src/spaceanalyzer/spaceanalyzerwidget.cpp
    src/spaceanalyzer/spaceanalyzerwidget.h
    src/core/diskutils.cpp
    src/core/diskutils.h
    src/core/smartdata.cpp
    src/core/smartdata.h
    src/core/satadata.cpp
    src/core/satadata.h
    src/core/nvmedata.cpp
    src/core/nvmedata.h
    src/core/smartfactory.cpp
    src/core/smartfactory.h
    src/core/diskdetector.cpp
    src/core/diskdetector.h
    src/core/smartattributedict.h
    resources.qrc
)

# 创建可执行文件
add_executable(DiskToolbox ${PROJECT_SOURCES})

# 包含头文件目录
target_include_directories(DiskToolbox PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core
    ${CMAKE_CURRENT_SOURCE_DIR}/src/dashboard
    ${CMAKE_CURRENT_SOURCE_DIR}/src/diskinfo
    ${CMAKE_CURRENT_SOURCE_DIR}/src/speedtest
    ${CMAKE_CURRENT_SOURCE_DIR}/src/smart
    ${CMAKE_CURRENT_SOURCE_DIR}/src/spaceanalyzer
)

# 链接Qt库
target_link_libraries(DiskToolbox
    Qt5::Widgets
    Qt5::Charts
    Qt5::Core
    Qt5::Gui
    Qt5::Network
    Qt5::Xml
    Qt5::Sql
    Qt5::Concurrent
)

# Windows特定链接
if(WIN32)
    target_link_libraries(DiskToolbox 
        advapi32
        setupapi
        version
    )
endif()

# 安装规则
install(TARGETS DiskToolbox DESTINATION bin) 