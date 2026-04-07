#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include "core/ConfigManager.h"

class QLabel;
class QPushButton;
class QTabWidget;

/// @brief 主配置界面 — 系统启动后的第一个界面
///
/// 布局：左侧导航标签页 + 右侧内容区 + 底部操作栏
/// 包含 5 个配置标签页：
///   食堂配置、学生行为配置、教学楼与排课配置、仿真运行参数、保存/加载配置
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    void onStartSimulation();
    void onResetConfig();
    void onConfigValidationFailed(const QStringList &errors);

private:
    void setupUi();
    void setupMenuBar();
    void setupCentralWidget();
    void setupTabBar();
    void setupStatusBar();
    void setupStyleSheet();

    // 布局组件
    QLabel       *m_titleLabel   = nullptr;
    QTabWidget   *m_tabWidget    = nullptr;
    QPushButton  *m_startBtn     = nullptr;
    QPushButton  *m_resetBtn     = nullptr;
    QLabel       *m_statusLabel  = nullptr;

    // 标签页 Widget（由 setupTabBar 创建）
    QWidget      *m_canteenPage       = nullptr;
    QWidget      *m_studentPage       = nullptr;
    QWidget      *m_teachingPage      = nullptr;
    QWidget      *m_simulationPage    = nullptr;
    QWidget      *m_configIOPage      = nullptr;

    // 配置管理器
    Core::ConfigManager *m_configManager = nullptr;
};

#endif // MAINWINDOW_H
