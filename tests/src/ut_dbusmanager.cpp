// SPDX-FileCopyrightText: 2019 - 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include <gtest/gtest.h>
#include <QDBusConnection>
#include <QDBusInterface>
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

// 4) getHomePathByFreeDesktop —— 结果取决于 login1 是否可用：
//    有系统总线时返回 "/home/<user>"，无则返回空串；断言与环境解耦
TEST(DBusManager_getHomePathByFreeDesktop_UT, DBusManager_getHomePathByFreeDesktop_UT_001)
{
    QString home = DBusManager::getHomePathByFreeDesktop();
    if (!home.isEmpty())
        EXPECT_TRUE(home.startsWith("/home/")) << "home=" << home.toStdString();
}

TEST(DBusManager_getHomePathByFreeDesktop_UT, DBusManager_getHomePathByFreeDesktop_UT_002)
{
    DBusManager *p = new DBusManager(nullptr);
    QString home = p->getHomePathByFreeDesktop();
    if (!home.isEmpty())
        EXPECT_TRUE(home.startsWith("/home/")) << "home=" << home.toStdString();
    delete p;
}

// 5) isScreenLocked —— 返回值取决于会话总线上的锁屏服务：
//    服务不存在时恒为 false；服务在运行时返回真实 Visible 状态
//    （无人值守的桌面会话可能正处锁屏，为 true），断言与环境解耦
static bool lockFrontVisibleForAssertion()
{
    const char *services[][3] = {
        {"org.deepin.dde.LockFront1", "/org/deepin/dde/LockFront1", "org.deepin.dde.LockFront1"},
        {"com.deepin.dde.lockFront", "/com/deepin/dde/lockFront", "com.deepin.dde.lockFront"},
    };
    for (const auto &svc : services) {
        QDBusInterface lockFront(svc[0], svc[1], svc[2], QDBusConnection::sessionBus());
        if (!lockFront.isValid())
            continue;
        QVariant visible = lockFront.property("Visible");
        if (visible.isValid())
            return visible.toBool();
    }
    return false;
}

TEST(DBusManager_isScreenLocked_UT, DBusManager_isScreenLocked_UT_001)
{
    EXPECT_EQ(DBusManager::isScreenLocked(), lockFrontVisibleForAssertion());
}

TEST(DBusManager_isScreenLocked_UT, DBusManager_isScreenLocked_UT_002)
{
    DBusManager *p = new DBusManager(nullptr);
    EXPECT_EQ(p->isScreenLocked(), lockFrontVisibleForAssertion());
    delete p;
}
