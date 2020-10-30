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
#include "logviewitemdelegate.h"

#include <gtest/gtest.h>
#include <gtest/src/stub.h>

#include <QDebug>
#include <QPainter>
TEST(LogViewItemDelegate_Constructor_UT, LogViewItemDelegate_Constructor_UT)
{
    LogViewItemDelegate *p = new LogViewItemDelegate(nullptr);
    EXPECT_NE(p, nullptr);
    p->deleteLater();
}

TEST(LogViewItemDelegate_paint_UT, LogViewItemDelegate_paint_UT)
{
    LogViewItemDelegate *p = new LogViewItemDelegate(nullptr);
    EXPECT_NE(p, nullptr);
    p->paint(new QPainter, QStyleOptionViewItem(), QModelIndex());
    p->deleteLater();
}

TEST(LogViewItemDelegate_createEditor_UT, LogViewItemDelegate_createEditor_UT)
{
    LogViewItemDelegate *p = new LogViewItemDelegate(nullptr);
    EXPECT_NE(p, nullptr);
    QWidget *w =  p->createEditor(nullptr, QStyleOptionViewItem(), QModelIndex());
    EXPECT_EQ(w, nullptr);
    p->deleteLater();
}

TEST(LogViewItemDelegate_sizeHint_UT, LogViewItemDelegate_sizeHint_UT)
{
    LogViewItemDelegate *p = new LogViewItemDelegate(nullptr);
    EXPECT_NE(p, nullptr);
    p->sizeHint(QStyleOptionViewItem(), QModelIndex());
    p->deleteLater();
}

TEST(LogViewItemDelegate_initStyleOption_UT, LogViewItemDelegate_initStyleOption_UT)
{
    LogViewItemDelegate *p = new LogViewItemDelegate(nullptr);
    EXPECT_NE(p, nullptr);
    p->initStyleOption(new QStyleOptionViewItem(), QModelIndex());
    p->deleteLater();
}

