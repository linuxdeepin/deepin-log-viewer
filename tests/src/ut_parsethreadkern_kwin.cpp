// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "parsethreadkern.h"
#include "parsethreadkwin.h"
#include "structdef.h"
#include "dbusproxy/dldbushandler.h"
#include "sharedmemorymanager.h"
#include "utils.h"
#include "../../application/qtcompat.h"

#include <stub.h>

#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QSignalSpy>
#include <QVariant>

#include <gtest/gtest.h>

// ===================== stub functions =====================

static QStringList stub_getFileInfo_empty(const QString &flag, bool unzip)
{
    Q_UNUSED(flag);
    Q_UNUSED(unzip);
    return QStringList();
}

static QStringList stub_getFileInfo_txt(const QString &flag, bool unzip)
{
    Q_UNUSED(flag);
    Q_UNUSED(unzip);
    return QStringList() << "/tmp/ut_test_kern.txt";
}

static QStringList stub_getFileInfo_nonexist(const QString &flag, bool unzip)
{
    Q_UNUSED(flag);
    Q_UNUSED(unzip);
    return QStringList() << "/tmp/ut_nonexistent_kern_log";
}

static QStringList stub_getFileInfo_gz(const QString &flag, bool unzip)
{
    Q_UNUSED(flag);
    Q_UNUSED(unzip);
    static int call = 0;
    if (call == 0) {
        call++;
        return QStringList() << "/tmp/ut_test_kern.gz";
    }
    return QStringList() << "/tmp/ut_test_kern.txt";
}

static qint64 stub_getLineCount_100(const QString &filePath)
{
    Q_UNUSED(filePath);
    return 100;
}

static qint64 stub_getLineCount_0(const QString &filePath)
{
    Q_UNUSED(filePath);
    return 0;
}

static QStringList stub_readLogLinesInRange_kern(const QString &filePath, qint64 startLine, qint64 lineCount, bool bReverse)
{
    Q_UNUSED(filePath);
    Q_UNUSED(startLine);
    Q_UNUSED(lineCount);
    Q_UNUSED(bReverse);
    return QStringList()
           << "Oct 27 12:00:00 uos-PC kernel: test kernel message one"
           << "2026-10-27 13:00:00 uos-PC kernel: test kernel message two"
           << "2026-10-27T14:00:00 uos-PC kernel: test kernel message three"
           << ""
           << "nomatchline";
}

static QStringList stub_readLogLinesInRange_kwin(const QString &filePath, qint64 startLine, qint64 lineCount, bool bReverse)
{
    Q_UNUSED(filePath);
    Q_UNUSED(startLine);
    Q_UNUSED(lineCount);
    Q_UNUSED(bReverse);
    return QStringList() << "kwin log line 1" << "" << "kwin log line 2";
}

// ---- QProcess stubs for pkexec path ----
static void stub_start(const QString &program, const QStringList &arguments, QIODevice::OpenMode mode)
{
    Q_UNUSED(program);
    Q_UNUSED(arguments);
    Q_UNUSED(mode);
}

static bool stub_waitForFinished(int msecs)
{
    Q_UNUSED(msecs);
    return true;
}

static void stub_setProcessChannelMode(QProcess::ProcessChannelMode mode)
{
    Q_UNUSED(mode);
}

static int stub_exitCode_zero()
{
    return 0;
}

static int stub_exitCode_nonzero()
{
    return 1;
}

// ---- SharedMemoryManager stubs ----
static void stub_setRunnableTag(ShareMemoryInfo info)
{
    Q_UNUSED(info);
}

static QString stub_getRunnableKey()
{
    return QStringLiteral("ut_test_key");
}

// ---- QFile::exists stubs (virtual) ----
static bool stub_exists_true()
{
    return true;
}

static bool stub_exists_false()
{
    return false;
}

// ===================== ParseThreadKern tests =====================

class ParseThreadKern_UT : public testing::Test
{
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    void SetUp()
    {
        m_kern = new ParseThreadKern();
        m_kern->setAutoDelete(false);
        m_kern->m_canRun = true;
        m_kern->m_filter.segementIndex = 0;
        m_kern->m_filter.timeFilterBegin = -1;
        m_kern->m_filter.timeFilterEnd = -1;
        m_kern->m_type = KERN;
    }
    void TearDown()
    {
        delete m_kern;
    }
    ParseThreadKern *m_kern = nullptr;
};

TEST_F(ParseThreadKern_UT, Constructor)
{
    ParseThreadKern *k = new ParseThreadKern();
    EXPECT_NE(k, nullptr);
    delete k;
}

TEST_F(ParseThreadKern_UT, Destructor)
{
    ParseThreadKern *k = new ParseThreadKern();
    delete k;
    SUCCEED();
}

TEST_F(ParseThreadKern_UT, Run_EmptyFileList)
{
    Stub stub;
    stub.set(ADDR(DLDBusHandler, getFileInfo), stub_getFileInfo_empty);
    m_kern->run();
    EXPECT_TRUE(m_kern->m_canRun);
}

