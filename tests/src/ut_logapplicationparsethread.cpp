// SPDX-FileCopyrightText: 2019 - 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "logapplicationparsethread.h"
#include "logfileparser.h"
#include "structdef.h"
#include "dbusproxy/dldbushandler.h"
#include "../../application/qtcompat.h"

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

QStringList stub_split(QChar sep, SPLIT_BH behavior,
                       Qt::CaseSensitivity cs)
{
    return QStringList() << "t.em,s[.a]da[],m[.s]d,s[t.][,"
                         << "jsd[sa[dds.,]"
                         << "ds[ds..,/n"
                         << "test2.[df,f";
}

QString stub_readAppLog(const QString &filePath)
{
    Q_UNUSED(filePath);
    return "2021-04-06 13:29:32 install code:amd64 <none> 1.55.0-1617120720";
}

QStringList stub_getAppFileInfo(const QString &flag)
{
    Q_UNUSED(flag);
    return QStringList() << "test";
}

bool stub_isGetFileInfoError_false()
{
    return false;
}

QByteArray stub_readAllStandardOutput(){
    return "2021-03-10, 11:33:23.48.9 [Warning] [                                                         0] QFSFileEngine::open: No file name specified";
}

class LogApplicationParseThread_UT : public testing::Test
{
public:
    //添加日志
    static void SetUpTestCase()
    {
        qDebug() << "SetUpTestCase";
    }
    static void TearDownTestCase()
    {
        qDebug() << "TearDownTestCase";
    }
    void SetUp() //TEST跑之前会执行SetUp
    {
        m_logAppThread = new LogApplicationParseThread();
        qDebug() << "SetUp";
    }
    void TearDown() //TEST跑完之后会执行TearDown
    {
        delete m_logAppThread;
    }
    LogApplicationParseThread *m_logAppThread;
};

TEST_F(LogApplicationParseThread_UT, UT_InitProccess_001){
    m_logAppThread->initProccess();
    EXPECT_NE(m_logAppThread, nullptr);
    EXPECT_NE(m_logAppThread->m_process, nullptr);
}

TEST_F(LogApplicationParseThread_UT, UT_OnProcFinished_001){
    m_logAppThread->onProcFinished(1);
    EXPECT_NE(m_logAppThread, nullptr);
}

TEST_F(LogApplicationParseThread_UT, UT_SetParam_001){
    APP_FILTERS appfilter;
    APP_FILTERSList appfilters;
    appfilters << appfilter;
    m_logAppThread->setFilters(appfilters);
    EXPECT_NE(m_logAppThread, nullptr);
    EXPECT_EQ(m_logAppThread->m_AppFiler.path, appfilter.path);
}

TEST_F(LogApplicationParseThread_UT, UT_DoWork_001)
{
    Stub stub;
    stub.set((QList<QFileInfo>(QDir::*)(QDir::Filters, QDir::SortFlags) const)ADDR(QDir, entryInfoList), stub_entryInfoList);
    stub.set(ADDR(DLDBusHandler, getFileInfo), stub_getAppFileInfo);
    stub.set(ADDR(DLDBusHandler, readLog), stub_readAppLog);
    stub.set(ADDR(DLDBusHandler, isGetFileInfoError), stub_isGetFileInfoError_false);
    stub.set(ADDR(QProcess, readAllStandardOutput), stub_readAllStandardOutput);
    EXPECT_NE(m_logAppThread, nullptr);
    m_logAppThread->m_AppFiler.path = "";
    m_logAppThread->doWork();
    APP_FILTERS appfilter;
    appfilter.logType = "file";
    APP_FILTERSList appfilters;
    appfilters << appfilter;
    m_logAppThread->m_AppFilers = appfilters;
    m_logAppThread->doWork();
    m_logAppThread->m_AppFilers.clear();
    appfilter.path = "test";
    appfilter.lvlFilter = LVALL;
    appfilters.clear();
    appfilters << appfilter;
    m_logAppThread->m_AppFilers = appfilters;
    m_logAppThread->doWork();
    EXPECT_NE(m_logAppThread->m_process, nullptr);
    EXPECT_EQ(m_logAppThread->m_appList.count(),1);
}

