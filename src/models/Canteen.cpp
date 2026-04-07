#include "Canteen.h"
#include <algorithm>

namespace Models {

Canteen::Canteen(QObject *parent)
    : QObject(parent)
{
}

void Canteen::setId(int id)            { m_id = id; }
void Canteen::setName(const QString &n){ m_name = n; }

// ---- 窗口管理 ----

int Canteen::windowCount() const { return m_windows.size(); }

const QList<Window*>& Canteen::windows() const { return m_windows; }

Window* Canteen::windowAt(int index) const
{
    if (index < 0 || index >= m_windows.size()) return nullptr;
    return m_windows[index];
}

Window* Canteen::findWindowById(int id) const
{
    for (auto *w : m_windows) {
        if (w->id() == id) return w;
    }
    return nullptr;
}

void Canteen::addWindow(Window *w)
{
    if (!w) return;
    m_windows.append(w);
    emit windowAdded(w->id());
}

void Canteen::removeWindow(int windowId)
{
    for (int i = 0; i < m_windows.size(); ++i) {
        if (m_windows[i]->id() == windowId) {
            m_windows[i]->deleteLater();
            m_windows.removeAt(i);
            emit windowRemoved(windowId);
            return;
        }
    }
}

void Canteen::clearWindows()
{
    qDeleteAll(m_windows);
    m_windows.clear();
}

// ---- 座位管理 ----

int Canteen::seatCount() const { return m_seats.size(); }

int Canteen::totalSeatCapacity() const { return m_seats.size(); }

int Canteen::occupiedSeatCount() const
{
    int count = 0;
    for (auto *s : m_seats) {
        if (s->isOccupied()) ++count;
    }
    return count;
}

int Canteen::vacantSeatCount() const
{
    return totalSeatCapacity() - occupiedSeatCount();
}

double Canteen::seatOccupancyRate() const
{
    if (m_seats.isEmpty()) return 0.0;
    return static_cast<double>(occupiedSeatCount()) / totalSeatCapacity();
}

const QList<Seat*>& Canteen::seats() const { return m_seats; }

Seat* Canteen::findVacantSeat() const
{
    for (auto *s : m_seats) {
        if (!s->isOccupied()) return s;
    }
    return nullptr;
}

void Canteen::addSeat(Seat *s)
{
    if (!s) return;
    m_seats.append(s);
}

void Canteen::clearSeats()
{
    qDeleteAll(m_seats);
    m_seats.clear();
}

// ---- 桌子配置 ----

void Canteen::addTable(int tableId, int seatCount)
{
    m_tableConfigs.append(qMakePair(tableId, seatCount));
}

void Canteen::clearTables()
{
    m_tableConfigs.clear();
}

// ---- 备用窗口 ----

void Canteen::setBackupConfig(int threshold, const QString &name,
                               double avgServiceTime, int maxQueue)
{
    m_backupTriggerThreshold = threshold;
    m_backupWindowName       = name;
    m_backupAvgServiceTime   = avgServiceTime;
    m_backupMaxQueue         = maxQueue;
}

// ---- 统计 ----

int Canteen::totalQueueLength() const
{
    int total = 0;
    for (auto *w : m_windows) {
        total += w->currentQueueLength();
    }
    return total;
}

int Canteen::totalServedCount() const
{
    // 此值由引擎累计，此处预留接口
    return 0;
}

} // namespace Models
