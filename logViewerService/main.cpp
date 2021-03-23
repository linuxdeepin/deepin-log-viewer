/*
* Copyright (C) 2019 ~ 2020 UnionTech Software Technology Co.,Ltd
*
* Author:     linxun <linxun@uniontech.com>
* Maintainer:  linxun <linxun@uniontech.com>
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "logviewerservice.h"
#include <QCoreApplication>
#include <DLog>
#include <QDBusConnection>
#include <QDBusError>

//service name
const QString LogViewrServiceName = "com.deepin.logviewer";
//service path
const QString LogViewrPath = "/com/deepin/logviewer";
//service log  path
const QString LogservicePath = "/var/log";

int main(int argc, char *argv[])
{
    //set env otherwise utils excutecmd  excute command failed
    QString PATH = qgetenv("PATH");

    qDebug() << "main start" << PATH;
    if (PATH.isEmpty()) {
        PATH = "/usr/bin";
    }
    PATH += ":/usr/sbin";
    PATH += ":/sbin";
    qputenv("PATH", PATH.toLatin1());

    QCoreApplication a(argc, argv);
    a.setOrganizationName("deepin");
    a.setApplicationName("deepin-logviewr-service");

    Dtk::Core::DLogManager::setlogFilePath(LogservicePath);
    Dtk::Core::DLogManager::registerConsoleAppender();
    Dtk::Core::DLogManager::registerFileAppender();

    qDebug() << "write log to" << Dtk::Core::DLogManager::getlogFilePath();
    QDBusConnection systemBus = QDBusConnection::systemBus();
    if (!systemBus.registerService(LogViewrServiceName)) {
        qCritical() << "registerService failed:" << systemBus.lastError();
        exit(0x0001);
    }
    LogViewerService service;
    service.getFileInfo("lpr");
    qDebug() << "systemBus.registerService success" << Dtk::Core::DLogManager::getlogFilePath();
    if (!systemBus.registerObject(LogViewrPath,
                                  &service,
                                  QDBusConnection::ExportAllSlots | QDBusConnection::ExportAllSignals)) {
        qCritical() << "registerObject failed:" << systemBus.lastError();
        exit(0x0002);
    }
    return a.exec();
}
