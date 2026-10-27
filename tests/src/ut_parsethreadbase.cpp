// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "parsethreadbase.h"
#include "logfileparser.h"
#include "structdef.h"
#include "utils.h"
#include "sharedmemorymanager.h"

#include <stub.h>

#include <QDebug>
#include <QProcess>

#include <gtest/gtest.h>

void stub_setRunnableTag001(ShareMemoryInfo iShareInfo)
{
    Q_UNUSED(iShareInfo);
}

class TestParseThread : public ParseThreadBase
{
public:
    explicit TestParseThread(QObject *parent = nullptr)
        : ParseThreadBase(parent)
    {
    }
    void run() override
    {
        m_canRun = false;
    }
};

class ParseThreadBase_UT : public testing::Test
{
public:
    static void SetUpTestCase()
    {
        qDebug() << "SetUpTestCase";
    }
    static void TearDownTestCase()
    {
        qDebug() << "TearDownTestCase";
    }
    void SetUp()
    {
        Utils::runInCmd = true;
        m_thread = new TestParseThread();
    }
    void TearDown()
    {
        delete m_thread;
        Utils::runInCmd = false;
    }
    TestParseThread *m_thread;
};

TEST_F(ParseThreadBase_UT, Constructor)
{
    EXPECT_NE(m_thread, nullptr);
    EXPECT_TRUE(m_thread->m_threadCount > 0);
    EXPECT_EQ(m_thread->m_type, NONE);
}

TEST_F(ParseThreadBase_UT, Constructor_WithLogFileParserParent)
{
    LogFileParser *parser = new LogFileParser();
    TestParseThread *thread = new TestParseThread(parser);
    EXPECT_NE(thread, nullptr);
    EXPECT_NE(thread->m_pParser, nullptr);
    delete thread;
    delete parser;
}

TEST_F(ParseThreadBase_UT, Constructor_WithoutLogFileParserParent)
{
    QObject *obj = new QObject();
    TestParseThread *thread = new TestParseThread(obj);
    EXPECT_NE(thread, nullptr);
    EXPECT_EQ(thread->m_pParser, nullptr);
    delete thread;
    delete obj;
}

TEST_F(ParseThreadBase_UT, Destructor)
{
    TestParseThread *thread = new TestParseThread();
    thread->initProccess();
    EXPECT_NE(thread->m_process, nullptr);
    delete thread;
    SUCCEED();
}

TEST_F(ParseThreadBase_UT, InitProccess_New)
{
    EXPECT_EQ(m_thread->m_process, nullptr);
    m_thread->initProccess();
    EXPECT_NE(m_thread->m_process, nullptr);
}

TEST_F(ParseThreadBase_UT, InitProccess_Existing)
{
    m_thread->initProccess();
    QProcess *firstProcess = m_thread->m_process.data();
    m_thread->initProccess();
    EXPECT_EQ(m_thread->m_process.data(), firstProcess);
}

TEST_F(ParseThreadBase_UT, StopProccess_NoProcess)
{
    EXPECT_EQ(m_thread->m_process, nullptr);
    m_thread->stopProccess();
    EXPECT_TRUE(m_thread->m_isStopProccess);
    EXPECT_FALSE(m_thread->m_canRun);
}

TEST_F(ParseThreadBase_UT, StopProccess_WithProcess)
{
    m_thread->initProccess();
    EXPECT_NE(m_thread->m_process, nullptr);
    m_thread->stopProccess();
    EXPECT_TRUE(m_thread->m_isStopProccess);
}

TEST_F(ParseThreadBase_UT, StopProccess_AlreadyStopping)
{
    m_thread->m_isStopProccess = true;
    m_thread->m_canRun = true;
    m_thread->stopProccess();
    EXPECT_TRUE(m_thread->m_canRun);
}

TEST_F(ParseThreadBase_UT, StopProccess_WithSharedMemory)
{
    Utils::runInCmd = false;
    Stub stub;
    stub.set(ADDR(SharedMemoryManager, setRunnableTag), stub_setRunnableTag001);
    m_thread->stopProccess();
    EXPECT_TRUE(m_thread->m_isStopProccess);
    Utils::runInCmd = true;
}

TEST_F(ParseThreadBase_UT, SetFilter)
{
    LOG_FILTER_BASE filter;
    filter.type = KERN;
    filter.filePath = "/var/log/kern.log";
    m_thread->setFilter(filter);
    EXPECT_EQ(m_thread->m_type, KERN);
    EXPECT_EQ(m_thread->m_filter.filePath, QString("/var/log/kern.log"));
}

TEST_F(ParseThreadBase_UT, GetIndex)
{
    int index = m_thread->getIndex();
    EXPECT_EQ(index, m_thread->m_threadCount);
}

TEST_F(ParseThreadBase_UT, Run)
{
    m_thread->m_canRun = true;
    m_thread->run();
    EXPECT_FALSE(m_thread->m_canRun);
}

TEST_F(ParseThreadBase_UT, InitConnect_WithParser)
{
    LogFileParser *parser = new LogFileParser();
    TestParseThread *thread = new TestParseThread(parser);
    EXPECT_NE(thread->m_pParser, nullptr);
    delete thread;
    delete parser;
}

TEST_F(ParseThreadBase_UT, InitConnect_WithoutParser)
{
    TestParseThread *thread = new TestParseThread(nullptr);
    EXPECT_EQ(thread->m_pParser, nullptr);
    delete thread;
}

TEST_F(ParseThreadBase_UT, Run_BaseClass)
{
    m_thread->m_canRun = true;
    m_thread->ParseThreadBase::run();
    EXPECT_FALSE(m_thread->m_canRun);
}
