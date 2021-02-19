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
#include "logexportthread.h"
#include "structdef.h"

#include "stuballthread.h"

#include <gtest/gtest.h>
#include <stub.h>

#include <DApplication>
#include <DStandardItem>

#include <QDebug>
#include <QIcon>
TEST(LogExportThread_Constructor_UT, LogExportThread_Constructor_UT)
{
    bool iscomplete = true;
    LogExportThread *p = new LogExportThread(iscomplete, nullptr);
    EXPECT_NE(p, nullptr);
    p->deleteLater();
}

TEST(LogExportThread_Destructor_UT, LogExportThread_Destructor_UT)
{
    bool iscomplete = true;
    LogExportThread *p = new LogExportThread(iscomplete, nullptr);
    EXPECT_NE(p, nullptr);
    p->deleteLater();
}

TEST(LogExportThread_exportToTxtPublic_UT, LogExportThread_exportToTxtPublic_UT_001)
{
    bool iscomplete = true;
    LogExportThread *p = new LogExportThread(iscomplete, nullptr);
    EXPECT_NE(p, nullptr);
    p->exportToTxtPublic("aa", nullptr, XORG);
    EXPECT_EQ(p->m_fileName, "aa");
    EXPECT_EQ(p->m_pModel, nullptr);
    EXPECT_EQ(p->m_flag, XORG);
    EXPECT_EQ(p->m_runMode, LogExportThread::TxtModel);
    EXPECT_EQ(p->m_canRunning, true);
    p->deleteLater();
}

TEST(LogExportThread_exportToTxtPublic_UT, LogExportThread_exportToTxtPublic_UT_002)
{
    bool iscomplete = true;
    LogExportThread *p = new LogExportThread(iscomplete, nullptr);
    EXPECT_NE(p, nullptr);
    QList<LOG_MSG_JOURNAL> jList;
    LOG_MSG_JOURNAL info;
    info.msg = "testmsg";
    jList << info;
    QStringList labels;
    labels << "testlabel1";
    p->exportToTxtPublic("aa", jList, labels, JOURNAL);
    EXPECT_EQ(p->m_fileName, "aa");
    EXPECT_EQ(p->m_flag, JOURNAL);
    EXPECT_EQ(p->m_runMode, LogExportThread::TxtJOURNAL);
    EXPECT_EQ(p->m_labels, labels);
    EXPECT_EQ(p->m_canRunning, true);
    p->deleteLater();
}

TEST(LogExportThread_exportToTxtPublic_UT, LogExportThread_exportToTxtPublic_UT_003)
{
    bool iscomplete = true;
    LogExportThread *p = new LogExportThread(iscomplete, nullptr);
    EXPECT_NE(p, nullptr);
    QList<LOG_MSG_APPLICATOIN> jList;
    LOG_MSG_APPLICATOIN info;
    info.msg = "testmsg";
    jList << info;
    QStringList labels;
    labels << "testlabel1";
    QString appname = "deepin-log-viewer";
    p->exportToTxtPublic("aa", jList, labels, appname);
    EXPECT_EQ(p->m_fileName, "aa");
    EXPECT_EQ(p->m_appName, appname);
    EXPECT_EQ(p->m_runMode, LogExportThread::TxtAPP);
    EXPECT_EQ(p->m_labels, labels);
    EXPECT_EQ(p->m_canRunning, true);
    p->deleteLater();
}

TEST(LogExportThread_exportToTxtPublic_UT, LogExportThread_exportToTxtPublic_UT_004)
{
    bool iscomplete = true;
    LogExportThread *p = new LogExportThread(iscomplete, nullptr);
    EXPECT_NE(p, nullptr);
    QList<LOG_MSG_DPKG> jList;
    LOG_MSG_DPKG info;
    info.msg = "testmsg";
    jList << info;
    QStringList labels;
    labels << "testlabel1";
    p->exportToTxtPublic("aa", jList, labels);
    EXPECT_EQ(p->m_fileName, "aa");
    EXPECT_EQ(p->m_runMode, LogExportThread::TxtDPKG);
    EXPECT_EQ(p->m_labels, labels);
    EXPECT_EQ(p->m_canRunning, true);
    p->deleteLater();
}

TEST(LogExportThread_exportToTxtPublic_UT, LogExportThread_exportToTxtPublic_UT_005)
{
    bool iscomplete = true;
    LogExportThread *p = new LogExportThread(iscomplete, nullptr);
    EXPECT_NE(p, nullptr);
    QList<LOG_MSG_BOOT> jList;
    LOG_MSG_BOOT info;
    info.msg = "testmsg";
    jList << info;
    QStringList labels;
    labels << "testlabel1";
    p->exportToTxtPublic("aa", jList, labels);
    EXPECT_EQ(p->m_fileName, "aa");
    EXPECT_EQ(p->m_runMode, LogExportThread::TxtBOOT);
    EXPECT_EQ(p->m_labels, labels);
    EXPECT_EQ(p->m_canRunning, true);
    p->deleteLater();
}

TEST(LogExportThread_exportToTxtPublic_UT, LogExportThread_exportToTxtPublic_UT_006)
{
    bool iscomplete = true;
    LogExportThread *p = new LogExportThread(iscomplete, nullptr);
    EXPECT_NE(p, nullptr);
    QList<LOG_MSG_BOOT> jList;
    LOG_MSG_BOOT info;
    info.msg = "testmsg";
    jList << info;
    QStringList labels;
    labels << "testlabel1";
    p->exportToTxtPublic("aa", jList, labels);
    EXPECT_EQ(p->m_fileName, "aa");
    EXPECT_EQ(p->m_runMode, LogExportThread::TxtBOOT);
    EXPECT_EQ(p->m_labels, labels);
    EXPECT_EQ(p->m_canRunning, true);
    p->deleteLater();
}

TEST(LogExportThread_exportToTxtPublic_UT, LogExportThread_exportToTxtPublic_UT_007)
{
    bool iscomplete = true;
    LogExportThread *p = new LogExportThread(iscomplete, nullptr);
    EXPECT_NE(p, nullptr);
    QList<LOG_MSG_XORG> jList;
    LOG_MSG_XORG info;
    info.msg = "testmsg";
    jList << info;
    QStringList labels;
    labels << "testlabel1";
    p->exportToTxtPublic("aa", jList, labels);
    EXPECT_EQ(p->m_fileName, "aa");
    EXPECT_EQ(p->m_runMode, LogExportThread::TxtXORG);
    EXPECT_EQ(p->m_labels, labels);
    EXPECT_EQ(p->m_canRunning, true);
    p->deleteLater();
}

TEST(LogExportThread_exportToTxtPublic_UT, LogExportThread_exportToTxtPublic_UT_008)
{
    bool iscomplete = true;
    LogExportThread *p = new LogExportThread(iscomplete, nullptr);
    EXPECT_NE(p, nullptr);
    QList<LOG_MSG_NORMAL> jList;
    LOG_MSG_NORMAL info;
    info.msg = "testmsg";
    jList << info;
    QStringList labels;
    labels << "testlabel1";
    p->exportToTxtPublic("aa", jList, labels);
    EXPECT_EQ(p->m_fileName, "aa");
    EXPECT_EQ(p->m_runMode, LogExportThread::TxtNORMAL);
    EXPECT_EQ(p->m_labels, labels);
    EXPECT_EQ(p->m_canRunning, true);
    p->deleteLater();
}

TEST(LogExportThread_exportToTxtPublic_UT, LogExportThread_exportToTxtPublic_UT_009)
{
    bool iscomplete = true;
    LogExportThread *p = new LogExportThread(iscomplete, nullptr);
    EXPECT_NE(p, nullptr);
    QList<LOG_MSG_KWIN> jList;
    LOG_MSG_KWIN info;
    info.msg = "testmsg";
    jList << info;
    QStringList labels;
    labels << "testlabel1";
    p->exportToTxtPublic("aa", jList, labels);
    EXPECT_EQ(p->m_fileName, "aa");
    EXPECT_EQ(p->m_runMode, LogExportThread::TxtKWIN);
    EXPECT_EQ(p->m_labels, labels);
    EXPECT_EQ(p->m_canRunning, true);
    p->deleteLater();
}

TEST(LogExportThread_exportToHtmlPublic_UT, LogExportThread_exportToHtmlPublic_UT_001)
{
    bool iscomplete = true;
    LogExportThread *p = new LogExportThread(iscomplete, nullptr);
    EXPECT_NE(p, nullptr);
    p->exportToHtmlPublic("aa", nullptr, XORG);
    EXPECT_EQ(p->m_fileName, "aa");
    EXPECT_EQ(p->m_pModel, nullptr);
    EXPECT_EQ(p->m_flag, XORG);
    EXPECT_EQ(p->m_runMode, LogExportThread::HtmlModel);
    EXPECT_EQ(p->m_canRunning, true);
    p->deleteLater();
}

TEST(LogExportThread_exportToHtmlPublic_UT, LogExportThread_exportToHtmlPublic_UT_002)
{
    bool iscomplete = true;
    LogExportThread *p = new LogExportThread(iscomplete, nullptr);
    EXPECT_NE(p, nullptr);
    QList<LOG_MSG_JOURNAL> jList;
    LOG_MSG_JOURNAL info;
    info.msg = "testmsg";
    jList << info;
    QStringList labels;
    labels << "testlabel1";
    p->exportToHtmlPublic("aa", jList, labels, JOURNAL);
    EXPECT_EQ(p->m_fileName, "aa");
    EXPECT_EQ(p->m_flag, JOURNAL);
    EXPECT_EQ(p->m_runMode, LogExportThread::HtmlJOURNAL);
    EXPECT_EQ(p->m_labels, labels);
    EXPECT_EQ(p->m_canRunning, true);
    p->deleteLater();
}

TEST(LogExportThread_exportToHtmlPublic_UT, LogExportThread_exportToHtmlPublic_UT_003)
{
    bool iscomplete = true;
    LogExportThread *p = new LogExportThread(iscomplete, nullptr);
    EXPECT_NE(p, nullptr);
    QList<LOG_MSG_APPLICATOIN> jList;
    LOG_MSG_APPLICATOIN info;
    info.msg = "testmsg";
    jList << info;
    QStringList labels;
    labels << "testlabel1";
    QString appname = "deepin-log-viewer";
    p->exportToHtmlPublic("aa", jList, labels, appname);
    EXPECT_EQ(p->m_fileName, "aa");
    EXPECT_EQ(p->m_appName, appname);
    EXPECT_EQ(p->m_runMode, LogExportThread::HtmlAPP);
    EXPECT_EQ(p->m_labels, labels);
    EXPECT_EQ(p->m_canRunning, true);
    p->deleteLater();
}

TEST(LogExportThread_exportToHtmlPublic_UT, LogExportThread_exportToHtmlPublic_UT_004)
{
    bool iscomplete = true;
    LogExportThread *p = new LogExportThread(iscomplete, nullptr);
    EXPECT_NE(p, nullptr);
    QList<LOG_MSG_DPKG> jList;
    LOG_MSG_DPKG info;
    info.msg = "testmsg";
    jList << info;
    QStringList labels;
    labels << "testlabel1";
    p->exportToHtmlPublic("aa", jList, labels);
    EXPECT_EQ(p->m_fileName, "aa");
    EXPECT_EQ(p->m_runMode, LogExportThread::HtmlDPKG);
    EXPECT_EQ(p->m_labels, labels);
    EXPECT_EQ(p->m_canRunning, true);
    p->deleteLater();
}

TEST(LogExportThread_exportToHtmlPublic_UT, LogExportThread_exportToHtmlPublic_UT_005)
{
    bool iscomplete = true;
    LogExportThread *p = new LogExportThread(iscomplete, nullptr);
    EXPECT_NE(p, nullptr);
    QList<LOG_MSG_BOOT> jList;
    LOG_MSG_BOOT info;
    info.msg = "testmsg";
    jList << info;
    QStringList labels;
    labels << "testlabel1";
    p->exportToHtmlPublic("aa", jList, labels);
    EXPECT_EQ(p->m_fileName, "aa");
    EXPECT_EQ(p->m_runMode, LogExportThread::HtmlBOOT);
    EXPECT_EQ(p->m_labels, labels);
    EXPECT_EQ(p->m_canRunning, true);
    p->deleteLater();
}

TEST(LogExportThread_exportToHtmlPublic_UT, LogExportThread_exportToHtmlPublic_UT_006)
{
    bool iscomplete = true;
    LogExportThread *p = new LogExportThread(iscomplete, nullptr);
    EXPECT_NE(p, nullptr);
    QList<LOG_MSG_BOOT> jList;
    LOG_MSG_BOOT info;
    info.msg = "testmsg";
    jList << info;
    QStringList labels;
    labels << "testlabel1";
    p->exportToHtmlPublic("aa", jList, labels);
    EXPECT_EQ(p->m_fileName, "aa");
    EXPECT_EQ(p->m_runMode, LogExportThread::HtmlBOOT);
    EXPECT_EQ(p->m_labels, labels);
    EXPECT_EQ(p->m_canRunning, true);
    p->deleteLater();
}

TEST(LogExportThread_exportToHtmlPublic_UT, LogExportThread_exportToHtmlPublic_UT_007)
{
    bool iscomplete = true;
    LogExportThread *p = new LogExportThread(iscomplete, nullptr);
    EXPECT_NE(p, nullptr);
    QList<LOG_MSG_XORG> jList;
    LOG_MSG_XORG info;
    info.msg = "testmsg";
    jList << info;
    QStringList labels;
    labels << "testlabel1";
    p->exportToHtmlPublic("aa", jList, labels);
    EXPECT_EQ(p->m_fileName, "aa");
    EXPECT_EQ(p->m_runMode, LogExportThread::HtmlXORG);
    EXPECT_EQ(p->m_labels, labels);
    EXPECT_EQ(p->m_canRunning, true);
    p->deleteLater();
}

TEST(LogExportThread_exportToHtmlPublic_UT, LogExportThread_exportToHtmlPublic_UT_008)
{
    bool iscomplete = true;
    LogExportThread *p = new LogExportThread(iscomplete, nullptr);
    EXPECT_NE(p, nullptr);
    QList<LOG_MSG_NORMAL> jList;
    LOG_MSG_NORMAL info;
    info.msg = "testmsg";
    jList << info;
    QStringList labels;
    labels << "testlabel1";
    p->exportToHtmlPublic("aa", jList, labels);
    EXPECT_EQ(p->m_fileName, "aa");
    EXPECT_EQ(p->m_runMode, LogExportThread::HtmlNORMAL);
    EXPECT_EQ(p->m_labels, labels);
    EXPECT_EQ(p->m_canRunning, true);
    p->deleteLater();
}

