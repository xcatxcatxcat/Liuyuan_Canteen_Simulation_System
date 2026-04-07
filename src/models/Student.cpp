#include "Student.h"

namespace Models {

Student::Student(QObject *parent)
    : QObject(parent)
{
}

void Student::setId(int id)           { m_id = id; }

void Student::setState(StudentState s)
{
    m_state = s;
    emit stateChanged(m_id, s);
}

void Student::arrive(int simTime)
{
    m_state = StudentState::Moving;
    m_arrivalTime = simTime;
    emit stateChanged(m_id, m_state);
}

void Student::startQueue(int windowId, int simTime)
{
    m_targetWindowId  = windowId;
    m_state           = StudentState::Queuing;
    m_queueStartTime  = simTime;
    emit stateChanged(m_id, m_state);
}

void Student::startService(int simTime)
{
    m_state            = StudentState::BeingServed;
    m_serviceStartTime = simTime;
    emit stateChanged(m_id, m_state);
}

void Student::startDining(int seatId, int simTime)
{
    m_seatId        = seatId;
    m_state         = StudentState::Dining;
    m_dineStartTime = simTime;
    emit stateChanged(m_id, m_state);
}

void Student::leave(int simTime)
{
    m_state     = StudentState::Leaving;
    m_leaveTime = simTime;
    emit stateChanged(m_id, m_state);
}

int Student::queueWaitTime() const
{
    return (m_queueStartTime > 0) ? m_serviceStartTime - m_queueStartTime : 0;
}

int Student::serviceTime() const
{
    return (m_serviceStartTime > 0) ? m_dineStartTime - m_serviceStartTime : 0;
}

int Student::diningTime() const
{
    return (m_dineStartTime > 0) ? m_leaveTime - m_dineStartTime : 0;
}

int Student::totalTime() const
{
    return m_leaveTime - m_arrivalTime;
}

} // namespace Models
