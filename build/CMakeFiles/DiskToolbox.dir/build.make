# CMAKE generated file: DO NOT EDIT!
# Generated by "MinGW Makefiles" Generator, CMake Version 4.0

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

SHELL = cmd.exe

# The CMake executable.
CMAKE_COMMAND = D:\CMake\bin\cmake.exe

# The command to remove a file.
RM = D:\CMake\bin\cmake.exe -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = D:\trae_project\test31

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = D:\trae_project\test31\build

# Include any dependencies generated for this target.
include CMakeFiles/DiskToolbox.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/DiskToolbox.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/DiskToolbox.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/DiskToolbox.dir/flags.make

DiskToolbox_autogen/timestamp: D:/qt/5.15.2/mingw81_64/bin/moc.exe
DiskToolbox_autogen/timestamp: D:/qt/5.15.2/mingw81_64/bin/uic.exe
DiskToolbox_autogen/timestamp: CMakeFiles/DiskToolbox.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --blue --bold --progress-dir=D:\trae_project\test31\build\CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Automatic MOC and UIC for target DiskToolbox"
	D:\CMake\bin\cmake.exe -E cmake_autogen D:/trae_project/test31/build/CMakeFiles/DiskToolbox_autogen.dir/AutogenInfo.json Release
	D:\CMake\bin\cmake.exe -E touch D:/trae_project/test31/build/DiskToolbox_autogen/timestamp

DiskToolbox_autogen/EWIEGA46WW/qrc_resources.cpp: D:/trae_project/test31/resources.qrc
DiskToolbox_autogen/EWIEGA46WW/qrc_resources.cpp: CMakeFiles/DiskToolbox_autogen.dir/AutoRcc_resources_EWIEGA46WW_Info.json
DiskToolbox_autogen/EWIEGA46WW/qrc_resources.cpp: D:/trae_project/test31/resources/images/splash.png
DiskToolbox_autogen/EWIEGA46WW/qrc_resources.cpp: D:/trae_project/test31/resources/styles/dark.qss
DiskToolbox_autogen/EWIEGA46WW/qrc_resources.cpp: D:/trae_project/test31/resources/styles/light.qss
DiskToolbox_autogen/EWIEGA46WW/qrc_resources.cpp: D:/trae_project/test31/resources/icons/app.png
DiskToolbox_autogen/EWIEGA46WW/qrc_resources.cpp: D:/trae_project/test31/resources/icons/dashboard.png
DiskToolbox_autogen/EWIEGA46WW/qrc_resources.cpp: D:/trae_project/test31/resources/icons/space.png
DiskToolbox_autogen/EWIEGA46WW/qrc_resources.cpp: D:/trae_project/test31/resources/icons/smart.png
DiskToolbox_autogen/EWIEGA46WW/qrc_resources.cpp: D:/trae_project/test31/resources/icons/speed.png
DiskToolbox_autogen/EWIEGA46WW/qrc_resources.cpp: D:/trae_project/test31/resources/icons/disk.png
DiskToolbox_autogen/EWIEGA46WW/qrc_resources.cpp: D:/qt/5.15.2/mingw81_64/bin/rcc.exe
DiskToolbox_autogen/EWIEGA46WW/qrc_resources.cpp: D:/qt/5.15.2/mingw81_64/bin/rcc.exe
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --blue --bold --progress-dir=D:\trae_project\test31\build\CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Automatic RCC for resources.qrc"
	D:\CMake\bin\cmake.exe -E cmake_autorcc D:/trae_project/test31/build/CMakeFiles/DiskToolbox_autogen.dir/AutoRcc_resources_EWIEGA46WW_Info.json Release

CMakeFiles/DiskToolbox.dir/codegen:
.PHONY : CMakeFiles/DiskToolbox.dir/codegen

CMakeFiles/DiskToolbox.dir/DiskToolbox_autogen/mocs_compilation.cpp.obj: CMakeFiles/DiskToolbox.dir/flags.make
CMakeFiles/DiskToolbox.dir/DiskToolbox_autogen/mocs_compilation.cpp.obj: CMakeFiles/DiskToolbox.dir/includes_CXX.rsp
CMakeFiles/DiskToolbox.dir/DiskToolbox_autogen/mocs_compilation.cpp.obj: DiskToolbox_autogen/mocs_compilation.cpp
CMakeFiles/DiskToolbox.dir/DiskToolbox_autogen/mocs_compilation.cpp.obj: CMakeFiles/DiskToolbox.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=D:\trae_project\test31\build\CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object CMakeFiles/DiskToolbox.dir/DiskToolbox_autogen/mocs_compilation.cpp.obj"
	D:\Qt\Tools\mingw810_64\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/DiskToolbox.dir/DiskToolbox_autogen/mocs_compilation.cpp.obj -MF CMakeFiles\DiskToolbox.dir\DiskToolbox_autogen\mocs_compilation.cpp.obj.d -o CMakeFiles\DiskToolbox.dir\DiskToolbox_autogen\mocs_compilation.cpp.obj -c D:\trae_project\test31\build\DiskToolbox_autogen\mocs_compilation.cpp

