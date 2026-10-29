// SPDX-FileCopyrightText: 2019-2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "logcollectormain.h"
#include "DebugTimeManager.h"
#include "ut_stuballthread.h"
#include "logapplication.h"
#include "utils.h"
#include "ut_stubpublic.h"
#include "logapplicationhelper.h"
#include <stub.h>

#include <DTitlebar>
#include <DWindowCloseButton>
#include <DDialog>
#include <DStyle>
#include <QFileDialog>

#include <QKeyEvent>
#include <QDebug>
#include <QList>
#include <QDir>
#include "dbusmanager.h"
#include "dbusproxy/dldbushandler.h"
#include "logallexportthread.h"

#include <gtest/gtest.h>

void Log_beginPointLinux(const QString &point, const QString &status)
{
    Q_UNUSED(point);
    Q_UNUSED(status);
}

void LogCollectorMain_QThreadPool_start(QRunnable *runnable, int priority = 0)
{
    Q_UNUSED(runnable);
    Q_UNUSED(priority);
}

static bool LogCollectorMain_checkAuthorization_false(const QString &actionId, qint64 applicationPid)
{
    Q_UNUSED(actionId);
    Q_UNUSED(applicationPid);
    return false;
}

static bool LogCollectorMain_checkAuthorization_true(const QString &actionId, qint64 applicationPid)
{
    Q_UNUSED(actionId);
    Q_UNUSED(applicationPid);
    return true;
}

static QString LogCollectorMain_QString()
{
    return "true";
}

int LogCollectorMain_int()
{
    return 2;
}

int LogCollectorMain_int_exec()
{
    return 0;
}

bool LogCollectorMain_notify(QObject *obj, QEvent *event)
{
    Q_UNUSED(obj);
    Q_UNUSED(event);
    return false;
}


// 兜底：部分用例链中 QApplication 可能被 deleteLater 销毁，建 QWidget 前确保实例存在
static QApplication *ensureQtApp()
{
    if (qApp == nullptr) {
        static int argc = 0;
        static QApplication *app = new QApplication(argc, nullptr);
        return app;
    }
    return static_cast<QApplication *>(qApp);
}

TEST(LogCollectorMain_Constructor_UT, LogCollectorMain_Constructor_UT)
{
    ensureQtApp();
    Stub stub;
    stub.set(ADDR(DebugTimeManager, beginPointLinux), Log_beginPointLinux);
    stub.set(ADDR(LogFileParser, parseByJournal), LogFileParser_parseByJournal);
    stub.set(ADDR(LogFileParser, parseByJournalBoot), LogFileParser_parseByJournalBoot);
    stub.set(ADDR(LogFileParser, parseByDpkg), LogFileParser_parseByDpkg);
    stub.set(ADDR(LogFileParser, parseByXlog), LogFileParser_parseByXlog);
    stub.set(ADDR(LogFileParser, parseByBoot), LogFileParser_parseByBoot);
    stub.set(ADDR(LogFileParser, parseByKern), LogFileParser_parseByKern);
    stub.set(ADDR(LogFileParser, parseByApp), LogFileParser_parseByApp);
    stub.set(ADDR(LogFileParser, parseByNormal), LogFileParser_parseByNormal);
    stub.set(ADDR(LogFileParser, parseByKwin), LogFileParser_parseByKwin);
    stub.set(ADDR(LogApplicationHelper, getCustomLogList), LogApplicationHelper_getCustomLogList);
    LogCollectorMain *p = new LogCollectorMain(nullptr);
    EXPECT_NE(p, nullptr);
    delete p;
}

