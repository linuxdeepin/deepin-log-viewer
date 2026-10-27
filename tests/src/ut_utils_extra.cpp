// SPDX-FileCopyrightText: 2019-2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "utils.h"
#include "logsettings.h"
#include "dbusproxy/dldbushandler.h"
#include "qtcompat.h"

#include <gtest/gtest.h>
#include <stub.h>

#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QTemporaryDir>
#include <QTemporaryFile>
#include <QTextStream>
#include <QProcess>

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#include <polkit-qt6-1/PolkitQt1/Authority>
#else
#include <polkit-qt5-1/PolkitQt1/Authority>
#endif

// ---------------------------------------------------------------------------
// getAppDataPath
// ---------------------------------------------------------------------------
TEST(Utils_getAppDataPath_UT, Utils_getAppDataPath_UT)
{
    QString path = Utils::getAppDataPath();
    EXPECT_FALSE(path.isEmpty());
}

// ---------------------------------------------------------------------------
// isTextFileType
// ---------------------------------------------------------------------------
TEST(Utils_isTextFileType_UT, Utils_isTextFileType_UT)
{
    QTemporaryFile tmpFile;
    ASSERT_TRUE(tmpFile.open());
    tmpFile.write("hello world");
    tmpFile.close();

    bool result = Utils::isTextFileType(tmpFile.fileName());
    EXPECT_TRUE(result);

    bool emptyResult = Utils::isTextFileType(QString("/nonexistent/path/to/file.bin"));
    EXPECT_FALSE(emptyResult);
}

// ---------------------------------------------------------------------------
// getReplaceColorStr
// ---------------------------------------------------------------------------
TEST(Utils_getReplaceColorStr_UT, Utils_getReplaceColorStr_UT)
{
    const char *input = "\x1B[31mRedText\x1B[0m\x002Normal";
    QString result = Utils::getReplaceColorStr(input);
    EXPECT_FALSE(result.contains("\x1B"));
    EXPECT_FALSE(result.contains("\x02"));
    EXPECT_FALSE(result.contains('\0'));
}

// ---------------------------------------------------------------------------
// sleep
// ---------------------------------------------------------------------------
TEST(Utils_sleep_UT, Utils_sleep_UT)
{
    bool result = Utils::sleep(1);
    EXPECT_EQ(result, true);
}

// ---------------------------------------------------------------------------
// mkMutiDir
// ---------------------------------------------------------------------------
TEST(Utils_mkMutiDir_UT, Utils_mkMutiDir_UT)
{
    QTemporaryDir tmpDir;
    ASSERT_TRUE(tmpDir.isValid());
    QString testPath = tmpDir.path() + "/a/b/c";
    QString result = Utils::mkMutiDir(testPath);
    EXPECT_TRUE(QDir(result).exists());

    // empty path returns empty
    QString emptyResult = Utils::mkMutiDir(QString(""));
    EXPECT_TRUE(emptyResult.isEmpty());
}

// ---------------------------------------------------------------------------
// fixDirOwnership
// ---------------------------------------------------------------------------
TEST(Utils_fixDirOwnership_UT, Utils_fixDirOwnership_UT)
{
    // Path not under homePath -> returns early
    Utils::fixDirOwnership(QString("/tmp/notunderhome"));
    // Path under homePath but getuid() == realUID -> returns early
    Utils::fixDirOwnership(Utils::homePath + "/test_fixdir_ownership");
    SUCCEED();
}

// ---------------------------------------------------------------------------
// checkAuthorization (stubbed)
// ---------------------------------------------------------------------------
PolkitQt1::Authority::Result stub_checkAuthorizationSync()
{
    return PolkitQt1::Authority::Yes;
}

TEST(Utils_checkAuthorization_UT, Utils_checkAuthorization_UT)
{
    Stub stub;
    stub.set(ADDR(PolkitQt1::Authority, checkAuthorizationSync), stub_checkAuthorizationSync);
    bool result = Utils::checkAuthorization("org.deepin.log.viewer.test", 1);
    EXPECT_EQ(result, true);
}

// ---------------------------------------------------------------------------
// osVersion
// ---------------------------------------------------------------------------
TEST(Utils_osVersion_UT, Utils_osVersion_UT)
{
    QString result = Utils::osVersion();
    // lsb_release may or may not return a version string; function coverage only
    SUCCEED();
}

