/*
 * Copyright (C) 2019 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     LZ <zhou.lu@archermind.com>
 *
 * Maintainer: LZ <zhou.lu@archermind.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "logiconbutton.h"
#include <QEvent>
#include <QResizeEvent>
#include <QDebug>
#define ICON_SIZE 32
LogIconButton::LogIconButton(QWidget *parent)
    : QPushButton(parent)
{
    this->setFlat(true);
    this->setFocusPolicy(Qt::NoFocus);
    this->setIconSize(QSize(32, 32));
}

LogIconButton::LogIconButton(QString text, QWidget *parent)
    : QPushButton(text, parent)
{
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
    return QSize(QPushButton::sizeHint().width(), h);
}

void LogIconButton::mousePressEvent(QMouseEvent *e)
{
    Q_UNUSED(e)
}

bool LogIconButton::event(QEvent *e)
{
    if (e->type() != QEvent::HoverEnter) {
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
    Q_UNUSED(e)
    int h = QFontMetrics(font()).height();
    resize(this->sizeHint().width(), h);

}
