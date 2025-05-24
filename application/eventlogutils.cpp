// SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "eventlogutils.h"
#include <QLibrary>
#include <QDir>
#include <QLibraryInfo>
#include <QJsonDocument>
#include <QThread>
#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(logEventUtils)
Q_LOGGING_CATEGORY(logEventUtils, "deepin.log.viewer.eventutils")

Eventlogutils *Eventlogutils::m_pInstance = nullptr;
Eventlogutils *Eventlogutils::GetInstance()
{
    if (m_pInstance == nullptr) {
        qCDebug(logEventUtils) << "Creating Eventlogutils singleton instance";
        m_pInstance  = new Eventlogutils();
    }
    return m_pInstance;
}

void Eventlogutils::writeLogs(QJsonObject &data)
{
    if (!writeEventLogFunc) {
        qCWarning(logEventUtils) << "writeEventLogFunc is null, skip writing log";
        return;
    }

    writeEventLogFunc(QJsonDocument(data).toJson(QJsonDocument::Compact).toStdString());
}

Eventlogutils::Eventlogutils()
{
    QLibrary library("libdeepin-event-log.so");
    initFunc = reinterpret_cast<bool (*)(const std::string &, bool)>(library.resolve("Initialize"));
    writeEventLogFunc = reinterpret_cast<void (*)(const std::string &)>(library.resolve("WriteEventLog"));

    qCDebug(logEventUtils) << "Loading libdeepin-event-log.so functions:"
                         << "initFunc:" << (initFunc ? "found" : "not found")
                         << "writeEventLogFunc:" << (writeEventLogFunc ? "found" : "not found");

    if (!initFunc) {
        qCWarning(logEventUtils) << "Failed to resolve Initialize function";
        return;
    }

    qCDebug(logEventUtils) << "Initializing event log with appName: deepin-log-viewer";
    initFunc("deepin-log-viewer", true);
}