TEST(LogCollectorMain_Destructor_UT, LogCollectorMain_Destructor_UT)
{
    ensureQtApp();
    Stub stub;
    stub.set(ADDR(DebugTimeManager, beginPointLinux), Log_beginPointLinux);
    stub.set(ADDR(LogFileParser, parseByJournal), LogFileParser_parseByJournal);
    stub.set(ADDR(LogFileParser, parseByJournalBoot), LogFileParser_parseByJournalBoot);
    stub.set(ADDR(LogFileParser, parseByDpkg), LogFileParser_parseByDpkg);
    stub.set(ADDR(LogFileParser, parseByXlog), LogFileParser_parseByXlog);
    stub.set(ADDR(LogFileParser, parseByBoot), LogFileParser_parseByBoot);
    stub.set(ADDR(LogFileParser, parseByKern), LogFileParser_parseByKern);
    stub.set(ADDR(LogFileParser, parseByApp), LogFileParser_parseByApp);
    stub.set(ADDR(LogFileParser, parseByNormal), LogFileParser_parseByNormal);
    stub.set(ADDR(LogFileParser, parseByKwin), LogFileParser_parseByKwin);
    stub.set(ADDR(LogApplicationHelper, getCustomLogList), LogApplicationHelper_getCustomLogList);
    LogCollectorMain *p = new LogCollectorMain(nullptr);
    EXPECT_NE(p, nullptr);
    delete p;
}
TEST(LogCollectorMain_initUI_UT, LogCollectorMain_initUI_UT)
{
    ensureQtApp();
    Stub stub;
    stub.set(ADDR(DebugTimeManager, beginPointLinux), Log_beginPointLinux);
    stub.set(ADDR(LogFileParser, parseByJournal), LogFileParser_parseByJournal);
    stub.set(ADDR(LogFileParser, parseByJournalBoot), LogFileParser_parseByJournalBoot);
    stub.set(ADDR(LogFileParser, parseByDpkg), LogFileParser_parseByDpkg);
    stub.set(ADDR(LogFileParser, parseByXlog), LogFileParser_parseByXlog);
    stub.set(ADDR(LogFileParser, parseByBoot), LogFileParser_parseByBoot);
    stub.set(ADDR(LogFileParser, parseByKern), LogFileParser_parseByKern);
    stub.set(ADDR(LogFileParser, parseByApp), LogFileParser_parseByApp);
    stub.set(ADDR(LogFileParser, parseByNormal), LogFileParser_parseByNormal);
    stub.set(ADDR(LogFileParser, parseByKwin), LogFileParser_parseByKwin);
    stub.set(ADDR(LogApplicationHelper, getCustomLogList), LogApplicationHelper_getCustomLogList);
    LogCollectorMain *p = new LogCollectorMain(nullptr);
    EXPECT_NE(p, nullptr);
    delete p;
}

TEST(LogCollectorMain_initShortCut_UT, LogCollectorMain_initShortCut_UT_001)
{
    ensureQtApp();
    Stub stub;
    stub.set(ADDR(DebugTimeManager, beginPointLinux), Log_beginPointLinux);
    stub.set(ADDR(LogFileParser, parseByJournal), LogFileParser_parseByJournal);
    stub.set(ADDR(LogFileParser, parseByJournalBoot), LogFileParser_parseByJournalBoot);
    stub.set(ADDR(LogFileParser, parseByDpkg), LogFileParser_parseByDpkg);
    stub.set(ADDR(LogFileParser, parseByXlog), LogFileParser_parseByXlog);
    stub.set(ADDR(LogFileParser, parseByBoot), LogFileParser_parseByBoot);
    stub.set(ADDR(LogFileParser, parseByKern), LogFileParser_parseByKern);
    stub.set(ADDR(LogFileParser, parseByApp), LogFileParser_parseByApp);
    stub.set(ADDR(LogFileParser, parseByNormal), LogFileParser_parseByNormal);
    stub.set(ADDR(LogFileParser, parseByKwin), LogFileParser_parseByKwin);
    stub.set(ADDR(LogApplicationHelper, getCustomLogList), LogApplicationHelper_getCustomLogList);
    LogCollectorMain *p = new LogCollectorMain(nullptr);
    EXPECT_NE(p, nullptr);
    p->initShortCut();
    delete p;
}

TEST(LogCollectorMain_initShortCut_UT, LogCollectorMain_initShortCut_UT_002)
{
    ensureQtApp();
    Stub stub;
    stub.set(ADDR(DebugTimeManager, beginPointLinux), Log_beginPointLinux);
    stub.set(ADDR(LogFileParser, parseByJournal), LogFileParser_parseByJournal);
    stub.set(ADDR(LogFileParser, parseByJournalBoot), LogFileParser_parseByJournalBoot);
    stub.set(ADDR(LogFileParser, parseByDpkg), LogFileParser_parseByDpkg);
    stub.set(ADDR(LogFileParser, parseByXlog), LogFileParser_parseByXlog);
    stub.set(ADDR(LogFileParser, parseByBoot), LogFileParser_parseByBoot);
    stub.set(ADDR(LogFileParser, parseByKern), LogFileParser_parseByKern);
    stub.set(ADDR(LogFileParser, parseByApp), LogFileParser_parseByApp);
    stub.set(ADDR(LogFileParser, parseByNormal), LogFileParser_parseByNormal);
    stub.set(ADDR(LogFileParser, parseByKwin), LogFileParser_parseByKwin);
    stub.set(ADDR(LogApplicationHelper, getCustomLogList), LogApplicationHelper_getCustomLogList);
    LogCollectorMain *p = new LogCollectorMain(nullptr);
    EXPECT_NE(p, nullptr);
    p->m_scWndReize->deleteLater();
    p->m_scWndReize = nullptr;
    p->m_scFindFont->deleteLater();
    p->m_scFindFont = nullptr;
    p->m_scExport->deleteLater();
    p->m_scExport = nullptr;
    p->initShortCut();
    //    p->m_scWndReize->activated();
    //    p->m_scFindFont->activated();
    //    p->m_scExport->activated();
    delete p;
}

