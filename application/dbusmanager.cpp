// SPDX-FileCopyrightText: 2019 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "dbusmanager.h"
#include "utils.h"

#include <unistd.h>
#include <pwd.h>
#include <QDBusInterface>
#include <QDBusReply>
#include <QDebug>

bool DBusManager::isGetedKlu = false;
QString DBusManager::isklusystemName = "";
/**
 * @brief DBusManager::DBusManager
 * dbus接口获取工具类
 * @param parent
 */
DBusManager::DBusManager(QObject *parent) : QObject(parent)
{

}

/**
 * @brief DBusManager::getSystemInfo
 * 通过dBus接口获取系统是否为klu/pangu/panguV
 * @return 返回klu/pangu/panguV或者空,如果没有此dbus接口也返回空,返回空表示不是klu/pangu/panguV
 */
QString DBusManager::getSystemInfo()
{
    if (!isGetedKlu) {
        auto osVersion = Utils::osVersion();
        if(osVersion == "20") {
            isklusystemName =  QDBusInterface("com.deepin.system.SystemInfo", "/com/deepin/system/SystemInfo", "com.deepin.system.SystemInfo", QDBusConnection::systemBus())
                               .property("ProductName")
                               .toString();
        } else if (osVersion == "23") {
            isklusystemName =  QDBusInterface("org.deepin.dde.SystemInfo1", "/org/deepin/dde/SystemInfo1", "org.deepin.dde.SystemInfo1", QDBusConnection::systemBus())
                               .property("ProductName")
                               .toString();
        } else {
            qWarning() << "Unknown OS version:" << osVersion;
        }
        isGetedKlu = true;
    }
    return  isklusystemName;
}

bool DBusManager::isSEOepn()
{
    bool bIsSEOpen = false;
    QDBusInterface interfaceSE("com.deepin.daemon.SecurityEnhance", "/com/deepin/daemon/SecurityEnhance", "com.deepin.daemon.SecurityEnhance", QDBusConnection::systemBus());
    if (interfaceSE.isValid()) {
        QDBusReply<QString> reply = interfaceSE.call(QStringLiteral("Status"));
        if (!reply.error().message().isEmpty())
            qWarning() << qPrintable(QString("com.deepin.daemon.SecurityEnhance.Status DBus error: %1").arg(reply.error().message()));

        if (reply.value() == "close")
            bIsSEOpen = false;
        else
            bIsSEOpen = true;
    } else {
        qWarning() << qPrintable(QString("isSEOpen failed! interface error: %1").arg(interfaceSE.lastError().message()));
    }

    return bIsSEOpen;
}

bool DBusManager::isAuditAdmin()
{
    bool bIsAuditAdmin = false;

    // 获取当前系统用户名
    struct passwd* pwd = getpwuid(getuid());
    QString currentUserName = pwd->pw_name;

    // 根据用户名判断用户身份，查看是否为审计管理员
    QDBusInterface interfaceSE("com.deepin.daemon.SecurityEnhance", "/com/deepin/daemon/SecurityEnhance", "com.deepin.daemon.SecurityEnhance", QDBusConnection::systemBus());
    if (interfaceSE.isValid()) {
        QDBusReply<QString> reply = interfaceSE.call(QStringLiteral("GetSEUserByName"), currentUserName);
        if (!reply.error().message().isEmpty())
            qWarning() << qPrintable(QString("com.deepin.daemon.SecurityEnhance.GetSEUserByName DBus error: %1").arg(reply.error().message()));

        if (reply.value() == "audadm_u" || reply.value() == "auditadm_u")
            bIsAuditAdmin = true;
    } else {
        qWarning() << qPrintable(QString("isAuditAdmin failed! interface error: %1").arg(interfaceSE.lastError().message()));
    }

    return false;
}

bool DBusManager::isSpecialComType(){
    bool isSpecialComType = false;
    qInfo() << "Utils::specialComType value is:" << Utils::specialComType;
    //机器类型未知时，走以前判读机器的类型，有可能是通过DConfig获取失败，导致机器类型未知
    if(Utils::specialComType != -1){
        isSpecialComType = Utils::specialComType ? true:false;
    }else{
        QString systemName = getSystemInfo();
        if (systemName == "klu" ||
                systemName == "panguV" ||
                systemName == "W515 PGUV-WBY0" ||
                systemName == "pangu" ||
                systemName.toUpper().contains("PGUV") ||
                systemName.toUpper().contains("PANGUV") ||
                systemName.toUpper().contains("KLU") ||
                systemName.toUpper().contains("PANGU")) {
            isSpecialComType = true;
        }
    }
    qInfo() << "Is it a special computer type?" << Utils::specialComType;
    return isSpecialComType;
}
