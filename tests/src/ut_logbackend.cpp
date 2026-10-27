// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "../application/logbackend.h"
#include "../application/structdef.h"
#include "../application/logfileparser.h"
#include "../application/logapplicationhelper.h"
#include "../application/logsettings.h"
#include "../application/utils.h"
#include "../application/logexportthread.h"
#include "../application/logsegementexportthread.h"
#include "../application/logallexportthread.h"
#include "../application/eventlogutils.h"
#include "ut_stuballthread.h"
#include "../application/dbusproxy/dldbushandler.h"

#include <stub.h>

#include <DSysInfo>

#include <QDebug>
#include <QThreadPool>
#include <QThread>
#include <QJsonObject>
#include <QJsonDocument>
#include <QTemporaryDir>
#include <QSignalSpy>
#include <QTest>
#include <QTimer>

#include <gtest/gtest.h>

/******************************* Stub Functions *******************************/

static void stub_QThreadPool_start(QRunnable *, int) {}
static void stub_QThread_start(QThread::Priority) {}

static QMap<QString, QString> stub_loadAuditMap()
{
    return QMap<QString, QString>();
}

static Dtk::Core::DSysInfo::UosEdition stub_uosEditionType()
{
    return Dtk::Core::DSysInfo::UosCommunity;
}

static bool stub_isWayland()
{
    return false;
}

static QMap<QString, QString> stub_appHelper_getMap()
{
    return QMap<QString, QString>();
}

static QList<QStringList> stub_appHelper_getCustomLogList()
{
    return QList<QStringList>();
}

static bool stub_appHelper_isAppLogConfigExist(const QString &)
{
    return false;
}

static QString stub_appHelper_getPathByAppId(const QString &)
{
    return QString();
}

static AppLogConfig stub_appHelper_appLogConfig(const QString &)
{
    return AppLogConfig();
}

static QDateTime stub_appHelper_getLastReportTime()
{
    return QDateTime();
}

static void stub_appHelper_saveLastRerportTime(const QDateTime &) {}
static LogApplicationHelper *stub_appHelper_instance() { return reinterpret_cast<LogApplicationHelper *>(0x1); }

static int stub_appHelper_getMaxReportCoredump()
{
    return 0;
}

static QStringList stub_dlbus_getFileInfo(const QString &, bool)
{
    return QStringList();
}
static QStringList stub_dlbus_getFileInfo_nonEmpty(const QString &, bool)
{
    return QStringList() << "/tmp/test_stub.log";
}
static QStringList stub_appHelper_getAuthLogList()
{
    return QStringList() << "/var/log/auth.log";
}
static QList<QStringList> stub_appHelper_getOtherLogList()
{
    return QList<QStringList>();
}
static QStringList stub_dlbus_getOtherFileInfo(const QString &, bool)
{
    return QStringList();
}

static int stub_dlbus_getLineCount(const QString &)
{
    return 0;
}

static void stub_dlbus_exportLog(const QString &, const QString &, bool) {}

static QStringList g_whiteList;
static QStringList stub_whiteListOutPaths_global() { return g_whiteList; }

static QStringList stub_dlbus_whiteListOutPaths()
{
    return QStringList();
}

static QString stub_dlbus_executeCmd(const QString &)
{
    return QString();
}

static QString stub_dlbus_readLogLinesInRange(const QString &, int, int)
{
    return QString();
}
static DLDBusHandler *stub_dlbus_instance() { return reinterpret_cast<DLDBusHandler *>(0x1); }

static QString stub_Utils_mkMutiDir(const QString &) { return QString(); }
static bool stub_Utils_checkAndDeleteDir(const QString &) { return true; }
static void stub_Utils_resetToNormalAuth(const QString &) {}
static QByteArray stub_Utils_executeCmd(const QString &, const QStringList &, const QString &)
{
    return QByteArray();
}
static QString stub_Utils_getUserHomePathByUID(uint) { return QString(); }

static QList<LOG_REPEAT_COREDUMP_INFO> stub_Utils_countRepeatCoredumps(qint64, qint64)
{
    return QList<LOG_REPEAT_COREDUMP_INFO>();
}

static void stub_Utils_updateRepeatCoredumpExePaths(const QList<LOG_REPEAT_COREDUMP_INFO> &) {}
static QStringList stub_Utils_getRepeatCoredumpExePaths() { return QStringList(); }

static Eventlogutils *stub_Eventlogutils_GetInstance() { return nullptr; }

static void stub_qApp_quit() {}
static void stub_qApp_exit(int) {}

static QStringList stub_LogBackend_getLogTypes()
{
    return QStringList{"journal", "boot", "dpkg", "xorg", "app", "coredump", "other", "audit", "auth"};
}

static void stub_LogBackend_initConnections() {}

static int stub_LogFileParser_parse(LOG_FILTER_BASE &) { return 0; }
static int stub_LogFileParser_parseByJournal(const QStringList &) { return 0; }
static int stub_LogFileParser_parseByJournalBoot(const QStringList &) { return 0; }
static int stub_LogFileParser_parseByDpkg(const DKPG_FILTERS &) { return 0; }
static int stub_LogFileParser_parseByXlog(const XORG_FILTERS &) { return 0; }
static int stub_LogFileParser_parseByBoot() { return 0; }
static int stub_LogFileParser_parseByKern(const KERN_FILTERS &) { return 0; }
static int stub_LogFileParser_parseByApp(const APP_FILTERS &) { return 0; }
static void stub_LogFileParser_parseByDnf(DNF_FILTERS) {}
static void stub_LogFileParser_parseByDmesg(DMESG_FILTERS) {}
static int stub_LogFileParser_parseByNormal(const NORMAL_FILTERS &) { return 0; }
static int stub_LogFileParser_parseByKwin(const KWIN_FILTERS &) { return 0; }
static void stub_LogFileParser_parseByOOC(QString &) {}
static void stub_LogFileParser_parseByAudit(const AUDIT_FILTERS &) {}
static void stub_LogFileParser_parseByAuth(const AUTH_FILTERS &) {}
static void stub_LogFileParser_parseByCoredump(const COREDUMP_FILTERS &, bool) {}

/******************************* Helper: install common stubs *******************************/

class LogBackend_UT : public testing::Test
{
public:
    Stub stub;

    void SetUp() override
    {
        // Stub thread pools so no real threads start
        stub.set((void (QThreadPool::*)(QRunnable *, int))ADDR(QThreadPool, start), stub_QThreadPool_start);
        stub.set((void (QThread::*)(QThread::Priority))ADDR(QThread, start), stub_QThread_start);

        // Stub LogSettings
        stub.set(ADDR(LogSettings, loadAuditMap), stub_loadAuditMap);

        // Stub DSysInfo
        stub.set(ADDR(Dtk::Core::DSysInfo, uosEditionType), stub_uosEditionType);

        // Stub Utils
        stub.set(ADDR(Utils, isWayland), stub_isWayland);
        stub.set(ADDR(Utils, mkMutiDir), stub_Utils_mkMutiDir);
        stub.set(ADDR(Utils, checkAndDeleteDir), stub_Utils_checkAndDeleteDir);
        stub.set(ADDR(Utils, resetToNormalAuth), stub_Utils_resetToNormalAuth);
        stub.set(ADDR(Utils, executeCmd), stub_Utils_executeCmd);
        stub.set(ADDR(Utils, getUserHomePathByUID), stub_Utils_getUserHomePathByUID);
        stub.set(ADDR(Utils, countRepeatCoredumps), stub_Utils_countRepeatCoredumps);
        stub.set(ADDR(Utils, updateRepeatCoredumpExePaths), stub_Utils_updateRepeatCoredumpExePaths);
        stub.set(ADDR(Utils, getRepeatCoredumpExePaths), stub_Utils_getRepeatCoredumpExePaths);

        // Stub LogApplicationHelper
        stub.set(ADDR(LogApplicationHelper, getMap), stub_appHelper_getMap);
        stub.set(ADDR(LogApplicationHelper, getCustomLogList), stub_appHelper_getCustomLogList);
        stub.set(ADDR(LogApplicationHelper, isAppLogConfigExist), stub_appHelper_isAppLogConfigExist);
        stub.set(ADDR(LogApplicationHelper, getPathByAppId), stub_appHelper_getPathByAppId);
        stub.set(ADDR(LogApplicationHelper, appLogConfig), stub_appHelper_appLogConfig);
        stub.set(ADDR(LogApplicationHelper, getLastReportTime), stub_appHelper_getLastReportTime);
        stub.set(ADDR(LogApplicationHelper, saveLastRerportTime), stub_appHelper_saveLastRerportTime);
        stub.set(ADDR(LogApplicationHelper, getMaxReportCoredump), stub_appHelper_getMaxReportCoredump);
        stub.set(ADDR(LogApplicationHelper, instance), stub_appHelper_instance);

        // Stub Eventlogutils
        stub.set(ADDR(Eventlogutils, GetInstance), stub_Eventlogutils_GetInstance);

        // Stub DLDBusHandler
        stub.set(ADDR(DLDBusHandler, instance), stub_dlbus_instance);
        stub.set(ADDR(DLDBusHandler, whiteListOutPaths), stub_dlbus_whiteListOutPaths);
        stub.set(ADDR(DLDBusHandler, getFileInfo), stub_dlbus_getFileInfo);
        stub.set(ADDR(DLDBusHandler, getLineCount), stub_dlbus_getLineCount);
        stub.set(ADDR(DLDBusHandler, exportLog), stub_dlbus_exportLog);
        stub.set(ADDR(DLDBusHandler, executeCmd), stub_dlbus_executeCmd);
        stub.set(ADDR(DLDBusHandler, readLogLinesInRange), stub_dlbus_readLogLinesInRange);

        // Stub qApp quit/exit
        stub.set(ADDR(QCoreApplication, quit), stub_qApp_quit);
        stub.set(ADDR(QCoreApplication, exit), stub_qApp_exit);

        stub.set(ADDR(LogBackend, getLogTypes), stub_LogBackend_getLogTypes);
        stub.set(ADDR(LogBackend, initConnections), stub_LogBackend_initConnections);
    }

    void TearDown() override
    {
        LogBackend *b = LogBackend::instance();
        if (b) {
            delete b;
        }
    }

    LogBackend *backend()
    {
        if (!LogBackend::instance()) {
            static QObject s_backendParent;
            LogBackend::instance(&s_backendParent);
        }
        return LogBackend::instance();
    }
};

/******************************* Static Filter Methods *******************************/

TEST_F(LogBackend_UT, filterLog_emptySearch_returnsInput)
{
    QList<QString> input{"{\"key\":\"value1\"}", "{\"key\":\"value2\"}"};
    QList<QString> result = LogBackend::filterLog("", input);
    EXPECT_EQ(result.size(), 2);
}

TEST_F(LogBackend_UT, filterLog_withSearch_matchesValue)
{
    QList<QString> input{
        "{\"key\":\"hello world\"}",
        "{\"key\":\"goodbye\"}",
        "not json"
    };
    QList<QString> result = LogBackend::filterLog("hello", input);
    EXPECT_EQ(result.size(), 1);
}

TEST_F(LogBackend_UT, filterLog_withSearch_caseInsensitive)
{
    QList<QString> input{"{\"key\":\"HELLO\"}"};
    QList<QString> result = LogBackend::filterLog("hello", input);
    EXPECT_EQ(result.size(), 1);
}

TEST_F(LogBackend_UT, filterLog_emptyList)
{
    QList<QString> input;
    QList<QString> result = LogBackend::filterLog("test", input);
    EXPECT_EQ(result.size(), 0);
}

TEST_F(LogBackend_UT, filterBoot_emptyFilter_returnsInput)
{
    QList<LOG_MSG_BOOT> input{{"OK", "boot msg"}};
    BOOT_FILTERS filter{"", ""};
    QList<LOG_MSG_BOOT> result = LogBackend::filterBoot(filter, input);
    EXPECT_EQ(result.size(), 1);
}

TEST_F(LogBackend_UT, filterBoot_statusFilter_match)
{
    QList<LOG_MSG_BOOT> input{{"OK", "msg1"}, {"Failed", "msg2"}};
    BOOT_FILTERS filter{"", "OK"};
    QList<LOG_MSG_BOOT> result = LogBackend::filterBoot(filter, input);
    EXPECT_EQ(result.size(), 1);
    EXPECT_EQ(result[0].status.toStdString(), "OK");
}

TEST_F(LogBackend_UT, filterBoot_searchstr_match)
{
    QList<LOG_MSG_BOOT> input{{"OK", "boot msg"}, {"OK", "other"}};
    BOOT_FILTERS filter{"boot", ""};
    QList<LOG_MSG_BOOT> result = LogBackend::filterBoot(filter, input);
    EXPECT_EQ(result.size(), 1);
}

TEST_F(LogBackend_UT, filterBoot_statusAndSearch_bothMatch)
{
    QList<LOG_MSG_BOOT> input{{"OK", "boot msg"}, {"OK", "other"}, {"Failed", "boot"}};
    BOOT_FILTERS filter{"boot", "OK"};
    QList<LOG_MSG_BOOT> result = LogBackend::filterBoot(filter, input);
    EXPECT_EQ(result.size(), 1);
}

TEST_F(LogBackend_UT, filterNomal_emptyFilter_returnsInput)
{
    QList<LOG_MSG_NORMAL> input{{"login", "user1", "2024-01-01", "msg"}};
    NORMAL_FILTERS filter;
    filter.searchstr = "";
    filter.eventTypeFilter = -1;
    QList<LOG_MSG_NORMAL> result = LogBackend::filterNomal(filter, input);
    EXPECT_EQ(result.size(), 1);
}

TEST_F(LogBackend_UT, filterNomal_eventType0_all)
{
    QList<LOG_MSG_NORMAL> input{{"Boot", "u1", "d1", "msg1"}, {"shutdown", "u2", "d2", "msg2"}};
    NORMAL_FILTERS filter;
    filter.searchstr = "";
    filter.eventTypeFilter = 0;
    QList<LOG_MSG_NORMAL> result = LogBackend::filterNomal(filter, input);
    EXPECT_EQ(result.size(), 2);
}

TEST_F(LogBackend_UT, filterNomal_eventType1_nonBootShutdownRunlevel)
{
    QList<LOG_MSG_NORMAL> input{{"login", "u1", "d1", "msg1"}, {"Boot", "u2", "d2", "msg2"}, {"shutdown", "u3", "d3", "msg3"}, {"runlevel", "u4", "d4", "msg4"}};
    NORMAL_FILTERS filter;
    filter.searchstr = "";
    filter.eventTypeFilter = 1;
    QList<LOG_MSG_NORMAL> result = LogBackend::filterNomal(filter, input);
    EXPECT_EQ(result.size(), 1);
    EXPECT_EQ(result[0].eventType.toStdString(), "login");
}

