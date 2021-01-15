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

LogViewerService::LogViewerService(QObject *parent)
    : QObject(parent)
{
}

/*!
 * \~chinese \brief LogViewerService::readLog 读取日志文件
 * \~chinese \param filePath 文件路径
 * \~chinese \return 读取的日志
 */
QString LogViewerService::readLog(const QString &filePath)
{
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
QStringList LogViewerService::getFileInfo(const QString &file)
{
    int fileNum = 0;
    if (tmpDir.isValid()) {
        tmpDirPath = tmpDir.path();
    }
    QStringList fileNamePath;
    QDir dir("/var/log");
    //要判断路径是否存在
    if (!dir.exists()) {
        qWarning() << "it is not true path";
    }
    dir.setFilter(QDir::Files | QDir::NoSymLinks); //实现对文件的过滤
    dir.setNameFilters(QStringList() << file + ".*"); //设置过滤
    dir.setSorting(QDir::Time);
    QFileInfoList fileList = dir.entryInfoList();
    for (int i = 0; i < fileList.count(); i++) {
        if (fileNum == 0) {
            if (QString::compare(fileList[i].absoluteFilePath(), "/var/log/" + file + ".log", Qt::CaseInsensitive) == 0) {
                fileNamePath.append(fileList[i].absoluteFilePath());
            }
        } else {
            if (QString::compare(fileList[i].absoluteFilePath(), "/var/log/" + file + ".log." + QString::number(fileNum), Qt::CaseInsensitive) == 0) {
                fileNamePath.append(fileList[i].absoluteFilePath());
            }
            if (QString::compare(fileList[i].suffix(), "gz", Qt::CaseInsensitive) == 0) {
                qDebug() << tmpDirPath;
                QProcess m_process;

                QString command = "gunzip";
                QStringList args;
                args.append("-c");
                qDebug() << fileList[i].absoluteFilePath();
                args.append(fileList[i].absoluteFilePath());
                m_process.setStandardOutputFile(tmpDirPath + "/" + QString::number(fileNum) + ".txt");
                m_process.start(command, args);
                m_process.waitForFinished(-1);
                qDebug() << m_process.readAll();
                fileNamePath.append(tmpDirPath + "/" + QString::number(fileNum) + ".txt");
            }
        }
        fileNum++;
    }
    //       qInfo()<<fileNamePath.count()<<fileNamePath<<"******************************";
    return fileNamePath;
}
