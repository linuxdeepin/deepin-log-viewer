// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "viewapplication.h"
#include "qtcompat.h"

#include <stub.h>

#include <gtest/gtest.h>

#include <QCoreApplication>
#include <QAbstractEventDispatcher>
#include <QtGui/private/qeventdispatcher_glib_p.h>
#include <QtCore/private/qcoreapplication_p.h>
#include <QProcess>
#include <QSharedMemory>
#include <QMetaObject>
#include <QByteArray>
#include <QString>
#include <QStringList>

#include <cstdlib>

// ---------------------------------------------------------------------------
// ViewApplication extends QCoreApplication, so constructing one replaces the
// global QCoreApplication::self (which the test main created as a QApplication).
// If we leave it pointing at the destroyed ViewApplication, later QWidget-based
// tests crash ("QWidget: Cannot create a QWidget without QApplication").
//
// QCoreApplication::self is a *protected static* member, but the test build
// uses -fno-access-control, so we can save/restore it directly.  We also use
// synchronous `delete` (not deleteLater) so the destructor actually executes,
// which is required for 100% lcov function coverage of ~ViewApplication.
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Stub helpers for QProcess
// ---------------------------------------------------------------------------
static void stub_QProcess_start(const QString &program, const QStringList &arguments,
                                QIODevice::OpenMode mode)
{
    Q_UNUSED(program)
    Q_UNUSED(arguments)
    Q_UNUSED(mode)
}

static bool stub_QProcess_waitForFinished(int msecs)
{
    Q_UNUSED(msecs)
    return true;
}

static void stub_QProcess_kill()
{
}

static QByteArray stub_QProcess_readAll()
{
    return QByteArray("stub-output-line\n");
}

// ---------------------------------------------------------------------------
// Stub helpers for QSharedMemory
// ---------------------------------------------------------------------------
static bool stub_QSharedMemory_create(int size)
{
    Q_UNUSED(size)
    return true;
}

static bool g_shmAttached = false;
static bool stub_QSharedMemory_isAttached()
{
    return g_shmAttached;
}

static bool stub_QSharedMemory_detach()
{
    return true;
}

static bool stub_QSharedMemory_attach(QSharedMemory::AccessMode mode)
{
    Q_UNUSED(mode)
    return true;
}

static bool stub_QSharedMemory_lock()
{
    return true;
}

static void stub_QSharedMemory_unlock()
{
}

// Controlled ShareMemoryInfo payload returned by the stubbed data() pointer.
// isStart defaults to true so that the signal lambdas take the read-out branch.
static ViewApplication::ShareMemoryInfo g_shareInfo = { true };

static void *stub_QSharedMemory_data()
{
    return &g_shareInfo;
}

// Stub for exit() so the lambda "stop" branch does not terminate the process.
static bool g_exitCalled = false;
static void stub_exit(int status)
{
    Q_UNUSED(status)
    g_exitCalled = true;
}

// ---------------------------------------------------------------------------
// Helper: install the common stubs needed to construct a ViewApplication
// without actually launching external processes or touching real shared memory.
// ---------------------------------------------------------------------------
static void installConstructorStubs(Stub &stub)
{
    stub.set((void (QProcess::*)(const QString &, const QStringList &, QIODevice::OpenMode))
             ADDR(QProcess, start), stub_QProcess_start);
    stub.set(ADDR(QProcess, waitForFinished), stub_QProcess_waitForFinished);
    stub.set(ADDR(QProcess, kill), stub_QProcess_kill);
    stub.set(ADDR(QProcess, readAll), stub_QProcess_readAll);

    stub.set(ADDR(QSharedMemory, create), stub_QSharedMemory_create);
    stub.set(ADDR(QSharedMemory, isAttached), stub_QSharedMemory_isAttached);
    stub.set(ADDR(QSharedMemory, detach), stub_QSharedMemory_detach);
    stub.set((bool (QSharedMemory::*)(QSharedMemory::AccessMode))ADDR(QSharedMemory, attach),
             stub_QSharedMemory_attach);
    stub.set(ADDR(QSharedMemory, lock), stub_QSharedMemory_lock);
    stub.set(ADDR(QSharedMemory, unlock), stub_QSharedMemory_unlock);
    stub.set(static_cast<void *(QSharedMemory::*)()>(&QSharedMemory::data), stub_QSharedMemory_data);
}

