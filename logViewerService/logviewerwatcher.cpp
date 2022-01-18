/*
* Copyright (C) 2019 ~ 2021 Uniontech Software Technology Co.,Ltd.
*
* Author:     linxun <linxun@uniontech.com>
*
* Maintainer: linxun <linxun@uniontech.com>
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "logviewerwatcher.h"

#include <QProcess>
#include <QCoreApplication>
#include <QDebug>

LogViewerWatcher::LogViewerWatcher():m_Timer(new QTimer (this))
{

    connect(m_Timer,&QTimer::timeout,this,&LogViewerWatcher::onTimeOut);
    m_Timer->start(1000);
}

/**
 * @brief 定时监控客户端
 */
void LogViewerWatcher::onTimeOut()
{
    QString cmd, outPut;
    //判断deepin-log-viewer客户端是否存在，如果不存在退出服务。
    cmd = QString("ps aux | grep -w deepin-log-viewer$");
    outPut= executCmd(cmd);
    int ret = outPut.length();
    if (!ret)
        QCoreApplication::exit(0);
}

/**
 * @brief 执行外部命令
 * @param strCmd:外部命令字符串
 */
QString LogViewerWatcher::executCmd(const QString &strCmd)
{
     QProcess proc;
     proc.start("bash", QStringList() << "-c" << strCmd);
     proc.waitForFinished(-1);
     return  proc.readAllStandardOutput();
}
