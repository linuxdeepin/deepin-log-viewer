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

#include "logcombox.h"

#include <gtest/gtest.h>
#include <stub.h>

#include <QDebug>
#include <QPaintEvent>
#include <QKeyEvent>
#include <QFocusEvent>
TEST(LogCombox_Constructor_UT, LogCombox_Constructor_UT)
{
    LogCombox *p = new LogCombox(nullptr);
    EXPECT_NE(p, nullptr);
    delete p;
}

TEST(LogCombox_setFocusReason_UT, LogCombox_setFocusReason_UT)
{
    LogCombox *p = new LogCombox(nullptr);
    EXPECT_NE(p, nullptr);
    Qt::FocusReason iReson = Qt::TabFocusReason;
    p->setFocusReason(iReson);
    EXPECT_EQ(p->m_reson, iReson) << "check the status after setFocusReason()";
    delete p;
}

TEST(LogCombox_getFocusReason_UT, LogCombox_getFocusReason_UT)
{
    LogCombox *p = new LogCombox(nullptr);
    EXPECT_NE(p, nullptr);
    Qt::FocusReason iReson = Qt::TabFocusReason;
    p->setFocusReason(iReson);
    EXPECT_EQ(p->getFocusReason(), iReson) << "check the status after getFocusReason()";
    delete p;
}

TEST(LogCombox_keyPressEvent_UT, LogCombox_keyPressEvent_UT_001)
{
    LogCombox *p = new LogCombox(nullptr);
    EXPECT_NE(p, nullptr);
    QKeyEvent *keyEvent = new QKeyEvent(QEvent::KeyPress, Qt::Key_Enter, Qt::NoModifier);
    p->keyPressEvent(keyEvent);
    delete keyEvent;
    delete p;
}

TEST(LogCombox_keyPressEvent_UT, LogCombox_keyPressEvent_UT_002)
{
    LogCombox *p = new LogCombox(nullptr);
    EXPECT_NE(p, nullptr);
    QKeyEvent *keyEvent = new QKeyEvent(QEvent::KeyPress, Qt::Key_0, Qt::NoModifier);
    p->keyPressEvent(keyEvent);
    delete keyEvent;
    delete p;
}

TEST(LogCombox_focusInEvent_UT, LogCombox_focusInEvent_UT_001)
{
    LogCombox *p = new LogCombox(nullptr);
    EXPECT_NE(p, nullptr);
    QFocusEvent *e = new QFocusEvent(QEvent::FocusIn, Qt::TabFocusReason);
    p->focusInEvent(e);
    delete e;
    delete p;
}

TEST(LogCombox_focusInEvent_UT, LogCombox_focusInEvent_UT_002)
{
    LogCombox *p = new LogCombox(nullptr);
    EXPECT_NE(p, nullptr);
    QFocusEvent *e = new QFocusEvent(QEvent::FocusIn, Qt::PopupFocusReason);
    p->focusInEvent(e);
    delete e;
    delete p;
}

TEST(LogCombox_focusOutEvent_UT, LogCombox_focusOutEvent_UT)
{
    LogCombox *p = new LogCombox(nullptr);
    EXPECT_NE(p, nullptr);
    QFocusEvent *e = new QFocusEvent(QEvent::FocusOut, Qt::NoFocusReason);
    p->focusOutEvent(e);
    delete e;
    delete p;
}
