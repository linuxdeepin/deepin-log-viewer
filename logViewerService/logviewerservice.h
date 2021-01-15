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
#ifndef LOGVIEWERSERVICE_H
#define LOGVIEWERSERVICE_H

#include <QObject>
#include <QDBusContext>
#include <QScopedPointer>
#include <QProcess>
#include <QTemporaryDir>

class LogViewerService : public QObject
    , protected QDBusContext
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "com.deepin.logviewer")
public:
    explicit LogViewerService(QObject *parent = nullptr);

Q_SIGNALS:

public Q_SLOTS:
    Q_SCRIPTABLE QString readLog(const QString &filePath);
    Q_SCRIPTABLE int exitCode();
    Q_SCRIPTABLE void quit();
    Q_SCRIPTABLE QStringList getFileInfo(const QString &file);

private:
    QTemporaryDir tmpDir;
    QProcess m_process;
    QString tmpDirPath;
};

#endif // LOGVIEWERSERVICE_H
