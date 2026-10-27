// SPDX-FileCopyrightText: 2019 - 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include <gtest/gtest.h>
#include <stub.h>

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QTemporaryDir>
#include <QTextStream>
#include <QCoreApplication>

#include "../../logViewerService/opslogexport.cpp"

static int stub_runProcess(const QStringList &args, const char *outputFile, const QString &pattern, int afterLines)
{
    Q_UNUSED(args);
    Q_UNUSED(outputFile);
    Q_UNUSED(pattern);
    Q_UNUSED(afterLines);
    return 0;
}

TEST(OpsLogExport_AppendToFile_UT, OpsLogExport_AppendToFile_UT_001)
{
    QTemporaryDir tmpDir;
    ASSERT_TRUE(tmpDir.isValid());
    QString filePath = tmpDir.path() + "/test_append.txt";

    appendToFile(filePath, "hello world\n");

    QFile file(filePath);
    ASSERT_TRUE(file.open(QIODevice::ReadOnly | QIODevice::Text));
    EXPECT_EQ(file.readAll().toStdString(), "hello world\n");
    file.close();
}

TEST(OpsLogExport_AppendToFile_UT, OpsLogExport_AppendToFile_UT_002)
{
    QTemporaryDir tmpDir;
    ASSERT_TRUE(tmpDir.isValid());
    QString filePath = tmpDir.path() + "/test_append.txt";

    appendToFile(filePath, "line1\n");
    appendToFile(filePath, "line2\n");

    QFile file(filePath);
    ASSERT_TRUE(file.open(QIODevice::ReadOnly | QIODevice::Text));
    EXPECT_EQ(file.readAll().toStdString(), "line1\nline2\n");
    file.close();
}

TEST(OpsLogExport_GrepA_UT, OpsLogExport_GrepA_UT_001)
{
    QByteArray data = "line1: error something\nline2: normal\nline3: error another\nline4: detail1\nline5: detail2\nline6: normal\n";
    QList<QStringList> result = grepA(data, "error", 2);

    // grepA reads sequentially: line1 matches, consumes line2+line3 as after-lines,
    // so line3 (also "error") is NOT re-matched -> 1 group of 3 lines.
    ASSERT_EQ(result.size(), 1);
    EXPECT_EQ(result.at(0).size(), 3);
    EXPECT_TRUE(result.at(0).at(0).contains("error"));
    EXPECT_TRUE(result.at(0).at(2).contains("error"));
}

TEST(OpsLogExport_GrepA_UT, OpsLogExport_GrepA_UT_002)
{
    QByteArray data = "no matching content here\nanother line\n";
    QList<QStringList> result = grepA(data, "nonexistent_pattern", 2);
    EXPECT_TRUE(result.isEmpty());
}

TEST(OpsLogExport_GrepA_UT, OpsLogExport_GrepA_UT_003)
{
    QByteArray data = "match_line\n";
    QList<QStringList> result = grepA(data, "match", 5);
    ASSERT_EQ(result.size(), 1);
    EXPECT_EQ(result.at(0).size(), 1);
    EXPECT_TRUE(result.at(0).at(0).contains("match"));
}

TEST(OpsLogExport_AppendToFileWithGrepA_UT, OpsLogExport_AppendToFileWithGrepA_UT_001)
{
    QTemporaryDir tmpDir;
    ASSERT_TRUE(tmpDir.isValid());
    QString filePath = tmpDir.path() + "/grep_result.txt";

    QByteArray data = "header\nerror: something\n detail1\n detail2\nfooter\nerror: another\n detail3\n";
    appendToFileWithGrepA(filePath, data, "error", 2);

    QFile file(filePath);
    ASSERT_TRUE(file.open(QIODevice::ReadOnly | QIODevice::Text));
    QString content = file.readAll();
    EXPECT_TRUE(content.contains("error: something"));
    EXPECT_TRUE(content.contains("detail1"));
    EXPECT_TRUE(content.contains("error: another"));
    file.close();
}

TEST(OpsLogExport_AppendToFileWithGrepA_UT, OpsLogExport_AppendToFileWithGrepA_UT_002)
{
    QTemporaryDir tmpDir;
    ASSERT_TRUE(tmpDir.isValid());
    QString filePath = tmpDir.path() + "/grep_result.txt";

    QByteArray data = "no matching content\n";
    appendToFileWithGrepA(filePath, data, "nonexistent_pattern", 2);

    QFile file(filePath);
    ASSERT_TRUE(file.open(QIODevice::ReadOnly | QIODevice::Text));
    EXPECT_TRUE(file.readAll().isEmpty());
    file.close();
}

