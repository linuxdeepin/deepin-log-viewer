/*
* Copyright (C) 2019 ~ 2020 UnionTech Software Technology Co.,Ltd
*
* Author:     linxun <linxun@uniontech.com>
* Maintainer:  linxun <linxun@uniontech.com>
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

#include "dldbushandler.h"
#include "logfileparser.h"

#include <stub.h>
#include <QDebug>
#include <gtest/gtest.h>


TEST(UT_DLDBusHandler_readLog, UT_DLDBusHandler_readLog_001)
{
    DLDBusHandler *DLDbus = new DLDBusHandler();
    ASSERT_TRUE(DLDbus);
    DLDbus->readLog("test");
    delete DLDbus;
}

TEST(UT_DLDBusHandler_exitCode, UT_DLDBusHandler_exitCode_001)
{
    DLDBusHandler *DLDbus = new DLDBusHandler();
    ASSERT_TRUE(DLDbus);
    DLDbus->exitCode();
    delete DLDbus;
}

TEST(UT_DLDBusHandler_quit, UT_DLDBusHandler_quit_001)
{
    DLDBusHandler *DLDbus = new DLDBusHandler();
    ASSERT_TRUE(DLDbus);
    DLDbus->quit();
    delete DLDbus;
}


TEST(UT_DLDBusHandler_exportLog, UT_DLDBusHandler_exportLog_001)
{
    DLDBusHandler *DLDbus = new DLDBusHandler();
    ASSERT_TRUE(DLDbus);
    DLDbus->exportLog("path", "path1", false);
    delete DLDbus;
}


