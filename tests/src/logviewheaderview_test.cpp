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
#include "logviewheaderview.h"

#include <gtest/gtest.h>
#include <gtest/src/stub.h>

#include <QDebug>
#include <QPainter>
#include <QEvent>
#include <QFocusEvent>

TEST(LogViewHeaderView_Constructor_UT, LogViewHeaderView_Constructor_UT_001)
{
    LogViewHeaderView *p = new LogViewHeaderView(Qt::Horizontal, nullptr);
    EXPECT_NE(p, nullptr);

    p->deleteLater();
}

TEST(LogViewHeaderView_paintSection_UT, LogViewHeaderView_paintSection_UT_001)
{
    LogViewHeaderView *p = new LogViewHeaderView(Qt::Horizontal, nullptr);
    EXPECT_NE(p, nullptr);
    p->paintSection(new QPainter, p->rect(), 0);
    p->deleteLater();
}

TEST(LogViewHeaderView_focusInEvent_UT, LogViewHeaderView_focusInEvent_UT)
{
    LogViewHeaderView *p = new LogViewHeaderView(Qt::Horizontal, nullptr);
    EXPECT_NE(p, nullptr);
    QFocusEvent   *e = new QFocusEvent(QEvent::FocusOut, Qt::ShortcutFocusReason);
    p->focusInEvent(e);
    EXPECT_EQ(p->m_reson, Qt::ShortcutFocusReason);
    p->deleteLater();
}


TEST(LogViewHeaderView_paintEvent_UT, LogViewHeaderView_paintEvent_UT_001)
{
    LogViewHeaderView *p = new LogViewHeaderView(Qt::Horizontal, nullptr);
    EXPECT_NE(p, nullptr);
    p->paintEvent(new QPaintEvent(p->rect()));
    p->deleteLater();
}


TEST(LogViewHeaderView_sizeHint_UT, LogViewHeaderView_sizeHint_UT_001)
{
    LogViewHeaderView *p = new LogViewHeaderView(Qt::Horizontal, nullptr);
    EXPECT_NE(p, nullptr);
    p->sizeHint();
    p->deleteLater();
}

TEST(LogViewHeaderView_sectionSizeHint_UT, LogViewHeaderView_sectionSizeHint_UT_001)
{
    LogViewHeaderView *p = new LogViewHeaderView(Qt::Horizontal, nullptr);
    EXPECT_NE(p, nullptr);
    p->sectionSizeHint(0);
    p->deleteLater();
}

