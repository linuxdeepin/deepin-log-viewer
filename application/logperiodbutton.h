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

#ifndef LOGPERIODBUTTON_H
#define LOGPERIODBUTTON_H
#include <DPushButton>
#include <QEvent>

class LogPeriodButton : public Dtk::Widget::DPushButton
{
public:
    LogPeriodButton(const QString text, QWidget *parent = nullptr);
    void setStandardSize(int iStahndardWidth);
    void enterEvent(QEvent *e);
    void leaveEvent(QEvent *e);

    void paintEvent(QPaintEvent *event);

private:
    bool isEnter {false};
    int m_stahndardWidth = -1;
};

#endif  // LOGPERIODBUTTON_H