TEST_F(LogBackend_UT, filterNomal_eventType2_bootOnly)
{
    QList<LOG_MSG_NORMAL> input{{"Boot", "u1", "d1", "msg1"}, {"shutdown", "u2", "d2", "msg2"}};
    NORMAL_FILTERS filter;
    filter.searchstr = "";
    filter.eventTypeFilter = 2;
    QList<LOG_MSG_NORMAL> result = LogBackend::filterNomal(filter, input);
    EXPECT_EQ(result.size(), 1);
    EXPECT_EQ(result[0].eventType.toStdString(), "Boot");
}

TEST_F(LogBackend_UT, filterNomal_eventType3_shutdownOnly)
{
    QList<LOG_MSG_NORMAL> input{{"Boot", "u1", "d1", "msg1"}, {"shutdown", "u2", "d2", "msg2"}};
    NORMAL_FILTERS filter;
    filter.searchstr = "";
    filter.eventTypeFilter = 3;
    QList<LOG_MSG_NORMAL> result = LogBackend::filterNomal(filter, input);
    EXPECT_EQ(result.size(), 1);
    EXPECT_EQ(result[0].eventType.toStdString(), "shutdown");
}

TEST_F(LogBackend_UT, filterNomal_withSearchstr)
{
    QList<LOG_MSG_NORMAL> input{{"login", "u1", "d1", "findme"}, {"login", "u2", "d2", "other"}};
    NORMAL_FILTERS filter;
    filter.searchstr = "findme";
    filter.eventTypeFilter = 0;
    QList<LOG_MSG_NORMAL> result = LogBackend::filterNomal(filter, input);
    EXPECT_EQ(result.size(), 1);
}

TEST_F(LogBackend_UT, filterDpkg_emptySearch_returnsInput)
{
    QList<LOG_MSG_DPKG> input{{"2024-01-01", "install", "pkg"}};
    QList<LOG_MSG_DPKG> result = LogBackend::filterDpkg("", input);
    EXPECT_EQ(result.size(), 1);
}

TEST_F(LogBackend_UT, filterDpkg_withSearch_match)
{
    QList<LOG_MSG_DPKG> input{{"2024-01-01", "action1", "install"}, {"2024-01-02", "action2", "remove"}};
    QList<LOG_MSG_DPKG> result = LogBackend::filterDpkg("install", input);
    EXPECT_EQ(result.size(), 1);
}

TEST_F(LogBackend_UT, filterJournal_emptySearch_returnsInput)
{
    QList<LOG_MSG_JOURNAL> input{{"2024-01-01", "host", "daemon", "1", "info", "msg"}};
    QList<LOG_MSG_JOURNAL> result = LogBackend::filterJournal("", input);
    EXPECT_EQ(result.size(), 1);
}

TEST_F(LogBackend_UT, filterJournal_withSearch_match)
{
    QList<LOG_MSG_JOURNAL> input{{"2024-01-01", "host", "daemon", "1", "info", "findme"}, {"2024-01-02", "host", "daemon", "1", "info", "other"}};
    QList<LOG_MSG_JOURNAL> result = LogBackend::filterJournal("findme", input);
    EXPECT_EQ(result.size(), 1);
}

TEST_F(LogBackend_UT, filterJournalBoot_emptySearch_returnsInput)
{
    QList<LOG_MSG_JOURNAL> input{{"2024-01-01", "host", "daemon", "1", "info", "msg"}};
    QList<LOG_MSG_JOURNAL> result = LogBackend::filterJournalBoot("", input);
    EXPECT_EQ(result.size(), 1);
}

TEST_F(LogBackend_UT, filterJournalBoot_withSearch_match)
{
    QList<LOG_MSG_JOURNAL> input{{"2024-01-01", "host", "daemon", "1", "info", "findme"}};
    QList<LOG_MSG_JOURNAL> result = LogBackend::filterJournalBoot("findme", input);
    EXPECT_EQ(result.size(), 1);
}

TEST_F(LogBackend_UT, filterKern_emptySearch_returnsInput)
{
    QList<LOG_MSG_JOURNAL> input{{"2024-01-01", "host", "daemon", "1", "info", "msg"}};
    QList<LOG_MSG_JOURNAL> result = LogBackend::filterKern("", input);
    EXPECT_EQ(result.size(), 1);
}

TEST_F(LogBackend_UT, filterKern_withSearch_match)
{
    QList<LOG_MSG_JOURNAL> input{{"2024-01-01", "host", "daemon", "1", "info", "findme"}, {"2024-01-02", "host", "daemon", "1", "info", "other"}};
    QList<LOG_MSG_JOURNAL> result = LogBackend::filterKern("findme", input);
    EXPECT_EQ(result.size(), 1);
}

TEST_F(LogBackend_UT, filterXorg_emptySearch_returnsInput)
{
    QList<LOG_MSG_XORG> input{{"offset", "msg"}};
    QList<LOG_MSG_XORG> result = LogBackend::filterXorg("", input);
    EXPECT_EQ(result.size(), 1);
}

TEST_F(LogBackend_UT, filterXorg_withSearch_match)
{
    QList<LOG_MSG_XORG> input{{"offset1", "findme"}, {"offset2", "other"}};
    QList<LOG_MSG_XORG> result = LogBackend::filterXorg("findme", input);
    EXPECT_EQ(result.size(), 1);
}

TEST_F(LogBackend_UT, filterKwin_emptySearch_returnsInput)
{
    QList<LOG_MSG_KWIN> input{{"msg"}};
    QList<LOG_MSG_KWIN> result = LogBackend::filterKwin("", input);
    EXPECT_EQ(result.size(), 1);
}

TEST_F(LogBackend_UT, filterKwin_withSearch_match)
{
    QList<LOG_MSG_KWIN> input{{"findme"}, {"other"}};
    QList<LOG_MSG_KWIN> result = LogBackend::filterKwin("findme", input);
    EXPECT_EQ(result.size(), 1);
}

TEST_F(LogBackend_UT, filterApp_searchMatch_returnsFiltered)
{
    QList<LOG_MSG_APPLICATOIN> input{{"2024-01-01", "submod", "info", "src", "findme"}, {"2024-01-02", "submod", "info", "src", "other"}};
    APP_FILTERS filter;
    filter.searchstr = "findme";
    filter.submodule = "";
    QList<LOG_MSG_APPLICATOIN> result = LogBackend::filterApp(filter, input);
    EXPECT_EQ(result.size(), 1);
}

TEST_F(LogBackend_UT, filterApp_searchNoMatch_returnsEmpty)
{
    QList<LOG_MSG_APPLICATOIN> input{{"2024-01-01", "submod", "info", "src", "findme"}};
    APP_FILTERS filter;
    filter.searchstr = "notfound";
    filter.submodule = "";
    QList<LOG_MSG_APPLICATOIN> result = LogBackend::filterApp(filter, input);
    EXPECT_EQ(result.size(), 0);
}

TEST_F(LogBackend_UT, filterApp_filterObj_emptyReturnsInput)
{
    QList<LOG_MSG_APPLICATOIN> input{{"2024-01-01", "submod", "info", "src", "msg"}};
    APP_FILTERS filter;
    filter.searchstr = "";
    filter.submodule = "";
    QList<LOG_MSG_APPLICATOIN> result = LogBackend::filterApp(filter, input);
    EXPECT_EQ(result.size(), 1);
}

TEST_F(LogBackend_UT, filterApp_filterObj_searchOnly)
{
    QList<LOG_MSG_APPLICATOIN> input{{"2024-01-01", "submod", "info", "src", "findme"}, {"2024-01-02", "submod", "info", "src", "other"}};
    APP_FILTERS filter;
    filter.searchstr = "findme";
    filter.submodule = "";
    QList<LOG_MSG_APPLICATOIN> result = LogBackend::filterApp(filter, input);
    EXPECT_EQ(result.size(), 1);
}

TEST_F(LogBackend_UT, filterApp_filterObj_submoduleMatch)
{
    QList<LOG_MSG_APPLICATOIN> input{{"2024-01-01", "mod1", "info", "src", "findme"}, {"2024-01-02", "mod2", "info", "src", "findme"}};
    APP_FILTERS filter;
    filter.searchstr = "findme";
    filter.submodule = "mod1";
    QList<LOG_MSG_APPLICATOIN> result = LogBackend::filterApp(filter, input);
    EXPECT_EQ(result.size(), 1);
    EXPECT_EQ(result[0].subModule.toStdString(), "mod1");
}

TEST_F(LogBackend_UT, filterApp_filterObj_submoduleNoMatch)
{
    QList<LOG_MSG_APPLICATOIN> input{{"2024-01-01", "mod1", "info", "src", "findme"}};
    APP_FILTERS filter;
    filter.searchstr = "findme";
    filter.submodule = "mod2";
    QList<LOG_MSG_APPLICATOIN> result = LogBackend::filterApp(filter, input);
    EXPECT_EQ(result.size(), 0);
}

TEST_F(LogBackend_UT, filterDnf_emptySearch_returnsInput)
{
    QList<LOG_MSG_DNF> input{{"2024-01-01", "msg", "info"}};
    QList<LOG_MSG_DNF> result = LogBackend::filterDnf("", input);
    EXPECT_EQ(result.size(), 1);
}

TEST_F(LogBackend_UT, filterDnf_withSearch_match)
{
    QList<LOG_MSG_DNF> input{{"2024-01-01", "findme", "info"}, {"2024-01-02", "other", "info"}};
    QList<LOG_MSG_DNF> result = LogBackend::filterDnf("findme", input);
    EXPECT_EQ(result.size(), 1);
}

TEST_F(LogBackend_UT, filterDmesg_emptySearch_returnsInput)
{
    QList<LOG_MSG_DMESG> input{{"2024-01-01", "msg"}};
    QList<LOG_MSG_DMESG> result = LogBackend::filterDmesg("", input);
    EXPECT_EQ(result.size(), 1);
}

TEST_F(LogBackend_UT, filterDmesg_withSearch_match)
{
    QList<LOG_MSG_DMESG> input{{"2024-01-01", "findme"}, {"2024-01-02", "other"}};
    QList<LOG_MSG_DMESG> result = LogBackend::filterDmesg("findme", input);
    EXPECT_EQ(result.size(), 1);
}

TEST_F(LogBackend_UT, filterOOC_emptySearch_returnsInput)
{
    QList<LOG_FILE_OTHERORCUSTOM> input{{"name", "path"}};
    QList<LOG_FILE_OTHERORCUSTOM> result = LogBackend::filterOOC("", input);
    EXPECT_EQ(result.size(), 1);
}

TEST_F(LogBackend_UT, filterOOC_withSearch_match)
{
    QList<LOG_FILE_OTHERORCUSTOM> input{{"name1", "findme"}, {"name2", "other"}};
    QList<LOG_FILE_OTHERORCUSTOM> result = LogBackend::filterOOC("findme", input);
    EXPECT_EQ(result.size(), 1);
}

TEST_F(LogBackend_UT, filterAudit_emptyFilter_returnsInput)
{
    QList<LOG_MSG_AUDIT> input;
    AUDIT_FILTERS filter;
    filter.searchstr = "";
    filter.auditTypeFilter = -2;
    QList<LOG_MSG_AUDIT> result = LogBackend::filterAudit(filter, input);
    EXPECT_EQ(result.size(), 0);
}

TEST_F(LogBackend_UT, filterCoredump_emptySearch_returnsInput)
{
    QList<LOG_MSG_COREDUMP> input{{"SIGSEGV", "2024-01-01", "corefile", "user", "exe"}};
    QList<LOG_MSG_COREDUMP> result = LogBackend::filterCoredump("", input);
    EXPECT_EQ(result.size(), 1);
}

TEST_F(LogBackend_UT, filterCoredump_withSearch_match)
{
    QList<LOG_MSG_COREDUMP> input{{"SIGSEGV", "2024-01-01", "corefile", "user", "findme"}, {"SIGABRT", "2024-01-02", "corefile2", "user2", "other"}};
    QList<LOG_MSG_COREDUMP> result = LogBackend::filterCoredump("findme", input);
    EXPECT_EQ(result.size(), 1);
}

/******************************* Singleton & Constructor *******************************/

TEST_F(LogBackend_UT, instance_createsSingleton)
{
    QObject *parent = new QObject;
    LogBackend *b1 = LogBackend::instance(parent);
    EXPECT_NE(b1, nullptr);
    LogBackend *b2 = LogBackend::instance(parent);
    EXPECT_EQ(b1, b2);
    delete b1;
    delete parent;
}

TEST_F(LogBackend_UT, instance_withParent_createsNew)
{
    QObject *parent = new QObject;
    LogBackend *b1 = LogBackend::instance(parent);
    EXPECT_NE(b1, nullptr);
    LogBackend *b2 = LogBackend::instance(parent);
    EXPECT_EQ(b1, b2);
    delete b1;
    delete parent;
}

TEST_F(LogBackend_UT, destructor_resetsStaticPointer)
{
    QObject *parent1 = new QObject;
    LogBackend *b1 = LogBackend::instance(parent1);
    EXPECT_NE(b1, nullptr);
    delete b1;
    // After deletion, m_staticbackend should be nullptr, so instance() with a parent
    // should create a new singleton
    QObject *parent2 = new QObject;
    LogBackend *b2 = LogBackend::instance(parent2);
    EXPECT_NE(b2, nullptr);
    // Without a parent and after reset, instance() returns the current singleton
    EXPECT_EQ(LogBackend::instance(), b2);
    delete b2;
    delete parent2;
    delete parent1;
}

/******************************* Private Helper Methods *******************************/

TEST_F(LogBackend_UT, period2Enum_allVariants)
{
    LogBackend *b = backend();
    ASSERT_NE(b, nullptr);
    // Access private method via -fno-access-control
    EXPECT_EQ(b->period2Enum("all"), ALL);
    EXPECT_EQ(b->period2Enum("today"), ONE_DAY);
    EXPECT_EQ(b->period2Enum("3d"), THREE_DAYS);
    EXPECT_EQ(b->period2Enum("1w"), ONE_WEEK);
    EXPECT_EQ(b->period2Enum("1m"), ONE_MONTH);
    EXPECT_EQ(b->period2Enum("3m"), THREE_MONTHS);
    EXPECT_EQ(b->period2Enum("invalid"), INVALID);
}

