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
#include "logspinnerwidget.h"

#include <gtest/gtest.h>
#include <stub.h>

#include <QDebug>
TEST(LogSpinnerWidget_Constructor_UT, LogSpinnerWidget_Constructor_UT_001)
{
    LogSpinnerWidget *p = new LogSpinnerWidget(nullptr);
    EXPECT_NE(p, nullptr);

    p->deleteLater();
}

TEST(LogSpinnerWidget_initUI_UT, LogSpinnerWidget_initUI_UT_001)
{
    LogSpinnerWidget *p = new LogSpinnerWidget(nullptr);
    EXPECT_NE(p, nullptr);
    p->initUI();
    p->deleteLater();
}

TEST(LogSpinnerWidget_spinnerStart_UT, LogSpinnerWidget_spinnerStart_UT)
{
    LogSpinnerWidget *p = new LogSpinnerWidget(nullptr);
    EXPECT_NE(p, nullptr);
    p->spinnerStart();
    p->deleteLater();
}

TEST(LogSpinnerWidget_spinnerStop_UT, LogSpinnerWidget_spinnerStop_UT)
{
    LogSpinnerWidget *p = new LogSpinnerWidget(nullptr);
    EXPECT_NE(p, nullptr);
    p->spinnerStop();
    p->deleteLater();
}


