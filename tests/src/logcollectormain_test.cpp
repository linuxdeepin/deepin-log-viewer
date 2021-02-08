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

#include "logcollectormain.h"
#include "DebugTimeManager.h"
#include <stuballthread.h>

#include <gtest/gtest.h>
#include <stub.h>

#include <DTitlebar>
#include <DWindowCloseButton>

#include <QKeyEvent>
#include <QDebug>

void Log_beginPointLinux(const QString &point, const QString &status)
{
}

TEST(LogCollectorMain_Constructor_UT, LogCollectorMain_Constructor_UT)
{
    Stub stub;
    stub.set(ADDR(DebugTimeManager, beginPointLinux), Log_beginPointLinux);
    stub.set(ADDR(LogFileParser, parseByJournal), LogFileParser_parseByJournal);
    stub.set(ADDR(LogFileParser, parseByJournalBoot), LogFileParser_parseByJournalBoot);
    stub.set(ADDR(LogFileParser, parseByDpkg), LogFileParser_parseByDpkg);
    stub.set(ADDR(LogFileParser, parseByXlog), LogFileParser_parseByXlog);
    stub.set(ADDR(LogFileParser, parseByBoot), LogFileParser_parseByBoot);
    stub.set(ADDR(LogFileParser, parseByKern), LogFileParser_parseByKern);
    stub.set(ADDR(LogFileParser, parseByApp), LogFileParser_parseByApp);
    stub.set(ADDR(LogFileParser, parseByNormal), LogFileParser_parseByNormal);
    stub.set(ADDR(LogFileParser, parseByKwin), LogFileParser_parseByKwin);
    LogCollectorMain *p = new LogCollectorMain(nullptr);
    EXPECT_NE(p, nullptr);
    p->deleteLater();
}

TEST(LogCollectorMain_Destructor_UT, LogCollectorMain_Destructor_UT)
{
    Stub stub;
    stub.set(ADDR(DebugTimeManager, beginPointLinux), Log_beginPointLinux);
    stub.set(ADDR(LogFileParser, parseByJournal), LogFileParser_parseByJournal);
    stub.set(ADDR(LogFileParser, parseByJournalBoot), LogFileParser_parseByJournalBoot);
    stub.set(ADDR(LogFileParser, parseByDpkg), LogFileParser_parseByDpkg);
    stub.set(ADDR(LogFileParser, parseByXlog), LogFileParser_parseByXlog);
    stub.set(ADDR(LogFileParser, parseByBoot), LogFileParser_parseByBoot);
    stub.set(ADDR(LogFileParser, parseByKern), LogFileParser_parseByKern);
    stub.set(ADDR(LogFileParser, parseByApp), LogFileParser_parseByApp);
    stub.set(ADDR(LogFileParser, parseByNormal), LogFileParser_parseByNormal);
    stub.set(ADDR(LogFileParser, parseByKwin), LogFileParser_parseByKwin);
    LogCollectorMain *p = new LogCollectorMain(nullptr);
    EXPECT_NE(p, nullptr);
    p->~LogCollectorMain();
    EXPECT_EQ(p->m_searchEdt, nullptr);
}
TEST(LogCollectorMain_initUI_UT, LogCollectorMain_initUI_UT)
{
    Stub stub;
    stub.set(ADDR(DebugTimeManager, beginPointLinux), Log_beginPointLinux);
    stub.set(ADDR(LogFileParser, parseByJournal), LogFileParser_parseByJournal);
    stub.set(ADDR(LogFileParser, parseByJournalBoot), LogFileParser_parseByJournalBoot);
    stub.set(ADDR(LogFileParser, parseByDpkg), LogFileParser_parseByDpkg);
    stub.set(ADDR(LogFileParser, parseByXlog), LogFileParser_parseByXlog);
    stub.set(ADDR(LogFileParser, parseByBoot), LogFileParser_parseByBoot);
    stub.set(ADDR(LogFileParser, parseByKern), LogFileParser_parseByKern);
    stub.set(ADDR(LogFileParser, parseByApp), LogFileParser_parseByApp);
    stub.set(ADDR(LogFileParser, parseByNormal), LogFileParser_parseByNormal);
    stub.set(ADDR(LogFileParser, parseByKwin), LogFileParser_parseByKwin);
    LogCollectorMain *p = new LogCollectorMain(nullptr);
    EXPECT_NE(p, nullptr);
    p->deleteLater();
}

