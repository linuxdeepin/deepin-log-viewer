// SPDX-FileCopyrightText: 2019-2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "logoocfileparsethread.h"
#include "logfileparser.h"
#include "structdef.h"
#include "dbusproxy/dldbushandler.h"
#include "ut_stubpublic.h"
#include "../../application/qtcompat.h"

#include <stub.h>

#include <QDebug>
#include <QDir>
#include <QFileInfo>

#include <gtest/gtest.h>

QStringList stub_getOtherFileInfo(const QString &flag)
{
    Q_UNUSED(flag);
    return QStringList() << "test";
}

bool stub_ooc_checkAuthentication_true(const QString &path)
{
    Q_UNUSED(path);
    return true;
}

class LogOOCFileParseThread_UT : public testing::Test
{
public:
    //添加日志
    static void SetUpTestCase()
    {
        qDebug() << "SetUpTestCase" << ENDL;
    }
    static void TearDownTestCase()
    {
        qDebug() << "TearDownTestCase" << ENDL;
    }
    void SetUp()//TEST跑之前会执行SetUp
    {
        m_logThread = new LogOOCFileParseThread();
        qDebug() << "SetUp" << ENDL;
    }
    void TearDown() //TEST跑完之后会执行TearDown
    {
        delete m_logThread;
    }
    LogOOCFileParseThread *m_logThread;
};

QString stub_ooc_readLog(const QString &filePath)
{
    Q_UNUSED(filePath);
    return "2021-04-06 13:29:32 install code:amd64 <none> 1.55.0-1617120720";
}

TEST_F(LogOOCFileParseThread_UT, UT_InitProccess_001){
    m_logThread->initProccess();
    EXPECT_EQ(m_logThread->getProcess().isNull(), false);
}

TEST_F(LogOOCFileParseThread_UT, UT_getProcess_001){
    m_logThread->getProcess();
}

TEST_F(LogOOCFileParseThread_UT, UT_SetParam_001){
    QString path("test");
    m_logThread->setParam(path);
    EXPECT_EQ(m_logThread->getPath(), path);
}

TEST_F(LogOOCFileParseThread_UT, UT_getPath_001){
    m_logThread->getPath();
}

TEST_F(LogOOCFileParseThread_UT, UT_DoWork_001)
{
    Stub stub;
    stub.set(ADDR(DLDBusHandler, readLog), stub_ooc_readLog);
    stub.set(ADDR(DLDBusHandler, getOtherFileInfo), stub_getOtherFileInfo);
    // pkexec fails in headless CI env (exit 127); stub checkAuthentication
    // so doWork proceeds to call the stubbed readLog
    stub.set(ADDR(LogOOCFileParseThread, checkAuthentication), stub_ooc_checkAuthentication_true);
    QString path("test");
    m_logThread->setParam(path);
    m_logThread->doWork();
    EXPECT_EQ(m_logThread->getFileData().length(),stub_ooc_readLog("").length());
}

TEST_F(LogOOCFileParseThread_UT, UT_getFileData)
{
    m_logThread->getFileData();
}

TEST_F(LogOOCFileParseThread_UT, UT_stopProcess)
{
    m_logThread->stopProccess();
    EXPECT_EQ(m_logThread->getProcess().isNull(), true);
}

TEST_F(LogOOCFileParseThread_UT, UT_GetIndex_001){
    m_logThread->getIndex();
}

TEST_F(LogOOCFileParseThread_UT, UT_checkAuthentication_001){
    m_logThread->checkAuthentication("test");
}

TEST_F(LogOOCFileParseThread_UT, UT_Run_001)
{
    // run() calls doWork(); empty path → doWork returns early, no DBus calls
    m_logThread->run();
}

TEST_F(LogOOCFileParseThread_UT, UT_StopProccess_DoubleCall_001)
{
    // First call executes stop logic; second call hits the m_isStopProccess guard
    m_logThread->stopProccess();
    m_logThread->stopProccess();
}