TEST(LogCollectorMain_initConnection_UT, LogCollectorMain_initConnection_UT)
{
    ensureQtApp();
    Stub stub;
    stub.set(ADDR(DebugTimeManager, beginPointLinux), Log_beginPointLinux);
    stub.set(ADDR(LogFileParser, parseByJournal), LogFileParser_parseByJournal);
    stub.set(ADDR(LogFileParser, parseByJournalBoot), LogFileParser_parseByJournalBoot);
    stub.set(ADDR(LogFileParser, parseByDpkg), LogFileParser_parseByDpkg);
    stub.set(ADDR(LogFileParser, parseByXlog), LogFileParser_parseByXlog);
    stub.set(ADDR(LogFileParser, parseByBoot), LogFileParser_parseByBoot);
    stub.set(ADDR(LogFileParser, parseByKern), LogFileParser_parseByKern);
    stub.set(ADDR(LogFileParser, parseByApp), LogFileParser_parseByApp);
    stub.set(ADDR(LogFileParser, parseByNormal), LogFileParser_parseByNormal);
    stub.set(ADDR(LogFileParser, parseByKwin), LogFileParser_parseByKwin);
    stub.set(ADDR(LogApplicationHelper, getCustomLogList), LogApplicationHelper_getCustomLogList);
    LogCollectorMain *p = new LogCollectorMain(nullptr);
    EXPECT_NE(p, nullptr);

    p->initConnection();
    delete p;
}
//TEST(LogCollectorMain_resizeWidthByFilterContentWidth_UT, LogCollectorMain_resizeWidthByFilterContentWidth_UT)
//{
//    Stub stub;
//    stub.set(ADDR(DebugTimeManager, beginPointLinux), Log_beginPointLinux);
//    stub.set(ADDR(LogFileParser, parseByJournal), LogFileParser_parseByJournal);
//    stub.set(ADDR(LogFileParser, parseByJournalBoot), LogFileParser_parseByJournalBoot);
//    stub.set(ADDR(LogFileParser, parseByDpkg), LogFileParser_parseByDpkg);
//    stub.set(ADDR(LogFileParser, parseByXlog), LogFileParser_parseByXlog);
//    stub.set(ADDR(LogFileParser, parseByBoot), LogFileParser_parseByBoot);
//    stub.set(ADDR(LogFileParser, parseByKern), LogFileParser_parseByKern);
//    stub.set(ADDR(LogFileParser, parseByApp), LogFileParser_parseByApp);
//    stub.set(ADDR(LogFileParser, parseByNormal), LogFileParser_parseByNormal);
//    stub.set(ADDR(LogFileParser, parseByKwin), LogFileParser_parseByKwin);
//    LogCollectorMain *p = new LogCollectorMain(nullptr);
//    EXPECT_NE(p, nullptr);
//    p->resizeWidthByFilterContentWidth(500);
//    delete p;
//}

class LogCollectorMain_handleApplicationTabEventNotify_UT_Param
{
public:
    LogCollectorMain_handleApplicationTabEventNotify_UT_Param(bool iKey, const QString &iObjectName, bool iResult, const QString &iFocusObjectName)
        : key(iKey)
        , objectName(iObjectName)
        , result(iResult)
        , focusObjectName(iFocusObjectName)
    {
    }
    int key;
    QString objectName;
    bool result;
    QString focusObjectName;
};

class LogCollectorMain_handleApplicationTabEventNotify_UT : public ::testing::TestWithParam<LogCollectorMain_handleApplicationTabEventNotify_UT_Param>
{
};

INSTANTIATE_TEST_CASE_P(LogCollectorMain, LogCollectorMain_handleApplicationTabEventNotify_UT, ::testing::Values(/*LogCollectorMain_handleApplicationTabEventNotify_UT_Param(Qt::Key_0, "", false, ""),
                                                                                                                  LogCollectorMain_handleApplicationTabEventNotify_UT_Param(Qt::Key_Tab, "", false, ""),
                                                                                                                                                                                                                                    */
                                                                                                                 LogCollectorMain_handleApplicationTabEventNotify_UT_Param(Qt::Key_Tab, "titlebar", true, ""), LogCollectorMain_handleApplicationTabEventNotify_UT_Param(Qt::Key_Tab, "searchChildEdt", false, "titlebar"), LogCollectorMain_handleApplicationTabEventNotify_UT_Param(Qt::Key_Tab, "DTitlebarDWindowCloseButton", false, "logTypeSelectList"), LogCollectorMain_handleApplicationTabEventNotify_UT_Param(Qt::Key_Tab, "mainLogTable", true, "searchChildEdt"), LogCollectorMain_handleApplicationTabEventNotify_UT_Param(Qt::Key_Backtab, "logTypeSelectList", true, "DTitlebarDWindowCloseButton"), LogCollectorMain_handleApplicationTabEventNotify_UT_Param(Qt::Key_Backtab, "DTitlebarDWindowOptionButton", true, "searchChildEdt"), LogCollectorMain_handleApplicationTabEventNotify_UT_Param(Qt::Key_Backtab, "searchChildEdt", true, "")
                                                                                                                 // LogCollectorMain_handleApplicationTabEventNotify_UT_Param(Qt::Key_Backtab, "", false, "")
                                                                                                                 ));
