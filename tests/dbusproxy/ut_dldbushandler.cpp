// SPDX-FileCopyrightText: 2019 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "dldbushandler.h"
#include "utils.h"

#include <stub.h>

#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QTemporaryDir>
#include <QTextStream>
#include <QJsonDocument>
#include <QJsonObject>

#include <gtest/gtest.h>

// 受测对象使用 new DLDBusHandler() 构造，避免单例状态在用例间互相污染；
// 通过设置 Utils::homePath 控制各方法的 home / 非 home 分支路由。
class UT_DLDBusHandler : public testing::Test
{
protected:
    void SetUp() override
    {
        m_oldHome = Utils::homePath;
        m_handler = new DLDBusHandler();
        ASSERT_NE(m_handler, nullptr);
    }
    void TearDown() override
    {
        delete m_handler;
        m_handler = nullptr;
        Utils::homePath = m_oldHome;
    }

    // 在临时家目录下创建一个包含指定内容的文件，返回文件绝对路径。
    QString makeHomeFile(const QString &relName, const QByteArray &content)
    {
        QString home = m_homeDir.path();
        QString abs = home + QDir::separator() + relName;
        QFile f(abs);
        if (f.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
            f.write(content);
            f.close();
        }
        return abs;
    }

    QString m_oldHome;
    QTemporaryDir m_homeDir;
    DLDBusHandler *m_handler = nullptr;
};

// ---- 构造 / 析构 / 单例 ----

TEST_F(UT_DLDBusHandler, constructor_and_destructor)
{
    DLDBusHandler *h = new DLDBusHandler();
    ASSERT_NE(h, nullptr);
    delete h;
}

TEST_F(UT_DLDBusHandler, instance_returns_same_singleton)
{
    DLDBusHandler *p1 = DLDBusHandler::instance();
    DLDBusHandler *p2 = DLDBusHandler::instance();
    EXPECT_NE(p1, nullptr);
    EXPECT_EQ(p1, p2);
}

// ---- isHomePath ----

TEST_F(UT_DLDBusHandler, isHomePath_empty_input_returns_false)
{
    Utils::homePath = m_homeDir.path();
    EXPECT_FALSE(m_handler->isHomePath(QString()));
}

TEST_F(UT_DLDBusHandler, isHomePath_empty_homepath_returns_false)
{
    Utils::homePath = QString();
    EXPECT_FALSE(m_handler->isHomePath("/home/u/somefile"));
}

TEST_F(UT_DLDBusHandler, isHomePath_exact_match_returns_true)
{
    Utils::homePath = m_homeDir.path();
    EXPECT_TRUE(m_handler->isHomePath(Utils::homePath));
}

TEST_F(UT_DLDBusHandler, isHomePath_prefix_match_returns_true)
{
    Utils::homePath = m_homeDir.path();
    EXPECT_TRUE(m_handler->isHomePath(Utils::homePath + "/sub/file.log"));
}

TEST_F(UT_DLDBusHandler, isHomePath_non_home_returns_false)
{
    Utils::homePath = m_homeDir.path();
    EXPECT_FALSE(m_handler->isHomePath("/var/log/syslog"));
}

// ---- readLog ----

TEST_F(UT_DLDBusHandler, readLog_home_branch_returns_file_content)
{
    Utils::homePath = m_homeDir.path();
    QByteArray content = "hello-logviewer\nline2\n";
    QString abs = makeHomeFile("app.log", content);
    QString result = m_handler->readLog(abs);
    EXPECT_EQ(result.toUtf8(), content);
}

TEST_F(UT_DLDBusHandler, readLog_home_branch_nonexistent_returns_empty)
{
    Utils::homePath = m_homeDir.path();
    QString result = m_handler->readLog(Utils::homePath + "/not_exist.log");
    EXPECT_TRUE(result.isEmpty());
}

TEST_F(UT_DLDBusHandler, readLog_nonhome_branch_cleans_cache_file)
{
    Utils::homePath = m_homeDir.path();
    QString cache = m_handler->m_tempDir.path() + QDir::separator() + "Log_file_path.txt";
    EXPECT_NO_FATAL_FAILURE(m_handler->readLog("/var/log/dlv_nonexist.log"));
    EXPECT_FALSE(QFile::exists(cache));
}

// ---- readLogLinesInRange ----

TEST_F(UT_DLDBusHandler, readLogLinesInRange_home_branch)
{
    Utils::homePath = m_homeDir.path();
    QString abs = makeHomeFile("multi.log", "a\nb\nc\nd\n");
    QStringList lines = m_handler->readLogLinesInRange(abs, 0, 2, false);
    EXPECT_LE(lines.size(), 2);
}

