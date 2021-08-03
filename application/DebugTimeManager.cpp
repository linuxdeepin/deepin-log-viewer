/*
* Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
*
* Author:     zyc <zyc@uniontech.com>
*
* Maintainer:  zyc <zyc@uniontech.com>
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

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
