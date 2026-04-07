#include "TeachingBuilding.h"

namespace Models {

TeachingBuilding::TeachingBuilding(QObject *parent)
    : QObject(parent)
{
}

void TeachingBuilding::setId(int id)               { m_id = id; }
void TeachingBuilding::setName(const QString &n)   { m_name = n; }
void TeachingBuilding::setLocation(const QString &l){ m_location = l; }

void TeachingBuilding::addClassEndTime(const QString &timeStr, int studentCount)
{
    m_classEndSchedule.append(qMakePair(timeStr, studentCount));
}

void TeachingBuilding::clearSchedule()
{
    m_classEndSchedule.clear();
}

} // namespace Models
