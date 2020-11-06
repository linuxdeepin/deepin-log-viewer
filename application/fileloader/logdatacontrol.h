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

#include <QObject>
template < typename DataType, typename FilterType >
class LoghDataControl : public QObject
{
    //  Q_OBJECT
public:
    explicit LoghDataControl(QObject *parent = nullptr): QObject(parent)
    {

    }
    virtual ~LoghDataControl()
    {

    }
public:
    virtual QStringList loadFileByDir(QString iDir) = 0;
signals:
    // void dataRecived(const QList <DataType> &iData);
    void dataFinished(bool isSuccess);
    void dataError(const QString &iError);
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
    QList <DataType> getData(int iFrom, int iTo);
    qint64 getCount();
    bool getIsComplete();
    void start()
    {
        QStringList fileDirs =   loadFileByDir(m_LogDir);


    }

    FilterType m_Filter;
    QString m_LogDir;


};

#endif // LOGHDATACONTROL_H
