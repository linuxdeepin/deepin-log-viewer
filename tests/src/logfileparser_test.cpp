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

#include <gtest/gtest.h>
#include <stub.h>

#include <QDebug>
#include <QMessageBox>
#include <QThreadPool>
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

//TEST(LogFileParser_parseByJournalBoot_UT, LogFileParser_parseByJournalBoot_UT)
//{
//    Stub *stub = new Stub;
//    stub->set(ADDR(QThreadPool, start), QThreadPool_start);
//    stub->set(ADDR(QThread, start), QThread_start);
//    LogFileParser *p = new LogFileParser(nullptr);
//    EXPECT_NE(p, nullptr);
//    p->parseByJournalBoot(QStringList());
//    p->deleteLater();
//}

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

TEST(LogFileParser_slot_journalFinished_UT, LogFileParser_slot_journalFinished_UT)
{
    LogFileParser *p = new LogFileParser(nullptr);
    EXPECT_NE(p, nullptr);
    p->slot_journalFinished();
    EXPECT_EQ(p->m_isJournalLoading, false);
    p->deleteLater();
}

TEST(LogFileParser_slot_journalBootFinished_UT, LogFileParser_slot_journalBootFinished_UT)
{
    LogFileParser *p = new LogFileParser(nullptr);
    EXPECT_NE(p, nullptr);
    p->slot_journalBootFinished();
    p->deleteLater();
}

TEST(LogFileParser_slot_applicationFinished_UT, LogFileParser_slot_applicationFinished_UT)
{
    LogFileParser *p = new LogFileParser(nullptr);
    EXPECT_NE(p, nullptr);
    QList<LOG_MSG_APPLICATOIN> appList;
    p->slot_applicationFinished(appList);
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
