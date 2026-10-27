// SPDX-FileCopyrightText: 2022-2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include <gtest/gtest.h>
#include "dbusproxy/dldbushandler.h"
#include "../application/logallexportthread.h"
#include <stub.h>

QStringList LogAllExportThread_stub_toString()
{
    QStringList list;
    list << "test" << "test2";
    return list;
}

bool LogAllExportThread_stub_bool(const QString &outDir, const QString &in, bool isFile)
{
    Q_UNUSED(outDir);
    Q_UNUSED(in);
    Q_UNUSED(isFile);
    return false;
}


TEST(LogAllExportThread_LogAllExportThread_UT, LogAllExportThread_LogAllExportThread_UT_001)
{
    QStringList thread;
    thread << "test" << "test2";
    LogAllExportThread *p = new LogAllExportThread(thread, "path");
    ASSERT_TRUE(p);
    EXPECT_EQ(p->m_outfile, "path");
    delete p;
}


TEST(LogAllExportThread_run_UT, LogAllExportThread_run_UT_001)
{
    QStringList thread;
    thread << "test" << "test2";
    LogAllExportThread *p = new LogAllExportThread(thread, "path");
    ASSERT_TRUE(p);
    Stub stub;
    stub.set(ADDR(DLDBusHandler, getFileInfo), LogAllExportThread_stub_toString);
    stub.set(ADDR(DLDBusHandler, exportLog), LogAllExportThread_stub_bool);
    p->m_types << JOUR_TREE_DATA << BOOT_KLU_TREE_DATA << DMESG_TREE_DATA << LAST_TREE_DATA <<
               DPKG_TREE_DATA << KERN_TREE_DATA << XORG_TREE_DATA << DNF_TREE_DATA << BOOT_TREE_DATA <<
               KWIN_TREE_DATA << APP_TREE_DATA;
    p->run();
    delete p;
}

TEST(LogAllExportThread_run_UT, LogAllExportThread_run_UT_002)
{
    QStringList thread;
    thread << "test" << "test2";
    Stub stub;
    stub.set(ADDR(DLDBusHandler, getFileInfo), LogAllExportThread_stub_toString);
    stub.set(ADDR(DLDBusHandler, exportLog), LogAllExportThread_stub_bool);
    LogAllExportThread *p = new LogAllExportThread(thread, "path");
    ASSERT_TRUE(p);
    p->run();
    delete p;
}

TEST(LogAllExportThread_slot_cancelExport_UT, LogAllExportThread_slot_cancelExport_UT_001)
{
    QStringList thread;
    thread << "test" << "test2";
    LogAllExportThread *p = new LogAllExportThread(thread, "path");
    ASSERT_TRUE(p);
    p->slot_cancelExport();
    delete p;
}

// Stub for exportOpsLog: creates a file at the given path and returns true.
// Note: first param receives the `this` pointer (member function stub convention).
bool stub_exportOpsLog(void *unused, const QString &zipFilePath)
{
    Q_UNUSED(unused);
    QFile f(zipFilePath);
    if (f.open(QIODevice::WriteOnly)) {
        f.write("dummy");
        f.close();
    }
    return true;
}

// Stub for Utils::exportUserPermissionOpsLogs (static method, no `this` pointer).
void stub_exportUserPermissionOpsLogs(const QString &outDir, const QString &userHomeDir)
{
    Q_UNUSED(outDir);
    Q_UNUSED(userHomeDir);
}

// Stub for Utils::executeCmd (static method, no `this` pointer).
QByteArray stub_executeCmd_retEmpty(const QString &cmdStr, const QStringList &args, const QString &workPath)
{
    Q_UNUSED(cmdStr);
    Q_UNUSED(args);
    Q_UNUSED(workPath);
    return QByteArray();
}

// Test: cover the lambda addDirToZip inside run() by making exportOpsLog succeed.
// This lambda is only reached when exportOpsLog returns true and the ops zip file exists.
TEST(LogAllExportThread_run_UT, LogAllExportThread_run_UT_003)
{
    QString outFile = QDir::tempPath() + "/ut_logallexport_test.zip";

    QStringList types;
    types << "nonexistent_type";
    LogAllExportThread *p = new LogAllExportThread(types, outFile);
    ASSERT_TRUE(p);

    Stub stub;
    stub.set(ADDR(DLDBusHandler, getFileInfo), LogAllExportThread_stub_toString);
    stub.set(ADDR(DLDBusHandler, exportLog), LogAllExportThread_stub_bool);
    stub.set(ADDR(DLDBusHandler, exportOpsLog), stub_exportOpsLog);
    stub.set(ADDR(Utils, exportUserPermissionOpsLogs), stub_exportUserPermissionOpsLogs);
    stub.set(ADDR(Utils, executeCmd), stub_executeCmd_retEmpty);

    p->run();
    delete p;

    QFile::remove(outFile);
}
