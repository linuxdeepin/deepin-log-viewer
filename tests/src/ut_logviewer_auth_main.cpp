// SPDX-FileCopyrightText: 2019 - 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include <gtest/gtest.h>
#include <stub.h>

#include <QCoreApplication>
#include <QCommandLineParser>
#include <QProcess>
#include <QSharedMemory>
#include <QString>
#include <QStringList>
#include <QByteArray>
#include <QIODevice>

#include <sys/wait.h>
#include <unistd.h>

extern "C" void __gcov_dump(void);

// Include viewapplication.cpp for the implementation
#include "../../logViewerAuth/viewapplication.cpp"

// Include main.cpp with main renamed to avoid symbol conflict
#define main test_logViewerAuth_main
#include "../../logViewerAuth/main.cpp"
#undef main

// ---- Stubs ----

static void stub_auth_process_app(const QCoreApplication &app)
{
    Q_UNUSED(app);
}

static QStringList stub_auth_positionalArgs_empty()
{
    return {};
}

static QStringList stub_auth_positionalArgs_two()
{
    return {"dmesg", "sharedkey"};
}

static void stub_auth_QProcess_start(const QString &program, const QStringList &arguments, QIODevice::OpenMode mode)
{
    Q_UNUSED(program);
    Q_UNUSED(arguments);
    Q_UNUSED(mode);
}

static bool stub_auth_waitForFinished(int msecs)
{
    Q_UNUSED(msecs);
    return true;
}

static bool stub_auth_isAttached()
{
    return false;
}

static bool stub_auth_create(int size)
{
    Q_UNUSED(size);
    return true;
}

static bool stub_auth_attach(QSharedMemory::AccessMode mode)
{
    Q_UNUSED(mode);
    return false;
}

static void runInChild(std::function<void()> func)
{
    pid_t pid = fork();
    ASSERT_GE(pid, 0);
    if (pid == 0) {
        func();
        __gcov_dump();
        _exit(0);
    }
    int status = 0;
    waitpid(pid, &status, 0);
    EXPECT_TRUE(WIFEXITED(status) && WEXITSTATUS(status) == 0);
}

// ---- Tests ----

TEST(LogViewerAuth_Main_UT, LogViewerAuth_Main_NoArgs)
{
    Stub stub;
    stub.set((void (QCommandLineParser::*)(const QCoreApplication &))ADDR(QCommandLineParser, process), stub_auth_process_app);
    stub.set((QStringList (QCommandLineParser::*)() const)ADDR(QCommandLineParser, positionalArguments), stub_auth_positionalArgs_empty);

    runInChild([]() {
        int argc = 1;
        char arg0[] = "test";
        char *argv[] = {arg0};
        test_logViewerAuth_main(argc, argv);
    });
}

TEST(LogViewerAuth_Main_UT, LogViewerAuth_Main_WithArgs)
{
    Stub stub;
    stub.set((void (QCommandLineParser::*)(const QCoreApplication &))ADDR(QCommandLineParser, process), stub_auth_process_app);
    stub.set((QStringList (QCommandLineParser::*)() const)ADDR(QCommandLineParser, positionalArguments), stub_auth_positionalArgs_two);
    stub.set((void (QProcess::*)(const QString &, const QStringList &, QIODevice::OpenMode))ADDR(QProcess, start), stub_auth_QProcess_start);
    stub.set(ADDR(QProcess, waitForFinished), stub_auth_waitForFinished);
    stub.set(ADDR(QSharedMemory, isAttached), stub_auth_isAttached);
    stub.set(ADDR(QSharedMemory, create), stub_auth_create);
    stub.set((bool (QSharedMemory::*)(QSharedMemory::AccessMode))ADDR(QSharedMemory, attach), stub_auth_attach);

    runInChild([]() {
        int argc = 1;
        char arg0[] = "test";
        char *argv[] = {arg0};
        test_logViewerAuth_main(argc, argv);
    });
}