CMakeFiles/DiskToolbox.dir/DiskToolbox_autogen/mocs_compilation.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/DiskToolbox.dir/DiskToolbox_autogen/mocs_compilation.cpp.i"
	D:\Qt\Tools\mingw810_64\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E D:\trae_project\test31\build\DiskToolbox_autogen\mocs_compilation.cpp > CMakeFiles\DiskToolbox.dir\DiskToolbox_autogen\mocs_compilation.cpp.i

CMakeFiles/DiskToolbox.dir/DiskToolbox_autogen/mocs_compilation.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/DiskToolbox.dir/DiskToolbox_autogen/mocs_compilation.cpp.s"
	D:\Qt\Tools\mingw810_64\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S D:\trae_project\test31\build\DiskToolbox_autogen\mocs_compilation.cpp -o CMakeFiles\DiskToolbox.dir\DiskToolbox_autogen\mocs_compilation.cpp.s

CMakeFiles/DiskToolbox.dir/src/main.cpp.obj: CMakeFiles/DiskToolbox.dir/flags.make
CMakeFiles/DiskToolbox.dir/src/main.cpp.obj: CMakeFiles/DiskToolbox.dir/includes_CXX.rsp
CMakeFiles/DiskToolbox.dir/src/main.cpp.obj: D:/trae_project/test31/src/main.cpp
CMakeFiles/DiskToolbox.dir/src/main.cpp.obj: CMakeFiles/DiskToolbox.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=D:\trae_project\test31\build\CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building CXX object CMakeFiles/DiskToolbox.dir/src/main.cpp.obj"
	D:\Qt\Tools\mingw810_64\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/DiskToolbox.dir/src/main.cpp.obj -MF CMakeFiles\DiskToolbox.dir\src\main.cpp.obj.d -o CMakeFiles\DiskToolbox.dir\src\main.cpp.obj -c D:\trae_project\test31\src\main.cpp

CMakeFiles/DiskToolbox.dir/src/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/DiskToolbox.dir/src/main.cpp.i"
	D:\Qt\Tools\mingw810_64\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E D:\trae_project\test31\src\main.cpp > CMakeFiles\DiskToolbox.dir\src\main.cpp.i

CMakeFiles/DiskToolbox.dir/src/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/DiskToolbox.dir/src/main.cpp.s"
	D:\Qt\Tools\mingw810_64\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S D:\trae_project\test31\src\main.cpp -o CMakeFiles\DiskToolbox.dir\src\main.cpp.s

CMakeFiles/DiskToolbox.dir/src/mainwindow.cpp.obj: CMakeFiles/DiskToolbox.dir/flags.make
CMakeFiles/DiskToolbox.dir/src/mainwindow.cpp.obj: CMakeFiles/DiskToolbox.dir/includes_CXX.rsp
CMakeFiles/DiskToolbox.dir/src/mainwindow.cpp.obj: D:/trae_project/test31/src/mainwindow.cpp
CMakeFiles/DiskToolbox.dir/src/mainwindow.cpp.obj: CMakeFiles/DiskToolbox.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=D:\trae_project\test31\build\CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Building CXX object CMakeFiles/DiskToolbox.dir/src/mainwindow.cpp.obj"
	D:\Qt\Tools\mingw810_64\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/DiskToolbox.dir/src/mainwindow.cpp.obj -MF CMakeFiles\DiskToolbox.dir\src\mainwindow.cpp.obj.d -o CMakeFiles\DiskToolbox.dir\src\mainwindow.cpp.obj -c D:\trae_project\test31\src\mainwindow.cpp

CMakeFiles/DiskToolbox.dir/src/mainwindow.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/DiskToolbox.dir/src/mainwindow.cpp.i"
	D:\Qt\Tools\mingw810_64\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E D:\trae_project\test31\src\mainwindow.cpp > CMakeFiles\DiskToolbox.dir\src\mainwindow.cpp.i

CMakeFiles/DiskToolbox.dir/src/mainwindow.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/DiskToolbox.dir/src/mainwindow.cpp.s"
	D:\Qt\Tools\mingw810_64\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S D:\trae_project\test31\src\mainwindow.cpp -o CMakeFiles\DiskToolbox.dir\src\mainwindow.cpp.s

CMakeFiles/DiskToolbox.dir/src/dashboard/dashboardwidget.cpp.obj: CMakeFiles/DiskToolbox.dir/flags.make
CMakeFiles/DiskToolbox.dir/src/dashboard/dashboardwidget.cpp.obj: CMakeFiles/DiskToolbox.dir/includes_CXX.rsp
CMakeFiles/DiskToolbox.dir/src/dashboard/dashboardwidget.cpp.obj: D:/trae_project/test31/src/dashboard/dashboardwidget.cpp
CMakeFiles/DiskToolbox.dir/src/dashboard/dashboardwidget.cpp.obj: CMakeFiles/DiskToolbox.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=D:\trae_project\test31\build\CMakeFiles --progress-num=$(CMAKE_PROGRESS_6) "Building CXX object CMakeFiles/DiskToolbox.dir/src/dashboard/dashboardwidget.cpp.obj"
	D:\Qt\Tools\mingw810_64\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/DiskToolbox.dir/src/dashboard/dashboardwidget.cpp.obj -MF CMakeFiles\DiskToolbox.dir\src\dashboard\dashboardwidget.cpp.obj.d -o CMakeFiles\DiskToolbox.dir\src\dashboard\dashboardwidget.cpp.obj -c D:\trae_project\test31\src\dashboard\dashboardwidget.cpp