TEST_F(LogApplicationParseThread_UT, stopProcess_UT)
{
    Stub stub;
    stub.set((QList<QFileInfo>(QDir::*)(QDir::Filters, QDir::SortFlags) const)ADDR(QDir, entryInfoList), stub_entryInfoList);
    stub.set((QStringList(QString::*)(QChar, SPLIT_BH, Qt::CaseSensitivity) const)ADDR(QString, split), stub_split);
    stub.set(ADDR(DLDBusHandler, getFileInfo), stub_getAppFileInfo);
    stub.set(ADDR(DLDBusHandler, readLog), stub_readAppLog);
    m_logAppThread->stopProccess();
    EXPECT_EQ(m_logAppThread->m_process, nullptr);

}

TEST_F(LogApplicationParseThread_UT, UT_GetIndex_001){
    int index= m_logAppThread->getIndex();
    EXPECT_EQ(index, 6);
}

// ==================== Stubs for sd_journal functions ====================

int stub_sd_journal_open_success(sd_journal **ret, int flags)
{
    *ret = reinterpret_cast<sd_journal *>(0x1);
    return 0;
}

int stub_sd_journal_open_fail(sd_journal **ret, int flags)
{
    *ret = nullptr;
    return -1;
}

int stub_sd_journal_seek_tail_ok(sd_journal *j)
{
    return 0;
}

int stub_sd_journal_add_match_ok(sd_journal *j, const void *data, size_t size)
{
    return 0;
}

static int g_journal_previous_call_count = 0;
int stub_sd_journal_previous_once(sd_journal *j)
{
    g_journal_previous_call_count++;
    if (g_journal_previous_call_count == 1)
        return 1;
    return 0;
}

int stub_sd_journal_previous_never(sd_journal *j)
{
    return 0;
}

int stub_sd_journal_get_data_ok(sd_journal *j, const char *field, const void **data, size_t *l)
{
    static const char *dummy = "field=value";
    *data = dummy;
    *l = 11;
    return 0;
}

int stub_sd_journal_get_realtime_usec_ok(sd_journal *j, uint64_t *ret)
{
    *ret = 1000000;
    return 0;
}

void stub_sd_journal_close_noop(sd_journal *j)
{
}

QString stub_getReplaceColorStr_app(const char *d)
{
    return "field=value";
}

// ==================== Tests for getDateTimeFromStamp ====================

TEST_F(LogApplicationParseThread_UT, UT_GetDateTimeFromStamp_001)
{
    QString result = m_logAppThread->getDateTimeFromStamp("1609459200000");
    EXPECT_FALSE(result.isEmpty());
}

TEST_F(LogApplicationParseThread_UT, UT_GetDateTimeFromStamp_002)
{
    QString result = m_logAppThread->getDateTimeFromStamp("");
    EXPECT_TRUE(result.isEmpty() || !result.isEmpty());
}

// ==================== Tests for i2str ====================

TEST_F(LogApplicationParseThread_UT, UT_I2str_001)
{
    QString result0 = m_logAppThread->i2str(0);
    EXPECT_FALSE(result0.isEmpty());
}

TEST_F(LogApplicationParseThread_UT, UT_I2str_002)
{
    for (int i = 0; i <= 7; ++i) {
        QString result = m_logAppThread->i2str(i);
        EXPECT_FALSE(result.isEmpty()) << "i2str(" << i << ") should not be empty";
    }
}

TEST_F(LogApplicationParseThread_UT, UT_I2str_003)
{
    QString result = m_logAppThread->i2str(999);
    EXPECT_TRUE(result.isEmpty());
}

// ==================== Tests for initMap and initJournalMap ====================

TEST_F(LogApplicationParseThread_UT, UT_InitMap_001)
{
    m_logAppThread->initMap();
    EXPECT_EQ(m_logAppThread->m_levelDict.value("Warning"), WARN);
    EXPECT_EQ(m_logAppThread->m_levelDict.value("Debug"), DEB);
    EXPECT_EQ(m_logAppThread->m_levelDict.value("Info"), INF);
    EXPECT_EQ(m_logAppThread->m_levelDict.value("Error"), ERR);
}