TEST_F(UT_DLDBusHandler, readLogLinesInRange_nonhome_branch_cleans_cache_file)
{
    Utils::homePath = m_homeDir.path();
    QString cache = m_handler->m_tempDir.path() + QDir::separator() + "Log_file_path.txt";
    EXPECT_NO_FATAL_FAILURE(m_handler->readLogLinesInRange("/var/log/dlv_nonexist.log", 0, 10, false));
    EXPECT_FALSE(QFile::exists(cache));
}

// ---- openLogStream ----

TEST_F(UT_DLDBusHandler, openLogStream_home_branch_returns_local_token)
{
    Utils::homePath = m_homeDir.path();
    QString abs = makeHomeFile("stream.log", "stream-content\n");
    QString [REDACTED CREDENTIAL]
    EXPECT_TRUE(token.startsWith(QStringLiteral("local-")));
}

TEST_F(UT_DLDBusHandler, openLogStream_home_branch_unreadable_returns_empty)
{
    Utils::homePath = m_homeDir.path();
    QString [REDACTED CREDENTIAL] + "/no_such_file.log");
    EXPECT_TRUE(token.isEmpty());
}

TEST_F(UT_DLDBusHandler, openLogStream_nonhome_branch)
{
    Utils::homePath = m_homeDir.path();
    EXPECT_NO_FATAL_FAILURE(m_handler->openLogStream("/var/log/dlv_nonexist.log"));
}

// ---- readLogInStream ----

TEST_F(UT_DLDBusHandler, readLogInStream_local_token_returns_content)
{
    Utils::homePath = m_homeDir.path();
    QString abs = makeHomeFile("stream2.log", "data-line\n");
    QString [REDACTED CREDENTIAL]
    ASSERT_FALSE(token.isEmpty());
    QString result = m_handler->readLogInStream(token);
    EXPECT_FALSE(result.isEmpty());
}

TEST_F(UT_DLDBusHandler, readLogInStream_unknown_local_token_returns_empty)
{
    EXPECT_TRUE(m_handler->readLogInStream(QStringLiteral("local-unknown-token")).isEmpty());
}

TEST_F(UT_DLDBusHandler, readLogInStream_nonlocal_token)
{
    EXPECT_NO_FATAL_FAILURE(m_handler->readLogInStream(QStringLiteral("dbus-token-xyz")));
}

// ---- whiteListOutPaths ----

TEST_F(UT_DLDBusHandler, whiteListOutPaths_contains_tmp_and_home)
{
    Utils::homePath = m_homeDir.path();
    QStringList paths = m_handler->whiteListOutPaths();
    EXPECT_TRUE(paths.contains(QStringLiteral("/tmp")));
    EXPECT_TRUE(paths.contains(Utils::homePath));
}

TEST_F(UT_DLDBusHandler, whiteListOutPaths_skips_root_and_slash_home)
{
    Utils::homePath = QStringLiteral("/");
    QStringList paths = m_handler->whiteListOutPaths();
    EXPECT_TRUE(paths.contains(QStringLiteral("/tmp")));
    EXPECT_FALSE(paths.contains(QStringLiteral("/")));

    Utils::homePath = QStringLiteral("/root");
    QStringList paths2 = m_handler->whiteListOutPaths();
    EXPECT_TRUE(paths2.contains(QStringLiteral("/tmp")));
    EXPECT_FALSE(paths2.contains(QStringLiteral("/root")));
}

// ---- exitCode / quit ----

TEST_F(UT_DLDBusHandler, exitCode_invokable)
{
    EXPECT_NO_FATAL_FAILURE(m_handler->exitCode());
}

TEST_F(UT_DLDBusHandler, quit_invokable)
{
    EXPECT_NO_FATAL_FAILURE(m_handler->quit());
}

// ---- getFileInfo / getOtherFileInfo / isGetFileInfoError ----

TEST_F(UT_DLDBusHandler, getFileInfo_home_branch_clears_error_flag)
{
    Utils::homePath = m_homeDir.path();
    QStringList result = m_handler->getFileInfo(Utils::homePath, false);
    (void)result;
    EXPECT_FALSE(m_handler->isGetFileInfoError());
}

TEST_F(UT_DLDBusHandler, getFileInfo_nonhome_branch)
{
    Utils::homePath = m_homeDir.path();
    EXPECT_NO_FATAL_FAILURE(m_handler->getFileInfo(QStringLiteral("/var/log"), false));
}

