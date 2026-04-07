#ifndef CANTEEN_H
#define CANTEEN_H

#include "Window.h"
#include "Seat.h"
#include "Types.h"
#include <QObject>
#include <QList>

namespace Models {

/// @brief 食堂 — 仿真核心实体
///
/// 属性：ID、名称、窗口列表、座位列表、桌子配置
/// 提供窗口与座位的统一管理接口
class Canteen : public QObject
{
    Q_OBJECT

public:
    explicit Canteen(QObject *parent = nullptr);

    // ---- 食堂属性 ----
    int              id()    const { return m_id; }
    const QString&   name()  const { return m_name; }
    void setId(int id);
    void setName(const QString &name);

    // ---- 窗口管理 ----
    int                         windowCount()     const;
    const QList<Window*>&       windows()         const;
    Window*                     windowAt(int index) const;
    Window*                     findWindowById(int id) const;
    void                        addWindow(Window *w);
    void                        removeWindow(int windowId);
    void                        clearWindows();

    // ---- 座位管理 ----
    int                         seatCount()       const;
    int                         totalSeatCapacity() const;  // 座位容量
    int                         occupiedSeatCount() const;  // 已占用座位数
    int                         vacantSeatCount() const;    // 空余座位数
    double                      seatOccupancyRate() const;  // 座位占用率
    const QList<Seat*>&         seats()           const;
    Seat*                       findVacantSeat() const;      // 找到第一个空座位
    void                        addSeat(Seat *s);
    void                        clearSeats();

    // ---- 桌子配置 ----
    void                        addTable(int tableId, int seatCount);
    const QList<QPair<int,int>>& tableConfigs() const { return m_tableConfigs; }
    int                         tableCount()       const { return m_tableConfigs.size(); }
    void                        clearTables();

    // ---- 备用窗口 ----
    int                         backupTriggerThreshold() const { return m_backupTriggerThreshold; }
    const QString&              backupWindowName()       const { return m_backupWindowName; }
    double                      backupAvgServiceTime()   const { return m_backupAvgServiceTime; }
    void                        setBackupConfig(int threshold, const QString &name,
                                                double avgServiceTime, int maxQueue);

    // ---- 统计 ----
    int   totalQueueLength()  const;   // 所有窗口排队人数总和
    int   totalServedCount()  const;   // 累计已服务人数

signals:
    void windowAdded(int windowId);
    void windowRemoved(int windowId);
    void seatOccupancyChanged(double rate);

private:
    int               m_id = 0;
    QString           m_name;
    QList<Window*>    m_windows;
    QList<Seat*>      m_seats;
    QList<QPair<int,int>> m_tableConfigs;  // (tableId, seatCount)

    // 备用窗口配置
    int     m_backupTriggerThreshold = 25;
    QString m_backupWindowName       = QStringLiteral("备用快餐口");
    double  m_backupAvgServiceTime   = 45.0;
    int     m_backupMaxQueue         = 20;
};

} // namespace Models

#endif // CANTEEN_H
