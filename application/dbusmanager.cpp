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

#ifdef QT_DEBUG
Q_LOGGING_CATEGORY(logDBusManager, "org.deepin.log.viewer.dbus.manager")
#else
Q_LOGGING_CATEGORY(logDBusManager, "org.deepin.log.viewer.dbus.manager", QtInfoMsg)
#endif

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
            qCWarning(logDBusManager) << "Unknown OS version:" << osVersion;
        }
        isGetedKlu = true;
    }
    return  isklusystemName;
}

//根据命令行 dmidecode -s system-product-name|awk '{print SNF}' 返回的结果判断是否是华为电脑
bool DBusManager::isHuaWei()
{
    QStringList options;
    options << QString(QStringLiteral("-c"));
    options << QString(QStringLiteral("dmidecode -s system-product-name|awk '{print $NF}'"));
    QProcess process;
    process.start(QString(QStringLiteral("bash")), options);
    process.waitForFinished();
    process.waitForReadyRead();
    QByteArray tempArray =  process.readAllStandardOutput();
    char *charTemp = tempArray.data();
    QString str_output = QString(QLatin1String(charTemp));
    process.close();
    //qInfo() << "system-product-name: " << str_output;
    options.clear();
    options << QString(QStringLiteral("-c"));
    options << QString(QStringLiteral("dmidecode | grep -i \"String 4\""));
    process.start(QString(QStringLiteral("bash")), options);
    process.waitForFinished();
    process.waitForReadyRead();
    QString str_output1 = QString(QLatin1String(process.readAllStandardOutput().data()));
    //qInfo() << "dmidecode | grep -i \"String 4\": " << str_output1;
    if (str_output.contains("KLVV", Qt::CaseInsensitive) ||
            str_output.contains("KLVU", Qt::CaseInsensitive) ||
            str_output.contains("PGUV", Qt::CaseInsensitive) ||
            str_output.contains("PGUW", Qt::CaseInsensitive) ||
            str_output1.contains("PWC30", Qt::CaseInsensitive) ||
            str_output.contains("L540", Qt::CaseInsensitive) ||
            str_output.contains("W585", Qt::CaseInsensitive) ||
            isPangu())
        return true;
    return false;
}

bool DBusManager::isPangu()
{
    QDBusInterface systemInfoInterface("com.deepin.daemon.SystemInfo",
                                       "/com/deepin/daemon/SystemInfo",
                                       "org.freedesktop.DBus.Properties",
                                       QDBusConnection::sessionBus());
    QDBusMessage replyCpu = systemInfoInterface.call("Get", "com.deepin.daemon.SystemInfo", "CPUHardware");
    QList<QVariant> outArgsCPU = replyCpu.arguments();
    if (outArgsCPU.count()) {
        QString CPUHardware = outArgsCPU.at(0).value<QDBusVariant>().variant().toString();
        if (CPUHardware.contains("PANGU")) {
            return true;
        }
    }
    return false;
}

bool DBusManager::isSEOepn()
{
    bool bIsSEOpen = false;
    QDBusInterface interfaceSE("com.deepin.daemon.SecurityEnhance", "/com/deepin/daemon/SecurityEnhance", "com.deepin.daemon.SecurityEnhance", QDBusConnection::systemBus());
    if (interfaceSE.isValid()) {
        QDBusReply<QString> reply = interfaceSE.call(QStringLiteral("Status"));
        if (!reply.error().message().isEmpty())
            qCWarning(logDBusManager) << qPrintable(QString("com.deepin.daemon.SecurityEnhance.Status DBus error: %1").arg(reply.error().message()));

        if (reply.value() == "close")
            bIsSEOpen = false;
        else
            bIsSEOpen = true;
    } else {
        qCWarning(logDBusManager) << qPrintable(QString("isSEOpen failed! interface error: %1").arg(interfaceSE.lastError().message()));
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
            qCWarning(logDBusManager) << qPrintable(QString("com.deepin.daemon.SecurityEnhance.GetSEUserByName DBus error: %1").arg(reply.error().message()));

        if (reply.value() == "audadm_u" || reply.value() == "auditadm_u")
            bIsAuditAdmin = true;
    } else {
        qCWarning(logDBusManager) << qPrintable(QString("isAuditAdmin failed! interface error: %1").arg(interfaceSE.lastError().message()));
    }

    return bIsAuditAdmin;
}

bool DBusManager::isSpecialComType(){
    bool isSpecialComType = false;
    //机器类型未知时，走以前判读机器的类型，有可能是通过DConfig获取失败，导致机器类型未知
    if(Utils::specialComType != -1){
        isSpecialComType = Utils::specialComType ? true:false;
    }else{
        QString systemName = getSystemInfo();
        if (!systemName.isEmpty()) {
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

        // 上一dbus服务可能在新版本已失效，使用dmidecode命令再判断一次
        if (!isSpecialComType)
            isSpecialComType = isHuaWei();
    }
    return isSpecialComType;
}
