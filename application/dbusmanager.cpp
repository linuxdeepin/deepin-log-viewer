#include "dbusmanager.h"
#include <QDBusInterface>
#include <QDebug>
DBusManager::DBusManager(QObject *parent) : QObject(parent)
{

}

QString DBusManager::getSystemInfo()
{
    QString systemName =  QDBusInterface("com.deepin.system.SystemInfo", "/com/deepin/system/SystemInfo", "com.deepin.system.SystemInfo")
                          .property("ProductName")
                          .toString();
    return  systemName;
}
