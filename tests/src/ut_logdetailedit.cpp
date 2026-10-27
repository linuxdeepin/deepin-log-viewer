// SPDX-FileCopyrightText: 2020-2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "logdetailedit.h"
#include <stub.h>

#include <QGestureEvent>
#include <QMouseEvent>
#include <QScrollBar>
#include <QTextCursor>
#include <QCoreApplication>
#include <QTapGesture>
#include <QTapAndHoldGesture>
#include <QTest>

#include <gtest/gtest.h>

Qt::MouseEventSource stud_source()
{
    return Qt::MouseEventSynthesizedByQt;
}

class ut_logDetailEdit_Test : public ::testing::Test
{
    // Test interface
protected:
    void SetUp()
    {
        m_infoTextEdit = new logDetailEdit;
    }
    void TearDown()
    {
        delete m_infoTextEdit;
    }

    logDetailEdit *m_infoTextEdit = nullptr;
};

TEST_F(ut_logDetailEdit_Test, ShowInstallInfoTextEdit_UT_slideGesture)
{
    m_infoTextEdit->slideGesture(5);
}

TEST_F(ut_logDetailEdit_Test, ShowInstallInfoTextEdit_UT_onSelectionArea)
{
    m_infoTextEdit->m_gestureAction = logDetailEdit::GA_tap;
    QTextCursor cursor = m_infoTextEdit->textCursor();
    cursor.selectedText() = "//";
    m_infoTextEdit->onSelectionArea();
}

// Cover the clearSelection branch of onSelectionArea (lines 37-39)
TEST_F(ut_logDetailEdit_Test, ShowInstallInfoTextEdit_UT_onSelectionArea_clearSelection)
{
    m_infoTextEdit->setText("hello world log detail edit content");
    m_infoTextEdit->m_gestureAction = logDetailEdit::GA_tap;
    QTextCursor cursor = m_infoTextEdit->textCursor();
    cursor.movePosition(QTextCursor::Start);
    cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, 5);
    m_infoTextEdit->setTextCursor(cursor);
    // setTextCursor triggers selectionChanged -> onSelectionArea with GA_tap
    EXPECT_EQ(m_infoTextEdit->m_gestureAction, logDetailEdit::GA_tap);
}

TEST(FlashTween_TEST, FlashTween_UT_start)
{
    FlashTween flash;
    EXPECT_EQ(flash.sinusoidalEaseOut(1.0, 1.0, 1.0, 1.0), 1.0 * sin(1.0 / 1.0 * (3.14 / 2)) + 1.0);
    flash.start(0, 0, 1, 0, 0);
    flash.start(0, 0, 1, 1, 0);
    flash.m_timer->start(5);
    usleep(5000);
    flash.m_timer->stop();
}

// Cover FlashTween::__run() by providing a valid callback and processing events
TEST(FlashTween_TEST, FlashTween_UT_run)
{
    FlashTween flash;
    qreal captured = 0.0;
    FunSlideInertial callback = [&captured](qreal v) { captured += v; };
    flash.start(0, 0, 10.0, 30.0, callback);
    // Process events so the QTimer fires and __run is invoked
    QCoreApplication::processEvents(QEventLoop::AllEvents);
    QTest::qWait(100);
    QCoreApplication::processEvents(QEventLoop::AllEvents);
    flash.stop();
    SUCCEED();
}

// Cover the FlashTween deleting destructor (D0)
TEST(FlashTween_TEST, FlashTween_UT_deletingDestructor)
{
    FlashTween *flash = new FlashTween();
    delete flash;
    SUCCEED();
}

int states = 1;

int state()
{
    return states;
}

TEST_F(ut_logDetailEdit_Test, ShowInstallInfoTextEdit_UT_tapGestureTriggered)
{
    Stub stub;
    stub.set(ADDR(QGesture, state), state);

    while (states <= 4) {
        QTapGesture tap;
        tap.setPosition(QPointF(10, 10));
        m_infoTextEdit->tapGestureTriggered(&tap);
        states++;
    }
}

// Cover the GestureCanceled else-branch (timeSpace >= TAP_MOVE_DELAY)
TEST_F(ut_logDetailEdit_Test, ShowInstallInfoTextEdit_UT_tapGestureCanceled_selectSlide)
{
    Stub stub;
    stub.set(ADDR(QGesture, state), state);
    states = 3; // Qt::GestureCanceled
    m_infoTextEdit->m_tapBeginTime = 0; // force large timeSpace >= TAP_MOVE_DELAY
    m_infoTextEdit->m_slideContinue = false;
    QTapGesture tap;
    m_infoTextEdit->tapGestureTriggered(&tap);
    EXPECT_EQ(m_infoTextEdit->m_gestureAction, logDetailEdit::GA_null);
}

// Cover tapAndHoldGestureTriggered with safe states (GestureStarted, GestureFinished)
// Note: GestureUpdated(2), GestureCanceled(3), and default hit Q_ASSERT(false) and
// cannot be covered without aborting the process.
TEST_F(ut_logDetailEdit_Test, ShowInstallInfoTextEdit_UT_tapAndHoldGestureTriggered)
{
    Stub stub;
    stub.set(ADDR(QGesture, state), state);

    // GestureStarted (1) - sets GA_hold
    states = 1;
    {
        QTapAndHoldGesture tapAndHold;
        tapAndHold.setPosition(QPointF(10, 10));
        m_infoTextEdit->tapAndHoldGestureTriggered(&tapAndHold);
    }

    // GestureFinished (3) - sets GA_null
    states = 3;
    {
        QTapAndHoldGesture tapAndHold;
        tapAndHold.setPosition(QPointF(10, 10));
        m_infoTextEdit->tapAndHoldGestureTriggered(&tapAndHold);
    }
}