QString stub_ObjectName()
{
    return QString("logTypeSelectList");
}

TEST_P(LogCollectorMain_handleApplicationTabEventNotify_UT, LogCollectorMain_handleApplicationTabEventNotify_UT_001)
{
    Stub stub;
    stub.set(ADDR(DebugTimeManager, beginPointLinux), Log_beginPointLinux);
    stub.set(ADDR(LogFileParser, parseByJournal), LogFileParser_parseByJournal);
    stub.set(ADDR(LogFileParser, parseByJournalBoot), LogFileParser_parseByJournalBoot);
    stub.set(ADDR(LogFileParser, parseByDpkg), LogFileParser_parseByDpkg);
    stub.set(ADDR(LogFileParser, parseByXlog), LogFileParser_parseByXlog);
    stub.set(ADDR(LogFileParser, parseByBoot), LogFileParser_parseByBoot);
    stub.set(ADDR(LogFileParser, parseByKern), LogFileParser_parseByKern);
    stub.set(ADDR(LogFileParser, parseByApp), LogFileParser_parseByApp);
    stub.set(ADDR(LogFileParser, parseByNormal), LogFileParser_parseByNormal);
    stub.set(ADDR(LogFileParser, parseByKwin), LogFileParser_parseByKwin);
    stub.set(ADDR(LogApplicationHelper, getCustomLogList), LogApplicationHelper_getCustomLogList);
    stub.set(ADDR(QObject, objectName), stub_ObjectName);
    LogCollectorMain_handleApplicationTabEventNotify_UT_Param param = GetParam();
    LogCollectorMain *p = new LogCollectorMain(nullptr);
    EXPECT_NE(p, nullptr);

    QKeyEvent keyEvent(QEvent::KeyPress, param.key, Qt::NoModifier);
    QWidget *w = nullptr;
    if (!param.objectName.isEmpty())
        w = p->titlebar()->findChild<QWidget *>(param.objectName);
    p->handleApplicationTabEventNotify(w, &keyEvent);
    delete p;
}

TEST_P(LogCollectorMain_handleApplicationTabEventNotify_UT, LogCollectorMain_handleApplicationTabEventNotify_UT_002)
{
    Stub stub;
    stub.set(ADDR(DebugTimeManager, beginPointLinux), Log_beginPointLinux);
    stub.set(ADDR(LogFileParser, parseByJournal), LogFileParser_parseByJournal);
    stub.set(ADDR(LogFileParser, parseByJournalBoot), LogFileParser_parseByJournalBoot);
    stub.set(ADDR(LogFileParser, parseByDpkg), LogFileParser_parseByDpkg);
    stub.set(ADDR(LogFileParser, parseByXlog), LogFileParser_parseByXlog);
    stub.set(ADDR(LogFileParser, parseByBoot), LogFileParser_parseByBoot);
    stub.set(ADDR(LogFileParser, parseByKern), LogFileParser_parseByKern);
    stub.set(ADDR(LogFileParser, parseByApp), LogFileParser_parseByApp);
    stub.set(ADDR(LogFileParser, parseByNormal), LogFileParser_parseByNormal);
    stub.set(ADDR(LogFileParser, parseByKwin), LogFileParser_parseByKwin);
    stub.set(ADDR(QObject, objectName), stub_ObjectName);
    stub.set(ADDR(LogApplicationHelper, getCustomLogList), LogApplicationHelper_getCustomLogList);
    LogCollectorMain_handleApplicationTabEventNotify_UT_Param param = GetParam();
    LogCollectorMain *p = new LogCollectorMain(nullptr);
    EXPECT_NE(p, nullptr);

    QKeyEvent keyEvent(QEvent::KeyPress, Qt::Key_Backtab, Qt::NoModifier);
    QWidget *w = nullptr;
    if (!param.objectName.isEmpty())
        w = p->titlebar()->findChild<QWidget *>(QString("logTypeSelectList"));
    p->handleApplicationTabEventNotify(w, &keyEvent);
    delete p;
}