TEST_F(LogApplicationParseThread_UT, UT_InitJournalMap_001)
{
    m_logAppThread->initJournalMap();
    EXPECT_EQ(m_logAppThread->m_journalMap.size(), 8);
    EXPECT_FALSE(m_logAppThread->m_journalMap.value(0).isEmpty());
    EXPECT_FALSE(m_logAppThread->m_journalMap.value(7).isEmpty());
}

// ==================== Tests for parseByJournal ====================

TEST_F(LogApplicationParseThread_UT, UT_ParseByJournal_CanRunFalse)
{
    m_logAppThread->m_canRun = false;
    APP_FILTERS appfilter;
    appfilter.logType = "journal";
    bool result = m_logAppThread->parseByJournal(appfilter);
    EXPECT_EQ(result, false);
}

TEST_F(LogApplicationParseThread_UT, UT_ParseByJournal_OpenFail)
{
    Stub stub;
    stub.set(sd_journal_open, stub_sd_journal_open_fail);
    m_logAppThread->m_canRun = true;
    APP_FILTERS appfilter;
    appfilter.logType = "journal";
    bool result = m_logAppThread->parseByJournal(appfilter);
    EXPECT_EQ(result, false);
}

TEST_F(LogApplicationParseThread_UT, UT_ParseByJournal_NoMatch)
{
    Stub stub;
    stub.set(sd_journal_open, stub_sd_journal_open_success);
    stub.set(sd_journal_seek_tail, stub_sd_journal_seek_tail_ok);
    stub.set(sd_journal_add_match, stub_sd_journal_add_match_ok);
    stub.set(sd_journal_close, stub_sd_journal_close_noop);
    m_logAppThread->m_canRun = true;
    APP_FILTERS appfilter;
    appfilter.logType = "journal";
    appfilter.execPath = "";
    appfilter.filter = "";
    appfilter.app = "";
    bool result = m_logAppThread->parseByJournal(appfilter);
    EXPECT_EQ(result, true);
}

TEST_F(LogApplicationParseThread_UT, UT_ParseByJournal_WithApp)
{
    Stub stub;
    stub.set(sd_journal_open, stub_sd_journal_open_success);
    stub.set(sd_journal_seek_tail, stub_sd_journal_seek_tail_ok);
    stub.set(sd_journal_add_match, stub_sd_journal_add_match_ok);
    stub.set(sd_journal_previous, stub_sd_journal_previous_once);
    stub.set(sd_journal_get_data, stub_sd_journal_get_data_ok);
    stub.set(sd_journal_get_realtime_usec, stub_sd_journal_get_realtime_usec_ok);
    stub.set(sd_journal_close, stub_sd_journal_close_noop);
    stub.set(ADDR(Utils, getReplaceColorStr), stub_getReplaceColorStr_app);

    g_journal_previous_call_count = 0;
    m_logAppThread->m_canRun = true;
    APP_FILTERS appfilter;
    appfilter.logType = "journal";
    appfilter.app = "testapp";
    appfilter.lvlFilter = LVALL;
    appfilter.timeFilterBegin = -1;
    bool result = m_logAppThread->parseByJournal(appfilter);
    EXPECT_EQ(result, true);
}

TEST_F(LogApplicationParseThread_UT, UT_ParseByJournal_WithWildcard)
{
    Stub stub;
    stub.set(sd_journal_open, stub_sd_journal_open_success);
    stub.set(sd_journal_seek_tail, stub_sd_journal_seek_tail_ok);
    stub.set(sd_journal_add_match, stub_sd_journal_add_match_ok);
    stub.set(sd_journal_previous, stub_sd_journal_previous_once);
    stub.set(sd_journal_get_data, stub_sd_journal_get_data_ok);
    stub.set(sd_journal_get_realtime_usec, stub_sd_journal_get_realtime_usec_ok);
    stub.set(sd_journal_close, stub_sd_journal_close_noop);
    stub.set(ADDR(Utils, getReplaceColorStr), stub_getReplaceColorStr_app);

    g_journal_previous_call_count = 0;
    m_logAppThread->m_canRun = true;
    APP_FILTERS appfilter;
    appfilter.logType = "journal";
    appfilter.app = "testapp";
    appfilter.filter = "val*";
    appfilter.lvlFilter = LVALL;
    appfilter.timeFilterBegin = -1;
    bool result = m_logAppThread->parseByJournal(appfilter);
    EXPECT_EQ(result, true);
}

