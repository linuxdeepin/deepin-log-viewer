// SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef LOGBACKEND_H
#define LOGBACKEND_H

#include <QObject>

class LogBackend : public QObject
{
    Q_OBJECT
public:
    static LogBackend *instance(QObject *parent = nullptr);
    ~LogBackend();
    explicit LogBackend(QObject *parent = nullptr);

    // 导出全部日志到指定目录
    void exportAllLogs(const QString& outDir = "");

    // 获取当前系统支持的日志种类
    QStringList getLogTypes();

private:
    static LogBackend *m_staticbackend;

private:
    QStringList m_logTypes;
    bool m_newDir {false};
    QString m_outPath {""};
};

#endif // LOGBACKEND_H
