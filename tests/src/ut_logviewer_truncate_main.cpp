// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include <gtest/gtest.h>
#include <stub.h>

#include <QCoreApplication>
#include <QCommandLineParser>
#include <QProcess>
#include <QString>
#include <QStringList>
#include <QByteArray>
#include <QIODevice>

#include <iostream>
#include <sys/wait.h>
#include <unistd.h>

extern "C" void __gcov_dump(void);

// Include the source file with main renamed to avoid symbol conflict
#define main test_logViewerTruncate_main
#include "../../logViewerTruncate/main.cpp"
#undef main

// ---- Stubs ----

static void stub_truncate_process_app(const QCoreApplication &app)
{
    Q_UNUSED(app);
}

static QStringList stub_truncate_positionalArgs_empty()
{
    return {};
}

static QStringList stub_truncate_positionalArgs_dmesg()
{
    return {"dmesg"};
}

static QStringList stub_truncate_positionalArgs_file()
{
    return {"/tmp/testfile"};
}

static void stub_truncate_QProcess_start(const QString &program, const QStringList &arguments, QIODevice::OpenMode mode)
{
    Q_UNUSED(program);
    Q_UNUSED(arguments);
    Q_UNUSED(mode);
}

static bool stub_truncate_waitForFinished(int msecs)
{
    Q_UNUSED(msecs);
    return true;
}

static QByteArray stub_truncate_readAllStandardOutput()
{
    return "test output\n";
}

// ---- Helper: run main() in forked child to avoid QCoreApplication conflict ----
static void runInChild(std::function<void()> func)
{
#if defined(__SANITIZE_ADDRESS__)
    // ASAN 进程 fork 出的子进程会在运行时内部锁上死锁，无法等到退出
    GTEST_SKIP() << "skip fork-based main test under AddressSanitizer";
#else
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
#endif
}

// ---- Tests ----

TEST(LogViewerTruncate_Main_UT, LogViewerTruncate_Main_NoArgs)
{
    Stub stub;
    stub.set((void (QCommandLineParser::*)(const QCoreApplication &))ADDR(QCommandLineParser, process), stub_truncate_process_app);
    stub.set((QStringList (QCommandLineParser::*)() const)ADDR(QCommandLineParser, positionalArguments), stub_truncate_positionalArgs_empty);

    runInChild([]() {
        int argc = 1;
        char arg0[] = "test";
        char *argv[] = {arg0};
        test_logViewerTruncate_main(argc, argv);
    });
}

TEST(LogViewerTruncate_Main_UT, LogViewerTruncate_Main_Dmesg)
{
    Stub stub;
    stub.set((void (QCommandLineParser::*)(const QCoreApplication &))ADDR(QCommandLineParser, process), stub_truncate_process_app);
    stub.set((QStringList (QCommandLineParser::*)() const)ADDR(QCommandLineParser, positionalArguments), stub_truncate_positionalArgs_dmesg);
    stub.set((void (QProcess::*)(const QString &, const QStringList &, QIODevice::OpenMode))ADDR(QProcess, start), stub_truncate_QProcess_start);
    stub.set(ADDR(QProcess, waitForFinished), stub_truncate_waitForFinished);
    stub.set(ADDR(QProcess, readAllStandardOutput), stub_truncate_readAllStandardOutput);

    runInChild([]() {
        int argc = 1;
        char arg0[] = "test";
        char *argv[] = {arg0};
        test_logViewerTruncate_main(argc, argv);
    });
}

TEST(LogViewerTruncate_Main_UT, LogViewerTruncate_Main_TruncateFile)
{
    Stub stub;
    stub.set((void (QCommandLineParser::*)(const QCoreApplication &))ADDR(QCommandLineParser, process), stub_truncate_process_app);
    stub.set((QStringList (QCommandLineParser::*)() const)ADDR(QCommandLineParser, positionalArguments), stub_truncate_positionalArgs_file);
    stub.set((void (QProcess::*)(const QString &, const QStringList &, QIODevice::OpenMode))ADDR(QProcess, start), stub_truncate_QProcess_start);
    stub.set(ADDR(QProcess, waitForFinished), stub_truncate_waitForFinished);
    stub.set(ADDR(QProcess, readAllStandardOutput), stub_truncate_readAllStandardOutput);

    runInChild([]() {
        int argc = 1;
        char arg0[] = "test";
        char *argv[] = {arg0};
        test_logViewerTruncate_main(argc, argv);
    });
}
