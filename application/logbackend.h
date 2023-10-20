// SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef LOGBACKEND_H
#define LOGBACKEND_H

#include "structdef.h"

#include <QObject>

class LogFileParser;
class LogBackend : public QObject
{
    Q_OBJECT
public:
    static LogBackend *instance(QObject *parent = nullptr);
    ~LogBackend();
    explicit LogBackend(QObject *parent = nullptr);

    // 设置命令行当前工作目录
    void setCmdWorkDir(const QString &dirPath);

    // 导出全部日志到指定目录
    void exportAllLogs(const QString &outDir = "");

    // 按类型导出日志
    void exportTypeLogs(const QString &outDir, const QString &type = "");

    // 按应用导出日志
    void exportLogsByApp(const QString &outDir, const QString &appName = "");

    // 获取当前系统支持的日志种类
    QStringList getLogTypes();

    QString getOutDirPath() const;
    void resetToNormalAuth(const QString &path);

private:
    static LogBackend *m_staticbackend;

private:
    bool getOutDirPath(const QString &path);
    LOG_FLAG type2Flag(const QString &type, QString &error);

private:
    QStringList m_logTypes;
    bool m_newDir {false};
    QString m_outPath {""};
    QString m_cmdWorkDir {""};

    //当前解析的日志类型
    LOG_FLAG m_flag {NONE};
};

#endif // LOGBACKEND_H