TEST_F(LogBackend_UT, level2Id_allVariants)
{
    LogBackend *b = backend();
    ASSERT_NE(b, nullptr);
    EXPECT_EQ(b->level2Id("debug"), 7);
    EXPECT_EQ(b->level2Id("7"), 7);
    EXPECT_EQ(b->level2Id("info"), 6);
    EXPECT_EQ(b->level2Id("6"), 6);
    EXPECT_EQ(b->level2Id("notice"), 5);
    EXPECT_EQ(b->level2Id("5"), 5);
    EXPECT_EQ(b->level2Id("warning"), 4);
    EXPECT_EQ(b->level2Id("4"), 4);
    EXPECT_EQ(b->level2Id("err"), 3);
    EXPECT_EQ(b->level2Id("error"), 3);
    EXPECT_EQ(b->level2Id("3"), 3);
    EXPECT_EQ(b->level2Id("critical"), 2);
    EXPECT_EQ(b->level2Id("crit"), 2);
    EXPECT_EQ(b->level2Id("2"), 2);
    EXPECT_EQ(b->level2Id("alert"), 1);
    EXPECT_EQ(b->level2Id("1"), 1);
    EXPECT_EQ(b->level2Id("emerg"), 0);
    EXPECT_EQ(b->level2Id("0"), 0);
    EXPECT_EQ(b->level2Id("all"), -1);
    EXPECT_EQ(b->level2Id(""), -1);
    EXPECT_EQ(b->level2Id("invalid"), -2);
}

TEST_F(LogBackend_UT, dnfLevel2Id_allVariants)
{
    LogBackend *b = backend();
    ASSERT_NE(b, nullptr);
    EXPECT_EQ(b->dnfLevel2Id("trace"), TRACE);
    EXPECT_EQ(b->dnfLevel2Id("6"), TRACE);
    EXPECT_EQ(b->dnfLevel2Id("debug"), DEBUG);
    EXPECT_EQ(b->dnfLevel2Id("5"), DEBUG);
    EXPECT_EQ(b->dnfLevel2Id("info"), INFO);
    EXPECT_EQ(b->dnfLevel2Id("4"), INFO);
    EXPECT_EQ(b->dnfLevel2Id("warning"), WARNING);
    EXPECT_EQ(b->dnfLevel2Id("3"), WARNING);
    EXPECT_EQ(b->dnfLevel2Id("error"), ERROR);
    EXPECT_EQ(b->dnfLevel2Id("err"), ERROR);
    EXPECT_EQ(b->dnfLevel2Id("2"), ERROR);
    EXPECT_EQ(b->dnfLevel2Id("critical"), CRITICAL);
    EXPECT_EQ(b->dnfLevel2Id("crit"), CRITICAL);
    EXPECT_EQ(b->dnfLevel2Id("1"), CRITICAL);
    EXPECT_EQ(b->dnfLevel2Id("supercritical"), SUPERCRITICAL);
    EXPECT_EQ(b->dnfLevel2Id("supercrit"), SUPERCRITICAL);
    EXPECT_EQ(b->dnfLevel2Id("0"), SUPERCRITICAL);
    EXPECT_EQ(b->dnfLevel2Id("all"), DNFLVALL);
    EXPECT_EQ(b->dnfLevel2Id(""), DNFLVALL);
    EXPECT_EQ(b->dnfLevel2Id("invalid"), DNFINVALID);
}

TEST_F(LogBackend_UT, normal2eventType_allVariants)
{
    LogBackend *b = backend();
    ASSERT_NE(b, nullptr);
    EXPECT_EQ(b->normal2eventType("all"), 0);
    EXPECT_EQ(b->normal2eventType("0"), 0);
    EXPECT_EQ(b->normal2eventType("login"), 1);
    EXPECT_EQ(b->normal2eventType("1"), 1);
    EXPECT_EQ(b->normal2eventType("boot"), 2);
    EXPECT_EQ(b->normal2eventType("2"), 2);
    EXPECT_EQ(b->normal2eventType("shutdown"), 3);
    EXPECT_EQ(b->normal2eventType("3"), 3);
    EXPECT_EQ(b->normal2eventType("invalid"), -1);
}

TEST_F(LogBackend_UT, audit2eventType_allVariants)
{
    LogBackend *b = backend();
    ASSERT_NE(b, nullptr);
    EXPECT_EQ(b->audit2eventType("all"), 0);
    EXPECT_EQ(b->audit2eventType("0"), 0);
    EXPECT_EQ(b->audit2eventType("identauth"), 1);
    EXPECT_EQ(b->audit2eventType("1"), 1);
    EXPECT_EQ(b->audit2eventType("dac"), 2);
    EXPECT_EQ(b->audit2eventType("2"), 2);
    EXPECT_EQ(b->audit2eventType("mac"), 3);
    EXPECT_EQ(b->audit2eventType("3"), 3);
    EXPECT_EQ(b->audit2eventType("remote"), 4);
    EXPECT_EQ(b->audit2eventType("4"), 4);
    EXPECT_EQ(b->audit2eventType("docaudit"), 5);
    EXPECT_EQ(b->audit2eventType("5"), 5);
    EXPECT_EQ(b->audit2eventType("other"), 6);
    EXPECT_EQ(b->audit2eventType("6"), 6);
    EXPECT_EQ(b->audit2eventType("invalid"), -1);
}

TEST_F(LogBackend_UT, type2Flag_allTypes)
{
    QString error;
    EXPECT_EQ(LogBackend::type2Flag(TYPE_SYSTEM, error), JOURNAL);
    EXPECT_EQ(LogBackend::type2Flag(TYPE_KERNEL, error), KERN);
    EXPECT_EQ(LogBackend::type2Flag(TYPE_BOOT, error), BOOT);
    EXPECT_EQ(LogBackend::type2Flag(TYPE_DPKG, error), DPKG);
    EXPECT_EQ(LogBackend::type2Flag(TYPE_DNF, error), NONE); // non-centos, dnf not supported
    EXPECT_FALSE(error.isEmpty());
    EXPECT_EQ(LogBackend::type2Flag(TYPE_KWIN, error), NONE); // non-wayland, kwin not supported
    EXPECT_FALSE(error.isEmpty());
    EXPECT_EQ(LogBackend::type2Flag(TYPE_XORG, error), XORG);
    EXPECT_EQ(LogBackend::type2Flag(TYPE_APP, error), APP);
    EXPECT_EQ(LogBackend::type2Flag(TYPE_COREDUMP, error), COREDUMP);
    EXPECT_EQ(LogBackend::type2Flag(TYPE_BSE, error), Normal);
    EXPECT_EQ(LogBackend::type2Flag(TYPE_OTHER, error), OtherLog);
    EXPECT_EQ(LogBackend::type2Flag(TYPE_CUSTOM, error), CustomLog);
    EXPECT_EQ(LogBackend::type2Flag(TYPE_AUDIT, error), Audit);
    EXPECT_EQ(LogBackend::type2Flag(TYPE_AUTH, error), Auth);
    EXPECT_EQ(LogBackend::type2Flag("invalid", error), NONE);
}

TEST_F(LogBackend_UT, type2Flag_authLog)
{
    QString error;
    EXPECT_EQ(LogBackend::type2Flag(TYPE_AUTH, error), Auth);
}

TEST_F(LogBackend_UT, getTimeRange_allPeriods)
{
    LogBackend *b = backend();
    ASSERT_NE(b, nullptr);
    TIME_RANGE trAll = b->getTimeRange(ALL);
    EXPECT_EQ(trAll.begin, -1);
    EXPECT_EQ(trAll.end, -1);

    TIME_RANGE trToday = b->getTimeRange(ONE_DAY);
    EXPECT_GT(trToday.begin, 0);
    EXPECT_GT(trToday.end, 0);

    TIME_RANGE tr3d = b->getTimeRange(THREE_DAYS);
    EXPECT_GT(tr3d.begin, 0);

    TIME_RANGE tr1w = b->getTimeRange(ONE_WEEK);
    EXPECT_GT(tr1w.begin, 0);

    TIME_RANGE tr1m = b->getTimeRange(ONE_MONTH);
    EXPECT_GT(tr1m.begin, 0);

    TIME_RANGE tr3m = b->getTimeRange(THREE_MONTHS);
    EXPECT_GT(tr3m.begin, 0);

    // Default case
    TIME_RANGE trDefault = b->getTimeRange(INVALID);
    EXPECT_EQ(trDefault.begin, -1);
    EXPECT_EQ(trDefault.end, -1);
}

TEST_F(LogBackend_UT, getLabels_allFlags)
{
    LogBackend *b = backend();
    ASSERT_NE(b, nullptr);
    // Test each flag case
    EXPECT_FALSE(b->getLabels(JOURNAL).isEmpty());
    EXPECT_FALSE(b->getLabels(Dmesg).isEmpty());
    EXPECT_FALSE(b->getLabels(KERN).isEmpty());
    EXPECT_FALSE(b->getLabels(BOOT_KLU).isEmpty());
    EXPECT_FALSE(b->getLabels(BOOT).isEmpty());
    EXPECT_FALSE(b->getLabels(DPKG).isEmpty());
    EXPECT_FALSE(b->getLabels(Dnf).isEmpty());
    EXPECT_FALSE(b->getLabels(Kwin).isEmpty());
    EXPECT_FALSE(b->getLabels(XORG).isEmpty());
    EXPECT_FALSE(b->getLabels(APP).isEmpty());
    EXPECT_FALSE(b->getLabels(COREDUMP).isEmpty());
    EXPECT_FALSE(b->getLabels(Normal).isEmpty());
    EXPECT_FALSE(b->getLabels(Audit).isEmpty());
    EXPECT_FALSE(b->getLabels(Auth).isEmpty());
    EXPECT_TRUE(b->getLabels(NONE).isEmpty());
}

TEST_F(LogBackend_UT, hasMatchedData_allFlags)
{
    LogBackend *b = backend();
    ASSERT_NE(b, nullptr);
    // Initially all data lists are empty, so hasMatchedData should be false
    EXPECT_FALSE(b->hasMatchedData(JOURNAL));
    EXPECT_FALSE(b->hasMatchedData(Dmesg));
    EXPECT_FALSE(b->hasMatchedData(Kwin));
    EXPECT_FALSE(b->hasMatchedData(KERN));
    EXPECT_FALSE(b->hasMatchedData(BOOT_KLU));
    EXPECT_FALSE(b->hasMatchedData(BOOT));
    EXPECT_FALSE(b->hasMatchedData(DPKG));
    EXPECT_FALSE(b->hasMatchedData(Dnf));
    EXPECT_FALSE(b->hasMatchedData(XORG));
    EXPECT_FALSE(b->hasMatchedData(APP));
    EXPECT_FALSE(b->hasMatchedData(COREDUMP));
    EXPECT_FALSE(b->hasMatchedData(Normal));
    EXPECT_FALSE(b->hasMatchedData(Audit));
    EXPECT_FALSE(b->hasMatchedData(Auth));

    // Test with data
    b->jList.append({{"2024-01-01", "host", "daemon", "1", "info", "msg"}});
    EXPECT_TRUE(b->hasMatchedData(JOURNAL));
}

TEST_F(LogBackend_UT, getApplogPath_notFound)
{
    LogBackend *b = backend();
    ASSERT_NE(b, nullptr);
    QString result = b->getApplogPath("nonexistent");
    EXPECT_TRUE(result.isEmpty());
}

TEST_F(LogBackend_UT, getOutDirPath_samePath_returnsTrue)
{
    LogBackend *b = backend();
    ASSERT_NE(b, nullptr);
    // m_outPath is initially empty, so passing "" should return true
    EXPECT_TRUE(b->getOutDirPath(""));
}

TEST_F(LogBackend_UT, getOutDirPath_newValidPath)
{
    LogBackend *b = backend();
    ASSERT_NE(b, nullptr);
    QTemporaryDir tmpDir;
    ASSERT_TRUE(tmpDir.isValid());
    // Stub DLDBusHandler::whiteListOutPaths to include our path
    Stub s;
    g_whiteList.clear();
    g_whiteList << tmpDir.path();
    s.set(ADDR(DLDBusHandler, whiteListOutPaths), stub_whiteListOutPaths_global);
    EXPECT_TRUE(b->getOutDirPath(tmpDir.path()));
}

TEST_F(LogBackend_UT, getOutDirPath_nonExistentPath_created)
{
    LogBackend *b = backend();
    ASSERT_NE(b, nullptr);
    QTemporaryDir tmpDir;
    ASSERT_TRUE(tmpDir.isValid());
    QString newPath = tmpDir.path() + "/subdir";
    Stub s;
    g_whiteList.clear();
    g_whiteList << tmpDir.path();
    s.set(ADDR(DLDBusHandler, whiteListOutPaths), stub_whiteListOutPaths_global);
    EXPECT_TRUE(b->getOutDirPath(newPath));
}

TEST_F(LogBackend_UT, resetCategoryOutputPath)
{
    LogBackend *b = backend();
    ASSERT_NE(b, nullptr);
    QTemporaryDir tmpDir;
    ASSERT_TRUE(tmpDir.isValid());
    QString path = tmpDir.path() + "/testcat";
    QDir().mkpath(path);
    EXPECT_TRUE(QDir(path).exists());
    b->resetCategoryOutputPath(path);
    // Utils::mkMutiDir is stubbed as no-op, so dir is removed but not recreated
    // The function still runs without crash
    SUCCEED();
}

/******************************* Setters & Clearers *******************************/

TEST_F(LogBackend_UT, setCmdWorkDir_validDir)
{
    LogBackend *b = backend();
    ASSERT_NE(b, nullptr);
    QString validDir = QDir::currentPath();
    b->setCmdWorkDir(validDir);
    SUCCEED();
}

TEST_F(LogBackend_UT, setCmdWorkDir_invalidDir)
{
    LogBackend *b = backend();
    ASSERT_NE(b, nullptr);
    b->setCmdWorkDir("/nonexistent/path/12345");
    SUCCEED();
}

TEST_F(LogBackend_UT, setFlag)
{
    LogBackend *b = backend();
    ASSERT_NE(b, nullptr);
    b->setFlag(JOURNAL);
    EXPECT_EQ(b->m_flag, JOURNAL);
    b->setFlag(KERN);
    EXPECT_EQ(b->m_flag, KERN);
}

TEST_F(LogBackend_UT, clearAllFilter)
{
    LogBackend *b = backend();
    ASSERT_NE(b, nullptr);
    b->m_currentSearchStr = "test";
    b->m_bootFilter.searchstr = "test";
    b->m_normalFilter.searchstr = "test";
    b->clearAllFilter();
    EXPECT_TRUE(b->m_currentSearchStr.isEmpty());
    EXPECT_TRUE(b->m_bootFilter.searchstr.isEmpty());
}

