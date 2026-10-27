// SPDX-FileCopyrightText: 2019 - 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include <gtest/gtest.h>
#include "../application/dbusmanager.h"

#include <stub.h>

#include <QString>

// 1) 构造函数
TEST(DBusManager_Constructor_UT, DBusManager_Constructor_UT_001)
{
    DBusManager *p = new DBusManager(nullptr);
    EXPECT_NE(p, nullptr);
    delete p;
}

TEST(DBusManager_Constructor_UT, DBusManager_Constructor_UT_002)
{
    QObject parent;
    DBusManager *p = new DBusManager(&parent);
    EXPECT_NE(p, nullptr);
    EXPECT_EQ(p->parent(), &parent);
    // parent 接管删除，不再手动 delete
}

// 2) isSEOpen —— 无 DBus 服务环境下接口不可用，应返回 false
TEST(DBusManager_isSEOpen_UT, DBusManager_isSEOpen_UT_001)
{
    bool rs = DBusManager::isSEOpen();
    EXPECT_EQ(rs, false);
}

TEST(DBusManager_isSEOpen_UT, DBusManager_isSEOpen_UT_002)
{
    DBusManager *p = new DBusManager(nullptr);
    bool rs = p->isSEOpen();
    EXPECT_FALSE(rs);
    delete p;
}

// 3) isAuditAdmin —— 无 DBus 服务环境下接口不可用，应返回 false
TEST(DBusManager_isAuditAdmin_UT, DBusManager_isAuditAdmin_UT_001)
{
    bool rs = DBusManager::isAuditAdmin();
    EXPECT_EQ(rs, false);
}

TEST(DBusManager_isAuditAdmin_UT, DBusManager_isAuditAdmin_UT_002)
{
    DBusManager *p = new DBusManager(nullptr);
    bool rs = p->isAuditAdmin();
    EXPECT_FALSE(rs);
    delete p;
}

// 4) getHomePathByFreeDesktop —— 无 DBus 属性时返回空串
TEST(DBusManager_getHomePathByFreeDesktop_UT, DBusManager_getHomePathByFreeDesktop_UT_001)
{
    QString home = DBusManager::getHomePathByFreeDesktop();
    EXPECT_TRUE(home.isEmpty());
}

TEST(DBusManager_getHomePathByFreeDesktop_UT, DBusManager_getHomePathByFreeDesktop_UT_002)
{
    DBusManager *p = new DBusManager(nullptr);
    QString home = p->getHomePathByFreeDesktop();
    EXPECT_EQ(home, QString());
    delete p;
}

// 5) isScreenLocked —— 锁屏服务不可用时视为未锁屏，应返回 false
TEST(DBusManager_isScreenLocked_UT, DBusManager_isScreenLocked_UT_001)
{
    bool rs = DBusManager::isScreenLocked();
    EXPECT_EQ(rs, false);
}

TEST(DBusManager_isScreenLocked_UT, DBusManager_isScreenLocked_UT_002)
{
    DBusManager *p = new DBusManager(nullptr);
    bool rs = p->isScreenLocked();
    EXPECT_FALSE(rs);
    delete p;
}