TEST(OpsLogExport_RunProcess_UT, OpsLogExport_RunProcess_UT_001)
{
    QStringList args;
    int result = runProcess(args);
    EXPECT_EQ(result, -1);
}

TEST(OpsLogExport_RunProcess_UT, OpsLogExport_RunProcess_UT_002)
{
    QTemporaryDir tmpDir;
    ASSERT_TRUE(tmpDir.isValid());
    QString outFile = tmpDir.path() + "/out.txt";

    QStringList args;
    args << "echo" << "hello_test";
    int result = runProcess(args, outFile.toLocal8Bit().constData());

    EXPECT_EQ(result, 0);
    QFile file(outFile);
    ASSERT_TRUE(file.open(QIODevice::ReadOnly | QIODevice::Text));
    EXPECT_TRUE(file.readAll().contains("hello_test"));
    file.close();
}

TEST(OpsLogExport_RunProcess_UT, OpsLogExport_RunProcess_UT_003)
{
    QTemporaryDir tmpDir;
    ASSERT_TRUE(tmpDir.isValid());
    QString outFile = tmpDir.path() + "/out_grep.txt";

    QStringList args;
    args << "echo" << "match_line\nother_line";
    int result = runProcess(args, outFile.toLocal8Bit().constData(), "match", 1);

    EXPECT_EQ(result, 0);
    QFile file(outFile);
    ASSERT_TRUE(file.open(QIODevice::ReadOnly | QIODevice::Text));
    QString content = file.readAll();
    EXPECT_TRUE(content.contains("match_line"));
    file.close();
}

TEST(OpsLogExport_ExpandPathWithWildcardIterator_UT, OpsLogExport_ExpandPathWithWildcardIterator_UT_001)
{
    QTemporaryDir tmpDir;
    ASSERT_TRUE(tmpDir.isValid());
    QString dir = tmpDir.path();

    QFile f1(dir + "/test1.log");
    f1.open(QIODevice::WriteOnly);
    f1.close();
    QFile f2(dir + "/test2.log");
    f2.open(QIODevice::WriteOnly);
    f2.close();
    QFile f3(dir + "/other.txt");
    f3.open(QIODevice::WriteOnly);
    f3.close();

    QStringList result = expandPathWithWildcardIterator(dir + "/test*.log");
    EXPECT_EQ(result.size(), 2);
}

TEST(OpsLogExport_ExpandPathWithWildcardIterator_UT, OpsLogExport_ExpandPathWithWildcardIterator_UT_002)
{
    QStringList result = expandPathWithWildcardIterator("/nonexistent_dir_xyz/*.log");
    EXPECT_TRUE(result.isEmpty());
}

TEST(OpsLogExport_Constructor_UT, OpsLogExport_Constructor_UT_001)
{
    QTemporaryDir tmpDir;
    ASSERT_TRUE(tmpDir.isValid());
    OpsLogExport exporter(tmpDir.path().toStdString());
    SUCCEED();
}

TEST(OpsLogExport_PathExists_UT, OpsLogExport_PathExists_UT_001)
{
    QTemporaryDir tmpDir;
    ASSERT_TRUE(tmpDir.isValid());
    OpsLogExport exporter(tmpDir.path().toStdString());
    EXPECT_TRUE(exporter.path_exists(tmpDir.path().toStdString()));
}

TEST(OpsLogExport_PathExists_UT, OpsLogExport_PathExists_UT_002)
{
    QTemporaryDir tmpDir;
    ASSERT_TRUE(tmpDir.isValid());
    OpsLogExport exporter(tmpDir.path().toStdString());
    EXPECT_FALSE(exporter.path_exists("/nonexistent_path_xyz_12345"));
}

TEST(OpsLogExport_CreateDirectories_UT, OpsLogExport_CreateDirectories_UT_001)
{
    QTemporaryDir tmpDir;
    ASSERT_TRUE(tmpDir.isValid());
    OpsLogExport exporter(tmpDir.path().toStdString());
    std::string newDir = tmpDir.path().toStdString() + "/a/b/c";
    EXPECT_TRUE(exporter.create_directories(newDir));
    EXPECT_TRUE(QDir(QString::fromStdString(newDir)).exists());
}

