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

#include "journalbootwork.h"
#include "utils.h"
#include <DApplication>
#include <QDateTime>
#include <QDebug>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QProcess>

DWIDGET_USE_NAMESPACE

std::atomic<JournalBootWork *> JournalBootWork::m_instance;
std::mutex JournalBootWork::m_mutex;

JournalBootWork::JournalBootWork(QStringList arg, QObject *parent)
    :  QObject(parent),
       QRunnable()
{
    qRegisterMetaType<QList<LOG_MSG_JOURNAL> >("QList<LOG_MSG_JOURNAL>");

    initMap();
    setAutoDelete(true);
    m_arg.append("-o");
    m_arg.append("json");
    if (!arg.isEmpty())
        m_arg.append(arg);
    thread_index++;
    m_threadIndex = thread_index;
}

JournalBootWork::JournalBootWork(QObject *parent)
    :  QObject(parent),
       QRunnable()
{
    qRegisterMetaType<QList<LOG_MSG_JOURNAL> >("QList<LOG_MSG_JOURNAL>");
    initMap();
    setAutoDelete(true);
    thread_index++;
    m_threadIndex = thread_index;
}

JournalBootWork::~JournalBootWork()
{
    logList.clear();
    m_map.clear();
}

void JournalBootWork::stopWork()
{
//    if (proc)
//        proc->kill();
    m_canRun = false;
    qDebug() << "stopWorkb";
    // deleteSd();
}

int JournalBootWork::getIndex()
{
    return m_threadIndex;
}

int JournalBootWork::getPublicIndex()
{
    return thread_index;
}

void JournalBootWork::setArg(QStringList arg)
{
    m_arg.clear();

    //    m_arg.append("-o");
    //    m_arg.append("json");

    if (!arg.isEmpty())
        m_arg.append(arg);
}



void JournalBootWork::deleteSd()
{

//    qDebug() << j;
//    if (j) {
//        sd_journal_close(j);
//        j = nullptr;
//    }
}

void JournalBootWork::run()
{
    doWork();

}



