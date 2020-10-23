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
#include "loglistview.h"

#include <gtest/gtest.h>
#include <gtest/src/stub.h>
#include <QPainter>
#include <QDebug>
#include <QHelpEvent>
#include <QTreeView>
#include <QPaintEvent>
#include <QToolTip>
TEST(LogListDelegate_Constructor_UT, LogListDelegate_Constructor_UT_001)
{
    LogListView *v = new LogListView(nullptr);
    LogListDelegate *p = new LogListDelegate(v);
    EXPECT_NE(p, nullptr);
    p->deleteLater();
}


TEST(LogListDelegate_paint_UT, LogListDelegate_paint_UT_001)
{
    LogListView *v = new LogListView(nullptr);
    LogListDelegate *p = new LogListDelegate(v);
    EXPECT_NE(p, nullptr);
    QStyleOptionViewItem op;
    p->paint(new QPainter, op, QModelIndex());
    p->deleteLater();
}

TEST(LogListDelegate_helpEvent_UT, LogListDelegate_helpEvent_UT_001)
{
    LogListView *v = new LogListView(nullptr);
    LogListDelegate *p = new LogListDelegate(v);
    EXPECT_NE(p, nullptr);
    QStyleOptionViewItem op;
    p->helpEvent(new QHelpEvent(QEvent::HelpRequest, QPoint(), QPoint()), new QTreeView(), op, QModelIndex());
    p->deleteLater();
}


TEST(LogListDelegate_hideTooltipImmediately_UT, LogListDelegate_hideTooltipImmediately_UT_001)
{
    LogListView *v = new LogListView(nullptr);
    LogListDelegate *p = new LogListDelegate(v);
    EXPECT_NE(p, nullptr);
    p->hideTooltipImmediately();
    p->deleteLater();
}

TEST(LogListView_Constructor_UT, LogListView_Constructor_UT_001)
{
    LogListView *p = new LogListView(nullptr);
    EXPECT_NE(p, nullptr);
    p->deleteLater();
}


TEST(LogListView_initUI_UT, LogListView_initUI_UT_001)
{
    LogListView *p = new LogListView(nullptr);
    EXPECT_NE(p, nullptr);
    p->initUI();
    p->deleteLater();
}

TEST(LogListView_setDefaultSelect_UT, LogListView_setDefaultSelectUT_001)
{
    LogListView *p = new LogListView(nullptr);
    EXPECT_NE(p, nullptr);
    p->setDefaultSelect();
    p->deleteLater();
}

TEST(LogListView_setCustomFont_UT, LogListView_setCustomFont_001)
{
    LogListView *p = new LogListView(nullptr);
    EXPECT_NE(p, nullptr);
    p->setCustomFont(nullptr);
    p->deleteLater();
}

TEST(LogListView_isFileExist_UT, LogListView_isFileExist_001)
{
    LogListView *p = new LogListView(nullptr);
    EXPECT_NE(p, nullptr);
    bool result = p->isFileExist("");
    EXPECT_EQ(result, false);
    p->deleteLater();
}

TEST(LogListView_onChangedTheme_UT, LogListView_onChangedTheme_UT_001)
{
    LogListView *p = new LogListView(nullptr);
    EXPECT_NE(p, nullptr);
    p->onChangedTheme(DGuiApplicationHelper::LightType);
    p->deleteLater();
}

TEST(LogListView_onChangedTheme_UT, LogListView_onChangedTheme_UT_002)
{
    LogListView *p = new LogListView(nullptr);
    EXPECT_NE(p, nullptr);
    p->onChangedTheme(DGuiApplicationHelper::DarkType);
    p->deleteLater();
}

TEST(LogListView_paintEvent_UT, LogListView_paintEvent_UT_002)
{
    LogListView *p = new LogListView(nullptr);
    EXPECT_NE(p, nullptr);
    p->paintEvent(new QPaintEvent(QRect()));
    p->deleteLater();
}

TEST(LogListView_currentChanged_UT, LogListView_currentChanged_UT_002)
{
    LogListView *p = new LogListView(nullptr);
    EXPECT_NE(p, nullptr);
    p->currentChanged(QModelIndex(), QModelIndex());
    p->deleteLater();
}


TEST(LogListView_truncateFile_UT, LogListView_truncateFile_UT_002)
{
    LogListView *p = new LogListView(nullptr);
    EXPECT_NE(p, nullptr);
    p->truncateFile("");
    p->deleteLater();
}


TEST(LogListView_slot_getAppPath_UT, LogListView_slot_getAppPath_UT_002)
{
    LogListView *p = new LogListView(nullptr);
    EXPECT_NE(p, nullptr);
    QString path = "aa";
    p->slot_getAppPath(path);
    EXPECT_EQ(path, p->g_path);
    p->deleteLater();
}