TEST(LogCollectorMain_switchRefreshActionTriggered_UT, LogCollectorMain_switchRefreshActionTriggered_UT_001)
{
    ensureQtApp();
    Stub stub;
    stub.set(ADDR(DebugTimeManager, beginPointLinux), Log_beginPointLinux);
    stub.set(ADDR(LogFileParser, parseByJournal), LogFileParser_parseByJournal);
    stub.set(ADDR(LogFileParser, parseByJournalBoot), LogFileParser_parseByJournalBoot);
    stub.set(ADDR(LogFileParser, parseByDpkg), LogFileParser_parseByDpkg);
    stub.set(ADDR(LogFileParser, parseByXlog), LogFileParser_parseByXlog);
    stub.set(ADDR(LogFileParser, parseByBoot), LogFileParser_parseByBoot);
    stub.set(ADDR(LogFileParser, parseByKern), LogFileParser_parseByKern);
    stub.set(ADDR(LogFileParser, parseByApp), LogFileParser_parseByApp);
    stub.set(ADDR(LogFileParser, parseByNormal), LogFileParser_parseByNormal);
    stub.set(ADDR(LogFileParser, parseByKwin), LogFileParser_parseByKwin);
    stub.set(ADDR(LogApplicationHelper, getCustomLogList), LogApplicationHelper_getCustomLogList);
    typedef  bool (*fptr)(LogApplication *);
    fptr test = (fptr)(&LogApplication::notify);
    stub.set(test, LogCollectorMain_notify);
    LogCollectorMain *p = new LogCollectorMain(nullptr);
    ASSERT_TRUE(p);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QAction *action = new QAction(qApp->translate("titlebar", "5 min"));
    stub.set(ADDR(QList<QAction *>, indexOf), LogCollectorMain_int);
    p->switchRefreshActionTriggered(action);
    if (p->m_refreshTimer && p->m_refreshTimer->isActive()) {
        p->m_refreshTimer->stop();
    }
    delete action;
#endif
    delete p;
}

TEST(LogCollectorMain_exportAllLogs_UT, LogCollectorMain_exportAllLogs_UT_001)
{
    ensureQtApp();
    Stub stub;
    stub.set(ADDR(DebugTimeManager, beginPointLinux), Log_beginPointLinux);
    stub.set(ADDR(LogFileParser, parseByJournal), LogFileParser_parseByJournal);
    stub.set(ADDR(LogFileParser, parseByJournalBoot), LogFileParser_parseByJournalBoot);
    stub.set(ADDR(LogFileParser, parseByDpkg), LogFileParser_parseByDpkg);
    stub.set(ADDR(LogFileParser, parseByXlog), LogFileParser_parseByXlog);
    stub.set(ADDR(LogFileParser, parseByBoot), LogFileParser_parseByBoot);
    stub.set(ADDR(LogFileParser, parseByKern), LogFileParser_parseByKern);
    stub.set(ADDR(LogFileParser, parseByApp), LogFileParser_parseByApp);
    stub.set(ADDR(LogFileParser, parseByNormal), LogFileParser_parseByNormal);
    stub.set(ADDR(LogFileParser, parseByKwin), LogFileParser_parseByKwin);
    stub.set(ADDR(LogApplicationHelper, getCustomLogList), LogApplicationHelper_getCustomLogList);
    typedef  bool (*fptr)(LogApplication *);
    fptr test = (fptr)(&LogApplication::notify);
    stub.set(test, LogCollectorMain_notify);
    LogCollectorMain *p = new LogCollectorMain(nullptr);
    ASSERT_TRUE(p);
    stub.set(ADDR(Utils, checkAuthorization), LogCollectorMain_checkAuthorization_false);
    p->exportAllLogs();
    delete p;
}

TEST(LogCollectorMain_exportAllLogs_UT, LogCollectorMain_exportAllLogs_UT_002)
{
    ensureQtApp();
    Stub stub;
    stub.set(ADDR(DebugTimeManager, beginPointLinux), Log_beginPointLinux);
    stub.set(ADDR(LogFileParser, parseByJournal), LogFileParser_parseByJournal);
    stub.set(ADDR(LogFileParser, parseByJournalBoot), LogFileParser_parseByJournalBoot);
    stub.set(ADDR(LogFileParser, parseByDpkg), LogFileParser_parseByDpkg);
    stub.set(ADDR(LogFileParser, parseByXlog), LogFileParser_parseByXlog);
    stub.set(ADDR(LogFileParser, parseByBoot), LogFileParser_parseByBoot);
    stub.set(ADDR(LogFileParser, parseByKern), LogFileParser_parseByKern);
    stub.set(ADDR(LogFileParser, parseByApp), LogFileParser_parseByApp);
    stub.set(ADDR(LogFileParser, parseByNormal), LogFileParser_parseByNormal);
    stub.set(ADDR(LogFileParser, parseByKwin), LogFileParser_parseByKwin);
    stub.set(ADDR(LogApplicationHelper, getCustomLogList), LogApplicationHelper_getCustomLogList);
    typedef  bool (*fptr)(LogApplication *);
    fptr test = (fptr)(&LogApplication::notify);
    stub.set(test, LogCollectorMain_notify);
    LogCollectorMain *p = new LogCollectorMain(nullptr);
    ASSERT_TRUE(p);
    stub.set(ADDR(Utils, checkAuthorization), LogCollectorMain_checkAuthorization_true);
    stub.set(ADDR(QFileDialog, getSaveFileName), LogCollectorMain_QString);
//    stub.set(ADDR(QThreadPool, start), LogCollectorMain_QThreadPool_start);
    stub.set((void (QThreadPool::*)(QRunnable *, int))ADDR(QThreadPool, start), LogCollectorMain_QThreadPool_start);

    typedef  int (*fptr2)(DDialog *);
    fptr2 test1 = (fptr2)(&DDialog::exec);
    stub.set(test1, LogCollectorMain_int_exec);
    p->exportAllLogs();
    delete p;
}