CMakeFiles/DiskToolbox.dir/src/dashboard/dashboardwidget.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/DiskToolbox.dir/src/dashboard/dashboardwidget.cpp.i"
	D:\Qt\Tools\mingw810_64\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E D:\trae_project\test31\src\dashboard\dashboardwidget.cpp > CMakeFiles\DiskToolbox.dir\src\dashboard\dashboardwidget.cpp.i

CMakeFiles/DiskToolbox.dir/src/dashboard/dashboardwidget.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/DiskToolbox.dir/src/dashboard/dashboardwidget.cpp.s"
	D:\Qt\Tools\mingw810_64\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S D:\trae_project\test31\src\dashboard\dashboardwidget.cpp -o CMakeFiles\DiskToolbox.dir\src\dashboard\dashboardwidget.cpp.s

CMakeFiles/DiskToolbox.dir/src/diskinfo/diskinfowidget.cpp.obj: CMakeFiles/DiskToolbox.dir/flags.make
CMakeFiles/DiskToolbox.dir/src/diskinfo/diskinfowidget.cpp.obj: CMakeFiles/DiskToolbox.dir/includes_CXX.rsp
CMakeFiles/DiskToolbox.dir/src/diskinfo/diskinfowidget.cpp.obj: D:/trae_project/test31/src/diskinfo/diskinfowidget.cpp
CMakeFiles/DiskToolbox.dir/src/diskinfo/diskinfowidget.cpp.obj: CMakeFiles/DiskToolbox.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=D:\trae_project\test31\build\CMakeFiles --progress-num=$(CMAKE_PROGRESS_7) "Building CXX object CMakeFiles/DiskToolbox.dir/src/diskinfo/diskinfowidget.cpp.obj"
	D:\Qt\Tools\mingw810_64\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/DiskToolbox.dir/src/diskinfo/diskinfowidget.cpp.obj -MF CMakeFiles\DiskToolbox.dir\src\diskinfo\diskinfowidget.cpp.obj.d -o CMakeFiles\DiskToolbox.dir\src\diskinfo\diskinfowidget.cpp.obj -c D:\trae_project\test31\src\diskinfo\diskinfowidget.cpp

CMakeFiles/DiskToolbox.dir/src/diskinfo/diskinfowidget.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/DiskToolbox.dir/src/diskinfo/diskinfowidget.cpp.i"
	D:\Qt\Tools\mingw810_64\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E D:\trae_project\test31\src\diskinfo\diskinfowidget.cpp > CMakeFiles\DiskToolbox.dir\src\diskinfo\diskinfowidget.cpp.i

CMakeFiles/DiskToolbox.dir/src/diskinfo/diskinfowidget.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/DiskToolbox.dir/src/diskinfo/diskinfowidget.cpp.s"
	D:\Qt\Tools\mingw810_64\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S D:\trae_project\test31\src\diskinfo\diskinfowidget.cpp -o CMakeFiles\DiskToolbox.dir\src\diskinfo\diskinfowidget.cpp.s

CMakeFiles/DiskToolbox.dir/src/speedtest/speedtestwidget.cpp.obj: CMakeFiles/DiskToolbox.dir/flags.make
CMakeFiles/DiskToolbox.dir/src/speedtest/speedtestwidget.cpp.obj: CMakeFiles/DiskToolbox.dir/includes_CXX.rsp
CMakeFiles/DiskToolbox.dir/src/speedtest/speedtestwidget.cpp.obj: D:/trae_project/test31/src/speedtest/speedtestwidget.cpp
CMakeFiles/DiskToolbox.dir/src/speedtest/speedtestwidget.cpp.obj: CMakeFiles/DiskToolbox.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=D:\trae_project\test31\build\CMakeFiles --progress-num=$(CMAKE_PROGRESS_8) "Building CXX object CMakeFiles/DiskToolbox.dir/src/speedtest/speedtestwidget.cpp.obj"
	D:\Qt\Tools\mingw810_64\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/DiskToolbox.dir/src/speedtest/speedtestwidget.cpp.obj -MF CMakeFiles\DiskToolbox.dir\src\speedtest\speedtestwidget.cpp.obj.d -o CMakeFiles\DiskToolbox.dir\src\speedtest\speedtestwidget.cpp.obj -c D:\trae_project\test31\src\speedtest\speedtestwidget.cpp