// ---------------------------------------------------------------------------
// auditType
// ---------------------------------------------------------------------------
TEST(Utils_auditType_UT, Utils_auditType_UT)
{
    // Populate the static map for testing
    Utils::m_mapAuditType2EventType.clear();
    Utils::m_mapAuditType2EventType.insert("typeA", QStringList() << "event1" << "event2");
    Utils::m_mapAuditType2EventType.insert("typeB", QStringList() << "event3");

    EXPECT_EQ(Utils::auditType("event1"), QString("typeA"));
    EXPECT_EQ(Utils::auditType("event3"), QString("typeB"));
    EXPECT_EQ(Utils::auditType("nonexistent"), QString(""));

    // cleanup
    Utils::m_mapAuditType2EventType.clear();
}

// ---------------------------------------------------------------------------
// convertToMB
// ---------------------------------------------------------------------------
TEST(Utils_convertToMB_UT, Utils_convertToMB_UT)
{
    double r0 = Utils::convertToMB(0, 1024);
    EXPECT_DOUBLE_EQ(r0, 0.0);

    double r1 = Utils::convertToMB(512, 1024);
    EXPECT_GT(r1, 0.0);

    double r2 = Utils::convertToMB(2048, 1024);
    EXPECT_GT(r2, 0.0);

    // large value
    double r3 = Utils::convertToMB(1024ULL * 1024 * 1024 * 10, 1024);
    EXPECT_GT(r3, 0.0);
}

// ---------------------------------------------------------------------------
// getUserNamebyUID / getUserHomePathByUID / getCurrentUserName
// ---------------------------------------------------------------------------
TEST(Utils_getUserNamebyUID_UT, Utils_getUserNamebyUID_UT)
{
    uint uid = getuid();
    QString name = Utils::getUserNamebyUID(uid);
    EXPECT_FALSE(name.isEmpty());

    // invalid uid
    QString invalid = Utils::getUserNamebyUID(99999);
    EXPECT_TRUE(invalid.isEmpty());
}

TEST(Utils_getUserHomePathByUID_UT, Utils_getUserHomePathByUID_UT)
{
    uint uid = getuid();
    QString home = Utils::getUserHomePathByUID(uid);
    EXPECT_FALSE(home.isEmpty());

    // invalid uid
    QString invalid = Utils::getUserHomePathByUID(99999);
    EXPECT_TRUE(invalid.isEmpty());
}

TEST(Utils_getCurrentUserName_UT, Utils_getCurrentUserName_UT)
{
    QString name = Utils::getCurrentUserName();
    EXPECT_FALSE(name.isEmpty());
}

// ---------------------------------------------------------------------------
// isCoredumpctlExist
// ---------------------------------------------------------------------------
TEST(Utils_isCoredumpctlExist_UT, Utils_isCoredumpctlExist_UT)
{
    bool result = Utils::isCoredumpctlExist();
    SUCCEED();
}

// ---------------------------------------------------------------------------
// appName
// ---------------------------------------------------------------------------
TEST(Utils_appName_UT, Utils_appName_UT)
{
    // empty path
    EXPECT_EQ(Utils::appName(QString("")), QString(""));

    // simple name with extension, no slash
    EXPECT_EQ(Utils::appName(QString("test.txt")), QString("test"));

    // simple name without extension, no slash
    EXPECT_EQ(Utils::appName(QString("testapp")), QString("testapp"));

    // full path with extension
    EXPECT_EQ(Utils::appName(QString("/usr/bin/testapp")), QString("testapp"));

    // full path with extension
    EXPECT_EQ(Utils::appName(QString("/opt/app/myapp.deb")), QString("myapp"));

    // trailing slash
    EXPECT_EQ(Utils::appName(QString("/usr/bin/testapp/")), QString("testapp"));
}

// ---------------------------------------------------------------------------
// resetToNormalAuth
// ---------------------------------------------------------------------------
TEST(Utils_resetToNormalAuth_UT, Utils_resetToNormalAuth_UT)
{
    // empty path -> no-op
    Utils::resetToNormalAuth(QString(""));
    SUCCEED();
}

// ---------------------------------------------------------------------------
// countRepeatCoredumps (stubbed DLDBusHandler::executeCmd)
// ---------------------------------------------------------------------------
QString stub_countRepeatCoredumps_executeCmd(const QString &cmd)
{
    Q_UNUSED(cmd);
    return QString("Mon 2024-01-15 10:30:00 Z 1000 1000 5 1234 0 /usr/bin/testapp SIGSEGV\n"
                   "Mon 2024-01-14 09:20:00 Z 1000 1000 5 1235 0 /usr/bin/testapp SIGABRT\n"
                   "Tue 2024-01-16 11:00:00 Z 1000 1000 5 1236 0 /usr/bin/otherapp SIGSEGV");
}