TEST(LogExportThread_exportToHtmlPublic_UT, LogExportThread_exportToHtmlPublic_UT_009)
{
    bool iscomplete = true;
    LogExportThread *p = new LogExportThread(iscomplete, nullptr);
    EXPECT_NE(p, nullptr);
    QList<LOG_MSG_KWIN> jList;
    LOG_MSG_KWIN info;
    info.msg = "testmsg";
    jList << info;
    QStringList labels;
    labels << "testlabel1";
    p->exportToHtmlPublic("aa", jList, labels);
    EXPECT_EQ(p->m_fileName, "aa");
    EXPECT_EQ(p->m_runMode, LogExportThread::HtmlKWIN);
    EXPECT_EQ(p->m_labels, labels);
    EXPECT_EQ(p->m_canRunning, true);
    p->deleteLater();
}

TEST(LogExportThread_exportToDocPublic_UT, LogExportThread_exportToDocPublic_UT_001)
{
    bool iscomplete = true;
    LogExportThread *p = new LogExportThread(iscomplete, nullptr);
    EXPECT_NE(p, nullptr);
    p->exportToDocPublic("aa", nullptr, XORG);
    EXPECT_EQ(p->m_fileName, "aa");
    EXPECT_EQ(p->m_pModel, nullptr);
    EXPECT_EQ(p->m_flag, XORG);
    EXPECT_EQ(p->m_runMode, LogExportThread::DocModel);
    EXPECT_EQ(p->m_canRunning, true);
    p->deleteLater();
}

TEST(LogExportThread_exportToDocPublic_UT, LogExportThread_exportToDocPublic_UT_002)
{
    bool iscomplete = true;
    LogExportThread *p = new LogExportThread(iscomplete, nullptr);
    EXPECT_NE(p, nullptr);
    QList<LOG_MSG_JOURNAL> jList;
    LOG_MSG_JOURNAL info;
    info.msg = "testmsg";
    jList << info;
    QStringList labels;
    labels << "testlabel1";
    p->exportToDocPublic("aa", jList, labels, JOURNAL);
    EXPECT_EQ(p->m_fileName, "aa");
    EXPECT_EQ(p->m_flag, JOURNAL);
    EXPECT_EQ(p->m_runMode, LogExportThread::DocJOURNAL);
    EXPECT_EQ(p->m_labels, labels);
    EXPECT_EQ(p->m_canRunning, true);
    p->deleteLater();
}

TEST(LogExportThread_exportToDocPublic_UT, LogExportThread_exportToDocPublic_UT_003)
{
    bool iscomplete = true;
    LogExportThread *p = new LogExportThread(iscomplete, nullptr);
    EXPECT_NE(p, nullptr);
    QList<LOG_MSG_APPLICATOIN> jList;
    LOG_MSG_APPLICATOIN info;
    info.msg = "testmsg";
    jList << info;
    QStringList labels;
    labels << "testlabel1";
    QString appname = "deepin-log-viewer";
    p->exportToDocPublic("aa", jList, labels, appname);
    EXPECT_EQ(p->m_fileName, "aa");
    EXPECT_EQ(p->m_appName, appname);
    EXPECT_EQ(p->m_runMode, LogExportThread::DocAPP);
    EXPECT_EQ(p->m_labels, labels);
    EXPECT_EQ(p->m_canRunning, true);
    p->deleteLater();
}

TEST(LogExportThread_exportToDocPublic_UT, LogExportThread_exportToDocPublic_UT_004)
{
    bool iscomplete = true;
    LogExportThread *p = new LogExportThread(iscomplete, nullptr);
    EXPECT_NE(p, nullptr);
    QList<LOG_MSG_DPKG> jList;
    LOG_MSG_DPKG info;
    info.msg = "testmsg";
    jList << info;
    QStringList labels;
    labels << "testlabel1";
    p->exportToDocPublic("aa", jList, labels);
    EXPECT_EQ(p->m_fileName, "aa");
    EXPECT_EQ(p->m_runMode, LogExportThread::DocDPKG);
    EXPECT_EQ(p->m_labels, labels);
    EXPECT_EQ(p->m_canRunning, true);
    p->deleteLater();
}

TEST(LogExportThread_exportToDocPublic_UT, LogExportThread_exportToDocPublic_UT_005)
{
    bool iscomplete = true;
    LogExportThread *p = new LogExportThread(iscomplete, nullptr);
    EXPECT_NE(p, nullptr);
    QList<LOG_MSG_BOOT> jList;
    LOG_MSG_BOOT info;
    info.msg = "testmsg";
    jList << info;
    QStringList labels;
    labels << "testlabel1";
    p->exportToDocPublic("aa", jList, labels);
    EXPECT_EQ(p->m_fileName, "aa");
    EXPECT_EQ(p->m_runMode, LogExportThread::DocBOOT);
    EXPECT_EQ(p->m_labels, labels);
    EXPECT_EQ(p->m_canRunning, true);
    p->deleteLater();
}

TEST(LogExportThread_exportToDocPublic_UT, LogExportThread_exportToDocPublic_UT_006)
{
    bool iscomplete = true;
    LogExportThread *p = new LogExportThread(iscomplete, nullptr);
    EXPECT_NE(p, nullptr);
    QList<LOG_MSG_BOOT> jList;
    LOG_MSG_BOOT info;
    info.msg = "testmsg";
    jList << info;
    QStringList labels;
    labels << "testlabel1";
    p->exportToDocPublic("aa", jList, labels);
    EXPECT_EQ(p->m_fileName, "aa");
    EXPECT_EQ(p->m_runMode, LogExportThread::DocBOOT);
    EXPECT_EQ(p->m_labels, labels);
    EXPECT_EQ(p->m_canRunning, true);
    p->deleteLater();
}

TEST(LogExportThread_exportToDocPublic_UT, LogExportThread_exportToDocPublic_UT_007)
{
    bool iscomplete = true;
    LogExportThread *p = new LogExportThread(iscomplete, nullptr);
    EXPECT_NE(p, nullptr);
    QList<LOG_MSG_XORG> jList;
    LOG_MSG_XORG info;
    info.msg = "testmsg";
    jList << info;
    QStringList labels;
    labels << "testlabel1";
    p->exportToDocPublic("aa", jList, labels);
    EXPECT_EQ(p->m_fileName, "aa");
    EXPECT_EQ(p->m_runMode, LogExportThread::DocXORG);
    EXPECT_EQ(p->m_labels, labels);
    EXPECT_EQ(p->m_canRunning, true);
    p->deleteLater();
}

TEST(LogExportThread_exportToDocPublic_UT, LogExportThread_exportToDocPublic_UT_008)
{
    bool iscomplete = true;
    LogExportThread *p = new LogExportThread(iscomplete, nullptr);
    EXPECT_NE(p, nullptr);
    QList<LOG_MSG_NORMAL> jList;
    LOG_MSG_NORMAL info;
    info.msg = "testmsg";
    jList << info;
    QStringList labels;
    labels << "testlabel1";
    p->exportToDocPublic("aa", jList, labels);
    EXPECT_EQ(p->m_fileName, "aa");
    EXPECT_EQ(p->m_runMode, LogExportThread::DocNORMAL);
    EXPECT_EQ(p->m_labels, labels);
    EXPECT_EQ(p->m_canRunning, true);
    p->deleteLater();
}

TEST(LogExportThread_exportToDocPublic_UT, LogExportThread_exportToDocPublic_UT_009)
{
    bool iscomplete = true;
    LogExportThread *p = new LogExportThread(iscomplete, nullptr);
    EXPECT_NE(p, nullptr);
    QList<LOG_MSG_KWIN> jList;
    LOG_MSG_KWIN info;
    info.msg = "testmsg";
    jList << info;
    QStringList labels;
    labels << "testlabel1";
    p->exportToDocPublic("aa", jList, labels);
    EXPECT_EQ(p->m_fileName, "aa");
    EXPECT_EQ(p->m_runMode, LogExportThread::DocKWIN);
    EXPECT_EQ(p->m_labels, labels);
    EXPECT_EQ(p->m_canRunning, true);
    p->deleteLater();
}

TEST(LogExportThread_exportToXlsPublic_UT, LogExportThread_exportToXlsPublic_UT_001)
{
    bool iscomplete = true;
    LogExportThread *p = new LogExportThread(iscomplete, nullptr);
    EXPECT_NE(p, nullptr);
    p->exportToXlsPublic("aa", nullptr, XORG);
    EXPECT_EQ(p->m_fileName, "aa");
    EXPECT_EQ(p->m_pModel, nullptr);
    EXPECT_EQ(p->m_flag, XORG);
    EXPECT_EQ(p->m_runMode, LogExportThread::XlsModel);
    EXPECT_EQ(p->m_canRunning, true);
    p->deleteLater();
}

TEST(LogExportThread_exportToXlsPublic_UT, LogExportThread_exportToXlsPublic_UT_002)
{
    bool iscomplete = true;
    LogExportThread *p = new LogExportThread(iscomplete, nullptr);
    EXPECT_NE(p, nullptr);
    QList<LOG_MSG_JOURNAL> jList;
    LOG_MSG_JOURNAL info;
    info.msg = "testmsg";
    jList << info;
    QStringList labels;
    labels << "testlabel1";
    p->exportToXlsPublic("aa", jList, labels, JOURNAL);
    EXPECT_EQ(p->m_fileName, "aa");
    EXPECT_EQ(p->m_flag, JOURNAL);
    EXPECT_EQ(p->m_runMode, LogExportThread::XlsJOURNAL);
    EXPECT_EQ(p->m_labels, labels);
    EXPECT_EQ(p->m_canRunning, true);
    p->deleteLater();
}

TEST(LogExportThread_exportToXlsPublic_UT, LogExportThread_exportToXlsPublic_UT_003)
{
    bool iscomplete = true;
    LogExportThread *p = new LogExportThread(iscomplete, nullptr);
    EXPECT_NE(p, nullptr);
    QList<LOG_MSG_APPLICATOIN> jList;
    LOG_MSG_APPLICATOIN info;
    info.msg = "testmsg";
    jList << info;
    QStringList labels;
    labels << "testlabel1";
    QString appname = "deepin-log-viewer";
    p->exportToXlsPublic("aa", jList, labels, appname);
    EXPECT_EQ(p->m_fileName, "aa");
    EXPECT_EQ(p->m_appName, appname);
    EXPECT_EQ(p->m_runMode, LogExportThread::XlsAPP);
    EXPECT_EQ(p->m_labels, labels);
    EXPECT_EQ(p->m_canRunning, true);
    p->deleteLater();
}

TEST(LogExportThread_exportToXlsPublic_UT, LogExportThread_exportToXlsPublic_UT_004)
{
    bool iscomplete = true;
    LogExportThread *p = new LogExportThread(iscomplete, nullptr);
    EXPECT_NE(p, nullptr);
    QList<LOG_MSG_DPKG> jList;
    LOG_MSG_DPKG info;
    info.msg = "testmsg";
    jList << info;
    QStringList labels;
    labels << "testlabel1";
    p->exportToXlsPublic("aa", jList, labels);
    EXPECT_EQ(p->m_fileName, "aa");
    EXPECT_EQ(p->m_runMode, LogExportThread::XlsDPKG);
    EXPECT_EQ(p->m_labels, labels);
    EXPECT_EQ(p->m_canRunning, true);
    p->deleteLater();
}

TEST(LogExportThread_exportToXlsPublic_UT, LogExportThread_exportToXlsPublic_UT_005)
{
    bool iscomplete = true;
    LogExportThread *p = new LogExportThread(iscomplete, nullptr);
    EXPECT_NE(p, nullptr);
    QList<LOG_MSG_BOOT> jList;
    LOG_MSG_BOOT info;
    info.msg = "testmsg";
    jList << info;
    QStringList labels;
    labels << "testlabel1";
    p->exportToXlsPublic("aa", jList, labels);
    EXPECT_EQ(p->m_fileName, "aa");
    EXPECT_EQ(p->m_runMode, LogExportThread::XlsBOOT);
    EXPECT_EQ(p->m_labels, labels);
    EXPECT_EQ(p->m_canRunning, true);
    p->deleteLater();
}

TEST(LogExportThread_exportToXlsPublic_UT, LogExportThread_exportToXlsPublic_UT_006)
{
    bool iscomplete = true;
    LogExportThread *p = new LogExportThread(iscomplete, nullptr);
    EXPECT_NE(p, nullptr);
    QList<LOG_MSG_BOOT> jList;
    LOG_MSG_BOOT info;
    info.msg = "testmsg";
    jList << info;
    QStringList labels;
    labels << "testlabel1";
    p->exportToXlsPublic("aa", jList, labels);
    EXPECT_EQ(p->m_fileName, "aa");
    EXPECT_EQ(p->m_runMode, LogExportThread::XlsBOOT);
    EXPECT_EQ(p->m_labels, labels);
    EXPECT_EQ(p->m_canRunning, true);
    p->deleteLater();
}

TEST(LogExportThread_exportToXlsPublic_UT, LogExportThread_exportToXlsPublic_UT_007)
{
    bool iscomplete = true;
    LogExportThread *p = new LogExportThread(iscomplete, nullptr);
    EXPECT_NE(p, nullptr);
    QList<LOG_MSG_XORG> jList;
    LOG_MSG_XORG info;
    info.msg = "testmsg";
    jList << info;
    QStringList labels;
    labels << "testlabel1";
    p->exportToXlsPublic("aa", jList, labels);
    EXPECT_EQ(p->m_fileName, "aa");
    EXPECT_EQ(p->m_runMode, LogExportThread::XlsXORG);
    EXPECT_EQ(p->m_labels, labels);
    EXPECT_EQ(p->m_canRunning, true);
    p->deleteLater();
}

TEST(LogExportThread_exportToXlsPublic_UT, LogExportThread_exportToXlsPublic_UT_008)
{
    bool iscomplete = true;
    LogExportThread *p = new LogExportThread(iscomplete, nullptr);
    EXPECT_NE(p, nullptr);
    QList<LOG_MSG_NORMAL> jList;
    LOG_MSG_NORMAL info;
    info.msg = "testmsg";
    jList << info;
    QStringList labels;
    labels << "testlabel1";
    p->exportToXlsPublic("aa", jList, labels);
    EXPECT_EQ(p->m_fileName, "aa");
    EXPECT_EQ(p->m_runMode, LogExportThread::XlsNORMAL);
    EXPECT_EQ(p->m_labels, labels);
    EXPECT_EQ(p->m_canRunning, true);
    p->deleteLater();
}

