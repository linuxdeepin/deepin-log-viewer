// SPDX-FileCopyrightText: 2019 - 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "../application/utils.h"
#include "ut_logViewerPlugin.h"
#include "../application/qtcompat.h"
#include "logapplicationhelper.h"
#include "../application/logexportthread.h"

#include <stub.h>
#include <QSignalSpy>
#include <QTemporaryDir>
#include <QStandardPaths>

LogViewerPlugin_UT::LogViewerPlugin_UT()
{

}

void LogViewerPlugin_UT::SetUp()
{
    LogApplicationHelper::instance();
    m_instance = new LogViewerPlugin;
    qDebug() << "SetUp" << ENDL;
}

void LogViewerPlugin_UT::TearDown()
{
    delete m_instance;
}

void QThreadPool_start(QRunnable *runnable, int priority)
{
    Q_UNUSED(runnable)
    Q_UNUSED(priority)
    qDebug() << "QThreadPool_start--";
}


void QThread_start(QThread::Priority)
{
    qDebug() << "QThread_start--";
}

// Stub for LogExportThread export methods — no-op to avoid real file I/O
void stub_exportToTxt_void() {}
void stub_exportToHtml_void() {}
void stub_exportToDoc_void() {}
void stub_exportToXls_void() {}

// ============================================================================
// Existing tests (kept — reasonable, exercise generateAppFile & exportAppLogFile)
// ============================================================================

TEST_F(LogViewerPlugin_UT, generateAppFile_UT001)
{
    Utils::sleep(200);
    Stub stub;
    stub.set((void (QThreadPool::*)(QRunnable *, int))ADDR(QThreadPool, start), QThreadPool_start);
    stub.set(ADDR(QThread, start), QThread_start);

    LogViewerPlugin *p = new LogViewerPlugin;
    p->generateAppFile(Utils::homePath + "/.cache/deepin/deepin-log-viewer/deepin-log-viewer.log", ALL, INF, "");
    connect(p, &LogViewerPlugin::sigAppData, this, [=](int index, QList<LOG_MSG_APPLICATOIN> iList) {
        qDebug() << "index:" << index << ENDL;
        qDebug() << "appDatas:" << ENDL;
        int listCount = iList.size();
        for (int i = 0; i < listCount; i++) {
            qDebug() << "datetime=" << iList[i].dateTime
                     << " level=" << iList[i].level
                     << " src=" << iList[i].src
                     << " msg=" << iList[i].msg
                     << " detailInfo=" << iList[i].detailInfo << ENDL;
        }
    });
    connect(p, &LogViewerPlugin::sigAppFinished, p, &LogViewerPlugin::deleteLater);
}

TEST_F(LogViewerPlugin_UT, exportLogFile_UT001)
{
    Utils::sleep(200);
    Stub stub;
    stub.set((void (QThreadPool::*)(QRunnable *, int))ADDR(QThreadPool, start), QThreadPool_start);
    stub.set(ADDR(QThread, start), QThread_start);

    LogViewerPlugin *p = new LogViewerPlugin;
    p->exportAppLogFile("/home/Desktop/test-plugin.txt", ALL, INF, "deepin-log-viewer");
    connect(p, &LogViewerPlugin::sigExportResult, p, &LogViewerPlugin::deleteLater);
}

// ============================================================================
// Parameterized generateAppFile — covers all 6 BUTTONID values
// ============================================================================

class LogViewerPlugin_generateAppFile_UT_Param
{
public:
    explicit LogViewerPlugin_generateAppFile_UT_Param(BUTTONID id)
    {
        time = id;
    }
    BUTTONID time;
};

class LogViewerPlugin_generateAppFile_UT : public ::testing::TestWithParam<LogViewerPlugin_generateAppFile_UT_Param>
{
    virtual void nothing();//消除编译警告
};

void LogViewerPlugin_generateAppFile_UT::nothing(){}

