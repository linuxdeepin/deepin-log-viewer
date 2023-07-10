// SPDX-FileCopyrightText: 2019 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef LOGVIEWERSERVICE_H
#define LOGVIEWERSERVICE_H

#include <QObject>
#include <QDBusContext>
#include <QScopedPointer>
#include <QProcess>
#include <QTemporaryDir>

class QTextStream;

class LogViewerService : public QObject
    , protected QDBusContext
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "com.deepin.logviewer")
public:
    explicit LogViewerService(QObject *parent = nullptr);
    ~LogViewerService();

Q_SIGNALS:

public Q_SLOTS:
    Q_SCRIPTABLE QString readLog(const QString &filePath);
    Q_SCRIPTABLE int exitCode();
    Q_SCRIPTABLE void quit();
    Q_SCRIPTABLE QStringList getFileInfo(const QString &file, bool unzip = true);
    Q_SCRIPTABLE QStringList getOtherFileInfo(const QString &file, bool unzip = true);
    Q_SCRIPTABLE bool exportLog(const QString &outDir, const QString &in, bool isFile);
    Q_SCRIPTABLE QString openLogStream(const QString &filePath);
    Q_SCRIPTABLE QString readLogInStream(const QString &token);
    Q_SCRIPTABLE bool isFileExist(const QString &filePath);
    Q_SCRIPTABLE quint64 getFileSize(const QString &filePath);

private:
    QTemporaryDir tmpDir;
    QProcess m_process;
    QString tmpDirPath;
    QMap<QString, QString> m_commands;
    QMap<QString, std::pair<QString, QTextStream*>> m_logMap;
    /**
     * @brief isValidInvoker 检验调研者是否是日志
     * @return
     */
    bool isValidInvoker();
};

#endif // LOGVIEWERSERVICE_H
