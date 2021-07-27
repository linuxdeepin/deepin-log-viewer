/*
* Copyright (C) 2019 ~ 2020 UnionTech Software Technology Co.,Ltd
*
* Author:     zyc <zyc@uniontech.com>
* Maintainer:  zyc <zyc@uniontech.com>
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* any later version.
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef SHAREDMEMORYMANAGER_H
#define SHAREDMEMORYMANAGER_H

#include <QObject>

#include <mutex>
struct ShareMemoryInfo {
    bool isStart = true ;
};
class QSharedMemory;
class SharedMemoryManager : public QObject
{
public:

    static SharedMemoryManager *instance()
    {
        SharedMemoryManager *sin = m_instance.load();
        if (!sin) {
            std::lock_guard<std::mutex> lock(m_mutex);
            sin = m_instance.load();
            if (!sin) {
                sin = new SharedMemoryManager();
                m_instance.store(sin);
            }
        }
        return sin;
    }
    void setRunnableTag(ShareMemoryInfo iShareInfo);
    QString getRunnableKey();
    bool isAttached();
    void releaseMemory();

protected:
    explicit SharedMemoryManager(QObject *parent = nullptr);

    void init();
private:
    static std::atomic<SharedMemoryManager *> m_instance;
    static std::mutex m_mutex;
    QSharedMemory  *m_commondM = nullptr;
    ShareMemoryInfo *m_pShareMemoryInfo = nullptr;
};

#endif // SHAREDMEMORYMANAGER_H
