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
#define LOG_SIZE_INFO_TAG_PREFIX "LOGAUTHSIZE"
#define LOG_FILE_DATA_TAG_PREFIX "LOGAUTHFILE"
std::atomic<SharedMemoryManager *> SharedMemoryManager::m_instance;
std::mutex SharedMemoryManager::m_mutex;
SharedMemoryManager::SharedMemoryManager(QObject *parent)
    :  BaseSharedMemoryManager(parent)
{
    init();
}
void SharedMemoryManager::setRunnableTag(ShareMemoryInfo iShareInfo)
{
    setMemomData<ShareMemoryInfo>(&m_commondM, &iShareInfo);

}

QString SharedMemoryManager::getRunnableKey()
{
    return  m_commondM->key();
}

bool SharedMemoryManager::isAttached()
{
    return (m_commondM && m_commondM->isAttached());
}

void SharedMemoryManager::releaseAllMem()
{
    releaseMemory(&m_commondM);
    foreach (QSharedMemory *item, m_sizeSharedMems.values()) {
        releaseMemory(&item);
    }
    foreach (QSharedMemory *item, m_dataSharedMems.values()) {
        releaseMemory(&item);
    }
}



bool SharedMemoryManager::initSizeInfo()
{
    QSharedMemory *sizeMem = nullptr;
    QString sizeTag = LOG_SIZE_INFO_TAG_PREFIX + QString::number(0);
    if (initShareMemory(&sizeMem, sizeTag, QSharedMemory::ReadOnly)) {
        m_sizeSharedMems.insert(sizeTag, sizeMem);
        return  true;
    } else {
        return  false;
    }

}

QStringList SharedMemoryManager::getSizeInfoAllTag()
{
    return m_sizeSharedMems.keys();

}

bool SharedMemoryManager::initDataInfo(const QString &iTag)
{
    QSharedMemory *dataMem = nullptr;
    if (initShareMemory(&dataMem, iTag, QSharedMemory::ReadOnly)) {
        m_dataSharedMems.insert(iTag, dataMem);
        return  true;
    } else {
        return  false;
    }
}

QStringList SharedMemoryManager::getDataInfoAllTag()
{
    return m_dataSharedMems.keys();
}
bool SharedMemoryManager::getSizeInfoByTag(const QString &iTag, ShareMemorySizeInfo &oInfo)
{
    QSharedMemory *mem = m_sizeSharedMems.value(iTag, nullptr);
    if (!mem) {
        return  false;
    }

    return  getMemomData<ShareMemorySizeInfo>(&mem, oInfo);
}

bool SharedMemoryManager::getDataByTag(const QString &iTag, char **oData)
{
    QSharedMemory *mem = m_dataSharedMems.value(iTag, nullptr);
    if (!mem) {
        return  false;
    }

    return    getMemomDataLong<char>(&mem, oData);
}



void SharedMemoryManager::init()
{
    QString tag = LOG_POLIKIT_STOP_TAG ;
    createShareMemoryWrite<ShareMemoryInfo>(&m_commondM, tag);
}

