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
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
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
#include <QtConcurrent>


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
    qRegisterMetaType<QList<LOG_MSG_KWIN> > ("QList<LOG_MSG_KWIN>");
    qRegisterMetaType<QList<LOG_MSG_XORG> > ("QList<LOG_MSG_XORG>");
    qRegisterMetaType<QList<LOG_MSG_DPKG> > ("QList<LOG_MSG_DPKG>");
    qRegisterMetaType<LOG_FLAG> ("LOG_FLAG");

}

LogFileParser::~LogFileParser() {}

void LogFileParser::parseByJournal(QStringList arg)
{
    stopAllLoad();
//    if (m_isJournalLoading) {
//        return;
//    }
    m_isJournalLoading = true;
//    if (work && work->isRunning()) {
//        work->stopWork();
//        // quitLogAuththread(work);
//        work->quit();
//        work->wait();

//    }

    work = journalWork::instance();

    //    work = new journalWorkd();
    disconnect(work, SIGNAL(journalFinished()), this, SLOT(slot_journalFinished()));
    disconnect(work, &journalWork::journalData, this, &LogFileParser::journalData);
    work->setArg(arg);
    connect(work, SIGNAL(journalFinished()), this,  SLOT(slot_journalFinished()),
            Qt::QueuedConnection);
    connect(work, &journalWork::journalData, this, &LogFileParser::journalData,
            Qt::QueuedConnection);
    //work->start();
    QtConcurrent::run(work, &journalWork::doWork);
}

void LogFileParser::parseByDpkg(qint64 ms)
{

    stopAllLoad();
    LogAuthThread   *authThread = new LogAuthThread(this);
    authThread->setType(DPKG);
    DKPG_FILTERS dpkgfilter;
    dpkgfilter.timeFilter = ms;
    authThread->setFileterParam(dpkgfilter);
    connect(authThread, &LogAuthThread::proccessError, this,
            &LogFileParser::slog_proccessError, Qt::UniqueConnection);
    connect(authThread, &LogAuthThread::dpkgFinished, this,
            &LogFileParser::dpkgFinished, Qt::UniqueConnection);
    QThreadPool::globalInstance()->start(authThread);
    //  m_authThread->start();

}