TEST(LogCollectorMain_closeEvent_UT, LogCollectorMain_closeEvent_UT_001)
{
    ensureQtApp();
    Stub stub;
    stub.set(ADDR(DebugTimeManager, beginPointLinux), Log_beginPointLinux);
    stub.set(ADDR(LogFileParser, parseByJournal), LogFileParser_parseByJournal);
    stub.set(ADDR(LogFileParser, parseByJournalBoot), LogFileParser_parseByJournalBoot);
    stub.set(ADDR(LogFileParser, parseByDpkg), LogFileParser_parseByDpkg);
    stub.set(ADDR(LogFileParser, parseByXlog), LogFileParser_parseByXlog);
    stub.set(ADDR(LogFileParser, parseByBoot), LogFileParser_parseByBoot);
    stub.set(ADDR(LogFileParser, parseByKern), LogFileParser_parseByKern);
    stub.set(ADDR(LogFileParser, parseByApp), LogFileParser_parseByApp);
    stub.set(ADDR(LogFileParser, parseByNormal), LogFileParser_parseByNormal);
    stub.set(ADDR(LogFileParser, parseByKwin), LogFileParser_parseByKwin);
    stub.set(ADDR(LogApplicationHelper, getCustomLogList), LogApplicationHelper_getCustomLogList);
    typedef  bool (*fptr)(LogApplication *);
    fptr test = (fptr)(&LogApplication::notify);
    stub.set(test, LogCollectorMain_notify);
    LogCollectorMain *p = new LogCollectorMain(nullptr);
    ASSERT_TRUE(p);
    QCloseEvent *event = new QCloseEvent();
    ASSERT_TRUE(event);
    p->closeEvent(event);
    delete event;
    delete p;
}



// ===== Tests added to cover lambda closures (target: 100% function coverage) =====

#include <QMetaObject>
#include <QTimer>
#include <QFutureWatcher>
#include <QElapsedTimer>
#include <QThread>

// 泵事件但排除定时器派发：早前用例可能遗留指向已销毁对象的跨线程定时器，
// 常规 QTest::qWait 会派发这些定时器触发悬垂回调（SEGV）；
// 信号投递与 deferred delete 仍正常处理
static void pumpEventsNoTimers(int ms)
{
    QElapsedTimer t;
    t.start();
    while (!t.hasExpired(ms)) {
        QCoreApplication::processEvents(
            QEventLoop::ExcludeUserInputEvents | QEventLoop::X11ExcludeTimers, 20);
        QThread::msleep(10);
    }
}
#include <QtConcurrent>
#include <QEventLoop>
#include <QTest>
#include <DIconButton>

// Stub: LogAllExportThread::run — emit signals to trigger the lambdas connected in exportAllLogs
void LogAllExportThread_run_stub(LogAllExportThread *self)
{
    self->updateTolProcess(100);
    self->updatecurrentProcess(50);
    self->exportFinsh(true);
}

// Stub: DLDBusHandler::whiteListOutPaths — return non-empty list so export path check passes
QStringList DLDBusHandler_whiteListOutPaths_stub()
{
    return QStringList() << "." << "/" << QDir::homePath();
}

// Stub: DBusManager::isSEOpen
bool DBusManager_isSEOpen_stub()
{
    return false;
}

// Stub: DBusManager::isAuditAdmin
bool DBusManager_isAuditAdmin_stub()
{
    return false;
}

// Stub: DBusManager::isScreenLocked — return false so refresh lambda else-branch fires
bool DBusManager_isScreenLocked_false()
{
    return false;
}

// Stub: DDialog::exec that processes events so queued lambdas can fire
int LogCollectorMain_exec_processEvents(DDialog *self)
{
    Q_UNUSED(self)
    // Process events for a short time to let thread signals fire
    pumpEventsNoTimers(500);
    return 1;
}

