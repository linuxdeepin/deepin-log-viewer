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
#include "logauththread.h"
#include "sharedmemorymanager.h"

#include <QDate>
#include <QDebug>
#include <QProcess>
#include <QFile>
TEST(LogAuthThread_Constructor_UT, LogAuthThread_Constructor_UT)
{
    LogAuthThread *p = new LogAuthThread(nullptr);
    EXPECT_NE(p, nullptr);
    p->deleteLater();
}

TEST(LogAuthThread_Destructor_UT, LogAuthThread_Destructor_UT)
{
    LogAuthThread *p = new LogAuthThread(nullptr);
    EXPECT_NE(p, nullptr);
    p->~LogAuthThread();
    EXPECT_EQ(p->m_process, nullptr);
}

TEST(LogAuthThread_getStandardOutput_UT, LogAuthThread_getStandardOutput_UT)
{
    LogAuthThread *p = new LogAuthThread(nullptr);
    EXPECT_NE(p, nullptr);
    p->getStandardOutput();
    p->deleteLater();
}
TEST(LogAuthThread_getStandardError_UT, LogAuthThread_getStandardError_UT)
{
    LogAuthThread *p = new LogAuthThread(nullptr);
    EXPECT_NE(p, nullptr);
    p->getStandardError();
    p->deleteLater();
}

class LogAuthThread_stopProccess_UT_Param
{
public:
    LogAuthThread_stopProccess_UT_Param(bool iIsStop, bool iIsProcess)
    {
        isStop = iIsStop;
        isProcess = iIsProcess;
    }
    bool isStop;
    bool isProcess;
};

class LogAuthThread_stopProccess_UT : public ::testing::TestWithParam<LogAuthThread_stopProccess_UT_Param>
{
};

INSTANTIATE_TEST_CASE_P(LogApplicationParseThread, LogAuthThread_stopProccess_UT, ::testing::Values(LogAuthThread_stopProccess_UT_Param(false, true), LogAuthThread_stopProccess_UT_Param(true, true), LogAuthThread_stopProccess_UT_Param(true, false)));

TEST_P(LogAuthThread_stopProccess_UT, LogAuthThread_stopProccess_UT_001)
{
    LogAuthThread *p = new LogAuthThread(nullptr);
    EXPECT_NE(p, nullptr);
    LogAuthThread_stopProccess_UT_Param param = GetParam();
    p->m_process = new QProcess();
    p->m_isStopProccess = param.isStop;
    p->m_process = param.isProcess ? p->m_process : nullptr;
    p->stopProccess();
    p->m_process->deleteLater();
    p->deleteLater();
}

class LogAuthThread_run_UT_Param
{
public:
    LogAuthThread_run_UT_Param(LOG_FLAG iType)
    {
        type = iType;
    }
    LOG_FLAG type;
};

bool stub_isAttached()
{
    return true;
}

bool stub_exists001()
{
    return true;
}

QByteArray stub_readAllStandardOutput001()
{
    return "[20]2[0]-0[7]-03,  [19:19:18.639,dsadj,dasjdajsd,  [adsdas.dasdasd,    [sad, asd. asd  .";
}

void LogAuthThread_handleBoot_UT_QProcess_start(void *, const QString &program, const QStringList &arguments, QProcess::OpenMode mode = QProcess::ReadWrite)
{
    qDebug() << "LogAuthThread_handleBoot_UT_start--";
}

TEST(LogAuthThread_handleBoot_UT, LogAuthThread_handleBoot_UT)
{
    LogAuthThread *p = new LogAuthThread(nullptr);
    EXPECT_NE(p, nullptr);
    Stub stub;
    stub.set((void (QProcess::*)(const QString &, const QStringList &, QProcess::OpenMode mode))ADDR(QProcess, start), LogAuthThread_handleBoot_UT_QProcess_start);
    stub.set((bool (QFile::*)() const)ADDR(QFile, exists), stub_exists001);
    p->m_canRun = true;
    p->handleBoot();
    p->deleteLater();
}

