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

#include "logfileparser.h"
#include "journalwork.h"
#include "utils.h"
#include <DMessageBox>
#include <QDateTime>
#include <QDebug>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QMessageBox>
#include <QProcess>


DWIDGET_USE_NAMESPACE

LogFileParser::LogFileParser(QWidget *parent)
    : QObject(parent)
{
    m_dateDict.clear();
    m_dateDict.insert("Jan", "1月");
    m_dateDict.insert("Feb", "2月");
    m_dateDict.insert("Mar", "3月");
    m_dateDict.insert("Apr", "4月");
    m_dateDict.insert("May", "5月");
    m_dateDict.insert("Jun", "6月");
    m_dateDict.insert("Jul", "7月");
    m_dateDict.insert("Aug", "8月");
    m_dateDict.insert("Sep", "9月");
    m_dateDict.insert("Oct", "10月");
    m_dateDict.insert("Nov", "11月");
    m_dateDict.insert("Dec", "12月");

    // TODO::
    m_levelDict.insert("Warning", WARN);
    m_levelDict.insert("Debug", DEB);
}

LogFileParser::~LogFileParser() {}

void LogFileParser::parseByJournal(QStringList arg)
{
    if (m_isJournalLoading) {
        return;
    }
    m_isJournalLoading = true;
    work = journalWork::instance();
    if (work->isRunning())
        work->terminate();
    //    work = new journalWork();
    disconnect(work, SIGNAL(journalFinished()), this, SLOT(slot_journalFinished()));
    work->setArg(arg);
    connect(work, SIGNAL(journalFinished()), this, SLOT(slot_journalFinished()),
            Qt::QueuedConnection);
    work->start();
}

void LogFileParser::parseByDpkg(QList<LOG_MSG_DPKG> &dList, qint64 ms)
{
    if (m_isDpkgLoading) {
        return;
    }
    m_isDpkgLoading = true;
    QFile file("/var/log/dpkg.log");  // if not,maybe crash
    if (!file.exists())
        return;

    if (!m_pDkpgDataLoader) {
        m_pDkpgDataLoader = new QProcess(this);
    }
    m_pDkpgDataLoader->start("cat /var/log/dpkg.log");  // file path is fixed. so write cmd direct
    m_pDkpgDataLoader->waitForFinished(-1);
    QByteArray outByte = m_pDkpgDataLoader->readAllStandardOutput();
    QString output = Utils::replaceEmptyByteArray(outByte);
    m_pDkpgDataLoader->close();

    for (QString str : output.split('\n')) {
        str.replace(QRegExp("\\x1B\\[\\d+(;\\d+){0,2}m"), "");
        QStringList strList = str.split(" ", QString::SkipEmptyParts);
        if (strList.size() < 3)
            continue;

        QString info;
        for (auto i = 3; i < strList.size(); i++) {
            info = info + strList[i] + " ";
        }

        LOG_MSG_DPKG dpkgLog;
        dpkgLog.dateTime = strList[0] + " " + strList[1];
        QDateTime dt = QDateTime::fromString(dpkgLog.dateTime, "yyyy-MM-dd hh:mm:ss");
        if (dt.toMSecsSinceEpoch() < ms)
            continue;
        dpkgLog.action = strList[2];
        dpkgLog.msg = info;

        //        dList.append(dpkgLog);
        dList.insert(0, dpkgLog);
    }

    createFile(output, dList.count());
    m_isDpkgLoading = false;
    emit dpkgFinished();

}