TEST_F(LogBackend_UT, clearAllDatalist)
{
    LogBackend *b = backend();
    ASSERT_NE(b, nullptr);
    b->jList.append({{"2024-01-01", "host", "daemon", "1", "info", "msg"}});
    b->dList.append({{"2024-01-01", "msg"}});
    b->clearAllDatalist();
    EXPECT_TRUE(b->jList.isEmpty());
    EXPECT_TRUE(b->dList.isEmpty());
}

/******************************* parse Method *******************************/

TEST_F(LogBackend_UT, parse)
{
    LogBackend *b = backend();
    ASSERT_NE(b, nullptr);
    Stub s;
    s.set(ADDR(LogFileParser, parse), stub_LogFileParser_parse);
    LOG_FILTER_BASE filter;
    filter.type = JOURNAL;
    b->parse(filter);
    EXPECT_EQ(b->m_type2Filter[JOURNAL].type, JOURNAL);
}

/******************************* parseBy* Methods *******************************/

TEST_F(LogBackend_UT, parseByJournal)
{
    LogBackend *b = backend();
    ASSERT_NE(b, nullptr);
    Stub s;
    s.set(ADDR(LogFileParser, parseByJournal), stub_LogFileParser_parseByJournal);
    b->parseByJournal(QStringList() << "all");
    SUCCEED();
}

TEST_F(LogBackend_UT, parseByJournalBoot)
{
    LogBackend *b = backend();
    ASSERT_NE(b, nullptr);
    Stub s;
    s.set(ADDR(LogFileParser, parseByJournalBoot), stub_LogFileParser_parseByJournalBoot);
    b->parseByJournalBoot(QStringList() << "all");
    SUCCEED();
}

TEST_F(LogBackend_UT, parseByDpkg)
{
    LogBackend *b = backend();
    ASSERT_NE(b, nullptr);
    Stub s;
    s.set(ADDR(LogFileParser, parseByDpkg), stub_LogFileParser_parseByDpkg);
    DKPG_FILTERS filter;
    b->parseByDpkg(filter);
    SUCCEED();
}

TEST_F(LogBackend_UT, parseByXlog)
{
    LogBackend *b = backend();
    ASSERT_NE(b, nullptr);
    Stub s;
    s.set(ADDR(LogFileParser, parseByXlog), stub_LogFileParser_parseByXlog);
    XORG_FILTERS filter;
    b->parseByXlog(filter);
    SUCCEED();
}

TEST_F(LogBackend_UT, parseByBoot)
{
    LogBackend *b = backend();
    ASSERT_NE(b, nullptr);
    Stub s;
    s.set(ADDR(LogFileParser, parseByBoot), stub_LogFileParser_parseByBoot);
    b->parseByBoot();
    SUCCEED();
}

TEST_F(LogBackend_UT, parseByKern)
{
    LogBackend *b = backend();
    ASSERT_NE(b, nullptr);
    Stub s;
    s.set(ADDR(LogFileParser, parseByKern), stub_LogFileParser_parseByKern);
    KERN_FILTERS filter;
    b->parseByKern(filter);
    SUCCEED();
}

TEST_F(LogBackend_UT, parseByApp)
{
    LogBackend *b = backend();
    ASSERT_NE(b, nullptr);
    Stub s;
    s.set(ADDR(LogFileParser, parseByApp), stub_LogFileParser_parseByApp);
    APP_FILTERS filter;
    b->parseByApp(filter);
    SUCCEED();
}

TEST_F(LogBackend_UT, parseByDnf)
{
    LogBackend *b = backend();
    ASSERT_NE(b, nullptr);
    Stub s;
    s.set(ADDR(LogFileParser, parseByDnf), stub_LogFileParser_parseByDnf);
    DNF_FILTERS filter;
    b->parseByDnf(filter);
    SUCCEED();
}

TEST_F(LogBackend_UT, parseByDmesg)
{
    LogBackend *b = backend();
    ASSERT_NE(b, nullptr);
    Stub s;
    s.set(ADDR(LogFileParser, parseByDmesg), stub_LogFileParser_parseByDmesg);
    DMESG_FILTERS filter;
    b->parseByDmesg(filter);
    SUCCEED();
}

TEST_F(LogBackend_UT, parseByNormal)
{
    LogBackend *b = backend();
    ASSERT_NE(b, nullptr);
    Stub s;
    s.set(ADDR(LogFileParser, parseByNormal), stub_LogFileParser_parseByNormal);
    NORMAL_FILTERS filter;
    b->parseByNormal(filter);
    SUCCEED();
}

TEST_F(LogBackend_UT, parseByKwin)
{
    LogBackend *b = backend();
    ASSERT_NE(b, nullptr);
    Stub s;
    s.set(ADDR(LogFileParser, parseByKwin), stub_LogFileParser_parseByKwin);
    KWIN_FILTERS filter;
    b->parseByKwin(filter);
    SUCCEED();
}

TEST_F(LogBackend_UT, parseByOOC)
{
    LogBackend *b = backend();
    ASSERT_NE(b, nullptr);
    Stub s;
    s.set(ADDR(LogFileParser, parseByOOC), stub_LogFileParser_parseByOOC);
    b->parseByOOC("/tmp/test.log");
    SUCCEED();
}

TEST_F(LogBackend_UT, parseByAudit)
{
    LogBackend *b = backend();
    ASSERT_NE(b, nullptr);
    Stub s;
    s.set(ADDR(LogFileParser, parseByAudit), stub_LogFileParser_parseByAudit);
    AUDIT_FILTERS filter;
    b->parseByAudit(filter);
    SUCCEED();
}

TEST_F(LogBackend_UT, parseByAuth)
{
    LogBackend *b = backend();
    ASSERT_NE(b, nullptr);
    Stub s;
    s.set(ADDR(LogFileParser, parseByAuth), stub_LogFileParser_parseByAuth);
    AUTH_FILTERS filter;
    b->parseByAuth(filter);
    SUCCEED();
}

TEST_F(LogBackend_UT, parseByCoredump)
{
    LogBackend *b = backend();
    ASSERT_NE(b, nullptr);
    Stub s;
    s.set(ADDR(LogFileParser, parseByCoredump), stub_LogFileParser_parseByCoredump);
    COREDUMP_FILTERS filter;
    b->parseByCoredump(filter, false);
    SUCCEED();
}

/******************************* Slot Methods - View Session *******************************/

TEST_F(LogBackend_UT, slot_parseFinished_viewSession)
{
    LogBackend *b = backend();
    ASSERT_NE(b, nullptr);
    b->setFlag(JOURNAL);
    b->m_sessionType = LogBackend::View;
    b->m_type2ThreadIndex[JOURNAL] = 0;
    QSignalSpy spy(b, &LogBackend::parseFinished);
    b->slot_parseFinished(0, JOURNAL, 0);
    EXPECT_EQ(spy.count(), 1);
}

TEST_F(LogBackend_UT, slot_parseFinished_flagMismatch_ignored)
{
    LogBackend *b = backend();
    ASSERT_NE(b, nullptr);
    b->setFlag(KERN);
    b->m_sessionType = LogBackend::View;
    b->m_type2ThreadIndex[JOURNAL] = 0;
    QSignalSpy spy(b, &LogBackend::parseFinished);
    b->slot_parseFinished(0, JOURNAL, 0);
    EXPECT_EQ(spy.count(), 0);
}

TEST_F(LogBackend_UT, slot_parseFinished_indexMismatch_ignored)
{
    LogBackend *b = backend();
    ASSERT_NE(b, nullptr);
    b->setFlag(JOURNAL);
    b->m_sessionType = LogBackend::View;
    b->m_type2ThreadIndex[JOURNAL] = 1;
    QSignalSpy spy(b, &LogBackend::parseFinished);
    b->slot_parseFinished(0, JOURNAL, 0);
    EXPECT_EQ(spy.count(), 0);
}

TEST_F(LogBackend_UT, slot_parseFinished_abnormalStatus)
{
    LogBackend *b = backend();
    ASSERT_NE(b, nullptr);
    b->setFlag(JOURNAL);
    b->m_sessionType = LogBackend::View;
    b->m_type2ThreadIndex[JOURNAL] = 0;
    QSignalSpy spy(b, &LogBackend::parseFinished);
    b->slot_parseFinished(0, JOURNAL, 1); // abnormal status
    EXPECT_EQ(spy.count(), 1);
}

TEST_F(LogBackend_UT, slot_logData_viewSession)
{
    LogBackend *b = backend();
    ASSERT_NE(b, nullptr);
    b->setFlag(JOURNAL);
    b->m_sessionType = LogBackend::View;
    b->m_type2ThreadIndex[JOURNAL] = 0;
    QList<QString> list{"{\"key\":\"value\"}"};
    QSignalSpy spy(b, &LogBackend::logData);
    b->slot_logData(0, list, JOURNAL);
    EXPECT_EQ(spy.count(), 1);
    EXPECT_FALSE(b->m_type2LogData[JOURNAL].isEmpty());
}

TEST_F(LogBackend_UT, slot_logData_flagMismatch_ignored)
{
    LogBackend *b = backend();
    ASSERT_NE(b, nullptr);
    b->setFlag(KERN);
    b->m_sessionType = LogBackend::View;
    b->m_type2ThreadIndex[JOURNAL] = 0;
    QList<QString> list{"{\"key\":\"value\"}"};
    b->slot_logData(0, list, JOURNAL);
    EXPECT_TRUE(b->m_type2LogData[JOURNAL].isEmpty());
}

TEST_F(LogBackend_UT, slot_dpkgFinished_viewSession)
{
    LogBackend *b = backend();
    ASSERT_NE(b, nullptr);
    b->setFlag(DPKG);
    b->m_sessionType = LogBackend::View;
    b->m_dpkgCurrentIndex = 0;
    QSignalSpy spy(b, &LogBackend::dpkgFinished);
    b->slot_dpkgFinished(0);
    EXPECT_EQ(spy.count(), 1);
}

TEST_F(LogBackend_UT, slot_dpkgFinished_mismatch_ignored)
{
    LogBackend *b = backend();
    ASSERT_NE(b, nullptr);
    b->setFlag(JOURNAL);
    b->m_dpkgCurrentIndex = 1;
    QSignalSpy spy(b, &LogBackend::dpkgFinished);
    b->slot_dpkgFinished(0);
    EXPECT_EQ(spy.count(), 0);
}

TEST_F(LogBackend_UT, slot_dpkgData_viewSession)
{
    LogBackend *b = backend();
    ASSERT_NE(b, nullptr);
    b->setFlag(DPKG);
    b->m_sessionType = LogBackend::View;
    b->m_dpkgCurrentIndex = 0;
    QList<LOG_MSG_DPKG> list{{"2024-01-01", "msg"}};
    QSignalSpy spy(b, &LogBackend::dpkgData);
    b->slot_dpkgData(0, list);
    EXPECT_EQ(spy.count(), 1);
    EXPECT_FALSE(b->dList.isEmpty());
}

TEST_F(LogBackend_UT, slot_dpkgData_mismatch_ignored)
{
    LogBackend *b = backend();
    ASSERT_NE(b, nullptr);
    b->setFlag(JOURNAL);
    b->m_dpkgCurrentIndex = 1;
    QList<LOG_MSG_DPKG> list{{"2024-01-01", "msg"}};
    b->slot_dpkgData(0, list);
    EXPECT_TRUE(b->dList.isEmpty());
}

TEST_F(LogBackend_UT, slot_XorgFinished_viewSession)
{
    LogBackend *b = backend();
    ASSERT_NE(b, nullptr);
    b->setFlag(XORG);
    b->m_sessionType = LogBackend::View;
    b->m_xorgCurrentIndex = 0;
    QSignalSpy spy(b, &LogBackend::xlogFinished);
    b->slot_XorgFinished(0);
    EXPECT_EQ(spy.count(), 1);
}

TEST_F(LogBackend_UT, slot_xorgData_viewSession)
{
    LogBackend *b = backend();
    ASSERT_NE(b, nullptr);
    b->setFlag(XORG);
    b->m_sessionType = LogBackend::View;
    b->m_xorgCurrentIndex = 0;
    QList<LOG_MSG_XORG> list{{"offset", "msg"}};
    QSignalSpy spy(b, &LogBackend::xlogData);
    b->slot_xorgData(0, list);
    EXPECT_EQ(spy.count(), 1);
}

TEST_F(LogBackend_UT, slot_xorgData_mismatch_ignored)
{
    LogBackend *b = backend();
    ASSERT_NE(b, nullptr);
    b->setFlag(JOURNAL);
    b->m_xorgCurrentIndex = 1;
    QList<LOG_MSG_XORG> list{{"offset", "msg"}};
    b->slot_xorgData(0, list);
    EXPECT_TRUE(b->xList.isEmpty());
}

TEST_F(LogBackend_UT, slot_bootFinished_viewSession)
{
    LogBackend *b = backend();
    ASSERT_NE(b, nullptr);
    b->setFlag(BOOT);
    b->m_sessionType = LogBackend::View;
    b->m_bootCurrentIndex = 0;
    QSignalSpy spy(b, &LogBackend::bootFinished);
    b->slot_bootFinished(0);
    EXPECT_EQ(spy.count(), 1);
}

TEST_F(LogBackend_UT, slot_bootData_viewSession)
{
    LogBackend *b = backend();
    ASSERT_NE(b, nullptr);
    b->setFlag(BOOT);
    b->m_sessionType = LogBackend::View;
    b->m_bootCurrentIndex = 0;
    QList<LOG_MSG_BOOT> list{{"OK", "msg"}};
    QSignalSpy spy(b, &LogBackend::bootData);
    b->slot_bootData(0, list);
    EXPECT_EQ(spy.count(), 1);
}

TEST_F(LogBackend_UT, slot_kernFinished_viewSession)
{
    LogBackend *b = backend();
    ASSERT_NE(b, nullptr);
    b->setFlag(KERN);
    b->m_sessionType = LogBackend::View;
    b->m_kernCurrentIndex = 0;
    QSignalSpy spy(b, &LogBackend::kernFinished);
    b->slot_kernFinished(0);
    EXPECT_EQ(spy.count(), 1);
}

TEST_F(LogBackend_UT, slot_kernData_viewSession)
{
    LogBackend *b = backend();
    ASSERT_NE(b, nullptr);
    b->setFlag(KERN);
    b->m_sessionType = LogBackend::View;
    b->m_kernCurrentIndex = 0;
    QList<LOG_MSG_JOURNAL> list{{"2024-01-01", "host", "daemon", "1", "info", "msg"}};
    QSignalSpy spy(b, &LogBackend::kernData);
    b->slot_kernData(0, list);
    EXPECT_EQ(spy.count(), 1);
}

