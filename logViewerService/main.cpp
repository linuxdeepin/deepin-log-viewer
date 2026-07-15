// SPDX-FileCopyrightText: 2019 - 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "logviewerservice.h"

#include <DLog>

#include <QCoreApplication>
#include <QDBusConnection>
#include <QDBusError>
#include <QDebug>
#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(logService)

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

    qCDebug(logService) << "log-viewer-service start, PATH" << PATH;

    QCoreApplication a(argc, argv);
    a.setOrganizationName("deepin");
    a.setApplicationName("deepin-log-viewer-service");
//#if (DTK_VERSION >= DTK_VERSION_CHECK(5, 6, 8, 0))
#if 0
    Dtk::Core::DLogManager::registerJournalAppender();
#ifdef QT_DEBUG
    Dtk::Core::DLogManager::registerConsoleAppender();
#endif
#else
    QDir dirCheck;
    QString LogPath = QString("%1/%2/%3/")
                          .arg("/var/log")
                          .arg(qApp->organizationName())
                          .arg(qApp->applicationName());

    if (!dirCheck.exists(LogPath)) {
        dirCheck.mkpath(LogPath);
    }
    QString serviceLogPath=LogPath+QString("%1.log").arg(qApp->applicationName());
    Dtk::Core::DLogManager::setlogFilePath(serviceLogPath);
    Dtk::Core::DLogManager::registerConsoleAppender();
    Dtk::Core::DLogManager::registerFileAppender();
#endif
    QDBusConnection systemBus = QDBusConnection::systemBus();
    // 先注册对象再注册服务名：well-known name 一旦对外可见，/com/deepin/logviewer
    // 对象必然已就绪，消除名称已注册但对象尚未注册的竞态窗口
    // （表现为 busctl tree 间歇性 "No such object path '/'"）。
    LogViewerService service;
    if (!systemBus.registerObject(LogViewrPath,
                                  &service,
                                  QDBusConnection::ExportAllSlots | QDBusConnection::ExportAllSignals)) {
        qCCritical(logService) << "registerObject failed:" << systemBus.lastError();
        exit(0x0002);
    }
    if (!systemBus.registerService(LogViewrServiceName)) {
        qCCritical(logService) << "registerService failed:" << systemBus.lastError();
        exit(0x0001);
    }
    return a.exec();
}