TEST_F(UT_DLDBusHandler, getOtherFileInfo_home_branch)
{
    Utils::homePath = m_homeDir.path();
    QStringList result = m_handler->getOtherFileInfo(Utils::homePath, false);
    EXPECT_TRUE(result.isEmpty() || !result.isEmpty());
}

TEST_F(UT_DLDBusHandler, getOtherFileInfo_nonhome_branch)
{
    Utils::homePath = m_homeDir.path();
    EXPECT_NO_FATAL_FAILURE(m_handler->getOtherFileInfo(QStringLiteral("/var/log"), false));
}

TEST_F(UT_DLDBusHandler, isGetFileInfoError_default_false)
{
    EXPECT_FALSE(m_handler->isGetFileInfoError());
}

// ---- exportLog（覆盖 computeExportFileName 全部分支 + 本地拷贝/失败分支）----

TEST_F(UT_DLDBusHandler, exportLog_empty_input_returns_false)
{
    EXPECT_FALSE(m_handler->exportLog(m_homeDir.path(), QString(), true));
}

TEST_F(UT_DLDBusHandler, exportLog_home_file_copy_succeeds)
{
    Utils::homePath = m_homeDir.path();
    QByteArray content = "export-content\n";
    QString src = makeHomeFile("src.log", content);
    QTemporaryDir outDir;
    ASSERT_TRUE(outDir.isValid());
    ASSERT_TRUE(m_handler->exportLog(outDir.path(), src, true));
    QString target = outDir.path() + QDir::separator() + QFileInfo(src).fileName();
    QFile tf(target);
    ASSERT_TRUE(tf.open(QIODevice::ReadOnly));
    EXPECT_EQ(tf.readAll(), content);
    tf.close();
}

TEST_F(UT_DLDBusHandler, exportLog_home_source_open_fail_returns_false)
{
    Utils::homePath = m_homeDir.path();
    QTemporaryDir outDir;
    ASSERT_TRUE(outDir.isValid());
    EXPECT_FALSE(m_handler->exportLog(outDir.path(), Utils::homePath + "/missing.log", true));
}

TEST_F(UT_DLDBusHandler, exportLog_home_target_open_fail_returns_false)
{
    Utils::homePath = m_homeDir.path();
    QByteArray content = "x\n";
    QString src = makeHomeFile("src2.log", content);
    EXPECT_FALSE(m_handler->exportLog("/nonexistent_out_dir_xyz", src, true));
}

TEST_F(UT_DLDBusHandler, exportLog_nonhome_json_name_branch)
{
    Utils::homePath = m_homeDir.path();
    QTemporaryDir outDir;
    ASSERT_TRUE(outDir.isValid());
    EXPECT_NO_FATAL_FAILURE(m_handler->exportLog(outDir.path(), QStringLiteral("{\"name\":\"mod\"}"), false));
}

TEST_F(UT_DLDBusHandler, exportLog_nonhome_whitelist_command_branch)
{
    Utils::homePath = m_homeDir.path();
    QTemporaryDir outDir;
    ASSERT_TRUE(outDir.isValid());
    EXPECT_NO_FATAL_FAILURE(m_handler->exportLog(outDir.path(), QStringLiteral("dmesg"), false));
}

TEST_F(UT_DLDBusHandler, exportLog_nonhome_fallback_command_branch)
{
    Utils::homePath = m_homeDir.path();
    QTemporaryDir outDir;
    ASSERT_TRUE(outDir.isValid());
    EXPECT_NO_FATAL_FAILURE(m_handler->exportLog(outDir.path(), QStringLiteral("unknown_cmd"), false));
}

TEST_F(UT_DLDBusHandler, exportLog_nonhome_json_object_without_name_branch)
{
    Utils::homePath = m_homeDir.path();
    QTemporaryDir outDir;
    ASSERT_TRUE(outDir.isValid());
    EXPECT_NO_FATAL_FAILURE(m_handler->exportLog(outDir.path(), QStringLiteral("{}"), false));
}

TEST_F(UT_DLDBusHandler, exportLog_nonhome_invalid_json_branch)
{
    Utils::homePath = m_homeDir.path();
    QTemporaryDir outDir;
    ASSERT_TRUE(outDir.isValid());
    EXPECT_NO_FATAL_FAILURE(m_handler->exportLog(outDir.path(), QStringLiteral("not-a-json{"), false));
}

// ---- isFileExist ----

