/*
* Copyright (C) 2019 ~ 2020 UnionTech Software Technology Co.,Ltd
*
* Author:     linxun <linxun@uniontech.com>
* Maintainer:  linxun <linxun@uniontech.com>
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

#include "logapplicationparsethread.h"
#include "logfileparser.h"
#include "structdef.h"

#include <stub.h>

#include <QDebug>
#include <QDir>
#include <QFileInfo>

#include <gtest/gtest.h>

QFileInfoList stub_entryInfoList(QDir::Filters filters, QDir::SortFlags sort)
{
    QFileInfo fileInfo("test.txt");
    QList<QFileInfo> m_list {fileInfo};
    return m_list;
}

QStringList stub_split(QChar sep, QString::SplitBehavior behavior,
                       Qt::CaseSensitivity cs)
{
    return QStringList() << "t.em,s[.a]da[],m[.s]d,s[t.][,"
                         << "jsd[sa[dds.,]"
                         << "ds[ds..,/n"
                         << "test2.[df,f";
}

class LogApplicationParseThread_UT : public testing::Test
{
public:
    //添加日志
    static void SetUpTestCase()
    {
        qDebug() << "SetUpTestCase" << endl;
    }
    static void TearDownTestCase()
    {
        qDebug() << "TearDownTestCase" << endl;
    }
    void SetUp() //TEST跑之前会执行SetUp
    {
        m_logAppThread = new LogApplicationParseThread();
        qDebug() << "SetUp" << endl;
    }
    void TearDown() //TEST跑完之后会执行TearDown
    {
        m_logAppThread->m_process = new QProcess();
        delete m_logAppThread;
    }
    LogApplicationParseThread *m_logAppThread;
};

TEST_F(LogApplicationParseThread_UT, LogApplicationParseThread_UT001)
{
    APP_FILTERS appfilter;
    Stub stub;
    stub.set((QList<QFileInfo>(QDir::*)(QDir::Filters, QDir::SortFlags) const)ADDR(QDir, entryInfoList), stub_entryInfoList);
    stub.set((QStringList(QString::*)(QChar, QString::SplitBehavior, Qt::CaseSensitivity) const)ADDR(QString, split), stub_split);
    m_logAppThread->initProccess();
    m_logAppThread->setParam(appfilter);
    m_logAppThread->m_AppFiler.path = "/home/test";
    m_logAppThread->doWork();
    m_logAppThread->run();
    m_logAppThread->onProcFinished(1);
}