void JournalBootWork::doWork()
{
    m_canRun = true;
    mutex.lock();
    logList.clear();
    mutex.unlock();
    if ((!m_canRun)) {
        mutex.unlock();
        deleteSd();
        return;
    }
#if 1
    int r;
    sd_journal *j ;
    if ((!m_canRun)) {
        mutex.unlock();
        deleteSd();
        return;
    }
    r = sd_journal_open(&j, SD_JOURNAL_LOCAL_ONLY);
    if ((!m_canRun)) {
        mutex.unlock();
        sd_journal_close(j);
        deleteSd();
        return;
    }
    if (r < 0) {
        QString errostr = QString("Failed to open journal: %1").arg(r);
        qDebug() << errostr;
        emit  journalBootError(errostr);
        return;
    }
    r = sd_journal_seek_tail(j);

    if (r < 0) {
        QString errostr = QString("Failed to seek tail journal: %1").arg(r);
        qDebug() << errostr;
        emit  journalBootError(errostr);
        return;
    }
    if ((!m_canRun)) {
        mutex.unlock();
        sd_journal_close(j);
        deleteSd();
        return;
    }
    //    sd_journal_add_match(j, "PRIORITY=3", 0);

    if (!m_arg.isEmpty()) {
        QString _priority = m_arg.at(0);
        if (_priority != "all")
            r = sd_journal_add_match(j, m_arg.at(0).toStdString().c_str(), 0);
        if (r < 0) {
            QString errostr = QString("Failed to add match journal: %1").arg(r);
            qDebug() << errostr;
            emit  journalBootError(errostr);
            return;
        }
    }
    if ((!m_canRun)) {
        mutex.unlock();
        sd_journal_close(j);
        deleteSd();
        return;
    }
    //sd_id128_t boot_id = SD_ID128_NULL;

    char match[9 + 32 + 1] = "_BOOT_ID=";
    if ((!m_canRun)) {
        mutex.unlock();
        sd_journal_close(j);
        deleteSd();
        return;
    }
    sd_id128_t current_id;
    //获取当前最新的正在运行的bootid
    sd_id128_get_boot(&current_id);
    if ((!m_canRun)) {
        mutex.unlock();
        sd_journal_close(j);
        deleteSd();
        return;
    }
    //拼接和把id转成字符串
    sd_id128_to_string(current_id, match + 9);
    if ((!m_canRun)) {
        mutex.unlock();
        sd_journal_close(j);
        deleteSd();
        return;
    }
    qDebug() << "match" << match;
    // r = sd_journal_add_match(j, "_BOOT_ID=56896a70f6164e7bb858f52ef72fce30", 0);
    //增加筛选条件
    r = sd_journal_add_match(j, match, sizeof(match) - 1);
    if (r < 0) {
        QString errostr = QString("Failed to add match journal: %1").arg(r);
        qDebug() << errostr;
        emit  journalBootError(errostr);
        return;
    }
    if ((!m_canRun)) {
        mutex.unlock();
        sd_journal_close(j);
        deleteSd();
        return;
    }
    //合并以上两个筛选条件 (等级和bootid)
    r =   sd_journal_add_conjunction(j);
    if (r < 0) {
        QString errostr = QString("Failed to add conjunction journal: %1").arg(r);
        qDebug() << errostr;
        emit  journalBootError(errostr);
        return;
    }
    if ((!m_canRun)) {
        mutex.unlock();
        sd_journal_close(j);
        deleteSd();
        return;
    }
    int cnt = 0;
    SD_JOURNAL_FOREACH_BACKWARDS(j) {
        if ((!m_canRun)) {
            mutex.unlock();
            sd_journal_close(j);
            deleteSd();
            return;
        }
        const char *d;
        size_t l;

        LOG_MSG_JOURNAL logMsg;

        //        r = sd_journal_get_data(j, "SYSLOG_TIMESTAMP", (const void **)&d, &l);
        //        if (r < 0) {
        r = sd_journal_get_data(j, "_SOURCE_REALTIME_TIMESTAMP", reinterpret_cast<const void **>(&d), &l);
        if (r < 0) {
            r = sd_journal_get_data(j, "__REALTIME_TIMESTAMP", reinterpret_cast<const void **>(&d), &l);
            if (r < 0) {
                continue;
            }
            //            }
        }
        uint64_t t;
        sd_journal_get_realtime_usec(j, &t);
        //解锁返回字符串长度上限，默认是64k，写0为无限
        // sd_journal_set_data_threshold(j, 0);
        QString dt = getReplaceColorStr(d).split("=").value(1);
        if (m_arg.size() == 2) {
            if (t < static_cast<uint64_t>(m_arg.at(1).toLongLong()))
                continue;
        }
        logMsg.dateTime = getDateTimeFromStamp(dt);

        r = sd_journal_get_data(j, "_HOSTNAME", reinterpret_cast<const void **>(&d), &l);
        if (r < 0)
            logMsg.hostName = "";
        else {
            logMsg.hostName = getReplaceColorStr(d).split("=").value(1);
        }

        r = sd_journal_get_data(j, "_PID", reinterpret_cast<const void **>(&d), &l);
        if (r < 0)
            logMsg.daemonId = "";
        else {
            logMsg.daemonId = getReplaceColorStr(d).split("=").value(1);
        }

        r = sd_journal_get_data(j, "_COMM", reinterpret_cast<const void **>(&d), &l);
        if (r < 0) {
            logMsg.daemonName = "unknown";
            qDebug() << logMsg.daemonId << "error code" << r;
        } else {
            logMsg.daemonName = getReplaceColorStr(d).split("=").value(1);
        }


        r = sd_journal_get_data(j, "MESSAGE", reinterpret_cast<const void **>(&d), &l);
        if (r < 0) {
            logMsg.msg = "";
        } else {
            logMsg.msg = getReplaceColorStr(d).split("=").value(1);
        }

        r = sd_journal_get_data(j, "PRIORITY", reinterpret_cast<const void **>(&d), &l);
        if (r < 0) {
            logMsg.level = "";
        } else {
            logMsg.level = i2str(getReplaceColorStr(d).split("=").value(1).toInt());
        }

        cnt++;
        mutex.lock();
        logList.append(logMsg);
        mutex.unlock();

        if (cnt % 500 == 0) {
            mutex.lock();
            emit journaBootlData(m_threadIndex, logList);
            logList.clear();
            //sleep(100);
            mutex.unlock();;
        }
        //  delete d;
    }
    if (logList.count() >= 0) {
        emit journaBootlData(m_threadIndex, logList);
    }

    emit journalBootFinished();
    //第一次加载时这个之后的代码都不执行?故放到最后
    deleteSd();
    sd_journal_close(j);
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

QString JournalBootWork::getReplaceColorStr(const char *d)
{
    QByteArray byteChar(d);
    byteChar = Utils::replaceEmptyByteArray(byteChar);
    QString d_str = QString(byteChar);
    d_str.replace(QRegExp("\\x1B\\[\\d+(;\\d+){0,2}m"), "");
    d_str.replace(QRegExp("\\002"), "");
    return  d_str;
}



QString JournalBootWork::getDateTimeFromStamp(QString str)
{
    QString ret = "";
    QString ums = str.right(6);
    QString dtstr = str.left(str.length() - 6);
    QDateTime dt = QDateTime::fromTime_t(dtstr.toUInt());
    ret = dt.toString("yyyy-MM-dd hh:mm:ss");  // + QString(".%1").arg(ums);
    return ret;
}

void JournalBootWork::initMap()
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

QString JournalBootWork::i2str(int prio)
{
    return m_map.value(prio);
}
