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
#define ICON_SIZE 32
LogIconButton::LogIconButton(QWidget *parent)
    : QPushButton(parent)
{
    this->setFlat(true);
    this->setFocusPolicy(Qt::NoFocus);

}

LogIconButton::LogIconButton(QString text, QWidget *parent)
    : QPushButton(text, parent)
{
    this->setFlat(true);
    this->setFocusPolicy(Qt::NoFocus);

}

/**
 * @brief LogIconButton::sizeHint 让此按钮高度与文字内容高度保持一致，防止放置此button的layout高度被撑高
 * @return
 */
QSize LogIconButton::sizeHint() const
{
    int h = QFontMetrics(font()).height();
    return QSize(this->width(), h);
}

void LogIconButton::mousePressEvent(QMouseEvent *e)
{
    Q_UNUSED(e)
}

/**
 * @brief LogIconButton::resizeEvent 在此虚函数中重设icon的高度，使其与字体大小保持一致
 * @param e
 */
void LogIconButton::resizeEvent(QResizeEvent *e)
{
    Q_UNUSED(e)
    int h = QFontMetrics(font()).height();
    resize(this->width(), h);
    this->setIconSize(QSize(h, h));

}
