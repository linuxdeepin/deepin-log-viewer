// SPDX-FileCopyrightText: 2019 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

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