CMakeFiles/DiskToolbox.dir/src/speedtest/speedtestwidget.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/DiskToolbox.dir/src/speedtest/speedtestwidget.cpp.i"
	D:\Qt\Tools\mingw810_64\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E D:\trae_project\test31\src\speedtest\speedtestwidget.cpp > CMakeFiles\DiskToolbox.dir\src\speedtest\speedtestwidget.cpp.i

CMakeFiles/DiskToolbox.dir/src/speedtest/speedtestwidget.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/DiskToolbox.dir/src/speedtest/speedtestwidget.cpp.s"
	D:\Qt\Tools\mingw810_64\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S D:\trae_project\test31\src\speedtest\speedtestwidget.cpp -o CMakeFiles\DiskToolbox.dir\src\speedtest\speedtestwidget.cpp.s

CMakeFiles/DiskToolbox.dir/src/smart/smartwidget.cpp.obj: CMakeFiles/DiskToolbox.dir/flags.make
CMakeFiles/DiskToolbox.dir/src/smart/smartwidget.cpp.obj: CMakeFiles/DiskToolbox.dir/includes_CXX.rsp
CMakeFiles/DiskToolbox.dir/src/smart/smartwidget.cpp.obj: D:/trae_project/test31/src/smart/smartwidget.cpp
CMakeFiles/DiskToolbox.dir/src/smart/smartwidget.cpp.obj: CMakeFiles/DiskToolbox.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=D:\trae_project\test31\build\CMakeFiles --progress-num=$(CMAKE_PROGRESS_9) "Building CXX object CMakeFiles/DiskToolbox.dir/src/smart/smartwidget.cpp.obj"
	D:\Qt\Tools\mingw810_64\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/DiskToolbox.dir/src/smart/smartwidget.cpp.obj -MF CMakeFiles\DiskToolbox.dir\src\smart\smartwidget.cpp.obj.d -o CMakeFiles\DiskToolbox.dir\src\smart\smartwidget.cpp.obj -c D:\trae_project\test31\src\smart\smartwidget.cpp

CMakeFiles/DiskToolbox.dir/src/smart/smartwidget.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/DiskToolbox.dir/src/smart/smartwidget.cpp.i"
	D:\Qt\Tools\mingw810_64\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E D:\trae_project\test31\src\smart\smartwidget.cpp > CMakeFiles\DiskToolbox.dir\src\smart\smartwidget.cpp.i

CMakeFiles/DiskToolbox.dir/src/smart/smartwidget.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/DiskToolbox.dir/src/smart/smartwidget.cpp.s"
	D:\Qt\Tools\mingw810_64\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S D:\trae_project\test31\src\smart\smartwidget.cpp -o CMakeFiles\DiskToolbox.dir\src\smart\smartwidget.cpp.s

CMakeFiles/DiskToolbox.dir/src/spaceanalyzer/spaceanalyzerwidget.cpp.obj: CMakeFiles/DiskToolbox.dir/flags.make
CMakeFiles/DiskToolbox.dir/src/spaceanalyzer/spaceanalyzerwidget.cpp.obj: CMakeFiles/DiskToolbox.dir/includes_CXX.rsp
CMakeFiles/DiskToolbox.dir/src/spaceanalyzer/spaceanalyzerwidget.cpp.obj: D:/trae_project/test31/src/spaceanalyzer/spaceanalyzerwidget.cpp
CMakeFiles/DiskToolbox.dir/src/spaceanalyzer/spaceanalyzerwidget.cpp.obj: CMakeFiles/DiskToolbox.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=D:\trae_project\test31\build\CMakeFiles --progress-num=$(CMAKE_PROGRESS_10) "Building CXX object CMakeFiles/DiskToolbox.dir/src/spaceanalyzer/spaceanalyzerwidget.cpp.obj"
	D:\Qt\Tools\mingw810_64\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/DiskToolbox.dir/src/spaceanalyzer/spaceanalyzerwidget.cpp.obj -MF CMakeFiles\DiskToolbox.dir\src\spaceanalyzer\spaceanalyzerwidget.cpp.obj.d -o CMakeFiles\DiskToolbox.dir\src\spaceanalyzer\spaceanalyzerwidget.cpp.obj -c D:\trae_project\test31\src\spaceanalyzer\spaceanalyzerwidget.cpp

CMakeFiles/DiskToolbox.dir/src/spaceanalyzer/spaceanalyzerwidget.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/DiskToolbox.dir/src/spaceanalyzer/spaceanalyzerwidget.cpp.i"
	D:\Qt\Tools\mingw810_64\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E D:\trae_project\test31\src\spaceanalyzer\spaceanalyzerwidget.cpp > CMakeFiles\DiskToolbox.dir\src\spaceanalyzer\spaceanalyzerwidget.cpp.i

CMakeFiles/DiskToolbox.dir/src/spaceanalyzer/spaceanalyzerwidget.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/DiskToolbox.dir/src/spaceanalyzer/spaceanalyzerwidget.cpp.s"
	D:\Qt\Tools\mingw810_64\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S D:\trae_project\test31\src\spaceanalyzer\spaceanalyzerwidget.cpp -o CMakeFiles\DiskToolbox.dir\src\spaceanalyzer\spaceanalyzerwidget.cpp.s

