/*
* Copyright (C) 2019 ~ 2020 UnionTech Software Technology Co.,Ltd
*
* Author:     zyc <zyc@uniontech.com>
* Maintainer:  zyc <zyc@uniontech.com>
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
#include "logfileparser.h"
#include "stuballthread.h"
#include "logauththread.h"
#include "structdef.h"
#include "sharedmemorymanager.h"
#include "wtmpparse.h"

#include <gtest/gtest.h>
#include <stub.h>

#include <QDebug>
#include <QMessageBox>
#include <QThreadPool>
#include <QDateTime>

bool stub_isAttached001()
{
    return true;
}

bool stub_Logexists001()
{
    return true;
}

void stub_Logstart001(const QString &program, const QStringList &arguments, QIODevice::OpenMode mode)
{
}

bool stub_LogwaitForFinished001(int msecs)
{
    return true;
}

void stub_LogsetRunnableTag001(ShareMemoryInfo iShareInfo)
{
}

QByteArray stub_LogreadAllStandardOutput001()
{
    return "2020-11-24 01:57:24 startup archives install \n2020-11-24 01:57:24 install base-passwd:amd64 <none> 3.5.46\n            2021-01-09, 17:04:10.721 [Debug  ] [                                                         0] onTermGetFocus 2";
}

QByteArray stub_LogreadAllStandardError001()
{
    return "noerror";
}

void stub_wtmp_close001(void)
{
}

QString stub_toString001(QStringView format)
{
    return "20190120";
}
TEST(LogFileParser_Constructor_UT, LogFileParser_Constructor_UT)
{
    LogFileParser *p = new LogFileParser(nullptr);
    EXPECT_NE(p, nullptr);
    p->deleteLater();
}

TEST(LogFileParser_Destructor_UT, LogFileParser_Destructor_UT)
{
    LogFileParser *p = new LogFileParser(nullptr);
    EXPECT_NE(p, nullptr);
    p->~LogFileParser();
}

//TEST(LogFileParser_parseByJournal_UT, LogFileParser_parseByJournal_UT)
//{
//    Stub *stub = new Stub;
//    stub->set(ADDR(QThreadPool, start), QThreadPool_start);
//    stub->set(ADDR(QThread, start), QThread_start);
//    LogFileParser *p = new LogFileParser(nullptr);
//    EXPECT_NE(p, nullptr);
//    p->parseByJournal(QStringList());
//    p->deleteLater();
//}

TEST(LogFileParser_parseByJournalBoot_UT, LogFileParser_parseByJournalBoot_UT)
{
    Stub *stub = new Stub;
    stub->set(ADDR(QThreadPool, start), QThreadPool_start);
    stub->set(ADDR(QThread, start), QThread_start);
    LogFileParser *p = new LogFileParser(nullptr);
    EXPECT_NE(p, nullptr);
    p->parseByJournalBoot(QStringList() << "test");
    p->deleteLater();
}

//TEST(LogFileParser_parseByDpkg_UT, LogFileParser_parseByDpkg_UT)
//{
//    Stub *stub = new Stub;
//    stub->set(ADDR(QThreadPool, start), QThreadPool_start);
//    stub->set(ADDR(QThread, start), QThread_start);
//    LogFileParser *p = new LogFileParser(nullptr);
//    EXPECT_NE(p, nullptr);
//    DKPG_FILTERS filter;
//    p->parseByDpkg(filter);
//    p->deleteLater();
//}

//TEST(LogFileParser_parseByXlog_UT, LogFileParser_parseByXlog_UT)
//{
//    Stub *stub = new Stub;
//    stub->set(ADDR(QThreadPool, start), QThreadPool_start);
//    stub->set(ADDR(QThread, start), QThread_start);
//    LogFileParser *p = new LogFileParser(nullptr);
//    EXPECT_NE(p, nullptr);
//    XORG_FILTERS filter;
//    p->parseByXlog(filter);
//    p->deleteLater();
//}

//TEST(LogFileParser_parseByNormal, LogFileParser_parseByXlog_UT)
//{
//    Stub *stub = new Stub;
//    stub->set(ADDR(QThreadPool, start), QThreadPool_start);
//    stub->set(ADDR(QThread, start), QThread_start);
//    LogFileParser *p = new LogFileParser(nullptr);
//    EXPECT_NE(p, nullptr);
//    NORMAL_FILTERS filter;
//    QList<LOG_MSG_NORMAL> nList;
//    p->parseByNormal(nList, filter);
//    p->deleteLater();
//}

//TEST(LogFileParser_parseByKwin_UT, LogFileParser_parseByKwin_UT)
//{
//    Stub *stub = new Stub;
//    stub->set(ADDR(QThreadPool, start), QThreadPool_start);
//    stub->set(ADDR(QThread, start), QThread_start);
//    LogFileParser *p = new LogFileParser(nullptr);
//    EXPECT_NE(p, nullptr);
//    KWIN_FILTERS filter;
//    p->parseByKwin(filter);
//    p->deleteLater();
//}
void LogFileParser_parseByBoot_UT_QThreadPool_start(QRunnable *runnable, int priority = 0)
{
    qDebug() << "LogFileParser_parseByBoot_UT_QThreadPool_start--";
}
//TEST(LogFileParser_parseByBoot_UT, LogFileParser_parseByBoot_UT)
//{
//    LogFileParser *p = new LogFileParser(nullptr);
//    EXPECT_NE(p, nullptr);
//    Stub *stub = new Stub;
//    stub->set(ADDR(QThreadPool, start), QThreadPool_start);
//    stub->set(ADDR(QThread, start), QThread_start);
//    p->parseByBoot();
//    p->deleteLater();
//}

//TEST(LogFileParser_parseByKern_UT, LogFileParser_parseByKern_UT)
//{
//    LogFileParser *p = new LogFileParser(nullptr);
//    EXPECT_NE(p, nullptr);
//    KERN_FILTERS filter;
//    Stub *stub = new Stub;
//    stub->set(ADDR(QThreadPool, start), QThreadPool_start);
//    stub->set(ADDR(QThread, start), QThread_start);
//    p->parseByKern(filter);
//    p->deleteLater();
//}

//TEST(LogFileParser_parseByApp_UT, LogFileParser_parseByApp_UT)
//{
//    Stub *stub = new Stub;
//    stub->set(ADDR(QThreadPool, start), QThreadPool_start);
//    stub->set(ADDR(QThread, start), QThread_start);
//    LogFileParser *p = new LogFileParser(nullptr);
//    EXPECT_NE(p, nullptr);
//    APP_FILTERS filter;
//    p->parseByApp(filter);
//    p->deleteLater();
//}

//TEST(LogFileParser_createFile_UT, LogFileParser_createFile_UT)
//{
//    Stub *stub = new Stub;
//    stub->set(ADDR(QThreadPool, start), QThreadPool_start);
//    stub->set(ADDR(QThread, start), QThread_start);
//    LogFileParser *p = new LogFileParser(nullptr);
//    EXPECT_NE(p, nullptr);
//    p->createFile("", 0);
//    p->deleteLater();
//}

TEST(LogFileParser_stopAllLoad_UT, LogFileParser_stopAllLoad_UT)
{
    LogFileParser *p = new LogFileParser(nullptr);
    EXPECT_NE(p, nullptr);
    p->stopAllLoad();
    p->deleteLater();
}

TEST(LogFileParser_quitLogAuththread_UT, LogFileParser_quitLogAuththread_UT_001)
{
    LogFileParser *p = new LogFileParser(nullptr);
    EXPECT_NE(p, nullptr);
    p->quitLogAuththread(nullptr);
    p->deleteLater();
}

TEST(LogFileParser_quitLogAuththread_UT, LogFileParser_quitLogAuththread_UT_002)
{
    LogFileParser *p = new LogFileParser(nullptr);
    EXPECT_NE(p, nullptr);
    QThread *thread = new QThread;
    thread->start();
    p->quitLogAuththread(thread);
    EXPECT_EQ(thread->isRunning(), false);
    thread->deleteLater();
    p->deleteLater();
}

QMessageBox::StandardButton LogFileParser_slog_proccessError_UT_DMessageBox_information(QWidget *parent, const QString &title,
                                                                                        const QString &text, QMessageBox::StandardButtons buttons = QMessageBox::Ok,
                                                                                        QMessageBox::StandardButton defaultButton = QMessageBox::NoButton)
{
    qDebug() << "info-------";
    return QMessageBox::NoButton;
}
TEST(LogFileParser_slog_proccessError_UT, LogFileParser_slog_proccessError_UT)
{
    LogFileParser *p = new LogFileParser(nullptr);
    EXPECT_NE(p, nullptr);
    typedef QMessageBox::StandardButton (*fptr)(QWidget *,
                                                const QString &, const QString &, QMessageBox::StandardButtons,
                                                QMessageBox::StandardButton);
    fptr p_func = (fptr)(&QMessageBox::information);
    Stub stub;
    stub.set(p_func, LogFileParser_slog_proccessError_UT_DMessageBox_information);
    p->slog_proccessError(QString("testError"));
    p->deleteLater();
}

class LogFileParser_UT : public testing::Test
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
        m_parser = new LogFileParser();
        qDebug() << "SetUp" << endl;
    }
    void TearDown() //TEST跑完之后会执行TearDown
    {
        delete m_parser;
    }
    LogFileParser *m_parser;
};

TEST_F(LogFileParser_UT, sLogFileParser_UT001)
{
    Stub stub;
    stub.set(ADDR(SharedMemoryManager, isAttached), stub_isAttached001);
    stub.set((bool (QFile::*)() const)ADDR(QFile, exists), stub_Logexists001);
    stub.set((void (QProcess::*)(const QString &, const QStringList &, QIODevice::OpenMode))ADDR(QProcess, start), stub_Logstart001);
    stub.set((QString(QDateTime::*)(QStringView) const)ADDR(QDateTime, toString), stub_toString001);
    stub.set(ADDR(QProcess, waitForFinished), stub_LogwaitForFinished001);
    stub.set(ADDR(QProcess, readAllStandardOutput), stub_LogreadAllStandardOutput001);
    stub.set(ADDR(QProcess, readAllStandardError), stub_LogreadAllStandardError001);
    stub.set(ADDR(SharedMemoryManager, setRunnableTag), stub_LogsetRunnableTag001);
    stub.set(wtmp_close, stub_wtmp_close001);

    struct KWIN_FILTERS fitler = {"test"};
    m_parser->parseByKwin(fitler);
}