TEST(LogCollectorMain_initShortCut_UT, LogCollectorMain_initShortCut_UT_001)
{
    Stub stub;
    stub.set(ADDR(DebugTimeManager, beginPointLinux), Log_beginPointLinux);
    stub.set(ADDR(LogFileParser, parseByJournal), LogFileParser_parseByJournal);
    stub.set(ADDR(LogFileParser, parseByJournalBoot), LogFileParser_parseByJournalBoot);
    stub.set(ADDR(LogFileParser, parseByDpkg), LogFileParser_parseByDpkg);
    stub.set(ADDR(LogFileParser, parseByXlog), LogFileParser_parseByXlog);
    stub.set(ADDR(LogFileParser, parseByBoot), LogFileParser_parseByBoot);
    stub.set(ADDR(LogFileParser, parseByKern), LogFileParser_parseByKern);
    stub.set(ADDR(LogFileParser, parseByApp), LogFileParser_parseByApp);
    stub.set(ADDR(LogFileParser, parseByNormal), LogFileParser_parseByNormal);
    stub.set(ADDR(LogFileParser, parseByKwin), LogFileParser_parseByKwin);
    LogCollectorMain *p = new LogCollectorMain(nullptr);
    EXPECT_NE(p, nullptr);
    p->initShortCut();
    p->deleteLater();
}

TEST(LogCollectorMain_initShortCut_UT, LogCollectorMain_initShortCut_UT_002)
{
    Stub stub;
    stub.set(ADDR(DebugTimeManager, beginPointLinux), Log_beginPointLinux);
    stub.set(ADDR(LogFileParser, parseByJournal), LogFileParser_parseByJournal);
    stub.set(ADDR(LogFileParser, parseByJournalBoot), LogFileParser_parseByJournalBoot);
    stub.set(ADDR(LogFileParser, parseByDpkg), LogFileParser_parseByDpkg);
    stub.set(ADDR(LogFileParser, parseByXlog), LogFileParser_parseByXlog);
    stub.set(ADDR(LogFileParser, parseByBoot), LogFileParser_parseByBoot);
    stub.set(ADDR(LogFileParser, parseByKern), LogFileParser_parseByKern);
    stub.set(ADDR(LogFileParser, parseByApp), LogFileParser_parseByApp);
    stub.set(ADDR(LogFileParser, parseByNormal), LogFileParser_parseByNormal);
    stub.set(ADDR(LogFileParser, parseByKwin), LogFileParser_parseByKwin);
    LogCollectorMain *p = new LogCollectorMain(nullptr);
    EXPECT_NE(p, nullptr);
    p->m_scWndReize->deleteLater();
    p->m_scWndReize = nullptr;
    p->m_scFindFont->deleteLater();
    p->m_scFindFont = nullptr;
    p->m_scExport->deleteLater();
    p->m_scExport = nullptr;
    p->initShortCut();
    p->deleteLater();
}

