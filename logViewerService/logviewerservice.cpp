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

#include "logviewerservice.h"

#include <QCoreApplication>
#include <QDebug>
#include <QStringList>
#include <QDBusConnection>
#include <QDBusMessage>
#include <QDBusConnectionInterface>

const QStringList ValidInvokerExePathList1 = {"/usr/bin/deepin-log-viewer"};

LogViewerService::LogViewerService(QObject *parent)
    : QObject(parent)
{
    m_commands.insert("dmesg", "dmesg -r");
    m_commands.insert("last", "last -x");
    m_commands.insert("journalctl_system", "journalctl -r");
    m_commands.insert("journalctl_boot", "journalctl -b -r");
}

/*!
 * \~chinese \brief LogViewerService::readLog 读取日志文件
 * \~chinese \param filePath 文件路径
 * \~chinese \return 读取的日志
 */
QString LogViewerService::readLog(const QString &filePath)
{
    //增加服务黑名单，只允许通过提权接口读取/var/log下，家目录下和临时目录下的文件
    if ((!filePath.startsWith("/var/log/") && !filePath.startsWith("/tmp") && !filePath.startsWith("/home")) || filePath.contains("..") || !isValidInvoker())  {
        return " ";
    }
    m_process.start("cat", QStringList() << filePath);
    m_process.waitForFinished(-1);
    QByteArray byte = m_process.readAllStandardOutput();
    return QString::fromUtf8(byte);
}

/*!
 * \~chinese \brief LogViewerService::exitCode 返回进程状态
 * \~chinese \return 进程返回值
 */
int LogViewerService::exitCode()
{
    return m_process.exitCode();
}

/*!
 * \~chinese \brief LogViewerService::quit 退出服务端程序
 */
void LogViewerService::quit()
{
    qDebug() << "LogViewService::Quit called";
    QCoreApplication::exit(0);
}

/*!
 * \~chinese \brief LogViewerService::getFileInfo 获取想到读取日志文件的路径
 * \~chinese \param file 日志文件的类型
 * \~chinese \return 所有日志文件路径列表
 */
QStringList LogViewerService::getFileInfo(const QString &file, bool unzip)
{
    int fileNum = 0;
    if (tmpDir.isValid()) {
        tmpDirPath = tmpDir.path();
    }
    QStringList fileNamePath;
    QString nameFilter;
    QDir dir;
    if (file.contains("deepin", Qt::CaseInsensitive) || file.contains("uos", Qt::CaseInsensitive)) {
        QFileInfo appFileInfo(file);
        if (!appFileInfo.isFile()) {
            return QStringList() << "";
        }
        QString appDir = appFileInfo.absolutePath();
        nameFilter = appDir.mid(appDir.lastIndexOf("/") + 1, appDir.size() - 1);
        dir.setPath(appDir);
    } else {
        dir.setPath("/var/log");
        nameFilter = file;
    }
    //要判断路径是否存在
    if (!dir.exists()) {
        qWarning() << "it is not true path";
        return QStringList() << "";
    }
    dir.setFilter(QDir::Files | QDir::NoSymLinks); //实现对文件的过滤
    dir.setNameFilters(QStringList() << nameFilter + ".*"); //设置过滤
    dir.setSorting(QDir::Time);
    QFileInfoList fileList = dir.entryInfoList();
    for (int i = 0; i < fileList.count(); i++) {
        if (QString::compare(fileList[i].suffix(), "gz", Qt::CaseInsensitive) != 0) {
            fileNamePath.append(fileList[i].absoluteFilePath());
        } else if (unzip) {
            //                qDebug() << tmpDirPath;
            QProcess m_process;

            QString command = "gunzip";
            QStringList args;
            args.append("-c");
            //                qDebug() << fileList[i].absoluteFilePath();
            args.append(fileList[i].absoluteFilePath());
            m_process.setStandardOutputFile(tmpDirPath + "/" + QString::number(fileNum) + ".txt");
            m_process.start(command, args);
            m_process.waitForFinished(-1);
            //                qDebug() << m_process.readAll();
            fileNamePath.append(tmpDirPath + "/" + QString::number(fileNum) + ".txt");
            fileNum++;
        }
    }
    //       qInfo()<<fileNamePath.count()<<fileNamePath<<"******************************";
    return fileNamePath;
}

bool LogViewerService::exportLog(const QString &outDir, const QString &in, bool isFile)
{
    if (outDir.isEmpty() || in.isEmpty()) {
        return false;
    }
    QString outFullPath = "";
    QStringList arg = {"-c", ""};
    if (isFile) {
        //增加服务黑名单，只允许通过提权接口读取/var/log下，家目录下和临时目录下的文件
        if ((!in.startsWith("/var/log/") && !in.startsWith("/tmp") && !in.startsWith("/home")) || in.contains("..")) {
            return false;
        }
        QFileInfo filein(in);
        if (!filein.isFile()) {
            qInfo() << "in not file:" << in;
            return false;
        }
        outFullPath = outDir + filein.fileName();
        //复制文件
        arg[1].append(QString("cp %1 %2;").arg(in, outDir));
    } else {
        auto it = m_commands.find(in);
        if (it == m_commands.end()) {
            qInfo() << "unknown command:" << in;
            return false;
        }
        outFullPath = outDir + in + ".txt";
        //结果重定向到文件
        arg[1].append(QString("%1 >& %2;").arg(it.value(), outFullPath));
    }
    //设置文件权限
    arg[1].append(QString("chmod 777 %1;").arg(outFullPath));
    QProcess process;
    process.start("/bin/bash", arg);
    if (!process.waitForFinished()) {
        qInfo() << "command error:" << arg;
        return false;
    }
    return true;
}

bool LogViewerService::isValidInvoker()
{
    bool valid = false;
    QDBusConnection conn = connection();
    QDBusMessage msg = message();

    //判断是否存在执行路径
    uint pid = conn.interface()->servicePid(msg.service()).value();
    QFileInfo f(QString("/proc/%1/exe").arg(pid));
    if (!f.exists()) {
        valid = false;
    } else {
        valid = true;
    }

    //是否存在于可调用者名单中
    QString invokerPath = f.canonicalFilePath();
    if (valid)
        valid = ValidInvokerExePathList1.contains(invokerPath);

    //非法调用
    if (!valid) {
        sendErrorReply(QDBusError::ErrorType::Failed,
                       QString("(pid: %1)[%2] is not allowed to configrate firewall")
                       .arg(pid)
                       .arg((invokerPath)));
        return false;
    }
    return true;
}