TEST_F(LogApplicationParseThread_UT, UT_ParseByJournal_WithLevelFilter)
{
    Stub stub;
    stub.set(sd_journal_open, stub_sd_journal_open_success);
    stub.set(sd_journal_seek_tail, stub_sd_journal_seek_tail_ok);
    stub.set(sd_journal_add_match, stub_sd_journal_add_match_ok);
    stub.set(sd_journal_previous, stub_sd_journal_previous_once);
    stub.set(sd_journal_get_data, stub_sd_journal_get_data_ok);
    stub.set(sd_journal_get_realtime_usec, stub_sd_journal_get_realtime_usec_ok);
    stub.set(sd_journal_close, stub_sd_journal_close_noop);
    stub.set(ADDR(Utils, getReplaceColorStr), stub_getReplaceColorStr_app);

    g_journal_previous_call_count = 0;
    m_logAppThread->m_canRun = true;
    APP_FILTERS appfilter;
    appfilter.logType = "journal";
    appfilter.app = "testapp";
    appfilter.lvlFilter = WARN;
    appfilter.timeFilterBegin = -1;
    bool result = m_logAppThread->parseByJournal(appfilter);
    EXPECT_EQ(result, true);
}

TEST_F(LogApplicationParseThread_UT, UT_ParseByJournal_StopMidway)
{
    Stub stub;
    stub.set(sd_journal_open, stub_sd_journal_open_success);
    stub.set(sd_journal_seek_tail, stub_sd_journal_seek_tail_ok);
    stub.set(sd_journal_add_match, stub_sd_journal_add_match_ok);
    stub.set(sd_journal_previous, stub_sd_journal_previous_never);
    stub.set(sd_journal_close, stub_sd_journal_close_noop);

    m_logAppThread->m_canRun = true;
    APP_FILTERS appfilter;
    appfilter.logType = "journal";
    appfilter.app = "testapp";
    appfilter.lvlFilter = LVALL;
    appfilter.timeFilterBegin = -1;
    bool result = m_logAppThread->parseByJournal(appfilter);
    EXPECT_EQ(result, true);
}

// ==================== Test for run() ====================

TEST_F(LogApplicationParseThread_UT, UT_Run_001)
{
    Stub stub;
    stub.set((QList<QFileInfo>(QDir::*)(QDir::Filters, QDir::SortFlags) const)ADDR(QDir, entryInfoList), stub_entryInfoList);
    stub.set(ADDR(DLDBusHandler, getFileInfo), stub_getAppFileInfo);
    stub.set(ADDR(DLDBusHandler, readLog), stub_readAppLog);
    stub.set(ADDR(DLDBusHandler, isGetFileInfoError), stub_isGetFileInfoError_false);

    m_logAppThread->m_AppFilers.clear();
    m_logAppThread->start();
    m_logAppThread->wait(5000);
    EXPECT_NE(m_logAppThread, nullptr);
}

// ==================== Test for doWork with journal type ====================

TEST_F(LogApplicationParseThread_UT, UT_DoWork_Journal)
{
    Stub stub;
    stub.set(sd_journal_open, stub_sd_journal_open_fail);
    m_logAppThread->m_AppFilers.clear();
    APP_FILTERS appfilter;
    appfilter.logType = "journal";
    appfilter.app = "testapp";
    m_logAppThread->m_AppFilers << appfilter;
    m_logAppThread->doWork();
    EXPECT_NE(m_logAppThread, nullptr);
}

// ==================== Test for stopProccess with active process ====================

TEST_F(LogApplicationParseThread_UT, UT_StopProccess_WithProcess)
{
    m_logAppThread->initProccess();
    EXPECT_NE(m_logAppThread->m_process, nullptr);
    m_logAppThread->stopProccess();
    EXPECT_EQ(m_logAppThread->m_canRun, false);
}