TEST(LogCollectorMain_initConnection_UT, LogCollectorMain_initConnection_UT)
{
    Stub stub;
    stub.set(ADDR(DebugTimeManager, beginPointLinux), Log_beginPointLinux);
    stub.set(ADDR(LogFileParser, parseByJournal), LogFileParser_parseByJournal);
    stub.set(ADDR(LogFileParser, parseByJournalBoot), LogFileParser_parseByJournalBoot);
    stub.set(ADDR(LogFileParser, parseByDpkg), LogFileParser_parseByDpkg);
    stub.set(ADDR(LogFileParser, parseByXlog), LogFileParser_parseByXlog);
    stub.set(ADDR(LogFileParser, parseByBoot), LogFileParser_parseByBoot);
    stub.set(ADDR(LogFileParser, parseByKern), LogFileParser_parseByKern);
    stub.set(ADDR(LogFileParser, parseByApp), LogFileParser_parseByApp);
    stub.set(ADDR(LogFileParser, parseByNormal), LogFileParser_parseByNormal);
    stub.set(ADDR(LogFileParser, parseByKwin), LogFileParser_parseByKwin);
    LogCollectorMain *p = new LogCollectorMain(nullptr);
    EXPECT_NE(p, nullptr);

    p->initConnection();
    p->deleteLater();
}
TEST(LogCollectorMain_resizeWidthByFilterContentWidth_UT, LogCollectorMain_resizeWidthByFilterContentWidth_UT)
{
    Stub stub;
    stub.set(ADDR(DebugTimeManager, beginPointLinux), Log_beginPointLinux);
    stub.set(ADDR(LogFileParser, parseByJournal), LogFileParser_parseByJournal);
    stub.set(ADDR(LogFileParser, parseByJournalBoot), LogFileParser_parseByJournalBoot);
    stub.set(ADDR(LogFileParser, parseByDpkg), LogFileParser_parseByDpkg);
    stub.set(ADDR(LogFileParser, parseByXlog), LogFileParser_parseByXlog);
    stub.set(ADDR(LogFileParser, parseByBoot), LogFileParser_parseByBoot);
    stub.set(ADDR(LogFileParser, parseByKern), LogFileParser_parseByKern);
    stub.set(ADDR(LogFileParser, parseByApp), LogFileParser_parseByApp);
    stub.set(ADDR(LogFileParser, parseByNormal), LogFileParser_parseByNormal);
    stub.set(ADDR(LogFileParser, parseByKwin), LogFileParser_parseByKwin);
    LogCollectorMain *p = new LogCollectorMain(nullptr);
    EXPECT_NE(p, nullptr);
    p->resizeWidthByFilterContentWidth(500);
    p->deleteLater();
}

class LogCollectorMain_handleApplicationTabEventNotify_UT_Param
{
public:
    LogCollectorMain_handleApplicationTabEventNotify_UT_Param(bool iKey, const QString &iObjectName, bool iResult, const QString &iFocusObjectName)
    {
        key = iKey;
        objectName = iObjectName;
        result = iResult;
        focusObjectName = iFocusObjectName;
    }
    int key;
    QString objectName;
    bool result;
    QString focusObjectName;
};

class LogCollectorMain_handleApplicationTabEventNotify_UT : public ::testing::TestWithParam<LogCollectorMain_handleApplicationTabEventNotify_UT_Param>
{
};

INSTANTIATE_TEST_CASE_P(LogCollectorMain, LogCollectorMain_handleApplicationTabEventNotify_UT, ::testing::Values(/*LogCollectorMain_handleApplicationTabEventNotify_UT_Param(Qt::Key_0, "", false, ""),
                                                                                                                  LogCollectorMain_handleApplicationTabEventNotify_UT_Param(Qt::Key_Tab, "", false, ""),
                                                                                                                                                                                                                                    */
                                                                                                                 LogCollectorMain_handleApplicationTabEventNotify_UT_Param(Qt::Key_Tab, "titlebar", true, ""), LogCollectorMain_handleApplicationTabEventNotify_UT_Param(Qt::Key_Tab, "searchChildEdt", false, "titlebar"), LogCollectorMain_handleApplicationTabEventNotify_UT_Param(Qt::Key_Tab, "DTitlebarDWindowCloseButton", false, "logTypeSelectList"), LogCollectorMain_handleApplicationTabEventNotify_UT_Param(Qt::Key_Tab, "mainLogTable", true, "searchChildEdt"), LogCollectorMain_handleApplicationTabEventNotify_UT_Param(Qt::Key_Backtab, "logTypeSelectList", true, "DTitlebarDWindowCloseButton"), LogCollectorMain_handleApplicationTabEventNotify_UT_Param(Qt::Key_Backtab, "DTitlebarDWindowOptionButton", true, "searchChildEdt"), LogCollectorMain_handleApplicationTabEventNotify_UT_Param(Qt::Key_Backtab, "searchChildEdt", true, "")
                                                                                                                 // LogCollectorMain_handleApplicationTabEventNotify_UT_Param(Qt::Key_Backtab, "", false, "")
                                                                                                                 ));
