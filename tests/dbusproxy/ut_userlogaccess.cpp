// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "userlogaccess.h"
#include "utils.h"

#include <stub.h>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QTemporaryDir>
#include <QTemporaryFile>
#include <QTextStream>
#include <QProcess>
#include <QStandardPaths>
#include <gtest/gtest.h>

#include <QList>

namespace {
// 创建一个临时目录并在其中写入指定内容文件，返回目录路径
QString makeDirWithFile(const QString &parent, const QString &dirName,
                        const QString &fileName, const QByteArray &content)
{
    QString dirPath = parent + QDir::separator() + dirName;
    QDir().mkpath(dirPath);
    QFile f(dirPath + QDir::separator() + fileName);
    if (f.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        f.write(content);
        f.close();
    }
    return dirPath;
}
}

// instance / 构造函数
TEST(UT_UserLogAccess_instance, UT_UserLogAccess_instance_001)
{
    UserLogAccess *p1 = UserLogAccess::instance();
    ASSERT_TRUE(p1);
    UserLogAccess *p2 = UserLogAccess::instance();
    EXPECT_EQ(p1, p2);
}

// readLog：空路径
TEST(UT_UserLogAccess_readLog, UT_UserLogAccess_readLog_empty)
{
    EXPECT_TRUE(UserLogAccess::instance()->readLog(QString()).isEmpty());
}

// readLog：不存在的文件
TEST(UT_UserLogAccess_readLog, UT_UserLogAccess_readLog_notExist)
{
    EXPECT_TRUE(UserLogAccess::instance()->readLog("/no/such/file/xyz.log").isEmpty());
}

// readLog：正常文件 + 0x00 替换为 0x20
TEST(UT_UserLogAccess_readLog, UT_UserLogAccess_readLog_normal)
{
    QTemporaryFile tmp;
    tmp.open();
    QByteArray raw("hello\x00world\n", 12);
    tmp.write(raw);
    tmp.close();

    QString result = UserLogAccess::instance()->readLog(tmp.fileName());
    EXPECT_FALSE(result.isEmpty());
    EXPECT_FALSE(result.contains(QChar(0x00)));
    EXPECT_TRUE(result.contains(QChar(0x20)));
}

// isFileExist
TEST(UT_UserLogAccess_isFileExist, UT_UserLogAccess_isFileExist_001)
{
    QTemporaryFile tmp;
    tmp.open();
    EXPECT_TRUE(UserLogAccess::instance()->isFileExist(tmp.fileName()));
    EXPECT_FALSE(UserLogAccess::instance()->isFileExist("/no/such/file/abc"));
}

// getFileSize
TEST(UT_UserLogAccess_getFileSize, UT_UserLogAccess_getFileSize_001)
{
    QTemporaryFile tmp;
    tmp.open();
    tmp.write("1234567890");
    tmp.close();
    EXPECT_EQ(UserLogAccess::instance()->getFileSize(tmp.fileName()), 10u);
    EXPECT_EQ(UserLogAccess::instance()->getFileSize("/no/such/file/size"), 0u);
}

// getLineCount：正常 / 打不开 / 空文件 / 无结尾换行
TEST(UT_UserLogAccess_getLineCount, UT_UserLogAccess_getLineCount_001)
{
    QTemporaryFile tmp;
    tmp.open();
    tmp.write("a\nb\nc\n");
    tmp.close();
    EXPECT_EQ(UserLogAccess::instance()->getLineCount(tmp.fileName()), 3);

    // 有1个换行符，计1行；补计仅在0行时触发
    QTemporaryFile tmp2;
    tmp2.open();
    tmp2.write("x\ny");
    tmp2.close();
    EXPECT_EQ(UserLogAccess::instance()->getLineCount(tmp2.fileName()), 1);

    EXPECT_EQ(UserLogAccess::instance()->getLineCount("/no/such/file/lines"), -1);
}

// readLogLinesInRange：空路径
TEST(UT_UserLogAccess_readLogLinesInRange, UT_UserLogAccess_readLogLinesInRange_empty)
{
    QStringList r = UserLogAccess::instance()->readLogLinesInRange(QString(), 0, 10, false);
    EXPECT_TRUE(r.isEmpty());
}

// readLogLinesInRange：打不开的文件
TEST(UT_UserLogAccess_readLogLinesInRange, UT_UserLogAccess_readLogLinesInRange_notExist)
{
    QStringList r = UserLogAccess::instance()->readLogLinesInRange("/no/such/file/range", 0, 10, false);
    EXPECT_TRUE(r.isEmpty());
}

