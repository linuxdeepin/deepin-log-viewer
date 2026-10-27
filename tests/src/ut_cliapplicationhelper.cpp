// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "cliapplicationhelper.h"

#include <gtest/gtest.h>

#include <QApplication>
#include <QLocalServer>
#include <QLocalSocket>
#include <QTest>

// 单实例辅助类单元测试
// 覆盖目标：application/cliapplicationhelper.cpp 中的全部可测函数
//   - CliApplicationHelper::instance()
//   - CliApplicationHelper::CliApplicationHelper() / ~CliApplicationHelper()
//   - CliApplicationHelper::setSingleInstance()（三种 SingleScope）
//   - CliApplicationHelper::staticCleanApplication()
//   - _CliApplicationHelper::helper() / clear() / defaultCreator()（通过公开接口间接覆盖）

// instance() 应返回非空指针，且重复调用返回同一实例（单例语义）
TEST(CliApplicationHelper_Instance_UT, Instance_ReturnsNonNull)
{
    CliApplicationHelper *p1 = CliApplicationHelper::instance();
    ASSERT_NE(p1, nullptr);

    CliApplicationHelper *p2 = CliApplicationHelper::instance();
    EXPECT_EQ(p1, p2);
}

// staticCleanApplication() 清理后再次 instance() 仍可正常获取实例，
// 间接覆盖 clear()、~CliApplicationHelper()、defaultCreator()、helper()
TEST(CliApplicationHelper_StaticCleanApplication_UT, StaticCleanApplication_ClearsAndRecreates)
{
    CliApplicationHelper *p1 = CliApplicationHelper::instance();
    ASSERT_NE(p1, nullptr);

    CliApplicationHelper::staticCleanApplication();

    // 清理后再次获取，应得到新的有效实例
    CliApplicationHelper *p2 = CliApplicationHelper::instance();
    EXPECT_NE(p2, nullptr);
}

// staticCleanApplication() 在未创建实例时也不应崩溃
TEST(CliApplicationHelper_StaticCleanApplication_UT, StaticCleanApplication_NoCrashWhenEmpty)
{
    // 先清理确保无残留
    CliApplicationHelper::staticCleanApplication();
    // 再次清理不应崩溃
    CliApplicationHelper::staticCleanApplication();
    EXPECT_TRUE(true);
}

// 直接构造/析构对象，覆盖构造函数（qAddPostRoutine）与析构函数（m_helper = nullptr）
TEST(CliApplicationHelper_Constructor_UT, Constructor_CreatesValidObject)
{
    CliApplicationHelper *p = new CliApplicationHelper();
    EXPECT_NE(p, nullptr);
    delete p;
}

// setSingleInstance —— UserScope 首次调用应成功（获取锁 + 监听）
TEST(CliApplicationHelper_SetSingleInstance_UT, SetSingleInstance_UserScope_ReturnsTrue)
{
    bool ret = CliApplicationHelper::setSingleInstance(
        QStringLiteral("ut_cli_helper_user"), CliApplicationHelper::UserScope);
    EXPECT_TRUE(ret);
}

// setSingleInstance —— GroupScope
TEST(CliApplicationHelper_SetSingleInstance_UT, SetSingleInstance_GroupScope_ReturnsTrue)
{
    bool ret = CliApplicationHelper::setSingleInstance(
        QStringLiteral("ut_cli_helper_group"), CliApplicationHelper::GroupScope);
    EXPECT_TRUE(ret);
}

// setSingleInstance —— WorldScope
TEST(CliApplicationHelper_SetSingleInstance_UT, SetSingleInstance_WorldScope_ReturnsTrue)
{
    bool ret = CliApplicationHelper::setSingleInstance(
        QStringLiteral("ut_cli_helper_world"), CliApplicationHelper::WorldScope);
    EXPECT_TRUE(ret);
}

// setSingleInstance —— 同一 key 再次调用（同进程内复用锁），应返回 true
TEST(CliApplicationHelper_SetSingleInstance_UT, SetSingleInstance_SameKeyTwice_ReturnsTrue)
{
    QString key = QStringLiteral("ut_cli_helper_samekey");
    bool first = CliApplicationHelper::setSingleInstance(key, CliApplicationHelper::UserScope);
    EXPECT_TRUE(first);

    bool second = CliApplicationHelper::setSingleInstance(key, CliApplicationHelper::UserScope);
    EXPECT_TRUE(second);
}

// waitTime 静态成员可读写，验证默认值合理性
TEST(CliApplicationHelper_Misc_UT, WaitTime_IsPositive)
{
    EXPECT_GT(CliApplicationHelper::waitTime, 0);
}

#include <QDataStream>
#include <QIODevice>

// 触发 setSingleInstance 中 newConnection/readyRead 两个 lambda（line 159/168）：
// 使用事件循环驱动（QTest::qWait 轮询），而非阻塞式 waitForReadyRead，
// 因为 server 端 QLocalSocket 的 flush() 需要事件循环才能将数据写入 socket。
TEST(CliApplicationHelper_SetSingleInstance_UT, SetSingleInstance_NewConnectionHandlerFires)
{
    QString key = QStringLiteral("ut_lambda_trigger");
    bool ret = CliApplicationHelper::setSingleInstance(key, CliApplicationHelper::UserScope);
    EXPECT_TRUE(ret);

    // UserScope on Linux: prefix + "<uid>_" + key
    QString socketKey = QStringLiteral("_d_deepin_logger_single_instance_%1_%2")
                            .arg(::getuid()).arg(key);

    QLocalSocket client;
    client.connectToServer(socketKey);
    ASSERT_TRUE(client.waitForConnected(3000));

    // 事件循环驱动：等待 server 端 newConnection lambda 触发并发送数据
    // QTest::qWait 运行事件循环，使 server 端 socket flush 生效
    for (int i = 0; i < 30 && client.bytesAvailable() < (int)sizeof(qint64); ++i) {
        QTest::qWait(100);
    }
    ASSERT_GT(client.bytesAvailable(), 0);

    QDataStream in(&client);
    qint8 version = 0;
    qint64 pid = -1;
    QStringList arguments;
    in >> version >> pid >> arguments;
    EXPECT_GE(pid, 0);

    // 回写数据触发 server 端 readyRead lambda（line 168）
    {
        QByteArray block;
        QDataStream out(&block, QIODevice::WriteOnly);
        out << (qint8)1 << (qint64)QCoreApplication::applicationPid()
            << QStringList{QStringLiteral("ut_lambda_arg")};
        client.write(block);
        client.flush();
    }

    // 事件循环驱动：等待 server 端 readyRead lambda 触发
    QTest::qWait(1000);

    client.disconnectFromServer();
    if (client.state() != QLocalSocket::UnconnectedState) {
        client.waitForDisconnected(1000);
    }
    SUCCEED();
}
