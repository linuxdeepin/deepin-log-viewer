// SPDX-FileCopyrightText: 2019 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "logiconbutton.h"

#include <gtest/gtest.h>

#include <QEvent>
#include <QMouseEvent>
#include <QResizeEvent>
#include <QSize>
#include <QFontMetrics>

// Constructor with parent only: verify flat, focus policy and icon size.
TEST(LogIconButton_Constructor, Constructor_ParentOnly)
{
    LogIconButton *btn = new LogIconButton(nullptr);
    ASSERT_NE(btn, nullptr);
    EXPECT_TRUE(btn->isFlat());
    EXPECT_EQ(btn->focusPolicy(), Qt::NoFocus);
    EXPECT_EQ(btn->iconSize(), QSize(32, 32));
    delete btn;
}

// Constructor with text and parent: verify text plus flat, focus policy, icon size.
TEST(LogIconButton_Constructor, Constructor_TextAndParent)
{
    const QString text = QStringLiteral("test_icon");
    LogIconButton *btn = new LogIconButton(text, nullptr);
    ASSERT_NE(btn, nullptr);
    EXPECT_EQ(btn->text(), text);
    EXPECT_TRUE(btn->isFlat());
    EXPECT_EQ(btn->focusPolicy(), Qt::NoFocus);
    EXPECT_EQ(btn->iconSize(), QSize(32, 32));
    delete btn;
}

// sizeHint() must return a size whose height equals font metrics height and
// whose width equals QPushButton::sizeHint().width().
TEST(LogIconButton_SizeHint, ReturnsValidSize)
{
    LogIconButton *btn = new LogIconButton(QStringLiteral("abc"), nullptr);
    const QSize hint = btn->sizeHint();
    EXPECT_GT(hint.width(), 0);
    EXPECT_GT(hint.height(), 0);
    const int fontHeight = QFontMetrics(btn->font()).height();
    EXPECT_EQ(hint.height(), fontHeight);
    delete btn;
}

// mousePressEvent() is a no-op; calling with a valid event must not crash.
TEST(LogIconButton_MousePressEvent, DoesNotCrashWithValidEvent)
{
    LogIconButton *btn = new LogIconButton(QStringLiteral("x"), nullptr);
    QMouseEvent me(QEvent::MouseButtonPress, QPointF(0, 0), QPointF(0, 0),
                   Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
    btn->mousePressEvent(&me);
    SUCCEED();
    delete btn;
}

// event() returns false for non-HoverEnter events (and forwards to QPushButton::event).
TEST(LogIconButton_Event, ReturnsFalseForNonHoverEnter)
{
    LogIconButton *btn = new LogIconButton(QStringLiteral("y"), nullptr);
    QEvent e(QEvent::QueryWhatsThis);
    EXPECT_FALSE(btn->event(&e));
    delete btn;
}

// event() returns false for HoverEnter events (does not forward to QPushButton::event).
TEST(LogIconButton_Event, ReturnsFalseForHoverEnter)
{
    LogIconButton *btn = new LogIconButton(QStringLiteral("z"), nullptr);
    QEvent e(QEvent::HoverEnter);
    EXPECT_FALSE(btn->event(&e));
    delete btn;
}

// resizeEvent() resizes the button to sizeHint().width() x fontMetrics().height().
TEST(LogIconButton_ResizeEvent, ResizesToSizeHint)
{
    LogIconButton *btn = new LogIconButton(QStringLiteral("r"), nullptr);
    const QSize oldSize(200, 200);
    btn->resize(oldSize);
    QResizeEvent re(QSize(100, 100), oldSize);
    btn->resizeEvent(&re);
    const int expectedHeight = QFontMetrics(btn->font()).height();
    const int expectedWidth = btn->sizeHint().width();
    EXPECT_EQ(btn->width(), expectedWidth);
    EXPECT_EQ(btn->height(), expectedHeight);
    delete btn;
}
