#include "Seat.h"

namespace Models {

Seat::Seat(QObject *parent)
    : QObject(parent)
{
}

void Seat::setId(int id)          { m_id = id; }
void Seat::setTableId(int tableId){ m_tableId = tableId; }

void Seat::occupy(int studentId, int releaseTimeSec)
{
    m_occupied    = true;
    m_occupiedBy  = studentId;
    m_releaseTime = releaseTimeSec;
    emit seatOccupied(m_id, studentId);
}

void Seat::release()
{
    m_occupied    = false;
    m_occupiedBy  = -1;
    m_releaseTime = 0;
    emit seatReleased(m_id);
}

} // namespace Models
