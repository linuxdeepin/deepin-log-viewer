/*
 * Copyright (C) 2019 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     LZ <zhou.lu@archermind.com>
 *
 * Maintainer: LZ <zhou.lu@archermind.com>
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

#ifndef JOURNALWORK_H
#define JOURNALWORK_H

#include <QMap>
#include <QObject>
#include <QProcess>
#include "structdef.h"

#include <QThread>

// class journalWork : public QObject
class journalWork : public QThread
{
    Q_OBJECT
public:
    explicit journalWork(QStringList arg, QObject *parent = nullptr);
    ~journalWork();

    void stopWork();

    void run();

signals:
    void journalFinished(QList<LOG_MSG_JOURNAL> list);

public slots:
    void doWork();

private:
    QString getDateTimeFromStamp(QString str);
    void initMap();
    QString i2str(int prio);

    QStringList m_arg;
    QList<LOG_MSG_JOURNAL> logList;
    QMap<int, QString> m_map;

    QProcess *proc {nullptr};
};

#endif  // JOURNALWORK_H
