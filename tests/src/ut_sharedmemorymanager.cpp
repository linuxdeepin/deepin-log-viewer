// SPDX-FileCopyrightText: 2019 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "sharedmemorymanager.h"

#include <gtest/gtest.h>
#include <stub.h>

#include <QDebug>
#include <QSharedMemory>

#include "addr_pri.h"

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

// 覆盖静态单例获取接口 getInstance()：与 instance() 保持一致（顺序无关）
TEST(SharedMemoryManager_getInstance_UT, SharedMemoryManager_getInstance_UT)
{
    SharedMemoryManager *before = SharedMemoryManager::getInstance();
    SharedMemoryManager *inst = SharedMemoryManager::instance();
    EXPECT_NE(inst, nullptr);
    EXPECT_EQ(SharedMemoryManager::getInstance(), inst);
    // 首次调用前为 nullptr，已初始化后为同一实例
    EXPECT_TRUE(before == nullptr || before == inst);
}

// 覆盖静态单例获取接口 instance()：双检锁创建并复用同一实例
TEST(SharedMemoryManager_instance_UT, SharedMemoryManager_instance_UT)
{
    SharedMemoryManager *p1 = SharedMemoryManager::instance();
    EXPECT_NE(p1, nullptr);
    SharedMemoryManager *p2 = SharedMemoryManager::instance();
    EXPECT_EQ(p1, p2);
    EXPECT_EQ(SharedMemoryManager::getInstance(), p1);
}

// 覆盖 releaseMemory()：m_commondM 非空且已 attached 时执行 detach
TEST(SharedMemoryManager_releaseMemory_UT, SharedMemoryManager_releaseMemory_attached)
{
    SharedMemoryManager *p = new SharedMemoryManager(nullptr);
    EXPECT_NE(p, nullptr);
    p->releaseMemory();
    p->deleteLater();
}

// 覆盖 releaseMemory()：m_commondM 为空时走告警分支
ACCESS_PRIVATE_FIELD(SharedMemoryManager, QSharedMemory *, m_commondM)
TEST(SharedMemoryManager_releaseMemory_UT, SharedMemoryManager_releaseMemory_null)
{
    SharedMemoryManager *p = new SharedMemoryManager(nullptr);
    EXPECT_NE(p, nullptr);
    QSharedMemory *&mem = access_private_field::SharedMemoryManagerm_commondM(*p);
    QSharedMemory *saved = mem;
    mem = nullptr;
    p->releaseMemory();
    mem = saved;
    p->deleteLater();
}
