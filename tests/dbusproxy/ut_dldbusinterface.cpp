// SPDX-FileCopyrightText: 2019 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "dldbusinterface.h"
#include "qtcompat.h"

#include <stub.h>

#include <QDebug>
#include <QTemporaryFile>
#include <QDBusUnixFileDescriptor>

#include <gtest/gtest.h>

class DlDbusinterface_UT : public testing::Test
{
public:
    static void SetUpTestCase()
    {
        qDebug() << "SetUpTestCase" << ENDL;
    }
    static void TearDownTestCase()
    {
        qDebug() << "TearDownTestCase" << ENDL;
    }
    void SetUp()
    {
        m_interface = new DeepinLogviewerInterface("test", "test", QDBusConnection::systemBus());
        qDebug() << "SetUp" << ENDL;
    }
    void TearDown()
    {
        delete m_interface;
    }
    DeepinLogviewerInterface *m_interface;
};

// 构造/析构随 fixture 调用，此处显式验证 staticInterfaceName。
TEST_F(DlDbusinterface_UT, StaticInterfaceName)
{
    EXPECT_STREQ(DeepinLogviewerInterface::staticInterfaceName(), "com.deepin.logviewer");
}

TEST_F(DlDbusinterface_UT, exitCode)
{
    m_interface->exitCode();
}

TEST_F(DlDbusinterface_UT, getFileInfo)
{
    m_interface->getFileInfo("path", false);
}

TEST_F(DlDbusinterface_UT, getOtherFileInfo)
{
    m_interface->getOtherFileInfo("path", false);
}

TEST_F(DlDbusinterface_UT, quit)
{
    m_interface->quit();
}

TEST_F(DlDbusinterface_UT, readLogByFilePath)
{
    m_interface->readLog(QString("path"));
}

TEST_F(DlDbusinterface_UT, readLogByFd)
{
    QDBusUnixFileDescriptor fd(::fileno(stdout));
    m_interface->readLog(fd);
}

TEST_F(DlDbusinterface_UT, readLogLinesInRangeByFilePath)
{
    m_interface->readLogLinesInRange(QString("path"), 0, 10, false);
}

TEST_F(DlDbusinterface_UT, readLogLinesInRangeByFd)
{
    QDBusUnixFileDescriptor fd(::fileno(stdout));
    m_interface->readLogLinesInRange(fd, 0, 10, false);
}

TEST_F(DlDbusinterface_UT, openLogStream)
{
    m_interface->openLogStream("path");
}

TEST_F(DlDbusinterface_UT, readLogInStream)
{
    m_interface->readLogInStream("token");
}

TEST_F(DlDbusinterface_UT, isFileExist)
{
    m_interface->isFileExist("path");
}

TEST_F(DlDbusinterface_UT, getFileSize)
{
    m_interface->getFileSize("path");
}

TEST_F(DlDbusinterface_UT, getLineCount)
{
    m_interface->getLineCount("path");
}

TEST_F(DlDbusinterface_UT, executeCmd)
{
    m_interface->executeCmd("cmd");
}

TEST_F(DlDbusinterface_UT, exportOpsLog)
{
    QDBusUnixFileDescriptor fd(::fileno(stdout));
    m_interface->exportOpsLog(fd);
}

// exportLog 成功打开目标文件分支（fileFd >= 0）。
TEST_F(DlDbusinterface_UT, exportLogWritablePath)
{
    QTemporaryFile tmp;
    ASSERT_TRUE(tmp.open());
    m_interface->exportLog(tmp.fileName(), "path1", false);
}

// exportLog 目标文件打开失败分支（fileFd < 0）。
TEST_F(DlDbusinterface_UT, exportLogInvalidPath)
{
    m_interface->exportLog("/nonexistent_dir_ut_abcdef/out.log", "path1", false);
}
