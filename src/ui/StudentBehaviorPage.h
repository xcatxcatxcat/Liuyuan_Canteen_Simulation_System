#ifndef STUDENTBEHAVIORPAGE_H
#define STUDENTBEHAVIORPAGE_H

#include <QWidget>
#include "core/ConfigManager.h"

class QSpinBox;
class QDoubleSpinBox;
class QComboBox;
class QTableWidget;

/// @brief 学生行为配置标签页
///
/// 配置：总学生数、高峰时段人流分布、窗口选择偏好、就餐时长分布、座位选择策略
class StudentBehaviorPage : public QWidget
{
    Q_OBJECT

public:
    explicit StudentBehaviorPage(Core::ConfigManager *configMgr, QWidget *parent = nullptr);

    void loadFromConfig();
    void saveToConfig();

private slots:
    void onAddTimePoint();
    void onRemoveTimePoint();
    void onDistCellChanged(int row, int col);

private:
    void setupUi();

    Core::ConfigManager *m_configMgr = nullptr;

    // 总学生数
    QSpinBox *m_totalStudentsSpin = nullptr;

    // 抵达分布表格
    QTableWidget *m_distTable = nullptr;

    // 窗口选择偏好
    QComboBox *m_windowPrefCombo = nullptr;

    // 座位选择策略
    QComboBox *m_seatPrefCombo = nullptr;

    // 就餐时长
    QDoubleSpinBox *m_mealMeanSpin = nullptr;
    QDoubleSpinBox *m_mealStddevSpin = nullptr;

    bool m_updatingFromConfig = false;
};

#endif // STUDENTBEHAVIORPAGE_H
