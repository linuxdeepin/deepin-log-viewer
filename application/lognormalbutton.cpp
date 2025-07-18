// SPDX-FileCopyrightText: 2019 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "lognormalbutton.h"
#include <QKeyEvent>
#include <QDebug>
#include <QStylePainter>
#include <QEvent>

#include <DStyle>
#include <DApplication>
#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(logApp)

LogNormalButton::LogNormalButton(QWidget *parent):
    DPushButton(parent)
{
    // qCDebug(logApp) << "LogNormalButton default constructor called";
    setFocusPolicy(Qt::StrongFocus);
}

LogNormalButton::LogNormalButton(const QString &text, QWidget *parent):
    DPushButton(text, parent)
{
    // qCDebug(logApp) << "LogNormalButton constructor with text called:" << text;
    setFocusPolicy(Qt::StrongFocus);
}

LogNormalButton::LogNormalButton(const QIcon &icon, const QString &text, QWidget *parent):
    DPushButton(icon, text, parent)
{
    // qCDebug(logApp) << "LogNormalButton constructor with icon and text called:" << text;
    setFocusPolicy(Qt::StrongFocus);
}

/**
 * @brief LogNormalButton::keyPressEvent
 * 增加回车触发按钮功能,捕获回车键盘事件发送空格键盘事件
 * @param event
 */
void LogNormalButton::keyPressEvent(QKeyEvent *event)
{
    // qCDebug(logApp) << "LogNormalButton::keyPressEvent called with key:" << event->key();
    if ((event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return)) {
        qCDebug(logApp) << "Enter/Return key pressed, sending space key event";
        QKeyEvent spaceEvent(QEvent::KeyPress, Qt::Key_Space, Qt::NoModifier, 65, 32, 16, " ", event->isAutoRepeat(), static_cast<ushort>(event->count()));
        DApplication::sendEvent(this, &spaceEvent);
        return;
    }
    DPushButton::keyPressEvent(event);
}

/**
 * @brief LogNormalButton::keyReleaseEvent
 * 增加回车触发按钮功能,捕获回车键盘事件发送空格键盘事件
 * @param event
 */
void LogNormalButton::keyReleaseEvent(QKeyEvent *event)
{
    // qCDebug(logApp) << "LogNormalButton::keyReleaseEvent called with key:" << event->key();
    if ((event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return)) {
        qCDebug(logApp) << "Enter/Return key released, sending space key release event";
        QKeyEvent spaceEvent(QEvent::KeyRelease, Qt::Key_Space, Qt::NoModifier, 65, 32, 16, " ", event->isAutoRepeat(), static_cast<ushort>(event->count()));
        DApplication::sendEvent(this, &spaceEvent);
        return;

    }
    DPushButton::keyReleaseEvent(event);
}

/**
 * @brief LogNormalButton::paintEvent
 * 绘制焦点边框,屏蔽默认绘制事件,只在tabfoucus时绘制边框
 * @param e
 */
void LogNormalButton::paintEvent(QPaintEvent *e)
{
    // qCDebug(logApp) << "LogNormalButton::paintEvent called";
    Q_UNUSED(e)
    DStyle *style = dynamic_cast<DStyle *>(DApplication::style());
    QStyleOptionButton btn;
    initStyleOption(&btn);
    QStylePainter painter(this);
    style->proxy()->drawControl(DStyle::CE_PushButtonBevel, &btn, &painter, this);
    QStyleOptionButton subopt = btn;
    subopt.rect = style->proxy()->subElementRect(DStyle::SE_PushButtonContents, &btn, this);
    style->proxy()->drawControl(DStyle::CE_PushButtonLabel, &subopt,  &painter, this);
    if (hasFocus() && (m_reson == Qt::TabFocusReason || m_reson == Qt::BacktabFocusReason)) {
        // qCDebug(logApp) << "Drawing focus rect for tab focus";
        QStyleOptionFocusRect fropt;
        fropt.QStyleOption::operator=(btn);
        fropt.rect = style->proxy()->subElementRect(DStyle::SE_PushButtonFocusRect, & btn, this);
        style->proxy()->drawPrimitive(DStyle::PE_FrameFocusRect, &fropt, &painter, this);
    }
}

/**
 * @brief LogNormalButton::focusInEvent
 * 捕获最近一次获得焦点的reason以区分是否为tabfoucs
 * @param event
 */
void LogNormalButton::focusInEvent(QFocusEvent *event)
{
    // qCDebug(logApp) << "LogNormalButton::focusInEvent called with reason:" << event->reason();
    if (event->reason() != Qt::ActiveWindowFocusReason) {
        m_reson = event->reason();
        // qCDebug(logApp) << "Focus reason updated to:" << m_reson;
    }
    DPushButton::focusInEvent(event);
}