TEST_F(ParseThreadKern_UT, HandleKern_EmptyFileList)
{
    Stub stub;
    stub.set(ADDR(DLDBusHandler, getFileInfo), stub_getFileInfo_empty);
    m_kern->handleKern();
    SUCCEED();
}

TEST_F(ParseThreadKern_UT, HandleKern_FileNotExist)
{
    Stub stub;
    stub.set(ADDR(DLDBusHandler, getFileInfo), stub_getFileInfo_nonexist);
    typedef bool (QFile::*fptr)() const;
    fptr A_foo = (fptr)(&QFile::exists);
    stub.set(A_foo, stub_exists_false);
    m_kern->handleKern();
    SUCCEED();
}

TEST_F(ParseThreadKern_UT, HandleKern_TxtFile_RunInCmd_True)
{
    Stub stub;
    stub.set(ADDR(DLDBusHandler, getFileInfo), stub_getFileInfo_txt);
    stub.set(ADDR(DLDBusHandler, getLineCount), stub_getLineCount_100);
    stub.set(ADDR(DLDBusHandler, readLogLinesInRange), stub_readLogLinesInRange_kern);
    Utils::runInCmd = true;
    m_kern->handleKern();
    Utils::runInCmd = false;
    SUCCEED();
}

TEST_F(ParseThreadKern_UT, HandleKern_GzFile_RunInCmd_True)
{
    Stub stub;
    stub.set(ADDR(DLDBusHandler, getFileInfo), stub_getFileInfo_gz);
    stub.set(ADDR(DLDBusHandler, getLineCount), stub_getLineCount_100);
    stub.set(ADDR(DLDBusHandler, readLogLinesInRange), stub_readLogLinesInRange_kern);
    typedef bool (QFile::*fptr)() const;
    fptr A_foo = (fptr)(&QFile::exists);
    stub.set(A_foo, stub_exists_true);
    Utils::runInCmd = true;
    m_kern->handleKern();
    Utils::runInCmd = false;
    SUCCEED();
}

TEST_F(ParseThreadKern_UT, HandleKern_PkexecPath_ExitCodeZero)
{
    Stub stub;
    stub.set(ADDR(DLDBusHandler, getFileInfo), stub_getFileInfo_txt);
    stub.set(ADDR(DLDBusHandler, getLineCount), stub_getLineCount_100);
    stub.set(ADDR(DLDBusHandler, readLogLinesInRange), stub_readLogLinesInRange_kern);
    stub.set((void (QProcess::*)(const QString &, const QStringList &, QIODevice::OpenMode))ADDR(QProcess, start), stub_start);
    stub.set(ADDR(QProcess, waitForFinished), stub_waitForFinished);
    stub.set(ADDR(QProcess, setProcessChannelMode), stub_setProcessChannelMode);
    stub.set(ADDR(QProcess, exitCode), stub_exitCode_zero);
    stub.set(ADDR(SharedMemoryManager, setRunnableTag), stub_setRunnableTag);
    stub.set(ADDR(SharedMemoryManager, getRunnableKey), stub_getRunnableKey);
    Utils::runInCmd = false;
    m_kern->handleKern();
    Utils::runInCmd = false;
    SUCCEED();
}

TEST_F(ParseThreadKern_UT, HandleKern_PkexecPath_ExitCodeNonZero)
{
    Stub stub;
    stub.set(ADDR(DLDBusHandler, getFileInfo), stub_getFileInfo_txt);
    stub.set((void (QProcess::*)(const QString &, const QStringList &, QIODevice::OpenMode))ADDR(QProcess, start), stub_start);
    stub.set(ADDR(QProcess, waitForFinished), stub_waitForFinished);
    stub.set(ADDR(QProcess, setProcessChannelMode), stub_setProcessChannelMode);
    stub.set(ADDR(QProcess, exitCode), stub_exitCode_nonzero);
    stub.set(ADDR(SharedMemoryManager, setRunnableTag), stub_setRunnableTag);
    stub.set(ADDR(SharedMemoryManager, getRunnableKey), stub_getRunnableKey);
    Utils::runInCmd = false;
    m_kern->handleKern();
    Utils::runInCmd = false;
    SUCCEED();
}

TEST_F(ParseThreadKern_UT, HandleKern_gStartLine_ExceedLineCount)
{
    Stub stub;
    stub.set(ADDR(DLDBusHandler, getFileInfo), stub_getFileInfo_txt);
    stub.set(ADDR(DLDBusHandler, getLineCount), stub_getLineCount_0);
    stub.set(ADDR(DLDBusHandler, readLogLinesInRange), stub_readLogLinesInRange_kern);
    m_kern->m_filter.segementIndex = 5;
    Utils::runInCmd = true;
    m_kern->handleKern();
    Utils::runInCmd = false;
    SUCCEED();
}

