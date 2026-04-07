#ifndef STUDENT_H
#define STUDENT_H

#include "Types.h"
#include <QObject>

namespace Models {

/// @brief 学生 — 仿真核心实体
///
/// 属性：ID、当前状态、目标窗口 ID、所在座位 ID、
///       各阶段时间戳（抵达、开始排队、开始就餐、离开）
class Student : public QObject
{
    Q_OBJECT

public:
    explicit Student(QObject *parent = nullptr);

    // ---- 属性访问 ----
    int             id()              const { return m_id; }
    StudentState    state()           const { return m_state; }
    int             targetWindowId()  const { return m_targetWindowId; }
    int             seatId()          const { return m_seatId; }

    int             arrivalTime()     const { return m_arrivalTime; }
    int             queueStartTime()  const { return m_queueStartTime; }
    int             serviceStartTime() const { return m_serviceStartTime; }
    int             dineStartTime()   const { return m_dineStartTime; }
    int             leaveTime()       const { return m_leaveTime; }

    // ---- 属性设置 ----
    void setId(int id);
    void setState(StudentState state);

    // ---- 仿真操作 ----
    void arrive(int simTime);                        // 学生抵达食堂
    void startQueue(int windowId, int simTime);       // 开始排队
    void startService(int simTime);                  // 开始被服务
    void startDining(int seatId, int simTime);        // 开始就餐
    void leave(int simTime);                         // 离开食堂

    // ---- 辅助计算 ----
    int  queueWaitTime()    const;   // 排队等待时间
    int  serviceTime()      const;   // 打饭服务时间
    int  diningTime()       const;   // 就餐时间
    int  totalTime()        const;   // 在食堂总时长

signals:
    void stateChanged(int studentId, StudentState newState);

private:
    int          m_id               = 0;
    StudentState m_state            = StudentState::Moving;
    int          m_targetWindowId   = -1;
    int          m_seatId           = -1;

    int m_arrivalTime      = 0;
    int m_queueStartTime   = 0;
    int m_serviceStartTime = 0;
    int m_dineStartTime    = 0;
    int m_leaveTime        = 0;
};

} // namespace Models

#endif // STUDENT_H
