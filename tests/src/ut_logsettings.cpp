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
#include "logsettings.h"

#include <gtest/gtest.h>
#include <stub.h>

#include <QDebug>
#include <QSize>
TEST(LogSettings_Constructor_UT, LogSettings_Constructor_UT_001)
{
    LogSettings *p = new LogSettings(nullptr);
    EXPECT_NE(p, nullptr);

    p->deleteLater();
}

TEST(LogSettings_getConfigWinSize_UT, LogSettings_getConfigWinSize_UT_001)
{
    LogSettings *p = new LogSettings(nullptr);
    EXPECT_NE(p, nullptr);
    EXPECT_EQ(p->getConfigWinSize().isValid(), true);
    p->deleteLater();
}

TEST(LogSettings_saveConfigWinSize_UT, LogSettings_saveConfigWinSize_UT_001)
{
    LogSettings *p = new LogSettings(nullptr);
    EXPECT_NE(p, nullptr);
    p->saveConfigWinSize(100, 200);
    p->deleteLater();
}
