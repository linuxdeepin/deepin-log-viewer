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
#ifndef LOGHDATACONTROL_H
#define LOGHDATACONTROL_H
#include "logfileloader.h"

#include <QObject>
#include <QThread>
#include <QVector>
template < typename DataType, typename FilterType >
class LoghDataControl : public QObject
{
    //  Q_OBJECT
    template< typename T>
    struct LogIndexData {
        int fileIndex;
        int dataIndex;
        T data;
    };
public:
    explicit LoghDataControl(QObject *parent = nullptr)
        : QObject(parent)
        , m_currrentShowFileIndex(0)
        , m_pageSize(50)

    {

    }
    virtual ~LoghDataControl()
    {

    }
public:
    virtual QStringList loadFileByDir(QString iDir) = 0;
    void releaseAllFiles()
    {
        foreach (LogFileLoader *item, m_files.values()) {
            item->deleteLater();
        }
    }
    void addFileLoader(const QString &iFilePath)
    {
        LogFileLoader *itemLoader = new LogFileLoader(iFilePath);
        QThread *itemThread = new QThread;
        itemLoader->moveToThread(itemThread);
        connect(itemThread, &QThread::finished, itemThread, &QThread::deleteLater);
        connect(itemLoader, &LogFileLoader::destroyed, itemThread, &QThread::quit);
        connect(itemThread, &QThread::finished, itemLoader, SLOT(deleteLater()));
        connect(this, &LoghDataControl::stopOpenProgress, itemLoader, &LogFileLoader::stopOpenProgress);
        connect(this, &LoghDataControl::startOpen, itemLoader, &LogFileLoader::openLoadData);
        connect(itemLoader, &LogFileLoader::firstPageFinished, this, &LoghDataControl::singleDataFirstPageFinished);
        connect(itemLoader, &LogFileLoader::dataFinished, this, &LoghDataControl::singleDataFinished);
        int key = m_files.size();
        m_files.insert(key, itemLoader);
        m_threads.insert(key, itemThread);
        itemThread->start();
    }
signals:
    // void dataRecived(const QList <DataType> &iData);
    void singleDataFinished();
    void singleDataFirstPageFinished();
    void dataError(const QString &iError);
    void stopOpenProgress();
    void startOpen();
    void onePageDataFinished();
public slots:
//   void exitProg();
    void setFilter(const FilterType &iFilter)
    {
        m_Filter = iFilter;
    }
    void setDir(const QString &iDir)
    {
        m_LogDir = iDir;
    }
    //  QList <DataType> getData(int iFrom, int iTo);
    qint64 getCount()
    {
        qint64 count = 0;
        foreach (LogFileLoader *file, m_files) {
            if (file) {
                count += file->getCount();
            }
        }
        return count;
    }
    bool getIsComplete()
    {
        bool result = true;
        foreach (LogFileLoader *file, m_files) {
            if (file) {
                result &= file->getIsComplete();
            }
        }
        return result;
    }
    void start()
    {
        QStringList fileDirs =   loadFileByDir(m_LogDir);
    }
    virtual  bool getSingleDataFromlineCnt(int iCount, DataType &oItemData) = 0;

    bool refreshPage(bool isAdd, bool iReset = false)
    {
        QString str ;
        DataType itemData;
        //   QList<DataType>　rs_abc;
//        bool isContainsStr = false;
//        if (m_kernFilter.searchstr.isEmpty()) {
//            isContainsStr = true;
//        }
        if (iReset) {
            m_currrentShowFileIndex = 0;
            foreach (LogFileLoader *itemFile, m_files.values()) {
                itemFile->m_curentDataLineRage.first = 0;
                itemFile->m_curentDataLineRage.second = 0;
            }
        }
        int count = 0;
        for (; m_currrentShowFileIndex < m_files.size(); ++m_currrentShowFileIndex) {
            while (count < m_pageSize ||) {

            }
            LogFileLoader *file = m_files.value(m_currrentShowFileIndex, nullptr);
            if (!file || !(file->getIsComplete())) {
                return false ;
            }



            if (isAdd) {
                if (file->m_curentDataLineRage.second >= file->m_lineCnt) {
                    return  false;
                }
            } else {
                if (file->m_curentDataLineRage.first < 0) {
                    return false;
                }
            }
            if (isAdd) {
                for (int i = file->m_curentDataLineRage.second ; i < file->m_lineCnt; ++i) {
                    file->m_curentDataLineRage.second = i;
                    file->m_curentDataLineRage.first ++;
                    if (!getSingleDataFromlineCnt(i, itemData)) {
                        continue;
                    }
                    m_currentData .push_back(LogIndexData<DataType>(m_currrentShowFileIndex, i, itemData));
                    if (count < m_pageSize) {
                        count++;
                    } else {

                        break;
                    }
                }
            } else {
                for (int i = file->m_curentDataLineRage.first ; i > 0; --i) {
                    file->m_curentDataLineRage.first = i;
                    if (!getSingleDataFromlineCnt(i, itemData)) {
                        continue;
                    }
                    m_currentData .push_front(LogIndexData<DataType>(m_currrentShowFileIndex, i, itemData));
                    if (count < m_pageSize) {
                        count++;
                    } else {
                        break;
                    }
                }
            }
            if (count >= m_pageSize) {
                break;
            }
        }
        LogIndexData<DataType>　itemdata;
        int lastFileIndex = -1;
        int lastDataIndex = -1;
        QPair<int, int>　dataRange;
        if (count > 0 && m_currentData.size() > m_pageSize * 2) {
            int limit = -1;
            if (isAdd) {
                for (int i = 0; i < m_pageSize; ++i) {
                    itemdata =  m_currentData.takeFirst();
                    lastFileIndex = itemData.fileIndex;
                    lastDataIndex = itemData.dataIndex;
                }
                foreach (int key, m_LineRage.keys()) {
                    if (key < lastFileIndex) {
                        m_LineRage.remove(key);
                    } else if (key == lastFileIndex) {
                        QPair<int, int> limit = m_LineRage[key];
                        limit.first = lastDataIndex + 1;
                    }
                }
            } else {
                for (int i = 0; i < m_pageSize; ++i) {
                    itemdata =  m_currentData.takeFlast();
                    lastFileIndex = itemData.fileIndex;
                    lastDataIndex = itemData.dataIndex;
                }
                foreach (int key, m_LineRage.keys()) {
                    if (key > lastFileIndex) {
                        m_LineRage.remove(key);
                    } else if (key == lastFileIndex) {
                        QPair<int, int> limit = m_LineRage[key];
                        limit.second = lastDataIndex - 1;
                    }
                }
            }
//            m_currentData.erase(iterBegin, iterEnd);
        }
        return true;

    }
    FilterType m_Filter;
    QString m_LogDir;
    QMap<int, LogFileLoader *> m_files;
    QMap<int, QThread *> m_threads;
    QList<LogIndexData<DataType>> m_currentData;

    QMap<int, QPair<int, int>> m_LineRage;
    int m_currrentShowFileIndex;
    int m_pageSize ;



};

#endif // LOGHDATACONTROL_H