void LogFileParser::parseByXlog(qint64 ms)  // modifed by Airy
{
    stopAllLoad();
    LogAuthThread   *authThread = new LogAuthThread(this);
    authThread->setType(XORG);
    XORG_FILTERS xorgfilter;
    xorgfilter.timeFilter = ms;
    authThread->setFileterParam(xorgfilter);
    connect(authThread, &LogAuthThread::proccessError, this,
            &LogFileParser::slog_proccessError, Qt::UniqueConnection);
    connect(authThread, &LogAuthThread::xorgFinished, this,
            &LogFileParser::xlogFinished, Qt::UniqueConnection);
    QThreadPool::globalInstance()->tryStart(authThread);
//    if (m_isXlogLoading) {
//        return;
//    }
//    m_isXlogLoading = true;
//    QFile file("/var/log/Xorg.0.log");  // if not,maybe crash
//    if (!file.exists())
//        return;
//    stopAllLoad();
//    if (!m_pXlogDataLoader) {
//        m_pXlogDataLoader = new QProcess(this);
//    }
//    m_pXlogDataLoader->start("cat /var/log/Xorg.0.log");  // file path is fixed. so write cmd direct
//    m_pXlogDataLoader->waitForFinished(-1);
//    QString errorStr(m_pXlogDataLoader->readAllStandardError());
//    Utils::CommandErrorType commandErrorType = Utils::isErroCommand(errorStr);
//    if (commandErrorType != Utils::NoError) {
//        if (commandErrorType == Utils::PermissionError) {
//            DMessageBox::information(nullptr, tr("information"),
//                                     errorStr + "\n" + "Please use 'sudo' run this application");
//        } else if (commandErrorType == Utils::RetryError) {
//            DMessageBox::information(nullptr, tr("information"),
//                                     "The password is incorrect,please try again");
//        }
//        return;
//    }

//    QByteArray outByte = m_pXlogDataLoader->readAllStandardOutput();
//    QString output = Utils::replaceEmptyByteArray(outByte);
//    m_pXlogDataLoader->close();
//    QDateTime curDt = QDateTime::currentDateTime();
//    qint64 curDtSecond = curDt.toMSecsSinceEpoch();
//    for (QString str : output.split('\n')) {
//        str.replace(QRegExp("\\x1B\\[\\d+(;\\d+){0,2}m"), "");

//        //        if (str.startsWith("[")) {
//        //            //            xList.append(str);
//        //            xList.insert(0, str);
//        //        } else {
//        //            str += " ";
//        //            //            xList[xList.size() - 1] += str;
//        //            xList[0] += str;
//        //        }
//        if (str.startsWith("[")) {
//            QStringList list = str.split("]", QString::SkipEmptyParts);
//            if (list.count() != 2)
//                continue;

//            QString timeStr = list[0];
//            QString msgInfo = list[1];

//            // get time
//            QString tStr = timeStr.split("[", QString::SkipEmptyParts)[0].trimmed();
//            qint64 realT = curDtSecond + qint64(tStr.toDouble() * 1000);
//            //   qint64 realT =  qint64(tStr.toDouble() * 1000);
//            QDateTime realDt = QDateTime::fromMSecsSinceEpoch(realT);
//            if (realDt.toMSecsSinceEpoch() < ms)  // add by Airy
//                continue;

//            LOG_MSG_XORG msg;
//            msg.dateTime = realDt.toString("yyyy-MM-dd hh:mm:ss.zzz");
//            msg.msg = msgInfo;

//            xList.insert(0, msg);
//        } else {
//            if (xList.length() > 0) {
//                xList[0].msg += str;
//            }
//        }
//    }
//    createFile(output, xList.count());
//    m_isXlogLoading = false;
//    emit xlogFinished();

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
    stopAllLoad();
    m_isNormalLoading = true;
    int ret = -2;
    struct utmp *utbufp;
    wtmp_next();
    if (wtmp_open(QString(WTMP_FILE).toLatin1().data()) == -1) {
        printf("open WTMP_FILE file error\n");
        return;  // exit(1) will exit this application
    }
    QList<utmp > normalList;
    QList<utmp > deadList;
    while ((utbufp = wtmp_next()) != (static_cast<struct utmp *>(nullptr))) {
        if (utbufp->ut_type != DEAD_PROCESS) {
            utmp value_ = *utbufp;
            normalList.append(value_);
        } else if (utbufp->ut_type == DEAD_PROCESS) {
            utmp value_ = *utbufp;

            deadList.append(value_);
        }
    }
//    foreach (utmp item, normalList) {
//        qDebug() << "normalList" << item.ut_name << "line" << item.ut_line  << "type" << item.ut_type << "time" << QDateTime::fromTime_t(item.ut_time).toString("yyyy-MM-dd hh:mm:ss");
//    }
//    foreach (utmp item, deadList) {
//        qDebug() << "deadList" << item.ut_name << "line" << item.ut_line  << "type" << item.ut_type << "time" << QDateTime::fromTime_t(item.ut_time).toString("yyyy-MM-dd hh:mm:ss");
//    }
    QString a_name = "~";
    foreach (utmp value, normalList) {
        QString strtmp = value.ut_name;
        qDebug() << value.ut_name << value.ut_type;
        if (strtmp.compare("runlevel") == 0 || (value.ut_type == RUN_LVL && strtmp != "shutdown") || value.ut_type == INIT_PROCESS) { // clear the runlevel
            //   if (strtmp.compare("runlevel") == 0) {  // clear the runlevel
            continue;
        }

        struct utmp nodeUTMP   = list_get_ele_and_del(deadList, value.ut_line, ret);
        LOG_MSG_NORMAL Nmsg;
        if (value.ut_type == USER_PROCESS) {
            Nmsg.eventType = "Login";
            Nmsg.userName = value.ut_name;
            a_name = Nmsg.userName;
        } else {
            Nmsg.eventType = value.ut_name;
            if (strtmp.compare("reboot") == 0) {
                Nmsg.eventType = "Boot";
            }
            Nmsg.userName = a_name;
        }
        QString end_str;
        if (deadList.length() > 0 && ret != -1)
            end_str = show_end_time(nodeUTMP.ut_time);
        else if (ret == -1 && value.ut_type == USER_PROCESS)
            end_str = "still logged in";
        else if (ret == -1 && value.ut_type == BOOT_TIME)
            end_str = "system boot";
        QString start_str = show_start_time(value.ut_time);

        QString n_time = QDateTime::fromTime_t(static_cast<uint>(value.ut_time)).toString("yyyy-MM-dd hh:mm:ss");
        end_str = end_str.remove(QChar('\n'), Qt::CaseInsensitive);
        start_str = start_str.remove(QChar('\n'), Qt::CaseInsensitive);
        Nmsg.dateTime = n_time;
        QDateTime nn_time = QDateTime::fromString(Nmsg.dateTime, "yyyy-MM-dd hh:mm:ss");
        if (nn_time.toMSecsSinceEpoch() < ms) { // add by Airy
            continue;
        }
        Nmsg.msg = start_str + "  ~  " + end_str;
        printf("\n");
        nList.insert(0, Nmsg);
    }
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
}