void LogFileParser::parseByXlog(QList<LOG_MSG_XORG> &xList, qint64 ms)  // modifed by Airy
{
    if (m_isXlogLoading) {
        return;
    }
    m_isXlogLoading = true;
    QFile file("/var/log/Xorg.0.log");  // if not,maybe crash
    if (!file.exists())
        return;

    if (!m_pXlogDataLoader) {
        m_pXlogDataLoader = new QProcess(this);
    }
    m_pXlogDataLoader->start("cat /var/log/Xorg.0.log");  // file path is fixed. so write cmd direct
    m_pXlogDataLoader->waitForFinished(-1);
    if (isErroCommand(QString(m_pXlogDataLoader->readAllStandardError())))
        return;
    QByteArray outByte = m_pXlogDataLoader->readAllStandardOutput();
    QString output = Utils::replaceEmptyByteArray(outByte);
    m_pXlogDataLoader->close();
    QDateTime curDt = QDateTime::currentDateTime();
    qint64 curDtSecond = curDt.toMSecsSinceEpoch();
    for (QString str : output.split('\n')) {
        str.replace(QRegExp("\\x1B\\[\\d+(;\\d+){0,2}m"), "");

        //        if (str.startsWith("[")) {
        //            //            xList.append(str);
        //            xList.insert(0, str);
        //        } else {
        //            str += " ";
        //            //            xList[xList.size() - 1] += str;
        //            xList[0] += str;
        //        }
        if (str.startsWith("[")) {
            QStringList list = str.split("]", QString::SkipEmptyParts);
            if (list.count() != 2)
                continue;

            QString timeStr = list[0];
            QString msgInfo = list[1];

            // get time
            QString tStr = timeStr.split("[", QString::SkipEmptyParts)[0].trimmed();
            qint64 realT = curDtSecond + qint64(tStr.toDouble() * 1000);
            //   qint64 realT =  qint64(tStr.toDouble() * 1000);
            QDateTime realDt = QDateTime::fromMSecsSinceEpoch(realT);
            if (realDt.toMSecsSinceEpoch() < ms)  // add by Airy
                continue;

            LOG_MSG_XORG msg;
            msg.dateTime = realDt.toString("yyyy-MM-dd hh:mm:ss.zzz");
            msg.msg = msgInfo;

            xList.insert(0, msg);
        } else {
            xList[0].msg += str;
        }
    }
    createFile(output, xList.count());
    m_isXlogLoading = false;
    emit xlogFinished();

}

// add by Airy
#include <time.h>
#include <utmp.h>
#include <utmpx.h>
#include <wtmpparse.h>
void LogFileParser::parseByNormal(QList<LOG_MSG_NORMAL> &nList, qint64 ms)
{
    if (m_isNormalLoading) {
        return;
    }
    m_isNormalLoading = true;
    int ret = -2;
    struct utmp *utbufp, *wtmp_next();

    if (wtmp_open(WTMP_FILE) == -1) {
        printf("open WTMP_FILE file error\n");
        return;  // exit(1) will exit this application
    }

    struct utmp_list *normalList = st_list_init();
    struct utmp_list *deadList = st_list_init();
    struct utmp *nodeUTMP = st_utmp_init();
    struct utmp_list *firstNormalList = normalList;
    struct utmp_list *firstDeadList = deadList;
    while ((utbufp = wtmp_next()) != ((struct utmp *)NULL)) {
        if (utbufp->ut_type != DEAD_PROCESS)
            list_insert(normalList, utbufp);
        else if (utbufp->ut_type == DEAD_PROCESS)
            list_insert(deadList, utbufp);
    }

    normalList = normalList->next;

    QString a_name = "~";
    while (normalList) {
        QString strtmp = normalList->value.ut_name;
        if (strtmp.compare("runlevel") == 0) {  // clear the runlevel
            normalList = normalList->next;
            continue;
        }

        memset(nodeUTMP, 0, sizeof(struct utmp));
        ret = list_get_ele_and_del(deadList, normalList->value.ut_line, nodeUTMP);

        LOG_MSG_NORMAL Nmsg;

        if (normalList->value.ut_type == USER_PROCESS) {
            Nmsg.eventType = "Login";
            Nmsg.userName = normalList->value.ut_name;
            a_name = Nmsg.userName;
        } else {
            Nmsg.eventType = normalList->value.ut_name;
            if (strtmp.compare("reboot") == 0) {
                Nmsg.eventType = "Boot";
            }
            Nmsg.userName = a_name;
        }

        QString end_str;
        if (deadList != NULL && ret != -1)
            end_str = show_end_time(nodeUTMP->ut_time);
        else if (ret == -1 && normalList->value.ut_type == USER_PROCESS)
            end_str = "still logged in";
        else if (ret == -1 && normalList->value.ut_type == BOOT_TIME)
            end_str = "system boot";

        QString start_str = show_start_time(normalList->value.ut_time);

        QString n_time =
            QDateTime::fromTime_t(normalList->value.ut_time).toString("yyyy-MM-dd hh:mm:ss");

        end_str = end_str.remove(QChar('\n'), Qt::CaseInsensitive);
        start_str = start_str.remove(QChar('\n'), Qt::CaseInsensitive);

        Nmsg.dateTime = n_time;
        QDateTime nn_time = QDateTime::fromString(Nmsg.dateTime, "yyyy-MM-dd hh:mm:ss");
        if (nn_time.toMSecsSinceEpoch() < ms) { // add by Airy
            normalList = normalList->next;
            continue;
        }

        Nmsg.msg = start_str + "  ~  " + end_str;
        normalList = normalList->next;
        printf("\n");
        nList.insert(0, Nmsg);
    }
    free(nodeUTMP);
    list_delete(firstNormalList);
    list_delete(firstDeadList);
    free(normalList);
    free(deadList);
    wtmp_close();
    m_isNormalLoading = false;
    emit normalFinished();

    //    QProcess proc;
    //    proc.start("last -x");  // file path is fixed. so write cmd direct
    //    proc.waitForFinished(-1);

    //    if (isErroCommand(QString(proc.readAllStandardError())))
    //        return;

    //    QString output = proc.readAllStandardOutput();
    //    proc.close();

    //    for (QString str : output.split('\n')) {
    //        QStringList list = str.split("  ", QString::SkipEmptyParts);

    //        if (list.size() < 4) {
    //            continue;
    //        } else {
    //            LOG_MSG_NORMAL Nmsg;
    //            Nmsg.user = list.at(0);
    //            Nmsg.src = list.at(1);
    //            Nmsg.datetime = list.at(2);
    //            Nmsg.status = list.at(3);

    //            nList.insert(0, Nmsg);
    //            for (int i = 0; i < list.size(); i++) {
    //                qDebug() << list[i];
    //            }
    //        }
    //    }
}  // end add
#if 0
void LogFileParser::parseByXlog(QStringList &xList)
{
    QProcess proc;
    proc.start("cat /var/log/Xorg.0.log");  // file path is fixed. so write cmd direct
    proc.waitForFinished(-1);

    if (isErroCommand(QString(proc.readAllStandardError())))
        return;

    QString output = proc.readAllStandardOutput();
    proc.close();

    for (QString str : output.split('\n')) {
        if (str.startsWith("[")) {
            //            xList.append(str);
            xList.insert(0, str);
        } else {
            str += " ";
            //            xList[xList.size() - 1] += str;
            xList[0] += str;
        }
    }
    createFile(output, xList.count());

    emit xlogFinished();
}
#endif

