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

#ifdef QT_DEBUG
Q_LOGGING_CATEGORY(logEventUtils, "org.deepin.log.viewer.eventutils")
#else
Q_LOGGING_CATEGORY(logEventUtils, "org.deepin.log.viewer.eventutils", QtInfoMsg)
#endif

Eventlogutils *Eventlogutils::m_pInstance = nullptr;
Eventlogutils *Eventlogutils::GetInstance()
{
    if (m_pInstance == nullptr) {
        m_pInstance  = new Eventlogutils();
    }
    return m_pInstance;
}

void Eventlogutils::writeLogs(QJsonObject &data)
{
    if (!writeEventLogFunc) {
        qCWarning(logEventUtils) << "writeEventLogFunc is null, skip writing event log";
        return;
    }

    writeEventLogFunc(QJsonDocument(data).toJson(QJsonDocument::Compact).toStdString());
}

Eventlogutils::Eventlogutils()
    : m_available(false)
{
    QLibrary library("libdeepin-event-log.so");
    initFunc = reinterpret_cast<bool (*)(const std::string &, bool)>(library.resolve("Initialize"));
    writeEventLogFunc = reinterpret_cast<void (*)(const std::string &)>(library.resolve("WriteEventLog"));

    if (!initFunc) {
        qCWarning(logEventUtils) << "Failed to resolve Initialize from libdeepin-event-log.so;"
                                 << "crash report upload disabled, watermark will not advance until the backend is available";
        return;
    }

    if (!writeEventLogFunc) {
        qCWarning(logEventUtils) << "Failed to resolve WriteEventLog from libdeepin-event-log.so;"
                                 << "crash report upload disabled, watermark will not advance until the backend is available";
    }

    // Initialize 仍按原行为调用；后端可写性由 m_available 反映，供上报链路门禁使用。
    // 注意 WriteEventLog 签名为 void，无法获知上传是否真正成功，因此 m_available 仅表示
    // “后端已加载且初始化完成”，覆盖 .so 缺失/符号缺失/Initialize 失败三类不可用情形。
    m_available = initFunc("deepin-log-viewer", true) && writeEventLogFunc;
}

bool Eventlogutils::isAvailable()
{
    return m_available;
}
