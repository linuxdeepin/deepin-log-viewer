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

#include "journalwork.h"
#include <DApplication>
#include <QDateTime>
#include <QDebug>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QProcess>

DWIDGET_USE_NAMESPACE

std::atomic<journalWork *> journalWork::m_instance;
std::mutex journalWork::m_mutex;

journalWork::journalWork(QStringList arg, QObject *parent)
    //    : QObject(parent)
    : QThread(parent)
{
    qRegisterMetaType<QList<LOG_MSG_JOURNAL> >("QList<LOG_MSG_JOURNAL>");

    initMap();

    m_arg.append("-o");
    m_arg.append("json");
    if (!arg.isEmpty())
        m_arg.append(arg);
}

journalWork::journalWork(QObject *parent)
    : QThread(parent)
{
    qRegisterMetaType<QList<LOG_MSG_JOURNAL> >("QList<LOG_MSG_JOURNAL>");

    initMap();
}

journalWork::~journalWork()
{
    logList.clear();
    m_map.clear();
}

void journalWork::stopWork()
{
    if (proc)
        proc->kill();
}

void journalWork::setArg(QStringList arg)
{
    m_arg.clear();

    //    m_arg.append("-o");
    //    m_arg.append("json");

    if (!arg.isEmpty())
        m_arg.append(arg);
}

void journalWork::run()
{
    doWork();
}

#include <systemd/sd-journal.h>
void journalWork::doWork()
{
    logList.clear();
#if 1
    int r;
    sd_journal *j;
    r = sd_journal_open(&j, SD_JOURNAL_LOCAL_ONLY);
    if (r < 0) {
        fprintf(stderr, "Failed to open journal: %s\n", strerror(-r));
        return;
    }

    sd_journal_seek_tail(j);

    //    sd_journal_add_match(j, "PRIORITY=3", 0);

    if (!m_arg.isEmpty()) {
        QString _priority = m_arg.at(0);
        if (_priority != "all")
            sd_journal_add_match(j, m_arg.at(0).toStdString().c_str(), 0);
    }

    int cnt = 0;
    SD_JOURNAL_FOREACH_BACKWARDS(j)
    {
        const char *d;
        size_t l;

        LOG_MSG_JOURNAL logMsg;

        //        r = sd_journal_get_data(j, "SYSLOG_TIMESTAMP", (const void **)&d, &l);
        //        if (r < 0) {
        r = sd_journal_get_data(j, "_SOURCE_REALTIME_TIMESTAMP", (const void **)&d, &l);
        if (r < 0) {
            r = sd_journal_get_data(j, "__REALTIME_TIMESTAMP", (const void **)&d, &l);
            if (r < 0) {
                continue;
            }
            //            }
        }
        uint64_t t;
        sd_journal_get_realtime_usec(j, &t);

        QString dt = QString(d).split("=")[1];
        if (m_arg.size() == 2) {
            if (t < m_arg.at(1).toLongLong())
                continue;
        }
        logMsg.dateTime = getDateTimeFromStamp(dt);

        r = sd_journal_get_data(j, "_HOSTNAME", (const void **)&d, &l);
        if (r < 0)
            logMsg.hostName = "";
        else {
            logMsg.hostName = QString(d).split("=")[1];
        }

        r = sd_journal_get_data(j, "_COMM", (const void **)&d, &l);
        if (r < 0)
            logMsg.daemonName = "";
        else {
            logMsg.daemonName = QString(d).split("=")[1];
        }

        r = sd_journal_get_data(j, "_PID", (const void **)&d, &l);
        if (r < 0)
            logMsg.daemonId = "";
        else {
            logMsg.daemonId = QString(d).split("=")[1];
        }

        r = sd_journal_get_data(j, "MESSAGE", (const void **)&d, &l);
        if (r < 0) {
            logMsg.msg = "";
        } else {
            logMsg.msg = QString(d).split("=")[1];
        }

        r = sd_journal_get_data(j, "PRIORITY", (const void **)&d, &l);
        if (r < 0) {
            logMsg.level = "";
        } else {
            logMsg.level = i2str(QString(d).split("=")[1].toInt());
        }

        cnt++;
        mutex.lock();
        logList.append(logMsg);
        mutex.unlock();

        if (cnt % 500 == 0) {
            mutex.lock();
            emit journalFinished();
            usleep(100);
        }
    }
    sd_journal_close(j);

    if (logList.count() >= 0)
        emit journalFinished();

        //    emit journalFinished(logList);

#else
    proc = new QProcess;
    //! by time: --since="xxxx-xx-xx" --until="xxxx-xx-xx" exclude U
    //! by priority: journalctl PRIORITY=x
    proc->start("journalctl", m_arg);
    proc->waitForFinished(-1);

    QByteArray output = proc->readAllStandardOutput();
    proc->close();

    // reverse by time
    QList<QByteArray> arrayList = output.split('\n');
    for (auto i = arrayList.count() - 1; i >= 0; i--) {
        QByteArray data = arrayList.at(i);
        //    for (QByteArray data : output.split('\n')) {
        if (data.isEmpty())
            continue;

        LOG_MSG_JOURNAL logMsg;

        cnt++;

        QJsonParseError erro;
        QJsonDocument jsonDoc(QJsonDocument::fromJson(data, &erro));

        if (erro.error != QJsonParseError::NoError) {
            qDebug() << "erro" << erro.error << erro.errorString();
            continue;
        }
        QJsonObject jsonObj = jsonDoc.object();
        // fill field
        QString dt = jsonObj.value("_SOURCE_REALTIME_TIMESTAMP").toString();
        if (dt.isEmpty())
            dt = jsonObj.value("__REALTIME_TIMESTAMP").toString();
        logMsg.dateTime = getDateTimeFromStamp(dt);
        logMsg.hostName = jsonObj.value("_HOSTNAME").toString();
        logMsg.daemonName = jsonObj.value("_COMM").toString();
        logMsg.daemonId = jsonObj.value("_PID").toString();
        logMsg.msg = jsonObj.value("MESSAGE").toString();
        logMsg.level = i2str(jsonObj.value("PRIORITY").toString().toInt());
        logList.append(logMsg);
        if (cnt == 500)
            break;
    }

    emit journalFinished(logList);
#endif
}

QString journalWork::getDateTimeFromStamp(QString str)
{
    QString ret = "";
    QString ums = str.right(6);
    QString dtstr = str.left(str.length() - 6);
    QDateTime dt = QDateTime::fromTime_t(dtstr.toUInt());
    ret = dt.toString("yyyy-MM-dd hh:mm:ss");  // + QString(".%1").arg(ums);
    return ret;
}

void journalWork::initMap()
{
    m_map.clear();
    m_map.insert(0, DApplication::translate("Level", "Emergency"));
    m_map.insert(1, DApplication::translate("Level", "Alert"));
    m_map.insert(2, DApplication::translate("Level", "Critical"));
    m_map.insert(3, DApplication::translate("Level", "Error"));
    m_map.insert(4, DApplication::translate("Level", "Warning"));
    m_map.insert(5, DApplication::translate("Level", "Notice"));
    m_map.insert(6, DApplication::translate("Level", "Info"));
    m_map.insert(7, DApplication::translate("Level", "Debug"));
}

QString journalWork::i2str(int prio)
{
    return m_map.value(prio);
}
