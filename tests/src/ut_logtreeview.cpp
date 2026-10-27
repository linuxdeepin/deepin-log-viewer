// SPDX-FileCopyrightText: 2019 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "logtreeview.h"

#include <gtest/gtest.h>
#include <stub.h>

#include <DStyle>

#include <QDebug>
#include <QEventPoint>
#include <QFocusEvent>
#include <QGuiApplication>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QPainter>
#include <QPixmap>
#include <QPointingDevice>
#include <QScrollBar>
#include <QSignalSpy>
#include <QStandardItemModel>
#include <QTest>
#include <QTouchEvent>
#include <QWidget>

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
    void SetUp()
    {
        m_treeView = new LogTreeView();
    }
    void TearDown()
    {
        delete m_treeView;
    }
    LogTreeView *m_treeView;
};

// Constructor + initUI: covered implicitly by SetUp; verify delegates installed.
TEST_F(LogTreeView_UT, constructor_UT)
{
    EXPECT_NE(m_treeView->itemDelegate(), nullptr);
    EXPECT_NE(m_treeView->header(), nullptr);
    EXPECT_FALSE(m_treeView->rootIsDecorated());
    EXPECT_EQ(m_treeView->editTriggers(), QAbstractItemView::NoEditTriggers);
    EXPECT_TRUE(m_treeView->testAttribute(Qt::WA_AcceptTouchEvents));
}

// singleRowHeight: null model -> -1
TEST_F(LogTreeView_UT, singleRowHeight_nullModel_UT)
{
    EXPECT_EQ(-1, m_treeView->singleRowHeight());
}

// singleRowHeight: valid model with index -> rowHeight (>0 after show)
TEST_F(LogTreeView_UT, singleRowHeight_validModel_UT)
{
    QStandardItemModel model(1, 1);
    model.setItem(0, 0, new QStandardItem("row0"));
    m_treeView->setModel(&model);
    // rowHeight requires the view to be laid out; index validity is the key branch.
    QModelIndex idx = m_treeView->model()->index(0, 0);
    ASSERT_TRUE(idx.isValid());
    int h = m_treeView->singleRowHeight();
    EXPECT_GE(h, 0);
    m_treeView->setModel(nullptr);
}

// singleRowHeight: model present but no rows -> invalid index -> -1
TEST_F(LogTreeView_UT, singleRowHeight_emptyModel_UT)
{
    QStandardItemModel model(0, 1);
    m_treeView->setModel(&model);
    EXPECT_EQ(-1, m_treeView->singleRowHeight());
    m_treeView->setModel(nullptr);
}

// paintEvent
TEST_F(LogTreeView_UT, paintEvent_UT)
{
    emit m_treeView->verticalScrollBar()->sliderPressed();
    QPaintEvent repaint(m_treeView->rect());
    m_treeView->paintEvent(&repaint);
    SUCCEED();
}

// drawRow: invalid index (odd-row branch), stubbed style metrics
TEST_F(LogTreeView_UT, drawRow_UT)
{
    Stub stub;
    stub.set((int (DStyle::*)(DStyle::PixelMetric, const QStyleOption *, const QWidget *) const)ADDR(DStyle, pixelMetric), stub_pixelMetric002);
    stub.set((void (DStyle::*)(DStyle::PrimitiveElement, const QStyleOption *, QPainter *, const QWidget *) const)ADDR(DStyle, drawPrimitive), stub_drawPrimitive002);
    QPixmap pixmap(100, 100);
    QPainter painter(&pixmap);
    QStyleOptionViewItem option;
    m_treeView->drawRow(&painter, option, QModelIndex());
    SUCCEED();
}

// drawRow: valid index (even row) with selection to exercise selected branch.
// The widget is shown and laid out so QTreeView::drawRow can render safely.
TEST_F(LogTreeView_UT, drawRow_selectedEvenRow_UT)
{
    Stub stub;
    stub.set((int (DStyle::*)(DStyle::PixelMetric, const QStyleOption *, const QWidget *) const)ADDR(DStyle, pixelMetric), stub_pixelMetric002);
    stub.set((void (DStyle::*)(DStyle::PrimitiveElement, const QStyleOption *, QPainter *, const QWidget *) const)ADDR(DStyle, drawPrimitive), stub_drawPrimitive002);
    QStandardItemModel model(2, 1);
    model.setItem(0, 0, new QStandardItem("even"));
    model.setItem(1, 0, new QStandardItem("odd"));
    m_treeView->setModel(&model);
    m_treeView->setCurrentIndex(model.index(0, 0));
    m_treeView->resize(200, 100);
    m_treeView->show();
    QTest::qWait(50);
    QPixmap pixmap(200, 100);
    QPainter painter(&pixmap);
    QStyleOptionViewItem option;
    option.rect = m_treeView->visualRect(model.index(0, 0));
    option.state |= QStyle::State_Enabled | QStyle::State_Selected;
    m_treeView->drawRow(&painter, option, model.index(0, 0));
    m_treeView->setModel(nullptr);
    SUCCEED();
}

// keyPressEvent: Key_Up emits pressed(currentIndex)
TEST_F(LogTreeView_UT, keyPressEvent_KeyUp_UT)
{
    QSignalSpy spy(m_treeView, &LogTreeView::pressed);
    QKeyEvent event(QEvent::KeyPress, Qt::Key_Up, Qt::NoModifier);
    m_treeView->keyPressEvent(&event);
    EXPECT_EQ(spy.count(), 1);
}

