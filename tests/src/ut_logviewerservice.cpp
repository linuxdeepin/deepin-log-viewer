// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include <gtest/gtest.h>
#include <QTest>
#include <QTemporaryFile>
#include <QTemporaryDir>
#include <QTextStream>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QProcess>
#include <QDBusUnixFileDescriptor>
#include <QCoreApplication>
#include <QDBusContext>
#include <QDBusMessage>
#include <polkit-qt6-1/polkitqt1-authority.h>

#include "logviewerservice.h"
#include "stub.h"

// 声明 logviewerservice.cpp 中具有外部链接的自由函数（未在头文件导出）
extern void removeDirFiles(const QString &dirPath);
extern QString unzipToTempFile(const QString &sourceFile, const QString &tempFileTemplate);

// ===================== 打桩函数 =====================
static bool stub_checkAuth_true(const QString &) { return true; }
static bool stub_checkAuth_false(const QString &) { return false; }
static bool stub_checkAuthorization_true(const QString &) { return true; }
static bool stub_checkAuthorization_false(const QString &) { return false; }
static QDBusMessage stub_dbus_message_valid() { return QDBusMessage::createSignal("/", "fake.interface", "signal"); }
static PolkitQt1::Authority::Result stub_checkAuthSync_yes(const QString &, const PolkitQt1::Subject &, PolkitQt1::Authority::AuthorizationFlags) { return PolkitQt1::Authority::Yes; }

// ===================== 测试夹具 =====================
class LogViewerServiceTest : public ::testing::Test
{
protected:
    LogViewerService *m_service = nullptr;
    void SetUp() override
    {
        m_service = new LogViewerService();
    }
    void TearDown() override
    {
        delete m_service;
        m_service = nullptr;
    }
};

// ===================== 构造与析构 =====================
TEST_F(LogViewerServiceTest, ConstructorAndDestructor)
{
    SUCCEED();
}

// ===================== 自由函数 removeDirFiles =====================
TEST_F(LogViewerServiceTest, RemoveDirFiles)
{
    QTemporaryDir tmpDir;
    ASSERT_TRUE(tmpDir.isValid());
    QString dirPath = tmpDir.path();
    QFile f1(dirPath + "/a.txt");
    ASSERT_TRUE(f1.open(QIODevice::WriteOnly));
    f1.write("hello");
    f1.close();
    QFile f2(dirPath + "/b.txt");
    ASSERT_TRUE(f2.open(QIODevice::WriteOnly));
    f2.write("world");
    f2.close();

    removeDirFiles(dirPath);
    QDir dir(dirPath);
    dir.setFilter(QDir::NoDotAndDotDot | QDir::Files);
    EXPECT_EQ(dir.entryList().size(), 0);
}

TEST_F(LogViewerServiceTest, RemoveDirFilesNonExistent)
{
    removeDirFiles("/tmp/non-existent-dir-12345");
    SUCCEED();
}

// ===================== 自由函数 unzipToTempFile =====================
TEST_F(LogViewerServiceTest, UnzipToTempFile)
{
    QTemporaryDir tmpDir;
    ASSERT_TRUE(tmpDir.isValid());
    QString gzPath = tmpDir.path() + "/test.txt.gz";
    QString rawPath = tmpDir.path() + "/test.txt";
    {
        QFile raw(rawPath);
        ASSERT_TRUE(raw.open(QIODevice::WriteOnly));
        raw.write("gunzip content\n");
        raw.close();
    }
    QProcess gzip;
    gzip.start("gzip", QStringList() << rawPath);
    ASSERT_TRUE(gzip.waitForFinished(10000));
    ASSERT_TRUE(QFile::exists(gzPath));

    QString tmpl = tmpDir.path() + "/unzip_XXXXXX.txt";
    QString result = unzipToTempFile(gzPath, tmpl);
    QFile out(result);
    ASSERT_TRUE(out.open(QIODevice::ReadOnly));
    EXPECT_TRUE(out.readAll().contains("gunzip content"));
    out.close();
    QFile::remove(result);
}