TEST(LogExportThread_exportToXlsPublic_UT, LogExportThread_exportToXlsPublic_UT_009)
{
    bool iscomplete = true;
    LogExportThread *p = new LogExportThread(iscomplete, nullptr);
    EXPECT_NE(p, nullptr);
    QList<LOG_MSG_KWIN> jList;
    LOG_MSG_KWIN info;
    info.msg = "testmsg";
    jList << info;
    QStringList labels;
    labels << "testlabel1";
    p->exportToXlsPublic("aa", jList, labels);
    EXPECT_EQ(p->m_fileName, "aa");
    EXPECT_EQ(p->m_runMode, LogExportThread::XlsKWIN);
    EXPECT_EQ(p->m_labels, labels);
    EXPECT_EQ(p->m_canRunning, true);
    p->deleteLater();
}

TEST(LogExportThread_isProcessing_UT, LogExportThread_isProcessing_UT)
{
    bool iscomplete = true;
    LogExportThread *p = new LogExportThread(iscomplete, nullptr);
    EXPECT_NE(p, nullptr);
    p->isProcessing();
    p->deleteLater();
}

TEST(LogExportThread_stopImmediately_UT, LogExportThread_stopImmediately_UT)
{
    bool iscomplete = true;
    LogExportThread *p = new LogExportThread(iscomplete, nullptr);
    EXPECT_NE(p, nullptr);
    p->stopImmediately();
    EXPECT_EQ(p->m_canRunning, false);
    p->deleteLater();
}

TEST(LogExportThread_exportToTxt_Model_UT, LogExportThread_exportToTxt_Model_UT_001)
{
    bool iscomplete = true;
    LogExportThread *p = new LogExportThread(iscomplete, nullptr);
    EXPECT_NE(p, nullptr);
    QStandardItemModel *p_model = new QStandardItemModel();
    p_model->setHorizontalHeaderLabels(QStringList()
                                       << Dtk::Widget::DApplication::translate("Table", "Level")
                                       << Dtk::Widget::DApplication::translate("Table", "Date and Time")
                                       << Dtk::Widget::DApplication::translate("Table", "Source")
                                       << Dtk::Widget::DApplication::translate("Table", "Info"));
    QList<QStandardItem *> items;
    Dtk::Widget::DStandardItem *item = nullptr;
    for (int i = 0; i < 5; i++) {
        items.clear();
        //int col = 0;
        item = new Dtk::Widget::DStandardItem;
        item->setData(QString("test%1").arg(i), Log_Item_SPACE::levelRole);
        item->setAccessibleText(QString("treeview_context_%1_%2").arg(i).arg(0));
        items << item;
        item = new Dtk::Widget::DStandardItem("2000-10-10 10:10:10");
        item->setAccessibleText(QString("treeview_context_%1_%2").arg(i).arg(1));
        items << item;
        //        item = new DStandardItem(list[i].src);
        item = new Dtk::Widget::DStandardItem("deepinlogviewer");
        item->setAccessibleText(QString("treeview_context_%1_%2").arg(i).arg(2));
        items << item;
        item = new Dtk::Widget::DStandardItem(QString("testmsg%1").arg(i));
        item->setData(APP_TABLE_DATA);
        item->setAccessibleText(QString("treeview_context_%1_%2").arg(i).arg(3));
        items << item;
        p_model->insertRow(p_model->rowCount(), items);
    }
    p->m_canRunning = true;

    bool result = p->exportToTxt("", p_model, APP);
    EXPECT_EQ(result, false);
    p->deleteLater();
}

TEST(LogExportThread_exportToTxt_Model_UT, LogExportThread_exportToTxt_Model_UT_002)
{
    bool iscomplete = true;
    LogExportThread *p = new LogExportThread(iscomplete, nullptr);
    EXPECT_NE(p, nullptr);
    QStandardItemModel *p_model = new QStandardItemModel();
    p_model->setHorizontalHeaderLabels(QStringList()
                                       << Dtk::Widget::DApplication::translate("Table", "Level")
                                       << Dtk::Widget::DApplication::translate("Table", "Date and Time")
                                       << Dtk::Widget::DApplication::translate("Table", "Source")
                                       << Dtk::Widget::DApplication::translate("Table", "Info"));
    QList<QStandardItem *> items;
    Dtk::Widget::DStandardItem *item = nullptr;
    for (int i = 0; i < 5; i++) {
        items.clear();
        //int col = 0;
        item = new Dtk::Widget::DStandardItem;

        item->setData(APP_TABLE_DATA);
        item->setData(QString("test%1").arg(i), Log_Item_SPACE::levelRole);
        item->setAccessibleText(QString("treeview_context_%1_%2").arg(i).arg(0));
        items << item;
        item = new Dtk::Widget::DStandardItem("2000-10-10 10:10:10");
        item->setData(APP_TABLE_DATA);
        item->setAccessibleText(QString("treeview_context_%1_%2").arg(i).arg(1));
        items << item;
        //        item = new DStandardItem(list[i].src);
        item = new Dtk::Widget::DStandardItem("deepinlogviewer");
        item->setData(APP_TABLE_DATA);
        item->setAccessibleText(QString("treeview_context_%1_%2").arg(i).arg(2));
        items << item;
        item = new Dtk::Widget::DStandardItem(QString("testmsg%1").arg(i));
        item->setData(APP_TABLE_DATA);
        item->setAccessibleText(QString("treeview_context_%1_%2").arg(i).arg(3));
        items << item;
        p_model->insertRow(p_model->rowCount(), items);
    }
    p->m_canRunning = true;
    bool result = p->exportToTxt("./testExport.txt", p_model, APP);
    EXPECT_EQ(result, true);
    p->deleteLater();
}

TEST(LogExportThread_exportToTxt_Model_UT, LogExportThread_exportToTxt_Model_UT003)
{
    bool iscomplete = true;
    LogExportThread *p = new LogExportThread(iscomplete, nullptr);
    EXPECT_NE(p, nullptr);
    QStandardItemModel *p_model = new QStandardItemModel();
    p_model->setHorizontalHeaderLabels(QStringList()
                                       << Dtk::Widget::DApplication::translate("Table", "Level")
                                       << Dtk::Widget::DApplication::translate("Table", "Date and Time")
                                       << Dtk::Widget::DApplication::translate("Table", "Source")
                                       << Dtk::Widget::DApplication::translate("Table", "Info"));
    QList<QStandardItem *> items;
    Dtk::Widget::DStandardItem *item = nullptr;
    for (int i = 0; i < 5; i++) {
        items.clear();
        //int col = 0;

        item = new Dtk::Widget::DStandardItem;
        item->setData(APP_TABLE_DATA);
        item->setData(QString("test%1").arg(i), Log_Item_SPACE::levelRole);
        item->setAccessibleText(QString("treeview_context_%1_%2").arg(i).arg(0));
        items << item;
        item = new Dtk::Widget::DStandardItem("2000-10-10 10:10:10");
        item->setData(APP_TABLE_DATA);
        item->setAccessibleText(QString("treeview_context_%1_%2").arg(i).arg(1));
        items << item;
        //        item = new DStandardItem(list[i].src);
        item = new Dtk::Widget::DStandardItem("deepinlogviewer");
        item->setData(APP_TABLE_DATA);
        item->setAccessibleText(QString("treeview_context_%1_%2").arg(i).arg(2));
        items << item;
        item = new Dtk::Widget::DStandardItem(QString("testmsg%1").arg(i));
        item->setData(APP_TABLE_DATA);
        item->setAccessibleText(QString("treeview_context_%1_%2").arg(i).arg(3));
        items << item;
        p_model->insertRow(p_model->rowCount(), items);
    }
    p->m_canRunning = true;
    bool result = p->exportToTxt("./testExport.txt", p_model, JOURNAL);
    EXPECT_EQ(result, true);
    p->deleteLater();
}

class LogExportThread_exportToTxt_JOURNAL_UT_Param
{
public:
    LogExportThread_exportToTxt_JOURNAL_UT_Param(bool iCanFileRead, LOG_FLAG iFlag, bool iResult)
        : canFileRead(iCanFileRead)
        , flag(iFlag)
        , result(iResult)
    {
    }
    bool canFileRead;
    LOG_FLAG flag;
    bool result;
};

class LogExportThread_exportToTxt_JOURNAL_UT : public ::testing::TestWithParam<LogExportThread_exportToTxt_JOURNAL_UT_Param>
{
};

INSTANTIATE_TEST_CASE_P(LogApplication, LogExportThread_exportToTxt_JOURNAL_UT, ::testing::Values(LogExportThread_exportToTxt_JOURNAL_UT_Param(false, KERN, false), LogExportThread_exportToTxt_JOURNAL_UT_Param(true, JOURNAL, true), LogExportThread_exportToTxt_JOURNAL_UT_Param(true, KERN, true), LogExportThread_exportToTxt_JOURNAL_UT_Param(true, APP, true)));

TEST_P(LogExportThread_exportToTxt_JOURNAL_UT, LogExportThread_exportToTxt_JOURNAL_UT)
{
    bool iscomplete = true;
    LogExportThread *p = new LogExportThread(iscomplete, nullptr);
    EXPECT_NE(p, nullptr);
    LogExportThread_exportToTxt_JOURNAL_UT_Param param = GetParam();
    QList<LOG_MSG_JOURNAL> jList;
    for (int i = 0; i < 5; ++i) {
        LOG_MSG_JOURNAL info;
        info.msg = "testmsg";
        info.level = "testlevel";
        info.daemonId = "testid";
        info.dateTime = "testdatetime";
        info.hostName = "testhostName";
        info.daemonName = "testdaemonName";
        jList << info;
    }
    QString fileName = param.canFileRead ? "./testExport.txt" : "";
    QStringList labels;

    for (int i = 0; i < 8; ++i) {
        labels << QString("testhead%1").arg(i);
    }
    p->m_canRunning = true;
    bool result = p->exportToTxt(fileName, jList, labels, param.flag);
    EXPECT_EQ(result, param.result);
    p->deleteLater();
}

TEST(LogExportThread_exportToTxt_APP_UT, LogExportThread_exportToTxt_APP_UT_001)
{
    bool iscomplete = true;
    LogExportThread *p = new LogExportThread(iscomplete, nullptr);
    EXPECT_NE(p, nullptr);
    QList<LOG_MSG_APPLICATOIN> jList;
    for (int i = 0; i < 5; ++i) {
        LOG_MSG_APPLICATOIN info;
        info.msg = "testmsg";
        info.level = "testlevel";
        info.dateTime = "testdatetime";
        info.src = "testsrc";
        jList << info;
    }
    QString fileName = "./testExport.txt";
    QStringList labels;

    for (int i = 0; i < 8; ++i) {
        labels << QString("testhead%1").arg(i);
    }
    QString appname("testapp");
    p->m_canRunning = true;
    bool result = p->exportToTxt(fileName, jList, labels, appname);
    EXPECT_EQ(result, true);
    p->deleteLater();
}

TEST(LogExportThread_exportToTxt_APP_UT, LogExportThread_exportToTxt_APP_UT_002)
{
    bool iscomplete = true;
    LogExportThread *p = new LogExportThread(iscomplete, nullptr);
    EXPECT_NE(p, nullptr);
    QList<LOG_MSG_APPLICATOIN> jList;
    for (int i = 0; i < 5; ++i) {
        LOG_MSG_APPLICATOIN info;
        info.msg = "testmsg";
        info.level = "testlevel";
        info.dateTime = "testdatetime";
        info.src = "testsrc";
        jList << info;
    }
    QString fileName = "";
    QStringList labels;

    for (int i = 0; i < 8; ++i) {
        labels << QString("testhead%1").arg(i);
    }
    QString appname("testapp");
    p->m_canRunning = true;
    bool result = p->exportToTxt(fileName, jList, labels, appname);
    EXPECT_EQ(result, false);
    p->deleteLater();
}

TEST(LogExportThread_exportToTxt_DPKG_UT, LogExportThread_exportToTxt_DPKG_UT_001)
{
    bool iscomplete = true;
    LogExportThread *p = new LogExportThread(iscomplete, nullptr);
    EXPECT_NE(p, nullptr);
    QList<LOG_MSG_DPKG> jList;
    for (int i = 0; i < 5; ++i) {
        LOG_MSG_DPKG info;
        info.msg = "testmsg";
        info.action = "testaction";
        info.dateTime = "testdatetime";
        jList << info;
    }
    QString fileName = "./testExport.txt";
    QStringList labels;

    for (int i = 0; i < 8; ++i) {
        labels << QString("testhead%1").arg(i);
    }
    p->m_canRunning = true;
    bool result = p->exportToTxt(fileName, jList, labels);
    EXPECT_EQ(result, true);
    p->deleteLater();
}

TEST(LogExportThread_exportToTxt_DPKG_UT, LogExportThread_exportToTxt_DPKG_UT_002)
{
    bool iscomplete = true;
    LogExportThread *p = new LogExportThread(iscomplete, nullptr);
    EXPECT_NE(p, nullptr);
    QList<LOG_MSG_DPKG> jList;
    for (int i = 0; i < 5; ++i) {
        LOG_MSG_DPKG info;
        info.msg = "testmsg";
        info.action = "testaction";
        info.dateTime = "testdatetime";
        jList << info;
    }
    QString fileName = "";
    QStringList labels;

    for (int i = 0; i < 8; ++i) {
        labels << QString("testhead%1").arg(i);
    }
    p->m_canRunning = true;
    bool result = p->exportToTxt(fileName, jList, labels);
    EXPECT_EQ(result, false);
    p->deleteLater();
}

TEST(LogExportThread_exportToTxt_BOOT_UT, LogExportThread_exportToTxt_BOOT_UT_001)
{
    bool iscomplete = true;
    LogExportThread *p = new LogExportThread(iscomplete, nullptr);
    EXPECT_NE(p, nullptr);
    QList<LOG_MSG_BOOT> jList;
    for (int i = 0; i < 5; ++i) {
        LOG_MSG_BOOT info;
        info.msg = "testmsg";
        info.status = "teststatus";
        jList << info;
    }
    QString fileName = "./testExport.txt";
    QStringList labels;

    for (int i = 0; i < 8; ++i) {
        labels << QString("testhead%1").arg(i);
    }
    p->m_canRunning = true;
    bool result = p->exportToTxt(fileName, jList, labels);
    EXPECT_EQ(result, true);
    p->deleteLater();
}

