// SPDX-FileCopyrightText: 2019 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "logtableview.h"

#include <gtest/gtest.h>
#include <stub.h>

#include <QDebug>
#include <QFocusEvent>

TEST(LogTableView_Constructor_UT, LogTableView_Constructor_UT_001)
{
    LogTableView *p = new LogTableView(nullptr);
    ASSERT_NE(p, nullptr);
    EXPECT_EQ(p->objectName(), QString()) << "check object after Constructor()";
    p->deleteLater();
}

TEST(LogTableView_Constructor_UT, LogTableView_Constructor_UT_002)
{
    QWidget *parent = new QWidget();
    LogTableView *p = new LogTableView(parent);
    EXPECT_NE(p, nullptr);
    EXPECT_EQ(p->parent(), parent);
    p->deleteLater();
    parent->deleteLater();
}

TEST(LogTableView_focusInEvent_UT, LogTableView_focusInEvent_UT_001)
{
    LogTableView *p = new LogTableView(nullptr);
    QFocusEvent focusEvent(QEvent::FocusIn, Qt::ActiveWindowFocusReason);
    p->focusInEvent(&focusEvent);
    EXPECT_NE(p, nullptr) << "check the status after focusInEvent()";
    p->deleteLater();
}

TEST(LogTableView_focusOutEvent_UT, LogTableView_focusOutEvent_UT_001)
{
    LogTableView *p = new LogTableView(nullptr);
    QFocusEvent focusEvent(QEvent::FocusOut, Qt::OtherFocusReason);
    p->focusOutEvent(&focusEvent);
    EXPECT_NE(p, nullptr) << "check the status after focusOutEvent()";
    p->deleteLater();
}