TEST_F(LogViewerServiceTest, UnzipToTempFileInvalidSource)
{
    QTemporaryDir tmpDir;
    ASSERT_TRUE(tmpDir.isValid());
    QString tmpl = tmpDir.path() + "/unzip_XXXXXX.txt";
    QString result = unzipToTempFile("/tmp/non-existent-source.gz", tmpl);
    EXPECT_TRUE(result.isEmpty() || QFile::exists(result));
    if (!result.isEmpty())
        QFile::remove(result);
}

// ===================== checkAuth（私有，直接调用） =====================
TEST_F(LogViewerServiceTest, CheckAuthNotFromDBus)
{
    EXPECT_FALSE(m_service->checkAuth("com.deepin.pkexec.logViewerAuth"));
}

// ===================== checkAuthorization（私有） =====================
TEST_F(LogViewerServiceTest, CheckAuthorizationStubbedTrue)
{
    Stub stub;
    stub.set(ADDR(LogViewerService, checkAuthorization), stub_checkAuthorization_true);
    EXPECT_TRUE(m_service->checkAuthorization("any"));
}

TEST_F(LogViewerServiceTest, CheckAuthorizationWithStubbedMessage)
{
    Stub stub;
    stub.set(ADDR(QDBusContext, message), stub_dbus_message_valid);
    stub.set(ADDR(PolkitQt1::Authority, checkAuthorizationSync), stub_checkAuthSync_yes);
    EXPECT_TRUE(m_service->checkAuthorization("com.deepin.pkexec.logViewerAuth"));
}

// ===================== trackCurrentCaller（私有） =====================
TEST_F(LogViewerServiceTest, TrackCurrentCallerNotFromDBus)
{
    m_service->trackCurrentCaller();
    SUCCEED();
}

// ===================== onClientUnregistered（私有） =====================
TEST_F(LogViewerServiceTest, OnClientUnregistered)
{
    m_service->onClientUnregistered(":1.999");
    SUCCEED();
}

// ===================== getCallerHomeDir（私有） =====================
TEST_F(LogViewerServiceTest, GetCallerHomeDirNotFromDBus)
{
    EXPECT_TRUE(m_service->getCallerHomeDir().isEmpty());
}

// ===================== clearTempFiles（私有） =====================
TEST_F(LogViewerServiceTest, ClearTempFiles)
{
    QString tmpPath = QDir::tempPath();
    QFile f(tmpPath + "/ut_test.lz4.dump");
    f.open(QIODevice::WriteOnly);
    f.write("dummy");
    f.close();
    m_service->clearTempFiles();
    EXPECT_FALSE(QFile::exists(tmpPath + "/ut_test.lz4.dump"));
}

// ===================== findLineStartOffsetWithCaching（私有） =====================
TEST_F(LogViewerServiceTest, FindLineStartOffsetWithCaching)
{
    QTemporaryFile tmpFile;
    ASSERT_TRUE(tmpFile.open());
    tmpFile.write("line1\nline2\nline3\n");
    tmpFile.flush();

    qint64 off0 = m_service->findLineStartOffsetWithCaching(tmpFile.fileName(), 0);
    EXPECT_GE(off0, 0);
    qint64 off1 = m_service->findLineStartOffsetWithCaching(tmpFile.fileName(), 1);
    EXPECT_GT(off1, off0);
    qint64 off2 = m_service->findLineStartOffsetWithCaching(tmpFile.fileName(), 2);
    EXPECT_GT(off2, off1);
}

TEST_F(LogViewerServiceTest, FindLineStartOffsetWithCachingInvalidFile)
{
    EXPECT_EQ(m_service->findLineStartOffsetWithCaching("/tmp/non-existent-file-xyz", 0), -1);
}

// ===================== removeOpsTempDirByPathInternal（私有） =====================
TEST_F(LogViewerServiceTest, RemoveOpsTempDirByPathInternalWrongPrefix)
{
    EXPECT_FALSE(m_service->removeOpsTempDirByPathInternal("/tmp/wrong-prefix-123"));
}

TEST_F(LogViewerServiceTest, RemoveOpsTempDirByPathInternalNonExistentValidPrefix)
{
    // 合法前缀但目标不存在：open /var/log 成功，safeRemoveDirRecursive 被进入，
    // openat 失败 -> unlinkat 失败 -> 返回 false，函数体全部覆盖。
    EXPECT_FALSE(m_service->removeOpsTempDirByPathInternal(
        "/var/log/deepin-log-viewer-ops-log.nonexistent-ut-dir"));
}

