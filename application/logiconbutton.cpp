// SPDX-FileCopyrightText: 2019 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "logiconbutton.h"

#include <QEvent>
#include <QResizeEvent>
#include <QDebug>
#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(logApp)

#define ICON_SIZE 32

LogIconButton::LogIconButton(QWidget *parent)
    : QPushButton(parent)
{
    // qCDebug(logApp) << "LogIconButton constructor entered";
    this->setFlat(true);
    this->setFocusPolicy(Qt::NoFocus);
    this->setIconSize(QSize(32, 32));
    // qCDebug(logApp) << "LogIconButton constructor exited";
}

LogIconButton::LogIconButton(const QString &text, QWidget *parent)
    : QPushButton(text, parent)
{
    // qCDebug(logApp) << "LogIconButton constructor with text called:" << text;
    this->setFlat(true);
    this->setFocusPolicy(Qt::NoFocus);
    this->setIconSize(QSize(32, 32));
}
//修复button占高过大的问题
/**
 * @brief LogIconButton::sizeHint
 *  修复iconbutton因为图表过大把按钮撑大的问题,根据自体计算按钮高度
 * @return
 */
QSize LogIconButton::sizeHint() const
{
    int h = QFontMetrics(font()).height();
    // qCDebug(logApp) << "LogIconButton::sizeHint calculated height:" << h;
    return QSize(QPushButton::sizeHint().width(), h);
}

void LogIconButton::mousePressEvent(QMouseEvent *e)
{
    // qCDebug(logApp) << "LogIconButton::mousePressEvent called";
    Q_UNUSED(e)
}

bool LogIconButton::event(QEvent *e)
{
    if (e->type() != QEvent::HoverEnter) {
        // qCDebug(logApp) << "LogIconButton::event handling event type:" << e->type();
        QPushButton::event(e);
    }
    return  false;
}




/**
 * @brief LogIconButton::resizeEvent
 * 修复iconbutton因为图表过大把按钮撑大的问题,根据自体计算按钮高度
 * @param e
 */
void LogIconButton::resizeEvent(QResizeEvent *e)
{
    // qCDebug(logApp) << "LogIconButton::resizeEvent called";
    Q_UNUSED(e)
    int h = QFontMetrics(font()).height();
    resize(this->sizeHint().width(), h);
}
