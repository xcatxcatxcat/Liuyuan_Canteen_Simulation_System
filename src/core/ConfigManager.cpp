#include "ConfigManager.h"
#include <QFile>
#include <QJsonParseError>
#include <QDir>

using namespace Core;

ConfigManager::ConfigManager(QObject *parent)
    : QObject(parent)
{
    resetToDefaults();
}

QString ConfigManager::defaultConfigFileName()
{
    return QStringLiteral("simulation_config.json");
}

void ConfigManager::resetToDefaults()
{
    m_config = SimulationConfig();

    m_config.canteen.canteenName = QStringLiteral("留园食堂");

    // 默认窗口配置
    m_config.canteen.windows = {
        { QStringLiteral("快餐一号"), 30.0,  1, 20 },
        { QStringLiteral("快餐二号"), 35.0,  1, 20 },
        { QStringLiteral("麻辣香锅"), 120.0, 1, 15 },
        { QStringLiteral("铁板烧"),   90.0,  1, 15 },
        { QStringLiteral("面食窗口"), 45.0,  1, 20 }
    };

    // 默认桌子配置
    m_config.canteen.tables = {
        {1, 4}, {2, 4}, {3, 4}, {4, 4}, {5, 4},
        {6, 4}, {7, 4}, {8, 4}, {9, 4}, {10, 4},
        {11, 6}, {12, 6}, {13, 6}, {14, 6}, {15, 6}
    };

    m_config.canteen.backupWindow = { 25, QStringLiteral("备用快餐口"), 45.0, 1, 20 };

    // 默认学生配置
    m_config.student.totalStudents   = 2000;
    m_config.student.arrivalDistribution = {
        { QStringLiteral("17:00"), 10 },
        { QStringLiteral("17:10"), 30 },
        { QStringLiteral("17:20"), 50 },
        { QStringLiteral("17:30"), 80 },
        { QStringLiteral("17:40"), 60 },
        { QStringLiteral("17:50"), 40 },
        { QStringLiteral("18:00"), 20 },
        { QStringLiteral("18:10"), 10 },
        { QStringLiteral("18:30"), 5 }
    };
    m_config.student.windowPreference   = Models::WindowPreference::ShortestQueue;
    m_config.student.seatPreference     = Models::SeatPreference::MostEmptyTable;
    m_config.student.mealDurationMean   = 1200.0;
    m_config.student.mealDurationStddev = 300.0;

    // 默认教学楼配置
    m_config.teachingBuildings = {
        { QStringLiteral("思源楼"), QStringLiteral("东区"), { { QStringLiteral("17:00"), 300 }, { QStringLiteral("11:50"), 500 } } },
        { QStringLiteral("逸夫楼"), QStringLiteral("西区"), { { QStringLiteral("17:10"), 250 }, { QStringLiteral("11:55"), 400 } } }
    };

    // 默认仿真参数
    m_config.simulation.simulationStartTime = QStringLiteral("17:00:00");
    m_config.simulation.simulationDuration  = 7200;
    m_config.simulation.timeStep            = 60;
    m_config.simulation.runMode             = Models::RunMode::Visual;
    m_config.simulation.logLevel            = Models::LogLevel::Normal;
}

// ============================================================
//  JSON 序列化
// ============================================================

QJsonObject ConfigManager::windowConfigToJson(const WindowConfig &wc) const
{
    return {
        { QStringLiteral("windowName"),      wc.windowName },
        { QStringLiteral("avgServiceTime"),  wc.avgServiceTime },
        { QStringLiteral("staffCount"),      wc.staffCount },
        { QStringLiteral("maxQueueLength"),  wc.maxQueueLength }
    };
}

WindowConfig ConfigManager::windowConfigFromJson(const QJsonObject &obj) const
{
    WindowConfig wc;
    wc.windowName      = obj[QStringLiteral("windowName")].toString(wc.windowName);
    wc.avgServiceTime  = obj[QStringLiteral("avgServiceTime")].toDouble(wc.avgServiceTime);
    wc.staffCount      = obj[QStringLiteral("staffCount")].toInt(wc.staffCount);
    wc.maxQueueLength  = obj[QStringLiteral("maxQueueLength")].toInt(wc.maxQueueLength);
    return wc;
}

QJsonObject ConfigManager::tableConfigToJson(const TableConfig &tc) const
{
    return {
        { QStringLiteral("tableID"),    tc.tableId },
        { QStringLiteral("seatCount"),  tc.seatCount }
    };
}

TableConfig ConfigManager::tableConfigFromJson(const QJsonObject &obj) const
{
    TableConfig tc;
    tc.tableId   = obj[QStringLiteral("tableID")].toInt(tc.tableId);
    tc.seatCount = obj[QStringLiteral("seatCount")].toInt(tc.seatCount);
    return tc;
}