TEST(LogExportThread_exportToTxt_BOOT_UT, LogExportThread_exportToTxt_BOOT_UT_002)
{
    bool iscomplete = true;
    LogExportThread *p = new LogExportThread(iscomplete, nullptr);
    EXPECT_NE(p, nullptr);
    QList<LOG_MSG_BOOT> jList;
    for (int i = 0; i < 5; ++i) {
        LOG_MSG_BOOT info;
        info.msg = "testmsg";
        info.status = "teststatus";
        jList << info;
    }
    QString fileName = "";
    QStringList labels;

    for (int i = 0; i < 8; ++i) {
        labels << QString("testhead%1").arg(i);
    }
    p->m_canRunning = true;
    bool result = p->exportToTxt(fileName, jList, labels);
    EXPECT_EQ(result, false);
    p->deleteLater();
}

TEST(LogExportThread_exportToTxt_XORG_UT, LogExportThread_exportToTxt_XORG_UT_001)
{
    bool iscomplete = true;
    LogExportThread *p = new LogExportThread(iscomplete, nullptr);
    EXPECT_NE(p, nullptr);
    QList<LOG_MSG_XORG> jList;
    for (int i = 0; i < 5; ++i) {
        LOG_MSG_XORG info;
        info.msg = "testmsg";
        info.dateTime = "testdateTime";
        jList << info;
    }

    QString fileName = "./testExport.txt";
    QStringList labels;

    for (int i = 0; i < 8; ++i) {
        labels << QString("testhead%1").arg(i);
    }
    p->m_canRunning = true;
    bool result = p->exportToTxt(fileName, jList, labels);
    EXPECT_EQ(result, true);
    p->deleteLater();
}

TEST(LogExportThread_exportToTxt_XORG_UT, LogExportThread_exportToTxt_XORG_UT_002)
{
    bool iscomplete = true;
    LogExportThread *p = new LogExportThread(iscomplete, nullptr);
    EXPECT_NE(p, nullptr);
    QList<LOG_MSG_XORG> jList;
    for (int i = 0; i < 5; ++i) {
        LOG_MSG_XORG info;
        info.msg = "testmsg";
        info.dateTime = "testdateTime";
        jList << info;
    }
    QString fileName = "";
    QStringList labels;

    for (int i = 0; i < 8; ++i) {
        labels << QString("testhead%1").arg(i);
    }
    p->m_canRunning = true;
    bool result = p->exportToTxt(fileName, jList, labels);
    EXPECT_EQ(result, false);
    p->deleteLater();
}

TEST(LogExportThread_exportToTxt_NORMAL_UT, LogExportThread_exportToTxt_NORMAL_UT_001)
{
    bool iscomplete = true;
    LogExportThread *p = new LogExportThread(iscomplete, nullptr);
    EXPECT_NE(p, nullptr);
    QList<LOG_MSG_NORMAL> jList;
    for (int i = 0; i < 5; ++i) {
        LOG_MSG_NORMAL info;
        info.msg = "testmsg";
        info.dateTime = "testdateTime";
        info.userName = "test";
        info.eventType = "test";
        jList << info;
    }
    QString fileName = "./testExport.txt";
    QStringList labels;

    for (int i = 0; i < 8; ++i) {
        labels << QString("testhead%1").arg(i);
    }
    p->m_canRunning = true;
    bool result = p->exportToTxt(fileName, jList, labels);
    EXPECT_EQ(result, true);
    p->deleteLater();
}

TEST(LogExportThread_exportToTxt_NORMAL_UT, LogExportThread_exportToTxt_NORMAL_UT_002)
{
    bool iscomplete = true;
    LogExportThread *p = new LogExportThread(iscomplete, nullptr);
    EXPECT_NE(p, nullptr);
    QList<LOG_MSG_NORMAL> jList;
    for (int i = 0; i < 5; ++i) {
        LOG_MSG_NORMAL info;
        info.msg = "testmsg";
        info.dateTime = "testdateTime";
        info.userName = "test";
        info.eventType = "test";
        jList << info;
    }
    QString fileName = "";
    QStringList labels;

    for (int i = 0; i < 8; ++i) {
        labels << QString("testhead%1").arg(i);
    }
    p->m_canRunning = true;
    bool result = p->exportToTxt(fileName, jList, labels);
    EXPECT_EQ(result, false);
    p->deleteLater();
}

TEST(LogExportThread_exportToTxt_KWIN_UT, LogExportThread_exportToTxt_KWIN_UT_001)
{
    bool iscomplete = true;
    LogExportThread *p = new LogExportThread(iscomplete, nullptr);
    EXPECT_NE(p, nullptr);
    QList<LOG_MSG_KWIN> jList;
    for (int i = 0; i < 5; ++i) {
        LOG_MSG_KWIN info;
        info.msg = "testmsg";
        jList << info;
    }
    QString fileName = "./testExport.txt";
    QStringList labels;

    for (int i = 0; i < 8; ++i) {
        labels << QString("testhead%1").arg(i);
    }
    p->m_canRunning = true;
    bool result = p->exportToTxt(fileName, jList, labels);
    EXPECT_EQ(result, true);
    p->deleteLater();
}

TEST(LogExportThread_exportToTxt_KWIN_UT, LogExportThread_exportToTxt_KWIN_UT_002)
{
    bool iscomplete = true;
    LogExportThread *p = new LogExportThread(iscomplete, nullptr);
    EXPECT_NE(p, nullptr);
    QList<LOG_MSG_KWIN> jList;
    for (int i = 0; i < 5; ++i) {
        LOG_MSG_KWIN info;
        info.msg = "testmsg";
        jList << info;
    }
    QString fileName = "";
    QStringList labels;

    for (int i = 0; i < 8; ++i) {
        labels << QString("testhead%1").arg(i);
    }
    p->m_canRunning = true;
    bool result = p->exportToTxt(fileName, jList, labels);
    EXPECT_EQ(result, false);
    p->deleteLater();
}

class LogExportThread_exportToDoc_JOURNAL_UT_Param
{
public:
    LogExportThread_exportToDoc_JOURNAL_UT_Param(bool iCanFileRead, LOG_FLAG iFlag, bool iResult)
        : canFileRead(iCanFileRead)
        , flag(iFlag)
        , result(iResult)
    {
    }
    bool canFileRead;
    LOG_FLAG flag;
    bool result;
};

class LogExportThread_exportToDoc_JOURNAL_UT : public ::testing::TestWithParam<LogExportThread_exportToDoc_JOURNAL_UT_Param>
{
};

INSTANTIATE_TEST_CASE_P(LogApplication, LogExportThread_exportToDoc_JOURNAL_UT, ::testing::Values(LogExportThread_exportToDoc_JOURNAL_UT_Param(false, KERN, false), LogExportThread_exportToDoc_JOURNAL_UT_Param(true, JOURNAL, true), LogExportThread_exportToDoc_JOURNAL_UT_Param(true, KERN, true), LogExportThread_exportToDoc_JOURNAL_UT_Param(true, APP, true)));

//TEST_P(LogExportThread_exportToDoc_JOURNAL_UT, LogExportThread_exportToDoc_JOURNAL_UT)
//{
//    Stub stub;
//    stub.set(ADDR(Docx::Document, save), Doc_Document_save);

//    LogExportThread *p = new LogExportThread(nullptr);
//    EXPECT_NE(p, nullptr);
//    LogExportThread_exportToDoc_JOURNAL_UT_Param param = GetParam();
//    QList<LOG_MSG_JOURNAL> jList;
//    for (int i = 0; i < 5; ++i) {
//        LOG_MSG_JOURNAL info;
//        info.msg = "testmsg";
//        info.level = "testlevel";
//        info.daemonId = "testid";
//        info.dateTime = "testdatetime";
//        info.hostName = "testhostName";
//        info.daemonName = "testdaemonName";
//        jList << info;
//    }
//    QString fileName = param.canFileRead ? "./testExport.doc" : "";
//    QStringList labels;

//    for (int i = 0; i < 2; ++i) {
//        labels << QString("testhead%1").arg(i);
//    }
//    p->m_canRunning = true;
//    bool result = p->exportToDoc(fileName, jList, labels, param.flag);
//    EXPECT_EQ(result, param.result);
//    p->deleteLater();
//}

//TEST(LogExportThread_exportToDoc_APP_UT, LogExportThread_exportToDoc_APP_UT_001)
//{
//    Stub stub;
//    stub.set(ADDR(Docx::Document, save), Doc_Document_save);
//    LogExportThread *p = new LogExportThread(nullptr);
//    EXPECT_NE(p, nullptr);
//    QList<LOG_MSG_APPLICATOIN> jList;
//    for (int i = 0; i < 5; ++i) {
//        LOG_MSG_APPLICATOIN info;
//        info.msg = "testmsg";
//        info.level = "testlevel";
//        info.dateTime = "testdatetime";
//        info.src = "testsrc";
//        jList << info;
//    }
//    QString fileName = "./testExport.doc";
//    QStringList labels;

//    for (int i = 0; i < 2; ++i) {
//        labels << QString("testhead%1").arg(i);
//    }
//    p->m_canRunning = true;
//    QString appname("testapp");
//    bool result = p->exportToDoc(fileName, jList, labels, appname);
//    EXPECT_EQ(result, true);
//    p->deleteLater();
//}

//TEST(LogExportThread_exportToDoc_APP_UT, LogExportThread_exportToDoc_APP_UT_002)
//{
//    Stub stub;
//    stub.set(ADDR(Docx::Document, save), Doc_Document_save);
//    LogExportThread *p = new LogExportThread(nullptr);
//    EXPECT_NE(p, nullptr);
//    QList<LOG_MSG_APPLICATOIN> jList;
//    for (int i = 0; i < 5; ++i) {
//        LOG_MSG_APPLICATOIN info;
//        info.msg = "testmsg";
//        info.level = "testlevel";
//        info.dateTime = "testdatetime";
//        info.src = "testsrc";
//        jList << info;
//    }
//    QString fileName = "";
//    QStringList labels;

//    for (int i = 0; i < 2; ++i) {
//        labels << QString("testhead%1").arg(i);
//    }
//    p->m_canRunning = true;
//    QString appname("testapp");
//    bool result = p->exportToDoc(fileName, jList, labels, appname);
//    EXPECT_EQ(result, true);
//    p->deleteLater();
//}

//TEST(LogExportThread_exportToDoc_DPKG_UT, LogExportThread_exportToDoc_DPKG_UT_001)
//{
//    Stub stub;
//    stub.set(ADDR(Docx::Document, save), Doc_Document_save);
//    LogExportThread *p = new LogExportThread(nullptr);
//    EXPECT_NE(p, nullptr);
//    QList<LOG_MSG_DPKG> jList;
//    for (int i = 0; i < 5; ++i) {
//        LOG_MSG_DPKG info;
//        info.msg = "testmsg";
//        info.action = "testaction";
//        info.dateTime = "testdatetime";
//        jList << info;
//    }
//    QString fileName = "./testExport.doc";
//    QStringList labels;

//    for (int i = 0; i < 2; ++i) {
//        labels << QString("testhead%1").arg(i);
//    }
//    p->m_canRunning = true;
//    bool result = p->exportToDoc(fileName, jList, labels);
//    EXPECT_EQ(result, true);
//    p->deleteLater();
//}

//TEST(LogExportThread_exportToDoc_DPKG_UT, LogExportThread_exportToDoc_DPKG_UT_002)
//{
//    Stub stub;
//    stub.set(ADDR(Docx::Document, save), Doc_Document_save);
//    LogExportThread *p = new LogExportThread(nullptr);
//    EXPECT_NE(p, nullptr);
//    QList<LOG_MSG_DPKG> jList;
//    for (int i = 0; i < 5; ++i) {
//        LOG_MSG_DPKG info;
//        info.msg = "testmsg";
//        info.action = "testaction";
//        info.dateTime = "testdatetime";
//        jList << info;
//    }
//    QString fileName = "";
//    QStringList labels;

//    for (int i = 0; i < 2; ++i) {
//        labels << QString("testhead%1").arg(i);
//    }
//    p->m_canRunning = true;
//    bool result = p->exportToDoc(fileName, jList, labels);
//    EXPECT_EQ(result, true);
//    p->deleteLater();
//}

//TEST(LogExportThread_exportToDoc_BOOT_UT, LogExportThread_exportToDoc_BOOT_UT_001)
//{
//    Stub stub;
//    stub.set(ADDR(Docx::Document, save), Doc_Document_save);
//    LogExportThread *p = new LogExportThread(nullptr);
//    EXPECT_NE(p, nullptr);
//    QList<LOG_MSG_BOOT> jList;
//    for (int i = 0; i < 5; ++i) {
//        LOG_MSG_BOOT info;
//        info.msg = "testmsg";
//        info.status = "teststatus";
//        jList << info;
//    }
//    QString fileName = "./testExport.doc";
//    QStringList labels;

//    for (int i = 0; i < 2; ++i) {
//        labels << QString("testhead%1").arg(i);
//    }
//    p->m_canRunning = true;
//    bool result = p->exportToDoc(fileName, jList, labels);
//    EXPECT_EQ(result, true);
//    p->deleteLater();
//}

//TEST(LogExportThread_exportToDoc_BOOT_UT, LogExportThread_exportToDoc_BOOT_UT_002)
//{
//    Stub stub;
//    stub.set(ADDR(Docx::Document, save), Doc_Document_save);
//    LogExportThread *p = new LogExportThread(nullptr);
//    EXPECT_NE(p, nullptr);
//    QList<LOG_MSG_BOOT> jList;
//    for (int i = 0; i < 5; ++i) {
//        LOG_MSG_BOOT info;
//        info.msg = "testmsg";
//        info.status = "teststatus";
//        jList << info;
//    }
//    QString fileName = "";
//    QStringList labels;

//    for (int i = 0; i < 2; ++i) {
//        labels << QString("testhead%1").arg(i);
//    }
//    p->m_canRunning = true;
//    bool result = p->exportToDoc(fileName, jList, labels);
//    EXPECT_EQ(result, true);
//    p->deleteLater();
//}

//TEST(LogExportThread_exportToDoc_XORG_UT, LogExportThread_exportToDoc_XORG_UT_001)
//{
//    Stub stub;
//    stub.set(ADDR(Docx::Document, save), Doc_Document_save);
//    LogExportThread *p = new LogExportThread(nullptr);
//    EXPECT_NE(p, nullptr);
//    QList<LOG_MSG_XORG> jList;
//    for (int i = 0; i < 5; ++i) {
//        LOG_MSG_XORG info;
//        info.msg = "testmsg";
//        info.dateTime = "testdateTime";
//        jList << info;
//    }