CMakeFiles/DiskToolbox.dir/src/core/diskutils.cpp.obj: CMakeFiles/DiskToolbox.dir/flags.make
CMakeFiles/DiskToolbox.dir/src/core/diskutils.cpp.obj: CMakeFiles/DiskToolbox.dir/includes_CXX.rsp
CMakeFiles/DiskToolbox.dir/src/core/diskutils.cpp.obj: D:/trae_project/test31/src/core/diskutils.cpp
CMakeFiles/DiskToolbox.dir/src/core/diskutils.cpp.obj: CMakeFiles/DiskToolbox.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=D:\trae_project\test31\build\CMakeFiles --progress-num=$(CMAKE_PROGRESS_11) "Building CXX object CMakeFiles/DiskToolbox.dir/src/core/diskutils.cpp.obj"
	D:\Qt\Tools\mingw810_64\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/DiskToolbox.dir/src/core/diskutils.cpp.obj -MF CMakeFiles\DiskToolbox.dir\src\core\diskutils.cpp.obj.d -o CMakeFiles\DiskToolbox.dir\src\core\diskutils.cpp.obj -c D:\trae_project\test31\src\core\diskutils.cpp

CMakeFiles/DiskToolbox.dir/src/core/diskutils.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/DiskToolbox.dir/src/core/diskutils.cpp.i"
	D:\Qt\Tools\mingw810_64\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E D:\trae_project\test31\src\core\diskutils.cpp > CMakeFiles\DiskToolbox.dir\src\core\diskutils.cpp.i

CMakeFiles/DiskToolbox.dir/src/core/diskutils.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/DiskToolbox.dir/src/core/diskutils.cpp.s"
	D:\Qt\Tools\mingw810_64\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S D:\trae_project\test31\src\core\diskutils.cpp -o CMakeFiles\DiskToolbox.dir\src\core\diskutils.cpp.s

CMakeFiles/DiskToolbox.dir/src/core/smartdata.cpp.obj: CMakeFiles/DiskToolbox.dir/flags.make
CMakeFiles/DiskToolbox.dir/src/core/smartdata.cpp.obj: CMakeFiles/DiskToolbox.dir/includes_CXX.rsp
CMakeFiles/DiskToolbox.dir/src/core/smartdata.cpp.obj: D:/trae_project/test31/src/core/smartdata.cpp
CMakeFiles/DiskToolbox.dir/src/core/smartdata.cpp.obj: CMakeFiles/DiskToolbox.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=D:\trae_project\test31\build\CMakeFiles --progress-num=$(CMAKE_PROGRESS_12) "Building CXX object CMakeFiles/DiskToolbox.dir/src/core/smartdata.cpp.obj"
	D:\Qt\Tools\mingw810_64\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/DiskToolbox.dir/src/core/smartdata.cpp.obj -MF CMakeFiles\DiskToolbox.dir\src\core\smartdata.cpp.obj.d -o CMakeFiles\DiskToolbox.dir\src\core\smartdata.cpp.obj -c D:\trae_project\test31\src\core\smartdata.cpp

CMakeFiles/DiskToolbox.dir/src/core/smartdata.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/DiskToolbox.dir/src/core/smartdata.cpp.i"
	D:\Qt\Tools\mingw810_64\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E D:\trae_project\test31\src\core\smartdata.cpp > CMakeFiles\DiskToolbox.dir\src\core\smartdata.cpp.i

CMakeFiles/DiskToolbox.dir/src/core/smartdata.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/DiskToolbox.dir/src/core/smartdata.cpp.s"
	D:\Qt\Tools\mingw810_64\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S D:\trae_project\test31\src\core\smartdata.cpp -o CMakeFiles\DiskToolbox.dir\src\core\smartdata.cpp.s

CMakeFiles/DiskToolbox.dir/src/core/satadata.cpp.obj: CMakeFiles/DiskToolbox.dir/flags.make
CMakeFiles/DiskToolbox.dir/src/core/satadata.cpp.obj: CMakeFiles/DiskToolbox.dir/includes_CXX.rsp
CMakeFiles/DiskToolbox.dir/src/core/satadata.cpp.obj: D:/trae_project/test31/src/core/satadata.cpp
CMakeFiles/DiskToolbox.dir/src/core/satadata.cpp.obj: CMakeFiles/DiskToolbox.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=D:\trae_project\test31\build\CMakeFiles --progress-num=$(CMAKE_PROGRESS_13) "Building CXX object CMakeFiles/DiskToolbox.dir/src/core/satadata.cpp.obj"
	D:\Qt\Tools\mingw810_64\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/DiskToolbox.dir/src/core/satadata.cpp.obj -MF CMakeFiles\DiskToolbox.dir\src\core\satadata.cpp.obj.d -o CMakeFiles\DiskToolbox.dir\src\core\satadata.cpp.obj -c D:\trae_project\test31\src\core\satadata.cpp

