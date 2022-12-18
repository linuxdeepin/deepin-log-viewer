// SPDX-FileCopyrightText: 2019 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

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
    m_commondM->lock();
    qDebug() << "setRunnableTag" << iShareInfo.isStart;
    m_pShareMemoryInfo = static_cast<ShareMemoryInfo *>(m_commondM->data());
    if (m_pShareMemoryInfo) {
        m_pShareMemoryInfo->isStart = iShareInfo.isStart;
    } else {
        qWarning() << "conntrol mem is Not Attech ";
    }
    m_commondM->unlock();

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
        //  m_commondM->unlock();
        qDebug() << "m_commondM->error" << m_commondM->error() << m_commondM->errorString();
        if (m_commondM->isAttached())      //检测程序当前是否关联共享内存
            m_commondM->detach();
        qDebug() << "m_commondM->error" << m_commondM->error() << m_commondM->errorString();

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
        qWarning() << "ShareMemory create error" << m_commondM->key() << QSharedMemory::SharedMemoryError(m_commondM->error()) << m_commondM->errorString();
        if (m_commondM->isAttached())      //检测程序当前是否关联共享内存
            m_commondM->detach();
        m_commondM->attach();

    } else {
        // 创建好以后，保持共享内存连接，防止释放。
        m_commondM->attach();
        // 主进程：首次赋值m_pShareMemoryInfo

    }
    ShareMemoryInfo info;
    info.isStart = true;

    setRunnableTag(info);
}
