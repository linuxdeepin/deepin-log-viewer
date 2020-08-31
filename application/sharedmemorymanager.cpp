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