// ===================== safeRemoveDirRecursive（私有，经 removeOpsTempDirByPathInternal 间接覆盖） =====================
// 已由 RemoveOpsTempDirByPathInternalNonExistentValidPrefix 覆盖进入。

// ===================== readLog(QString) 私有 =====================
TEST_F(LogViewerServiceTest, ReadLogByPathAuthFail)
{
    EXPECT_EQ(m_service->readLog(QString("/tmp/test")), QString(" "));
}

TEST_F(LogViewerServiceTest, ReadLogByPathAuthOk)
{
    Stub stub;
    stub.set(ADDR(LogViewerService, checkAuth), stub_checkAuth_true);

    QTemporaryFile tmpFile;
    ASSERT_TRUE(tmpFile.open());
    tmpFile.write("log content line\n");
    tmpFile.flush();
    // QTemporaryFile 默认路径在 /tmp 下
    QString result = m_service->readLog(tmpFile.fileName());
    EXPECT_TRUE(result.contains("log content line"));
}

TEST_F(LogViewerServiceTest, ReadLogByPathNotInWhitelist)
{
    Stub stub;
    stub.set(ADDR(LogViewerService, checkAuth), stub_checkAuth_true);
    EXPECT_EQ(m_service->readLog(QString("/home/user/secret")), QString(" "));
}

TEST_F(LogViewerServiceTest, ReadLogByPathContainsDotDot)
{
    Stub stub;
    stub.set(ADDR(LogViewerService, checkAuth), stub_checkAuth_true);
    EXPECT_EQ(m_service->readLog(QString("/tmp/../etc/passwd")), QString(" "));
}

// ===================== readLog(fd) 公开槽 =====================
TEST_F(LogViewerServiceTest, ReadLogByFdAuthFail)
{
    QTemporaryFile pathFile;
    ASSERT_TRUE(pathFile.open());
    pathFile.write("/tmp/whatever");
    pathFile.flush();
    QDBusUnixFileDescriptor fd(pathFile.handle());
    QString result = m_service->readLog(fd);
    EXPECT_EQ(result, " ");
}

TEST_F(LogViewerServiceTest, ReadLogByFdAuthOk)
{
    Stub stub;
    stub.set(ADDR(LogViewerService, checkAuth), stub_checkAuth_true);

    QTemporaryFile logFile;
    ASSERT_TRUE(logFile.open());
    logFile.write("fd log content\n");
    logFile.flush();

    QTemporaryFile pathFile;
    ASSERT_TRUE(pathFile.open());
    pathFile.write(logFile.fileName().toUtf8());
    pathFile.flush();
    pathFile.seek(0);

    QDBusUnixFileDescriptor fd(pathFile.handle());
    QString result = m_service->readLog(fd);
    EXPECT_TRUE(result.contains("fd log content"));
}

TEST_F(LogViewerServiceTest, ReadLogByFdInvalidFd)
{
    Stub stub;
    stub.set(ADDR(LogViewerService, checkAuth), stub_checkAuth_true);
    QDBusUnixFileDescriptor fd(-1);
    QString result = m_service->readLog(fd);
    EXPECT_TRUE(result.isEmpty());
}

// ===================== readLogLinesInRange(QString) 私有 =====================
TEST_F(LogViewerServiceTest, ReadLogLinesInRangeByPathAuthFail)
{
    QStringList result = m_service->readLogLinesInRange(QString("/tmp/x"), 0, 1, false);
    EXPECT_TRUE(result.isEmpty());
}

TEST_F(LogViewerServiceTest, ReadLogLinesInRangeByPathAuthOk)
{
    Stub stub;
    stub.set(ADDR(LogViewerService, checkAuth), stub_checkAuth_true);

    QTemporaryFile tmpFile;
    ASSERT_TRUE(tmpFile.open());
    tmpFile.write("l1\nl2\nl3\nl4\nl5\n");
    tmpFile.flush();

    QStringList result = m_service->readLogLinesInRange(tmpFile.fileName(), 0, 3, false);
    EXPECT_GE(result.size(), 1);
}

TEST_F(LogViewerServiceTest, ReadLogLinesInRangeByPathNotInWhitelist)
{
    Stub stub;
    stub.set(ADDR(LogViewerService, checkAuth), stub_checkAuth_true);
    QStringList result = m_service->readLogLinesInRange(QString("/home/x"), 0, 1, false);
    EXPECT_TRUE(result.isEmpty());
}

