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

#ifndef LOGSPINNERWIDGET_H
#define LOGSPINNERWIDGET_H

#include <DSpinner>
#include <DWidget>

class LogSpinnerWidget : public Dtk::Widget::DWidget
{
public:
    LogSpinnerWidget();
    void initUI();

    void spinnerStart();
    void spinnerStop();

private:
    Dtk::Widget::DSpinner *m_spinner;
};

#endif  // LOGSPINNERWIDGET_H
