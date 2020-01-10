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
#include "journalwork.h"

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
    work = journalWork::instance();
    if (work->isRunning())
        work->terminate();
    //    work = new journalWork();
    disconnect(work, SIGNAL(journalFinished()), this,
               SLOT(slot_journalFinished()));
    work->setArg(arg);
    connect(work, SIGNAL(journalFinished()), this,
            SLOT(slot_journalFinished()), Qt::QueuedConnection);
    work->start();
}

void LogFileParser::parseByDpkg(QList<LOG_MSG_DPKG> &dList, qint64 ms)
{
    QProcess proc;
    proc.start("cat /var/log/dpkg.log");  // file path is fixed. so write cmd direct
    proc.waitForFinished(-1);
    QString output = proc.readAllStandardOutput();
    proc.close();

    for (QString str : output.split('\n')) {
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
    emit dpkgFinished();
}

void LogFileParser::parseByXlog(QList<LOG_MSG_XORG> &xList)
{
    QProcess proc;
    proc.start("cat /var/log/Xorg.0.log");  // file path is fixed. so write cmd direct
    proc.waitForFinished(-1);

    if (isErroCommand(QString(proc.readAllStandardError())))
        return;

    QString output = proc.readAllStandardOutput();
    proc.close();

    QDateTime curDt = QDateTime::currentDateTime();
    qint64 curDtSecond = curDt.toMSecsSinceEpoch();
    for (QString str : output.split('\n')) {
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
            QDateTime realDt = QDateTime::fromMSecsSinceEpoch(realT);

            LOG_MSG_XORG msg;
            msg.dateTime = realDt.toString("yyyy-MM-dd hh:mm:ss.zzz");
            msg.msg = msgInfo;

            xList.insert(0, msg);
        } else {
            xList[0].msg += str;
        }
    }
    createFile(output, xList.count());

    emit xlogFinished();
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
    m_appThread = LogApplicationParseThread::instance();
    if (m_appThread->isRunning())
        m_appThread->terminate();

    disconnect(m_appThread, SIGNAL(appCmdFinished(QList<LOG_MSG_APPLICATOIN>)), this,
               SIGNAL(applicationFinished(QList<LOG_MSG_APPLICATOIN>)));

    m_appThread->setParam(path, lv, ms);

    connect(m_appThread, SIGNAL(appCmdFinished(QList<LOG_MSG_APPLICATOIN>)), this,
            SIGNAL(applicationFinished(QList<LOG_MSG_APPLICATOIN>)));

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

bool LogFileParser::isErroCommand(QString str)
{
    if (str.contains("权限") || str.contains("permission", Qt::CaseInsensitive)) {
        DMessageBox::information(nullptr, tr("infomation"),
                                 str + "\n" + tr("Please use 'sudo' run this application"));
        return true;
    }
    if (str.contains("请重试") || str.contains("retry", Qt::CaseInsensitive)) {
        DMessageBox::information(nullptr, tr("infomation"),
                                 tr("The password is incorrect,please try again"));
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
    emit journalFinished();
}

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
        emit bootFinished(bList);

    } break;
    case KERN: {
        QList<LOG_MSG_JOURNAL> kList;
        //            qDebug() << "ms::" << m_selectTime << output;

        for (QString str : output.split('\n')) {
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
        }

        createFile(output, kList.count());
        emit kernFinished(kList);
    } break;
    default:
        break;
    }
}