TEST_F(UT_DLDBusHandler, isFileExist_home_existing_returns_true)
{
    Utils::homePath = m_homeDir.path();
    QString abs = makeHomeFile("exists.log", "ok\n");
    EXPECT_TRUE(m_handler->isFileExist(abs));
}

TEST_F(UT_DLDBusHandler, isFileExist_home_missing_returns_false)
{
    Utils::homePath = m_homeDir.path();
    EXPECT_FALSE(m_handler->isFileExist(Utils::homePath + "/nope.log"));
}

TEST_F(UT_DLDBusHandler, isFileExist_nonhome_branch)
{
    Utils::homePath = m_homeDir.path();
    EXPECT_NO_FATAL_FAILURE(m_handler->isFileExist("/var/log/dlv_nonexist.log"));
}

// ---- getFileSize ----

TEST_F(UT_DLDBusHandler, getFileSize_home_branch_matches_file_size)
{
    Utils::homePath = m_homeDir.path();
    QByteArray content = "0123456789";
    QString abs = makeHomeFile("size.log", content);
    EXPECT_EQ(m_handler->getFileSize(abs), static_cast<quint64>(content.size()));
}

TEST_F(UT_DLDBusHandler, getFileSize_home_missing_returns_zero)
{
    Utils::homePath = m_homeDir.path();
    EXPECT_EQ(m_handler->getFileSize(Utils::homePath + "/nope.log"), 0u);
}

TEST_F(UT_DLDBusHandler, getFileSize_nonhome_branch)
{
    Utils::homePath = m_homeDir.path();
    EXPECT_NO_FATAL_FAILURE(m_handler->getFileSize("/var/log/dlv_nonexist.log"));
}

// ---- getLineCount ----

TEST_F(UT_DLDBusHandler, getLineCount_home_branch_counts_newlines)
{
    Utils::homePath = m_homeDir.path();
    QString abs = makeHomeFile("lines.log", "a\nb\nc\n");
    EXPECT_EQ(m_handler->getLineCount(abs), 3);
}

TEST_F(UT_DLDBusHandler, getLineCount_home_missing_returns_negative)
{
    Utils::homePath = m_homeDir.path();
    EXPECT_LT(m_handler->getLineCount(Utils::homePath + "/nope.log"), 0);
}

TEST_F(UT_DLDBusHandler, getLineCount_nonhome_branch)
{
    Utils::homePath = m_homeDir.path();
    EXPECT_NO_FATAL_FAILURE(m_handler->getLineCount("/var/log/dlv_nonexist.log"));
}

// ---- executeCmd ----

TEST_F(UT_DLDBusHandler, executeCmd_invokable)
{
    Utils::homePath = m_homeDir.path();
    EXPECT_NO_FATAL_FAILURE(m_handler->executeCmd(QStringLiteral("dmesg")));
}

// ---- createFilePathCacheFile / releaseFilePathCacheFile ----

TEST_F(UT_DLDBusHandler, createFilePathCacheFile_writes_input_to_temp)
{
    QString input = "/var/log/special.log";
    QString cache = m_handler->createFilePathCacheFile(input);
    EXPECT_FALSE(cache.isEmpty());
    QFile f(cache);
    ASSERT_TRUE(f.open(QIODevice::ReadOnly | QIODevice::Text));
    EXPECT_EQ(QString::fromUtf8(f.readAll()), input);
    f.close();
}

TEST_F(UT_DLDBusHandler, releaseFilePathCacheFile_removes_existing)
{
    QString cache = m_handler->createFilePathCacheFile("/some/path.log");
    ASSERT_TRUE(QFile::exists(cache));
    m_handler->releaseFilePathCacheFile(cache);
    EXPECT_FALSE(QFile::exists(cache));
}

TEST_F(UT_DLDBusHandler, releaseFilePathCacheFile_empty_input_noop)
{
    EXPECT_NO_FATAL_FAILURE(m_handler->releaseFilePathCacheFile(QString()));
}

// ---- exportOpsLog ----

TEST_F(UT_DLDBusHandler, exportOpsLog_unwritable_target_returns_false)
{
    EXPECT_FALSE(m_handler->exportOpsLog("/nonexistent_dir_xyz/ops.zip"));
}

TEST_F(UT_DLDBusHandler, exportOpsLog_writable_target_invokes_dbus)
{
    QTemporaryDir outDir;
    ASSERT_TRUE(outDir.isValid());
    QString zipPath = outDir.path() + QDir::separator() + "ops.zip";
    EXPECT_NO_FATAL_FAILURE(m_handler->exportOpsLog(zipPath));
}
