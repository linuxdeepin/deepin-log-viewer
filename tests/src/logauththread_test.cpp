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
    Q_UNUSED(program);
    Q_UNUSED(arguments);
    Q_UNUSED(mode);
}

bool stub_LogwaitForFinished(int msecs)
{
    Q_UNUSED(msecs);
    return true;
}

void stub_LogsetRunnableTag(ShareMemoryInfo iShareInfo)
{
    Q_UNUSED(iShareInfo);
}

QByteArray stub_LogreadAllStandardOutput()
{
    return "2020-11-24 01:57:24 startup archives install \n2020-11-24 01:57:24 install base-passwd:amd64 <none> 3.5.46\n            2021-01-09, 17:04:10.721 [Debug  ] [                                                         0] onTermGetFocus 2";
}

QByteArray dmesgLogreadAllStandardOutput()
{
    return "[101619.805280] snd_hda_codec_hdmi hdaudioC1D0: hda_codec_cleanup_stream: NID=0x8";
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
    Q_UNUSED(format);
    return "20190120";
}

void stub_setProcessChannelMode(QProcess::ProcessChannelMode mode)
{
    Q_UNUSED(mode);
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

QString stub_xorgReadLog(const QString &filePath)
{
    Q_UNUSED(filePath);
    return "[  9.576 ] (II) Loading /usr/lib/xorg/modules/drivers/fbdev_drv.so";
}

QString stub_dnfReadLog(const QString &filePath)
{
    Q_UNUSED(filePath);
    return "2021-05-21T02:08:36Z DEBUG 加载插件：builddep, changelog, \nconfig-manager, copr, debug, debuginfo-install, download, \ngenerate_completion_cache, needs-restarting, playground, repoclosure, repodiff, repograph, repomanage, reposync";
}

QByteArray fileReadLine(qint64 maxlen = 0)
{
    Q_UNUSED(maxlen);
    return "211111";
}

qint64 dnfToMSecsSinceEpoch()
{
    return 1999999999999999999;
}

int dmesgIndexIn(const QString &str, int offset = 0, QRegExp::CaretMode caretMode = QRegExp::CaretAtZero)
{
    Q_UNUSED(str);
    Q_UNUSED(offset);
    Q_UNUSED(caretMode);
    return 1;
}

QString &dmesgReplace(const QRegExp &rx, const QString &after)
{
    Q_UNUSED(rx);
    Q_UNUSED(after);
    static QString str = "[ 101619.805280 ] snd_hda_codec_hdmi hdaudioC1D0: hda_codec_cleanup_stream: NID=0x8";
    return str;
}

QStringList dmesgCapturedTexts()
{
    return QStringList() << "[ 101619.805280"
                         << "snd_hda_codec_"
                         << "ddd"
                         << "5555555"
                         << "test"
                         << "NID=0x8";
}

void handleFile()
{
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
    typedef bool (QFile::*fptr)() const;
    fptr A_foo = (fptr)(&QFile::exists); //获取虚函数地址
    stub.set(A_foo, stub_Logexists);

    stub.set(ADDR(SharedMemoryManager, isAttached), stub_isAttached);
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
    m_logAuthThread->m_type = LOG_FLAG::Normal;
    m_logAuthThread->handleNormal();
    m_logAuthThread->m_type = LOG_FLAG::Kwin;
    m_logAuthThread->handleKwin();
    m_logAuthThread->formatDateTime("04", "23", "14:25");
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
}

TEST_F(LogAuthThread_UT, handleXorg_UT)
{
    Stub stub;
    typedef bool (QFile::*fptr)() const;
    fptr A_foo = (fptr)(&QFile::exists); //获取虚函数地址
    stub.set(A_foo, stub_Logexists);

    stub.set(ADDR(SharedMemoryManager, isAttached), stub_isAttached);
    stub.set((void (QProcess::*)(const QString &, const QStringList &, QIODevice::OpenMode))ADDR(QProcess, start), stub_Logstart);
    stub.set((QString(QDateTime::*)(QStringView) const)ADDR(QDateTime, toString), stub_toString);
    stub.set(ADDR(QProcess, waitForFinished), stub_LogwaitForFinished);
    stub.set(ADDR(QProcess, readAllStandardOutput), stub_LogreadAllStandardOutput);
    stub.set(ADDR(QProcess, readAllStandardError), stub_LogreadAllStandardError);
    stub.set(ADDR(SharedMemoryManager, setRunnableTag), stub_LogsetRunnableTag);
    stub.set(wtmp_close, stub_wtmp_close);
    stub.set(ADDR(QProcess, setProcessChannelMode), stub_setProcessChannelMode);
    stub.set(ADDR(QProcess, exitCode), stub_exitCode);
    stub.set(ADDR(DLDBusHandler, readLog), stub_xorgReadLog);
    stub.set((QByteArray(QIODevice::*)(qint64))ADDR(QIODevice, readLine), fileReadLine);
    m_logAuthThread->m_FilePath = QStringList() << "/test"
                                                << "/xorg.old";
    m_logAuthThread->handleXorg();
}

TEST_F(LogAuthThread_UT, handleDnf_UT)
{
    Stub stub;
    typedef bool (QFile::*fptr)() const;
    fptr A_foo = (fptr)(&QFile::exists); //获取虚函数地址
    stub.set(A_foo, stub_Logexists);

    stub.set(ADDR(SharedMemoryManager, isAttached), stub_isAttached);
    stub.set((void (QProcess::*)(const QString &, const QStringList &, QIODevice::OpenMode))ADDR(QProcess, start), stub_Logstart);
    stub.set((QString(QDateTime::*)(QStringView) const)ADDR(QDateTime, toString), stub_toString);
    stub.set(ADDR(QProcess, waitForFinished), stub_LogwaitForFinished);
    stub.set(ADDR(QProcess, readAllStandardOutput), stub_LogreadAllStandardOutput);
    stub.set(ADDR(QProcess, readAllStandardError), stub_LogreadAllStandardError);
    stub.set(ADDR(SharedMemoryManager, setRunnableTag), stub_LogsetRunnableTag);
    stub.set(wtmp_close, stub_wtmp_close);
    stub.set(ADDR(QProcess, setProcessChannelMode), stub_setProcessChannelMode);
    stub.set(ADDR(QProcess, exitCode), stub_exitCode);
    stub.set(ADDR(DLDBusHandler, readLog), stub_dnfReadLog);
    stub.set((QByteArray(QIODevice::*)(qint64))ADDR(QIODevice, readLine), fileReadLine);
    stub.set(ADDR(QDateTime, toMSecsSinceEpoch), dnfToMSecsSinceEpoch);
    m_logAuthThread->m_FilePath = QStringList() << "/test"
                                                << "/xorg.old";
    m_logAuthThread->m_dnfFilters.levelfilter = DNFPRIORITY::DNFLVALL;
    m_logAuthThread->handleDnf();
}

TEST_F(LogAuthThread_UT, handleDmesg_UT)
{
    Stub stub;
    typedef bool (QFile::*fptr)() const;
    fptr A_foo = (fptr)(&QFile::exists); //获取虚函数地址
    stub.set(A_foo, stub_Logexists);

    stub.set(ADDR(SharedMemoryManager, isAttached), stub_isAttached);
    stub.set((void (QProcess::*)(const QString &, const QStringList &, QIODevice::OpenMode))ADDR(QProcess, start), stub_Logstart);
    stub.set((QString(QDateTime::*)(QStringView) const)ADDR(QDateTime, toString), stub_toString);
    stub.set(ADDR(QProcess, waitForFinished), stub_LogwaitForFinished);
    stub.set(ADDR(QProcess, readAllStandardOutput), dmesgLogreadAllStandardOutput);
    stub.set(ADDR(QProcess, readAllStandardError), stub_LogreadAllStandardError);
    stub.set(ADDR(SharedMemoryManager, setRunnableTag), stub_LogsetRunnableTag);
    stub.set(wtmp_close, stub_wtmp_close);
    stub.set(ADDR(QProcess, setProcessChannelMode), stub_setProcessChannelMode);
    stub.set(ADDR(QProcess, exitCode), stub_exitCode);
    stub.set(ADDR(DLDBusHandler, readLog), stub_dnfReadLog);
    stub.set((QByteArray(QIODevice::*)(qint64))ADDR(QIODevice, readLine), fileReadLine);
    stub.set(ADDR(QDateTime, toMSecsSinceEpoch), dnfToMSecsSinceEpoch);
    stub.set(ADDR(QRegExp, indexIn), dmesgIndexIn);
    stub.set((QString & (QString::*)(const QRegExp &, const QString &)) ADDR(QString, replace), dmesgReplace);
    stub.set((QStringList(QRegExp::*)())ADDR(QRegExp, capturedTexts), dmesgCapturedTexts);
    m_logAuthThread->m_FilePath = QStringList() << "/test"
                                                << "/xorg.old";
    m_logAuthThread->handleDmesg();
}

TEST_F(LogAuthThread_UT, run_UT)
{
    Stub stub;
    stub.set(ADDR(LogAuthThread, handleDkpg), handleFile);
    stub.set(ADDR(LogAuthThread, handleNormal), handleFile);
    stub.set(ADDR(LogAuthThread, handleDnf), handleFile);
    stub.set(ADDR(LogAuthThread, handleDmesg), handleFile);
    m_logAuthThread->m_type = LOG_FLAG::DPKG;
    m_logAuthThread->run();
    m_logAuthThread->m_type = LOG_FLAG::Normal;
    m_logAuthThread->run();
    m_logAuthThread->m_type = LOG_FLAG::Dnf;
    m_logAuthThread->run();
    m_logAuthThread->m_type = LOG_FLAG::Dmesg;
    m_logAuthThread->run();
    m_logAuthThread->m_type = LOG_FLAG::NONE;
    m_logAuthThread->run();
}
