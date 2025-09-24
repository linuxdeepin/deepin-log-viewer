// SPDX-FileCopyrightText: 2025 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef LOG_EXPORT_INTERFACE_H
#define LOG_EXPORT_INTERFACE_H

#include <QObject>
#include <QDBusContext>

class LogExportInterface : public QObject, protected QDBusContext
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.deepin.logviewer.service.opsLogExport")

public:
    explicit LogExportInterface(QObject *parent = Q_NULLPTR);

public Q_SLOTS:
    bool ExportOpsLog(const QString &outDir, const QString &homeDir);
};

#endif
