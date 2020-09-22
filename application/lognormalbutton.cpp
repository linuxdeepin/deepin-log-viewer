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
#include "lognormalbutton.h"
#include <QKeyEvent>
#include <QDebug>
#include <QStylePainter>
#include <QEvent>

#include <DStyle>
#include <DApplication>

LogNormalButton::LogNormalButton(QWidget *parent):
    DPushButton(parent)
{
    setFocusPolicy(Qt::StrongFocus);
}

LogNormalButton::LogNormalButton(const QString &text, QWidget *parent):
    DPushButton(text, parent)
{
    setFocusPolicy(Qt::StrongFocus);
}

LogNormalButton::LogNormalButton(const QIcon &icon, const QString &text, QWidget *parent):
    DPushButton(icon, text, parent)
{
    setFocusPolicy(Qt::StrongFocus);
}
/**
 * @brief LogNormalButton::keyPressEvent
 * 增加回车触发按钮功能,捕获回车键盘事件发送空格键盘事件
 * @param event
 */
void LogNormalButton::keyPressEvent(QKeyEvent *event)
{
    if ((event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return)) {
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
    if ((event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return)) {
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
    if (event->reason() != Qt::ActiveWindowFocusReason) {
        m_reson = event->reason();
    }
    DPushButton::focusInEvent(event);
}


