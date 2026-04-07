#ifndef SEAT_H
#define SEAT_H

#include "Types.h"
#include <QObject>

namespace Models {

/// @brief 座位 — 仿真核心实体
///
/// 属性：ID、所属桌子 ID、占用状态、占用学生 ID、预计释放时间
class Seat : public QObject
{
    Q_OBJECT

public:
    explicit Seat(QObject *parent = nullptr);

    int     id()              const { return m_id; }
    int     tableId()         const { return m_tableId; }
    bool    isOccupied()      const { return m_occupied; }
    int     occupiedBy()      const { return m_occupiedBy; }  // 返回占用学生的 ID，未占用返回 -1
    int     releaseTime()     const { return m_releaseTime; }  // 仿真时间（秒）

    void setId(int id);
    void setTableId(int tableId);
    void occupy(int studentId, int releaseTimeSec);   // 占用座位
    void release();                                    // 释放座位

signals:
    void seatOccupied(int seatId, int studentId);
    void seatReleased(int seatId);

private:
    int  m_id          = 0;
    int  m_tableId     = 0;
    bool m_occupied    = false;
    int  m_occupiedBy  = -1;       // 占用学生的 ID
    int  m_releaseTime = 0;        // 预计释放时间（仿真时间，秒）
};

} // namespace Models

#endif // SEAT_H