QString stub_ObjectName()
{
    return QString("logTypeSelectList");
}

TEST_P(LogCollectorMain_handleApplicationTabEventNotify_UT, LogCollectorMain_handleApplicationTabEventNotify_UT_001)
{
    Stub stub;
    stub.set(ADDR(DebugTimeManager, beginPointLinux), Log_beginPointLinux);
    stub.set(ADDR(LogFileParser, parseByJournal), LogFileParser_parseByJournal);
    stub.set(ADDR(LogFileParser, parseByJournalBoot), LogFileParser_parseByJournalBoot);
    stub.set(ADDR(LogFileParser, parseByDpkg), LogFileParser_parseByDpkg);
    stub.set(ADDR(LogFileParser, parseByXlog), LogFileParser_parseByXlog);
    stub.set(ADDR(LogFileParser, parseByBoot), LogFileParser_parseByBoot);
    stub.set(ADDR(LogFileParser, parseByKern), LogFileParser_parseByKern);
    stub.set(ADDR(LogFileParser, parseByApp), LogFileParser_parseByApp);
    stub.set(ADDR(LogFileParser, parseByNormal), LogFileParser_parseByNormal);
    stub.set(ADDR(LogFileParser, parseByKwin), LogFileParser_parseByKwin);
    LogCollectorMain_handleApplicationTabEventNotify_UT_Param param = GetParam();
    LogCollectorMain *p = new LogCollectorMain(nullptr);
    EXPECT_NE(p, nullptr);

    QKeyEvent *keyEvent = new QKeyEvent(QEvent::KeyPress, param.key, Qt::NoModifier);
    QWidget *w = nullptr;
    if (!param.objectName.isEmpty())
        w = p->titlebar()->findChild<QWidget *>(param.objectName);
    p->handleApplicationTabEventNotify(w, keyEvent);
    //    EXPECT_EQ(rs, param.result);

    //    EXPECT_EQ(p->focusWidget()->objectName(), param.focusObjectName) ;
    p->deleteLater();
}

TEST_P(LogCollectorMain_handleApplicationTabEventNotify_UT, LogCollectorMain_handleApplicationTabEventNotify_UT_002)
{
    Stub stub;
    stub.set(ADDR(DebugTimeManager, beginPointLinux), Log_beginPointLinux);
    stub.set(ADDR(LogFileParser, parseByJournal), LogFileParser_parseByJournal);
    stub.set(ADDR(LogFileParser, parseByJournalBoot), LogFileParser_parseByJournalBoot);
    stub.set(ADDR(LogFileParser, parseByDpkg), LogFileParser_parseByDpkg);
    stub.set(ADDR(LogFileParser, parseByXlog), LogFileParser_parseByXlog);
    stub.set(ADDR(LogFileParser, parseByBoot), LogFileParser_parseByBoot);
    stub.set(ADDR(LogFileParser, parseByKern), LogFileParser_parseByKern);
    stub.set(ADDR(LogFileParser, parseByApp), LogFileParser_parseByApp);
    stub.set(ADDR(LogFileParser, parseByNormal), LogFileParser_parseByNormal);
    stub.set(ADDR(LogFileParser, parseByKwin), LogFileParser_parseByKwin);
    stub.set(ADDR(QObject, objectName), stub_ObjectName);
    LogCollectorMain_handleApplicationTabEventNotify_UT_Param param = GetParam();
    LogCollectorMain *p = new LogCollectorMain(nullptr);
    EXPECT_NE(p, nullptr);

    QKeyEvent *keyEvent = new QKeyEvent(QEvent::KeyPress, Qt::Key_Backtab, Qt::NoModifier);
    QWidget *w = nullptr;
    if (!param.objectName.isEmpty())
        w = p->titlebar()->findChild<QWidget *>(QString("logTypeSelectList"));
    p->handleApplicationTabEventNotify(w, keyEvent);
    //    EXPECT_EQ(rs, param.result);

    //    EXPECT_EQ(p->focusWidget()->objectName(), param.focusObjectName) ;
    p->deleteLater();
}
