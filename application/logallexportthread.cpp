/*
* Copyright (C) 2021 ~ 2022 Uniontech Software Technology Co.,Ltd.
*
* Author:     liuyanga <liuyanga@uniontech.com>
*
* Maintainer: liuyanga <liuyanga@uniontech.com>
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
#include "logallexportthread.h"
#include "dbusproxy/dldbushandler.h"
#include "logapplicationhelper.h"
#include "utils.h"
LogAllExportThread::LogAllExportThread(const QStringList &types, const QString &outfile, QObject *parent)
    : QObject(parent)
    , m_types(types)
    , m_outfile(outfile)
{
}

void LogAllExportThread::run()
{
    //判断权限
    QFileInfo info(m_outfile);
    if (!QFileInfo(info.path()).isWritable()) {
        emit exportFinsh(false);
        return;
    }

    QStringList files;
    QStringList commands;

    if (files.isEmpty() && commands.isEmpty()) {
        //获取所有文件
        for (auto &it : m_types) {
            if (it.contains(JOUR_TREE_DATA, Qt::CaseInsensitive)) {
                commands.push_back("journalctl_system");
            } else if (it.contains(BOOT_KLU_TREE_DATA, Qt::CaseInsensitive)) {
                commands.push_back("journalctl_boot");
            } else if (it.contains(DMESG_TREE_DATA, Qt::CaseInsensitive)) {
                commands.push_back("dmesg");
            } else if (it.contains(LAST_TREE_DATA, Qt::CaseInsensitive)) {
                commands.push_back("last");
            } else if (it.contains(DPKG_TREE_DATA, Qt::CaseInsensitive)) {
                files.append(DLDBusHandler::instance(nullptr)->getFileInfo("dpkg", false));
            } else if (it.contains(KERN_TREE_DATA, Qt::CaseInsensitive)) {
                files.append(DLDBusHandler::instance(nullptr)->getFileInfo("kern", false));
            } else if (it.contains(XORG_TREE_DATA, Qt::CaseInsensitive)) {
                files.append(DLDBusHandler::instance(nullptr)->getFileInfo("Xorg", false));
            } else if (it.contains(DNF_TREE_DATA, Qt::CaseInsensitive)) {
                files.append(DLDBusHandler::instance(nullptr)->getFileInfo("dnf", false));
            } else if (it.contains(BOOT_TREE_DATA, Qt::CaseInsensitive)) {
                files.append(DLDBusHandler::instance(nullptr)->getFileInfo("boot", false));
            } else if (it.contains(KWIN_TREE_DATA, Qt::CaseInsensitive)) {
                files.append(KWIN_TREE_DATA);
            } else if (it.contains(APP_TREE_DATA, Qt::CaseInsensitive)) {
                QMap<QString, QString> appData = LogApplicationHelper::instance()->getMap();
                for (auto &it : appData.toStdMap()) {
                    files.append(it.second);
                }
            }
            //取消导出直接返回
            if (m_cancel) {
                files.clear();
                commands.clear();
                emit exportFinsh(false);
                return;
            }
        }
    }

    if (files.isEmpty() && commands.isEmpty()) {
        emit exportFinsh(false);
        return;
    }

    //删除原文件
    if (info.exists() && !QFile::remove(m_outfile)) {
        emit exportFinsh(false);
        return;
    }

    int tolProcess = files.size() + commands.size() + 10;
    int currentProcess = 1;
    emit updateTolProcess(tolProcess);
    QString tmpPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/tmp/";
    QDir dir(tmpPath);
    //删除临时目录
    dir.removeRecursively();
    //创建临时目录
    Utils::mkMutiDir(tmpPath);
    //复制文件
    for (auto &it : files) {
        DLDBusHandler::instance(this)->exportLog(tmpPath, it, true);
        emit updatecurrentProcess(currentProcess++);
        if (m_cancel) {
            break;
        }
    }
    if (false == m_cancel) {
        for (auto &it : commands) {
            DLDBusHandler::instance(this)->exportLog(tmpPath, it, false);
            emit updatecurrentProcess(currentProcess++);
            if (m_cancel) {
                break;
            }
        }
    }
    if (false == m_cancel) {
        //打包日志文件
        QProcess procss;
        procss.setWorkingDirectory(tmpPath);
        QStringList arg = {"-c"};
        arg.append(QString("zip tmp.zip *.*;mv tmp.zip '%1'").arg(m_outfile));
        procss.start("/bin/bash", arg);
        procss.waitForFinished(-1);
        currentProcess += 9;
        emit updatecurrentProcess(currentProcess);
    }
    //删除临时目录
    dir.removeRecursively();
    //取消导出删除输出文件
    if (m_cancel) {
        QFile::remove(m_outfile);
    }
    emit exportFinsh(!m_cancel && QFileInfo(m_outfile).exists());
}