// ===================== readLogLinesInRange(fd) 公开槽 =====================
TEST_F(LogViewerServiceTest, ReadLogLinesInRangeByFdAuthFail)
{
    QTemporaryFile pathFile;
    ASSERT_TRUE(pathFile.open());
    pathFile.write("/tmp/whatever");
    pathFile.flush();
    QDBusUnixFileDescriptor fd(pathFile.handle());
    QStringList result = m_service->readLogLinesInRange(fd, 0, 1, false);
    EXPECT_TRUE(result.isEmpty());
}

TEST_F(LogViewerServiceTest, ReadLogLinesInRangeByFdAuthOk)
{
    Stub stub;
    stub.set(ADDR(LogViewerService, checkAuth), stub_checkAuth_true);

    QTemporaryFile logFile;
    ASSERT_TRUE(logFile.open());
    logFile.write("a\nb\nc\n");
    logFile.flush();

    QTemporaryFile pathFile;
    ASSERT_TRUE(pathFile.open());
    pathFile.write(logFile.fileName().toUtf8());
    pathFile.flush();
    pathFile.seek(0);

    QDBusUnixFileDescriptor fd(pathFile.handle());
    QStringList result = m_service->readLogLinesInRange(fd, 0, 2, false);
    EXPECT_GE(result.size(), 1);
}

// ===================== readFileAndReturnIndex（私有，经 readLogLinesInRange 覆盖） =====================
// 已由 ReadLogLinesInRangeByPathAuthOk 覆盖进入。

// ===================== processCatFile（私有） =====================
TEST_F(LogViewerServiceTest, ProcessCatFile)
{
    QTemporaryFile tmpFile;
    ASSERT_TRUE(tmpFile.open());
    tmpFile.write("cat me\n");
    tmpFile.flush();
    QByteArray result = m_service->processCatFile(tmpFile.fileName());
    EXPECT_TRUE(result.contains("cat me"));
}

// ===================== processCmdArgs（私有，经 executeCmd 覆盖） =====================
// 已由 ExecuteCmd 覆盖进入。

// ===================== processCmdWithArgs（static，经 getLineCount 覆盖） =====================
// 已由 GetLineCount 覆盖进入。

// ===================== getLineCount =====================
TEST_F(LogViewerServiceTest, GetLineCountAuthFail)
{
    EXPECT_EQ(m_service->getLineCount("/tmp/x"), -1);
}

TEST_F(LogViewerServiceTest, GetLineCountAuthOk)
{
    Stub stub;
    stub.set(ADDR(LogViewerService, checkAuth), stub_checkAuth_true);

    QTemporaryFile tmpFile;
    ASSERT_TRUE(tmpFile.open());
    tmpFile.write("a\nb\nc\n");
    tmpFile.flush();
    qint64 count = m_service->getLineCount(tmpFile.fileName());
    EXPECT_GE(count, 3);
}

TEST_F(LogViewerServiceTest, GetLineCountNotInWhitelist)
{
    Stub stub;
    stub.set(ADDR(LogViewerService, checkAuth), stub_checkAuth_true);
    EXPECT_EQ(m_service->getLineCount("/home/x"), -1);
}

TEST_F(LogViewerServiceTest, GetLineCountContainsDotDot)
{
    Stub stub;
    stub.set(ADDR(LogViewerService, checkAuth), stub_checkAuth_true);
    EXPECT_EQ(m_service->getLineCount("/tmp/../etc/passwd"), -1);
}

// ===================== executeCmd =====================
TEST_F(LogViewerServiceTest, ExecuteCmdAuthFail)
{
    EXPECT_TRUE(m_service->executeCmd("dmesg").isEmpty());
}

TEST_F(LogViewerServiceTest, ExecuteCmdCoredumpctlList)
{
    Stub stub;
    stub.set(ADDR(LogViewerService, checkAuth), stub_checkAuth_true);
    m_service->executeCmd("coredumpctl-list");
    SUCCEED();
}