CMakeFiles/DiskToolbox.dir/src/core/satadata.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/DiskToolbox.dir/src/core/satadata.cpp.i"
	D:\Qt\Tools\mingw810_64\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E D:\trae_project\test31\src\core\satadata.cpp > CMakeFiles\DiskToolbox.dir\src\core\satadata.cpp.i

CMakeFiles/DiskToolbox.dir/src/core/satadata.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/DiskToolbox.dir/src/core/satadata.cpp.s"
	D:\Qt\Tools\mingw810_64\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S D:\trae_project\test31\src\core\satadata.cpp -o CMakeFiles\DiskToolbox.dir\src\core\satadata.cpp.s

CMakeFiles/DiskToolbox.dir/src/core/nvmedata.cpp.obj: CMakeFiles/DiskToolbox.dir/flags.make
CMakeFiles/DiskToolbox.dir/src/core/nvmedata.cpp.obj: CMakeFiles/DiskToolbox.dir/includes_CXX.rsp
CMakeFiles/DiskToolbox.dir/src/core/nvmedata.cpp.obj: D:/trae_project/test31/src/core/nvmedata.cpp
CMakeFiles/DiskToolbox.dir/src/core/nvmedata.cpp.obj: CMakeFiles/DiskToolbox.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=D:\trae_project\test31\build\CMakeFiles --progress-num=$(CMAKE_PROGRESS_14) "Building CXX object CMakeFiles/DiskToolbox.dir/src/core/nvmedata.cpp.obj"
	D:\Qt\Tools\mingw810_64\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/DiskToolbox.dir/src/core/nvmedata.cpp.obj -MF CMakeFiles\DiskToolbox.dir\src\core\nvmedata.cpp.obj.d -o CMakeFiles\DiskToolbox.dir\src\core\nvmedata.cpp.obj -c D:\trae_project\test31\src\core\nvmedata.cpp

CMakeFiles/DiskToolbox.dir/src/core/nvmedata.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/DiskToolbox.dir/src/core/nvmedata.cpp.i"
	D:\Qt\Tools\mingw810_64\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E D:\trae_project\test31\src\core\nvmedata.cpp > CMakeFiles\DiskToolbox.dir\src\core\nvmedata.cpp.i

CMakeFiles/DiskToolbox.dir/src/core/nvmedata.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/DiskToolbox.dir/src/core/nvmedata.cpp.s"
	D:\Qt\Tools\mingw810_64\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S D:\trae_project\test31\src\core\nvmedata.cpp -o CMakeFiles\DiskToolbox.dir\src\core\nvmedata.cpp.s

CMakeFiles/DiskToolbox.dir/src/core/smartfactory.cpp.obj: CMakeFiles/DiskToolbox.dir/flags.make
CMakeFiles/DiskToolbox.dir/src/core/smartfactory.cpp.obj: CMakeFiles/DiskToolbox.dir/includes_CXX.rsp
CMakeFiles/DiskToolbox.dir/src/core/smartfactory.cpp.obj: D:/trae_project/test31/src/core/smartfactory.cpp
CMakeFiles/DiskToolbox.dir/src/core/smartfactory.cpp.obj: CMakeFiles/DiskToolbox.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=D:\trae_project\test31\build\CMakeFiles --progress-num=$(CMAKE_PROGRESS_15) "Building CXX object CMakeFiles/DiskToolbox.dir/src/core/smartfactory.cpp.obj"
	D:\Qt\Tools\mingw810_64\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/DiskToolbox.dir/src/core/smartfactory.cpp.obj -MF CMakeFiles\DiskToolbox.dir\src\core\smartfactory.cpp.obj.d -o CMakeFiles\DiskToolbox.dir\src\core\smartfactory.cpp.obj -c D:\trae_project\test31\src\core\smartfactory.cpp

CMakeFiles/DiskToolbox.dir/src/core/smartfactory.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/DiskToolbox.dir/src/core/smartfactory.cpp.i"
	D:\Qt\Tools\mingw810_64\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E D:\trae_project\test31\src\core\smartfactory.cpp > CMakeFiles\DiskToolbox.dir\src\core\smartfactory.cpp.i

CMakeFiles/DiskToolbox.dir/src/core/smartfactory.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/DiskToolbox.dir/src/core/smartfactory.cpp.s"
	D:\Qt\Tools\mingw810_64\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S D:\trae_project\test31\src\core\smartfactory.cpp -o CMakeFiles\DiskToolbox.dir\src\core\smartfactory.cpp.s

CMakeFiles/DiskToolbox.dir/src/core/diskdetector.cpp.obj: CMakeFiles/DiskToolbox.dir/flags.make
CMakeFiles/DiskToolbox.dir/src/core/diskdetector.cpp.obj: CMakeFiles/DiskToolbox.dir/includes_CXX.rsp
CMakeFiles/DiskToolbox.dir/src/core/diskdetector.cpp.obj: D:/trae_project/test31/src/core/diskdetector.cpp
CMakeFiles/DiskToolbox.dir/src/core/diskdetector.cpp.obj: CMakeFiles/DiskToolbox.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=D:\trae_project\test31\build\CMakeFiles --progress-num=$(CMAKE_PROGRESS_16) "Building CXX object CMakeFiles/DiskToolbox.dir/src/core/diskdetector.cpp.obj"
	D:\Qt\Tools\mingw810_64\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/DiskToolbox.dir/src/core/diskdetector.cpp.obj -MF CMakeFiles\DiskToolbox.dir\src\core\diskdetector.cpp.obj.d -o CMakeFiles\DiskToolbox.dir\src\core\diskdetector.cpp.obj -c D:\trae_project\test31\src\core\diskdetector.cpp