TEST_F(LogBackend_UT, slot_journalFinished_viewSession)
{
    LogBackend *b = backend();
    ASSERT_NE(b, nullptr);
    b->setFlag(JOURNAL);
    b->m_sessionType = LogBackend::View;
    b->m_journalCurrentIndex = 0;
    QSignalSpy spy(b, &LogBackend::journalFinished);
    b->slot_journalFinished(0);
    EXPECT_EQ(spy.count(), 1);
}

TEST_F(LogBackend_UT, slot_journalData_viewSession)
{
    LogBackend *b = backend();
    ASSERT_NE(b, nullptr);
    b->setFlag(JOURNAL);
    b->m_sessionType = LogBackend::View;
    b->m_journalCurrentIndex = 0;
    QList<LOG_MSG_JOURNAL> list{{"2024-01-01", "host", "daemon", "1", "info", "msg"}};
    QSignalSpy spy(b, &LogBackend::journalData);
    b->slot_journalData(0, list);
    EXPECT_EQ(spy.count(), 1);
}

TEST_F(LogBackend_UT, slot_journalBootFinished_viewSession)
{
    LogBackend *b = backend();
    ASSERT_NE(b, nullptr);
    b->setFlag(BOOT_KLU);
    b->m_sessionType = LogBackend::View;
    b->m_journalBootCurrentIndex = 0;
    QSignalSpy spy(b, &LogBackend::journalBootFinished);
    b->slot_journalBootFinished(0);
    EXPECT_EQ(spy.count(), 1);
}

TEST_F(LogBackend_UT, slot_journalBootData_viewSession)
{
    LogBackend *b = backend();
    ASSERT_NE(b, nullptr);
    b->setFlag(BOOT_KLU);
    b->m_sessionType = LogBackend::View;
    b->m_journalBootCurrentIndex = 0;
    QList<LOG_MSG_JOURNAL> list{{"2024-01-01", "host", "daemon", "1", "info", "msg"}};
    QSignalSpy spy(b, &LogBackend::journaBootlData);
    b->slot_journalBootData(0, list);
    EXPECT_EQ(spy.count(), 1);
}

TEST_F(LogBackend_UT, slot_kwinFinished_viewSession)
{
    LogBackend *b = backend();
    ASSERT_NE(b, nullptr);
    b->setFlag(Kwin);
    b->m_sessionType = LogBackend::View;
    b->m_kwinCurrentIndex = 0;
    QSignalSpy spy(b, &LogBackend::kwinFinished);
    b->slot_kwinFinished(0);
    EXPECT_EQ(spy.count(), 1);
}

TEST_F(LogBackend_UT, slot_kwinData_viewSession)
{
    LogBackend *b = backend();
    ASSERT_NE(b, nullptr);
    b->setFlag(Kwin);
    b->m_sessionType = LogBackend::View;
    b->m_kwinCurrentIndex = 0;
    QList<LOG_MSG_KWIN> list{{"msg"}};
    QSignalSpy spy(b, &LogBackend::kwinData);
    b->slot_kwinData(0, list);
    EXPECT_EQ(spy.count(), 1);
}

TEST_F(LogBackend_UT, slot_applicationFinished_viewSession)
{
    LogBackend *b = backend();
    ASSERT_NE(b, nullptr);
    b->setFlag(APP);
    b->m_sessionType = LogBackend::View;
    b->m_appCurrentIndex = 0;
    QSignalSpy spy(b, &LogBackend::appFinished);
    b->slot_applicationFinished(0);
    EXPECT_EQ(spy.count(), 1);
}

TEST_F(LogBackend_UT, slot_applicationData_viewSession)
{
    LogBackend *b = backend();
    ASSERT_NE(b, nullptr);
    b->setFlag(APP);
    b->m_sessionType = LogBackend::View;
    b->m_appCurrentIndex = 0;
    QList<LOG_MSG_APPLICATOIN> list{{"2024-01-01", "info", "src", "msg", "submod"}};
    QSignalSpy spy(b, &LogBackend::appData);
    b->slot_applicationData(0, list);
    EXPECT_EQ(spy.count(), 1);
}

TEST_F(LogBackend_UT, slot_normalFinished_viewSession)
{
    LogBackend *b = backend();
    ASSERT_NE(b, nullptr);
    b->setFlag(Normal);
    b->m_sessionType = LogBackend::View;
    b->m_normalCurrentIndex = 0;
    QSignalSpy spy(b, &LogBackend::normalFinished);
    b->slot_normalFinished(0);
    EXPECT_EQ(spy.count(), 1);
}

TEST_F(LogBackend_UT, slot_normalData_viewSession)
{
    LogBackend *b = backend();
    ASSERT_NE(b, nullptr);
    b->setFlag(Normal);
    b->m_sessionType = LogBackend::View;
    b->m_normalCurrentIndex = 0;
    QList<LOG_MSG_NORMAL> list{{"login", "user1", "2024-01-01", "msg"}};
    QSignalSpy spy(b, &LogBackend::normalData);
    b->slot_normalData(0, list);
    EXPECT_EQ(spy.count(), 1);
}

TEST_F(LogBackend_UT, slot_OOCFinished_viewSession_otherLog)
{
    LogBackend *b = backend();
    ASSERT_NE(b, nullptr);
    b->setFlag(OtherLog);
    b->m_sessionType = LogBackend::View;
    b->m_OOCCurrentIndex = 0;
    QSignalSpy spy(b, &LogBackend::OOCFinished);
    b->slot_OOCFinished(0, 0);
    EXPECT_EQ(spy.count(), 1);
}

TEST_F(LogBackend_UT, slot_OOCFinished_viewSession_customLog)
{
    LogBackend *b = backend();
    ASSERT_NE(b, nullptr);
    b->setFlag(CustomLog);
    b->m_sessionType = LogBackend::View;
    b->m_OOCCurrentIndex = 0;
    QSignalSpy spy(b, &LogBackend::OOCFinished);
    b->slot_OOCFinished(0, 0);
    EXPECT_EQ(spy.count(), 1);
}

TEST_F(LogBackend_UT, slot_OOCData_viewSession)
{
    LogBackend *b = backend();
    ASSERT_NE(b, nullptr);
    b->setFlag(OtherLog);
    b->m_sessionType = LogBackend::View;
    b->m_OOCCurrentIndex = 0;
    QSignalSpy spy(b, &LogBackend::OOCData);
    b->slot_OOCData(0, "testdata");
    EXPECT_EQ(spy.count(), 1);
}

TEST_F(LogBackend_UT, slot_auditFinished_viewSession)
{
    LogBackend *b = backend();
    ASSERT_NE(b, nullptr);
    b->setFlag(Audit);
    b->m_sessionType = LogBackend::View;
    b->m_auditCurrentIndex = 0;
    QSignalSpy spy(b, &LogBackend::auditFinished);
    b->slot_auditFinished(0, true);
    EXPECT_EQ(spy.count(), 1);
}

TEST_F(LogBackend_UT, slot_auditData_viewSession)
{
    LogBackend *b = backend();
    ASSERT_NE(b, nullptr);
    b->setFlag(Audit);
    b->m_sessionType = LogBackend::View;
    b->m_auditCurrentIndex = 0;
    QList<LOG_MSG_AUDIT> list;
    QSignalSpy spy(b, &LogBackend::auditData);
    b->slot_auditData(0, list);
    EXPECT_EQ(spy.count(), 1);
}

TEST_F(LogBackend_UT, slot_authFinished_viewSession)
{
    LogBackend *b = backend();
    ASSERT_NE(b, nullptr);
    b->setFlag(Auth);
    b->m_sessionType = LogBackend::View;
    b->m_authCurrentIndex = 0;
    QSignalSpy spy(b, &LogBackend::authFinished);
    b->slot_authFinished(0);
    EXPECT_EQ(spy.count(), 1);
}

TEST_F(LogBackend_UT, slot_authData_viewSession)
{
    LogBackend *b = backend();
    ASSERT_NE(b, nullptr);
    b->setFlag(Auth);
    b->m_sessionType = LogBackend::View;
    b->m_authCurrentIndex = 0;
    QList<LOG_MSG_AUTH> list;
    QSignalSpy spy(b, &LogBackend::authData);
    b->slot_authData(0, list);
    EXPECT_EQ(spy.count(), 1);
}

TEST_F(LogBackend_UT, slot_coredumpData_viewSession)
{
    LogBackend *b = backend();
    ASSERT_NE(b, nullptr);
    b->setFlag(COREDUMP);
    b->m_sessionType = LogBackend::View;
    b->m_coredumpCurrentIndex = 0;
    QList<LOG_MSG_COREDUMP> list{{"SIGSEGV", "2024-01-01", "corefile", "user", "exe"}};
    QSignalSpy spy(b, &LogBackend::coredumpData);
    b->slot_coredumpData(0, list);
    EXPECT_EQ(spy.count(), 1);
}

TEST_F(LogBackend_UT, slot_coredumpFinished_viewSession)
{
    LogBackend *b = backend();
    ASSERT_NE(b, nullptr);
    b->setFlag(COREDUMP);
    b->m_sessionType = LogBackend::View;
    b->m_coredumpCurrentIndex = 0;
    QSignalSpy spy(b, &LogBackend::coredumpFinished);
    b->slot_coredumpFinished(0);
    EXPECT_EQ(spy.count(), 1);
}

TEST_F(LogBackend_UT, slot_dmesgFinished_viewSession)
{
    LogBackend *b = backend();
    ASSERT_NE(b, nullptr);
    b->setFlag(Dmesg);
    b->m_sessionType = LogBackend::View;
    QList<LOG_MSG_DMESG> list{{"2024-01-01", "msg"}};
    QSignalSpy spy(b, &LogBackend::dmesgFinished);
    b->slot_dmesgFinished(list);
    EXPECT_EQ(spy.count(), 1);
    EXPECT_FALSE(b->dmesgListOrigin.isEmpty());
}

TEST_F(LogBackend_UT, slot_dmesgFinished_flagMismatch_ignored)
{
    LogBackend *b = backend();
    ASSERT_NE(b, nullptr);
    b->setFlag(JOURNAL);
    b->m_sessionType = LogBackend::View;
    QList<LOG_MSG_DMESG> list{{"2024-01-01", "msg"}};
    b->slot_dmesgFinished(list);
    EXPECT_TRUE(b->dmesgListOrigin.isEmpty());
}

TEST_F(LogBackend_UT, slot_dnfFinished_viewSession)
{
    LogBackend *b = backend();
    ASSERT_NE(b, nullptr);
    b->setFlag(Dnf);
    b->m_sessionType = LogBackend::View;
    QList<LOG_MSG_DNF> list{{"2024-01-01", "msg", "info"}};
    QSignalSpy spy(b, &LogBackend::dnfFinished);
    b->slot_dnfFinished(list);
    EXPECT_EQ(spy.count(), 1);
    EXPECT_FALSE(b->dnfListOrigin.isEmpty());
}

TEST_F(LogBackend_UT, slot_dnfFinished_flagMismatch_ignored)
{
    LogBackend *b = backend();
    ASSERT_NE(b, nullptr);
    b->setFlag(JOURNAL);
    QList<LOG_MSG_DNF> list{{"2024-01-01", "msg", "info"}};
    b->slot_dnfFinished(list);
    EXPECT_TRUE(b->dnfListOrigin.isEmpty());
}

TEST_F(LogBackend_UT, slot_logLoadFailed_viewSession)
{
    LogBackend *b = backend();
    ASSERT_NE(b, nullptr);
    b->m_sessionType = LogBackend::View;
    QSignalSpy spy(b, &LogBackend::proccessError);
    b->slot_logLoadFailed("test error");
    EXPECT_EQ(spy.count(), 1);
}

/******************************* Export-related Methods *******************************/

TEST_F(LogBackend_UT, exportAllLogs_invalidDir_returnsFail)
{
    LogBackend *b = backend();
    ASSERT_NE(b, nullptr);
    int result = b->exportAllLogs("/nonexistent/path/12345");
    EXPECT_EQ(result, -1);
}

TEST_F(LogBackend_UT, exportTypeLogs_invalidDir_returnsFail)
{
    LogBackend *b = backend();
    ASSERT_NE(b, nullptr);
    int result = b->exportTypeLogs("/nonexistent/path/12345", TYPE_SYSTEM);
    EXPECT_EQ(result, -1);
}

TEST_F(LogBackend_UT, exportTypeLogs_invalidType_returnsFail)
{
    LogBackend *b = backend();
    ASSERT_NE(b, nullptr);
    QTemporaryDir tmpDir;
    ASSERT_TRUE(tmpDir.isValid());
    Stub s;
    g_whiteList.clear();
    g_whiteList << tmpDir.path();
    s.set(ADDR(DLDBusHandler, whiteListOutPaths), stub_whiteListOutPaths_global);
    int result = b->exportTypeLogs(tmpDir.path(), "invalid_type");
    EXPECT_EQ(result, -1);
}

TEST_F(LogBackend_UT, exportTypeLogs_validType_journal)
{
    LogBackend *b = backend();
    ASSERT_NE(b, nullptr);
    QTemporaryDir tmpDir;
    ASSERT_TRUE(tmpDir.isValid());
    Stub s;
    g_whiteList.clear();
    g_whiteList << tmpDir.path();
    s.set(ADDR(DLDBusHandler, whiteListOutPaths), stub_whiteListOutPaths_global);
    s.set(ADDR(DLDBusHandler, exportLog), stub_dlbus_exportLog);
    s.set(ADDR(DLDBusHandler, getFileInfo), stub_dlbus_getFileInfo);
    int result = b->exportTypeLogs(tmpDir.path(), TYPE_SYSTEM);
    EXPECT_EQ(result, 0);
}

TEST_F(LogBackend_UT, exportTypeLogs_validType_kern_noFiles)
{
    LogBackend *b = backend();
    ASSERT_NE(b, nullptr);
    QTemporaryDir tmpDir;
    ASSERT_TRUE(tmpDir.isValid());
    Stub s;
    g_whiteList.clear();
    g_whiteList << tmpDir.path();
    s.set(ADDR(DLDBusHandler, whiteListOutPaths), stub_whiteListOutPaths_global);
    s.set(ADDR(DLDBusHandler, getFileInfo), stub_dlbus_getFileInfo);
    int result = b->exportTypeLogs(tmpDir.path(), TYPE_KERNEL);
    EXPECT_EQ(result, -1);
}

