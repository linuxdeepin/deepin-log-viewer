// SPDX-FileCopyrightText: 2019 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "logperiodbutton.h"
#include "../../application/qtcompat.h"

#include <gtest/gtest.h>
#include <stub.h>

#include <QDebug>
#include <QPaintEvent>
#include <QFocusEvent>
#include <QApplication>
#include <DStyle>
#include <DApplication>
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#include <QEnterEvent>
#endif

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
    EXPECT_EQ(p->getFocusReason(), Qt::MouseFocusReason);
    p->m_reson = Qt::TabFocusReason;
    EXPECT_EQ(p->getFocusReason(), Qt::TabFocusReason);
    delete p;
}

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
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
#else
TEST(LogPeriodButton_enterEvent_UT, LogPeriodButton_enterEvent_UT_001)
{
    LogPeriodButton *p = new LogPeriodButton("", nullptr);
    EXPECT_NE(p, nullptr);
    QEnterEvent *e = new QEnterEvent(QPointF(0, 0), QPointF(0, 0), QPointF(0, 0));
    p->enterEvent(e);
    EXPECT_EQ(p->isEnter, true);
    delete e;
    delete p;
}

TEST(LogPeriodButton_leaveEvent_UT, LogPeriodButton_leaveEvent_UT_001)
{
    LogPeriodButton *p = new LogPeriodButton("", nullptr);
    EXPECT_NE(p, nullptr);
    QEnterEvent *enterE = new QEnterEvent(QPointF(0, 0), QPointF(0, 0), QPointF(0, 0));
    p->enterEvent(enterE);
    EXPECT_EQ(p->isEnter, true);
    delete enterE;

    QEvent *e = new QEvent(QEvent::Leave);
    p->leaveEvent(e);
    EXPECT_EQ(p->isEnter, false);
    delete e;
    delete p;
}
#endif

TEST(LogPeriodButton_paintEvent_UT, LogPeriodButton_paintEvent_UT_001)
{
    // Ensure DApplication::style() returns a DStyle so dynamic_cast succeeds
    QApplication::setStyle(new Dtk::Widget::DStyle);

    LogPeriodButton *p = new LogPeriodButton("", nullptr);
    EXPECT_NE(p, nullptr);
    QPaintEvent e(p->rect());

    // isEnter = false branch
    p->paintEvent(&e);
    EXPECT_EQ(p->isEnter, false);

    // isEnter = true branch (hover state)
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    QEnterEvent enterE(QPointF(0, 0), QPointF(0, 0), QPointF(0, 0));
    p->enterEvent(&enterE);
#else
    QEvent enterE(QEvent::Enter);
    p->enterEvent(&enterE);
#endif
    EXPECT_EQ(p->isEnter, true);
    p->paintEvent(&e);

    // focus rect branch: TabFocusReason
    p->m_reson = Qt::TabFocusReason;
    p->setFocus(Qt::TabFocusReason);
    p->paintEvent(&e);

    // focus rect branch: BacktabFocusReason
    p->m_reson = Qt::BacktabFocusReason;
    p->paintEvent(&e);

    delete p;
}

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
    EXPECT_EQ(p->getFocusReason(), param.reason);
    delete focusEvent;
    delete p;
}
