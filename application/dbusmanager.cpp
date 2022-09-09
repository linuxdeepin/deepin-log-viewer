// SPDX-FileCopyrightText: 2019 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "dbusmanager.h"

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
        isklusystemName =  QDBusInterface("com.deepin.system.SystemInfo", "/com/deepin/system/SystemInfo", "com.deepin.system.SystemInfo", QDBusConnection::systemBus())
                           .property("ProductName")
                           .toString();
        isGetedKlu = true;
    }
    return  isklusystemName;
}
