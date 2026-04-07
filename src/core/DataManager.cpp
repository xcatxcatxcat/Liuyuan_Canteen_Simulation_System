#include "DataManager.h"
#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QFileInfoList>
#include <QDateTime>

using namespace Core;

DataManager::DataManager(QObject *parent)
    : QObject(parent)
    , m_outputDir(QStringLiteral("data"))
{
}

void DataManager::setOutputDir(const QString &dir)
{
    m_outputDir = dir;
}

void DataManager::appendEventLog(int simTime, const QString &eventType,
                                   const QString &description)
{
    // 格式: 时间,事件类型,描述
    m_eventLogBuffer.append(
        QString("%1,%2,%3")
            .arg(simTime)
            .arg(eventType)
            .arg(description));
}

void DataManager::appendTimeSeriesSnapshot(int simTime, int totalInCanteen,
                                            int totalQueuing, double seatOccupancyRate)
{
    // 首行或数据行
    if (m_timeSeriesBuffer.isEmpty()) {
        m_timeSeriesBuffer.append(QStringLiteral("Timestamp,TotalInCanteen,TotalQueuing,SeatOccupancyRate"));
    }

    // 将 simTime（秒）转换为 HH:MM:SS
    int h = simTime / 3600;
    int m = (simTime % 3600) / 60;
    int s = simTime % 60;
    QString timestamp = QString("%1:%2:%3")
        .arg(h, 2, 10, QChar('0'))
        .arg(m, 2, 10, QChar('0'))
        .arg(s, 2, 10, QChar('0'));

    m_timeSeriesBuffer.append(
        QString("%1,%2,%3,%4")
            .arg(timestamp)
            .arg(totalInCanteen)
            .arg(totalQueuing)
            .arg(seatOccupancyRate, 0, 'f', 2));
}

bool DataManager::saveSimulationResults(const QString &configName,
                                         const QJsonObject &results)
{
    ensureDir(m_outputDir);

    QJsonObject root;
    root[QStringLiteral("simulationMetadata")] = QJsonObject{
        { QStringLiteral("configFileName"),    configName },
        { QStringLiteral("simulationEndTime"), QDateTime::currentDateTime().toString(Qt::ISODate) }
    };
    root[QStringLiteral("results")] = results;

    QString filePath = m_outputDir + QStringLiteral("/simulation_results.json");
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly)) return false;

    QJsonDocument doc(root);
    file.write(doc.toJson(QJsonDocument::Indented));
    file.close();
    return true;
}

bool DataManager::saveTimeSeriesData()
{
    if (m_timeSeriesBuffer.isEmpty()) return true;

    ensureDir(m_outputDir);

    QString filePath = m_outputDir + QStringLiteral("/timeseries_stats.csv");
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly)) return false;

    QTextStream stream(&file);
    for (const auto &line : m_timeSeriesBuffer)
        stream << line << "\n";
    file.close();
    return true;
}

void DataManager::clearRuntimeData()
{
    m_eventLogBuffer.clear();
    m_timeSeriesBuffer.clear();
}

QStringList DataManager::savedConfigList() const
{
    QStringList list;
    QDir dir(m_outputDir);
    if (!dir.exists()) return list;

    QFileInfoList files = dir.entryInfoList(
        QStringList() << QStringLiteral("*.json"),
        QDir::Files);
    for (const auto &f : files) {
        list << f.fileName();
    }
    return list;
}

QStringList DataManager::savedResultList() const
{
    QStringList list;
    QDir dir(m_outputDir);
    if (!dir.exists()) return list;

    QFileInfoList files = dir.entryInfoList(
        QStringList() << QStringLiteral("simulation_results.json") << QStringLiteral("timeseries_stats.csv"),
        QDir::Files);
    for (const auto &f : files) {
        list << f.fileName();
    }
    return list;
}

void DataManager::ensureDir(const QString &path)
{
    QDir dir(path);
    if (!dir.exists()) dir.mkpath(QStringLiteral("."));
}