// readLogLinesInRange：正向读取
TEST(UT_UserLogAccess_readLogLinesInRange, UT_UserLogAccess_readLogLinesInRange_forward)
{
    QTemporaryFile tmp;
    tmp.open();
    tmp.write("l0\nl1\nl2\nl3\nl4\n");
    tmp.close();

    QStringList r = UserLogAccess::instance()->readLogLinesInRange(tmp.fileName(), 0, 2, false);
    ASSERT_EQ(r.size(), 2);
    EXPECT_EQ(r[0], QStringLiteral("l0"));
    EXPECT_EQ(r[1], QStringLiteral("l1"));
}

// readLogLinesInRange：反向读取
TEST(UT_UserLogAccess_readLogLinesInRange, UT_UserLogAccess_readLogLinesInRange_reverse)
{
    QTemporaryFile tmp;
    tmp.open();
    tmp.write("l0\nl1\nl2\nl3\nl4\n");
    tmp.close();

    QStringList r = UserLogAccess::instance()->readLogLinesInRange(tmp.fileName(), 0, 3, true);
    EXPECT_LE(r.size(), 3);
}

// readLogLinesInRange：缓存命中（同一文件第二次调用）
TEST(UT_UserLogAccess_readLogLinesInRange, UT_UserLogAccess_readLogLinesInRange_cacheHit)
{
    QTemporaryFile tmp;
    tmp.open();
    tmp.write("c0\nc1\nc2\n");
    tmp.close();

    QStringList r1 = UserLogAccess::instance()->readLogLinesInRange(tmp.fileName(), 0, 2, false);
    QStringList r2 = UserLogAccess::instance()->readLogLinesInRange(tmp.fileName(), 0, 2, false);
    EXPECT_EQ(r1.size(), 2);
    EXPECT_EQ(r2.size(), 2);
}

// openLogStream：正常文件返回 token
TEST(UT_UserLogAccess_openLogStream, UT_UserLogAccess_openLogStream_normal)
{
    QTemporaryFile tmp;
    tmp.open();
    tmp.write("stream line 1\nstream line 2\n");
    tmp.close();

    QString [REDACTED CREDENTIAL]
    EXPECT_FALSE(token.isEmpty());
    EXPECT_TRUE(token.startsWith(QStringLiteral("local-")));
}

// openLogStream：读取失败返回空 token
TEST(UT_UserLogAccess_openLogStream, UT_UserLogAccess_openLogStream_empty)
{
    QString [REDACTED CREDENTIAL]
    EXPECT_TRUE(token.isEmpty());
}

// readLogInStream：无效 token
TEST(UT_UserLogAccess_readLogInStream, UT_UserLogAccess_readLogInStream_invalidToken)
{
    EXPECT_TRUE(UserLogAccess::instance()->readLogInStream("invalid-token-xxx").isEmpty());
}

// readLogInStream：有效 token 读取内容，读尽后移除
TEST(UT_UserLogAccess_readLogInStream, UT_UserLogAccess_readLogInStream_valid)
{
    QTemporaryFile tmp;
    tmp.open();
    tmp.write("aaa\nbbb\n");
    tmp.close();

    QString [REDACTED CREDENTIAL]
    ASSERT_FALSE(token.isEmpty());

    QString first = UserLogAccess::instance()->readLogInStream(token);
    EXPECT_FALSE(first.isEmpty());

    // 读尽后再读，返回空并移除 token
    QString second;
    for (int i = 0; i < 10; ++i) {
        second = UserLogAccess::instance()->readLogInStream(token);
        if (second.isEmpty())
            break;
    }
    EXPECT_TRUE(second.isEmpty());
}

// getFileInfo：空路径
TEST(UT_UserLogAccess_getFileInfo, UT_UserLogAccess_getFileInfo_empty)
{
    EXPECT_TRUE(UserLogAccess::instance()->getFileInfo(QString()).isEmpty());
}

// getFileInfo：deepin 分支（文件）
TEST(UT_UserLogAccess_getFileInfo, UT_UserLogAccess_getFileInfo_deepinFile)
{
    QTemporaryDir base;
    QString dirPath = makeDirWithFile(base.path(), "deepin-ut-dir", "deepin-ut-dir.log", "app log\n");
    QString target = dirPath + QDir::separator() + "deepin-ut-dir.log";

    QStringList r = UserLogAccess::instance()->getFileInfo(target, false);
    EXPECT_FALSE(r.isEmpty());
}

