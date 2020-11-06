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
#include "sharedmemorymanager.h"

#include <gtest/gtest.h>
#include <gtest/src/stub.h>

#include <QDebug>

TEST(SharedMemoryManager_Constructor_UT, SharedMemoryManager_Constructor_UT)
{
    SharedMemoryManager *p = new SharedMemoryManager(nullptr);
    EXPECT_NE(p, nullptr);
    p->deleteLater();
}

TEST(SharedMemoryManager_setRunnableTag_UT, SharedMemoryManager_setRunnableTag_UT)
{
    SharedMemoryManager *p = new SharedMemoryManager(nullptr);
    EXPECT_NE(p, nullptr);
    ShareMemoryInfo info;
    info.isStart = true;
    p->setRunnableTag(info);
    EXPECT_EQ(p->m_pShareMemoryInfo->isStart, true);
    p->deleteLater();
}


TEST(SharedMemoryManager_getRunnableKey_UT, SharedMemoryManager_getRunnableKey_UT)
{
    SharedMemoryManager *p = new SharedMemoryManager(nullptr);
    EXPECT_NE(p, nullptr);
    p->getRunnableKey();
    p->deleteLater();
}

TEST(SharedMemoryManager_isAttached_UT, SharedMemoryManager_isAttached_UT)
{
    SharedMemoryManager *p = new SharedMemoryManager(nullptr);
    EXPECT_NE(p, nullptr);
    p->isAttached();
    p->deleteLater();
}

TEST(SharedMemoryManager_init_UT, SharedMemoryManager_init_UT)
{
    SharedMemoryManager *p = new SharedMemoryManager(nullptr);
    EXPECT_NE(p, nullptr);
    p->init();
    p->deleteLater();
}

