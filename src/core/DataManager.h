#ifndef DATAMANAGER_H
#define DATAMANAGER_H

#include <QObject>
#include <QString>

namespace Core {

/// @brief DataManager — 仿真运行数据管理
///
/// 职责：
///   - 管理仿真过程中产生的实时数据和历史数据
///   - 向其他模块提供统一的数据存取接口
///   - 输出结果文件（simulation_results.json / timeseries_stats.csv）
class DataManager : public QObject
{
    Q_OBJECT

public:
    explicit DataManager(QObject *parent = nullptr);

    /// 设置输出目录（默认为 ./data/）
    void setOutputDir(const QString &dir);

    /// 写入事件日志条目
    void appendEventLog(int simTime, const QString &eventType,
                        const QString &description);

    /// 写入时间序列统计快照
    void appendTimeSeriesSnapshot(int simTime, int totalInCanteen,
                                  int totalQueuing, double seatOccupancyRate);

    /// 保存仿真结果摘要（simulation_results.json）
    bool saveSimulationResults(const QString &configName,
                               const QJsonObject &results);

    /// 保存时间序列数据（timeseries_stats.csv）
    bool saveTimeSeriesData();

    /// 清空运行时缓存
    void clearRuntimeData();

    /// 获取输出目录
    QString outputDir() const { return m_outputDir; }

    /// 获取已保存的配置列表
    QStringList savedConfigList() const;

    /// 获取已保存的结果列表
    QStringList savedResultList() const;

private:
    QString m_outputDir;
    QStringList m_eventLogBuffer;    // 运行时事件日志缓存
    QStringList m_timeSeriesBuffer;  // 时间序列 CSV 缓存

    void ensureDir(const QString &path);
};

} // namespace Core

#endif // DATAMANAGER_H