// keyPressEvent: Key_Down emits pressed(currentIndex)
TEST_F(LogTreeView_UT, keyPressEvent_KeyDown_UT)
{
    QSignalSpy spy(m_treeView, &LogTreeView::pressed);
    QKeyEvent event(QEvent::KeyPress, Qt::Key_Down, Qt::NoModifier);
    m_treeView->keyPressEvent(&event);
    EXPECT_EQ(spy.count(), 1);
}

// keyPressEvent: other key does not emit pressed
TEST_F(LogTreeView_UT, keyPressEvent_OtherKey_UT)
{
    QSignalSpy spy(m_treeView, &LogTreeView::pressed);
    QKeyEvent event(QEvent::KeyPress, Qt::Key_0, Qt::NoModifier);
    m_treeView->keyPressEvent(&event);
    EXPECT_EQ(spy.count(), 0);
}

// event: TouchBegin with a real touch-screen device and a single touch point.
TEST_F(LogTreeView_UT, event_TouchBeginSinglePoint_UT)
{
    QPointingDevice device("testTouch", 1, QInputDevice::DeviceType::TouchScreen,
                           QPointingDevice::PointerType::Finger,
                           QPointingDevice::Capability::Position, 1, 1);
    QList<QEventPoint> points;
    points << QEventPoint(0, QEventPoint::State::Pressed, QPointF(10, 10), QPointF(10, 10));
    QTouchEvent touchEvent(QEvent::TouchBegin, &device, Qt::NoModifier, points);
    EXPECT_FALSE(m_treeView->m_isPressed);
    bool ret = m_treeView->event(&touchEvent);
    EXPECT_TRUE(m_treeView->m_isPressed);
    (void)ret;
}

// event: TouchBegin while already pressed -> inner branch skipped
TEST_F(LogTreeView_UT, event_TouchBeginAlreadyPressed_UT)
{
    m_treeView->m_isPressed = true;
    QPointingDevice device("testTouch", 1, QInputDevice::DeviceType::TouchScreen,
                           QPointingDevice::PointerType::Finger,
                           QPointingDevice::Capability::Position, 1, 1);
    QList<QEventPoint> points;
    points << QEventPoint(0, QEventPoint::State::Pressed, QPointF(20, 20), QPointF(20, 20));
    QTouchEvent touchEvent(QEvent::TouchBegin, &device, Qt::NoModifier, points);
    m_treeView->event(&touchEvent);
    EXPECT_TRUE(m_treeView->m_isPressed);
}

// event: non-TouchBegin event just delegates to parent
TEST_F(LogTreeView_UT, event_nonTouch_UT)
{
    QEvent hideEvent(QEvent::Hide);
    bool ret = m_treeView->event(&hideEvent);
    EXPECT_FALSE(m_treeView->m_isPressed);
    (void)ret;
}

// mouseMoveEvent: pressed -> custom scroll path
TEST_F(LogTreeView_UT, mouseMoveEvent_pressed_UT)
{
    m_treeView->m_isPressed = true;
    m_treeView->m_lastTouchBeginPos = QPointF(0, 0);
    QMouseEvent moveEvent(QEvent::MouseMove, QPoint(50, 50), QPoint(50, 50), Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
    m_treeView->mouseMoveEvent(&moveEvent);
    EXPECT_TRUE(m_treeView->m_isPressed);
}

// mouseMoveEvent: not pressed -> parent path
TEST_F(LogTreeView_UT, mouseMoveEvent_notPressed_UT)
{
    m_treeView->m_isPressed = false;
    QMouseEvent moveEvent(QEvent::MouseMove, QPoint(0, 0), QPoint(10, 0), Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
    m_treeView->mouseMoveEvent(&moveEvent);
    EXPECT_FALSE(m_treeView->m_isPressed);
}

// mouseReleaseEvent: pressed -> resets flag
TEST_F(LogTreeView_UT, mouseReleaseEvent_pressed_UT)
{
    m_treeView->m_isPressed = true;
    QMouseEvent event(QEvent::MouseButtonRelease, QPointF(20, 20), QPointF(20, 20), Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
    m_treeView->mouseReleaseEvent(&event);
    EXPECT_FALSE(m_treeView->m_isPressed);
}

// mouseReleaseEvent: not pressed -> parent path
TEST_F(LogTreeView_UT, mouseReleaseEvent_notPressed_UT)
{
    m_treeView->m_isPressed = false;
    QMouseEvent event(QEvent::MouseButtonRelease, QPointF(20, 20), QPointF(20, 20), Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
    m_treeView->mouseReleaseEvent(&event);
    EXPECT_FALSE(m_treeView->m_isPressed);
}

// focusInEvent: records the focus reason
TEST_F(LogTreeView_UT, focusInEvent_UT)
{
    EXPECT_NE(m_treeView->m_reson, Qt::TabFocusReason);
    QFocusEvent focusEvent(QEvent::FocusIn, Qt::TabFocusReason);
    m_treeView->focusInEvent(&focusEvent);
    EXPECT_EQ(m_treeView->m_reson, Qt::TabFocusReason);
}

// focusInEvent: BacktabFocusReason
TEST_F(LogTreeView_UT, focusInEvent_backtab_UT)
{
    QFocusEvent focusEvent(QEvent::FocusIn, Qt::BacktabFocusReason);
    m_treeView->focusInEvent(&focusEvent);
    EXPECT_EQ(m_treeView->m_reson, Qt::BacktabFocusReason);
}
