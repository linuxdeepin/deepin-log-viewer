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
#include "logtreeview.h"

#include <gtest/gtest.h>
#include <stub.h>

#include <DStyle>

#include <QDebug>
#include <QPaintEvent>
#include <QPainter>
DWIDGET_USE_NAMESPACE

int stub_pixelMetric002(DStyle::PixelMetric m, const QStyleOption *opt = nullptr, const QWidget *widget = nullptr)
{
    return 1;
}

void stub_drawPrimitive002(DStyle::PrimitiveElement pe, const QStyleOption *opt, QPainter *p, const QWidget *w = nullptr)
{
}

class LogTreeView_UT : public testing::Test
{
public:
    //添加日志
    static void SetUpTestCase()
    {
        qDebug() << "SetUpTestCase" << endl;
    }
    static void TearDownTestCase()
    {
        qDebug() << "TearDownTestCase" << endl;
    }
    void SetUp() //TEST跑之前会执行SetUp
    {
        m_treeView = new LogTreeView();
        qDebug() << "SetUp" << endl;
    }
    void TearDown() //TEST跑完之后会执行TearDown
    {
        delete m_treeView;
    }
    LogTreeView *m_treeView;
};

TEST_F(LogTreeView_UT, singleRowHeight_UT)
{
    EXPECT_EQ(-1, m_treeView->singleRowHeight());
}

TEST_F(LogTreeView_UT, paintEvent_UT)
{
    QPaintEvent repait(m_treeView->rect());
    m_treeView->paintEvent(&repait);
}

TEST_F(LogTreeView_UT, drawRow_UT)
{
    Stub stub;
    stub.set((int (DStyle::*)(DStyle::PixelMetric, const QStyleOption *, const QWidget *) const)ADDR(DStyle, pixelMetric), stub_pixelMetric002);
    stub.set((void (DStyle::*)(DStyle::PrimitiveElement, const QStyleOption *, QPainter *, const QWidget *) const)ADDR(DStyle, drawPrimitive), stub_drawPrimitive002);
    QPainter painter;
    QStyleOptionViewItem option;
    m_treeView->drawRow(&painter, option, QModelIndex());
}

TEST_F(LogTreeView_UT, keyPressEvent_UT)
{
    QKeyEvent event(QEvent::KeyPress, Qt::Key_Up, Qt::NoModifier);
    m_treeView->keyPressEvent(&event);
}