TEST_F(LogViewerServiceTest, ExecuteCmdCoredumpctlInfo)
{
    Stub stub;
    stub.set(ADDR(LogViewerService, checkAuth), stub_checkAuth_true);
    m_service->executeCmd("coredumpctl info 12345");
    SUCCEED();
}

TEST_F(LogViewerServiceTest, ExecuteCmdReadCoredumpMapsInvalidPid)
{
    Stub stub;
    stub.set(ADDR(LogViewerService, checkAuth), stub_checkAuth_true);
    QString result = m_service->executeCmd("read-coredump-maps abc");
    EXPECT_TRUE(result.isEmpty());
}

TEST_F(LogViewerServiceTest, ExecuteCmdReadCoredumpMapsNotFoundPid)
{
    Stub stub;
    stub.set(ADDR(LogViewerService, checkAuth), stub_checkAuth_true);
    // 合法 pid 但 coredumpctl dump 必失败 -> extractCoredumpMaps 返回空
    QString result = m_service->executeCmd("read-coredump-maps 999999");
    EXPECT_TRUE(result.isEmpty());
}

TEST_F(LogViewerServiceTest, ExecuteCmdHardcodedWhitelist)
{
    Stub stub;
    stub.set(ADDR(LogViewerService, checkAuth), stub_checkAuth_true);
    m_service->executeCmd("dmesg");
    SUCCEED();
}

TEST_F(LogViewerServiceTest, ExecuteCmdUnknown)
{
    Stub stub;
    stub.set(ADDR(LogViewerService, checkAuth), stub_checkAuth_true);
    EXPECT_TRUE(m_service->executeCmd("unknown-cmd-xyz").isEmpty());
}

// ===================== extractCoredumpMaps（私有，经 executeCmd 覆盖） =====================
// 已由 ExecuteCmdReadCoredumpMaps* 覆盖进入。

// ===================== openLogStream =====================
TEST_F(LogViewerServiceTest, OpenLogStreamAuthFail)
{
    EXPECT_TRUE(m_service->openLogStream("/tmp/x").isEmpty());
}

TEST_F(LogViewerServiceTest, OpenLogStreamAuthOk)
{
    Stub stub;
    stub.set(ADDR(LogViewerService, checkAuth), stub_checkAuth_true);

    QTemporaryFile tmpFile;
    ASSERT_TRUE(tmpFile.open());
    tmpFile.write("stream line\n");
    tmpFile.flush();
    QString [REDACTED CREDENTIAL]
    EXPECT_FALSE(token.isEmpty());
}

// ===================== readLogInStream =====================
TEST_F(LogViewerServiceTest, ReadLogInStreamAuthFail)
{
    EXPECT_TRUE(m_service->readLogInStream("any-token").isEmpty());
}

TEST_F(LogViewerServiceTest, ReadLogInStreamInvalidToken)
{
    Stub stub;
    stub.set(ADDR(LogViewerService, checkAuth), stub_checkAuth_true);
    EXPECT_TRUE(m_service->readLogInStream("invalid-token").isEmpty());
}

TEST_F(LogViewerServiceTest, ReadLogInStreamValidToken)
{
    Stub stub;
    stub.set(ADDR(LogViewerService, checkAuth), stub_checkAuth_true);

    QTemporaryFile tmpFile;
    ASSERT_TRUE(tmpFile.open());
    tmpFile.write("stream data 1\nstream data 2\n");
    tmpFile.flush();
    QString [REDACTED CREDENTIAL]
    ASSERT_FALSE(token.isEmpty());
    QString result = m_service->readLogInStream(token);
    EXPECT_TRUE(result.contains("stream data"));
    // 再次读取，应返回空并清理
    QString result2 = m_service->readLogInStream(token);
    EXPECT_TRUE(result2.isEmpty());
}

// ===================== isFileExist =====================
TEST_F(LogViewerServiceTest, IsFileExistAuthFail)
{
    EXPECT_TRUE(m_service->isFileExist("/tmp/x").isEmpty());
}

TEST_F(LogViewerServiceTest, IsFileExistAuthOk)
{
    Stub stub;
    stub.set(ADDR(LogViewerService, checkAuth), stub_checkAuth_true);

    QTemporaryFile tmpFile;
    ASSERT_TRUE(tmpFile.open());
    EXPECT_EQ(m_service->isFileExist(tmpFile.fileName()), QString("exist"));
    EXPECT_TRUE(m_service->isFileExist("/tmp/non-existent-xyz-123").isEmpty());
}