INSTANTIATE_TEST_CASE_P(LogViewerPlugin, LogViewerPlugin_generateAppFile_UT,
                        ::testing::Values(LogViewerPlugin_generateAppFile_UT_Param(ALL), LogViewerPlugin_generateAppFile_UT_Param(ONE_DAY),
                        LogViewerPlugin_generateAppFile_UT_Param(THREE_DAYS), LogViewerPlugin_generateAppFile_UT_Param(ONE_WEEK),
                        LogViewerPlugin_generateAppFile_UT_Param(ONE_MONTH), LogViewerPlugin_generateAppFile_UT_Param(THREE_MONTHS)));

TEST_P(LogViewerPlugin_generateAppFile_UT, LogViewerPlugin_generateAppFile_UT_001)
{
    LogApplicationHelper::instance();
    Stub stub;
    stub.set((void (QThreadPool::*)(QRunnable *, int))ADDR(QThreadPool, start), QThreadPool_start);
    stub.set(ADDR(QThread, start), QThread_start);

    LogViewerPlugin_generateAppFile_UT_Param param = GetParam();
    LogViewerPlugin *p = new LogViewerPlugin();
    EXPECT_NE(p, nullptr);
    Utils::sleep(200);
    p->generateAppFile("", param.time, INF, "");
    p->deleteLater();
}

// ============================================================================
// New tests — covering all remaining methods for 100% function coverage
// ============================================================================

// --- clearAllFilter ---
TEST_F(LogViewerPlugin_UT, clearAllFilter_UT001)
{
    m_instance->clearAllFilter();
    EXPECT_TRUE(m_instance->m_currentSearchStr.isEmpty());
    EXPECT_EQ(m_instance->m_normalFilter.searchstr, "");
}

// --- clearAllDatalist ---
TEST_F(LogViewerPlugin_UT, clearAllDatalist_UT001)
{
    // Populate lists with some data first
    LOG_MSG_JOURNAL jMsg;
    m_instance->jList.append(jMsg);
    m_instance->jListOrigin.append(jMsg);
    m_instance->appList.append(LOG_MSG_APPLICATOIN());
    m_instance->appListOrigin.append(LOG_MSG_APPLICATOIN());

    m_instance->clearAllDatalist();

    EXPECT_TRUE(m_instance->jList.isEmpty());
    EXPECT_TRUE(m_instance->jListOrigin.isEmpty());
    EXPECT_TRUE(m_instance->dList.isEmpty());
    EXPECT_TRUE(m_instance->dListOrigin.isEmpty());
    EXPECT_TRUE(m_instance->xList.isEmpty());
    EXPECT_TRUE(m_instance->xListOrigin.isEmpty());
    EXPECT_TRUE(m_instance->bList.isEmpty());
    EXPECT_TRUE(m_instance->currentBootList.isEmpty());
    EXPECT_TRUE(m_instance->kList.isEmpty());
    EXPECT_TRUE(m_instance->kListOrigin.isEmpty());
    EXPECT_TRUE(m_instance->appList.isEmpty());
    EXPECT_TRUE(m_instance->appListOrigin.isEmpty());
    EXPECT_TRUE(m_instance->norList.isEmpty());
    EXPECT_TRUE(m_instance->nortempList.isEmpty());
    EXPECT_TRUE(m_instance->m_currentKwinList.isEmpty());
    EXPECT_TRUE(m_instance->m_kwinList.isEmpty());
    EXPECT_TRUE(m_instance->jBootList.isEmpty());
    EXPECT_TRUE(m_instance->jBootListOrigin.isEmpty());
    EXPECT_TRUE(m_instance->dnfList.isEmpty());
    EXPECT_TRUE(m_instance->dnfListOrigin.isEmpty());
}

// --- filterApp ---
TEST_F(LogViewerPlugin_UT, filterApp_UT001)
{
    // Empty search string returns input unchanged
    QList<LOG_MSG_APPLICATOIN> input;
    LOG_MSG_APPLICATOIN msg1;
    msg1.dateTime = "2024-01-01";
    msg1.level = "INFO";
    msg1.src = "app1";
    msg1.msg = "hello world";
    input.append(msg1);

    QList<LOG_MSG_APPLICATOIN> result = m_instance->filterApp("", input);
    EXPECT_EQ(result.size(), 1);

    // Matching search string
    result = m_instance->filterApp("hello", input);
    EXPECT_EQ(result.size(), 1);

    // Matching in dateTime field
    result = m_instance->filterApp("2024", input);
    EXPECT_EQ(result.size(), 1);

    // Matching in level field (case insensitive)
    result = m_instance->filterApp("info", input);
    EXPECT_EQ(result.size(), 1);

    // Matching in src field
    result = m_instance->filterApp("app1", input);
    EXPECT_EQ(result.size(), 1);

    // Non-matching search string
    result = m_instance->filterApp("nonexistent", input);
    EXPECT_EQ(result.size(), 0);

    // Empty input list
    QList<LOG_MSG_APPLICATOIN> emptyList;
    result = m_instance->filterApp("test", emptyList);
    EXPECT_EQ(result.size(), 0);
}