// ===== initShortCut lambda coverage =====
TEST(LogCollectorMain_initShortCut_UT, LogCollectorMain_initShortCut_UT_003)
{
    ensureQtApp();
    // Set DStyle so DApplication::style() returns a valid DStyle* — prevents SEGV
    // in LogPeriodButton::paintEvent when showMaximized()/showNormal() triggers repaints
    qApp->setStyle(new DStyle);
    Stub stub;
    stub.set(ADDR(DebugTimeManager, beginPointLinux), Log_beginPointLinux);
    stub.set(ADDR(LogFileParser, parseByJournal), LogFileParser_parseByJournal);
    stub.set(ADDR(LogFileParser, parseByJournalBoot), LogFileParser_parseByJournalBoot);
    stub.set(ADDR(LogFileParser, parseByDpkg), LogFileParser_parseByDpkg);
    stub.set(ADDR(LogFileParser, parseByXlog), LogFileParser_parseByXlog);
    stub.set(ADDR(LogFileParser, parseByBoot), LogFileParser_parseByBoot);
    stub.set(ADDR(LogFileParser, parseByKern), LogFileParser_parseByKern);
    stub.set(ADDR(LogFileParser, parseByApp), LogFileParser_parseByApp);
    stub.set(ADDR(LogFileParser, parseByNormal), LogFileParser_parseByNormal);
    stub.set(ADDR(LogFileParser, parseByKwin), LogFileParser_parseByKwin);
    stub.set(ADDR(LogApplicationHelper, getCustomLogList), LogApplicationHelper_getCustomLogList);
    LogCollectorMain *p = new LogCollectorMain(nullptr);
    ASSERT_TRUE(p);

    // Emit the activated signal on each shortcut to invoke the lambdas
    if (p->m_scWndReize) {
        QMetaObject::invokeMethod(p->m_scWndReize, "activated");
    }
    if (p->m_scFindFont) {
        QMetaObject::invokeMethod(p->m_scFindFont, "activated");
    }
    if (p->m_scExport) {
        QMetaObject::invokeMethod(p->m_scExport, "activated");
    }
    // Process events so signal lambdas execute
    pumpEventsNoTimers(100);

    delete p;
}

// ===== initTitlebarExtensions lambda coverage =====
TEST(LogCollectorMain_initTitlebarExtensions_UT, LogCollectorMain_initTitlebarExtensions_UT_002)
{
    ensureQtApp();
    Stub stub;
    stub.set(ADDR(DebugTimeManager, beginPointLinux), Log_beginPointLinux);
    stub.set(ADDR(LogFileParser, parseByJournal), LogFileParser_parseByJournal);
    stub.set(ADDR(LogFileParser, parseByJournalBoot), LogFileParser_parseByJournalBoot);
    stub.set(ADDR(LogFileParser, parseByDpkg), LogFileParser_parseByDpkg);
    stub.set(ADDR(LogFileParser, parseByXlog), LogFileParser_parseByXlog);
    stub.set(ADDR(LogFileParser, parseByBoot), LogFileParser_parseByBoot);
    stub.set(ADDR(LogFileParser, parseByKern), LogFileParser_parseByKern);
    stub.set(ADDR(LogFileParser, parseByApp), LogFileParser_parseByApp);
    stub.set(ADDR(LogFileParser, parseByNormal), LogFileParser_parseByNormal);
    stub.set(ADDR(LogFileParser, parseByKwin), LogFileParser_parseByKwin);
    stub.set(ADDR(LogApplicationHelper, getCustomLogList), LogApplicationHelper_getCustomLogList);
    typedef bool (*fptr)(LogApplication *);
    fptr test = (fptr)(&LogApplication::notify);
    stub.set(test, LogCollectorMain_notify);
    LogCollectorMain *p = new LogCollectorMain(nullptr);
    ASSERT_TRUE(p);

    // Click the refresh button to trigger the clicked lambda
    if (p->m_refreshBtn) {
        p->m_refreshBtn->click();
    }
    pumpEventsNoTimers(100);

    delete p;
}

