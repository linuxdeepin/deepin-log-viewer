/*
* Copyright (C) 2022 UnionTech Software Technology Co.,Ltd
*
* Author:     linxinping <linxinping@uniontech.com>
* Maintainer:  linxinping <linxinping@uniontech.com>
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

#include "DebugTimeManager.h"
#include <stub.h>
#include <gtest/gtest.h>


TEST(UT_DebugTimeManager_clear, UT_DebugTimeManager_clear_001)
{
    DebugTimeManager *Dtime = new DebugTimeManager();
    ASSERT_TRUE(Dtime);
    Dtime->clear();
    delete Dtime;
}