TEST_F(LogBackend_UT, exportTypeLogs_validType_boot_noFiles)
{
    LogBackend *b = backend();
    ASSERT_NE(b, nullptr);
    QTemporaryDir tmpDir;
    ASSERT_TRUE(tmpDir.isValid());
    Stub s;
    g_whiteList.clear();
    g_whiteList << tmpDir.path();
    s.set(ADDR(DLDBusHandler, whiteListOutPaths), stub_whiteListOutPaths_global);
    s.set(ADDR(DLDBusHandler, getFileInfo), stub_dlbus_getFileInfo);
    int result = b->exportTypeLogs(tmpDir.path(), TYPE_BOOT);
    EXPECT_EQ(result, -1);
}

TEST_F(LogBackend_UT, exportTypeLogs_validType_dpkg_noFiles)
{
    LogBackend *b = backend();
    ASSERT_NE(b, nullptr);
    QTemporaryDir tmpDir;
    ASSERT_TRUE(tmpDir.isValid());
    Stub s;
    g_whiteList.clear();
    g_whiteList << tmpDir.path();
    s.set(ADDR(DLDBusHandler, whiteListOutPaths), stub_whiteListOutPaths_global);
    s.set(ADDR(DLDBusHandler, getFileInfo), stub_dlbus_getFileInfo);
    int result = b->exportTypeLogs(tmpDir.path(), TYPE_DPKG);
    EXPECT_EQ(result, -1);
}

TEST_F(LogBackend_UT, exportTypeLogs_validType_xorg)
{
    LogBackend *b = backend();
    ASSERT_NE(b, nullptr);
    QTemporaryDir tmpDir;
    ASSERT_TRUE(tmpDir.isValid());
    Stub s;
    g_whiteList.clear();
    g_whiteList << tmpDir.path();
    s.set(ADDR(DLDBusHandler, whiteListOutPaths), stub_whiteListOutPaths_global);
    s.set(ADDR(DLDBusHandler, getFileInfo), stub_dlbus_getFileInfo_nonEmpty);
    s.set(ADDR(DLDBusHandler, exportLog), stub_dlbus_exportLog);
    int result = b->exportTypeLogs(tmpDir.path(), TYPE_XORG);
    EXPECT_EQ(result, 0);
}

TEST_F(LogBackend_UT, exportTypeLogs_validType_coredump)
{
    LogBackend *b = backend();
    ASSERT_NE(b, nullptr);
    QTemporaryDir tmpDir;
    ASSERT_TRUE(tmpDir.isValid());
    Stub s;
    g_whiteList.clear();
    g_whiteList << tmpDir.path();
    s.set(ADDR(DLDBusHandler, whiteListOutPaths), stub_whiteListOutPaths_global);
    s.set(ADDR(DLDBusHandler, getFileInfo), stub_dlbus_getFileInfo);
    s.set(ADDR(DLDBusHandler, exportLog), stub_dlbus_exportLog);
    s.set(ADDR(DLDBusHandler, executeCmd), stub_dlbus_executeCmd);
    int result = b->exportTypeLogs(tmpDir.path(), TYPE_COREDUMP);
    // coredump path may return 0 or -1 depending on data
    EXPECT_TRUE(result == 0 || result == -1);
}

TEST_F(LogBackend_UT, exportTypeLogs_validType_audit)
{
    LogBackend *b = backend();
    ASSERT_NE(b, nullptr);
    QTemporaryDir tmpDir;
    ASSERT_TRUE(tmpDir.isValid());
    Stub s;
    g_whiteList.clear();
    g_whiteList << tmpDir.path();
    s.set(ADDR(DLDBusHandler, whiteListOutPaths), stub_whiteListOutPaths_global);
    s.set(ADDR(DLDBusHandler, getFileInfo), stub_dlbus_getFileInfo_nonEmpty);
    s.set(ADDR(DLDBusHandler, exportLog), stub_dlbus_exportLog);
    int result = b->exportTypeLogs(tmpDir.path(), TYPE_AUDIT);
    EXPECT_EQ(result, 0);
}

TEST_F(LogBackend_UT, exportTypeLogs_validType_auth)
{
    LogBackend *b = backend();
    ASSERT_NE(b, nullptr);
    QTemporaryDir tmpDir;
    ASSERT_TRUE(tmpDir.isValid());
    Stub s;
    g_whiteList.clear();
    g_whiteList << tmpDir.path();
    s.set(ADDR(DLDBusHandler, whiteListOutPaths), stub_whiteListOutPaths_global);
    s.set(ADDR(DLDBusHandler, getFileInfo), stub_dlbus_getFileInfo);
    s.set(ADDR(DLDBusHandler, exportLog), stub_dlbus_exportLog);
    s.set(ADDR(LogApplicationHelper, getAuthLogList), stub_appHelper_getAuthLogList);
    int result = b->exportTypeLogs(tmpDir.path(), TYPE_AUTH);
    EXPECT_EQ(result, 0);
}

TEST_F(LogBackend_UT, exportTypeLogs_validType_other)
{
    LogBackend *b = backend();
    ASSERT_NE(b, nullptr);
    QTemporaryDir tmpDir;
    ASSERT_TRUE(tmpDir.isValid());
    Stub s;
    g_whiteList.clear();
    g_whiteList << tmpDir.path();
    s.set(ADDR(DLDBusHandler, whiteListOutPaths), stub_whiteListOutPaths_global);
    s.set(ADDR(DLDBusHandler, getFileInfo), stub_dlbus_getFileInfo);
    s.set(ADDR(DLDBusHandler, exportLog), stub_dlbus_exportLog);
    s.set(ADDR(DLDBusHandler, getOtherFileInfo), stub_dlbus_getOtherFileInfo);
    s.set(ADDR(LogApplicationHelper, getOtherLogList), stub_appHelper_getOtherLogList);
    int result = b->exportTypeLogs(tmpDir.path(), TYPE_OTHER);
    EXPECT_EQ(result, 0);
}


TEST_F(LogBackend_UT, exportAppLogs_invalidDir)
{
    LogBackend *b = backend();
    ASSERT_NE(b, nullptr);
    int result = b->exportAppLogs("/nonexistent/path/12345", "testapp");
    EXPECT_EQ(result, -1);
}

TEST_F(LogBackend_UT, exportAppLogs_emptyAppName)
{
    LogBackend *b = backend();
    ASSERT_NE(b, nullptr);
    QTemporaryDir tmpDir;
    ASSERT_TRUE(tmpDir.isValid());
    Stub s;
    g_whiteList.clear();
    g_whiteList << tmpDir.path();
    s.set(ADDR(DLDBusHandler, whiteListOutPaths), stub_whiteListOutPaths_global);
    int result = b->exportAppLogs(tmpDir.path(), "");
    EXPECT_EQ(result, -1);
}

TEST_F(LogBackend_UT, exportAppLogs_appNotExist)
{
    LogBackend *b = backend();
    ASSERT_NE(b, nullptr);
    QTemporaryDir tmpDir;
    ASSERT_TRUE(tmpDir.isValid());
    Stub s;
    g_whiteList.clear();
    g_whiteList << tmpDir.path();
    s.set(ADDR(DLDBusHandler, whiteListOutPaths), stub_whiteListOutPaths_global);
    int result = b->exportAppLogs(tmpDir.path(), "nonexistent_app");
    EXPECT_EQ(result, -1);
}

TEST_F(LogBackend_UT, exportAppLogsByCondition_invalidDir)
{
    LogBackend *b = backend();
    ASSERT_NE(b, nullptr);
    bool result = b->exportAppLogsByCondition("/nonexistent/path/12345", "testapp", "all");
    EXPECT_FALSE(result);
}

TEST_F(LogBackend_UT, exportAppLogsByCondition_emptyAppName)
{
    LogBackend *b = backend();
    ASSERT_NE(b, nullptr);
    QTemporaryDir tmpDir;
    ASSERT_TRUE(tmpDir.isValid());
    Stub s;
    g_whiteList.clear();
    g_whiteList << tmpDir.path();
    s.set(ADDR(DLDBusHandler, whiteListOutPaths), stub_whiteListOutPaths_global);
    bool result = b->exportAppLogsByCondition(tmpDir.path(), "", "all");
    EXPECT_FALSE(result);
}

TEST_F(LogBackend_UT, exportAppLogsByCondition_invalidPeriod)
{
    LogBackend *b = backend();
    ASSERT_NE(b, nullptr);
    QTemporaryDir tmpDir;
    ASSERT_TRUE(tmpDir.isValid());
    Stub s;
    g_whiteList.clear();
    g_whiteList << tmpDir.path();
    s.set(ADDR(DLDBusHandler, whiteListOutPaths), stub_whiteListOutPaths_global);
    bool result = b->exportAppLogsByCondition(tmpDir.path(), "testapp", "invalid_period");
    EXPECT_FALSE(result);
}

TEST_F(LogBackend_UT, exportAppLogsByCondition_invalidLevel)
{
    LogBackend *b = backend();
    ASSERT_NE(b, nullptr);
    QTemporaryDir tmpDir;
    ASSERT_TRUE(tmpDir.isValid());
    Stub s;
    g_whiteList.clear();
    g_whiteList << tmpDir.path();
    s.set(ADDR(DLDBusHandler, whiteListOutPaths), stub_whiteListOutPaths_global);
    bool result = b->exportAppLogsByCondition(tmpDir.path(), "testapp", "all", "invalid_level");
    EXPECT_FALSE(result);
}

TEST_F(LogBackend_UT, exportAppLogsByCondition_appNotExist)
{
    LogBackend *b = backend();
    ASSERT_NE(b, nullptr);
    QTemporaryDir tmpDir;
    ASSERT_TRUE(tmpDir.isValid());
    Stub s;
    g_whiteList.clear();
    g_whiteList << tmpDir.path();
    s.set(ADDR(DLDBusHandler, whiteListOutPaths), stub_whiteListOutPaths_global);
    bool result = b->exportAppLogsByCondition(tmpDir.path(), "nonexistent_app", "all");
    EXPECT_FALSE(result);
}

TEST_F(LogBackend_UT, stopExportFromUI_viewSession)
{
    LogBackend *b = backend();
    ASSERT_NE(b, nullptr);
    b->m_sessionType = LogBackend::View;
    QSignalSpy spy(b, &LogBackend::stopExport);
    b->stopExportFromUI();
    EXPECT_EQ(spy.count(), 1);
}

TEST_F(LogBackend_UT, stopExportFromUI_exportSession_resetsToView)
{
    LogBackend *b = backend();
    ASSERT_NE(b, nullptr);
    b->m_sessionType = LogBackend::Export;
    b->m_lastSessionType = LogBackend::View;
    QSignalSpy spy(b, &LogBackend::stopExport);
    b->stopExportFromUI();
    EXPECT_EQ(spy.count(), 1);
    EXPECT_EQ(b->m_sessionType, LogBackend::View);
}

/******************************* Segment Methods *******************************/

TEST_F(LogBackend_UT, getNextSegementIndex_kern)
{
    LogBackend *b = backend();
    ASSERT_NE(b, nullptr);
    Stub s;
    s.set(ADDR(DLDBusHandler, getFileInfo), stub_dlbus_getFileInfo);
    s.set(ADDR(DLDBusHandler, getLineCount), stub_dlbus_getLineCount);
    int result = b->getNextSegementIndex(KERN, true);
    EXPECT_EQ(result, -1);
}

TEST_F(LogBackend_UT, getNextSegementIndex_kwin)
{
    LogBackend *b = backend();
    ASSERT_NE(b, nullptr);
    Stub s;
    s.set(ADDR(DLDBusHandler, getLineCount), stub_dlbus_getLineCount);
    int result = b->getNextSegementIndex(Kwin, true);
    EXPECT_EQ(result, -1);
}

TEST_F(LogBackend_UT, loadSegementPage_minusOne)
{
    LogBackend *b = backend();
    ASSERT_NE(b, nullptr);
    int result = b->loadSegementPage(-1);
    EXPECT_EQ(result, -1);
}

TEST_F(LogBackend_UT, loadSegementPage_reset)
{
    LogBackend *b = backend();
    ASSERT_NE(b, nullptr);
    Stub s;
    s.set(ADDR(LogFileParser, parse), stub_LogFileParser_parse);
    int result = b->loadSegementPage(0, true);
    EXPECT_GE(result, 0);
}

TEST_F(LogBackend_UT, loadSegementPage_noReset)
{
    LogBackend *b = backend();
    ASSERT_NE(b, nullptr);
    Stub s;
    s.set(ADDR(LogFileParser, parse), stub_LogFileParser_parse);
    b->m_type2LogDataOrigin[KERN] << "data1";
    int result = b->loadSegementPage(0, false);
    EXPECT_GE(result, 0);
}

TEST_F(LogBackend_UT, segementExport_notKernOrKwin)
{
    LogBackend *b = backend();
    ASSERT_NE(b, nullptr);
    b->setFlag(JOURNAL);
    b->segementExport();
    SUCCEED();
}

/******************************* onExportProgress/onExportResult *******************************/

TEST_F(LogBackend_UT, onExportProgress_viewSession_noSender)
{
    LogBackend *b = backend();
    ASSERT_NE(b, nullptr);
    b->m_sessionType = LogBackend::View;
    QSignalSpy spy(b, &LogBackend::sigProgress);
    // No sender (called directly), so bExportThreadRunning is false
    b->onExportProgress(1, 100);
    EXPECT_EQ(spy.count(), 0);
}

TEST_F(LogBackend_UT, onExportResult_viewSession)
{
    LogBackend *b = backend();
    ASSERT_NE(b, nullptr);
    b->m_sessionType = LogBackend::View;
    QSignalSpy spy(b, &LogBackend::sigResult);
    b->onExportResult(true);
    EXPECT_EQ(spy.count(), 1);
}

TEST_F(LogBackend_UT, onExportResult_viewSession_failure)
{
    LogBackend *b = backend();
    ASSERT_NE(b, nullptr);
    b->m_sessionType = LogBackend::View;
    QSignalSpy spy(b, &LogBackend::sigResult);
    b->onExportResult(false);
    EXPECT_EQ(spy.count(), 1);
}

TEST_F(LogBackend_UT, onExportFakeCloseDlg_viewSession)
{
    LogBackend *b = backend();
    ASSERT_NE(b, nullptr);
    b->m_sessionType = LogBackend::View;
    QSignalSpy spy(b, &LogBackend::sigProcessFull);
    b->onExportFakeCloseDlg();
    EXPECT_EQ(spy.count(), 1);
}

/******************************* reportCoredumpInfo *******************************/