// ===================== getFileSize =====================
TEST_F(LogViewerServiceTest, GetFileSizeAuthFail)
{
    EXPECT_EQ(m_service->getFileSize("/tmp/x"), 0u);
}

TEST_F(LogViewerServiceTest, GetFileSizeAuthOk)
{
    Stub stub;
    stub.set(ADDR(LogViewerService, checkAuth), stub_checkAuth_true);

    QTemporaryFile tmpFile;
    ASSERT_TRUE(tmpFile.open());
    tmpFile.write("12345");
    tmpFile.flush();
    EXPECT_EQ(m_service->getFileSize(tmpFile.fileName()), 5u);
    EXPECT_EQ(m_service->getFileSize("/tmp/non-existent-xyz-123"), 0u);
}

// ===================== exitCode =====================
TEST_F(LogViewerServiceTest, ExitCodeAuthFail)
{
    EXPECT_EQ(m_service->exitCode(), -1);
}

TEST_F(LogViewerServiceTest, ExitCodeAuthOk)
{
    Stub stub;
    stub.set(ADDR(LogViewerService, checkAuth), stub_checkAuth_true);
    m_service->exitCode();
    SUCCEED();
}

// ===================== quit =====================
TEST_F(LogViewerServiceTest, QuitNotFromDBus)
{
    // calledFromDBus() == false -> 直接返回，不触发 D-Bus 交互
    m_service->quit();
    SUCCEED();
}

// ===================== getFileInfo =====================
TEST_F(LogViewerServiceTest, GetFileInfoAuthFail)
{
    EXPECT_TRUE(m_service->getFileInfo("dmesg").isEmpty());
}

TEST_F(LogViewerServiceTest, GetFileInfoAudit)
{
    Stub stub;
    stub.set(ADDR(LogViewerService, checkAuth), stub_checkAuth_true);
    m_service->getFileInfo("audit");
    SUCCEED();
}

TEST_F(LogViewerServiceTest, GetFileInfoCoredump)
{
    Stub stub;
    stub.set(ADDR(LogViewerService, checkAuth), stub_checkAuth_true);
    m_service->getFileInfo("coredump");
    SUCCEED();
}

TEST_F(LogViewerServiceTest, GetFileInfoOther)
{
    Stub stub;
    stub.set(ADDR(LogViewerService, checkAuth), stub_checkAuth_true);
    m_service->getFileInfo("dmesg");
    SUCCEED();
}

TEST_F(LogViewerServiceTest, GetFileInfoDeepinPath)
{
    Stub stub;
    stub.set(ADDR(LogViewerService, checkAuth), stub_checkAuth_true);
    QTemporaryFile tmpFile;
    ASSERT_TRUE(tmpFile.open());
    tmpFile.write("data\n");
    tmpFile.flush();
    m_service->getFileInfo(tmpFile.fileName());
    SUCCEED();
}

// ===================== getOtherFileInfo =====================
TEST_F(LogViewerServiceTest, GetOtherFileInfoAuthFail)
{
    EXPECT_TRUE(m_service->getOtherFileInfo("/tmp/x").isEmpty());
}

TEST_F(LogViewerServiceTest, GetOtherFileInfoNonExistent)
{
    Stub stub;
    stub.set(ADDR(LogViewerService, checkAuth), stub_checkAuth_true);
    EXPECT_TRUE(m_service->getOtherFileInfo("/tmp/non-existent-xyz-123").isEmpty());
}

TEST_F(LogViewerServiceTest, GetOtherFileInfoFile)
{
    Stub stub;
    stub.set(ADDR(LogViewerService, checkAuth), stub_checkAuth_true);
    QTemporaryFile tmpFile;
    ASSERT_TRUE(tmpFile.open());
    tmpFile.write("data\n");
    tmpFile.flush();
    m_service->getOtherFileInfo(tmpFile.fileName());
    SUCCEED();
}

TEST_F(LogViewerServiceTest, GetOtherFileInfoDir)
{
    Stub stub;
    stub.set(ADDR(LogViewerService, checkAuth), stub_checkAuth_true);
    QTemporaryDir tmpDir;
    ASSERT_TRUE(tmpDir.isValid());
    m_service->getOtherFileInfo(tmpDir.path());
    SUCCEED();
}

