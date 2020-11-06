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
#ifndef LOGFILELOADER_H
#define LOGFILELOADER_H

#include <QObject>
#include <QFile>
#include <QString>
#include <QVector>
#include <QDebug>

#include <future>
template < typename DataType, typename FilterType >
class LogFileLoader : public QObject
{
    //Q_OBJECT
public:
    explicit LogFileLoader(QObject *parent = nullptr, const QString &iFilePath = "")
        : QObject(parent)
        , m_fileMem(nullptr)
        , m_size(0)
        , m_canOpenProgress(false)
        , m_dataCur(-1)
        , m_dataFrom(-1)
        , m_dataTo(-1)
    {

    }
    virtual ~LogFileLoader()
    {
        close();
    }
signals:

public slots:
    bool openLoadData(const QString &iFilePath)
    {
        m_canOpenProgress = true;
        close();
        m_file =  new QFile(iFilePath);
        if (!m_file->open(QIODevice::ReadOnly)) {
            qWarning() << "open file path falied";
            m_canOpenProgress = false;
            close();
            return false;
        }
        m_size = m_file->size();
        if (m_size <= 0) {
            qDebug() << "file is empty";
            m_canOpenProgress = false;
            close();
            return false;
        }
        auto tempMem = m_file->map(0, m_size, QFileDevice::MapPrivateOption);
        tempMem[m_size - 1] = '\0';
        m_fileMem = (char *)(tempMem);
        m_enters.push_back(-1);
        m_enterCharOffset = 2;
        char *firstlineData = strchr(m_fileMem, '\n');
        if (firstlineData != nullptr) {
            if (firstlineData == m_fileMem || *(firstlineData - 1) != '\r') {
                m_enterCharOffset = 1;
            }
            --m_enterCharOffset;
            splitLines();
        }

        if (!m_canOpenProgress)
            return false;

        if (m_enters.last() < m_size) {//没有\n的最后一行
            qDebug() << "add last line";
            if (m_size >= 2 && m_fileMem[m_size - 2] == '\r') { //\r\0
                m_enters.push_back(m_size - 2);
            } else {
                m_enters.push_back(m_size - 1);
            }
        }



        m_lineCnt = m_enters.size() - 1;

        //detectTextCodec();
        m_canOpenProgress = false;
    }
    void close()
    {
        if (m_file) {
            m_enters.clear();
            if (m_fileMem) {
                m_file->unmap((uchar *)m_fileMem);
                m_fileMem = nullptr;
            }
            m_file->close();
            delete m_file;
            m_file = nullptr;
        }
    }
    void stopOpenProgress()
    {
        m_canOpenProgress = false;
    }
    void splitLines()
    {
        const qint64 blockSize = 524288000; //500M
        int extraParts = m_size / blockSize;

        if (m_size - extraParts * blockSize == 0) {//临界情况，刚好是500M的整数倍
            extraParts -= 1;
        }

        qDebug() << "open using extra thread count: " << extraParts;

        m_dataCur = 0;
        m_dataFrom = 1;
        m_dataTo = m_size / 100; //按一行100个字符估计总行数

        auto *extraEnters = new QVector<qint64>[extraParts];
        auto *extraRets = new std::future<void>[extraParts];
        auto *chBackup = new char[extraParts];
        for (int i = 0; i < extraParts; i++) {
            qint64 pos = blockSize * (i + 1);
            QVector<qint64> *enters = &extraEnters[i];
            enters->clear();

            //临时把mMem[pos]修改为0，方便分段处理
            chBackup[i] = m_fileMem[pos];
            if (chBackup[i] == '\n') {
                enters->push_back(i - m_enterCharOffset);
            }
            m_fileMem[pos] = 0;

            extraRets[i] = async(std::launch::async, [ &, enters] {
                splitLine(&m_dataCur, enters, m_fileMem + pos + 1, false);
            });
        }

        splitLine(&m_dataCur, &m_enters, m_fileMem, true);//FIXME:extraParts>0的时候进度统计的有问题

        for (int i = 0; i < extraParts; i++) {
            extraRets[i].wait();

            //恢复mMem[pos]
            qint64 pos = blockSize * (i + 1);
            m_fileMem[pos] = chBackup[i];

            //合并结果
            m_enters.append(extraEnters[i]);
            QVector<qint64>().swap(extraEnters[i]);//提前释放内存
        }

        delete[] extraEnters;
        delete[] extraRets;
        delete[] chBackup;
    }
    void splitLine(int &iCur, QVector<qint64> *iEnters, char *ptr, bool isProgress)
    {
        while (m_canOpenProgress && (ptr = strchr(ptr, '\n')) != nullptr) {
            iEnters->push_back(ptr - m_fileMem - m_enterCharOffset);//定位在\r（如有）或\n上
            if (isProgress)
                ++iCur;
            ++ptr;
        }
    }
public:
    QFile *m_file;
    QVector<qint64> m_enters;
    char *m_fileMem;
    qint64 m_size;
    int m_enterCharOffset;
    int m_lineCnt;
    bool m_canOpenProgress;
    int m_dataCur;
    int m_dataFrom;
    int m_dataTo; //按一行100个字符估计总行数

};

#endif // LOGFILELOADER_H