//    QString fileName = "./testExport.doc";
//    QStringList labels;

//    for (int i = 0; i < 2; ++i) {
//        labels << QString("testhead%1").arg(i);
//    }
//    p->m_canRunning = true;
//    bool result = p->exportToDoc(fileName, jList, labels);
//    EXPECT_EQ(result, true);
//    p->deleteLater();
//}

//TEST(LogExportThread_exportToDoc_XORG_UT, LogExportThread_exportToDoc_XORG_UT_002)
//{
//    Stub stub;
//    stub.set(ADDR(Docx::Document, save), Doc_Document_save);
//    LogExportThread *p = new LogExportThread(nullptr);
//    EXPECT_NE(p, nullptr);
//    QList<LOG_MSG_XORG> jList;
//    for (int i = 0; i < 5; ++i) {
//        LOG_MSG_XORG info;
//        info.msg = "testmsg";
//        info.dateTime = "testdateTime";
//        jList << info;
//    }
//    QString fileName = "";
//    QStringList labels;

//    for (int i = 0; i < 2; ++i) {
//        labels << QString("testhead%1").arg(i);
//    }
//    p->m_canRunning = true;
//    bool result = p->exportToDoc(fileName, jList, labels);
//    EXPECT_EQ(result, true);
//    p->deleteLater();
//}

//TEST(LogExportThread_exportToDoc_NORMAL_UT, LogExportThread_exportToDoc_NORMAL_UT_001)
//{
//    Stub stub;
//    stub.set(ADDR(Docx::Document, save), Doc_Document_save);
//    LogExportThread *p = new LogExportThread(nullptr);
//    EXPECT_NE(p, nullptr);
//    QList<LOG_MSG_NORMAL> jList;
//    for (int i = 0; i < 5; ++i) {
//        LOG_MSG_NORMAL info;
//        info.msg = "testmsg";
//        info.dateTime = "testdateTime";
//        info.userName = "test";
//        info.eventType = "test";
//        jList << info;
//    }
//    QString fileName = "./testExport.doc";
//    QStringList labels;

//    for (int i = 0; i < 2; ++i) {
//        labels << QString("testhead%1").arg(i);
//    }
//    p->m_canRunning = true;
//    bool result = p->exportToDoc(fileName, jList, labels);
//    EXPECT_EQ(result, true);
//    p->deleteLater();
//}

//TEST(LogExportThread_exportToDoc_NORMAL_UT, LogExportThread_exportToDoc_NORMAL_UT_002)
//{
//    LogExportThread *p = new LogExportThread(nullptr);
//    EXPECT_NE(p, nullptr);
//    QList<LOG_MSG_NORMAL> jList;
//    for (int i = 0; i < 5; ++i) {
//        LOG_MSG_NORMAL info;
//        info.msg = "testmsg";
//        info.dateTime = "testdateTime";
//        info.userName = "test";
//        info.eventType = "test";
//        jList << info;
//    }
//    QString fileName = "";
//    QStringList labels;

//    for (int i = 0; i < 2; ++i) {
//        labels << QString("testhead%1").arg(i);
//    }
//    p->m_canRunning = true;
//    bool result = p->exportToDoc(fileName, jList, labels);
//    EXPECT_EQ(result, true);
//    p->deleteLater();
//}

//TEST(LogExportThread_exportToDoc_KWIN_UT, LogExportThread_exportToDoc_KWIN_UT_001)
//{
//    LogExportThread *p = new LogExportThread(nullptr);
//    EXPECT_NE(p, nullptr);
//    QList<LOG_MSG_KWIN> jList;
//    for (int i = 0; i < 5; ++i) {
//        LOG_MSG_KWIN info;
//        info.msg = "testmsg";
//        jList << info;
//    }
//    QString fileName = "./testExport.doc";
//    QStringList labels;

//    for (int i = 0; i < 1; ++i) {
//        labels << QString("testhead%1").arg(i);
//    }
//    p->m_canRunning = true;
//    bool result = p->exportToDoc(fileName, jList, labels);
//    EXPECT_EQ(result, true);
//    p->deleteLater();
//}

//TEST(LogExportThread_exportToDoc_KWIN_UT, LogExportThread_exportToDoc_KWIN_UT_002)
//{
//    LogExportThread *p = new LogExportThread(nullptr);
//    EXPECT_NE(p, nullptr);
//    QList<LOG_MSG_KWIN> jList;
//    for (int i = 0; i < 5; ++i) {
//        LOG_MSG_KWIN info;
//        info.msg = "testmsg";
//        jList << info;
//    }
//    QString fileName = "";
//    QStringList labels;

//    for (int i = 0; i < 1; ++i) {
//        labels << QString("testhead%1").arg(i);
//    }
//    p->m_canRunning = true;
//    bool result = p->exportToDoc(fileName, jList, labels);
//    EXPECT_EQ(result, true);
//    p->deleteLater();
//}

//TEST(LogExportThread_exportToHtml_Model_UT, LogExportThread_exportToHtml_Model_UT_001)
//{
//    LogExportThread *p = new LogExportThread(nullptr);
//    EXPECT_NE(p, nullptr);
//    QStandardItemModel *p_model = new QStandardItemModel();
//    p_model->setHorizontalHeaderLabels(QStringList()
//                                       << Dtk::Widget::DApplication::translate("Table", "Level")
//                                       << Dtk::Widget::DApplication::translate("Table", "Date and Time")
//                                       << Dtk::Widget::DApplication::translate("Table", "Source")
//                                       << Dtk::Widget::DApplication::translate("Table", "Info"));
//    QList<QStandardItem *> items;
//    Dtk::Widget::DStandardItem *item = nullptr;
//    for (int i = 0; i < 5; i++) {
//        items.clear();
//        //int col = 0;
//        item = new Dtk::Widget::DStandardItem;
//        item->setData(QString("test%1").arg(i), Log_Item_SPACE::levelRole);
//        item->setAccessibleText(QString("treeview_context_%1_%2").arg(i).arg(0));
//        items << item;
//        item = new Dtk::Widget::DStandardItem("2000-10-10 10:10:10");
//        item->setAccessibleText(QString("treeview_context_%1_%2").arg(i).arg(1));
//        items << item;
//        //        item = new DStandardItem(list[i].src);
//        item = new Dtk::Widget::DStandardItem("deepinlogviewer");
//        item->setAccessibleText(QString("treeview_context_%1_%2").arg(i).arg(2));
//        items << item;
//        item = new Dtk::Widget::DStandardItem(QString("testmsg%1").arg(i));
//        item->setData(APP_TABLE_DATA);
//        item->setAccessibleText(QString("treeview_context_%1_%2").arg(i).arg(3));
//        items << item;
//        p_model->insertRow(p_model->rowCount(), items);
//    }

//    p->m_canRunning = true;
//    bool result = p->exportToHtml("", p_model, APP);
//    EXPECT_EQ(result, false);
//    p->deleteLater();
//}

//TEST(LogExportThread_exportToHtml_Model_UT, LogExportThread_exportToHtml_Model_UT_002)
//{
//    LogExportThread *p = new LogExportThread(nullptr);
//    EXPECT_NE(p, nullptr);
//    QStandardItemModel *p_model = new QStandardItemModel();
//    p_model->setHorizontalHeaderLabels(QStringList()
//                                       << Dtk::Widget::DApplication::translate("Table", "Level")
//                                       << Dtk::Widget::DApplication::translate("Table", "Date and Time")
//                                       << Dtk::Widget::DApplication::translate("Table", "Source")
//                                       << Dtk::Widget::DApplication::translate("Table", "Info"));
//    QList<QStandardItem *> items;
//    Dtk::Widget::DStandardItem *item = nullptr;
//    for (int i = 0; i < 5; i++) {
//        items.clear();
//        //int col = 0;
//        item = new Dtk::Widget::DStandardItem;

//        item->setData(APP_TABLE_DATA);
//        item->setData(QString("test%1").arg(i), Log_Item_SPACE::levelRole);
//        item->setAccessibleText(QString("treeview_context_%1_%2").arg(i).arg(0));
//        items << item;
//        item = new Dtk::Widget::DStandardItem("2000-10-10 10:10:10");
//        item->setData(APP_TABLE_DATA);
//        item->setAccessibleText(QString("treeview_context_%1_%2").arg(i).arg(1));
//        items << item;
//        //        item = new DStandardItem(list[i].src);
//        item = new Dtk::Widget::DStandardItem("deepinlogviewer");
//        item->setData(APP_TABLE_DATA);
//        item->setAccessibleText(QString("treeview_context_%1_%2").arg(i).arg(2));
//        items << item;
//        item = new Dtk::Widget::DStandardItem(QString("testmsg%1").arg(i));
//        item->setData(APP_TABLE_DATA);
//        item->setAccessibleText(QString("treeview_context_%1_%2").arg(i).arg(3));
//        items << item;
//        p_model->insertRow(p_model->rowCount(), items);
//    }
//    p->m_canRunning = true;
//    bool result = p->exportToHtml("./testExport.doc", p_model, APP);
//    EXPECT_EQ(result, true);
//    p->deleteLater();
//}

//TEST(LogExportThread_exportToHtml_Model_UT, LogExportThread_exportToHtml_Model_UT003)
//{
//    LogExportThread *p = new LogExportThread(nullptr);
//    EXPECT_NE(p, nullptr);
//    QStandardItemModel *p_model = new QStandardItemModel();
//    p_model->setHorizontalHeaderLabels(QStringList()
//                                       << Dtk::Widget::DApplication::translate("Table", "Level")
//                                       << Dtk::Widget::DApplication::translate("Table", "Date and Time")
//                                       << Dtk::Widget::DApplication::translate("Table", "Source")
//                                       << Dtk::Widget::DApplication::translate("Table", "Info"));
//    QList<QStandardItem *> items;
//    Dtk::Widget::DStandardItem *item = nullptr;
//    for (int i = 0; i < 5; i++) {
//        items.clear();
//        //int col = 0;

//        item = new Dtk::Widget::DStandardItem;
//        item->setData(APP_TABLE_DATA);
//        item->setData(QString("test%1").arg(i), Log_Item_SPACE::levelRole);
//        item->setAccessibleText(QString("treeview_context_%1_%2").arg(i).arg(0));
//        items << item;
//        item = new Dtk::Widget::DStandardItem("2000-10-10 10:10:10");
//        item->setData(APP_TABLE_DATA);
//        item->setAccessibleText(QString("treeview_context_%1_%2").arg(i).arg(1));
//        items << item;
//        //        item = new DStandardItem(list[i].src);
//        item = new Dtk::Widget::DStandardItem("deepinlogviewer");
//        item->setData(APP_TABLE_DATA);
//        item->setAccessibleText(QString("treeview_context_%1_%2").arg(i).arg(2));
//        items << item;
//        item = new Dtk::Widget::DStandardItem(QString("testmsg%1").arg(i));
//        item->setData(APP_TABLE_DATA);
//        item->setAccessibleText(QString("treeview_context_%1_%2").arg(i).arg(3));
//        items << item;
//        p_model->insertRow(p_model->rowCount(), items);
//    }
//    p->m_canRunning = true;
//    bool result = p->exportToHtml("./testExport.doc", p_model, JOURNAL);
//    EXPECT_EQ(result, true);
//    p->deleteLater();
//}

class LogExportThread_exportToHtml_JOURNAL_UT_Param
{
public:
    LogExportThread_exportToHtml_JOURNAL_UT_Param(bool iCanFileRead, LOG_FLAG iFlag, bool iResult)
        : canFileRead(iCanFileRead)
        , flag(iFlag)
        , result(iResult)
    {
    }
    bool canFileRead;
    LOG_FLAG flag;
    bool result;
};

class LogExportThread_exportToHtml_JOURNAL_UT : public ::testing::TestWithParam<LogExportThread_exportToHtml_JOURNAL_UT_Param>
{
};

INSTANTIATE_TEST_CASE_P(LogApplication, LogExportThread_exportToHtml_JOURNAL_UT, ::testing::Values(LogExportThread_exportToHtml_JOURNAL_UT_Param(false, KERN, false), LogExportThread_exportToHtml_JOURNAL_UT_Param(true, JOURNAL, true), LogExportThread_exportToHtml_JOURNAL_UT_Param(true, KERN, true), LogExportThread_exportToHtml_JOURNAL_UT_Param(true, APP, true)));

TEST_P(LogExportThread_exportToHtml_JOURNAL_UT, LogExportThread_exportToHtml_JOURNAL_UT)
{
    //    LogExportThread *p = new LogExportThread(nullptr);
    //    EXPECT_NE(p, nullptr);
    //    LogExportThread_exportToHtml_JOURNAL_UT_Param param = GetParam();
    //    QList<LOG_MSG_JOURNAL> jList;
    //    for (int i = 0; i < 5; ++i) {
    //        LOG_MSG_JOURNAL info;
    //        info.msg = "testmsg";
    //        info.level = "testlevel";
    //        info.daemonId = "testid";
    //        info.dateTime = "testdatetime";
    //        info.hostName = "testhostName";
    //        info.daemonName = "testdaemonName";
    //        jList << info;
    //    }
    //    QString fileName = param.canFileRead ? "./testExport.doc" : "";
    //    QStringList labels;

    //    for (int i = 0; i < 2; ++i) {
    //        labels << QString("testhead%1").arg(i);
    //    }
    //    p->m_canRunning = true;
    //    bool result = p->exportToHtml(fileName, jList, labels, param.flag);
    //    EXPECT_EQ(result, param.result);
    //    p->deleteLater();
}

//TEST(LogExportThread_exportToHtml_APP_UT, LogExportThread_exportToHtml_APP_UT_001)
//{
//    LogExportThread *p = new LogExportThread(nullptr);
//    EXPECT_NE(p, nullptr);
//    QList<LOG_MSG_APPLICATOIN> jList;
//    for (int i = 0; i < 5; ++i) {
//        LOG_MSG_APPLICATOIN info;
//        info.msg = "testmsg";
//        info.level = "testlevel";
//        info.dateTime = "testdatetime";
//        info.src = "testsrc";
//        jList << info;
//    }
//    QString fileName = "./testExport.doc";
//    QStringList labels;

//    for (int i = 0; i < 2; ++i) {
//        labels << QString("testhead%1").arg(i);
//    }
//    p->m_canRunning = true;
//    QString appname("testapp");
//    bool result = p->exportToHtml(fileName, jList, labels, appname);
//    EXPECT_EQ(result, true);
//    p->deleteLater();
//}

