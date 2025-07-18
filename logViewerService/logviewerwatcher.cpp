// SPDX-FileCopyrightText: 2019 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "logviewerwatcher.h"

#include <QProcess>
#include <QCoreApplication>
#include <QDebug>
#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(logService)

LogViewerWatcher::LogViewerWatcher():m_Timer(new QTimer (this))
{
    qCDebug(logService) << "LogViewerWatcher constructor called";
    connect(m_Timer,&QTimer::timeout,this,&LogViewerWatcher::onTimeOut);
    m_Timer->start(1000);
    qCDebug(logService) << "Timer started with 1000ms interval";
}

/**
 * @brief 定时监控客户端
 */
void LogViewerWatcher::onTimeOut()
{
    qCDebug(logService) << "Timer timeout, checking for deepin-log-viewer process";
    QString cmd, outPut;
    QStringList args;
    //判断deepin-log-viewer客户端是否存在，如果不存在退出服务。
    cmd = "ps";
    args << "aux";
    outPut= executCmd(cmd, args);
    bool bHasLogViewer = false;
    QStringList rows = outPut.split('\n');
    for (auto line : rows) {
        QStringList items = line.split(' ');
        if (items.contains("deepin-log-viewer")) {
            bHasLogViewer = true;
            break;
        }
    }
    if (!bHasLogViewer) {
        qCDebug(logService) << "deepin-log-viewer process not found, exiting service";
        QCoreApplication::exit(0);
    } else {
        qCDebug(logService) << "deepin-log-viewer process found, continuing service";
    }
}

/**
 * @brief 执行外部命令
 * @param strCmd:外部命令字符串
 */
QString LogViewerWatcher::executCmd(const QString &strCmd, const QStringList &args)
{
     qCDebug(logService) << "Executing command:" << strCmd << "with args:" << args;
     QProcess proc;
     proc.start(strCmd, args);
     proc.waitForFinished(-1);
     QString output = proc.readAllStandardOutput();
     qCDebug(logService) << "Command execution completed";
     return output;
}
