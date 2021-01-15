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
#ifndef DLDBUSHANDLER_H
#define DLDBUSHANDLER_H

#include "dldbusinterface.h"
#include <QObject>

class DLDBusHandler : public QObject
{
    Q_OBJECT
public:
    static DLDBusHandler *instance(QObject *parent = nullptr);
    ~DLDBusHandler();
    QString readLog(const QString &filePath);
    QStringList getFileInfo(const QString &flag);
    int exitCode();
    void quit();

private:
    explicit DLDBusHandler(QObject *parent = nullptr);

private:
    static DLDBusHandler *m_statichandeler;
    DeepinLogviewerInterface *m_dbus;
    QStringList filePath;
};

#endif // DLDBUSHANDLER_H
