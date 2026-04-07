#ifndef SIMULATIONPARAMSPAGE_H
#define SIMULATIONPARAMSPAGE_H

#include <QWidget>
#include <qlabel.h>
#include "core/ConfigManager.h"

class QSpinBox;
class QTimeEdit;
class QComboBox;

/// @brief 仿真运行参数标签页
///
/// 配置：仿真开始时间、仿真时长、时间步长、运行模式、日志级别
class SimulationParamsPage : public QWidget
{
    Q_OBJECT

public:
    explicit SimulationParamsPage(Core::ConfigManager *configMgr, QWidget *parent = nullptr);

    void loadFromConfig();
    void saveToConfig();

private slots:
    void onDurationChanged(int seconds);

private:
    void setupUi();

    Core::ConfigManager *m_configMgr = nullptr;

    QTimeEdit  *m_startTimeEdit   = nullptr;
    QSpinBox   *m_durationSpin    = nullptr;
    QLabel     *m_endTimeLabel    = nullptr;
    QSpinBox   *m_timeStepSpin    = nullptr;
    QLabel     *m_ratioLabel      = nullptr;
    QComboBox  *m_runModeCombo    = nullptr;
    QComboBox  *m_logLevelCombo   = nullptr;

    bool m_updatingFromConfig = false;
};

#endif // SIMULATIONPARAMSPAGE_H
