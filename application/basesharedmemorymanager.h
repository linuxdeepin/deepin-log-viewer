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
#ifndef BASESHAREDMEMORYMANAGER_H
#define BASESHAREDMEMORYMANAGER_H

#include <QObject>
#include <QSharedMemory>
#include<QDebug>
class BaseSharedMemoryManager : public QObject
{
    Q_OBJECT
public:
    explicit BaseSharedMemoryManager(QObject *parent = nullptr)  : QObject(parent)
    {

    }


signals:

public :
    bool initShareMemory(QSharedMemory **iMem, const QString &iTag,  QSharedMemory::AccessMode mode = QSharedMemory::ReadWrite)
    {
        bool result = false;
        if ((*iMem)) {
            result =   releaseMemory(iMem);
            if (!result) {
                return  false;
            }
        }

        (*iMem) = new QSharedMemory(iTag, this);
        if ((*iMem)->isAttached())      //检测程序当前是否关联共享内存
            (*iMem)->detach();          //解除关联
        result = (*iMem)->attach(mode);
        if (!result) {
            releaseMemory(iMem);
        }
        return  result ;
    }
    bool releaseMemory(QSharedMemory **iMem)
    {
        bool result = false;
        if ((*iMem)) {
            result = (*iMem)->unlock();
            if ((*iMem)->isAttached()) {     //检测程序当前是否关联共享内存
                result = (*iMem)->detach();
                qDebug() << "releaseMemory name:" << (*iMem)->key() << "-- isSuccess" << result << "-- isAttached:" << (*iMem)->isAttached() << "-- size:" << (*iMem)->size();
                return  result;
            } else {
                return true;
            }
        } else {
            return true;
        }
    }
    template<typename T>
    bool createShareMemoryWrite(QSharedMemory **iMem, const QString &iTag,  int iSize = -1)
    {
        qDebug() << "createShareMemoryWrite" << "111111111";
        bool result = false;
        if (iMem) {
            result =   releaseMemory(iMem);
            if (!result) {
                return  false;
            }
        }
        qDebug() << "createShareMemoryWrite" << "22222";
        *iMem = new QSharedMemory(iTag, this);

        if ((*iMem)->isAttached())      //检测程序当前是否关联共享内存
            (*iMem)->detach();
        int size = iSize > 0 ? iSize : sizeof(T);
        if (!(*iMem)->create(size)) {     //创建共享内存，大小为size
            qDebug() << "ShareMemory create error" << (*iMem)->key() << QSharedMemory:: SharedMemoryError((*iMem)->error()) << (*iMem)->errorString();
            if ((*iMem)->isAttached())      //检测程序当前是否关联共享内存
                (*iMem)->detach();
            result = (*iMem)->attach();
        } else {
            qDebug() << "createShareMemoryWrite" << "3333333";
            // 创建好以后，保持共享内存连接，防止释放。
            result = (*iMem)->attach();
            // 主进程：首次赋值m_pShareMemoryInfo
        }
        if (!result) {
            qDebug() << "ShareMemory attach error" << result << (*iMem)->key() << QSharedMemory:: SharedMemoryError((*iMem)->error()) << (*iMem)->errorString();
            releaseMemory(iMem);
        }
        return  result ;

    }
    template<typename T>
    bool setMemomData(QSharedMemory **iMem, T *iData, qint64 size = -1)
    {
        if (!(*iMem) || !(*iMem)->isAttached()) {
            return false;
        }
        if (!(*iMem)->lock()) {
            return false;
        }
        T *dataInfo = static_cast<T *>((*iMem)->data());

        if (!dataInfo) {
            (*iMem)->unlock();
            return  false;
        }
        // memcpy(dataInfo, iData, size > 0 ? size : (*iMem)->size());
        dataInfo = iData;
        return (*iMem)->unlock();
    }
    template<typename T>
    bool getMemomData(QSharedMemory **iMem, T &oData)
    {
        if (!(*iMem) || !(*iMem)->isAttached()) {
            return false;
        }
        if (!(*iMem)->lock()) {
            return false;
        }
        T *dataInfo = static_cast<T *>((*iMem)->data());
        if (!dataInfo) {
            (*iMem)->unlock();
            return  false;
        }
        oData = *dataInfo;
        return (*iMem)->unlock();
    }

    template<typename T>
    bool getMemomDataLong(QSharedMemory **iMem, T **oData)
    {
        if (!iMem || !(*iMem)->isAttached()) {
            return false;
        }
        if (!(*iMem)->lock()) {
            return false;
        }
        *oData = static_cast<T *>((*iMem)->data());
        if (!(*oData)) {
            (*iMem)->unlock();
            return  false;
        }
        return true;
    }

};
#endif // BASESHAREDMEMORYMANAGER_H