TEST(Utils_countRepeatCoredumps_UT, Utils_countRepeatCoredumps_UT)
{
    Stub stub;
    stub.set(ADDR(DLDBusHandler, executeCmd), stub_countRepeatCoredumps_executeCmd);

    // no time filter
    QList<LOG_REPEAT_COREDUMP_INFO> result = Utils::countRepeatCoredumps();
    EXPECT_GE(result.size(), 1);

    // with time filter
    QList<LOG_REPEAT_COREDUMP_INFO> filtered = Utils::countRepeatCoredumps(0, 0);
    EXPECT_GE(filtered.size(), 0);
}

// ---------------------------------------------------------------------------
// getRepeatCoredumpExePaths
// ---------------------------------------------------------------------------
TEST(Utils_getRepeatCoredumpExePaths_UT, Utils_getRepeatCoredumpExePaths_UT)
{
    QStringList result = Utils::getRepeatCoredumpExePaths();
    SUCCEED();
}

// ---------------------------------------------------------------------------
// updateRepeatCoredumpExePaths
// ---------------------------------------------------------------------------
TEST(Utils_updateRepeatCoredumpExePaths_UT, Utils_updateRepeatCoredumpExePaths_UT)
{
    // empty list -> early return
    Utils::updateRepeatCoredumpExePaths();

    // with data that meets threshold
    QList<LOG_REPEAT_COREDUMP_INFO> infos;
    LOG_REPEAT_COREDUMP_INFO info;
    info.exePath = "/usr/bin/testapp";
    info.times = 5;
    info.repetitionRate = 0.9f;
    infos.append(info);
    Utils::updateRepeatCoredumpExePaths(infos);

    SUCCEED();
}

// ---------------------------------------------------------------------------
// processCmdWithArgs / executeCmd
// ---------------------------------------------------------------------------
TEST(Utils_processCmdWithArgs_UT, Utils_processCmdWithArgs_UT)
{
    QByteArray result = Utils::processCmdWithArgs("echo", QString(), QStringList() << "hello");
    EXPECT_TRUE(result.contains("hello"));
}

TEST(Utils_executeCmd_UT, Utils_executeCmd_UT)
{
    QByteArray result = Utils::executeCmd("echo", QStringList() << "world");
    EXPECT_TRUE(result.contains("world"));
}

// ---------------------------------------------------------------------------
// appendToFile
// ---------------------------------------------------------------------------
TEST(Utils_appendToFile_UT, Utils_appendToFile_UT)
{
    QTemporaryDir tmpDir;
    ASSERT_TRUE(tmpDir.isValid());
    QString filePath = tmpDir.path() + "/test_append.txt";

    Utils::appendToFile(filePath, QByteArray("line1\n"));
    Utils::appendToFile(filePath, QByteArray("line2\n"));

    QFile file(filePath);
    ASSERT_TRUE(file.open(QIODevice::ReadOnly));
    QString content = QString::fromUtf8(file.readAll());
    EXPECT_TRUE(content.contains("line1"));
    EXPECT_TRUE(content.contains("line2"));
}

// ---------------------------------------------------------------------------
// safeCpSkipSymlinks
// ---------------------------------------------------------------------------
TEST(Utils_safeCpSkipSymlinks_UT, Utils_safeCpSkipSymlinks_UT)
{
    QTemporaryDir tmpDir;
    ASSERT_TRUE(tmpDir.isValid());

    // create a source file
    QString srcFile = tmpDir.path() + "/src.txt";
    QFile f(srcFile);
    ASSERT_TRUE(f.open(QIODevice::WriteOnly));
    f.write("test content");
    f.close();

    QString destDir = tmpDir.path() + "/dest";
    QDir().mkpath(destDir);

    Utils::safeCpSkipSymlinks(QStringList() << srcFile, destDir + "/copied.txt", false);

    // empty sources
    Utils::safeCpSkipSymlinks(QStringList(), destDir + "/empty.txt", false);
    SUCCEED();
}

// ---------------------------------------------------------------------------
// getPhysicalInterfaces
// ---------------------------------------------------------------------------
TEST(Utils_getPhysicalInterfaces_UT, Utils_getPhysicalInterfaces_UT)
{
    QStringList result = Utils::getPhysicalInterfaces();
    SUCCEED();
}