// Build an argc/argv pair from a list of tokens.  The returned storage is
// static so the QCoreApplication reference to argc stays valid.
static void makeArgs(const QStringList &tokens, int &argc, char **&argv)
{
    static char argStorage[16][256];
    static char *argvStorage[16];
    argc = tokens.size();
    for (int i = 0; i < argc; ++i) {
        std::strncpy(argStorage[i], tokens[i].toUtf8().constData(), 255);
        argStorage[i][255] = '\0';
        argvStorage[i] = argStorage[i];
    }
    argv = argvStorage;
}

// Construct a ViewApplication, run the supplied test body, then synchronously
// delete it while restoring the global QCoreApplication::self around the
// destruction so the original QApplication (qApp) survives intact.
//
// ViewApplication extends QCoreApplication.  Its destructor calls
// cleanupThreadData() which nullifies the main-thread's eventDispatcher
// pointer (but does NOT delete the object — QThreadData for the main thread
// is still alive).  Without restoring the original dispatcher, later
// QWidget-based tests fail because the freshly-created replacement lacks the
// original's registered timers and glib sources.
//
// Fix: save the original dispatcher before constructing ViewApplication,
// then restore it after delete.  The object is still alive; only the thread
// data's pointer was cleared.
template <typename Body>
static void withViewApplication(const QStringList &tokens, Stub &stub, Body body)
{
    int argc = 0;
    char **argv = nullptr;
    makeArgs(tokens, argc, argv);

    QCoreApplication *savedSelf = QCoreApplication::self;
    QAbstractEventDispatcher *savedDisp = QCoreApplication::eventDispatcher();
    bool savedIsAppClosing = QCoreApplicationPrivate::is_app_closing;
    bool savedIsAppRunning = QCoreApplicationPrivate::is_app_running;
    QCoreApplicationPrivate::eventDispatcher = nullptr;
    ViewApplication *app = new ViewApplication(argc, argv);
    body(app);
    QCoreApplication::self = savedSelf;
    delete app;
    QCoreApplication::self = savedSelf;
    QCoreApplicationPrivate::eventDispatcher = savedDisp;
    QCoreApplicationPrivate::is_app_closing = savedIsAppClosing;
    QCoreApplicationPrivate::is_app_running = savedIsAppRunning;
    if (!QCoreApplication::eventDispatcher())
        QCoreApplication::setEventDispatcher(savedDisp);
}

// ===========================================================================
// Smoke test — verifies that constructing and destroying a ViewApplication
// (which extends QCoreApplication) does not corrupt the main-thread event
// dispatcher state.  withViewApplication saves/restores the dispatcher for
// every test, so this case simply exercises the construct→destroy cycle once.
// ===========================================================================
TEST(ViewApplication_Inoculation_UT, ConstructAndDestroy_UT)
{
    Stub stub;
    installConstructorStubs(stub);
    g_shmAttached = true;
    g_shareInfo.isStart = true;
    withViewApplication(QStringList() << "testapp" << "dmesg" << "inoc-key", stub, [](ViewApplication *app) {
        EXPECT_NE(app, nullptr);
    });
}

// ===========================================================================
// Constructor tests — cover every command-resolution branch
// ===========================================================================

// Fewer than two positional arguments -> early return (m_proc never created).
TEST(ViewApplication_Constructor_UT, ViewApplication_Constructor_LessThanTwoArgs_UT)
{
    Stub stub;
    installConstructorStubs(stub);
    withViewApplication(QStringList() << "testapp", stub, [](ViewApplication *app) {
        EXPECT_NE(app, nullptr);
        // m_proc is left uninitialized on the early-return path; do not touch it.
    });
}

