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
#ifndef LOGSCROLLBAR_H
#define LOGSCROLLBAR_H

#include <QScrollBar>
class LogScrollBar : public QScrollBar
{
public:
public:
    explicit LogScrollBar(QWidget *parent = nullptr);
    explicit LogScrollBar(Qt::Orientation o, QWidget *parent = nullptr);
protected:
    void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
private:
    bool m_isOnPress = false;

};

#endif // LOGSCROLLBAR_H
