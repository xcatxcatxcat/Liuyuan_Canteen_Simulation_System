#ifndef TEACHINGBUILDINGPAGE_H
#define TEACHINGBUILDINGPAGE_H

#include <QWidget>
#include "core/ConfigManager.h"

class QTableWidget;
class QComboBox;
class QLabel;

/// @brief 教学楼与排课配置标签页
///
/// 配置：教学楼列表、下课时间表、错峰方案预设
class TeachingBuildingPage : public QWidget
{
    Q_OBJECT

public:
    explicit TeachingBuildingPage(Core::ConfigManager *configMgr, QWidget *parent = nullptr);

    void loadFromConfig();
    void saveToConfig();

private slots:
    void onAddBuilding();
    void onRemoveBuilding();
    void onAddSchedule();
    void onRemoveSchedule();
    void onBuildingCellChanged(int row, int col);
    void onCurrentBuildingChanged(int row, int col);
    void onPresetSchemeChanged(int index);

private:
    void setupUi();
    void refreshScheduleTable();

    Core::ConfigManager *m_configMgr = nullptr;

    // 教学楼表格
    QTableWidget *m_buildingTable = nullptr;

    // 下课时间表
    QLabel       *m_currentBuildingLabel = nullptr;
    QTableWidget *m_scheduleTable = nullptr;

    // 错峰方案
    QComboBox    *m_presetCombo = nullptr;

    bool m_updatingFromConfig = false;
};

#endif // TEACHINGBUILDINGPAGE_H