TEST(OpsLogExport_CopyFileOrDir_UT, OpsLogExport_CopyFileOrDir_UT_001)
{
    QTemporaryDir tmpDir;
    ASSERT_TRUE(tmpDir.isValid());
    QString srcFile = tmpDir.path() + "/src.txt";
    QFile f(srcFile);
    f.open(QIODevice::WriteOnly);
    f.write("test_content");
    f.close();

    OpsLogExport exporter(tmpDir.path().toStdString());
    std::string dstDir = tmpDir.path().toStdString() + "/dst";
    exporter.copy_file_or_dir(srcFile.toStdString(), dstDir);

    QFile copied(QString::fromStdString(dstDir) + "/src.txt");
    ASSERT_TRUE(copied.open(QIODevice::ReadOnly));
    EXPECT_EQ(copied.readAll().toStdString(), "test_content");
    copied.close();
}

TEST(OpsLogExport_CopyFileOrDir_UT, OpsLogExport_CopyFileOrDir_UT_002)
{
    QTemporaryDir tmpDir;
    ASSERT_TRUE(tmpDir.isValid());
    OpsLogExport exporter(tmpDir.path().toStdString());
    exporter.copy_file_or_dir("/nonexistent_src_xyz_12345", tmpDir.path().toStdString());
    SUCCEED();
}

TEST(OpsLogExport_CopyFileOrDir_UT, OpsLogExport_CopyFileOrDir_UT_003)
{
    QTemporaryDir tmpDir;
    ASSERT_TRUE(tmpDir.isValid());
    QString srcFile = tmpDir.path() + "/src.txt";
    QFile f(srcFile);
    f.open(QIODevice::WriteOnly);
    f.write("data");
    f.close();

    QString linkFile = tmpDir.path() + "/link.txt";
    QFile::link(srcFile, linkFile);

    OpsLogExport exporter(tmpDir.path().toStdString());
    std::string dstDir = tmpDir.path().toStdString() + "/dst_link";
    exporter.copy_file_or_dir(linkFile.toStdString(), dstDir);

    EXPECT_FALSE(QFile::exists(QString::fromStdString(dstDir) + "/link.txt"));
}

TEST(OpsLogExport_CopyFileOrDir_UT, OpsLogExport_CopyFileOrDir_UT_004)
{
    QTemporaryDir tmpDir;
    ASSERT_TRUE(tmpDir.isValid());
    QString srcDir = tmpDir.path() + "/srcdir";
    QDir().mkpath(srcDir);
    QFile f(srcDir + "/inner.txt");
    f.open(QIODevice::WriteOnly);
    f.write("inner");
    f.close();

    OpsLogExport exporter(tmpDir.path().toStdString());
    std::string dstDir = tmpDir.path().toStdString() + "/dstdir";

    Stub stub;
    stub.set(runProcess, stub_runProcess);
    exporter.copy_file_or_dir(srcDir.toStdString(), dstDir);
}

TEST(OpsLogExport_ExecuteCommand_UT, OpsLogExport_ExecuteCommand_UT_001)
{
    QTemporaryDir tmpDir;
    ASSERT_TRUE(tmpDir.isValid());
    OpsLogExport exporter(tmpDir.path().toStdString());

    Stub stub;
    stub.set(runProcess, stub_runProcess);
    exporter.execute_command({"echo", "test"}, tmpDir.path().toStdString() + "/out.txt");
    SUCCEED();
}

TEST(OpsLogExport_CreateDirStruct_UT, OpsLogExport_CreateDirStruct_UT_001)
{
    QTemporaryDir tmpDir;
    ASSERT_TRUE(tmpDir.isValid());
    OpsLogExport exporter(tmpDir.path().toStdString());
    exporter.createDirStruct();

    QDir base(tmpDir.path());
    EXPECT_TRUE(base.exists("kernel"));
    EXPECT_TRUE(base.exists("system"));
    EXPECT_TRUE(base.exists("dde"));
    EXPECT_TRUE(base.exists("app"));
    EXPECT_TRUE(base.exists("app/deepin-log-viewer"));
    EXPECT_TRUE(base.exists("app/deepin-calculator"));
    EXPECT_TRUE(base.exists("dde/dde-file-manager"));
    EXPECT_TRUE(base.exists("system/pulseaudio"));
    EXPECT_TRUE(base.exists("apt"));
    EXPECT_TRUE(base.exists("uos-ste"));
    EXPECT_TRUE(base.exists("uosste_logs"));
}

