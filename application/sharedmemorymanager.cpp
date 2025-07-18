// SPDX-FileCopyrightText: 2019 - 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "sharedmemorymanager.h"
#include <QSharedMemory>
#include <QDebug>
#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(logApp)

#define LOG_POLIKIT_STOP_TAG "LOGAUTHCONTROL"
std::atomic<SharedMemoryManager *> SharedMemoryManager::m_instance;
std::mutex SharedMemoryManager::m_mutex;
SharedMemoryManager::SharedMemoryManager(QObject *parent)
    :  QObject(parent)
{
    qCDebug(logApp) << "SharedMemoryManager constructor";
    init();
}

SharedMemoryManager *SharedMemoryManager::getInstance()
{
    qCDebug(logApp) << "Getting SharedMemoryManager instance";
    return m_instance.load();
}

void SharedMemoryManager::setRunnableTag(ShareMemoryInfo iShareInfo)
{
    qCDebug(logApp) << "Setting runnable tag, isStart:" << iShareInfo.isStart;
    m_commondM->lock();
    qCDebug(logApp) << "ShareMemoryInfo.isStart:" << iShareInfo.isStart;
    m_pShareMemoryInfo = static_cast<ShareMemoryInfo *>(m_commondM->data());
    if (m_pShareMemoryInfo) {
        m_pShareMemoryInfo->isStart = iShareInfo.isStart;
        qCDebug(logApp) << "Successfully set runnable tag";
    } else {
        qCWarning(logApp) << "Failed to set runnable tag - control memory not attached";
    }
    m_commondM->unlock();
}

QString SharedMemoryManager::getRunnableKey()
{
    qCDebug(logApp) << "Getting runnable key";
    return  m_commondM->key();
}

bool SharedMemoryManager::isAttached()
{
    bool attached = (m_commondM && m_commondM->isAttached());
    qCDebug(logApp) << "Checking if attached:" << attached;
    return attached;
}

void SharedMemoryManager::releaseMemory()
{
    qCDebug(logApp) << "Releasing shared memory";
    if (m_commondM) {
        qCDebug(logApp) << "Current shared memory error:" << m_commondM->error() << m_commondM->errorString();
        if (m_commondM->isAttached()) {
            qCDebug(logApp) << "Detaching from shared memory";
            m_commondM->detach();
        }
        qCDebug(logApp) << "Shared memory error after detach:" << m_commondM->error() << m_commondM->errorString();
    } else {
        qCWarning(logApp) << "Attempted to release null shared memory";
    }
}

void SharedMemoryManager::init()
{
    qCDebug(logApp) << "Initializing shared memory";
    m_commondM = new QSharedMemory(this);
    QString tag = LOG_POLIKIT_STOP_TAG;
    qCDebug(logApp) << "Setting shared memory key:" << tag;
    m_commondM->setKey(tag);
    
    if (m_commondM->isAttached()) {
        qCDebug(logApp) << "Already attached to shared memory, detaching first";
        m_commondM->detach();
    }
    
    if (!m_commondM->create(sizeof(ShareMemoryInfo))) {
        qCWarning(logApp) << "Failed to create shared memory:" << m_commondM->key()
                        << QSharedMemory::SharedMemoryError(m_commondM->error())
                        << m_commondM->errorString();
        if (m_commondM->isAttached()) {
            qCDebug(logApp) << "Detaching after failed create";
            m_commondM->detach();
        }
        m_commondM->attach();

    } else {
        // 创建好以后，保持共享内存连接，防止释放。
        m_commondM->attach();
        // 主进程：首次赋值m_pShareMemoryInfo

    }
    ShareMemoryInfo info;
    info.isStart = true;
    qCDebug(logApp) << "Setting initial runnable tag";
    setRunnableTag(info);
}