//TEST(LogExportThread_exportToHtml_APP_UT, LogExportThread_exportToHtml_APP_UT_002)
//{
//    LogExportThread *p = new LogExportThread(nullptr);
//    EXPECT_NE(p, nullptr);
//    QList<LOG_MSG_APPLICATOIN> jList;
//    for (int i = 0; i < 5; ++i) {
//        LOG_MSG_APPLICATOIN info;
//        info.msg = "testmsg";
//        info.level = "testlevel";
//        info.dateTime = "testdatetime";
//        info.src = "testsrc";
//        jList << info;
//    }
//    QString fileName = "";
//    QStringList labels;

//    for (int i = 0; i < 2; ++i) {
//        labels << QString("testhead%1").arg(i);
//    }
//    p->m_canRunning = true;
//    QString appname("testapp");
//    bool result = p->exportToHtml(fileName, jList, labels, appname);
//    EXPECT_EQ(result, false);
//    p->deleteLater();
//}

//TEST(LogExportThread_exportToHtml_DPKG_UT, LogExportThread_exportToHtml_DPKG_UT_001)
//{
//    LogExportThread *p = new LogExportThread(nullptr);
//    EXPECT_NE(p, nullptr);
//    QList<LOG_MSG_DPKG> jList;
//    for (int i = 0; i < 5; ++i) {
//        LOG_MSG_DPKG info;
//        info.msg = "testmsg";
//        info.action = "testaction";
//        info.dateTime = "testdatetime";
//        jList << info;
//    }
//    QString fileName = "./testExport.doc";
//    QStringList labels;

//    for (int i = 0; i < 2; ++i) {
//        labels << QString("testhead%1").arg(i);
//    }
//    p->m_canRunning = true;
//    bool result = p->exportToHtml(fileName, jList, labels);
//    EXPECT_EQ(result, true);
//    p->deleteLater();
//}

//TEST(LogExportThread_exportToHtml_DPKG_UT, LogExportThread_exportToHtml_DPKG_UT_002)
//{
//    LogExportThread *p = new LogExportThread(nullptr);
//    EXPECT_NE(p, nullptr);
//    QList<LOG_MSG_DPKG> jList;
//    for (int i = 0; i < 5; ++i) {
//        LOG_MSG_DPKG info;
//        info.msg = "testmsg";
//        info.action = "testaction";
//        info.dateTime = "testdatetime";
//        jList << info;
//    }
//    QString fileName = "";
//    QStringList labels;

//    for (int i = 0; i < 2; ++i) {
//        labels << QString("testhead%1").arg(i);
//    }
//    p->m_canRunning = true;
//    bool result = p->exportToHtml(fileName, jList, labels);
//    EXPECT_EQ(result, false);
//    p->deleteLater();
//}

//TEST(LogExportThread_exportToHtml_BOOT_UT, LogExportThread_exportToHtml_BOOT_UT_001)
//{
//    LogExportThread *p = new LogExportThread(nullptr);
//    EXPECT_NE(p, nullptr);
//    QList<LOG_MSG_BOOT> jList;
//    for (int i = 0; i < 5; ++i) {
//        LOG_MSG_BOOT info;
//        info.msg = "testmsg";
//        info.status = "teststatus";
//        jList << info;
//    }
//    QString fileName = "./testExport.doc";
//    QStringList labels;

//    for (int i = 0; i < 2; ++i) {
//        labels << QString("testhead%1").arg(i);
//    }
//    p->m_canRunning = true;
//    bool result = p->exportToHtml(fileName, jList, labels);
//    EXPECT_EQ(result, true);
//    p->deleteLater();
//}

//TEST(LogExportThread_exportToHtml_BOOT_UT, LogExportThread_exportToHtml_BOOT_UT_002)
//{
//    LogExportThread *p = new LogExportThread(nullptr);
//    EXPECT_NE(p, nullptr);
//    QList<LOG_MSG_BOOT> jList;
//    for (int i = 0; i < 5; ++i) {
//        LOG_MSG_BOOT info;
//        info.msg = "testmsg";
//        info.status = "teststatus";
//        jList << info;
//    }
//    QString fileName = "";
//    QStringList labels;

//    for (int i = 0; i < 2; ++i) {
//        labels << QString("testhead%1").arg(i);
//    }
//    p->m_canRunning = true;
//    bool result = p->exportToHtml(fileName, jList, labels);
//    EXPECT_EQ(result, false);
//    p->deleteLater();
//}

//TEST(LogExportThread_exportToHtml_XORG_UT, LogExportThread_exportToHtml_XORG_UT_001)
//{
//    LogExportThread *p = new LogExportThread(nullptr);
//    EXPECT_NE(p, nullptr);
//    QList<LOG_MSG_XORG> jList;
//    for (int i = 0; i < 5; ++i) {
//        LOG_MSG_XORG info;
//        info.msg = "testmsg";
//        info.dateTime = "testdateTime";
//        jList << info;
//    }

//    QString fileName = "./testExport.doc";
//    QStringList labels;

//    for (int i = 0; i < 2; ++i) {
//        labels << QString("testhead%1").arg(i);
//    }
//    p->m_canRunning = true;
//    bool result = p->exportToHtml(fileName, jList, labels);
//    EXPECT_EQ(result, true);
//    p->deleteLater();
//}

//TEST(LogExportThread_exportToHtml_XORG_UT, LogExportThread_exportToHtml_XORG_UT_002)
//{
//    LogExportThread *p = new LogExportThread(nullptr);
//    EXPECT_NE(p, nullptr);
//    QList<LOG_MSG_XORG> jList;
//    for (int i = 0; i < 5; ++i) {
//        LOG_MSG_XORG info;
//        info.msg = "testmsg";
//        info.dateTime = "testdateTime";
//        jList << info;
//    }
//    QString fileName = "";
//    QStringList labels;

//    for (int i = 0; i < 2; ++i) {
//        labels << QString("testhead%1").arg(i);
//    }
//    p->m_canRunning = true;
//    bool result = p->exportToHtml(fileName, jList, labels);
//    EXPECT_EQ(result, false);
//    p->deleteLater();
//}

//TEST(LogExportThread_exportToHtml_NORMAL_UT, LogExportThread_exportToHtml_NORMAL_UT_001)
//{
//    LogExportThread *p = new LogExportThread(nullptr);
//    EXPECT_NE(p, nullptr);
//    QList<LOG_MSG_NORMAL> jList;
//    for (int i = 0; i < 5; ++i) {
//        LOG_MSG_NORMAL info;
//        info.msg = "testmsg";
//        info.dateTime = "testdateTime";
//        info.userName = "test";
//        info.eventType = "test";
//        jList << info;
//    }
//    QString fileName = "./testExport.doc";
//    QStringList labels;

//    for (int i = 0; i < 2; ++i) {
//        labels << QString("testhead%1").arg(i);
//    }
//    p->m_canRunning = true;
//    bool result = p->exportToHtml(fileName, jList, labels);
//    EXPECT_EQ(result, true);
//    p->deleteLater();
//}

//TEST(LogExportThread_exportToHtml_NORMAL_UT, LogExportThread_exportToHtml_NORMAL_UT_002)
//{
//    LogExportThread *p = new LogExportThread(nullptr);
//    EXPECT_NE(p, nullptr);
//    QList<LOG_MSG_NORMAL> jList;
//    for (int i = 0; i < 5; ++i) {
//        LOG_MSG_NORMAL info;
//        info.msg = "testmsg";
//        info.dateTime = "testdateTime";
//        info.userName = "test";
//        info.eventType = "test";
//        jList << info;
//    }
//    QString fileName = "";
//    QStringList labels;

//    for (int i = 0; i < 2; ++i) {
//        labels << QString("testhead%1").arg(i);
//    }
//    p->m_canRunning = true;
//    bool result = p->exportToHtml(fileName, jList, labels);
//    EXPECT_EQ(result, false);
//    p->deleteLater();
//}

//TEST(LogExportThread_exportToHtml_KWIN_UT, LogExportThread_exportToHtml_KWIN_UT_001)
//{
//    LogExportThread *p = new LogExportThread(nullptr);
//    EXPECT_NE(p, nullptr);
//    QList<LOG_MSG_KWIN> jList;
//    for (int i = 0; i < 5; ++i) {
//        LOG_MSG_KWIN info;
//        info.msg = "testmsg";
//        jList << info;
//    }
//    QString fileName = "./testExport.doc";
//    QStringList labels;

//    for (int i = 0; i < 1; ++i) {
//        labels << QString("testhead%1").arg(i);
//    }
//    p->m_canRunning = true;
//    bool result = p->exportToHtml(fileName, jList, labels);
//    EXPECT_EQ(result, true);
//    p->deleteLater();
//}

//TEST(LogExportThread_exportToHtml_KWIN_UT, LogExportThread_exportToHtml_KWIN_UT_002)
//{
//    LogExportThread *p = new LogExportThread(nullptr);
//    EXPECT_NE(p, nullptr);
//    QList<LOG_MSG_KWIN> jList;
//    for (int i = 0; i < 5; ++i) {
//        LOG_MSG_KWIN info;
//        info.msg = "testmsg";
//        jList << info;
//    }
//    QString fileName = "";
//    QStringList labels;

//    for (int i = 0; i < 1; ++i) {
//        labels << QString("testhead%1").arg(i);
//    }
//    p->m_canRunning = true;
//    bool result = p->exportToHtml(fileName, jList, labels);
//    EXPECT_EQ(result, false);
//    p->deleteLater();
//}

//TEST(LogExportThread_exportToXls_Model_UT, LogExportThread_exportToXls_Model_UT_001)
//{
//    LogExportThread *p = new LogExportThread(nullptr);
//    EXPECT_NE(p, nullptr);
//    QStandardItemModel *p_model = new QStandardItemModel();
//    p_model->setHorizontalHeaderLabels(QStringList()
//                                       << Dtk::Widget::DApplication::translate("Table", "Level")
//                                       << Dtk::Widget::DApplication::translate("Table", "Date and Time")
//                                       << Dtk::Widget::DApplication::translate("Table", "Source")
//                                       << Dtk::Widget::DApplication::translate("Table", "Info"));
//    QList<QStandardItem *> items;
//    Dtk::Widget::DStandardItem *item = nullptr;
//    for (int i = 0; i < 5; i++) {
//        items.clear();
//        //int col = 0;
//        item = new Dtk::Widget::DStandardItem;
//        item->setData(QString("test%1").arg(i), Log_Item_SPACE::levelRole);
//        item->setAccessibleText(QString("treeview_context_%1_%2").arg(i).arg(0));
//        items << item;
//        item = new Dtk::Widget::DStandardItem("2000-10-10 10:10:10");
//        item->setAccessibleText(QString("treeview_context_%1_%2").arg(i).arg(1));
//        items << item;
//        //        item = new DStandardItem(list[i].src);
//        item = new Dtk::Widget::DStandardItem("deepinlogviewer");
//        item->setAccessibleText(QString("treeview_context_%1_%2").arg(i).arg(2));
//        items << item;
//        item = new Dtk::Widget::DStandardItem(QString("testmsg%1").arg(i));
//        item->setData(APP_TABLE_DATA);
//        item->setAccessibleText(QString("treeview_context_%1_%2").arg(i).arg(3));
//        items << item;
//        p_model->insertRow(p_model->rowCount(), items);
//    }

//    p->m_canRunning = true;
//    bool result = p->exportToXls("", p_model, APP);
//    EXPECT_EQ(result, false);
//    p->deleteLater();
//}

//TEST(LogExportThread_exportToXls_Model_UT, LogExportThread_exportToXls_Model_UT_002)
//{
//    LogExportThread *p = new LogExportThread(nullptr);
//    EXPECT_NE(p, nullptr);
//    QStandardItemModel *p_model = new QStandardItemModel();
//    p_model->setHorizontalHeaderLabels(QStringList()
//                                       << Dtk::Widget::DApplication::translate("Table", "Level")
//                                       << Dtk::Widget::DApplication::translate("Table", "Date and Time")
//                                       << Dtk::Widget::DApplication::translate("Table", "Source")
//                                       << Dtk::Widget::DApplication::translate("Table", "Info"));
//    QList<QStandardItem *> items;
//    Dtk::Widget::DStandardItem *item = nullptr;
//    for (int i = 0; i < 5; i++) {
//        items.clear();
//        //int col = 0;
//        item = new Dtk::Widget::DStandardItem;

//        item->setData(APP_TABLE_DATA);
//        item->setData(QString("test%1").arg(i), Log_Item_SPACE::levelRole);
//        item->setAccessibleText(QString("treeview_context_%1_%2").arg(i).arg(0));
//        items << item;
//        item = new Dtk::Widget::DStandardItem("2000-10-10 10:10:10");
//        item->setData(APP_TABLE_DATA);
//        item->setAccessibleText(QString("treeview_context_%1_%2").arg(i).arg(1));
//        items << item;
//        //        item = new DStandardItem(list[i].src);
//        item = new Dtk::Widget::DStandardItem("deepinlogviewer");
//        item->setData(APP_TABLE_DATA);
//        item->setAccessibleText(QString("treeview_context_%1_%2").arg(i).arg(2));
//        items << item;
//        item = new Dtk::Widget::DStandardItem(QString("testmsg%1").arg(i));
//        item->setData(APP_TABLE_DATA);
//        item->setAccessibleText(QString("treeview_context_%1_%2").arg(i).arg(3));
//        items << item;
//        p_model->insertRow(p_model->rowCount(), items);
//    }
//    p->m_canRunning = true;
//    bool result = p->exportToXls("./testExport.doc", p_model, APP);
//    EXPECT_EQ(result, true);
//    p->deleteLater();
//}

//TEST(LogExportThread_exportToXls_Model_UT, LogExportThread_exportToXls_Model_UT003)
//{
//    LogExportThread *p = new LogExportThread(nullptr);
//    EXPECT_NE(p, nullptr);
//    QStandardItemModel *p_model = new QStandardItemModel();
//    p_model->setHorizontalHeaderLabels(QStringList()
//                                       << Dtk::Widget::DApplication::translate("Table", "Level")
//                                       << Dtk::Widget::DApplication::translate("Table", "Date and Time")
//                                       << Dtk::Widget::DApplication::translate("Table", "Source")
//                                       << Dtk::Widget::DApplication::translate("Table", "Info"));
//    QList<QStandardItem *> items;
//    Dtk::Widget::DStandardItem *item = nullptr;
//    for (int i = 0; i < 5; i++) {
//        items.clear();
//        //int col = 0;