QJsonObject ConfigManager::backupConfigToJson(const BackupWindowConfig &bc) const
{
    return {
        { QStringLiteral("triggerThreshold"), bc.triggerThreshold },
        { QStringLiteral("windowTemplate"), QJsonObject{
            { QStringLiteral("windowName"),      bc.windowName },
            { QStringLiteral("avgServiceTime"),  bc.avgServiceTime },
            { QStringLiteral("staffCount"),      bc.staffCount },
            { QStringLiteral("maxQueueLength"),  bc.maxQueueLength }
        }}
    };
}

BackupWindowConfig ConfigManager::backupConfigFromJson(const QJsonObject &obj) const
{
    BackupWindowConfig bc;
    bc.triggerThreshold = obj[QStringLiteral("triggerThreshold")].toInt(bc.triggerThreshold);
    auto tmpl = obj[QStringLiteral("windowTemplate")].toObject();
    bc.windowName      = tmpl[QStringLiteral("windowName")].toString(bc.windowName);
    bc.avgServiceTime  = tmpl[QStringLiteral("avgServiceTime")].toDouble(bc.avgServiceTime);
    bc.staffCount      = tmpl[QStringLiteral("staffCount")].toInt(bc.staffCount);
    bc.maxQueueLength  = tmpl[QStringLiteral("maxQueueLength")].toInt(bc.maxQueueLength);
    return bc;
}

QJsonObject ConfigManager::canteenConfigToJson(const CanteenConfigData &cc) const
{
    QJsonArray windowsArr, tablesArr;
    for (const auto &w : cc.windows)  windowsArr.append(windowConfigToJson(w));
    for (const auto &t : cc.tables)   tablesArr.append(tableConfigToJson(t));

    return {
        { QStringLiteral("canteenName"),  cc.canteenName },
        { QStringLiteral("windows"),      windowsArr },
        { QStringLiteral("tables"),       tablesArr },
        { QStringLiteral("backupWindow"), backupConfigToJson(cc.backupWindow) }
    };
}

CanteenConfigData ConfigManager::canteenConfigFromJson(const QJsonObject &obj) const
{
    CanteenConfigData cc;
    cc.canteenName = obj[QStringLiteral("canteenName")].toString(cc.canteenName);

    QJsonArray windowsArr = obj[QStringLiteral("windows")].toArray();
    for (const auto &wv : windowsArr)
        cc.windows.append(windowConfigFromJson(wv.toObject()));

    QJsonArray tablesArr = obj[QStringLiteral("tables")].toArray();
    for (const auto &tv : tablesArr)
        cc.tables.append(tableConfigFromJson(tv.toObject()));

    cc.backupWindow = backupConfigFromJson(obj[QStringLiteral("backupWindow")].toObject());

    return cc;
}

QJsonObject ConfigManager::studentConfigToJson(const StudentConfigData &sc) const
{
    QJsonArray distArr;
    for (const auto &d : sc.arrivalDistribution) {
        distArr.append(QJsonObject{
            { QStringLiteral("time"), d.time },
            { QStringLiteral("rate"), d.rate }
        });
    }

    return {
        { QStringLiteral("totalStudents"),         sc.totalStudents },
        { QStringLiteral("arrivalDistribution"),  distArr },
        { QStringLiteral("windowPreferenceStrategy"), Models::windowPreferenceToString(sc.windowPreference) },
        { QStringLiteral("seatPreferenceStrategy"),   Models::seatPreferenceToString(sc.seatPreference) },
        { QStringLiteral("mealDuration"), QJsonObject{
            { QStringLiteral("mean"),   sc.mealDurationMean },
            { QStringLiteral("stddev"), sc.mealDurationStddev }
        }}
    };
}

StudentConfigData ConfigManager::studentConfigFromJson(const QJsonObject &obj) const
{
    StudentConfigData sc;
    sc.totalStudents = obj[QStringLiteral("totalStudents")].toInt(sc.totalStudents);
    sc.windowPreference = Models::stringToWindowPreference(
        obj[QStringLiteral("windowPreferenceStrategy")].toString());
    sc.seatPreference = Models::stringToSeatPreference(
        obj[QStringLiteral("seatPreferenceStrategy")].toString());

    QJsonArray distArr = obj[QStringLiteral("arrivalDistribution")].toArray();
    for (const auto &dv : distArr) {
        auto d = dv.toObject();
        sc.arrivalDistribution.append({ d[QStringLiteral("time")].toString(),
                                          d[QStringLiteral("rate")].toInt() });
    }

    auto meal = obj[QStringLiteral("mealDuration")].toObject();
    sc.mealDurationMean   = meal[QStringLiteral("mean")].toDouble(sc.mealDurationMean);
    sc.mealDurationStddev = meal[QStringLiteral("stddev")].toDouble(sc.mealDurationStddev);

    return sc;
}

QJsonObject ConfigManager::simulationParamsToJson(const SimulationParamsData &sp) const
{
    return {
        { QStringLiteral("simulationStartTime"), sp.simulationStartTime },
        { QStringLiteral("simulationDuration"),  sp.simulationDuration },
        { QStringLiteral("timeStep"),            sp.timeStep },
        { QStringLiteral("runMode"),             sp.runMode == Models::RunMode::Visual ?
                                                    QStringLiteral("Visual") : QStringLiteral("Headless") },
        { QStringLiteral("logDetailLevel"),      Models::logLevelToString(sp.logLevel) }
    };
}