void LogFileParser::parseByKwin(KWIN_FILTERS iKwinfilter)
{
    stopAllLoad();
    LogAuthThread   *authThread = new LogAuthThread(this);
    authThread->setType(Kwin);
    authThread->setFileterParam(iKwinfilter);
    connect(authThread, &LogAuthThread::kwinFinished, this,
            &LogFileParser::kwinFinished);
    QThreadPool::globalInstance()->start(authThread);
}
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
//    if (m_isBootLoading) {
//        qDebug() << __FUNCTION__ << m_isBootLoading;
//        return;
//    }
    stopAllLoad();
    m_isBootLoading = true;
    LogAuthThread   *authThread = new LogAuthThread(this);
    authThread->setType(BOOT);
    connect(authThread, &LogAuthThread::bootFinished, this,
            &LogFileParser::slot_bootFinished);
    connect(this, &LogFileParser::stopBoot, authThread,
            &LogAuthThread::stopProccess);
    QThreadPool::globalInstance()->start(authThread);

}

void LogFileParser::parseByKern(qint64 ms)
{
//    if (m_isKernLoading) {
//        return;
//    }

    stopAllLoad();
    m_isKernLoading = true;
    LogAuthThread   *authThread = new LogAuthThread(this);
    authThread->setType(KERN);
    m_selectTime = ms;
    connect(authThread, &LogAuthThread::kernFinished, this,
            &LogFileParser::slot_kernFinished);
    connect(this, &LogFileParser::stopKern, authThread,
            &LogAuthThread::stopProccess);
    QThreadPool::globalInstance()->start(authThread);
}

void LogFileParser::parseByApp(QString path, int lv, qint64 ms)
{
    if (m_isAppLoading) {
        return;
    }
    stopAllLoad();
    m_isAppLoading = true;

    m_appThread = LogApplicationParseThread::instance();
    quitLogAuththread(m_appThread);

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
    //  quitLogAuththread(work);
//    if (m_authThread) {
//        m_authThread->stopProccess();
//        quitLogAuththread(m_authThread);
//    }
    emit stopBoot();
    emit stopKern();
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

void LogFileParser::quitLogAuththread(QThread *iThread)
{
    if (iThread && iThread->isRunning()) {
        //  iThread->terminate();
        iThread->quit();
        iThread->wait();
    }
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
void LogFileParser::slot_kernFinished(LOG_FLAG flag, QString output)
{
    Q_UNUSED(flag)
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
}
void LogFileParser::slot_bootFinished(LOG_FLAG flag, QString output)
{
    Q_UNUSED(flag)
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

}
void LogFileParser::slog_proccessError(const QString &iError)
{
    DMessageBox::information(nullptr, tr("information"), iError);
}




