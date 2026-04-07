#ifndef CANTEENCONFIGPAGE_H
#define CANTEENCONFIGPAGE_H

#include <QWidget>
#include "core/ConfigManager.h"

class QTableWidget;
class QPushButton;
class QSpinBox;
class QDoubleSpinBox;
class QLineEdit;
class QLabel;

/// @brief 食堂配置标签页
///
/// 管理窗口列表（增删改）、桌子/座位配置、备用窗口设置
class CanteenConfigPage : public QWidget
{
    Q_OBJECT

public:
    explicit CanteenConfigPage(Core::ConfigManager *configMgr, QWidget *parent = nullptr);

    /// 从 ConfigManager 加载当前配置到界面
    void loadFromConfig();

    /// 将界面当前状态写回 ConfigManager
    void saveToConfig();

private slots:
    void onAddWindow();
    void onRemoveWindow();
    void onWindowCellChanged(int row, int col);

private:
    void setupUi();

    Core::ConfigManager *m_configMgr = nullptr;

    // 食堂名称
    QLineEdit *m_canteenNameEdit = nullptr;

    // 窗口表格
    QTableWidget *m_windowTable = nullptr;
    QPushButton  *m_addWindowBtn = nullptr;
    QPushButton  *m_removeWindowBtn = nullptr;

    // 桌子配置
    QSpinBox     *m_tableCountSpin  = nullptr;
    QSpinBox     *m_seatsPerTableSpin = nullptr;
    QLabel       *m_totalSeatsLabel = nullptr;

    // 备用窗口
    QSpinBox     *m_backupThresholdSpin = nullptr;
    QLineEdit    *m_backupNameEdit = nullptr;
    QDoubleSpinBox *m_backupServiceTimeSpin = nullptr;
    QSpinBox     *m_backupQueueLenSpin = nullptr;

    bool m_updatingFromConfig = false;  // 防止循环更新
};

#endif // CANTEENCONFIGPAGE_H
