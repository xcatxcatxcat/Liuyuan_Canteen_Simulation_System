#include "Window.h"

namespace Models {

Window::Window(QObject *parent)
    : QObject(parent)
{
}

void Window::setId(int id)              { m_id = id; }
void Window::setName(const QString &n)  { m_name = n; }
void Window::setAvgServiceTime(double s){ m_avgServiceTime = s; }
void Window::setStaffCount(int c)       { m_staffCount = c; }
void Window::setMaxQueueLength(int m)   { m_maxQueueLength = m; }

bool Window::enqueueStudent(int studentId)
{
    if (isQueueFull()) return false;
    m_queue.push(studentId);
    if (m_state == WindowState::Idle) {
        m_state = WindowState::Serving;
        emit stateChanged(m_id, m_state);
    }
    emit studentEnqueued(m_id, studentId);
    return true;
}

int Window::dequeueStudent()
{
    if (m_queue.empty()) return -1;
    int sid = m_queue.front();
    m_queue.pop();
    emit studentDequeued(m_id, sid);
    if (m_queue.empty() && m_state == WindowState::Serving) {
        m_state = WindowState::Idle;
        emit stateChanged(m_id, m_state);
    }
    return sid;
}

bool Window::isQueueFull()  const { return static_cast<int>(m_queue.size()) >= m_maxQueueLength; }
bool Window::isQueueEmpty() const { return m_queue.empty(); }

} // namespace Models
