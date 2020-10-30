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
#include "logscrollbar.h"
#include <gtest/gtest.h>
#include <gtest/src/stub.h>

#include <QMouseEvent>
#include <QDebug>
TEST(LogScrollBar_Constructor_UT, LogScrollBar_Constructor_UT_001)
{
    LogScrollBar *p = new LogScrollBar(nullptr);
    EXPECT_NE(p, nullptr);

    p->deleteLater();
}

TEST(LogScrollBar_Constructor_UT, LogScrollBar_Constructor_UT_002)
{
    LogScrollBar *p = new LogScrollBar(Qt::Horizontal, nullptr);
    EXPECT_NE(p, nullptr);

    p->deleteLater();
}

TEST(LogScrollBar_mousePressEvent_UT, LogScrollBar_mousePressEvent_UT_002)
{
    LogScrollBar *p = new LogScrollBar(Qt::Horizontal, nullptr);
    EXPECT_NE(p, nullptr);

    p->mousePressEvent(new QMouseEvent(QEvent::MouseButtonPress, QPoint(1, 1), Qt::LeftButton, Qt::LeftButton, Qt::NoModifier));
    EXPECT_EQ(p->m_isOnPress, true);
    p->deleteLater();
}


