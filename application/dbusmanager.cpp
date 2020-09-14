#include "dbusmanager.h"
#include <QDBusInterface>
#include <QDebug>
bool DBusManager::isGetedKlu = false;
QString DBusManager::isklusystemName = "";

DBusManager::DBusManager(QObject *parent) : QObject(parent)
{

}

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
