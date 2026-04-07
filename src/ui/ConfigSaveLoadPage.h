#ifndef CONFIGSAVELOADPAGE_H
#define CONFIGSAVELOADPAGE_H

#include <QWidget>
#include "core/ConfigManager.h"

class QListWidget;
class QLineEdit;
class QLabel;

/// @brief 保存/加载配置标签页
///
/// 管理配置方案的保存、加载、删除，显示已保存配置列表
class ConfigSaveLoadPage : public QWidget
{
    Q_OBJECT

public:
    explicit ConfigSaveLoadPage(Core::ConfigManager *configMgr, QWidget *parent = nullptr);

    void loadFromConfig();

private slots:
    void onSaveConfig();
    void onLoadConfig();
    void onDeleteConfig();
    void onRefreshList();
    void onExportToJson();
    void onImportFromJson();

private:
    void setupUi();
    void refreshConfigList();

    Core::ConfigManager *m_configMgr = nullptr;

    QListWidget *m_configListWidget = nullptr;
    QLineEdit   *m_configNameEdit   = nullptr;
    QLabel      *m_infoLabel        = nullptr;
};

#endif // CONFIGSAVELOADPAGE_H