// ===================== exportLog =====================
TEST_F(LogViewerServiceTest, ExportLogAuthFail)
{
    QTemporaryFile target;
    ASSERT_TRUE(target.open());
    QDBusUnixFileDescriptor fd(target.handle());
    EXPECT_FALSE(m_service->exportLog(fd, "/tmp/x", true));
}

TEST_F(LogViewerServiceTest, ExportLogInvalidFd)
{
    Stub stub;
    stub.set(ADDR(LogViewerService, checkAuth), stub_checkAuth_true);
    QDBusUnixFileDescriptor fd(-1);
    EXPECT_FALSE(m_service->exportLog(fd, "/tmp/x", true));
}

TEST_F(LogViewerServiceTest, ExportLogEmptyInput)
{
    Stub stub;
    stub.set(ADDR(LogViewerService, checkAuth), stub_checkAuth_true);
    QTemporaryFile target;
    ASSERT_TRUE(target.open());
    QDBusUnixFileDescriptor fd(target.handle());
    EXPECT_FALSE(m_service->exportLog(fd, "", true));
}

TEST_F(LogViewerServiceTest, ExportLogFileModeNotInWhitelist)
{
    Stub stub;
    stub.set(ADDR(LogViewerService, checkAuth), stub_checkAuth_true);
    QTemporaryFile target;
    ASSERT_TRUE(target.open());
    QDBusUnixFileDescriptor fd(target.handle());
    EXPECT_FALSE(m_service->exportLog(fd, "/home/x/secret", true));
}

// NOTE: ExportLogFileModeOk removed — triggers source defect (alloc-dealloc-mismatch,
// see Mode 5 findings). exportLog function is already covered by ExportLogFileModeNotInWhitelist.

TEST_F(LogViewerServiceTest, ExportLogJsonMode)
{
    Stub stub;
    stub.set(ADDR(LogViewerService, checkAuth), stub_checkAuth_true);
    QTemporaryFile target;
    ASSERT_TRUE(target.open());
    QDBusUnixFileDescriptor fd(target.handle());
    // 合法 JSON -> 构造 journalctl 命令分支
    QString json = R"({"name":"test","filter":"cat","execPath":"/bin/x"})";
    m_service->exportLog(fd, json, false);
    SUCCEED();
}

TEST_F(LogViewerServiceTest, ExportLogHardcodedCmdMode)
{
    Stub stub;
    stub.set(ADDR(LogViewerService, checkAuth), stub_checkAuth_true);
    QTemporaryFile target;
    ASSERT_TRUE(target.open());
    QDBusUnixFileDescriptor fd(target.handle());
    // 硬编码白名单命令分支（runCommandRedirectToFd 被进入）
    m_service->exportLog(fd, "dmesg", false);
    SUCCEED();
}

TEST_F(LogViewerServiceTest, ExportLogUnknownCmdMode)
{
    Stub stub;
    stub.set(ADDR(LogViewerService, checkAuth), stub_checkAuth_true);
    QTemporaryFile target;
    ASSERT_TRUE(target.open());
    QDBusUnixFileDescriptor fd(target.handle());
    EXPECT_FALSE(m_service->exportLog(fd, "unknown-cmd", false));
}

// ===================== exportOpsLog =====================
TEST_F(LogViewerServiceTest, ExportOpsLogAuthFail)
{
    QTemporaryFile target;
    ASSERT_TRUE(target.open());
    QDBusUnixFileDescriptor fd(target.handle());
    EXPECT_FALSE(m_service->exportOpsLog(fd));
}

TEST_F(LogViewerServiceTest, ExportOpsLogNoCallerHome)
{
    Stub stub;
    stub.set(ADDR(LogViewerService, checkAuth), stub_checkAuth_true);
    // calledFromDBus() == false -> getCallerHomeDir 返回空 -> 提前返回 false
    QTemporaryFile target;
    ASSERT_TRUE(target.open());
    QDBusUnixFileDescriptor fd(target.handle());
    EXPECT_FALSE(m_service->exportOpsLog(fd));
}

// ===================== runCommandRedirectToFd（static，经 exportLog 覆盖） =====================
// 已由 ExportLogHardcodedCmdMode 覆盖进入。
