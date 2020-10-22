/*
* Copyright (C) 2019 ~ 2020 UnionTech Software Technology Co.,Ltd
*
* Author:     zyc <zyc@uniontech.com>
* Maintainer:  zyc <zyc@uniontech.com>
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License detachas published by
* the Free Software Foundation, either version 3 of the License, or
* any later version.
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "sharedmemorymanager.h"
#include <QSharedMemory>
#include <QDebug>
#define LOG_POLIKIT_STOP_TAG "LOGAUTHCONTROL"
std::atomic<SharedMemoryManager *> SharedMemoryManager::m_instance;
std::mutex SharedMemoryManager::m_mutex;
SharedMemoryManager::SharedMemoryManager(QObject *parent)
    :  QObject(parent)
{
    init();
}
void SharedMemoryManager::setRunnableTag(ShareMemoryInfo iShareInfo)
{
    qDebug() << "setRunnableTag" << iShareInfo.isStart;
    m_pShareMemoryInfo->isStart = iShareInfo.isStart;

}

QString SharedMemoryManager::getRunnableKey()
{
    return  m_commondM->key();
}

bool SharedMemoryManager::isAttached()
{
    return (m_commondM && m_commondM->isAttached());
}

void SharedMemoryManager::releaseMemory()
{
    if (m_commondM) {
        m_commondM->unlock();
        if (m_commondM->isAttached())      //检测程序当前是否关联共享内存
            m_commondM->detach();
    }
}



void SharedMemoryManager::init()
{
    m_commondM = new QSharedMemory(this);
    QString tag = LOG_POLIKIT_STOP_TAG ;//+ QString::number(m_threadCount);
    m_commondM->setKey(tag);
    if (m_commondM->isAttached())      //检测程序当前是否关联共享内存
        m_commondM->detach();
    if (!m_commondM->create(sizeof(ShareMemoryInfo))) {     //创建共享内存，大小为size
        qDebug() << "ShareMemory create error" << m_commondM->key() << QSharedMemory:: SharedMemoryError(m_commondM->error()) << m_commondM->errorString();
        if (m_commondM->isAttached())      //检测程序当前是否关联共享内存
            m_commondM->detach();
        m_commondM->attach();

    } else {
        // 创建好以后，保持共享内存连接，防止释放。
        m_commondM->attach();
        // 主进程：首次赋值m_pShareMemoryInfo

    }
    m_pShareMemoryInfo = static_cast<ShareMemoryInfo *>(m_commondM->data());
    m_pShareMemoryInfo->isStart = true;
}