TEST_F(ut_logDetailEdit_Test, ShowInstallInfoTextEdit_UT_gestureEvent)
{
    QGesture *gesture = new QGesture();

    QGestureEvent gestureEvent(QList<QGesture *> {gesture, gesture});
    m_infoTextEdit->gestureEvent(&gestureEvent);
    delete gesture;
}

// Cover gestureEvent tap/tapAndHold branches (lines 68-73) with real gesture types
TEST_F(ut_logDetailEdit_Test, ShowInstallInfoTextEdit_UT_gestureEvent_realGestures)
{
    Stub stub;
    stub.set(ADDR(QGesture, state), state);
    states = 1; // GestureStarted - safe state for both tap and tapAndHold
    QTapGesture *tap = new QTapGesture();
    QTapAndHoldGesture *hold = new QTapAndHoldGesture();
    QGestureEvent gestureEvent(QList<QGesture *> {tap, hold});
    m_infoTextEdit->gestureEvent(&gestureEvent);
    delete tap;
    delete hold;
}

// Cover event() gesture branch (lines 53-54)
TEST_F(ut_logDetailEdit_Test, ShowInstallInfoTextEdit_UT_event_gesture)
{
    Stub stub;
    stub.set(ADDR(QGesture, state), state);
    states = 1; // GestureStarted - safe state for both tap and tapAndHold
    QTapGesture *tap = new QTapGesture();
    QTapAndHoldGesture *hold = new QTapAndHoldGesture();
    QGestureEvent gestureEvent(QList<QGesture *> {tap, hold});
    m_infoTextEdit->event(&gestureEvent);
    delete tap;
    delete hold;
}

// Cover event() non-gesture path
TEST_F(ut_logDetailEdit_Test, ShowInstallInfoTextEdit_UT_event_nonGesture)
{
    QEvent nonGestureEvent(QEvent::MouseMove);
    m_infoTextEdit->event(&nonGestureEvent);
}

TEST_F(ut_logDetailEdit_Test, ShowInstallInfoTextEdit_UT_mouseReleaseEvent)
{
    QMouseEvent releaseEvent(QEvent::MouseButtonRelease, QPoint(0, 0), QPoint(0, 0), Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
    m_infoTextEdit->mouseReleaseEvent(&releaseEvent);
}

// Cover mouseReleaseEvent synthesized+GA_slide branch (lines 175-179)
TEST_F(ut_logDetailEdit_Test, ShowInstallInfoTextEdit_UT_mouseReleaseEvent_synthesized)
{
    Stub stub;
    stub.set(ADDR(QMouseEvent, source), stud_source);
    m_infoTextEdit->m_gestureAction = logDetailEdit::GA_slide;
    QMouseEvent releaseEvent(QEvent::MouseButtonRelease, QPoint(0, 0), QPoint(0, 0), Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
    m_infoTextEdit->mouseReleaseEvent(&releaseEvent);
    EXPECT_EQ(m_infoTextEdit->m_gestureAction, logDetailEdit::GA_null);
}

// Cover mouseReleaseEvent synthesized fast-scroll accept branch (lines 185-186)
TEST_F(ut_logDetailEdit_Test, ShowInstallInfoTextEdit_UT_mouseReleaseEvent_accept)
{
    Stub stub;
    stub.set(ADDR(QMouseEvent, source), stud_source);
    QString longText;
    for (int i = 0; i < 200; i++) {
        longText += QString("line %1\n").arg(i);
    }
    m_infoTextEdit->setText(longText);
    m_infoTextEdit->resize(100, 50);
    m_infoTextEdit->m_start = 0;
    m_infoTextEdit->m_end = 20; // m_end - m_start > 10
    m_infoTextEdit->verticalScrollBar()->setValue(m_infoTextEdit->verticalScrollBar()->minimum() + 1);
    QMouseEvent releaseEvent(QEvent::MouseButtonRelease, QPoint(0, 0), QPoint(0, 0), Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
    m_infoTextEdit->mouseReleaseEvent(&releaseEvent);
    SUCCEED();
}

TEST_F(ut_logDetailEdit_Test, ShowInstallInfoTextEdit_UT_mouseMoveEvent)
{
    QMouseEvent moveEvent(QEvent::MouseMove, QPoint(0, 0), QPoint(10, 0), Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
    m_infoTextEdit->mouseMoveEvent(&moveEvent);
}

TEST_F(ut_logDetailEdit_Test, ShowInstallInfoTextEdit_UT_mouseMoveEvent_source)
{
    Stub stub;
    stub.set(ADDR(QMouseEvent, source), stud_source);

    m_infoTextEdit->m_gestureAction = logDetailEdit::GA_slide;
    QMouseEvent moveEvent(QEvent::MouseMove, QPoint(0, 0), QPoint(10, 0), Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
    m_infoTextEdit->mouseMoveEvent(&moveEvent);
}
