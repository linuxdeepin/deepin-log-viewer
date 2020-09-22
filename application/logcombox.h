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
#ifndef LOGCOMBOX_H
#define LOGCOMBOX_H
#include <DComboBox>
DWIDGET_USE_NAMESPACE
class LogCombox: public DComboBox
{
public:
    explicit LogCombox(QWidget *parent = nullptr);
    void setFocusReason(Qt::FocusReason iReson);
    Qt::FocusReason getFocusReason();
protected:
    void paintEvent(QPaintEvent *e) override;
    void keyPressEvent(QKeyEvent *event) override;
    void focusInEvent(QFocusEvent *event)override;
    void focusOutEvent(QFocusEvent *event)override;
private:
    Qt::FocusReason m_reson;
};

#endif // LOGCOMBOX_H