SimulationParamsData ConfigManager::simulationParamsFromJson(const QJsonObject &obj) const
{
    SimulationParamsData sp;
    sp.simulationStartTime = obj[QStringLiteral("simulationStartTime")].toString(sp.simulationStartTime);
    sp.simulationDuration  = obj[QStringLiteral("simulationDuration")].toInt(sp.simulationDuration);
    sp.timeStep            = obj[QStringLiteral("timeStep")].toInt(sp.timeStep);
    sp.logLevel            = Models::stringToLogLevel(obj[QStringLiteral("logDetailLevel")].toString());

    QString mode = obj[QStringLiteral("runMode")].toString();
    sp.runMode = (mode == QStringLiteral("Headless")) ?
                     Models::RunMode::Headless : Models::RunMode::Visual;

    return sp;
}

// ============================================================
//  公共接口
// ============================================================

QJsonObject ConfigManager::toJson() const
{
    QJsonObject root;
    root[QStringLiteral("canteenConfig")]   = canteenConfigToJson(m_config.canteen);
    root[QStringLiteral("studentConfig")]   = studentConfigToJson(m_config.student);
    root[QStringLiteral("simulationParams")] = simulationParamsToJson(m_config.simulation);

    // 教学楼配置
    QJsonArray tbArr;
    for (const auto &tb : m_config.teachingBuildings) {
        QJsonArray scheduleArr;
        for (const auto &s : tb.classEndSchedule) {
            scheduleArr.append(QJsonObject{
                { QStringLiteral("time"),  s.first },
                { QStringLiteral("count"), s.second }
            });
        }
        tbArr.append(QJsonObject{
            { QStringLiteral("buildingName"), tb.buildingName },
            { QStringLiteral("location"),     tb.location },
            { QStringLiteral("classEndSchedule"), scheduleArr }
        });
    }
    root[QStringLiteral("teachingBuildings")] = tbArr;

    return root;
}

bool ConfigManager::fromJson(const QJsonObject &json)
{
    m_config.canteen    = canteenConfigFromJson(json[QStringLiteral("canteenConfig")].toObject());
    m_config.student    = studentConfigFromJson(json[QStringLiteral("studentConfig")].toObject());
    m_config.simulation = simulationParamsFromJson(json[QStringLiteral("simulationParams")].toObject());

    QJsonArray tbArr = json[QStringLiteral("teachingBuildings")].toArray();
    m_config.teachingBuildings.clear();
    for (const auto &tbv : tbArr) {
        auto tbObj = tbv.toObject();
        TeachingBuildingConfigData tb;
        tb.buildingName = tbObj[QStringLiteral("buildingName")].toString();
        tb.location     = tbObj[QStringLiteral("location")].toString();

        QJsonArray schedArr = tbObj[QStringLiteral("classEndSchedule")].toArray();
        for (const auto &sv : schedArr) {
            auto s = sv.toObject();
            tb.classEndSchedule.append({
                s[QStringLiteral("time")].toString(),
                s[QStringLiteral("count")].toInt()
            });
        }
        m_config.teachingBuildings.append(tb);
    }

    return true;
}

bool ConfigManager::loadFromFile(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) return false;

    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll(), &err);
    file.close();

    if (err.error != QJsonParseError::NoError) return false;
    if (!doc.isObject()) return false;

    if (fromJson(doc.object())) {
        emit configLoaded(filePath);
        return true;
    }
    return false;
}

bool ConfigManager::saveToFile(const QString &filePath)
{
    QJsonDocument doc(toJson());
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly)) return false;
    file.write(doc.toJson(QJsonDocument::Indented));
    file.close();

    emit configSaved(filePath);
    return true;
}

bool ConfigManager::validate(QStringList &errors) const
{
    errors.clear();

    if (m_config.canteen.windows.isEmpty())
        errors << QStringLiteral("至少需要配置一个打饭窗口");
    if (m_config.canteen.tables.isEmpty())
        errors << QStringLiteral("至少需要配置一张桌子");
    if (m_config.student.totalStudents <= 0)
        errors << QStringLiteral("就餐学生总数必须大于 0");
    if (m_config.student.arrivalDistribution.isEmpty())
        errors << QStringLiteral("至少需要配置一个抵达分布时间点");
    if (m_config.student.mealDurationMean <= 0)
        errors << QStringLiteral("平均就餐时长必须大于 0");
    if (m_config.simulation.simulationDuration <= 0)
        errors << QStringLiteral("仿真时长必须大于 0");
    if (m_config.simulation.timeStep <= 0)
        errors << QStringLiteral("时间步长必须大于 0");

    for (const auto &w : m_config.canteen.windows) {
        if (w.avgServiceTime <= 0)
            errors << QString("窗口「%1」平均服务时长必须大于 0").arg(w.windowName);
        if (w.maxQueueLength <= 0)
            errors << QString("窗口「%1」队列容量必须大于 0").arg(w.windowName);
    }

    return errors.isEmpty();
}