CMakeFiles/DiskToolbox.dir/src/core/diskdetector.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/DiskToolbox.dir/src/core/diskdetector.cpp.i"
	D:\Qt\Tools\mingw810_64\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E D:\trae_project\test31\src\core\diskdetector.cpp > CMakeFiles\DiskToolbox.dir\src\core\diskdetector.cpp.i

CMakeFiles/DiskToolbox.dir/src/core/diskdetector.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/DiskToolbox.dir/src/core/diskdetector.cpp.s"
	D:\Qt\Tools\mingw810_64\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S D:\trae_project\test31\src\core\diskdetector.cpp -o CMakeFiles\DiskToolbox.dir\src\core\diskdetector.cpp.s

CMakeFiles/DiskToolbox.dir/DiskToolbox_autogen/EWIEGA46WW/qrc_resources.cpp.obj: CMakeFiles/DiskToolbox.dir/flags.make
CMakeFiles/DiskToolbox.dir/DiskToolbox_autogen/EWIEGA46WW/qrc_resources.cpp.obj: CMakeFiles/DiskToolbox.dir/includes_CXX.rsp
CMakeFiles/DiskToolbox.dir/DiskToolbox_autogen/EWIEGA46WW/qrc_resources.cpp.obj: DiskToolbox_autogen/EWIEGA46WW/qrc_resources.cpp
CMakeFiles/DiskToolbox.dir/DiskToolbox_autogen/EWIEGA46WW/qrc_resources.cpp.obj: CMakeFiles/DiskToolbox.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=D:\trae_project\test31\build\CMakeFiles --progress-num=$(CMAKE_PROGRESS_17) "Building CXX object CMakeFiles/DiskToolbox.dir/DiskToolbox_autogen/EWIEGA46WW/qrc_resources.cpp.obj"
	D:\Qt\Tools\mingw810_64\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/DiskToolbox.dir/DiskToolbox_autogen/EWIEGA46WW/qrc_resources.cpp.obj -MF CMakeFiles\DiskToolbox.dir\DiskToolbox_autogen\EWIEGA46WW\qrc_resources.cpp.obj.d -o CMakeFiles\DiskToolbox.dir\DiskToolbox_autogen\EWIEGA46WW\qrc_resources.cpp.obj -c D:\trae_project\test31\build\DiskToolbox_autogen\EWIEGA46WW\qrc_resources.cpp

CMakeFiles/DiskToolbox.dir/DiskToolbox_autogen/EWIEGA46WW/qrc_resources.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/DiskToolbox.dir/DiskToolbox_autogen/EWIEGA46WW/qrc_resources.cpp.i"
	D:\Qt\Tools\mingw810_64\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E D:\trae_project\test31\build\DiskToolbox_autogen\EWIEGA46WW\qrc_resources.cpp > CMakeFiles\DiskToolbox.dir\DiskToolbox_autogen\EWIEGA46WW\qrc_resources.cpp.i

CMakeFiles/DiskToolbox.dir/DiskToolbox_autogen/EWIEGA46WW/qrc_resources.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/DiskToolbox.dir/DiskToolbox_autogen/EWIEGA46WW/qrc_resources.cpp.s"
	D:\Qt\Tools\mingw810_64\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S D:\trae_project\test31\build\DiskToolbox_autogen\EWIEGA46WW\qrc_resources.cpp -o CMakeFiles\DiskToolbox.dir\DiskToolbox_autogen\EWIEGA46WW\qrc_resources.cpp.s

# Object files for target DiskToolbox
DiskToolbox_OBJECTS = \
"CMakeFiles/DiskToolbox.dir/DiskToolbox_autogen/mocs_compilation.cpp.obj" \
"CMakeFiles/DiskToolbox.dir/src/main.cpp.obj" \
"CMakeFiles/DiskToolbox.dir/src/mainwindow.cpp.obj" \
"CMakeFiles/DiskToolbox.dir/src/dashboard/dashboardwidget.cpp.obj" \
"CMakeFiles/DiskToolbox.dir/src/diskinfo/diskinfowidget.cpp.obj" \
"CMakeFiles/DiskToolbox.dir/src/speedtest/speedtestwidget.cpp.obj" \
"CMakeFiles/DiskToolbox.dir/src/smart/smartwidget.cpp.obj" \
"CMakeFiles/DiskToolbox.dir/src/spaceanalyzer/spaceanalyzerwidget.cpp.obj" \
"CMakeFiles/DiskToolbox.dir/src/core/diskutils.cpp.obj" \
"CMakeFiles/DiskToolbox.dir/src/core/smartdata.cpp.obj" \
"CMakeFiles/DiskToolbox.dir/src/core/satadata.cpp.obj" \
"CMakeFiles/DiskToolbox.dir/src/core/nvmedata.cpp.obj" \
"CMakeFiles/DiskToolbox.dir/src/core/smartfactory.cpp.obj" \
"CMakeFiles/DiskToolbox.dir/src/core/diskdetector.cpp.obj" \
"CMakeFiles/DiskToolbox.dir/DiskToolbox_autogen/EWIEGA46WW/qrc_resources.cpp.obj"

