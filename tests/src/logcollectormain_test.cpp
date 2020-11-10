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
#include <gtest/gtest.h>
#include <stub.h>
#include "logcollectormain.h"

#include <DTitlebar>
#include <DWindowCloseButton>

#include <QKeyEvent>
#include <QDebug>
TEST(LogCollectorMain_Constructor_UT, LogCollectorMain_Constructor_UT)
{
    LogCollectorMain *p = new LogCollectorMain(nullptr);
    EXPECT_NE(p, nullptr);
    p->deleteLater();
}

TEST(LogCollectorMain_Destructor_UT, LogCollectorMain_Destructor_UT)
{
    LogCollectorMain *p = new LogCollectorMain(nullptr);
    EXPECT_NE(p, nullptr);
    p->~LogCollectorMain();
    EXPECT_EQ(p->m_searchEdt, nullptr);
}
TEST(LogCollectorMain_initUI_UT, LogCollectorMain_initUI_UT)
{
    LogCollectorMain *p = new LogCollectorMain(nullptr);
    EXPECT_NE(p, nullptr);
    p->deleteLater();
}

TEST(LogCollectorMain_initShortCut_UT, LogCollectorMain_initShortCut_UT_001)
{
    LogCollectorMain *p = new LogCollectorMain(nullptr);
    EXPECT_NE(p, nullptr);
    p->initShortCut();
    p->deleteLater();
}


TEST(LogCollectorMain_initShortCut_UT, LogCollectorMain_initShortCut_UT_002)
{
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
    LogCollectorMain *p = new LogCollectorMain(nullptr);
    EXPECT_NE(p, nullptr);

    p->initConnection();
    p->deleteLater();
}
TEST(LogCollectorMain_resizeWidthByFilterContentWidth_UT, LogCollectorMain_resizeWidthByFilterContentWidth_UT)
{
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
                             LogCollectorMain_handleApplicationTabEventNotify_UT_Param(Qt::Key_Tab, "titlebar", true, ""),
                             LogCollectorMain_handleApplicationTabEventNotify_UT_Param(Qt::Key_Tab, "searchChildEdt", false, "titlebar"),
                             LogCollectorMain_handleApplicationTabEventNotify_UT_Param(Qt::Key_Tab, "DTitlebarDWindowCloseButton", false, "logTypeSelectList"),
                             LogCollectorMain_handleApplicationTabEventNotify_UT_Param(Qt::Key_Tab, "mainLogTable", true, "searchChildEdt"),
                             LogCollectorMain_handleApplicationTabEventNotify_UT_Param(Qt::Key_Backtab, "logTypeSelectList", true, "DTitlebarDWindowCloseButton"),
                             LogCollectorMain_handleApplicationTabEventNotify_UT_Param(Qt::Key_Backtab, "DTitlebarDWindowOptionButton", true, "searchChildEdt"),
                             LogCollectorMain_handleApplicationTabEventNotify_UT_Param(Qt::Key_Backtab, "searchChildEdt", true, "")
                             // LogCollectorMain_handleApplicationTabEventNotify_UT_Param(Qt::Key_Backtab, "", false, "")
                         ));

TEST_P(LogCollectorMain_handleApplicationTabEventNotify_UT, LogCollectorMain_handleApplicationTabEventNotify_UT_001)
{
    LogCollectorMain_handleApplicationTabEventNotify_UT_Param param = GetParam();
    LogCollectorMain *p = new LogCollectorMain(nullptr);
    EXPECT_NE(p, nullptr);

    QKeyEvent   *keyEvent = new QKeyEvent(QEvent::KeyPress, param.key, Qt::NoModifier);
    QWidget *w = nullptr;
    if (!param.objectName.isEmpty())
        w = p->titlebar()->findChild<QWidget *>(param.objectName);
    bool rs = p->handleApplicationTabEventNotify(w, keyEvent);
//    EXPECT_EQ(rs, param.result);

//    EXPECT_EQ(p->focusWidget()->objectName(), param.focusObjectName) ;
    p->deleteLater();
}