TEST(OpsLogExport_ExportAppLogs_UT, OpsLogExport_ExportAppLogs_UT_001)
{
    QTemporaryDir tmpDir;
    ASSERT_TRUE(tmpDir.isValid());
    OpsLogExport exporter(tmpDir.path().toStdString());
    exporter.createDirStruct();

    Stub stub;
    stub.set(runProcess, stub_runProcess);
    exporter.exportAppLogs();
    SUCCEED();
}

TEST(OpsLogExport_ExportSystemLogs_UT, OpsLogExport_ExportSystemLogs_UT_001)
{
    QTemporaryDir tmpDir;
    ASSERT_TRUE(tmpDir.isValid());
    OpsLogExport exporter(tmpDir.path().toStdString());
    exporter.createDirStruct();

    Stub stub;
    stub.set(runProcess, stub_runProcess);
    exporter.exportSystemLogs();
    SUCCEED();
}

TEST(OpsLogExport_ExportKernelLogs_UT, OpsLogExport_ExportKernelLogs_UT_001)
{
    QTemporaryDir tmpDir;
    ASSERT_TRUE(tmpDir.isValid());
    OpsLogExport exporter(tmpDir.path().toStdString());
    exporter.createDirStruct();

    Stub stub;
    stub.set(runProcess, stub_runProcess);
    exporter.exportKernelLogs();
    SUCCEED();
}

TEST(OpsLogExport_ExportDDELogs_UT, OpsLogExport_ExportDDELogs_UT_001)
{
    QTemporaryDir tmpDir;
    ASSERT_TRUE(tmpDir.isValid());
    OpsLogExport exporter(tmpDir.path().toStdString());
    exporter.createDirStruct();

    Stub stub;
    stub.set(runProcess, stub_runProcess);
    exporter.exportDDELogs();
    SUCCEED();
}

TEST(OpsLogExport_ExportHWLogs_UT, OpsLogExport_ExportHWLogs_UT_001)
{
    QTemporaryDir tmpDir;
    ASSERT_TRUE(tmpDir.isValid());
    OpsLogExport exporter(tmpDir.path().toStdString());

    Stub stub;
    stub.set(runProcess, stub_runProcess);
    exporter.exportHWLogs();
    SUCCEED();
}

TEST(OpsLogExport_ExportOSVersionLogs_UT, OpsLogExport_ExportOSVersionLogs_UT_001)
{
    QTemporaryDir tmpDir;
    ASSERT_TRUE(tmpDir.isValid());
    OpsLogExport exporter(tmpDir.path().toStdString());

    Stub stub;
    stub.set(runProcess, stub_runProcess);
    exporter.exportOSVersionLogs();
    SUCCEED();
}

TEST(OpsLogExport_ExportAptLogs_UT, OpsLogExport_ExportAptLogs_UT_001)
{
    QTemporaryDir tmpDir;
    ASSERT_TRUE(tmpDir.isValid());
    OpsLogExport exporter(tmpDir.path().toStdString());

    exporter.exportAptLogs();
    SUCCEED();
}

TEST(OpsLogExport_ExportUosSteLogs_UT, OpsLogExport_ExportUosSteLogs_UT_001)
{
    QTemporaryDir tmpDir;
    ASSERT_TRUE(tmpDir.isValid());
    OpsLogExport exporter(tmpDir.path().toStdString());

    exporter.exportUosSteLogs();
    SUCCEED();
}

TEST(OpsLogExport_ExportUosSteTwoLogs_UT, OpsLogExport_ExportUosSteTwoLogs_UT_001)
{
    QTemporaryDir tmpDir;
    ASSERT_TRUE(tmpDir.isValid());
    OpsLogExport exporter(tmpDir.path().toStdString());

    exporter.exportUosSteTwoLogs();
    SUCCEED();
}

TEST(OpsLogExport_Run_UT, OpsLogExport_Run_UT_001)
{
    QTemporaryDir tmpDir;
    ASSERT_TRUE(tmpDir.isValid());
    OpsLogExport exporter(tmpDir.path().toStdString());

    Stub stub;
    stub.set(runProcess, stub_runProcess);
    exporter.run();

    QDir base(tmpDir.path());
    EXPECT_TRUE(base.exists("kernel"));
    EXPECT_TRUE(base.exists("system"));
    EXPECT_TRUE(base.exists("app"));
}