//        item = new Dtk::Widget::DStandardItem;
//        item->setData(APP_TABLE_DATA);
//        item->setData(QString("test%1").arg(i), Log_Item_SPACE::levelRole);
//        item->setAccessibleText(QString("treeview_context_%1_%2").arg(i).arg(0));
//        items << item;
//        item = new Dtk::Widget::DStandardItem("2000-10-10 10:10:10");
//        item->setData(APP_TABLE_DATA);
//        item->setAccessibleText(QString("treeview_context_%1_%2").arg(i).arg(1));
//        items << item;
//        //        item = new DStandardItem(list[i].src);
//        item = new Dtk::Widget::DStandardItem("deepinlogviewer");
//        item->setData(APP_TABLE_DATA);
//        item->setAccessibleText(QString("treeview_context_%1_%2").arg(i).arg(2));
//        items << item;
//        item = new Dtk::Widget::DStandardItem(QString("testmsg%1").arg(i));
//        item->setData(APP_TABLE_DATA);
//        item->setAccessibleText(QString("treeview_context_%1_%2").arg(i).arg(3));
//        items << item;
//        p_model->insertRow(p_model->rowCount(), items);
//    }
//    p->m_canRunning = true;
//    bool result = p->exportToXls("./testExport.doc", p_model, JOURNAL);
//    EXPECT_EQ(result, true);
//    p->deleteLater();
//}

//class LogExportThread_exportToXls_JOURNAL_UT_Param
//{
//public:
//    LogExportThread_exportToXls_JOURNAL_UT_Param(bool iCanFileRead, LOG_FLAG iFlag, bool iResult)
//    {
//        canFileRead = iCanFileRead;
//        flag = iFlag;
//        result = iResult;
//    }
//    bool canFileRead;
//    LOG_FLAG flag;
//    bool result;
//};

//class LogExportThread_exportToXls_JOURNAL_UT : public ::testing::TestWithParam<LogExportThread_exportToXls_JOURNAL_UT_Param>
//{
//};

//INSTANTIATE_TEST_CASE_P(LogApplication, LogExportThread_exportToXls_JOURNAL_UT, ::testing::Values(LogExportThread_exportToXls_JOURNAL_UT_Param(false, KERN, false), LogExportThread_exportToXls_JOURNAL_UT_Param(true, JOURNAL, true), LogExportThread_exportToXls_JOURNAL_UT_Param(true, KERN, true), LogExportThread_exportToXls_JOURNAL_UT_Param(true, APP, true)));

//TEST_P(LogExportThread_exportToXls_JOURNAL_UT, LogExportThread_exportToXls_JOURNAL_UT)
//{
//    //    LogExportThread *p = new LogExportThread(nullptr);
//    //    EXPECT_NE(p, nullptr);
//    //    LogExportThread_exportToXls_JOURNAL_UT_Param param = GetParam();
//    //    QList<LOG_MSG_JOURNAL> jList;
//    //    for (int i = 0; i < 5; ++i) {
//    //        LOG_MSG_JOURNAL info;
//    //        info.msg = "testmsg";
//    //        info.level = "testlevel";
//    //        info.daemonId = "testid";
//    //        info.dateTime = "testdatetime";
//    //        info.hostName = "testhostName";
//    //        info.daemonName = "testdaemonName";
//    //        jList << info;
//    //    }
//    //    QString fileName = param.canFileRead ? "./testExport.doc" : "";
//    //    QStringList labels;

//    //    for (int i = 0; i < 2; ++i) {
//    //        labels << QString("testhead%1").arg(i);
//    //    }
//    //    p->m_canRunning = true;
//    //    bool result = p->exportToXls(fileName, jList, labels, param.flag);
//    //    EXPECT_EQ(result, param.result);
//    //    p->deleteLater();
//}

//TEST(LogExportThread_exportToXls_APP_UT, LogExportThread_exportToXls_APP_UT_001)
//{
//    LogExportThread *p = new LogExportThread(nullptr);
//    EXPECT_NE(p, nullptr);
//    QList<LOG_MSG_APPLICATOIN> jList;
//    for (int i = 0; i < 5; ++i) {
//        LOG_MSG_APPLICATOIN info;
//        info.msg = "testmsg";
//        info.level = "testlevel";
//        info.dateTime = "testdatetime";
//        info.src = "testsrc";
//        jList << info;
//    }
//    QString fileName = "./testExport.doc";
//    QStringList labels;

//    for (int i = 0; i < 2; ++i) {
//        labels << QString("testhead%1").arg(i);
//    }
//    p->m_canRunning = true;
//    QString appname("testapp");
//    bool result = p->exportToXls(fileName, jList, labels, appname);
//    EXPECT_EQ(result, true);
//    p->deleteLater();
//}

//TEST(LogExportThread_exportToXls_APP_UT, LogExportThread_exportToXls_APP_UT_002)
//{
//    LogExportThread *p = new LogExportThread(nullptr);
//    EXPECT_NE(p, nullptr);
//    QList<LOG_MSG_APPLICATOIN> jList;
//    for (int i = 0; i < 5; ++i) {
//        LOG_MSG_APPLICATOIN info;
//        info.msg = "testmsg";
//        info.level = "testlevel";
//        info.dateTime = "testdatetime";
//        info.src = "testsrc";
//        jList << info;
//    }
//    QString fileName = "";
//    QStringList labels;

//    for (int i = 0; i < 2; ++i) {
//        labels << QString("testhead%1").arg(i);
//    }
//    p->m_canRunning = true;
//    QString appname("testapp");
//    bool result = p->exportToXls(fileName, jList, labels, appname);
//    EXPECT_EQ(result, false);
//    p->deleteLater();
//}

//TEST(LogExportThread_exportToXls_DPKG_UT, LogExportThread_exportToXls_DPKG_UT_001)
//{
//    LogExportThread *p = new LogExportThread(nullptr);
//    EXPECT_NE(p, nullptr);
//    QList<LOG_MSG_DPKG> jList;
//    for (int i = 0; i < 5; ++i) {
//        LOG_MSG_DPKG info;
//        info.msg = "testmsg";
//        info.action = "testaction";
//        info.dateTime = "testdatetime";
//        jList << info;
//    }
//    QString fileName = "./testExport.doc";
//    QStringList labels;

//    for (int i = 0; i < 2; ++i) {
//        labels << QString("testhead%1").arg(i);
//    }
//    p->m_canRunning = true;
//    bool result = p->exportToXls(fileName, jList, labels);
//    EXPECT_EQ(result, true);
//    p->deleteLater();
//}

//TEST(LogExportThread_exportToXls_DPKG_UT, LogExportThread_exportToXls_DPKG_UT_002)
//{
//    LogExportThread *p = new LogExportThread(nullptr);
//    EXPECT_NE(p, nullptr);
//    QList<LOG_MSG_DPKG> jList;
//    for (int i = 0; i < 5; ++i) {
//        LOG_MSG_DPKG info;
//        info.msg = "testmsg";
//        info.action = "testaction";
//        info.dateTime = "testdatetime";
//        jList << info;
//    }
//    QString fileName = "";
//    QStringList labels;

//    for (int i = 0; i < 2; ++i) {
//        labels << QString("testhead%1").arg(i);
//    }
//    p->m_canRunning = false;
//    bool result = p->exportToXls(fileName, jList, labels);
//    EXPECT_EQ(result, false);
//    p->deleteLater();
//}

//TEST(LogExportThread_exportToXls_BOOT_UT, LogExportThread_exportToXls_BOOT_UT_001)
//{
//    LogExportThread *p = new LogExportThread(nullptr);
//    EXPECT_NE(p, nullptr);
//    QList<LOG_MSG_BOOT> jList;
//    for (int i = 0; i < 5; ++i) {
//        LOG_MSG_BOOT info;
//        info.msg = "testmsg";
//        info.status = "teststatus";
//        jList << info;
//    }
//    QString fileName = "./testExport.doc";
//    QStringList labels;

//    for (int i = 0; i < 2; ++i) {
//        labels << QString("testhead%1").arg(i);
//    }
//    p->m_canRunning = true;
//    bool result = p->exportToXls(fileName, jList, labels);
//    EXPECT_EQ(result, true);
//    p->deleteLater();
//}

//TEST(LogExportThread_exportToXls_BOOT_UT, LogExportThread_exportToXls_BOOT_UT_002)
//{
//    LogExportThread *p = new LogExportThread(nullptr);
//    EXPECT_NE(p, nullptr);
//    QList<LOG_MSG_BOOT> jList;
//    for (int i = 0; i < 5; ++i) {
//        LOG_MSG_BOOT info;
//        info.msg = "testmsg";
//        info.status = "teststatus";
//        jList << info;
//    }
//    QString fileName = "";
//    QStringList labels;

//    for (int i = 0; i < 2; ++i) {
//        labels << QString("testhead%1").arg(i);
//    }
//    p->m_canRunning = true;
//    bool result = p->exportToXls(fileName, jList, labels);
//    EXPECT_EQ(result, false);
//    p->deleteLater();
//}

//TEST(LogExportThread_exportToXls_XORG_UT, LogExportThread_exportToXls_XORG_UT_001)
//{
//    LogExportThread *p = new LogExportThread(nullptr);
//    EXPECT_NE(p, nullptr);
//    QList<LOG_MSG_XORG> jList;
//    for (int i = 0; i < 5; ++i) {
//        LOG_MSG_XORG info;
//        info.msg = "testmsg";
//        info.dateTime = "testdateTime";
//        jList << info;
//    }

//    QString fileName = "./testExport.doc";
//    QStringList labels;

//    for (int i = 0; i < 2; ++i) {
//        labels << QString("testhead%1").arg(i);
//    }
//    p->m_canRunning = true;
//    bool result = p->exportToXls(fileName, jList, labels);
//    EXPECT_EQ(result, true);
//    p->deleteLater();
//}

//TEST(LogExportThread_exportToXls_XORG_UT, LogExportThread_exportToXls_XORG_UT_002)
//{
//    LogExportThread *p = new LogExportThread(nullptr);
//    EXPECT_NE(p, nullptr);
//    QList<LOG_MSG_XORG> jList;
//    for (int i = 0; i < 5; ++i) {
//        LOG_MSG_XORG info;
//        info.msg = "testmsg";
//        info.dateTime = "testdateTime";
//        jList << info;
//    }
//    QString fileName = "";
//    QStringList labels;

//    for (int i = 0; i < 2; ++i) {
//        labels << QString("testhead%1").arg(i);
//    }
//    p->m_canRunning = true;
//    bool result = p->exportToXls(fileName, jList, labels);
//    EXPECT_EQ(result, false);
//    p->deleteLater();
//}

//TEST(LogExportThread_exportToXls_NORMAL_UT, LogExportThread_exportToXls_NORMAL_UT_001)
//{
//    LogExportThread *p = new LogExportThread(nullptr);
//    EXPECT_NE(p, nullptr);
//    QList<LOG_MSG_NORMAL> jList;
//    for (int i = 0; i < 5; ++i) {
//        LOG_MSG_NORMAL info;
//        info.msg = "testmsg";
//        info.dateTime = "testdateTime";
//        info.userName = "test";
//        info.eventType = "test";
//        jList << info;
//    }
//    QString fileName = "./testExport.doc";
//    QStringList labels;

//    for (int i = 0; i < 2; ++i) {
//        labels << QString("testhead%1").arg(i);
//    }
//    p->m_canRunning = true;
//    bool result = p->exportToXls(fileName, jList, labels);
//    EXPECT_EQ(result, true);
//    p->deleteLater();
//}

//TEST(LogExportThread_exportToXls_NORMAL_UT, LogExportThread_exportToXls_NORMAL_UT_002)
//{
//    LogExportThread *p = new LogExportThread(nullptr);
//    EXPECT_NE(p, nullptr);
//    QList<LOG_MSG_NORMAL> jList;
//    for (int i = 0; i < 5; ++i) {
//        LOG_MSG_NORMAL info;
//        info.msg = "testmsg";
//        info.dateTime = "testdateTime";
//        info.userName = "test";
//        info.eventType = "test";
//        jList << info;
//    }
//    QString fileName = "";
//    QStringList labels;

//    for (int i = 0; i < 2; ++i) {
//        labels << QString("testhead%1").arg(i);
//    }
//    p->m_canRunning = true;
//    bool result = p->exportToXls(fileName, jList, labels);
//    EXPECT_EQ(result, false);
//    p->deleteLater();
//}

//TEST(LogExportThread_exportToXls_KWIN_UT, LogExportThread_exportToXls_KWIN_UT_001)
//{
//    LogExportThread *p = new LogExportThread(nullptr);
//    EXPECT_NE(p, nullptr);
//    QList<LOG_MSG_KWIN> jList;
//    for (int i = 0; i < 5; ++i) {
//        LOG_MSG_KWIN info;
//        info.msg = "testmsg";
//        jList << info;
//    }
//    QString fileName = "./testExport.doc";
//    QStringList labels;

//    for (int i = 0; i < 1; ++i) {
//        labels << QString("testhead%1").arg(i);
//    }
//    p->m_canRunning = true;
//    bool result = p->exportToXls(fileName, jList, labels);
//    EXPECT_EQ(result, true);
//    p->deleteLater();
//}

//TEST(LogExportThread_exportToXls_KWIN_UT, LogExportThread_exportToXls_KWIN_UT_002)
//{
//    LogExportThread *p = new LogExportThread(nullptr);
//    EXPECT_NE(p, nullptr);
//    QList<LOG_MSG_KWIN> jList;
//    for (int i = 0; i < 5; ++i) {
//        LOG_MSG_KWIN info;
//        info.msg = "testmsg";
//        jList << info;
//    }
//    QString fileName = "";
//    QStringList labels;

//    for (int i = 0; i < 1; ++i) {
//        labels << QString("testhead%1").arg(i);
//    }
//    p->m_canRunning = true;
//    bool result = p->exportToXls(fileName, jList, labels);
//    EXPECT_EQ(result, false);
//    p->deleteLater();
//}