void LogFileParser::parseByBoot()
{
    if (m_isBootLoading) {
        return;
    }
    m_isBootLoading = true;
    m_authThread = LogAuthThread::instance();
    if (m_authThread->isRunning())
        m_authThread->terminate();

    disconnect(m_authThread, SIGNAL(cmdFinished(LOG_FLAG, QString)), this,
               SLOT(slot_threadFinished(LOG_FLAG, QString)));
    m_authThread->setType(BOOT);
    connect(m_authThread, SIGNAL(cmdFinished(LOG_FLAG, QString)), this,
            SLOT(slot_threadFinished(LOG_FLAG, QString)));

    m_authThread->start();
}

void LogFileParser::parseByKern(qint64 ms)
{
    if (m_isKernLoading) {
        return;
    }
    m_isKernLoading = true;
    m_authThread = LogAuthThread::instance();
    if (m_authThread->isRunning())
        m_authThread->terminate();

    disconnect(m_authThread, SIGNAL(cmdFinished(LOG_FLAG, QString)), this,
               SLOT(slot_threadFinished(LOG_FLAG, QString)));
    m_authThread->setType(KERN);
    m_selectTime = ms;
    connect(m_authThread, SIGNAL(cmdFinished(LOG_FLAG, QString)), this,
            SLOT(slot_threadFinished(LOG_FLAG, QString)));

    m_authThread->start();
}

void LogFileParser::parseByApp(QString path, int lv, qint64 ms)
{
    if (m_isAppLoading) {
        return;
    }
    m_isAppLoading = true;

    m_appThread = LogApplicationParseThread::instance();
    if (m_appThread->isRunning())
        m_appThread->terminate();

    disconnect(m_appThread, SIGNAL(appCmdFinished(QList<LOG_MSG_APPLICATOIN>)), this,
               SLOT(slot_applicationFinished(QList<LOG_MSG_APPLICATOIN>)));

    m_appThread->setParam(path, lv, ms);

    connect(m_appThread, SIGNAL(appCmdFinished(QList<LOG_MSG_APPLICATOIN>)), this,
            SLOT(slot_applicationFinished(QList<LOG_MSG_APPLICATOIN>)));

    m_appThread->start();
}

void LogFileParser::createFile(QString output, int count)
{
#if 1
    Q_UNUSED(output)
    Q_UNUSED(count)
#else
    // this is for test parser.
    QFile fi("tempFile");
    if (!fi.open(QIODevice::ReadWrite | QIODevice::Truncate))
        return;
    fi.write(output.toLatin1());
    fi.write(QString::number(count).toLatin1());
    fi.close();
#endif
}

