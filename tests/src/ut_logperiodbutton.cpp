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
#include "logperiodbutton.h"

#include <gtest/gtest.h>
#include <stub.h>

#include <QDebug>
#include <QPaintEvent>
TEST(LogPeriodButton_Constructor_UT, LogPeriodButton_Constructor_UT_001)
{
    LogPeriodButton *p = new LogPeriodButton("", nullptr);
    EXPECT_NE(p, nullptr);

    delete p;
}

TEST(LogPeriodButton_setStandardSize_UT, LogPeriodButton_setStandardSize_UT_001)
{
    LogPeriodButton *p = new LogPeriodButton("", nullptr);
    EXPECT_NE(p, nullptr);
    p->setStandardSize(20);
    EXPECT_EQ(p->m_stahndardWidth, 20);
    delete p;
}

TEST(LogPeriodButton_getFocusReason_UT, LogPeriodButton_getFocusReason_UT_001)
{
    LogPeriodButton *p = new LogPeriodButton("", nullptr);
    EXPECT_NE(p, nullptr);
    p->getFocusReason();
    delete p;
}

TEST(LogPeriodButton_enterEvent_UT, LogPeriodButton_enterEvent_UT_001)
{
    LogPeriodButton *p = new LogPeriodButton("", nullptr);
    EXPECT_NE(p, nullptr);
    QEvent *e = new QEvent(QEvent::Enter);
    p->enterEvent(e);
    EXPECT_EQ(p->isEnter, true);
    delete e;
    delete p;
}

TEST(LogPeriodButton_leaveEvent_UT, LogPeriodButton_leaveEvent_UT_001)
{
    LogPeriodButton *p = new LogPeriodButton("", nullptr);
    EXPECT_NE(p, nullptr);
    QEvent *e = new QEvent(QEvent::Leave);
    p->leaveEvent(e);
    EXPECT_EQ(p->isEnter, false);
    delete e;
    delete p;
}

//TEST(LogPeriodButton_paintEvent_UT, LogPeriodButton_paintEvent_UT_001)
//{
//    LogPeriodButton *p = new LogPeriodButton("", nullptr);
//    EXPECT_NE(p, nullptr);

//    p->paintEvent(new QPaintEvent(p->rect()));

//    p->deleteLater();
//}

class LogPeriodButton_focusInEvent_UT_Param
{
public:
    explicit LogPeriodButton_focusInEvent_UT_Param(Qt::FocusReason iReason)
        : reason(iReason)
    {
    }
    Qt::FocusReason reason;
};

class LogPeriodButton_focusInEvent_UT : public ::testing::TestWithParam<LogPeriodButton_focusInEvent_UT_Param>
{
};

INSTANTIATE_TEST_CASE_P(LogPeriodButton, LogPeriodButton_focusInEvent_UT, ::testing::Values(LogPeriodButton_focusInEvent_UT_Param(Qt::MouseFocusReason), LogPeriodButton_focusInEvent_UT_Param(Qt::NoFocusReason)));

TEST_P(LogPeriodButton_focusInEvent_UT, LogPeriodButton_focusInEvent_UT_001)
{
    LogPeriodButton_focusInEvent_UT_Param param = GetParam();
    LogPeriodButton *p = new LogPeriodButton(nullptr);
    EXPECT_NE(p, nullptr);
    p->setChecked(false);
    QFocusEvent *focusEvent = new QFocusEvent(QEvent::FocusIn, param.reason);
    p->focusInEvent(focusEvent);
    delete focusEvent;
    delete p;
}