//TEST(LogExportThread_initMap_UT, LogExportThread_initMap_UT)
//{
//    LogExportThread *p = new LogExportThread(nullptr);
//    EXPECT_NE(p, nullptr);
//    p->initMap();
//    QMap<QString, QString> levelStrMap;
//    levelStrMap.insert("Emergency", Dtk::Widget::DApplication::translate("Level", "Emergency"));
//    levelStrMap.insert("Alert", Dtk::Widget::DApplication::translate("Level", "Alert"));
//    levelStrMap.insert("Critical", Dtk::Widget::DApplication::translate("Level", "Critical"));
//    levelStrMap.insert("Error", Dtk::Widget::DApplication::translate("Level", "Error"));
//    levelStrMap.insert("Warning", Dtk::Widget::DApplication::translate("Level", "Warning"));
//    levelStrMap.insert("Notice", Dtk::Widget::DApplication::translate("Level", "Notice"));
//    levelStrMap.insert("Info", Dtk::Widget::DApplication::translate("Level", "Info"));
//    levelStrMap.insert("Debug", Dtk::Widget::DApplication::translate("Level", "Debug"));
//    bool result = true;
//    foreach (QString key, p->m_levelStrMap.keys()) {
//        if (levelStrMap.value(key, "") != p->m_levelStrMap.value(key, "")) {
//            result = false;
//        }
//    }
//    EXPECT_EQ(result, true);
//    p->deleteLater();
//}

//class LogExportThread_strTranslate_UT_Param
//{
//public:
//    LogExportThread_strTranslate_UT_Param(const QString &iKey, const QString &iValue)
//    {
//        key = iKey;
//        value = iValue;
//    }
//    QString key;
//    QString value;
//};

//class LogExportThread_strTranslate_UT : public ::testing::TestWithParam<LogExportThread_strTranslate_UT_Param>
//{
//};

//INSTANTIATE_TEST_CASE_P(LogExportThread, LogExportThread_strTranslate_UT, ::testing::Values(LogExportThread_strTranslate_UT_Param("Emergency", Dtk::Widget::DApplication::translate("Level", "Emergency")), LogExportThread_strTranslate_UT_Param("Alert", Dtk::Widget::DApplication::translate("Level", "Alert")), LogExportThread_strTranslate_UT_Param("Critical", Dtk::Widget::DApplication::translate("Level", "Critical")), LogExportThread_strTranslate_UT_Param("Error", Dtk::Widget::DApplication::translate("Level", "Error")), LogExportThread_strTranslate_UT_Param("Warning", Dtk::Widget::DApplication::translate("Level", "Warning")), LogExportThread_strTranslate_UT_Param("Notice", Dtk::Widget::DApplication::translate("Level", "Notice")), LogExportThread_strTranslate_UT_Param("Info", Dtk::Widget::DApplication::translate("Level", "Info")), LogExportThread_strTranslate_UT_Param("Debug", Dtk::Widget::DApplication::translate("Level", "Debug"))));

//TEST_P(LogExportThread_strTranslate_UT, LogExportThread_strTranslate_UT_001)
//{
//    LogExportThread_strTranslate_UT_Param param = GetParam();
//    LogExportThread *p = new LogExportThread(nullptr);
//    EXPECT_NE(p, nullptr);
//    EXPECT_EQ(param.value == p->strTranslate(param.key), true);
//    p->deleteLater();
//}

//class LogExportThread_run_UT_Param
//{
//public:
//    LogExportThread_run_UT_Param(LogExportThread::RUN_MODE iMode)
//    {
//        mode = iMode;
//    }
//    LogExportThread::RUN_MODE mode;

//};

//class LogExportThread_run_UT : public ::testing::TestWithParam<LogExportThread_run_UT_Param>
//{
//};

//INSTANTIATE_TEST_CASE_P(LogExportThread, LogExportThread_run_UT, ::testing::Values(LogExportThread_run_UT_Param(LogExportThread::HtmlModel)
//                                                                                    , LogExportThread_run_UT_Param(LogExportThread::HtmlJOURNAL)
//                                                                                    , LogExportThread_run_UT_Param(LogExportThread::HtmlAPP)
//                                                                                    , LogExportThread_run_UT_Param(LogExportThread::HtmlDPKG)
//                                                                                    , LogExportThread_run_UT_Param(LogExportThread::HtmlBOOT)
//                                                                                    , LogExportThread_run_UT_Param(LogExportThread::HtmlXORG)
//                                                                                    , LogExportThread_run_UT_Param(LogExportThread::HtmlNORMAL)
//                                                                                    , LogExportThread_run_UT_Param(LogExportThread::HtmlKWIN)
//                                                                                    , LogExportThread_run_UT_Param(LogExportThread::TxtModel)
//                                                                                    , LogExportThread_run_UT_Param(LogExportThread::TxtJOURNAL)
//                                                                                    , LogExportThread_run_UT_Param(LogExportThread::TxtAPP)
//                                                                                    , LogExportThread_run_UT_Param(LogExportThread::TxtDPKG)
//                                                                                    , LogExportThread_run_UT_Param(LogExportThread::TxtBOOT)
//                                                                                    , LogExportThread_run_UT_Param(LogExportThread::TxtXORG)
//                                                                                    , LogExportThread_run_UT_Param(LogExportThread::TxtNORMAL)
//                                                                                    , LogExportThread_run_UT_Param(LogExportThread::TxtKWIN)
////                                                                                    , LogExportThread_run_UT_Param(LogExportThread::DocModel)
////                                                                                    , LogExportThread_run_UT_Param(LogExportThread::DocJOURNAL)
////                                                                                    , LogExportThread_run_UT_Param(LogExportThread::DocAPP)
////                                                                                    , LogExportThread_run_UT_Param(LogExportThread::DocDPKG)
////                                                                                    , LogExportThread_run_UT_Param(LogExportThread::DocBOOT)
////                                                                                    , LogExportThread_run_UT_Param(LogExportThread::DocXORG)
////                                                                                    , LogExportThread_run_UT_Param(LogExportThread::DocNORMAL)
////                                                                                    , LogExportThread_run_UT_Param(LogExportThread::DocKWIN)
//                                                                                    , LogExportThread_run_UT_Param(LogExportThread::XlsModel)
//                                                                                    , LogExportThread_run_UT_Param(LogExportThread::XlsJOURNAL)
//                                                                                    , LogExportThread_run_UT_Param(LogExportThread::XlsAPP)
//                                                                                    , LogExportThread_run_UT_Param(LogExportThread::XlsDPKG)
//                                                                                    , LogExportThread_run_UT_Param(LogExportThread::XlsBOOT)
//                                                                                    , LogExportThread_run_UT_Param(LogExportThread::XlsXORG)
//                                                                                    , LogExportThread_run_UT_Param(LogExportThread::XlsNORMAL)
//                                                                                    , LogExportThread_run_UT_Param(LogExportThread::XlsKWIN)
//                                                                                    , LogExportThread_run_UT_Param(LogExportThread::NoneExportType)
//                                                                                   ));

//TEST_P(LogExportThread_run_UT, LogExportThread_run_UT)
//{
//    LogExportThread_run_UT_Param param = GetParam();
//    LogExportThread *p = new LogExportThread(nullptr);
//    EXPECT_NE(p, nullptr);
//    QList<LOG_MSG_JOURNAL> jList;
//    for (int i = 0; i < 5; ++i) {
//        LOG_MSG_JOURNAL info;
//        info.msg = "testmsg";
//        info.level = "testlevel";
//        info.daemonId = "testid";
//        info.dateTime = "testdatetime";
//        info.hostName = "testhostName";
//        info.daemonName = "testdaemonName";
//        jList << info;
//    }
//    p->m_jList = jList;
//    p->m_runMode = param.mode;
//    if (param.mode == LogExportThread::HtmlJOURNAL || param.mode == LogExportThread::TxtJOURNAL || param.mode == LogExportThread::XlsJOURNAL || param.mode == LogExportThread::DocJOURNAL) {
//        p->m_flag = JOURNAL;
//    }
//    p->run();
//    p->deleteLater();
//}
class LogExportthread_UT : public testing::Test
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
        bool iscomplete = true;
        exportThread = new LogExportThread(iscomplete);
        qDebug() << "SetUp" << endl;
    }
    void TearDown() //TEST跑完之后会执行TearDown
    {
        delete exportThread;
    }
    LogExportThread *exportThread;
};

TEST_F(LogExportthread_UT, LogExportThread001_UT)
{
    QStandardItemModel m_model;
    m_model.appendRow(new QStandardItem());
    LOG_FLAG flag;
    struct LOG_MSG_JOURNAL m_journal = {"20190503", "10:02", "uos", "d", "1", "2"};
    struct LOG_MSG_APPLICATOIN m_app = {"20190503", "10:02", "uos", "d"};
    struct LOG_MSG_DPKG m_dpkg = {"20190503", "10:02", "uos"};
    struct LOG_MSG_BOOT m_boot = {"20190503", "10:02"};
    struct LOG_MSG_XORG m_xorg = {"20190503", "10:02"};
    struct LOG_MSG_NORMAL m_normal = {"20190503", "10:02", "uos", "d"};
    struct LOG_MSG_KWIN m_kwin = {"test"};
    QList<LOG_MSG_JOURNAL> m_List;
    QList<LOG_MSG_APPLICATOIN> m_appList;
    QList<LOG_MSG_DPKG> m_dpkgList;
    QList<LOG_MSG_BOOT> m_bootList;
    QList<LOG_MSG_XORG> m_xorgList;
    QList<LOG_MSG_NORMAL> m_normalList;
    QList<LOG_MSG_KWIN> m_kwinList;
    m_appList.append(m_app);
    m_List.append(m_journal);
    m_dpkgList.append(m_dpkg);
    m_bootList.append(m_boot);
    m_xorgList.append(m_xorg);
    m_normalList.append(m_normal);
    m_kwinList.append(m_kwin);
    QString test = "test";
    exportThread->m_canRunning = true;
    exportThread->exportToDoc("test", m_List, QStringList() << "test", flag);
    exportThread->exportToDoc("test", m_appList, QStringList() << "test", test);
    exportThread->exportToDoc("test", m_dpkgList, QStringList() << "test");
    exportThread->exportToDoc("test", m_bootList, QStringList() << "test");
    exportThread->exportToDoc("test", m_xorgList, QStringList() << "test");
    exportThread->exportToDoc("test", m_normalList, QStringList() << "test");
    exportThread->exportToDoc("test", m_kwinList, QStringList() << "test");

    //        exportThread->exportToDoc("test",m_List,QStringList()<<"test",test);
}

TEST_F(LogExportthread_UT, LogExportThread002_UT)
{
    QStandardItemModel m_model;
    m_model.appendRow(new QStandardItem());
    LOG_FLAG flag;
    struct LOG_MSG_JOURNAL m_journal = {"20190503", "10:02", "uos", "d", "1", "2"};
    struct LOG_MSG_APPLICATOIN m_app = {"20190503", "10:02", "uos", "d"};
    struct LOG_MSG_DPKG m_dpkg = {"20190503", "10:02", "uos"};
    struct LOG_MSG_BOOT m_boot = {"20190503", "10:02"};
    struct LOG_MSG_XORG m_xorg = {"20190503", "10:02"};
    struct LOG_MSG_NORMAL m_normal = {"20190503", "10:02", "uos", "d"};
    struct LOG_MSG_KWIN m_kwin = {"test"};
    QList<LOG_MSG_JOURNAL> m_List;
    QList<LOG_MSG_APPLICATOIN> m_appList;
    QList<LOG_MSG_DPKG> m_dpkgList;
    QList<LOG_MSG_BOOT> m_bootList;
    QList<LOG_MSG_XORG> m_xorgList;
    QList<LOG_MSG_NORMAL> m_normalList;
    QList<LOG_MSG_KWIN> m_kwinList;
    m_appList.append(m_app);
    m_List.append(m_journal);
    m_dpkgList.append(m_dpkg);
    m_bootList.append(m_boot);
    m_xorgList.append(m_xorg);
    m_normalList.append(m_normal);
    m_kwinList.append(m_kwin);
    QString test = "test";
    exportThread->m_canRunning = true;
    exportThread->exportToHtml("test", new QStandardItemModel(), flag);
    exportThread->exportToHtml("test", m_List, QStringList() << "test", flag);
    exportThread->exportToHtml("test", m_appList, QStringList() << "test", test);
    exportThread->exportToHtml("test", m_dpkgList, QStringList() << "test");
    exportThread->exportToHtml("test", m_bootList, QStringList() << "test");
    exportThread->exportToHtml("test", m_xorgList, QStringList() << "test");
    exportThread->exportToHtml("test", m_normalList, QStringList() << "test");
    exportThread->exportToHtml("test", m_kwinList, QStringList() << "test");
}

TEST_F(LogExportthread_UT, LogExportThread003_UT)
{
    QStandardItemModel m_model;
    m_model.appendRow(new QStandardItem());
    LOG_FLAG flag;
    struct LOG_MSG_JOURNAL m_journal = {"20190503", "10:02", "uos", "d", "1", "2"};
    struct LOG_MSG_APPLICATOIN m_app = {"20190503", "10:02", "uos", "d"};
    struct LOG_MSG_DPKG m_dpkg = {"20190503", "10:02", "uos"};
    struct LOG_MSG_BOOT m_boot = {"20190503", "10:02"};
    struct LOG_MSG_XORG m_xorg = {"20190503", "10:02"};
    struct LOG_MSG_NORMAL m_normal = {"20190503", "10:02", "uos", "d"};
    struct LOG_MSG_KWIN m_kwin = {"test"};
    QList<LOG_MSG_JOURNAL> m_List;
    QList<LOG_MSG_APPLICATOIN> m_appList;
    QList<LOG_MSG_DPKG> m_dpkgList;
    QList<LOG_MSG_BOOT> m_bootList;
    QList<LOG_MSG_XORG> m_xorgList;
    QList<LOG_MSG_NORMAL> m_normalList;
    QList<LOG_MSG_KWIN> m_kwinList;
    m_appList.append(m_app);
    m_List.append(m_journal);
    m_dpkgList.append(m_dpkg);
    m_bootList.append(m_boot);
    m_xorgList.append(m_xorg);
    m_normalList.append(m_normal);
    m_kwinList.append(m_kwin);
    QString test = "test";
    exportThread->m_canRunning = true;
    exportThread->exportToXls("test", m_List, QStringList() << "test", flag);
    exportThread->exportToXls("test", m_appList, QStringList() << "test", test);
    exportThread->exportToXls("test", m_dpkgList, QStringList() << "test");
    exportThread->exportToXls("test", m_bootList, QStringList() << "test");
    exportThread->exportToXls("test", m_xorgList, QStringList() << "test");
    exportThread->exportToXls("test", m_normalList, QStringList() << "test");
    exportThread->exportToXls("test", m_kwinList, QStringList() << "test");
}