TEST_F(ParseThreadKern_UT, HandleKern_StoppedMidway)
{
    Stub stub;
    stub.set(ADDR(DLDBusHandler, getFileInfo), stub_getFileInfo_txt);
    stub.set(ADDR(DLDBusHandler, getLineCount), stub_getLineCount_100);
    stub.set(ADDR(DLDBusHandler, readLogLinesInRange), stub_readLogLinesInRange_kern);
    m_kern->m_canRun = false;
    Utils::runInCmd = true;
    m_kern->handleKern();
    Utils::runInCmd = false;
    SUCCEED();
}

TEST_F(ParseThreadKern_UT, FormatDateTime_ThreeArgs)
{
    qint64 result = m_kern->formatDateTime(QStringLiteral("Oct"), QStringLiteral("27"), QStringLiteral("12:00:00"));
    EXPECT_NE(result, 0);
}

TEST_F(ParseThreadKern_UT, FormatDateTime_TwoArgs)
{
    qint64 result = m_kern->formatDateTime(QStringLiteral("2026-10-27"), QStringLiteral("12:00:00"));
    EXPECT_NE(result, 0);
}

TEST_F(ParseThreadKern_UT, FormatDateTime_ThreeArgs_Invalid)
{
    qint64 result = m_kern->formatDateTime(QStringLiteral("InvalidMonth"), QStringLiteral("99"), QStringLiteral("99:99:99"));
    EXPECT_EQ(result, 0);
}

TEST_F(ParseThreadKern_UT, FormatDateTime_TwoArgs_Invalid)
{
    qint64 result = m_kern->formatDateTime(QStringLiteral("not-a-date"), QStringLiteral("not-a-time"));
    EXPECT_EQ(result, 0);
}

// ===================== ParseThreadKwin tests =====================

class ParseThreadKwin_UT : public testing::Test
{
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    void SetUp()
    {
        m_kwin = new ParseThreadKwin();
        m_kwin->setAutoDelete(false);
        m_kwin->m_canRun = true;
        m_kwin->m_filter.segementIndex = 0;
        m_kwin->m_filter.timeFilterBegin = -1;
        m_kwin->m_filter.timeFilterEnd = -1;
        m_kwin->m_type = Kwin;
    }
    void TearDown()
    {
        delete m_kwin;
    }
    ParseThreadKwin *m_kwin = nullptr;
};

TEST_F(ParseThreadKwin_UT, Constructor)
{
    ParseThreadKwin *k = new ParseThreadKwin();
    EXPECT_NE(k, nullptr);
    delete k;
}

TEST_F(ParseThreadKwin_UT, Destructor)
{
    ParseThreadKwin *k = new ParseThreadKwin();
    delete k;
    SUCCEED();
}

TEST_F(ParseThreadKwin_UT, Run_FileNotExist)
{
    Stub stub;
    typedef bool (QFile::*fptr)() const;
    fptr A_foo = (fptr)(&QFile::exists);
    stub.set(A_foo, stub_exists_false);
    m_kwin->run();
    EXPECT_TRUE(m_kwin->m_canRun);
}

TEST_F(ParseThreadKwin_UT, HandleKwin_FileNotExist)
{
    Stub stub;
    typedef bool (QFile::*fptr)() const;
    fptr A_foo = (fptr)(&QFile::exists);
    stub.set(A_foo, stub_exists_false);
    m_kwin->handleKwin();
    SUCCEED();
}

TEST_F(ParseThreadKwin_UT, HandleKwin_FileExist_NormalParse)
{
    Stub stub;
    typedef bool (QFile::*fptr)() const;
    fptr A_foo = (fptr)(&QFile::exists);
    stub.set(A_foo, stub_exists_true);
    stub.set(ADDR(DLDBusHandler, getLineCount), stub_getLineCount_100);
    stub.set(ADDR(DLDBusHandler, readLogLinesInRange), stub_readLogLinesInRange_kwin);
    m_kwin->handleKwin();
    SUCCEED();
}

TEST_F(ParseThreadKwin_UT, HandleKwin_gStartLine_ExceedLineCount)
{
    Stub stub;
    typedef bool (QFile::*fptr)() const;
    fptr A_foo = (fptr)(&QFile::exists);
    stub.set(A_foo, stub_exists_true);
    stub.set(ADDR(DLDBusHandler, getLineCount), stub_getLineCount_0);
    stub.set(ADDR(DLDBusHandler, readLogLinesInRange), stub_readLogLinesInRange_kwin);
    m_kwin->m_filter.segementIndex = 5;
    m_kwin->handleKwin();
    SUCCEED();
}

TEST_F(ParseThreadKwin_UT, HandleKwin_StoppedMidway)
{
    Stub stub;
    typedef bool (QFile::*fptr)() const;
    fptr A_foo = (fptr)(&QFile::exists);
    stub.set(A_foo, stub_exists_true);
    stub.set(ADDR(DLDBusHandler, getLineCount), stub_getLineCount_100);
    stub.set(ADDR(DLDBusHandler, readLogLinesInRange), stub_readLogLinesInRange_kwin);
    m_kwin->m_canRun = false;
    m_kwin->handleKwin();
    SUCCEED();
}