// getFileInfo：deepin 分支（目录）
TEST(UT_UserLogAccess_getFileInfo, UT_UserLogAccess_getFileInfo_deepinDir)
{
    QTemporaryDir base;
    QString dirPath = makeDirWithFile(base.path(), "deepin-ut-dir2", "deepin-ut-dir2.log", "x\n");

    QStringList r = UserLogAccess::instance()->getFileInfo(dirPath, false);
    EXPECT_FALSE(r.isEmpty());
}

// getFileInfo：非 deepin 分支（文件）
TEST(UT_UserLogAccess_getFileInfo, UT_UserLogAccess_getFileInfo_otherFile)
{
    QTemporaryDir base;
    QString dirPath = makeDirWithFile(base.path(), "otherdir", "otherfile.txt", "data\n");
    QString target = dirPath + QDir::separator() + "otherfile.txt";

    QStringList r = UserLogAccess::instance()->getFileInfo(target, false);
    EXPECT_FALSE(r.isEmpty());
}

// getFileInfo：非 deepin 分支（目录）
TEST(UT_UserLogAccess_getFileInfo, UT_UserLogAccess_getFileInfo_otherDir)
{
    QTemporaryDir base;
    QString dirPath = makeDirWithFile(base.path(), "plaindir", "plaindir.log", "y\n");

    QStringList r = UserLogAccess::instance()->getFileInfo(dirPath, false);
    EXPECT_FALSE(r.isEmpty());
}

// getFileInfo：deepin 分支但路径既非文件也非目录
TEST(UT_UserLogAccess_getFileInfo, UT_UserLogAccess_getFileInfo_deepinNotExist)
{
    QStringList r = UserLogAccess::instance()->getFileInfo("/tmp/deepin-not-exist-xyz/notexist", false);
    EXPECT_TRUE(r.isEmpty());
}

// getOtherFileInfo：路径不存在
TEST(UT_UserLogAccess_getOtherFileInfo, UT_UserLogAccess_getOtherFileInfo_notExist)
{
    EXPECT_TRUE(UserLogAccess::instance()->getOtherFileInfo("/no/such/other/path").isEmpty());
}

// getOtherFileInfo：文件
TEST(UT_UserLogAccess_getOtherFileInfo, UT_UserLogAccess_getOtherFileInfo_file)
{
    QTemporaryDir base;
    QString dirPath = makeDirWithFile(base.path(), "odir", "ofile.txt", "z\n");
    QString target = dirPath + QDir::separator() + "ofile.txt";

    QStringList r = UserLogAccess::instance()->getOtherFileInfo(target, false);
    EXPECT_FALSE(r.isEmpty());
}

// getOtherFileInfo：目录
TEST(UT_UserLogAccess_getOtherFileInfo, UT_UserLogAccess_getOtherFileInfo_dir)
{
    QTemporaryDir base;
    QString dirPath = makeDirWithFile(base.path(), "odir2", "ofile2.txt", "w\n");

    QStringList r = UserLogAccess::instance()->getOtherFileInfo(dirPath, false);
    EXPECT_FALSE(r.isEmpty());
}

// getFileInfo：.gz 解压分支（unzip=true）
TEST(UT_UserLogAccess_getFileInfo, UT_UserLogAccess_getFileInfo_gz)
{
    if (QStandardPaths::findExecutable("gunzip").isEmpty() ||
        QStandardPaths::findExecutable("gzip").isEmpty()) {
        GTEST_SKIP() << "gzip/gunzip not available";
    }
    QTemporaryDir base;
    QString dirPath = makeDirWithFile(base.path(), "deepin-gz-dir", "deepin-gz-dir.txt", "gz content\n");
    // 压缩得到 .gz
    QString gzFile = dirPath + QDir::separator() + "deepin-gz-dir.txt.gz";
    QProcess::execute("gzip", QStringList() << (dirPath + QDir::separator() + "deepin-gz-dir.txt"));
    ASSERT_TRUE(QFile::exists(gzFile));

    QStringList r = UserLogAccess::instance()->getFileInfo(gzFile, true);
    EXPECT_FALSE(r.isEmpty());
}

// ---- 私有方法（-fno-access-control 可直接调用）----

// createExtractSubdir
TEST(UT_UserLogAccess_createExtractSubdir, UT_UserLogAccess_createExtractSubdir_001)
{
    QString subdir = UserLogAccess::instance()->createExtractSubdir();
    EXPECT_FALSE(subdir.isEmpty());
    EXPECT_TRUE(QDir(subdir).exists());
}

