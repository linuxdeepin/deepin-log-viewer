// SPDX-FileCopyrightText: 2025 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "logexportinterface.h"

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
    Q_UNUSED(homeDir)

    std::string cmd = ("dmidecode -t 0 >> " + outDir + "/system_info.txt").toStdString();
    system(cmd.c_str());
    cmd = ("dmidecode -t 11 >> " + outDir + "/system_info.txt").toStdString();
    system(cmd.c_str());
    cmd = ("cat /etc/hw_version >> " + outDir + "/system_info.txt").toStdString();
    system(cmd.c_str());
    cmd = ("echo >> " + outDir + "/system_info.txt").toStdString();
    system(cmd.c_str());
    cmd = ("/usr/sbin/hwfirmware -v >> " + outDir + "/system_info.txt").toStdString();
    system(cmd.c_str());
    cmd = ("echo >> " + outDir + "/system_info.txt").toStdString();
    system(cmd.c_str());
    cmd = ("lscpu >> " + outDir + "/system_info.txt").toStdString();
    system(cmd.c_str());

    // 递归设置目录及文件的权限
    cmd = ("chmod -R 777 " + outDir).toStdString();
    system(cmd.c_str());

    QTimer::singleShot(1000, this, [=](){
        qApp->exit(0);
    });

    return true;
}
