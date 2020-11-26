#include "DebugTimeManager.h"
#include <QDateTime>
#include <QDebug>

#include <sys/time.h>
#include <time.h>


DebugTimeManager    *DebugTimeManager::s_Instance = nullptr;

DebugTimeManager::DebugTimeManager()
{

}

void DebugTimeManager::clear()
{
    m_MapPoint.clear();
    m_MapLinuxPoint.clear();
}

void DebugTimeManager::beginPointQt(const QString &point, const QString &status)
{
    PointInfo info;
    info.desc = status;
    info.time = QDateTime::currentMSecsSinceEpoch();
    m_MapPoint.insert(point, info);
}
void DebugTimeManager::endPointQt(const QString &point)
{
    if (m_MapPoint.find(point) != m_MapPoint.end()) {
        m_MapPoint[point].time = QDateTime::currentMSecsSinceEpoch() - m_MapPoint[point].time;
        qDebug() << QString("[GRABPOINT] %1 %2 time=%3ms").arg(point).arg(m_MapPoint[point].desc).arg(m_MapPoint[point].time);
    }
}

void DebugTimeManager::beginPointLinux(const QString &point, const QString &status)
{
    PointInfoLinux info;
    info.desc = status;
    timespec beginTime;
    int result = clock_gettime(CLOCK_MONOTONIC, &beginTime);
    if (result) {
        return;
    }
    info.time = beginTime;
    m_MapLinuxPoint.insert(point, info);

}
void DebugTimeManager::endPointLinux(const QString &point, const QString &status)
{
    if (m_MapLinuxPoint.contains(point)) {
        timespec endTime;
        int result = clock_gettime(CLOCK_MONOTONIC, &endTime);
        if (result) {
            return;
        }
        timespec diffTime =  diff(m_MapLinuxPoint[point].time, endTime);
        qInfo() << QString("[GRABPOINT] %1 %2 %3 time=%4ms").arg(point).arg(m_MapLinuxPoint[point].desc).arg(status).arg(diffTime.tv_sec * 1000 + (diffTime.tv_nsec) / 1000000);
        m_MapLinuxPoint.remove(point);
    }
}

timespec DebugTimeManager::diff(timespec start, timespec end)
{
    timespec temp;
    temp.tv_sec = end.tv_sec - start.tv_sec;
    temp.tv_nsec = end.tv_nsec - start.tv_nsec;
    return temp;
}
