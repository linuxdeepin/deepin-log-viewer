// SPDX-FileCopyrightText: 2019 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "logviewerservice.h"
#include "logviewerwatcher.h"

#include <DLog>

#include <QCoreApplication>
#include <QDBusConnection>
#include <QDBusError>
#include <QDebug>

//service name
const QString LogViewrServiceName = "com.deepin.logviewer";
//service path
const QString LogViewrPath = "/com/deepin/logviewer";

int main(int argc, char *argv[])
{
    //set env otherwise utils excutecmd  excute command failed
    QString PATH = qgetenv("PATH");

    if (PATH.isEmpty()) {
        PATH = "/usr/bin";
    }
    PATH += ":/usr/sbin";
    PATH += ":/sbin";
    qputenv("PATH", PATH.toLatin1());

    qDebug() << "log-viewer-service start, PATH" << PATH;

    QCoreApplication a(argc, argv);
    a.setOrganizationName("deepin");
    a.setApplicationName("deepin-log-viewer-service");
#if (DTK_VERSION >= DTK_VERSION_CHECK(5, 6, 8, 0))
    Dtk::Core::DLogManager::registerJournalAppender();
#ifdef QT_DEBUG
    Dtk::Core::DLogManager::registerConsoleAppender();
#endif
#else
    QDir dirCheck;
    QString LogPath = QString("%1/%2/%3/Log/")
                          .arg("/var/log")
                          .arg(qApp->organizationName())
                          .arg(qApp->applicationName());

    if (!dirCheck.exists(LogPath)) {
        dirCheck.mkpath(LogPath);
    }
    QString serviceLogPath=LogPath+QString("deepin-log-viewer-service.log");
    Dtk::Core::DLogManager::setlogFilePath(serviceLogPath);
    Dtk::Core::DLogManager::registerConsoleAppender();
    Dtk::Core::DLogManager::registerFileAppender();
#endif
    QDBusConnection systemBus = QDBusConnection::systemBus();
    if (!systemBus.registerService(LogViewrServiceName)) {
        qCritical() << "registerService failed:" << systemBus.lastError();
        exit(0x0001);
    }
    LogViewerWatcher watcher;
    LogViewerService service;
    if (!systemBus.registerObject(LogViewrPath,
                                  &service,
                                  QDBusConnection::ExportAllSlots | QDBusConnection::ExportAllSignals)) {
        qCritical() << "registerObject failed:" << systemBus.lastError();
        exit(0x0002);
    }
    return a.exec();
}
