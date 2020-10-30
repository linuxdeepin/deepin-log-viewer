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
#include <QMenu>
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
QAction *LogListView_showRightMenu_UT_QMenu_exec_Func(void **p, const QPoint &pos, QAction *at = nullptr)
{
    qDebug() << "QMenu exec";
    return new QAction;
}

TEST(LogListView_showRightMenu_UT, LogListView_showRightMenu_UT_001)
{
    LogListView *p = new LogListView(nullptr);
    EXPECT_NE(p, nullptr);
    Stub *stub = new Stub;
    stub->set((QAction * (QMenu::*)(const QPoint & pos, QAction * at))ADDR(QMenu, exec), LogListView_showRightMenu_UT_QMenu_exec_Func);
    p->showRightMenu(QPoint(10, 10), true);
    p->deleteLater();
}


TEST(LogListView_showRightMenu_UT, LogListView_showRightMenu_UT_002)
{
    LogListView *p = new LogListView(nullptr);
    EXPECT_NE(p, nullptr);
    Stub *stub = new Stub;
    stub->set((QAction * (QMenu::*)(const QPoint & pos, QAction * at))ADDR(QMenu, exec), LogListView_showRightMenu_UT_QMenu_exec_Func);
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

class LogListView_keyPressEvent_UT_Param
{
public:
    LogListView_keyPressEvent_UT_Param(int iKey)
    {
        key = iKey;
    }
    int key;

};

class LogListView_keyPressEvent_UT : public ::testing::TestWithParam<LogListView_keyPressEvent_UT_Param>
{
};

INSTANTIATE_TEST_SUITE_P(LogListView, LogListView_keyPressEvent_UT, ::testing::Values(LogListView_keyPressEvent_UT_Param(Qt::Key_Up)
                                                                                      , LogListView_keyPressEvent_UT_Param(Qt::Key_Down)
                                                                                      , LogListView_keyPressEvent_UT_Param(Qt::Key_0)));

TEST_P(LogListView_keyPressEvent_UT, LogListView_keyPressEvent_UT_001)
{
    LogListView_keyPressEvent_UT_Param param = GetParam();
    LogListView *p = new LogListView(nullptr);
    EXPECT_NE(p, nullptr);

    QKeyEvent   *keyEvent = new QKeyEvent(QEvent::KeyPress, param.key, Qt::NoModifier);
    p->keyPressEvent(keyEvent);
    p->deleteLater();
}

class LogListView_mousePressEvent_UT_Param
{
public:
    LogListView_mousePressEvent_UT_Param(Qt::MouseButton iKey)
    {
        key = iKey;
    }
    Qt::MouseButton key;

};

class LogListView_mousePressEvent_UT : public ::testing::TestWithParam<LogListView_mousePressEvent_UT_Param>
{
};

INSTANTIATE_TEST_SUITE_P(LogListView, LogListView_mousePressEvent_UT, ::testing::Values(LogListView_mousePressEvent_UT_Param(Qt::RightButton)
                                                                                        , LogListView_mousePressEvent_UT_Param(Qt::NoButton)
                                                                                       ));

TEST_P(LogListView_mousePressEvent_UT, LogListView_mousePressEvent_UT_001)
{
    LogListView_mousePressEvent_UT_Param param = GetParam();
    LogListView *p = new LogListView(nullptr);
    EXPECT_NE(p, nullptr);
    QMouseEvent   *keyEvent = new QMouseEvent(QEvent::MouseButtonPress, QPoint(1, 1), param.key, param.key, Qt::NoModifier);
    p->mousePressEvent(keyEvent);
    p->deleteLater();
}

class LogListView_focusInEvent_UT_Param
{
public:
    LogListView_focusInEvent_UT_Param(Qt::FocusReason iReason)
    {
        reason = iReason;
    }
    Qt::FocusReason reason;

};

class LogListView_focusInEvent_UT : public ::testing::TestWithParam<LogListView_focusInEvent_UT_Param>
{
};

INSTANTIATE_TEST_SUITE_P(LogListView, LogListView_focusInEvent_UT, ::testing::Values(LogListView_focusInEvent_UT_Param(Qt::PopupFocusReason)
                                                                                     , LogListView_focusInEvent_UT_Param(Qt::ActiveWindowFocusReason)
                                                                                     , LogListView_focusInEvent_UT_Param(Qt::NoFocusReason)
                                                                                    ));

TEST_P(LogListView_focusInEvent_UT, LogListView_focusInEvent_UT_001)
{
    LogListView_focusInEvent_UT_Param param = GetParam();
    LogListView *p = new LogListView(nullptr);
    EXPECT_NE(p, nullptr);

    QFocusEvent   *focusEvent = new QFocusEvent(QEvent::FocusIn, param.reason);
    p->focusInEvent(focusEvent);
    p->deleteLater();
}

TEST(LogListView_focusOutEventcontextMenuEvent_UT, LogListView_focusOutEvent_UT_001)
{
    LogListView *p = new LogListView(nullptr);
    EXPECT_NE(p, nullptr);

    QFocusEvent   *focusEvent = new QFocusEvent(QEvent::FocusOut, Qt::OtherFocusReason);
    p->focusOutEvent(focusEvent);
    p->deleteLater();
}