// --- getAppName ---
TEST_F(LogViewerPlugin_UT, getAppName_UT001)
{
    // Empty path
    QString result = m_instance->getAppName("");
    EXPECT_TRUE(result.isEmpty());

    // Path with slash and dot — normal case
    result = m_instance->getAppName("/home/user/.cache/deepin/deepin-log-viewer.log");
    // transName may return empty if not in map, but the parsing logic is exercised

    // Path without slash, with dot
    result = m_instance->getAppName("testfile.log");

    // Path without slash, without dot
    result = m_instance->getAppName("testfile");

    // Path with slash, no dot in filename
    result = m_instance->getAppName("/home/user/nodotfile");
}

// --- slot_appFinished ---
TEST_F(LogViewerPlugin_UT, slot_appFinished_UT001)
{
    // Guard: m_flag != APP — should not emit signal
    m_instance->m_flag = JOURNAL;
    m_instance->m_appCurrentIndex = 0;
    QSignalSpy spy(m_instance, &LogViewerPlugin::sigAppFinished);
    m_instance->slot_appFinished(0);
    EXPECT_EQ(spy.count(), 0);

    // Guard: index mismatch — should not emit signal
    m_instance->m_flag = APP;
    m_instance->m_appCurrentIndex = 5;
    m_instance->slot_appFinished(0);
    EXPECT_EQ(spy.count(), 0);

    // Valid case — should emit signal
    m_instance->m_flag = APP;
    m_instance->m_appCurrentIndex = 3;
    m_instance->slot_appFinished(3);
    EXPECT_EQ(spy.count(), 1);
    EXPECT_EQ(spy.takeFirst().at(0).toInt(), 3);
}

// --- slot_appData ---
TEST_F(LogViewerPlugin_UT, slot_appData_UT001)
{
    QList<LOG_MSG_APPLICATOIN> dataList;
    LOG_MSG_APPLICATOIN msg;
    msg.dateTime = "2024-01-01";
    msg.level = "INFO";
    msg.src = "src";
    msg.msg = "message";
    dataList.append(msg);

    // Guard: m_flag != APP — should not emit signal
    m_instance->m_flag = JOURNAL;
    m_instance->m_appCurrentIndex = 0;
    QSignalSpy spy(m_instance, &LogViewerPlugin::sigAppData);
    m_instance->slot_appData(0, dataList);
    EXPECT_EQ(spy.count(), 0);

    // Guard: index mismatch
    m_instance->m_flag = APP;
    m_instance->m_appCurrentIndex = 5;
    m_instance->slot_appData(0, dataList);
    EXPECT_EQ(spy.count(), 0);

    // Valid case — should emit signal with filtered data
    m_instance->m_flag = APP;
    m_instance->m_appCurrentIndex = 1;
    m_instance->m_currentSearchStr = "";
    m_instance->m_firstLoadPageData = true;
    m_instance->slot_appData(1, dataList);
    EXPECT_EQ(spy.count(), 1);
    QList<QVariant> args = spy.takeFirst();
    EXPECT_EQ(args.at(0).toInt(), 1);
}

// --- slot_exportResult ---
TEST_F(LogViewerPlugin_UT, slot_exportResult_UT001)
{
    QSignalSpy spy(m_instance, &LogViewerPlugin::sigExportResult);
    m_instance->slot_exportResult(true);
    EXPECT_EQ(spy.count(), 1);
    EXPECT_EQ(spy.takeFirst().at(0).toBool(), true);

    m_instance->slot_exportResult(false);
    EXPECT_EQ(spy.count(), 1);
    EXPECT_EQ(spy.takeFirst().at(0).toBool(), false);
}

