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

#ifndef MYICONBUTTON_H
#define MYICONBUTTON_H

#include <QPushButton>
/**
 * @brief The LogIconButton class 详细信息部分显示等级图标和文字内容的无边框button
 */
class LogIconButton : public QPushButton
{
public:
    explicit LogIconButton(QWidget *parent = nullptr);
    explicit LogIconButton(QString text, QWidget *parent = nullptr);
protected:
    QSize sizeHint() const override;
    void resizeEvent(QResizeEvent *e)override;
    void mousePressEvent(QMouseEvent *e) override;
};

#endif  // MYICONBUTTON_H
