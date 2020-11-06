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
    :  BaseSharedMemoryManager(parent)
    , m_stopSharedMem(nullptr)
{
    init();
}


QString AuthSharedMemoryManager::getRunnableKey()
{
    return  m_stopSharedMem->key();
}

void AuthSharedMemoryManager::releaseAllMem()
{
    qDebug() << "releaseAllMem----------------start";
    releaseMemory(&m_stopSharedMem);
    qDebug() << "m_sizeSharedMems.size" << m_sizeSharedMems.size();
    qDebug() << "m_fileDataSharedMems.size" << m_fileDataSharedMems.size();
    foreach (QSharedMemory *item, m_sizeSharedMems.values()) {
        releaseMemory(&item);
    }
    foreach (QSharedMemory *item, m_fileDataSharedMems.values()) {
        releaseMemory(&item);
    }
    qDebug() << "releaseAllMem----------------end";
}


bool AuthSharedMemoryManager::isAttached()
{
    return (m_stopSharedMem && m_stopSharedMem->isAttached());
}

bool AuthSharedMemoryManager::initRunnableTagMem(const QString &iTag)
{
    return  initShareMemory(&m_stopSharedMem, iTag, QSharedMemory::ReadOnly);
}

bool AuthSharedMemoryManager::addDataInfo(qint64 iInfoSize, char *iDataInfo, QString &oTag)
{
    QSharedMemory *sizeMem = nullptr;
    QString sizeTag = LOG_SIZE_INFO_TAG_PREFIX + QString::number(m_sizeSharedMems.size());
    QString dataTag = LOG_FILE_DATA_TAG_PREFIX + QString::number(m_sizeSharedMems.size());
    ShareMemorySizeInfo sizeInfo;
    qDebug() << "111111111";
    bool ret = createShareMemoryWrite<ShareMemorySizeInfo>(&sizeMem, sizeTag);
    if (!ret) {
        return  false;
    }
    qDebug() << "2222222";
    sizeInfo.infoSize = iInfoSize;
    if (!setMemomData<ShareMemorySizeInfo>(&sizeMem, &sizeInfo)) {
        releaseMemory(&sizeMem);
        return false;
    }
    qDebug() << "3333333333";
    QSharedMemory *dataMem = nullptr;
    oTag = sizeTag;
    ret =  createShareMemoryWrite<char>(&dataMem, dataTag, 5);

    if (ret) {
        qDebug() << "4444444444";
        if (!setMemomData<char >(&dataMem, iDataInfo)) {
            qDebug() << "66666666";
            releaseMemory(&sizeMem);
            releaseMemory(&dataMem);
            return false;
        }
        m_fileDataSharedMems.insert(QString(m_sizeSharedMems.size()), dataMem);
        m_sizeSharedMems.insert(QString(m_sizeSharedMems.size()), sizeMem);
    }
    qDebug() << "55555555";
    return  ret;


}

ShareMemoryInfo AuthSharedMemoryManager::getRunnableTag()
{
    ShareMemoryInfo defaultInfo;
    defaultInfo.isStart = false;

    bool ret =  getMemomData<ShareMemoryInfo>(&m_stopSharedMem, defaultInfo);
    if (ret) {
        return defaultInfo;
    } else {
        ShareMemoryInfo defaultInfors;
        defaultInfors.isStart = false;
        return defaultInfors;
    }
}





void AuthSharedMemoryManager::init()
{



}



