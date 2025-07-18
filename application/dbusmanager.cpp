// SPDX-FileCopyrightText: 2019 - 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "dbusmanager.h"
#include "utils.h"

#include <unistd.h>
#include <pwd.h>
#include <QDBusInterface>
#include <QDBusReply>
#include <QDebug>
#include <QLoggingCategory>
#include <QProcess>

Q_DECLARE_LOGGING_CATEGORY(logApp)

/**
 * @brief DBusManager::DBusManager
 * dbus接口获取工具类
 * @param parent
 */
DBusManager::DBusManager(QObject *parent) : QObject(parent)
{
    qCDebug(logApp) << "DBusManager initialized";
}

bool DBusManager::isSEOpen()
{
    qCDebug(logApp) << "Checking SecurityEnhance status";
    bool bIsSEOpen = false;
    QDBusInterface interfaceSE("com.deepin.daemon.SecurityEnhance", "/com/deepin/daemon/SecurityEnhance", "com.deepin.daemon.SecurityEnhance", QDBusConnection::systemBus());
    if (interfaceSE.isValid()) {
        qCInfo(logApp) << "SecurityEnhance interface is valid, calling Status method";
        QDBusReply<QString> reply = interfaceSE.call(QStringLiteral("Status"));
        if (!reply.error().message().isEmpty()) {
            qCWarning(logApp) << qPrintable(QString("com.deepin.daemon.SecurityEnhance.Status DBus error: %1").arg(reply.error().message()));
        }

        if (reply.value() == "close") {
            qCInfo(logApp) << "SecurityEnhance status: closed";
            bIsSEOpen = false;
        } else {
            qCInfo(logApp) << "SecurityEnhance status: open";
            bIsSEOpen = true;
        }
    } else {
        qCWarning(logApp) << qPrintable(QString("isSEOpen failed! interface error: %1").arg(interfaceSE.lastError().message()));
    }

    qCDebug(logApp) << "SecurityEnhance status check completed, returning:" << bIsSEOpen;
    return bIsSEOpen;
}

bool DBusManager::isAuditAdmin()
{
    qCDebug(logApp) << "Checking audit admin status";
    bool bIsAuditAdmin = false;

    // 获取当前系统用户名
    struct passwd* pwd = getpwuid(getuid());
    QString currentUserName = pwd->pw_name;
    qCInfo(logApp) << "Current user name:" << currentUserName;

    // 根据用户名判断用户身份，查看是否为审计管理员
    QDBusInterface interfaceSE("com.deepin.daemon.SecurityEnhance", "/com/deepin/daemon/SecurityEnhance", "com.deepin.daemon.SecurityEnhance", QDBusConnection::systemBus());
    if (interfaceSE.isValid()) {
        qCInfo(logApp) << "SecurityEnhance interface is valid, calling GetSEUserByName method";
        QDBusReply<QString> reply = interfaceSE.call(QStringLiteral("GetSEUserByName"), currentUserName);
        if (!reply.error().message().isEmpty()) {
            qCWarning(logApp) << qPrintable(QString("com.deepin.daemon.SecurityEnhance.GetSEUserByName DBus error: %1").arg(reply.error().message()));
        }

        if (reply.value() == "audadm_u" || reply.value() == "auditadm_u") {
            qCInfo(logApp) << "User is audit admin";
            bIsAuditAdmin = true;
        } else {
            qCInfo(logApp) << "User is not audit admin";
        }
    } else {
        qCWarning(logApp) << qPrintable(QString("isAuditAdmin failed! interface error: %1").arg(interfaceSE.lastError().message()));
    }

    qCDebug(logApp) << "Audit admin check completed, returning:" << bIsAuditAdmin;
    return bIsAuditAdmin;
}

QString DBusManager::getHomePathByFreeDesktop()
{
    qCDebug(logApp) << "Getting home path via FreeDesktop";
    QString homePath;
    QDBusInterface interface("org.freedesktop.login1", "/org/freedesktop/login1/user/self", "org.freedesktop.login1.User", QDBusConnection::systemBus());
    QString userName = qvariant_cast<QString>(interface.property("Name"));
    qCInfo(logApp) << "Got user name from FreeDesktop:" << userName;

    if (!userName.isEmpty()) {
        homePath = "/home/" + userName;
        qCInfo(logApp) << "Home path determined:" << homePath;
    } else {
        qCWarning(logApp) << "Empty user name received from FreeDesktop";
    }

    qCDebug(logApp) << "Home path lookup completed, returning:" << homePath;
    return homePath;
}