TEST(LogAuthThread_handleKern_UT, LogAuthThread_handleKern_UT)
{
    LogAuthThread *p = new LogAuthThread(nullptr);
    EXPECT_NE(p, nullptr);
    Stub stub;
    stub.set((void (QProcess::*)(const QString &, const QStringList &, QProcess::OpenMode mode))ADDR(QProcess, start), LogAuthThread_handleBoot_UT_QProcess_start);
    stub.set(ADDR(SharedMemoryManager, isAttached), stub_isAttached);
    stub.set(ADDR(QProcess, readAllStandardOutput), stub_readAllStandardOutput001);
    p->m_canRun = true;
    p->handleKern();
    p->deleteLater();
}

TEST(LogAuthThread_handleKwin_UT, LogAuthThread_handleKwin_UT)
{
    LogAuthThread *p = new LogAuthThread(nullptr);
    EXPECT_NE(p, nullptr);
    Stub stub;
    stub.set(ADDR(QProcess, readAllStandardOutput), stub_readAllStandardOutput001);
    stub.set((bool (QFile::*)() const)ADDR(QFile, exists), stub_exists001);
    p->m_canRun = true;
    p->handleKwin();
    p->deleteLater();
}

TEST(LogAuthThread_handleXorg_UT, LogAuthThread_handleXorg_UT)
{
    LogAuthThread *p = new LogAuthThread(nullptr);
    EXPECT_NE(p, nullptr);
    p->m_canRun = true;
    Stub stub;
    stub.set(ADDR(QProcess, readAllStandardOutput), stub_readAllStandardOutput001);
    p->handleXorg();
    p->deleteLater();
}

TEST(LogAuthThread_handleXorg_UT, LogAuthThread_handleNormal_UT)
{
    LogAuthThread *p = new LogAuthThread(nullptr);
    EXPECT_NE(p, nullptr);
    p->m_canRun = true;
    Stub stub;
    stub.set(ADDR(QProcess, readAllStandardOutput), stub_readAllStandardOutput001);
    p->handleNormal();
    p->deleteLater();
}

TEST(LogAuthThread_handleDkpg_UT, LogAuthThread_handleDkpg_UT)
{
    LogAuthThread *p = new LogAuthThread(nullptr);
    EXPECT_NE(p, nullptr);
    p->m_canRun = true;
    Stub stub;
    stub.set(ADDR(QProcess, readAllStandardOutput), stub_readAllStandardOutput001);
    p->handleDkpg();
    p->deleteLater();
}

TEST(LogAuthThread_initProccess_UT, LogAuthThread_initProccess_UT)
{
    LogAuthThread *p = new LogAuthThread(nullptr);
    EXPECT_NE(p, nullptr);
    p->initProccess();
    p->deleteLater();
}

int yearStub(void *obj)
{
    return 2020;
}
//TEST(LogAuthThread_formatDateTime_UT, LogAuthThread_formatDateTime_UT)
//{
//    LogAuthThread *p = new LogAuthThread(nullptr);
//    EXPECT_NE(p, nullptr);
//    Stub stub;
//    stub.set(ADDR(QDate, year), yearStub);
//    qint64 result = p->formatDateTime("Oct", "21", "09:50:16");
//    qint64 standard = 1603245016000;
//    // EXPECT_EQ(result, standard);
//    p->deleteLater();
//}

TEST(LogAuthThread_onFinished_UT, LogAuthThread_onFinished_UT_001)
{
    LogAuthThread *p = new LogAuthThread(nullptr);
    EXPECT_NE(p, nullptr);
    p->m_type = Normal;
    p->onFinished(0);
    p->deleteLater();
}

TEST(LogAuthThread_onFinished_UT, LogAuthThread_onFinished_UT_002)
{
    LogAuthThread *p = new LogAuthThread(nullptr);
    EXPECT_NE(p, nullptr);
    p->initProccess();
    EXPECT_NE(p->m_process, nullptr);
    p->m_type = KERN;
    p->onFinished(0);
    p->deleteLater();
}

TEST(LogAuthThread_kernDataRecived_UT, LogAuthThread_kernDataRecived_UT)
{
    LogAuthThread *p = new LogAuthThread(nullptr);
    EXPECT_NE(p, nullptr);
    p->initProccess();
    EXPECT_NE(p->m_process, nullptr);
    p->kernDataRecived();
    p->deleteLater();
}