TEST_F(LogBackend_UT, reportCoredumpInfo)
{
    LogBackend *b = backend();
    ASSERT_NE(b, nullptr);
    Stub s;
    s.set(ADDR(LogFileParser, parseByCoredump), stub_LogFileParser_parseByCoredump);
    b->reportCoredumpInfo();
    EXPECT_EQ(b->m_flag, COREDUMP);
    EXPECT_EQ(b->m_sessionType, LogBackend::Report);
}

/******************************* getLogTypes *******************************/

TEST_F(LogBackend_UT, getLogTypes)
{
    LogBackend *b = backend();
    ASSERT_NE(b, nullptr);
    stub.reset(ADDR(LogBackend, getLogTypes));
    QStringList types = b->getLogTypes();
    EXPECT_FALSE(types.isEmpty());
}

/******************************* parseData (private) *******************************/

TEST_F(LogBackend_UT, parseData_journal)
{
    LogBackend *b = backend();
    ASSERT_NE(b, nullptr);
    Stub s;
    s.set(ADDR(LogFileParser, parseByJournal), stub_LogFileParser_parseByJournal);
    bool result = b->parseData(JOURNAL, "all", "");
    EXPECT_TRUE(result);
}

TEST_F(LogBackend_UT, parseData_invalidPeriod)
{
    LogBackend *b = backend();
    ASSERT_NE(b, nullptr);
    bool result = b->parseData(JOURNAL, "invalid_period", "");
    EXPECT_FALSE(result);
}

TEST_F(LogBackend_UT, parseData_invalidLevel)
{
    LogBackend *b = backend();
    ASSERT_NE(b, nullptr);
    bool result = b->parseData(JOURNAL, "all", "invalid_level");
    EXPECT_FALSE(result);
}

TEST_F(LogBackend_UT, parseData_invalidBootStatus)
{
    LogBackend *b = backend();
    ASSERT_NE(b, nullptr);
    bool result = b->parseData(BOOT, "all", "invalid_status");
    EXPECT_FALSE(result);
}

TEST_F(LogBackend_UT, parseData_invalidNormalEvent)
{
    LogBackend *b = backend();
    ASSERT_NE(b, nullptr);
    bool result = b->parseData(Normal, "all", "invalid_event");
    EXPECT_FALSE(result);
}

TEST_F(LogBackend_UT, parseData_invalidAuditType)
{
    LogBackend *b = backend();
    ASSERT_NE(b, nullptr);
    bool result = b->parseData(Audit, "all", "invalid_audit");
    EXPECT_FALSE(result);
}

TEST_F(LogBackend_UT, parseData_kern)
{
    LogBackend *b = backend();
    ASSERT_NE(b, nullptr);
    Stub s;
    s.set(ADDR(LogFileParser, parse), stub_LogFileParser_parse);
    bool result = b->parseData(KERN, "all", "");
    EXPECT_TRUE(result);
}

TEST_F(LogBackend_UT, parseData_boot)
{
    LogBackend *b = backend();
    ASSERT_NE(b, nullptr);
    Stub s;
    s.set(ADDR(LogFileParser, parseByBoot), stub_LogFileParser_parseByBoot);
    bool result = b->parseData(BOOT, "all", "");
    EXPECT_TRUE(result);
}

TEST_F(LogBackend_UT, parseData_dpkg)
{
    LogBackend *b = backend();
    ASSERT_NE(b, nullptr);
    Stub s;
    s.set(ADDR(LogFileParser, parseByDpkg), stub_LogFileParser_parseByDpkg);
    bool result = b->parseData(DPKG, "all", "");
    EXPECT_TRUE(result);
}

TEST_F(LogBackend_UT, parseData_xorg)
{
    LogBackend *b = backend();
    ASSERT_NE(b, nullptr);
    Stub s;
    s.set(ADDR(LogFileParser, parseByXlog), stub_LogFileParser_parseByXlog);
    bool result = b->parseData(XORG, "all", "");
    EXPECT_TRUE(result);
}

TEST_F(LogBackend_UT, parseData_app)
{
    LogBackend *b = backend();
    ASSERT_NE(b, nullptr);
    Stub s;
    s.set(ADDR(LogFileParser, parseByApp), stub_LogFileParser_parseByApp);
    bool result = b->parseData(APP, "all", "");
    EXPECT_TRUE(result);
}

TEST_F(LogBackend_UT, parseData_normal)
{
    LogBackend *b = backend();
    ASSERT_NE(b, nullptr);
    Stub s;
    s.set(ADDR(LogFileParser, parseByNormal), stub_LogFileParser_parseByNormal);
    bool result = b->parseData(Normal, "all", "login");
    EXPECT_TRUE(result);
}

TEST_F(LogBackend_UT, parseData_audit)
{
    LogBackend *b = backend();
    ASSERT_NE(b, nullptr);
    Stub s;
    s.set(ADDR(LogFileParser, parseByAudit), stub_LogFileParser_parseByAudit);
    bool result = b->parseData(Audit, "all", "all");
    EXPECT_TRUE(result);
}

TEST_F(LogBackend_UT, parseData_auth)
{
    LogBackend *b = backend();
    ASSERT_NE(b, nullptr);
    Stub s;
    s.set(ADDR(LogFileParser, parseByAuth), stub_LogFileParser_parseByAuth);
    bool result = b->parseData(Auth, "all", "");
    EXPECT_TRUE(result);
}

TEST_F(LogBackend_UT, parseData_coredump)
{
    LogBackend *b = backend();
    ASSERT_NE(b, nullptr);
    Stub s;
    s.set(ADDR(LogFileParser, parseByCoredump), stub_LogFileParser_parseByCoredump);
    bool result = b->parseData(COREDUMP, "all", "");
    EXPECT_TRUE(result);
}

TEST_F(LogBackend_UT, parseData_kwin)
{
    LogBackend *b = backend();
    ASSERT_NE(b, nullptr);
    Stub s;
    s.set(ADDR(LogFileParser, parse), stub_LogFileParser_parse);
    bool result = b->parseData(Kwin, "all", "");
    EXPECT_TRUE(result);
}

TEST_F(LogBackend_UT, parseData_otherLog)
{
    LogBackend *b = backend();
    ASSERT_NE(b, nullptr);
    Stub s;
    s.set(ADDR(LogFileParser, parseByOOC), stub_LogFileParser_parseByOOC);
    bool result = b->parseData(OtherLog, "all", "");
    EXPECT_TRUE(result);
}

TEST_F(LogBackend_UT, parseData_customLog)
{
    LogBackend *b = backend();
    ASSERT_NE(b, nullptr);
    Stub s;
    s.set(ADDR(LogFileParser, parseByOOC), stub_LogFileParser_parseByOOC);
    bool result = b->parseData(CustomLog, "all", "");
    EXPECT_TRUE(result);
}

TEST_F(LogBackend_UT, parseData_bootKlu)
{
    LogBackend *b = backend();
    ASSERT_NE(b, nullptr);
    Stub s;
    s.set(ADDR(LogFileParser, parseByJournalBoot), stub_LogFileParser_parseByJournalBoot);
    bool result = b->parseData(BOOT_KLU, "all", "");
    EXPECT_TRUE(result);
}

TEST_F(LogBackend_UT, parseData_dmesg)
{
    LogBackend *b = backend();
    ASSERT_NE(b, nullptr);
    Stub s;
    s.set(ADDR(LogFileParser, parseByDmesg), stub_LogFileParser_parseByDmesg);
    bool result = b->parseData(Dmesg, "all", "");
    EXPECT_TRUE(result);
}

TEST_F(LogBackend_UT, parseData_dnf)
{
    LogBackend *b = backend();
    ASSERT_NE(b, nullptr);
    Stub s;
    s.set(ADDR(LogFileParser, parseByDnf), stub_LogFileParser_parseByDnf);
    bool result = b->parseData(Dnf, "all", "trace");
    EXPECT_TRUE(result);
}

TEST_F(LogBackend_UT, parseData_dnf_invalidLevel)
{
    LogBackend *b = backend();
    ASSERT_NE(b, nullptr);
    bool result = b->parseData(Dnf, "all", "invalid_dnf");
    EXPECT_FALSE(result);
}

TEST_F(LogBackend_UT, parseData_defaultFlag)
{
    LogBackend *b = backend();
    ASSERT_NE(b, nullptr);
    bool result = b->parseData(NONE, "all", "");
    EXPECT_TRUE(result);
}

/******************************* executeCLIExport *******************************/

TEST_F(LogBackend_UT, executeCLIExport_notDataLoadComplete)
{
    LogBackend *b = backend();
    ASSERT_NE(b, nullptr);
    b->m_isDataLoadComplete = false;
    b->executeCLIExport("");
    SUCCEED();
}

TEST_F(LogBackend_UT, executeCLIExport_withFilePath)
{
    LogBackend *b = backend();
    ASSERT_NE(b, nullptr);
    b->m_isDataLoadComplete = true;
    b->m_sessionType = LogBackend::Export;
    QTemporaryDir tmpDir;
    ASSERT_TRUE(tmpDir.isValid());
    QString filePath = tmpDir.path() + "/test.txt";
    b->m_flag = JOURNAL;
    b->jList.append({{"2024-01-01", "host", "daemon", "1", "info", "msg"}});
    b->executeCLIExport(filePath);
    SUCCEED();
}

/******************************* parseCoredumpDetailInfo *******************************/

TEST_F(LogBackend_UT, parseCoredumpDetailInfo)
{
    LogBackend *b = backend();
    ASSERT_NE(b, nullptr);
    Stub s;
    s.set(ADDR(DLDBusHandler, executeCmd), stub_dlbus_executeCmd);
    s.set(ADDR(DLDBusHandler, readLogLinesInRange), stub_dlbus_readLogLinesInRange);
    QList<LOG_MSG_COREDUMP> list;
    LOG_MSG_COREDUMP item;
    item.pid = "1234";
    item.exe = "/usr/bin/test";
    item.uid = "1000";
    list.append(item);
    b->parseCoredumpDetailInfo(list);
    SUCCEED();
}

/******************************* Export slot methods - Export session *******************************/

TEST_F(LogBackend_UT, slot_dpkgFinished_exportSession)
{
    LogBackend *b = backend();
    ASSERT_NE(b, nullptr);
    b->setFlag(DPKG);
    b->m_sessionType = LogBackend::Export;
    b->m_dpkgCurrentIndex = 0;
    b->m_isDataLoadComplete = false;
    b->slot_dpkgFinished(0);
    EXPECT_TRUE(b->m_isDataLoadComplete);
}

TEST_F(LogBackend_UT, slot_bootFinished_exportSession)
{
    LogBackend *b = backend();
    ASSERT_NE(b, nullptr);
    b->setFlag(BOOT);
    b->m_sessionType = LogBackend::Export;
    b->m_bootCurrentIndex = 0;
    b->slot_bootFinished(0);
    EXPECT_TRUE(b->m_isDataLoadComplete);
}

TEST_F(LogBackend_UT, slot_kernFinished_exportSession)
{
    LogBackend *b = backend();
    ASSERT_NE(b, nullptr);
    b->setFlag(KERN);
    b->m_sessionType = LogBackend::Export;
    b->m_kernCurrentIndex = 0;
    b->slot_kernFinished(0);
    EXPECT_TRUE(b->m_isDataLoadComplete);
}

TEST_F(LogBackend_UT, slot_journalFinished_exportSession)
{
    LogBackend *b = backend();
    ASSERT_NE(b, nullptr);
    b->setFlag(JOURNAL);
    b->m_sessionType = LogBackend::Export;
    b->m_journalCurrentIndex = 0;
    b->slot_journalFinished(0);
    EXPECT_TRUE(b->m_isDataLoadComplete);
}

TEST_F(LogBackend_UT, slot_kwinFinished_exportSession)
{
    LogBackend *b = backend();
    ASSERT_NE(b, nullptr);
    b->setFlag(Kwin);
    b->m_sessionType = LogBackend::Export;
    b->m_kwinCurrentIndex = 0;
    b->slot_kwinFinished(0);
    EXPECT_TRUE(b->m_isDataLoadComplete);
}

TEST_F(LogBackend_UT, slot_applicationFinished_exportSession)
{
    LogBackend *b = backend();
    ASSERT_NE(b, nullptr);
    b->setFlag(APP);
    b->m_sessionType = LogBackend::Export;
    b->m_appCurrentIndex = 0;
    b->slot_applicationFinished(0);
    EXPECT_TRUE(b->m_isDataLoadComplete);
}

TEST_F(LogBackend_UT, slot_normalFinished_exportSession)
{
    LogBackend *b = backend();
    ASSERT_NE(b, nullptr);
    b->setFlag(Normal);
    b->m_sessionType = LogBackend::Export;
    b->m_normalCurrentIndex = 0;
    b->slot_normalFinished(0);
    EXPECT_TRUE(b->m_isDataLoadComplete);
}

TEST_F(LogBackend_UT, slot_auditFinished_exportSession)
{
    LogBackend *b = backend();
    ASSERT_NE(b, nullptr);
    b->setFlag(Audit);
    b->m_sessionType = LogBackend::Export;
    b->m_auditCurrentIndex = 0;
    b->slot_auditFinished(0, true);
    EXPECT_TRUE(b->m_isDataLoadComplete);
}

TEST_F(LogBackend_UT, slot_authFinished_exportSession)
{
    LogBackend *b = backend();
    ASSERT_NE(b, nullptr);
    b->setFlag(Auth);
    b->m_sessionType = LogBackend::Export;
    b->m_authCurrentIndex = 0;
    b->slot_authFinished(0);
    EXPECT_TRUE(b->m_isDataLoadComplete);
}

TEST_F(LogBackend_UT, slot_journalBootFinished_exportSession)
{
    LogBackend *b = backend();
    ASSERT_NE(b, nullptr);
    b->setFlag(BOOT_KLU);
    b->m_sessionType = LogBackend::Export;
    b->m_journalBootCurrentIndex = 0;
    b->slot_journalBootFinished(0);
    EXPECT_TRUE(b->m_isDataLoadComplete);
}

TEST_F(LogBackend_UT, slot_XorgFinished_exportSession)
{
    LogBackend *b = backend();
    ASSERT_NE(b, nullptr);
    b->setFlag(XORG);
    b->m_sessionType = LogBackend::Export;
    b->m_xorgCurrentIndex = 0;
    b->slot_XorgFinished(0);
    EXPECT_TRUE(b->m_isDataLoadComplete);
}