TEST(LogListView_focusReson_UT, LogListView_focusReson_UT_002)
{
    LogListView *p = new LogListView(nullptr);
    EXPECT_NE(p, nullptr);
    p->focusReson();
    p->deleteLater();
}


TEST(LogListView_showRightMenu_UT, LogListView_showRightMenu_UT_001)
{
    LogListView *p = new LogListView(nullptr);
    EXPECT_NE(p, nullptr);
    p->showRightMenu(QPoint(10, 10), true);
    p->deleteLater();
}


TEST(LogListView_showRightMenu_UT, LogListView_showRightMenu_UT_002)
{
    LogListView *p = new LogListView(nullptr);
    EXPECT_NE(p, nullptr);
    p->showRightMenu(QPoint(), false);
    p->deleteLater();
}

TEST(LogListView_requestshowRightMenu_UT, LogListView_requestshowRightMenu_UT_001)
{
    LogListView *p = new LogListView(nullptr);
    EXPECT_NE(p, nullptr);
    p->requestshowRightMenu(QPoint());
    p->deleteLater();
}

TEST(LogListView_rmouseMoveEvent_UT, LogListView_mouseMoveEvent_UT_001)
{
    LogListView *p = new LogListView(nullptr);
    EXPECT_NE(p, nullptr);
    QToolTip::showText(QPoint(), "test", p);
    p->mouseMoveEvent(nullptr);
    p->deleteLater();
}


TEST(LogListView_rmouseMoveEvent_UT, LogListView_mouseMoveEvent_UT_002)
{
    LogListView *p = new LogListView(nullptr);
    EXPECT_NE(p, nullptr);
    p->mouseMoveEvent(nullptr);
    p->deleteLater();
}

//class LogListView_keyPressEvent_UT_Param
//{
//public:
//    LogListView_keyPressEvent_UT_Param(bool iKey, const QString &iObjectName, bool iResult, const QString &iFocusObjectName)
//    {
//        key = iKey;
//        objectName = iObjectName;
//        result = iResult;
//        focusObjectName = iFocusObjectName;
//    }
//    int key;
//    QString objectName;
//    bool result;
//    QString focusObjectName;
//};

//class LogListView_keyPressEvent_UT : public ::testing::TestWithParam<LogListView_keyPressEvent_UT_Param>
//{
//};

//INSTANTIATE_TEST_SUITE_P(LogListView, LogListView_keyPressEvent_UT, ::testing::Values(/*LogListView_keyPressEvent_UT_Param(Qt::Key_0, "", false, ""),
//                                                                                                                  LogListView_keyPressEvent_UT_Param(Qt::Key_Tab, "", false, ""),
//                                                                                                                                                                                                                                    */
//                             LogListView_keyPressEvent_UT_Param(Qt::Key_Tab, "titlebar", true, ""),
//                             LogListView_keyPressEvent_UT_Param(Qt::Key_Tab, "searchChildEdt", false, "titlebar"),
//                             LogListView_keyPressEvent_UT_Param(Qt::Key_Tab, "DTitlebarDWindowCloseButton", false, "logTypeSelectList"),
//                             LogListView_keyPressEvent_UT_Param(Qt::Key_Tab, "mainLogTable", true, "searchChildEdt"),
//                             LogListView_keyPressEvent_UT_Param(Qt::Key_Backtab, "logTypeSelectList", true, "DTitlebarDWindowCloseButton"),
//                             LogListView_keyPressEvent_UT_Param(Qt::Key_Backtab, "DTitlebarDWindowOptionButton", true, "searchChildEdt"),
//                             LogListView_keyPressEvent_UT_Param(Qt::Key_Backtab, "searchChildEdt", true, "")
//                             // LogListView_keyPressEvent_UT_Param(Qt::Key_Backtab, "", false, "")
//                         ));

//TEST_P(LogListView_keyPressEvent_UT, LogListView_keyPressEvent_UT_001)
//{
//    LogListView_keyPressEvent_UT_Param param = GetParam();
//    LogCollectorMain *p = new LogCollectorMain(nullptr);
//    EXPECT_NE(p, nullptr);

//    QKeyEvent   *keyEvent = new QKeyEvent(QEvent::KeyPress, param.key, Qt::NoModifier);
//    QWidget *w = nullptr;
//    if (!param.objectName.isEmpty())
//        w = p->titlebar()->findChild<QWidget *>(param.objectName);
//    bool rs = p->handleApplicationTabEventNotify(w, keyEvent);
////    EXPECT_EQ(rs, param.result);

////    EXPECT_EQ(p->focusWidget()->objectName(), param.focusObjectName) ;
//    p->deleteLater();
//}