// "dmesg" path: useFinishedSignal=false, onlyExec=false -> readyRead lambda.
TEST(ViewApplication_Constructor_UT, ViewApplication_Constructor_Dmesg_UT)
{
    Stub stub;
    installConstructorStubs(stub);
    g_shmAttached = true;
    g_shareInfo.isStart = true;
    withViewApplication(QStringList() << "testapp" << "dmesg" << "shmkey-dmesg", stub, [](ViewApplication *app) {
        EXPECT_NE(app, nullptr);
        // Trigger the readyReadStandardOutput lambda (read-out branch).
        emit app->m_proc->readyReadStandardOutput(QProcess::QPrivateSignal{});
    });
}

// "coredumpctl-list" path: useFinishedSignal=true -> finished lambda.
TEST(ViewApplication_Constructor_UT, ViewApplication_Constructor_CoredumpctlList_UT)
{
    Stub stub;
    installConstructorStubs(stub);
    g_shmAttached = true;
    g_shareInfo.isStart = true;
    withViewApplication(QStringList() << "testapp" << "coredumpctl-list" << "shmkey-clist", stub, [](ViewApplication *app) {
        EXPECT_NE(app, nullptr);
        emit app->m_proc->finished(0, QProcess::NormalExit);
    });
}

// "coredumpctl-info" path: readyRead lambda (onlyExec=false).
TEST(ViewApplication_Constructor_UT, ViewApplication_Constructor_CoredumpctlInfo_UT)
{
    Stub stub;
    installConstructorStubs(stub);
    g_shmAttached = true;
    g_shareInfo.isStart = true;
    withViewApplication(QStringList() << "testapp" << "coredumpctl-info" << "shmkey-cinfo", stub, [](ViewApplication *app) {
        EXPECT_NE(app, nullptr);
        emit app->m_proc->readyReadStandardOutput(QProcess::QPrivateSignal{});
    });
}

// "coredumpctl-dump" path: onlyExec=true -> no signal connection.
TEST(ViewApplication_Constructor_UT, ViewApplication_Constructor_CoredumpctlDump_UT)
{
    Stub stub;
    installConstructorStubs(stub);
    g_shmAttached = true;
    g_shareInfo.isStart = true;
    withViewApplication(QStringList() << "testapp" << "coredumpctl-dump" << "shmkey-dump" << "outfile", stub, [](ViewApplication *app) {
        EXPECT_NE(app, nullptr);
    });
}

// "readelf" path: useFinishedSignal=true -> finished lambda.
TEST(ViewApplication_Constructor_UT, ViewApplication_Constructor_Readelf_UT)
{
    Stub stub;
    installConstructorStubs(stub);
    g_shmAttached = true;
    g_shareInfo.isStart = true;
    withViewApplication(QStringList() << "testapp" << "readelf" << "shmkey-readelf", stub, [](ViewApplication *app) {
        EXPECT_NE(app, nullptr);
        emit app->m_proc->finished(0, QProcess::NormalExit);
    });
}

// Default (else) path: cmd="cat", readyRead lambda.
TEST(ViewApplication_Constructor_UT, ViewApplication_Constructor_Cat_UT)
{
    Stub stub;
    installConstructorStubs(stub);
    g_shmAttached = true;
    g_shareInfo.isStart = true;
    withViewApplication(QStringList() << "testapp" << "somefile" << "shmkey-cat", stub, [](ViewApplication *app) {
        EXPECT_NE(app, nullptr);
        emit app->m_proc->readyReadStandardOutput(QProcess::QPrivateSignal{});
    });
}

// "dmesg" readyRead lambda — stop branch (isStart=false -> kill + releaseMemery + exit).
TEST(ViewApplication_Constructor_UT, ViewApplication_Constructor_Dmesg_StopBranch_UT)
{
    Stub stub;
    installConstructorStubs(stub);
    stub.set((void (*)(int))QCoreApplication::exit, stub_exit);
    g_shmAttached = true;
    g_shareInfo.isStart = false;
    g_exitCalled = false;
    withViewApplication(QStringList() << "testapp" << "dmesg" << "shmkey-dmesg-stop", stub, [](ViewApplication *app) {
        EXPECT_NE(app, nullptr);
        emit app->m_proc->readyReadStandardOutput(QProcess::QPrivateSignal{});
        EXPECT_TRUE(g_exitCalled);
    });
}