# External object files for target DiskToolbox
DiskToolbox_EXTERNAL_OBJECTS =

DiskToolbox.exe: CMakeFiles/DiskToolbox.dir/DiskToolbox_autogen/mocs_compilation.cpp.obj
DiskToolbox.exe: CMakeFiles/DiskToolbox.dir/src/main.cpp.obj
DiskToolbox.exe: CMakeFiles/DiskToolbox.dir/src/mainwindow.cpp.obj
DiskToolbox.exe: CMakeFiles/DiskToolbox.dir/src/dashboard/dashboardwidget.cpp.obj
DiskToolbox.exe: CMakeFiles/DiskToolbox.dir/src/diskinfo/diskinfowidget.cpp.obj
DiskToolbox.exe: CMakeFiles/DiskToolbox.dir/src/speedtest/speedtestwidget.cpp.obj
DiskToolbox.exe: CMakeFiles/DiskToolbox.dir/src/smart/smartwidget.cpp.obj
DiskToolbox.exe: CMakeFiles/DiskToolbox.dir/src/spaceanalyzer/spaceanalyzerwidget.cpp.obj
DiskToolbox.exe: CMakeFiles/DiskToolbox.dir/src/core/diskutils.cpp.obj
DiskToolbox.exe: CMakeFiles/DiskToolbox.dir/src/core/smartdata.cpp.obj
DiskToolbox.exe: CMakeFiles/DiskToolbox.dir/src/core/satadata.cpp.obj
DiskToolbox.exe: CMakeFiles/DiskToolbox.dir/src/core/nvmedata.cpp.obj
DiskToolbox.exe: CMakeFiles/DiskToolbox.dir/src/core/smartfactory.cpp.obj
DiskToolbox.exe: CMakeFiles/DiskToolbox.dir/src/core/diskdetector.cpp.obj
DiskToolbox.exe: CMakeFiles/DiskToolbox.dir/DiskToolbox_autogen/EWIEGA46WW/qrc_resources.cpp.obj
DiskToolbox.exe: CMakeFiles/DiskToolbox.dir/build.make
DiskToolbox.exe: D:/qt/5.15.2/mingw81_64/lib/libQt5Charts.a
DiskToolbox.exe: D:/qt/5.15.2/mingw81_64/lib/libQt5Network.a
DiskToolbox.exe: D:/qt/5.15.2/mingw81_64/lib/libQt5Xml.a
DiskToolbox.exe: D:/qt/5.15.2/mingw81_64/lib/libQt5Sql.a
DiskToolbox.exe: D:/qt/5.15.2/mingw81_64/lib/libQt5Concurrent.a
DiskToolbox.exe: D:/qt/5.15.2/mingw81_64/lib/libQt5Widgets.a
DiskToolbox.exe: D:/qt/5.15.2/mingw81_64/lib/libQt5Gui.a
DiskToolbox.exe: D:/qt/5.15.2/mingw81_64/lib/libQt5Core.a
DiskToolbox.exe: CMakeFiles/DiskToolbox.dir/linkLibs.rsp
DiskToolbox.exe: CMakeFiles/DiskToolbox.dir/objects1.rsp
DiskToolbox.exe: CMakeFiles/DiskToolbox.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --bold --progress-dir=D:\trae_project\test31\build\CMakeFiles --progress-num=$(CMAKE_PROGRESS_18) "Linking CXX executable DiskToolbox.exe"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles\DiskToolbox.dir\link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/DiskToolbox.dir/build: DiskToolbox.exe
.PHONY : CMakeFiles/DiskToolbox.dir/build

CMakeFiles/DiskToolbox.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles\DiskToolbox.dir\cmake_clean.cmake
.PHONY : CMakeFiles/DiskToolbox.dir/clean

CMakeFiles/DiskToolbox.dir/depend: DiskToolbox_autogen/EWIEGA46WW/qrc_resources.cpp
CMakeFiles/DiskToolbox.dir/depend: DiskToolbox_autogen/timestamp
	$(CMAKE_COMMAND) -E cmake_depends "MinGW Makefiles" D:\trae_project\test31 D:\trae_project\test31 D:\trae_project\test31\build D:\trae_project\test31\build D:\trae_project\test31\build\CMakeFiles\DiskToolbox.dir\DependInfo.cmake "--color=$(COLOR)"
.PHONY : CMakeFiles/DiskToolbox.dir/depend