void LogFileParser::stopAllLoad()
{
    return;
//    if (work && work->isRunning())
//        work->terminate();
//    if (m_pDkpgDataLoader && m_pDkpgDataLoader->isOpen()) {
//        m_pDkpgDataLoader->terminate();
//        m_pDkpgDataLoader->close();
//    }
//    if (m_pXlogDataLoader && m_pXlogDataLoader->isOpen()) {
//        m_pXlogDataLoader->terminate();
//        m_pXlogDataLoader->close();
//    }

//    if (m_authThread && m_authThread->isRunning()) {
//        m_authThread->terminate();
//        m_authThread->wait();
//    }
//    if (m_authThread && m_authThread->isRunning()) {
//        m_authThread->terminate();
//        m_authThread->wait();
//    }

//    if (m_appThread && m_appThread->isRunning()) {
//        m_appThread->terminate();
//        m_appThread->wait();
//    }
//    m_isProcess = false;
}

bool LogFileParser::isErroCommand(QString str)
{
    if (str.contains("权限") || str.contains("permission", Qt::CaseInsensitive)) {
        DMessageBox::information(nullptr, tr("information"),
                                 str + "\n" + "Please use 'sudo' run this application");
        return true;
    }
    if (str.contains("请重试") || str.contains("retry", Qt::CaseInsensitive)) {
        DMessageBox::information(nullptr, tr("information"),
                                 "The password is incorrect,please try again");
        m_rootPasswd = "";
        return true;
    }
    return false;
}

qint64 LogFileParser::formatDateTime(QString m, QString d, QString t)
{
    //    QDateTime::fromString("9月 24 2019 10:32:34", "MMM d yyyy hh:mm:ss");
    // default year =2019
    QLocale local(QLocale::English, QLocale::UnitedStates);

    QDate curdt = QDate::currentDate();

    QString tStr = QString("%1 %2 %3 %4").arg(m).arg(d).arg(curdt.year()).arg(t);
    QDateTime dt = local.toDateTime(tStr, "MMM d yyyy hh:mm:ss");
    return dt.toMSecsSinceEpoch();
}

void LogFileParser::slot_journalFinished()
{
    m_isJournalLoading = false;
    emit journalFinished();

}

void LogFileParser::slot_applicationFinished(QList<LOG_MSG_APPLICATOIN> iAppList)
{
    m_isAppLoading = false;
    emit applicationFinished(iAppList);
}

#include <unistd.h>
#include <QApplication>
void LogFileParser::slot_threadFinished(LOG_FLAG flag, QString output)
{
    switch (flag) {
    case BOOT: {
        QList<LOG_MSG_BOOT> bList;

        for (QString lineStr : output.split('\n')) {
            if (lineStr.startsWith("/dev"))
                continue;

            // remove Useless characters
            lineStr.replace(QRegExp("\\x1B\\[\\d+(;\\d+){0,2}m"), "");

            QStringList retList;
            LOG_MSG_BOOT bMsg;
            retList = lineStr.split(" ", QString::SkipEmptyParts);
            if (lineStr.startsWith("[")) {
                bMsg.status = retList[1];
                QStringList leftList = retList.mid(3);
                bMsg.msg += leftList.join(" ");
                bList.insert(0, bMsg);
            } else {
                if (bList.size() == 0)
                    continue;

                bList[0].msg += retList.join(" ");
            }
        }

        createFile(output, bList.count());
        m_isBootLoading = false;
        emit bootFinished(bList);

    } break;
    case KERN: {
        QList<LOG_MSG_JOURNAL> kList;
        //            qDebug() << "ms::" << m_selectTime << output;
        QStringList a = output.split('\n');
        for (QString str : a) {
            LOG_MSG_JOURNAL msg;

            str.replace(QRegExp("\\#033\\[\\d+(;\\d+){0,2}m"), "");
            QStringList list = str.split(" ", QString::SkipEmptyParts);
            if (list.size() < 5)
                continue;

            QStringList timeList;
            timeList.append(list[0]);
            timeList.append(list[1]);
            timeList.append(list[2]);
            qint64 iTime = formatDateTime(list[0], list[1], list[2]);
            if (iTime < m_selectTime)
                continue;

            msg.dateTime = timeList.join(" ");
            msg.hostName = list[3];

            QStringList tmpList = list[4].split("[");
            if (tmpList.size() != 2) {
                msg.daemonName = list[4].split(":")[0];
            } else {
                msg.daemonName = list[4].split("[")[0];
                QString id = list[4].split("[")[1];
                id.chop(2);
                msg.daemonId = id;
            }

            QString msgInfo;
            for (auto i = 5; i < list.size(); i++) {
                msgInfo.append(list[i] + " ");
            }
            msg.msg = msgInfo;

            //            kList.append(msg);
            kList.insert(0, msg);

            QApplication::processEvents();  // devide UI and parser
        }

        createFile(output, kList.count());
        m_isKernLoading = false;
        emit kernFinished(kList);
    } break;
    default:
        break;
    }
}