// --- exportAppLogFile (empty appid → return false) ---
TEST_F(LogViewerPlugin_UT, exportAppLogFile_UT001)
{
    // Empty appid → getPathByAppId returns empty → return false
    bool result = m_instance->exportAppLogFile("/tmp/test.txt", ALL, INF, "");
    EXPECT_FALSE(result);
}

// --- exportAppLogFile (valid path via direct m_curAppLog set) ---
TEST_F(LogViewerPlugin_UT, exportAppLogFile_UT002)
{
    Stub stub;
    stub.set((void (QThreadPool::*)(QRunnable *, int))ADDR(QThreadPool, start), QThreadPool_start);
    stub.set(ADDR(QThread, start), QThread_start);

    // Set m_curAppLog directly to simulate non-empty path from getPathByAppId
    m_instance->m_curAppLog = "/tmp/test_app.log";
    m_instance->m_flag = APP;

    // Call exportLogFile directly with each format
    QTemporaryDir tmpDir;
    ASSERT_TRUE(tmpDir.isValid());

    QString txtPath = tmpDir.filePath("export.txt");
    m_instance->m_flag = JOURNAL;
    m_instance->exportLogFile(txtPath);
}

// --- exportLogFile — comprehensive: all LOG_FLAG values × all formats ---
class LogViewerPlugin_exportLogFile_UT : public ::testing::TestWithParam<std::tuple<int, QString>>
{
};

INSTANTIATE_TEST_CASE_P(LogViewerPlugin, LogViewerPlugin_exportLogFile_UT,
                        ::testing::Combine(
                            ::testing::Values(JOURNAL, KERN, BOOT, XORG, DPKG, APP,
                                              Normal, Kwin, BOOT_KLU, Dnf, Dmesg, NONE),
                            ::testing::Values("txt", "html", "doc", "xls")
                        ));

TEST_P(LogViewerPlugin_exportLogFile_UT, exportLogFile_all_combos)
{
    Stub stub;
    stub.set((void (QThreadPool::*)(QRunnable *, int))ADDR(QThreadPool, start), QThreadPool_start);
    stub.set(ADDR(QThread, start), QThread_start);

    LogViewerPlugin *p = new LogViewerPlugin();
    ASSERT_NE(p, nullptr);

    int flag = std::get<0>(GetParam());
    QString format = std::get<1>(GetParam());

    p->m_flag = static_cast<LOG_FLAG>(flag);
    p->m_curAppLog = "/tmp/test_app_export.log";

    QTemporaryDir tmpDir;
    ASSERT_TRUE(tmpDir.isValid());
    QString filePath = tmpDir.filePath("export_test." + format);

    p->exportLogFile(filePath);

    delete p;
}

// ============================================================================
// Lambda coverage: exportAppLogFile lambda (triggers exportLogFile via signal)
// ============================================================================

QString stub_getPathByAppId(const QString &)
{
    return "/tmp/test_app_export.log";
}

TEST_F(LogViewerPlugin_UT, exportAppLogFile_lambda_UT001)
{
    Stub stub;
    stub.set((void (QThreadPool::*)(QRunnable *, int))ADDR(QThreadPool, start), QThreadPool_start);
    stub.set(ADDR(QThread, start), QThread_start);
    stub.set(ADDR(LogApplicationHelper, getPathByAppId), stub_getPathByAppId);

    QTemporaryDir tmpDir;
    ASSERT_TRUE(tmpDir.isValid());
    QString exportPath = tmpDir.filePath("lambda_export.txt");

    // exportAppLogFile sets m_flag=APP, connects lambda to sigAppFinished, returns true
    bool result = m_instance->exportAppLogFile(exportPath, ALL, INF, "deepin-log-viewer");
    EXPECT_TRUE(result);
    EXPECT_EQ(m_instance->m_flag, APP);

    // Manually emit sigAppFinished to trigger the lambda which calls exportLogFile
    emit m_instance->sigAppFinished(m_instance->m_appCurrentIndex);

    // Process events to ensure the lambda executes
    QCoreApplication::processEvents();
    Utils::sleep(100);
}
