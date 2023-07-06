// SPDX-FileCopyrightText: 2019 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "dbusmanager.h"
#include "utils.h"

#include <QDBusInterface>
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
