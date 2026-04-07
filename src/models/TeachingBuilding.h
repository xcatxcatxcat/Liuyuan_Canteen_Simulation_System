#ifndef TEACHINGBUILDING_H
#define TEACHINGBUILDING_H

#include <QObject>
#include <QString>
#include <QList>
#include <QPair>

namespace Models {

/// @brief 教学楼 — 宏观仿真实体
///
/// 属性：ID、名称、位置描述、下课时间表、各时段下课学生人数分布
class TeachingBuilding : public QObject
{
    Q_OBJECT

public:
    explicit TeachingBuilding(QObject *parent = nullptr);

    int              id()    const { return m_id; }
    const QString&   name()  const { return m_name; }
    const QString&   location() const { return m_location; }

    void setId(int id);
    void setName(const QString &name);
    void setLocation(const QString &loc);

    /// 添加下课时间段
    /// @param timeStr 时间字符串，如 "17:00"
    /// @param studentCount 该时段下课学生人数
    void addClassEndTime(const QString &timeStr, int studentCount);

    /// 获取所有下课时间安排
    const QList<QPair<QString, int>>& classEndSchedule() const { return m_classEndSchedule; }

    /// 清空下课时间表
    void clearSchedule();

private:
    int     m_id = 0;
    QString m_name;
    QString m_location;
    QList<QPair<QString, int>> m_classEndSchedule;  // (时间字符串, 学生人数)
};

} // namespace Models

#endif // TEACHINGBUILDING_H
