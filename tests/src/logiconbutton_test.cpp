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
#include "logiconbutton.h"

#include <gtest/gtest.h>
#include <stub.h>

#include <QDebug>
#include <QEvent>
TEST(LogCombox_Constructor_UT, LogCombox_Constructor_UT_001)
{
    LogIconButton *p = new LogIconButton(nullptr);
    EXPECT_NE(p, nullptr);
    p->deleteLater();
}

TEST(LogCombox_Constructor_UT, LogCombox_Constructor_UT_002)
{
    LogIconButton *p = new LogIconButton("aa", nullptr);
    EXPECT_NE(p, nullptr);
    p->deleteLater();
}

//TEST(LogCombox_sizeHint_UT, LogCombox_sizeHint_UT)
//{
//    LogIconButton *p = new LogIconButton("aa", nullptr);
//    EXPECT_NE(p, nullptr);
//    p->sizeHint();
//    p->deleteLater();
//}

//TEST(LogCombox_mousePressEvent_UT, LogCombox_mousePressEvent_UT)
//{
//    LogIconButton *p = new LogIconButton("aa", nullptr);
//    EXPECT_NE(p, nullptr);
//    p->mousePressEvent(nullptr);
//    p->deleteLater();
//}

//TEST(LogCombox_mousePressEvent_UT, LogCombox_mousePressEvent_UT_001)
//{
//    LogIconButton *p = new LogIconButton("aa", nullptr);
//    EXPECT_NE(p, nullptr);
//    p->event(new QEvent(QEvent::QueryWhatsThis));
//    p->deleteLater();
//}

//TEST(LogCombox_mousePressEvent_UT, LogCombox_mousePressEvent_UT_002)
//{
//    LogIconButton *p = new LogIconButton("aa", nullptr);
//    EXPECT_NE(p, nullptr);

//    p->event(new QEvent(QEvent::HoverEnter));
//    p->deleteLater();
//}

//TEST(LogCombox_resizeEvent_UT, LogCombox_resizeEvent_UT)
//{
//    LogIconButton *p = new LogIconButton("aa", nullptr);
//    EXPECT_NE(p, nullptr);
//    p->resizeEvent(nullptr);
//    p->deleteLater();
//}
