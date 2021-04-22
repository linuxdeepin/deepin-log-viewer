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
#include <stub.h>

#include <QDebug>
#include <QPainter>
#include <QEvent>
#include <QFocusEvent>
#include <DStyle>

int stub_pixelMetric(DStyle::PixelMetric m, const QStyleOption *opt = nullptr, const QWidget *widget = nullptr)
{
    return 1;
}

void stub_drawPrimitive(DStyle::PrimitiveElement pe, const QStyleOption *opt, QPainter *p, const QWidget *w = nullptr)
{
}

class LogViewHeaderView_UT : public testing::Test
{
public:
    //添加日志
    //    static void SetUpTestCase()
    //    {
    //        qDebug() << "SetUpTestCase" << endl;
    //    }
    //    static void TearDownTestCase()
    //    {
    //        qDebug() << "TearDownTestCase" << endl;
    //    }
    void SetUp() //TEST跑之前会执行SetUp
    {
        m_LogViewHeaderView = new LogViewHeaderView(Qt::Vertical);
        qDebug() << "SetUp" << endl;
    }
    void TearDown() //TEST跑完之后会执行TearDown
    {
        delete m_LogViewHeaderView;
    }
    LogViewHeaderView *m_LogViewHeaderView;
};

TEST_F(LogViewHeaderView_UT, LogViewHeaderView_Constructor_UT_001)
{
    EXPECT_NE(m_LogViewHeaderView, nullptr);
}

TEST(LogViewHeaderView_paintSection_UT, LogViewHeaderView_paintSection_UT_001)
{
    Stub stub;
    stub.set((int (DStyle::*)(DStyle::PixelMetric, const QStyleOption *, const QWidget *) const)ADDR(DStyle, pixelMetric), stub_pixelMetric);
    stub.set((void (DStyle::*)(DStyle::PrimitiveElement, const QStyleOption *, QPainter *, const QWidget *) const)ADDR(DStyle, drawPrimitive), stub_drawPrimitive);
    LogViewHeaderView *p = new LogViewHeaderView(Qt::Horizontal, nullptr);
    EXPECT_NE(p, nullptr);
    p->paintSection(new QPainter, p->rect(), 0);
    p->deleteLater();
}

TEST_F(LogViewHeaderView_UT, LogViewHeaderView_focusInEvent_UT)
{
    EXPECT_NE(m_LogViewHeaderView, nullptr);
    QFocusEvent *e = new QFocusEvent(QEvent::FocusOut, Qt::ShortcutFocusReason);
    m_LogViewHeaderView->focusInEvent(e);
    EXPECT_EQ(m_LogViewHeaderView->m_reson, Qt::ShortcutFocusReason);
    delete e;
}

TEST_F(LogViewHeaderView_UT, LogViewHeaderView_paintEvent_UT_001)
{
    Stub stub;
    stub.set((int (DStyle::*)(DStyle::PixelMetric, const QStyleOption *, const QWidget *) const)ADDR(DStyle, pixelMetric), stub_pixelMetric);
    EXPECT_NE(m_LogViewHeaderView, nullptr);
    QPaintEvent repaint(m_LogViewHeaderView->rect());
    m_LogViewHeaderView->paintEvent(&repaint);
}

TEST_F(LogViewHeaderView_UT, LogViewHeaderView_sizeHint_UT_001)
{
    EXPECT_NE(m_LogViewHeaderView, nullptr);
    m_LogViewHeaderView->sizeHint();
}

TEST(LogViewHeaderView_sectionSizeHint_UT, LogViewHeaderView_sectionSizeHint_UT_001)
{
    Stub stub;
    stub.set((int (DStyle::*)(DStyle::PixelMetric, const QStyleOption *, const QWidget *) const)ADDR(DStyle, pixelMetric), stub_pixelMetric);
    LogViewHeaderView *p = new LogViewHeaderView(Qt::Horizontal, nullptr);
    EXPECT_NE(p, nullptr);
    p->sectionSizeHint(0);
    p->deleteLater();
}
