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
#include <gtest/gtest.h>
#include <stub.h>
#include "logapplicationparsethread.h"
#include "structdef.h"
#include <QDebug>
#include <QProcess>
TEST(LogApplicationParseThread_Constructor_UT, LogApplicationParseThread_Constructor_UT)
{
    LogApplicationParseThread *p = new LogApplicationParseThread(nullptr);
    EXPECT_NE(p, nullptr);
    p->deleteLater();
}

TEST(LogApplicationParseThread_Destructor_UT, LogApplicationParseThread_Destructor_UT_001)
{
    LogApplicationParseThread *p = new LogApplicationParseThread(nullptr);
    EXPECT_NE(p, nullptr);
    p->~LogApplicationParseThread();
    EXPECT_EQ(p->m_process, nullptr);
    //  delete  p;
}

TEST(LogApplicationParseThread_setParam_UT, LogApplicationParseThread_setParam_UT_001)
{
    LogApplicationParseThread *p = new LogApplicationParseThread(nullptr);
    EXPECT_NE(p, nullptr);
    APP_FILTERS filter;
    p->setParam(filter);
    EXPECT_EQ(p->m_AppFiler.timeFilterBegin, filter.timeFilterBegin);
    EXPECT_EQ(p->m_AppFiler.timeFilterEnd, filter.timeFilterEnd);
    EXPECT_EQ(p->m_AppFiler.lvlFilter, filter.lvlFilter);
    EXPECT_EQ(p->m_AppFiler.path, filter.path);
    p->deleteLater();
}

TEST(LogApplicationHelper_stopProccess_UT, LogApplicationHelper_stopProccess_UT)
{
    LogApplicationParseThread *p = new LogApplicationParseThread(nullptr);
    EXPECT_NE(p, nullptr);
    p->m_process = new QProcess();
    p->stopProccess();
    EXPECT_EQ(p->m_canRun, false);
    EXPECT_EQ(p->m_process->isOpen(), false);
    p->m_process->deleteLater();
    p->deleteLater();
}

class LogApplicationParseThread_doWork_UT_Param
{
public:
    LogApplicationParseThread_doWork_UT_Param(bool iIsPathEmpty, bool iIsTimeEmpty, bool iIsLvlEmpty)
    {
        isPathEmpty = iIsPathEmpty;
        isTimeEmpty = iIsTimeEmpty;
        isLvlEmpty = iIsLvlEmpty;
    }
    bool isPathEmpty;
    bool isTimeEmpty;
    bool isLvlEmpty;


};

class LogApplicationParseThread_doWork_UT : public ::testing::TestWithParam<LogApplicationParseThread_doWork_UT_Param>
{
};

INSTANTIATE_TEST_CASE_P(LogApplicationParseThread, LogApplicationParseThread_doWork_UT, ::testing::Values(LogApplicationParseThread_doWork_UT_Param(false, true, true)
                                                                                                          , LogApplicationParseThread_doWork_UT_Param(true, true, true)
                                                                                                          , LogApplicationParseThread_doWork_UT_Param(true, false, false)
                                                                                                         ));

QByteArray stub_readAllStandardOutput()
{
    return "2020-07-03, 19:19:18.639,dsadj,dasjdajsd,adsdas.dasdasd,sad,asd.asd.";
}

TEST_P(LogApplicationParseThread_doWork_UT, LogApplicationParseThread_doWork_UT_001)
{
    LogApplicationParseThread_doWork_UT_Param param = GetParam();
    LogApplicationParseThread *p = new LogApplicationParseThread(nullptr);
    EXPECT_NE(p, nullptr);
    APP_FILTERS filter;
    filter.path = param.isPathEmpty ? "" : "../sources/dde-calendar.log";
    filter.lvlFilter = param.isLvlEmpty ? LVALL : INF;
    filter.timeFilterBegin = param.isTimeEmpty ? -1 : 0;
    filter.timeFilterEnd = param.isTimeEmpty ? -1 : 2 ^ 10;
    p->doWork();
    p->deleteLater();
}

TEST_P(LogApplicationParseThread_doWork_UT, LogApplicationParseThread_doWork_UT_002)
{
    Stub stub;
    stub.set(ADDR(QProcess, readAllStandardOutput), stub_readAllStandardOutput);
    LogApplicationParseThread_doWork_UT_Param param = GetParam();
    LogApplicationParseThread *p = new LogApplicationParseThread(nullptr);
    EXPECT_NE(p, nullptr);
    APP_FILTERS filter;
    filter.path = param.isPathEmpty ? "" : "../sources/dde-calendar.log";
    filter.lvlFilter = param.isLvlEmpty ? LVALL : INF;
    filter.timeFilterBegin = param.isTimeEmpty ? -1 : 0;
    filter.timeFilterEnd = param.isTimeEmpty ? -1 : 2 ^ 10;
    p->m_AppFiler.path = "test";
    p->m_AppFiler.timeFilterBegin = 20;
    p->m_AppFiler.timeFilterEnd = 20;
    p->m_AppFiler.lvlFilter = LVALL;
    p->doWork();
    p->deleteLater();
}

TEST(LogApplicationParseThread_onProcFinished_UT, LogApplicationParseThread_onProcFinished_UT)
{
    LogApplicationParseThread *p = new LogApplicationParseThread(nullptr);
    EXPECT_NE(p, nullptr);
    p->onProcFinished(0);
    p->deleteLater();
}

TEST(LogApplicationParseThread_initMap_UT, LogApplicationParseThread_initMap_UT)
{
    LogApplicationParseThread *p = new LogApplicationParseThread(nullptr);
    EXPECT_NE(p, nullptr);
    p->initMap();
    EXPECT_EQ(p->m_levelDict.value("Warning"), WARN);
    EXPECT_EQ(p->m_levelDict.value("Debug"), DEB);
    EXPECT_EQ(p->m_levelDict.value("Info"), INF);
    EXPECT_EQ(p->m_levelDict.value("Error"), ERR);
    p->deleteLater();
}


TEST(LogApplicationParseThread_initProccess_UT, LogApplicationParseThread_initProccess_UT)
{
    LogApplicationParseThread *p = new LogApplicationParseThread(nullptr);
    EXPECT_NE(p, nullptr);
    p->initProccess();
    EXPECT_NE(p->m_process, nullptr);
    p->deleteLater();
}