// ---------------------------------------------------------------------------
// expandPathWithWildcardIterator
// ---------------------------------------------------------------------------
TEST(Utils_expandPathWithWildcardIterator_UT, Utils_expandPathWithWildcardIterator_UT)
{
    QTemporaryDir tmpDir;
    ASSERT_TRUE(tmpDir.isValid());

    // create some test files
    QFile f1(tmpDir.path() + "/test_a.txt");
    f1.open(QIODevice::WriteOnly);
    f1.close();
    QFile f2(tmpDir.path() + "/test_b.txt");
    f2.open(QIODevice::WriteOnly);
    f2.close();

    QStringList result = Utils::expandPathWithWildcardIterator(tmpDir.path() + "/test_*.txt");
    EXPECT_GE(result.size(), 2);
}

// ---------------------------------------------------------------------------
// exportUserPermissionAppLogs
// ---------------------------------------------------------------------------
TEST(Utils_exportUserPermissionAppLogs_UT, Utils_exportUserPermissionAppLogs_UT)
{
    QTemporaryDir tmpDir;
    ASSERT_TRUE(tmpDir.isValid());
    Utils::exportUserPermissionAppLogs(tmpDir.path(), tmpDir.path());
    SUCCEED();
}

// ---------------------------------------------------------------------------
// exportUserPermissionSystemLogs
// ---------------------------------------------------------------------------
TEST(Utils_exportUserPermissionSystemLogs_UT, Utils_exportUserPermissionSystemLogs_UT)
{
    QTemporaryDir tmpDir;
    ASSERT_TRUE(tmpDir.isValid());
    Utils::exportUserPermissionSystemLogs(tmpDir.path(), tmpDir.path());
    SUCCEED();
}

// ---------------------------------------------------------------------------
// exportUserPermissionKernelLogs
// ---------------------------------------------------------------------------
TEST(Utils_exportUserPermissionKernelLogs_UT, Utils_exportUserPermissionKernelLogs_UT)
{
    QTemporaryDir tmpDir;
    ASSERT_TRUE(tmpDir.isValid());
    Utils::exportUserPermissionKernelLogs(tmpDir.path());
    SUCCEED();
}

// ---------------------------------------------------------------------------
// exportUserPermissionDDELogs
// ---------------------------------------------------------------------------
TEST(Utils_exportUserPermissionDDELogs_UT, Utils_exportUserPermissionDDELogs_UT)
{
    QTemporaryDir tmpDir;
    ASSERT_TRUE(tmpDir.isValid());
    Utils::exportUserPermissionDDELogs(tmpDir.path(), tmpDir.path());
    SUCCEED();
}

// ---------------------------------------------------------------------------
// exportUserPermissionOpsLogs
// ---------------------------------------------------------------------------
TEST(Utils_exportUserPermissionOpsLogs_UT, Utils_exportUserPermissionOpsLogs_UT)
{
    QTemporaryDir tmpDir;
    ASSERT_TRUE(tmpDir.isValid());
    Utils::exportUserPermissionOpsLogs(tmpDir.path(), tmpDir.path());
    SUCCEED();
}

// ---------------------------------------------------------------------------
// LoggerRules (guarded by DTKCORE_CLASS_DConfigFile)
// ---------------------------------------------------------------------------
#ifdef DTKCORE_CLASS_DConfigFile
TEST(LoggerRules_UT, LoggerRules_constructor_destructor_UT)
{
    LoggerRules *lr = new LoggerRules();
    EXPECT_NE(lr, nullptr);
    lr->initLoggerRules();
    delete lr;
}

TEST(LoggerRules_UT, LoggerRules_initLoggerRules_UT)
{
    LoggerRules lr;
    lr.initLoggerRules();
    // rules should be set (possibly empty, but function called)
    QString rules = lr.rules();
    SUCCEED();
}

TEST(LoggerRules_UT, LoggerRules_valueChangedCallback_UT)
{
    // -fno-access-control lets us reach private m_config
    LoggerRules *lr = new LoggerRules();
    lr->initLoggerRules();
    // Emit valueChanged with matching key to trigger the lambda body
    QMetaObject::invokeMethod(lr->m_config, "valueChanged",
                              Qt::DirectConnection,
                              Q_ARG(QString, QString("log_rules")));
    // Emit with non-matching key to exercise the false branch
    QMetaObject::invokeMethod(lr->m_config, "valueChanged",
                              Qt::DirectConnection,
                              Q_ARG(QString, QString("other_key")));
    SUCCEED();
    delete lr;
}

TEST(LoggerRules_UT, LoggerRules_setRules_UT)
{
    LoggerRules *lr = new LoggerRules();
    lr->initLoggerRules();
    lr->setRules("*.debug=true;*.warning=false");
    EXPECT_TRUE(lr->rules().contains("*.debug=true"));
    EXPECT_TRUE(lr->rules().contains("*.warning=false"));
    delete lr;
}
#endif
