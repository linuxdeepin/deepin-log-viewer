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
#include <mutex>
#include "structdef.h"
#include <systemd/sd-journal.h>
#include <QMap>
#include <QObject>
#include <QProcess>
#include <QRunnable>
#include <QMutexLocker>
#include <QEventLoop>
#include <QMutex>
// class journalWork : public QObject
class journalWork :  public QObject, public QRunnable
{
    Q_OBJECT

public:
    explicit journalWork(QStringList arg, QObject *parent = nullptr);
    explicit journalWork(QObject *parent = nullptr);
    ~journalWork();

    static journalWork *instance()
    {
        journalWork *sin = m_instance.load();
        if (!sin) {
            std::lock_guard<std::mutex> lock(m_mutex);
            sin = m_instance.load();
            if (!sin) {
                sin = new journalWork();
                m_instance.store(sin);
            }
        }
        return sin;
    }



    void setArg(QStringList arg);
    void deleteSd();
    void run() override;

signals:
    void journalData(int aaaindex, QList<LOG_MSG_JOURNAL> list);
    void journalFinished();

public slots:
    void doWork();
    QString getReplaceColorStr(const char *d);
    void stopWork();
    int getIndex();
    static int getPublicIndex();
public:
    QList<LOG_MSG_JOURNAL> logList;
    QMutex mutex;
    static int thread_index ;
private:
    QString getDateTimeFromStamp(QString str);
    void initMap();
    QString i2str(int prio);

    QStringList m_arg;
    QMap<int, QString> m_map;
    // sd_journal *j {nullptr};
    QProcess *proc {nullptr};
    static std::atomic<journalWork *> m_instance;
    static std::mutex m_mutex;
    QEventLoop loop;
    bool m_canRun = false;
    int m_threadIndex;


};

#endif  // JOURNALWORK_H