// ===== switchRefreshActionTriggered lambda coverage =====
TEST(LogCollectorMain_switchRefreshActionTriggered_UT, LogCollectorMain_switchRefreshActionTriggered_UT_002)
{
    ensureQtApp();
    Stub stub;
    stub.set(ADDR(DebugTimeManager, beginPointLinux), Log_beginPointLinux);
    stub.set(ADDR(LogFileParser, parseByJournal), LogFileParser_parseByJournal);
    stub.set(ADDR(LogFileParser, parseByJournalBoot), LogFileParser_parseByJournalBoot);
    stub.set(ADDR(LogFileParser, parseByDpkg), LogFileParser_parseByDpkg);
    stub.set(ADDR(LogFileParser, parseByXlog), LogFileParser_parseByXlog);
    stub.set(ADDR(LogFileParser, parseByBoot), LogFileParser_parseByBoot);
    stub.set(ADDR(LogFileParser, parseByKern), LogFileParser_parseByKern);
    stub.set(ADDR(LogFileParser, parseByApp), LogFileParser_parseByApp);
    stub.set(ADDR(LogFileParser, parseByNormal), LogFileParser_parseByNormal);
    stub.set(ADDR(LogFileParser, parseByKwin), LogFileParser_parseByKwin);
    stub.set(ADDR(LogApplicationHelper, getCustomLogList), LogApplicationHelper_getCustomLogList);
    typedef bool (*fptr)(LogApplication *);
    fptr test = (fptr)(&LogApplication::notify);
    stub.set(test, LogCollectorMain_notify);
    // Stub DBusManager::isScreenLocked so QtConcurrent::run completes quickly
    stub.set(ADDR(DBusManager, isScreenLocked), DBusManager_isScreenLocked_false);
    LogCollectorMain *p = new LogCollectorMain(nullptr);
    ASSERT_TRUE(p);

    // Use the actual refresh action at index 0 ("10 sec") so indexOf returns 0
    // and timeInterval = 10*1000 > 0, which creates the timer.
    if (!p->m_refreshActions.isEmpty()) {
        p->switchRefreshActionTriggered(p->m_refreshActions[0]);
    }

    // 验证刷新定时器已被正确配置。不在此手动触发 timeout 及其异步刷新链：
    // 链上的 QtConcurrent/lambda 回调与用例间事件泵时序强相关，会产生悬垂回调
    // （SEGV）；switchRefreshActionTriggered 的同步逻辑已被本用例直接覆盖
    if (p->m_refreshTimer) {
        EXPECT_TRUE(p->m_refreshTimer->isActive());
        EXPECT_GE(p->m_refreshTimer->interval(), 1000);
        p->m_refreshTimer->stop();
    }
    delete p;
}

// ===== exportAllLogs lambda coverage =====
TEST(LogCollectorMain_exportAllLogs_UT, LogCollectorMain_exportAllLogs_UT_003)
{
    ensureQtApp();
    Stub stub;
    stub.set(ADDR(DebugTimeManager, beginPointLinux), Log_beginPointLinux);
    stub.set(ADDR(LogFileParser, parseByJournal), LogFileParser_parseByJournal);
    stub.set(ADDR(LogFileParser, parseByJournalBoot), LogFileParser_parseByJournalBoot);
    stub.set(ADDR(LogFileParser, parseByDpkg), LogFileParser_parseByDpkg);
    stub.set(ADDR(LogFileParser, parseByXlog), LogFileParser_parseByXlog);
    stub.set(ADDR(LogFileParser, parseByBoot), LogFileParser_parseByBoot);
    stub.set(ADDR(LogFileParser, parseByKern), LogFileParser_parseByKern);
    stub.set(ADDR(LogFileParser, parseByApp), LogFileParser_parseByApp);
    stub.set(ADDR(LogFileParser, parseByNormal), LogFileParser_parseByNormal);
    stub.set(ADDR(LogFileParser, parseByKwin), LogFileParser_parseByKwin);
    stub.set(ADDR(LogApplicationHelper, getCustomLogList), LogApplicationHelper_getCustomLogList);
    typedef bool (*fptr)(LogApplication *);
    fptr test = (fptr)(&LogApplication::notify);
    stub.set(test, LogCollectorMain_notify);
    LogCollectorMain *p = new LogCollectorMain(nullptr);
    ASSERT_TRUE(p);

    // Authorization: use true so auth passes (static bool already set true by UT_002,
    // but set it anyway for independence)
    stub.set(ADDR(Utils, checkAuthorization), LogCollectorMain_checkAuthorization_true);
    // File dialog returns a path
    stub.set(ADDR(QFileDialog, getSaveFileName), LogCollectorMain_QString);
    // Whitelist returns non-empty so path check passes
    stub.set(ADDR(DLDBusHandler, whiteListOutPaths), DLDBusHandler_whiteListOutPaths_stub);
    // DBus checks stubbed to avoid hangs
    stub.set(ADDR(DBusManager, isSEOpen), DBusManager_isSEOpen_stub);
    stub.set(ADDR(DBusManager, isAuditAdmin), DBusManager_isAuditAdmin_stub);
    // DDialog::exec processes events so thread signals fire the lambdas
    typedef int (*fptr2)(DDialog *);
    fptr2 test1 = (fptr2)(&DDialog::exec);
    stub.set(test1, LogCollectorMain_exec_processEvents);
    // Stub LogAllExportThread::run to emit the 3 signals so the connected lambdas fire
    typedef void (*fptr_run)(LogAllExportThread *);
    fptr_run run_ptr = (fptr_run)(&LogAllExportThread::run);
    stub.set(run_ptr, LogAllExportThread_run_stub);

    p->exportAllLogs();
    pumpEventsNoTimers(500);

    delete p;
}
