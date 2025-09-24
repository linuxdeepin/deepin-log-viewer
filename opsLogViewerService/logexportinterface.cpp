// SPDX-FileCopyrightText: 2025 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "logexportinterface.h"
#include "opslogexport.h"

#include <QDBusConnection>
#include <QDebug>
#include <QTimer>
#include <QCoreApplication>

constexpr const char* kLogExportService = "org.deepin.logviewer.service.opsLogExport";
constexpr const char* kLogExportPath = "/org/deepin/logviewer/service/opsLogExport";

LogExportInterface::LogExportInterface(QObject *parent)
    : QObject(parent)
{
    QDBusConnection systemBus = QDBusConnection::systemBus();
    if (!systemBus.registerService(kLogExportService)) {
        qCritical() << "registerService failed:" << systemBus.lastError();
        exit(0x0001);
    }
    if (!systemBus.registerObject(kLogExportPath,
                                  this,
                                  QDBusConnection::ExportAllSlots | QDBusConnection::ExportAllSignals)) {
        qCritical() << "registerObject failed:" << systemBus.lastError();
        exit(0x0002);
    }
}

bool LogExportInterface::ExportOpsLog(const QString &outDir, const QString &homeDir)
{
    OpsLogExport ops(outDir.toStdString(), homeDir.toStdString());
    ops.run();

    QTimer::singleShot(1000, this, [=](){
        qApp->exit(0);
    });

    return true;
}
