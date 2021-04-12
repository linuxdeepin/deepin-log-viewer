/*
* Copyright (C) 2019 ~ 2020 UnionTech Software Technology Co.,Ltd
*
* Author:     linxun <linxun@uniontech.com>
* Maintainer:  linxun <linxun@uniontech.com>
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* any later version.
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "logauththread.h"
#include "structdef.h"
#include "sharedmemorymanager.h"
#include "wtmpparse.h"
#include "dldbushandler.h"

#include <stub.h>

#include <QDebug>
#include <QDateTime>

#include <gtest/gtest.h>
bool stub_isAttached()
{
    return true;
}

bool stub_Logexists()
{
    return true;
}

void stub_Logstart(const QString &program, const QStringList &arguments, QIODevice::OpenMode mode)
{
}

bool stub_LogwaitForFinished(int msecs)
{
    return true;
}

void stub_LogsetRunnableTag(ShareMemoryInfo iShareInfo)
{
}

QByteArray stub_LogreadAllStandardOutput()
{
    return "2020-11-24 01:57:24 startup archives install \n2020-11-24 01:57:24 install base-passwd:amd64 <none> 3.5.46\n            2021-01-09, 17:04:10.721 [Debug  ] [                                                         0] onTermGetFocus 2";
}

QByteArray stub_LogreadAllStandardError()
{
    return "noerror";
}

void stub_wtmp_close(void)
{
}

QString stub_toString(QStringView format)
{
    return "20190120";
}

void stub_setProcessChannelMode(QProcess::ProcessChannelMode mode)
{
}

int stub_exitCode()
{
    return 0;
}

QString stub_readLog(const QString &filePath)
{
    Q_UNUSED(filePath);
    return "2021-04-06 13:29:32 install code:amd64 <none> 1.55.0-1617120720";
}

class LogAuthThread_UT : public testing::Test
{
public:
    //添加日志
    static void SetUpTestCase()
    {
        qDebug() << "SetUpTestCase" << endl;
    }
    static void TearDownTestCase()
    {
        qDebug() << "TearDownTestCase" << endl;
    }
    void SetUp() //TEST跑之前会执行SetUp
    {
        m_logAuthThread = LogAuthThread::instance();
        qDebug() << "SetUp" << endl;
    }
    void TearDown() //TEST跑完之后会执行TearDown
    {
    }
    LogAuthThread *m_logAuthThread;
};

TEST_F(LogAuthThread_UT, LogAuthThread_UT001)
{
    Stub stub;
    stub.set(ADDR(SharedMemoryManager, isAttached), stub_isAttached);
    stub.set((bool (QFile::*)() const)ADDR(QFile, exists), stub_Logexists);
    stub.set((void (QProcess::*)(const QString &, const QStringList &, QIODevice::OpenMode))ADDR(QProcess, start), stub_Logstart);
    stub.set((QString(QDateTime::*)(QStringView) const)ADDR(QDateTime, toString), stub_toString);
    stub.set(ADDR(QProcess, waitForFinished), stub_LogwaitForFinished);
    stub.set(ADDR(QProcess, readAllStandardOutput), stub_LogreadAllStandardOutput);
    stub.set(ADDR(QProcess, readAllStandardError), stub_LogreadAllStandardError);
    stub.set(ADDR(SharedMemoryManager, setRunnableTag), stub_LogsetRunnableTag);
    stub.set(wtmp_close, stub_wtmp_close);
    stub.set(ADDR(QProcess, setProcessChannelMode), stub_setProcessChannelMode);
    stub.set(ADDR(QProcess, exitCode), stub_exitCode);
    stub.set(ADDR(DLDBusHandler, readLog), stub_readLog);

    m_logAuthThread->m_process.reset(new QProcess);
    m_logAuthThread->m_isStopProccess = true;
    m_logAuthThread->m_type = LOG_FLAG::KERN;
    m_logAuthThread->m_FilePath = QStringList() << "/var/log/kern.log";
    m_logAuthThread->run();
    m_logAuthThread->m_canRun = true;
    m_logAuthThread->m_type = LOG_FLAG::BOOT;
    m_logAuthThread->m_FilePath = QStringList() << "/var/log/boot.log";
    m_logAuthThread->handleBoot();
    m_logAuthThread->m_FilePath = QStringList() << "/test";
    m_logAuthThread->handleBoot();
    m_logAuthThread->m_type = LOG_FLAG::DPKG;
    m_logAuthThread->m_FilePath = QStringList() << "/var/log/dpkg.log";
    m_logAuthThread->handleDkpg();
    m_logAuthThread->m_FilePath = QStringList() << "/test";
    m_logAuthThread->handleDkpg();
    m_logAuthThread->m_type = LOG_FLAG::XORG;
    m_logAuthThread->handleXorg();
    m_logAuthThread->m_type = LOG_FLAG::Normal;
    m_logAuthThread->handleNormal();
    m_logAuthThread->m_type = LOG_FLAG::Kwin;
    m_logAuthThread->handleKwin();

    KWIN_FILTERS kwin;
    m_logAuthThread->setFileterParam(kwin);
    KERN_FILTERS kern;
    m_logAuthThread->setFileterParam(kern);
    DKPG_FILTERS dpkg;
    m_logAuthThread->setFileterParam(dpkg);
    XORG_FILTERS xorg;
    m_logAuthThread->setFileterParam(xorg);
    NORMAL_FILTERS normal;
    m_logAuthThread->setFileterParam(normal);

    m_logAuthThread->thread_count = 1;
    EXPECT_EQ(m_logAuthThread->getIndex(), 1);
    delete LogAuthThread::instance();
}

//TEST_F(LogAuthThread_UT, LogAuthThread_UT002)
//{
//    m_logAuthThread->m_isStopProccess = false;
//}