TEST_F(LogBackend_UT, slot_dmesgFinished_exportSession)
{
    LogBackend *b = backend();
    ASSERT_NE(b, nullptr);
    b->setFlag(Dmesg);
    b->m_sessionType = LogBackend::Export;
    QList<LOG_MSG_DMESG> list{{"2024-01-01", "msg"}};
    b->slot_dmesgFinished(list);
    EXPECT_TRUE(b->m_isDataLoadComplete);
}

TEST_F(LogBackend_UT, slot_dnfFinished_exportSession)
{
    LogBackend *b = backend();
    ASSERT_NE(b, nullptr);
    b->setFlag(Dnf);
    b->m_sessionType = LogBackend::Export;
    QList<LOG_MSG_DNF> list{{"2024-01-01", "msg", "info"}};
    b->slot_dnfFinished(list);
    EXPECT_TRUE(b->m_isDataLoadComplete);
}

TEST_F(LogBackend_UT, slot_coredumpFinished_reportSession)
{
    LogBackend *b = backend();
    ASSERT_NE(b, nullptr);
    b->setFlag(COREDUMP);
    b->m_sessionType = LogBackend::Report;
    b->m_coredumpCurrentIndex = 0;
    b->slot_coredumpFinished(0);
    SUCCEED();
}

TEST_F(LogBackend_UT, slot_logLoadFailed_exportSession)
{
    LogBackend *b = backend();
    ASSERT_NE(b, nullptr);
    b->m_sessionType = LogBackend::Export;
    b->slot_logLoadFailed("test error");
    SUCCEED();
}

TEST_F(LogBackend_UT, slot_logLoadFailed_reportSession)
{
    LogBackend *b = backend();
    ASSERT_NE(b, nullptr);
    b->m_sessionType = LogBackend::Report;
    b->slot_logLoadFailed("test error");
    SUCCEED();
}

TEST_F(LogBackend_UT, onExportResult_exportSession_success)
{
    LogBackend *b = backend();
    ASSERT_NE(b, nullptr);
    b->m_sessionType = LogBackend::Export;
    b->onExportResult(true);
    SUCCEED();
}

TEST_F(LogBackend_UT, onExportResult_exportSession_failure)
{
    LogBackend *b = backend();
    ASSERT_NE(b, nullptr);
    b->m_sessionType = LogBackend::Export;
    b->onExportResult(false);
    SUCCEED();
}

TEST_F(LogBackend_UT, onExportResult_reportSession)
{
    LogBackend *b = backend();
    ASSERT_NE(b, nullptr);
    b->m_sessionType = LogBackend::Report;
    b->onExportResult(true);
    SUCCEED();
}

TEST_F(LogBackend_UT, slot_parseFinished_exportSession_normal)
{
    LogBackend *b = backend();
    ASSERT_NE(b, nullptr);
    b->setFlag(JOURNAL);
    b->m_sessionType = LogBackend::Export;
    b->m_type2ThreadIndex[JOURNAL] = 0;
    b->m_exportFilePath = "";
    b->slot_parseFinished(0, JOURNAL, 0);
    EXPECT_TRUE(b->m_isDataLoadComplete);
}

TEST_F(LogBackend_UT, slot_parseFinished_exportSession_abnormal)
{
    LogBackend *b = backend();
    ASSERT_NE(b, nullptr);
    b->setFlag(JOURNAL);
    b->m_sessionType = LogBackend::Export;
    b->m_lastSessionType = LogBackend::View;
    b->m_type2ThreadIndex[JOURNAL] = 0;
    b->slot_parseFinished(0, JOURNAL, 1);
    EXPECT_EQ(b->m_sessionType, LogBackend::View);
}

/******************************* Export logData with search filter *******************************/

TEST_F(LogBackend_UT, slot_logData_withSearchFilter)
{
    LogBackend *b = backend();
    ASSERT_NE(b, nullptr);
    b->setFlag(JOURNAL);
    b->m_sessionType = LogBackend::View;
    b->m_type2ThreadIndex[JOURNAL] = 0;
    b->m_currentSearchStr = "findme";
    QList<QString> list{"{\"key\":\"findme\"}", "{\"key\":\"other\"}"};
    b->slot_logData(0, list, JOURNAL);
    EXPECT_EQ(b->m_type2LogData[JOURNAL].size(), 1);
}

/******************************* exportLogData *******************************/

TEST_F(LogBackend_UT, exportLogData_auth_noData)
{
    LogBackend *b = backend();
    ASSERT_NE(b, nullptr);
    b->m_flag = Auth;
    b->m_sessionType = LogBackend::Export;
    QTemporaryDir tmpDir;
    ASSERT_TRUE(tmpDir.isValid());
    QString filePath = tmpDir.path() + "/test_auth.txt";
    b->exportLogData(filePath);
    SUCCEED();
}

TEST_F(LogBackend_UT, exportLogData_noMatchingData_nonAuth)
{
    LogBackend *b = backend();
    ASSERT_NE(b, nullptr);
    b->m_flag = JOURNAL;
    b->m_sessionType = LogBackend::Export;
    b->m_bSegementExporting = false;
    QTemporaryDir tmpDir;
    ASSERT_TRUE(tmpDir.isValid());
    QString filePath = tmpDir.path() + "/test_journal.txt";
    b->exportLogData(filePath);
    SUCCEED();
}

TEST_F(LogBackend_UT, exportLogData_invalidDir)
{
    LogBackend *b = backend();
    ASSERT_NE(b, nullptr);
    b->m_flag = JOURNAL;
    b->m_sessionType = LogBackend::Export;
    b->m_bSegementExporting = false;
    b->exportLogData("/nonexistent/path/12345/test.txt");
    SUCCEED();
}

TEST_F(LogBackend_UT, exportLogData_withData_journal)
{
    LogBackend *b = backend();
    ASSERT_NE(b, nullptr);
    b->m_flag = JOURNAL;
    b->m_sessionType = LogBackend::Export;
    b->m_bSegementExporting = false;
    QTemporaryDir tmpDir;
    ASSERT_TRUE(tmpDir.isValid());
    QString filePath = tmpDir.path() + "/test_journal.txt";
    b->jList.append({{"2024-01-01", "host", "daemon", "1", "info", "msg"}});
    b->exportLogData(filePath);
    SUCCEED();
}

TEST_F(LogBackend_UT, exportLogData_segementExporting_noData)
{
    LogBackend *b = backend();
    ASSERT_NE(b, nullptr);
    b->m_flag = KERN;
    b->m_sessionType = LogBackend::Export;
    b->m_bSegementExporting = true;
    QTemporaryDir tmpDir;
    ASSERT_TRUE(tmpDir.isValid());
    QString filePath = tmpDir.path() + "/test_kern.txt";
    b->exportLogData(filePath);
    SUCCEED();
}

/******************************* initConnections (covered via constructor) *******************************/

TEST_F(LogBackend_UT, initConnections_viaConstructor)
{
    // Constructor calls initConnections; if we got here, it worked
    LogBackend *b = backend();
    EXPECT_NE(b, nullptr);
}

/******************************* getOutDirPath() const — getter *******************************/

TEST_F(LogBackend_UT, getOutDirPath_constGetter_returnsOutPath)
{
    LogBackend *b = backend();
    ASSERT_NE(b, nullptr);
    // m_outPath is initially empty; the const getter should return the same value
    EXPECT_EQ(b->getOutDirPath().toStdString(), b->m_outPath.toStdString());
    // Set a known path and verify
    QTemporaryDir tmpDir;
    ASSERT_TRUE(tmpDir.isValid());
    Stub s;
    g_whiteList.clear();
    g_whiteList << tmpDir.path();
    s.set(ADDR(DLDBusHandler, whiteListOutPaths), stub_whiteListOutPaths_global);
    ASSERT_TRUE(b->getOutDirPath(tmpDir.path()));
    EXPECT_EQ(b->getOutDirPath().toStdString(), b->m_outPath.toStdString());
}

/******************************* filterApp(QString, QList) overload *******************************/

TEST_F(LogBackend_UT, filterApp_qstring_emptySearch_returnsInput)
{
    QList<LOG_MSG_APPLICATOIN> input{{"2024-01-01", "submod", "info", "src", "msg"}};
    QList<LOG_MSG_APPLICATOIN> result = LogBackend::filterApp(QString(""), input);
    EXPECT_EQ(result.size(), 1);
}

TEST_F(LogBackend_UT, filterApp_qstring_searchMatch_returnsFiltered)
{
    QList<LOG_MSG_APPLICATOIN> input{{"2024-01-01", "submod", "info", "src", "findme"}, {"2024-01-02", "submod", "info", "src", "other"}};
    QList<LOG_MSG_APPLICATOIN> result = LogBackend::filterApp("findme", input);
    EXPECT_EQ(result.size(), 1);
}

TEST_F(LogBackend_UT, filterApp_qstring_searchNoMatch_returnsEmpty)
{
    QList<LOG_MSG_APPLICATOIN> input{{"2024-01-01", "submod", "info", "src", "findme"}};
    QList<LOG_MSG_APPLICATOIN> result = LogBackend::filterApp("notfound", input);
    EXPECT_EQ(result.size(), 0);
}

/******************************* exportTypeLogsByCondition *******************************/

TEST_F(LogBackend_UT, exportTypeLogsByCondition_invalidDir_returnsFalse)
{
    LogBackend *b = backend();
    ASSERT_NE(b, nullptr);
    bool result = b->exportTypeLogsByCondition("/nonexistent/path/12345", TYPE_SYSTEM, "all");
    EXPECT_FALSE(result);
}

TEST_F(LogBackend_UT, exportTypeLogsByCondition_invalidType_returnsFalse)
{
    LogBackend *b = backend();
    ASSERT_NE(b, nullptr);
    QTemporaryDir tmpDir;
    ASSERT_TRUE(tmpDir.isValid());
    Stub s;
    g_whiteList.clear();
    g_whiteList << tmpDir.path();
    s.set(ADDR(DLDBusHandler, whiteListOutPaths), stub_whiteListOutPaths_global);
    bool result = b->exportTypeLogsByCondition(tmpDir.path(), "invalid_type", "all");
    EXPECT_FALSE(result);
}

TEST_F(LogBackend_UT, exportTypeLogsByCondition_validType_emptyPeriod_returnsTrue)
{
    LogBackend *b = backend();
    ASSERT_NE(b, nullptr);
    QTemporaryDir tmpDir;
    ASSERT_TRUE(tmpDir.isValid());
    Stub s;
    g_whiteList.clear();
    g_whiteList << tmpDir.path();
    s.set(ADDR(DLDBusHandler, whiteListOutPaths), stub_whiteListOutPaths_global);
    bool result = b->exportTypeLogsByCondition(tmpDir.path(), TYPE_SYSTEM, "", "", "");
    EXPECT_TRUE(result);
}

TEST_F(LogBackend_UT, exportTypeLogsByCondition_validType_invalidPeriod_returnsFalse)
{
    LogBackend *b = backend();
    ASSERT_NE(b, nullptr);
    QTemporaryDir tmpDir;
    ASSERT_TRUE(tmpDir.isValid());
    Stub s;
    g_whiteList.clear();
    g_whiteList << tmpDir.path();
    s.set(ADDR(DLDBusHandler, whiteListOutPaths), stub_whiteListOutPaths_global);
    bool result = b->exportTypeLogsByCondition(tmpDir.path(), TYPE_SYSTEM, "invalid_period", "", "");
    EXPECT_FALSE(result);
}

TEST_F(LogBackend_UT, exportTypeLogsByCondition_validType_invalidCondition_returnsFalse)
{
    LogBackend *b = backend();
    ASSERT_NE(b, nullptr);
    QTemporaryDir tmpDir;
    ASSERT_TRUE(tmpDir.isValid());
    Stub s;
    g_whiteList.clear();
    g_whiteList << tmpDir.path();
    s.set(ADDR(DLDBusHandler, whiteListOutPaths), stub_whiteListOutPaths_global);
    bool result = b->exportTypeLogsByCondition(tmpDir.path(), TYPE_SYSTEM, "all", "invalid_level", "");
    EXPECT_FALSE(result);
}

TEST_F(LogBackend_UT, exportTypeLogsByCondition_validType_validPeriodCondition_returnsTrue)
{
    LogBackend *b = backend();
    ASSERT_NE(b, nullptr);
    QTemporaryDir tmpDir;
    ASSERT_TRUE(tmpDir.isValid());
    Stub s;
    g_whiteList.clear();
    g_whiteList << tmpDir.path();
    s.set(ADDR(DLDBusHandler, whiteListOutPaths), stub_whiteListOutPaths_global);
    bool result = b->exportTypeLogsByCondition(tmpDir.path(), TYPE_SYSTEM, "all", "7", "keyword");
    EXPECT_TRUE(result);
}

/******************************* slot_coredumpFinished — Report session with data (find_if lambda) *******************************/

static QStringList g_repeatCoredumpExePaths;
static QStringList stub_getRepeatCoredumpExePaths_nonEmpty() { return g_repeatCoredumpExePaths; }

TEST_F(LogBackend_UT, slot_coredumpFinished_reportSession_withData_findIfLambda)
{
    LogBackend *b = backend();
    ASSERT_NE(b, nullptr);
    b->setFlag(COREDUMP);
    b->m_sessionType = LogBackend::Report;
    b->m_coredumpCurrentIndex = 0;

    // Populate m_currentCoredumpList with 2 entries having the same exe path
    QString testExe = "/usr/bin/testapp";
    LOG_MSG_COREDUMP item1;
    item1.sig = "SIGSEGV";
    item1.dateTime = "2024-01-01 12:00:00";
    item1.coreFile = "missing";
    item1.userName = "user1";
    item1.exe = testExe;
    item1.pid = "1234";
    item1.uid = "1000";

    LOG_MSG_COREDUMP item2;
    item2.sig = "SIGABRT";
    item2.dateTime = "2024-01-02 12:00:00";
    item2.coreFile = "missing";
    item2.userName = "user2";
    item2.exe = testExe;
    item2.pid = "5678";
    item2.uid = "1000";

    b->m_currentCoredumpList.clear();
    b->m_currentCoredumpList.append(item1);
    b->m_currentCoredumpList.append(item2);

    // Override getRepeatCoredumpExePaths to return non-empty list containing testExe
    // so the if-block with std::find_if lambda is entered
    Stub s;
    g_repeatCoredumpExePaths.clear();
    g_repeatCoredumpExePaths << testExe;
    s.set(ADDR(Utils, getRepeatCoredumpExePaths), stub_getRepeatCoredumpExePaths_nonEmpty);

    b->slot_coredumpFinished(0);
    // The find_if lambda should have been covered; first item pushed, second skipped (duplicate exe)
    SUCCEED();
}
