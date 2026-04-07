#ifndef WINDOW_H
#define WINDOW_H

#include "Types.h"
#include <QObject>
#include <queue>

namespace Models {

/// @brief 打饭窗口 — 仿真核心实体
///
/// 属性：ID、名称、平均服务时长（秒/人）、服务员工数、
///       队列容量、当前队列、忙闲状态
class Window : public QObject
{
    Q_OBJECT

public:
    explicit Window(QObject *parent = nullptr);

    // ---- 属性访问 ----
    int             id()           const { return m_id; }
    const QString&  name()         const { return m_name; }
    double          avgServiceTime() const { return m_avgServiceTime; }
    int             staffCount()   const { return m_staffCount; }
    int             maxQueueLength() const { return m_maxQueueLength; }
    int             currentQueueLength() const { return static_cast<int>(m_queue.size()); }
    WindowState     state()        const { return m_state; }

    // ---- 属性设置 ----
    void setId(int id);
    void setName(const QString &name);
    void setAvgServiceTime(double seconds);
    void setStaffCount(int count);
    void setMaxQueueLength(int maxLen);

    // ---- 仿真操作 ----
    bool enqueueStudent(int studentId);      // 学生入队，成功返回 true
    int  dequeueStudent();                   // 学生出队，返回 studentId（无学生返回 -1）
    bool isQueueFull()  const;               // 队列是否已满
    bool isQueueEmpty() const;               // 队列是否为空

signals:
    void studentEnqueued(int windowId, int studentId);
    void studentDequeued(int windowId, int studentId);
    void stateChanged(int windowId, WindowState newState);

private:
    int         m_id = 0;
    QString     m_name;
    double      m_avgServiceTime = 30.0;  // 秒/人，默认值
    int         m_staffCount = 1;
    int         m_maxQueueLength = 20;
    WindowState m_state = WindowState::Idle;

    std::queue<int> m_queue;   // 存储排队学生的 ID
};

} // namespace Models

#endif // WINDOW_H