TEST_F(LogTreeView_UT, mouseMoveEvent_UT)
{
    QMouseEvent moveEvent(QEvent::MouseMove, QPoint(0, 0), QPoint(10, 0), Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
    m_treeView->m_isPressed = true;
    m_treeView->mouseMoveEvent(&moveEvent);
    ;
}

TEST_F(LogTreeView_UT, mousePressEvent_UT)
{
    QMouseEvent event(QEvent::MouseButtonPress, QPointF(0, 0), Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
    m_treeView->mousePressEvent(&event);
}

TEST_F(LogTreeView_UT, mouseReleaseEvent_UT)
{
    QMouseEvent event(QEvent::MouseButtonRelease, QPointF(20, 20), Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
    m_treeView->mouseReleaseEvent(&event);
}

TEST_F(LogTreeView_UT, Event_UT)
{
    QEvent en(QEvent::TouchBegin);
    m_treeView->event(&en);
}

//TEST(LogTreeView_Constructor_UT, LogTreeView_Constructor_UT_001)
//{
//    LogTreeView *p = new LogTreeView(nullptr);
//    EXPECT_NE(p, nullptr);

//    p->deleteLater();
//}

//TEST(LogTreeView_singleRowHeight_UT, LogTreeView_singleRowHeight_UT_001)
//{
//    LogTreeView *p = new LogTreeView(nullptr);
//    EXPECT_NE(p, nullptr);
//    p->singleRowHeight();
//    p->deleteLater();
//}

//TEST(LogTreeView_initUI_UT, LogTreeView_initUI_UT_001)
//{
//    LogTreeView *p = new LogTreeView(nullptr);
//    EXPECT_NE(p, nullptr);
//    p->initUI();
//    p->deleteLater();
//}

////TEST(LogTreeView_paintEvent_UT, LogTreeView_paintEvent_UT_001)
////{
////    LogTreeView *p = new LogTreeView(nullptr);
////    EXPECT_NE(p, nullptr);
////    p->paintEvent(new QPaintEvent(p->rect()));
////    p->deleteLater();
////}

////TEST(LogTreeView_drawRow_UT, LogTreeView_drawRow_UT_001)
////{
////    LogTreeView *p = new LogTreeView(nullptr);
////    EXPECT_NE(p, nullptr);
////    p->drawRow(new QPainter, QStyleOptionViewItem(), QModelIndex());
////    p->deleteLater();
////}

//class LogTreeView_keyPressEvent_UT_Param
//{
//public:
//    LogTreeView_keyPressEvent_UT_Param(int iKey)
//    {
//        key = iKey;
//    }
//    int key;
//};

//class LogTreeView_keyPressEvent_UT : public ::testing::TestWithParam<LogTreeView_keyPressEvent_UT_Param>
//{
//};

//INSTANTIATE_TEST_CASE_P(LogTreeView, LogTreeView_keyPressEvent_UT, ::testing::Values(LogTreeView_keyPressEvent_UT_Param(Qt::Key_Up), LogTreeView_keyPressEvent_UT_Param(Qt::Key_Down), LogTreeView_keyPressEvent_UT_Param(Qt::Key_0)));

//TEST_P(LogTreeView_keyPressEvent_UT, LogTreeView_keyPressEvent_UT_001)
//{
//    LogTreeView_keyPressEvent_UT_Param param = GetParam();
//    LogTreeView *p = new LogTreeView(nullptr);
//    EXPECT_NE(p, nullptr);

//    QKeyEvent *keyEvent = new QKeyEvent(QEvent::KeyPress, param.key, Qt::NoModifier);
//    p->keyPressEvent(keyEvent);
//    p->deleteLater();
//}

//class LogTreeView_event_UT_Param
//{
//public:
//    LogTreeView_event_UT_Param(QEvent::Type iKey)
//    {
//        key = iKey;
//    }
//    QEvent::Type key;
//};

//class LogTreeView_event_UT : public ::testing::TestWithParam<LogTreeView_event_UT_Param>
//{
//};

//INSTANTIATE_TEST_CASE_P(LogTreeView, LogTreeView_event_UT, ::testing::Values(LogTreeView_event_UT_Param(QEvent::TouchBegin), LogTreeView_event_UT_Param(QEvent::Hide)));

//TEST_P(LogTreeView_event_UT, LogTreeView_event_UT_001)
//{
//    LogTreeView_event_UT_Param param = GetParam();
//    LogTreeView *p = new LogTreeView(nullptr);
//    EXPECT_NE(p, nullptr);
//    QTouchEvent a(param.key);
//    p->event(&a);
//    p->deleteLater();
//}

//TEST(LogTreeView_mousePressEvent_UT, LogTreeView_mousePressEvent_UT)
//{
//    LogTreeView *p = new LogTreeView(nullptr);
//    EXPECT_NE(p, nullptr);
//    p->mousePressEvent(new QMouseEvent(QEvent::MouseButtonPress, QPoint(1, 1), Qt::LeftButton, Qt::LeftButton, Qt::NoModifier));
//    p->deleteLater();
//}

//TEST(LogTreeView_mouseMoveEvent_UT, LogTreeView_mouseMoveEvent_UT_001)
//{
//    LogTreeView *p = new LogTreeView(nullptr);
//    EXPECT_NE(p, nullptr);
//    p->m_isPressed = true;
//    p->mouseMoveEvent(new QMouseEvent(QEvent::MouseMove, QPoint(1, 1), Qt::NoButton, Qt::NoButton, Qt::NoModifier));
//    p->deleteLater();
//}

//TEST(LogTreeView_mouseMoveEvent_UT, LogTreeView_mouseMoveEvent_UT_002)
//{
//    LogTreeView *p = new LogTreeView(nullptr);
//    EXPECT_NE(p, nullptr);
//    p->m_isPressed = false;
//    p->mouseMoveEvent(new QMouseEvent(QEvent::MouseMove, QPoint(1, 1), Qt::NoButton, Qt::NoButton, Qt::NoModifier));
//    p->deleteLater();
//}

//TEST(LogTreeView_mouseReleaseEvent_UT, LogTreeView_mouseReleaseEvent_UT_001)
//{
//    LogTreeView *p = new LogTreeView(nullptr);
//    EXPECT_NE(p, nullptr);
//    p->m_isPressed = false;
//    p->mouseReleaseEvent(new QMouseEvent(QEvent::MouseButtonRelease, QPoint(1, 1), Qt::LeftButton, Qt::LeftButton, Qt::NoModifier));
//    p->deleteLater();
//}

//TEST(LogTreeView_mouseReleaseEvent_UT, LogTreeView_mouseReleaseEvent_UT_002)
//{
//    LogTreeView *p = new LogTreeView(nullptr);
//    EXPECT_NE(p, nullptr);
//    p->m_isPressed = true;
//    p->mouseReleaseEvent(new QMouseEvent(QEvent::MouseButtonRelease, QPoint(1, 1), Qt::LeftButton, Qt::LeftButton, Qt::NoModifier));
//    p->deleteLater();
//}

//TEST(LogTreeView_focusInEvent_UT, LogTreeView_focusInEvent_UT_001)
//{
//    LogTreeView *p = new LogTreeView(nullptr);
//    EXPECT_NE(p, nullptr);

//    QFocusEvent *focusEvent = new QFocusEvent(QEvent::FocusIn);
//    p->focusInEvent(focusEvent);
//    p->deleteLater();
//}
