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
#include "authsharedmemorymanager.h"

#include <QDebug>
#define LOG_POLIKIT_STOP_TAG "LOGAUTHCONTROL"
#define LOG_SIZE_INFO_TAG_PREFIX "LOGAUTHSIZE"
#define LOG_FILE_DATA_TAG_PREFIX "LOGAUTHFILE"
std::atomic<AuthSharedMemoryManager *> AuthSharedMemoryManager::m_instance;
std::mutex AuthSharedMemoryManager::m_mutex;
AuthSharedMemoryManager::AuthSharedMemoryManager(QObject *parent)
    :  QObject(parent)
    , m_stopSharedMem(nullptr)
    , m_pShareMemoryInfo(nullptr)
{
    init();
}


QString AuthSharedMemoryManager::getRunnableKey()
{
    return  m_stopSharedMem->key();
}

void AuthSharedMemoryManager::releaseAllMem()
{
    releaseMemory(m_stopSharedMem);
    foreach (QSharedMemory *item, m_sizeSharedMems.values()) {
        releaseMemory(item);
    }
    foreach (QSharedMemory *item, m_fileDataSharedMems.values()) {
        releaseMemory(item);
    }
}


bool AuthSharedMemoryManager::isAttached()
{
    return (m_stopSharedMem && m_stopSharedMem->isAttached());
}

bool AuthSharedMemoryManager::initRunnableTagMem(const QString &iTag)
{
    return  initShareMemory(m_stopSharedMem, iTag, QSharedMemory::ReadOnly);
}

bool AuthSharedMemoryManager::addDataInfo(qint64 iInfoSize, char *iDataInfo, QString &oTag)
{
    QSharedMemory *sizeMem = nullptr;
    QString sizeTag = LOG_SIZE_INFO_TAG_PREFIX + QString(m_sizeSharedMems.size());
    QString dataTag = LOG_FILE_DATA_TAG_PREFIX + QString(m_sizeSharedMems.size());
    ShareMemorySizeInfo *sizeInfo;
    bool ret = createShareMemoryWrite<ShareMemorySizeInfo>(sizeMem, sizeTag);
    if (!ret) {
        return  false;
    }

    sizeInfo->infoSize = iInfoSize;
    QSharedMemory *dataMem = nullptr;
    oTag = sizeTag;
    ret =  createShareMemoryWrite<char>(dataMem, dataTag, iInfoSize);
    if (ret) {
        m_fileDataSharedMems.insert(QString(m_sizeSharedMems.size()), dataMem);
        m_sizeSharedMems.insert(QString(m_sizeSharedMems.size()), sizeMem);

    }
    return  ret;


}

ShareMemoryInfo *AuthSharedMemoryManager::getRunnableTag()
{
    return  m_pShareMemoryInfo;
}

bool AuthSharedMemoryManager::releaseMemory(QSharedMemory *iMem)
{
    bool result = false;
    if (iMem) {
        result = iMem->unlock();
        if (!result) {
            return result;
        }
        if (iMem->isAttached()) {     //检测程序当前是否关联共享内存
            result =   iMem->detach();
            return  result;
        } else {
            return true;
        }
    } else {
        return true;
    }
}





void AuthSharedMemoryManager::init()
{



}

template<typename T>
bool AuthSharedMemoryManager::createShareMemoryWrite(QSharedMemory *iMem, const QString &iTag, int iSize)
{
    bool result = false;
    if (iMem) {
        result =   releaseMemory(iMem);
        if (!result) {
            return  false;
        }
    }

    iMem = new QSharedMemory(iTag, this);

    if (iMem->isAttached())      //检测程序当前是否关联共享内存
        iMem->detach();
    int size = iSize > 0 ? iSize : sizeof(T);
    if (!iMem->create(size)) {     //创建共享内存，大小为size
        qDebug() << "ShareMemory create error" << iMem->key() << QSharedMemory:: SharedMemoryError(iMem->error()) << iMem->errorString();
        if (iMem->isAttached())      //检测程序当前是否关联共享内存
            iMem->detach();
        result = iMem->attach();
    } else {
        // 创建好以后，保持共享内存连接，防止释放。
        result =  iMem->attach();
        // 主进程：首次赋值m_pShareMemoryInfo
    }
    if (!result) {
        releaseMemory(iMem);
    }
    return  result ;


}



bool AuthSharedMemoryManager::initShareMemory(QSharedMemory *iMem, const QString &iTag, QSharedMemory::AccessMode mode)
{
    bool result = false;
    if (iMem) {
        result =   releaseMemory(iMem);
        if (!result) {
            return  false;
        }
    }

    iMem = new QSharedMemory(iTag, this);
    if (iMem->isAttached())      //检测程序当前是否关联共享内存
        iMem->detach();          //解除关联
    result =  iMem->attach(mode);
    if (!result) {
        releaseMemory(iMem);
    }
    return  result ;

}
