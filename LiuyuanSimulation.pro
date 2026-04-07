# ============================================================
#  北京交通大学留园食堂就餐仿真系统 - qmake 项目配置
#  技术栈: C++ / Qt 6.7.3 / MSVC 2022 64-bit
#  架构: C/S 桌面应用程序
# ============================================================

QT += core gui widgets

greaterThan(QT_MAJOR_VERSION, 5): QT += widgets

CONFIG += c++17 utf8_source
TARGET = LiuyuanSimulation
TEMPLATE = app

# MSVC 特定编译选项
msvc {
    QMAKE_CXXFLAGS += /utf-8 /W3
    QMAKE_CXXFLAGS_WARN_ON += /wd4267
    QMAKE_LFLAGS += /OPT:REF /OPT:ICF
}

# 包含路径（使 #include "core/xxx.h" 等相对路径生效）
INCLUDEPATH += $$PWD/src

# 输出目录
DESTDIR     = $$PWD/bin
OBJECTS_DIR = $$PWD/build/obj
MOC_DIR     = $$PWD/build/moc
RCC_DIR     = $$PWD/build/rcc
UI_DIR      = $$PWD/build/ui

# ============================================================
#  源文件
# ============================================================

SOURCES += \
    src/main.cpp \
    src/ui/MainWindow.cpp \
    src/ui/CanteenConfigPage.cpp \
    src/ui/StudentBehaviorPage.cpp \
    src/ui/TeachingBuildingPage.cpp \
    src/ui/SimulationParamsPage.cpp \
    src/ui/ConfigSaveLoadPage.cpp \
    src/core/ConfigManager.cpp \
    src/core/DataManager.cpp \
    src/models/Student.cpp \
    src/models/Canteen.cpp \
    src/models/Window.cpp \
    src/models/Seat.cpp \
    src/models/TeachingBuilding.cpp \
    src/models/Types.cpp

HEADERS += \
    src/ui/MainWindow.h \
    src/ui/CanteenConfigPage.h \
    src/ui/StudentBehaviorPage.h \
    src/ui/TeachingBuildingPage.h \
    src/ui/SimulationParamsPage.h \
    src/ui/ConfigSaveLoadPage.h \
    src/core/ConfigManager.h \
    src/core/DataManager.h \
    src/models/Student.h \
    src/models/Canteen.h \
    src/models/Window.h \
    src/models/Seat.h \
    src/models/TeachingBuilding.h \
    src/models/Types.h

# ============================================================
#  资源文件
# ============================================================

RESOURCES += \
    resources/resources.qrc