// "readelf" finished lambda — stop branch (isStart=false -> kill + releaseMemery + exit).
TEST(ViewApplication_Constructor_UT, ViewApplication_Constructor_Readelf_StopBranch_UT)
{
    Stub stub;
    installConstructorStubs(stub);
    stub.set((void (*)(int))QCoreApplication::exit, stub_exit);
    g_shmAttached = true;
    g_shareInfo.isStart = false;
    g_exitCalled = false;
    withViewApplication(QStringList() << "testapp" << "readelf" << "shmkey-readelf-stop", stub, [](ViewApplication *app) {
        EXPECT_NE(app, nullptr);
        emit app->m_proc->finished(0, QProcess::NormalExit);
        EXPECT_TRUE(g_exitCalled);
    });
}

// ===========================================================================
// getControlInfo tests
// ===========================================================================

TEST(ViewApplication_GetControlInfo_UT, ViewApplication_GetControlInfo_NotAttached_UT)
{
    Stub stub;
    installConstructorStubs(stub);
    g_shmAttached = false;
    withViewApplication(QStringList() << "testapp" << "dmesg" << "shmkey-gci-na", stub, [](ViewApplication *app) {
        ViewApplication::ShareMemoryInfo info = app->getControlInfo();
        EXPECT_FALSE(info.isStart);
    });
}

TEST(ViewApplication_GetControlInfo_UT, ViewApplication_GetControlInfo_Attached_UT)
{
    Stub stub;
    installConstructorStubs(stub);
    g_shmAttached = true;
    g_shareInfo.isStart = true;
    withViewApplication(QStringList() << "testapp" << "dmesg" << "shmkey-gci-a", stub, [](ViewApplication *app) {
        ViewApplication::ShareMemoryInfo info = app->getControlInfo();
        EXPECT_TRUE(info.isStart);
    });
}

// ===========================================================================
// releaseMemery tests
// ===========================================================================

TEST(ViewApplication_ReleaseMemery_UT, ViewApplication_ReleaseMemery_Attached_UT)
{
    Stub stub;
    installConstructorStubs(stub);
    g_shmAttached = true;
    withViewApplication(QStringList() << "testapp" << "dmesg" << "shmkey-rm-a", stub, [](ViewApplication *app) {
        app->releaseMemery();
        SUCCEED();
    });
}

TEST(ViewApplication_ReleaseMemery_UT, ViewApplication_ReleaseMemery_NotAttached_UT)
{
    Stub stub;
    installConstructorStubs(stub);
    g_shmAttached = false;
    withViewApplication(QStringList() << "testapp" << "dmesg" << "shmkey-rm-na", stub, [](ViewApplication *app) {
        app->releaseMemery();
        SUCCEED();
    });
}

// Null-pointer guard: m_commondM == nullptr -> releaseMemery is a no-op.
TEST(ViewApplication_ReleaseMemery_UT, ViewApplication_ReleaseMemery_NullPointer_UT)
{
    Stub stub;
    installConstructorStubs(stub);
    g_shmAttached = true;
    withViewApplication(QStringList() << "testapp" << "dmesg" << "shmkey-rm-null", stub, [](ViewApplication *app) {
        QSharedMemory *saved = app->m_commondM;
        app->m_commondM = nullptr;
        app->releaseMemery();          // should be a no-op
        app->m_commondM = saved;       // restore so destructor cleanup is consistent
    });
}

// ===========================================================================
// Destructor test (explicit — withViewApplication already deletes every app,
// but this case documents the ~ViewApplication -> releaseMemery chain).
// ===========================================================================
TEST(ViewApplication_Destructor_UT, ViewApplication_Destructor_UT)
{
    Stub stub;
    installConstructorStubs(stub);
    g_shmAttached = true;
    withViewApplication(QStringList() << "testapp" << "dmesg" << "shmkey-dtor", stub, [](ViewApplication *app) {
        EXPECT_NE(app, nullptr);
    });
    SUCCEED();
}