// removeDirFiles
TEST(UT_UserLogAccess_removeDirFiles, UT_UserLogAccess_removeDirFiles_001)
{
    QTemporaryDir tmp;
    QFile f1(tmp.path() + QDir::separator() + "a.txt");
    f1.open(QIODevice::WriteOnly);
    f1.write("a");
    f1.close();
    QFile f2(tmp.path() + QDir::separator() + "b.txt");
    f2.open(QIODevice::WriteOnly);
    f2.write("b");
    f2.close();

    UserLogAccess::instance()->removeDirFiles(tmp.path());
    EXPECT_EQ(QDir(tmp.path()).entryList(QDir::Files).size(), 0);
}

// unzipToTempFile
TEST(UT_UserLogAccess_unzipToTempFile, UT_UserLogAccess_unzipToTempFile_001)
{
    if (QStandardPaths::findExecutable("gzip").isEmpty() ||
        QStandardPaths::findExecutable("gunzip").isEmpty()) {
        GTEST_SKIP() << "gzip/gunzip not available";
    }
    QTemporaryFile src;
    src.open();
    src.write("unzip me\n");
    src.close();
    QString gzPath = src.fileName() + ".gz";
    QProcess::execute("gzip", QStringList() << src.fileName());
    ASSERT_TRUE(QFile::exists(gzPath));

    QString extractDir = UserLogAccess::instance()->createExtractSubdir();
    QString tmpl = extractDir + QDir::separator() + "Log_extract_XXXXXX.txt";
    QString out = UserLogAccess::instance()->unzipToTempFile(gzPath, tmpl);
    EXPECT_FALSE(out.isEmpty());
    EXPECT_TRUE(QFile::exists(out));
}

// unzipToTempFile：临时文件模板打开失败（目录不存在）
TEST(UT_UserLogAccess_unzipToTempFile, UT_UserLogAccess_unzipToTempFile_badTemplate)
{
    QString out = UserLogAccess::instance()->unzipToTempFile("/no/such.src", "/nonexistent/dir/XXXXXX");
    EXPECT_TRUE(out.isEmpty());
}

// readFileAndReturnIndex：正向，空缓存
TEST(UT_UserLogAccess_readFileAndReturnIndex, UT_UserLogAccess_readFileAndReturnIndex_forward)
{
    QTemporaryFile tmp;
    tmp.open();
    tmp.write("r0\nr1\nr2\nr3\n");
    tmp.close();

    QList<uint64_t> indexes;
    qint64 off = UserLogAccess::instance()->readFileAndReturnIndex(tmp.fileName(), 1, indexes, false);
    EXPECT_GE(off, 0);
    EXPECT_FALSE(indexes.isEmpty());
}

// readFileAndReturnIndex：反向
TEST(UT_UserLogAccess_readFileAndReturnIndex, UT_UserLogAccess_readFileAndReturnIndex_reverse)
{
    QTemporaryFile tmp;
    tmp.open();
    tmp.write("r0\nr1\nr2\nr3\n");
    tmp.close();

    QList<uint64_t> indexes;
    qint64 off = UserLogAccess::instance()->readFileAndReturnIndex(tmp.fileName(), 0, indexes, true);
    EXPECT_FALSE(indexes.isEmpty());
}

// readFileAndReturnIndex：文件打不开
TEST(UT_UserLogAccess_readFileAndReturnIndex, UT_UserLogAccess_readFileAndReturnIndex_notExist)
{
    QList<uint64_t> indexes;
    qint64 off = UserLogAccess::instance()->readFileAndReturnIndex("/no/such/index/file", 0, indexes, false);
    EXPECT_EQ(off, -1);
}

// readFileAndReturnIndex：缓存命中分支
TEST(UT_UserLogAccess_readFileAndReturnIndex, UT_UserLogAccess_readFileAndReturnIndex_cacheHit)
{
    QTemporaryFile tmp;
    tmp.open();
    tmp.write("r0\nr1\nr2\nr3\n");
    tmp.close();

    QList<uint64_t> indexes;
    UserLogAccess::instance()->readFileAndReturnIndex(tmp.fileName(), 1, indexes, false);
    // 第二次：缓存非空，startLine 在范围内、非反向 → 走缓存命中分支
    qint64 off = UserLogAccess::instance()->readFileAndReturnIndex(tmp.fileName(), 0, indexes, false);
    EXPECT_GE(off, 0);
}

// 析构函数：通过 delete 触发 deleting destructor (D0)
TEST(UT_UserLogAccess_destructor, UT_UserLogAccess_destructor_delete)
{
    // -fno-access-control 允许直接调用私有构造函数
    auto *obj = new UserLogAccess(nullptr);
    delete obj;
    SUCCEED();
}
