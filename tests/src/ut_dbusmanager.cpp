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

#include <gtest/gtest.h>
#include "../application/dbusmanager.h"
#include <stub.h>

TEST(DBusManager_Constructor_UT, DBusManager_Constructor_UT_001)
{
    DBusManager *p = new DBusManager(nullptr);
    EXPECT_NE(p, nullptr);
    delete p;
}
TEST(DBusManager_GetSystemInfo_UT, DBusManager_GetSystemInfo_UT_001)
{
    DBusManager *p = new DBusManager(nullptr);
    EXPECT_NE(p, nullptr);
    QString info = p->getSystemInfo();
    bool rs = (info == "" || info == "klu" || info == "panguV" || info == "pangu");
    EXPECT_EQ(rs, true);
    delete p;
}
