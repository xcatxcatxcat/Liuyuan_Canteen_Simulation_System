#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_H

#include <QObject>
#include <QString>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include "models/Types.h"

namespace Core {

/// @brief 窗口配置数据结构
struct WindowConfig {
    QString windowName;
    double  avgServiceTime = 30.0;   // 秒/人
    int     staffCount     = 1;
    int     maxQueueLength = 20;
};

/// @brief 桌子配置数据结构
struct TableConfig {
    int tableId   = 0;
    int seatCount = 4;
};

/// @brief 备用窗口配置
struct BackupWindowConfig {
    int     triggerThreshold = 25;
    QString windowName       = QStringLiteral("备用快餐口");
    double  avgServiceTime   = 45.0;
    int     staffCount       = 1;
    int     maxQueueLength   = 20;
};

/// @brief 食堂配置
struct CanteenConfigData {
    QString                canteenName = QStringLiteral("留园食堂");
    QList<WindowConfig>    windows;
    QList<TableConfig>     tables;
    BackupWindowConfig     backupWindow;
};

/// @brief 抵达分布数据点
struct ArrivalDistributionPoint {
    QString time;     // "17:00"
    int     rate = 0; // 人/分钟
};

/// @brief 学生行为配置
struct StudentConfigData {
    int                                          totalStudents = 2000;
    QList<ArrivalDistributionPoint>              arrivalDistribution;
    Models::WindowPreference                     windowPreference = Models::WindowPreference::ShortestQueue;
    Models::SeatPreference                       seatPreference   = Models::SeatPreference::MostEmptyTable;
    double                                       mealDurationMean  = 1200.0;  // 秒
    double                                       mealDurationStddev = 300.0;  // 秒
};

/// @brief 教学楼配置
struct TeachingBuildingConfigData {
    QString buildingName;
    QString location;
    QList<QPair<QString, int>> classEndSchedule;  // (时间, 下课人数)
};

/// @brief 仿真运行参数
struct SimulationParamsData {
    QString            simulationStartTime = QStringLiteral("17:00:00");
    int                simulationDuration  = 7200;  // 秒（2小时）
    int                timeStep            = 60;    // 秒
    Models::RunMode    runMode             = Models::RunMode::Visual;
    Models::LogLevel   logLevel            = Models::LogLevel::Normal;
};

/// @brief 完整的仿真配置
struct SimulationConfig {
    CanteenConfigData              canteen;
    StudentConfigData              student;
    QList<TeachingBuildingConfigData> teachingBuildings;
    SimulationParamsData           simulation;
};

// ============================================================
//  ConfigManager — 配置读写与验证
// ============================================================

class ConfigManager : public QObject
{
    Q_OBJECT

public:
    explicit ConfigManager(QObject *parent = nullptr);

    /// 从 JSON 文件加载配置
    bool loadFromFile(const QString &filePath);

    /// 将配置保存到 JSON 文件
    bool saveToFile(const QString &filePath);

    /// 序列化为 JSON 字符串
    QJsonObject toJson() const;

    /// 从 JSON 对象反序列化
    bool fromJson(const QJsonObject &json);

    /// 基本验证
    bool validate(QStringList &errors) const;

    /// 重置为默认配置
    void resetToDefaults();

    /// 获取当前配置（可读写）
    SimulationConfig& config() { return m_config; }
    const SimulationConfig& config() const { return m_config; }

    /// 获取默认配置文件名
    static QString defaultConfigFileName();

signals:
    void configLoaded(const QString &filePath);
    void configSaved(const QString &filePath);
    void configReset();
    void configValidationFailed(const QStringList &errors);

private:
    SimulationConfig m_config;

    // JSON 序列化/反序列化子方法
    QJsonObject windowConfigToJson(const WindowConfig &wc) const;
    WindowConfig windowConfigFromJson(const QJsonObject &obj) const;

    QJsonObject tableConfigToJson(const TableConfig &tc) const;
    TableConfig tableConfigFromJson(const QJsonObject &obj) const;

    QJsonObject backupConfigToJson(const BackupWindowConfig &bc) const;
    BackupWindowConfig backupConfigFromJson(const QJsonObject &obj) const;

    QJsonObject canteenConfigToJson(const CanteenConfigData &cc) const;
    CanteenConfigData canteenConfigFromJson(const QJsonObject &obj) const;

    QJsonObject studentConfigToJson(const StudentConfigData &sc) const;
    StudentConfigData studentConfigFromJson(const QJsonObject &obj) const;

    QJsonObject simulationParamsToJson(const SimulationParamsData &sp) const;
    SimulationParamsData simulationParamsFromJson(const QJsonObject &obj) const;
};

} // namespace Core

#endif // CONFIGMANAGER_H
