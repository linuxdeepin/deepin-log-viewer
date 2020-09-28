/*
* Copyright (C) 2019 ~ 2020 UnionTech Software Technology Co.,Ltd
*
* Author:     zyc <zyc@uniontech.com>
* Maintainer:  zyc <zyc@uniontech.com>
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* any later version.
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef DBUSMANAGER_H
#define DBUSMANAGER_H

#include <QObject>
#include <QVariantMap>
/**
 * @brief The DBusManager class  dbus接口获取工具类
 */
class DBusManager : public QObject
{
    Q_OBJECT
public:
    explicit DBusManager(QObject *parent = nullptr);
    static QString getSystemInfo();
    static bool isGetedKlu ;
    static QString isklusystemName ;

signals:

public slots:
};

#endif // DBUSMANAGER_H
