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
#include "logauththread.h"
#include "utils.h"
#include <QDebug>
#include <QDateTime>


#include "sharedmemorymanager.h"
std::atomic<LogAuthThread *> LogAuthThread::m_instance;
std::mutex LogAuthThread::m_mutex;
int LogAuthThread::thread_count = 0;

LogAuthThread::LogAuthThread(QObject *parent)
    :  QObject(parent),
       QRunnable()

{
    setAutoDelete(true);
    thread_count++;
    m_threadCount = thread_count;


}

LogAuthThread::~LogAuthThread()
{
    stopProccess();
    if (m_process) {
        //   m_process->kill();
        m_process->close();
        m_process->deleteLater();
        m_process = nullptr;
    }


}

QString LogAuthThread::getStandardOutput()
{
    return m_output;
}

QString LogAuthThread::getStandardError()
{
    return m_error;
}

void LogAuthThread::stopProccess()
{

    if (m_isStopProccess) {
        return;
    }
    m_isStopProccess = true;
    m_canRun = false;
    ShareMemoryInfo   shareInfo ;
    shareInfo.isStart = false;
    SharedMemoryManager::instance()->setRunnableTag(shareInfo);
    if (m_process) {
        m_process->kill();

    }

    // kill(qvariant_cast<pid_t>(m_process->pid()), SIGKILL);


    // m_process->close();
//        delete  m_process;
//        m_process = nullptr;
    //m_process->terminate();

    //  m_process->close();
    //m_process->waitForFinished(-1);

}

void LogAuthThread::run()
{
    qDebug() << " LogAuthThread::run" << m_type;
    m_canRun = true;
    switch (m_type) {
    case KERN:
        handleKern();
        break;
    case BOOT:
        handleBoot();
        break;
    case Kwin:
        handleKwin();
        break;
    case XORG:
        handleXorg();
        break;
    case DPKG:
        handleDkpg();
        break;
    default:
        break;
    }

    m_canRun = false;
}

#include <QFile>
void LogAuthThread::handleBoot()
{
    QList<LOG_MSG_BOOT> bList;
    QFile file("/var/log/boot.log");  // add by Airy
    if (!file.exists()) {
        emit bootFinished(bList);
        return;
    }
    if (!m_canRun) {
        return;
    }
    initProccess();
    m_process->setProcessChannelMode(QProcess::MergedChannels);
    ShareMemoryInfo   shareInfo ;
    shareInfo.isStart = true;
    SharedMemoryManager::instance()->setRunnableTag(shareInfo);
    m_process->start("pkexec", QStringList() << "logViewerAuth"
                     << "/var/log/boot.log" << SharedMemoryManager::instance()->getRunnableKey());
    m_process->waitForFinished(-1);

    QByteArray byte =   m_process->readAllStandardOutput();
    QTextStream stream(&byte);
    QByteArray encode;
    stream.setCodec(encode);
    QString str = stream.readAll();
    QStringList l = str.split('\n');
    qDebug() << __FUNCTION__ << "byte" << byte.length();
    qDebug() << __FUNCTION__ << "str" << str.length();


    for (QString lineStr : str.split('\n')) {
        if (lineStr.startsWith("/dev"))
            continue;

        // remove Useless characters
        lineStr.replace(QRegExp("\\x1B\\[\\d+(;\\d+){0,2}m"), "");
        Utils::replaceColorfulFont(&lineStr);
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
    emit bootFinished(bList);
}

void LogAuthThread::handleKern()
{
    QList<LOG_MSG_JOURNAL> kList;
    QFile file("/var/log/kern.log"); // add by Airy
    if (!file.exists()) {
        emit kernFinished(kList);
        return;
    }
    if (!m_canRun) {
        return;
    }
    initProccess();
    if (!m_canRun) {
        return;
    }
    // connect(proc, &QProcess::readyRead, this, &LogAuthThread::onFinishedRead);
    m_process->setProcessChannelMode(QProcess::MergedChannels);
    if (!m_canRun) {
        return;
    }
    if (!SharedMemoryManager::instance()->isAttached()) {
        return;
    }
    ShareMemoryInfo   shareInfo ;
    shareInfo.isStart = true;
    SharedMemoryManager::instance()->setRunnableTag(shareInfo);
    m_process->start("pkexec", QStringList() << "logViewerAuth"
                     //         << "/home/zyc/Documents/tech/同方内核日志没有/kern.log" << SharedMemoryManager::instance()->getRunnableKey());
                     //   << "/home/zyc/Documents/tech/klu内核日志读取崩溃日志/kern.log" << SharedMemoryManager::instance()->getRunnableKey());
                     << "/var/log/kern.log" << SharedMemoryManager::instance()->getRunnableKey());
    //proc->start("pkexec", QStringList() << "/bin/bash" << "-c" << QString("cat %1").arg("/var/log/kern.log"));
    // proc->start("pkexec", QStringList() << QString("cat") << QString("/var/log/kern.log"));
    m_process->waitForFinished(-1);
    qDebug() << " m_process->exitCode() " << m_process->exitCode();
    if (m_process->exitCode() != 0) {
        emit kernFinished(kList);
        return;
    }
    if (!m_canRun) {
        return;
    }
    QByteArray byte =   m_process->readLine();
    while (!byte.isNull()) {
        //   qDebug() << "111111";
        if (!m_canRun) {
            return;
        }
        QTextStream stream(&byte);
        if (!m_canRun) {
            return;
        }
        QByteArray encode;
        if (!m_canRun) {
            return;
        }
        stream.setCodec(encode);
        if (!m_canRun) {
            return;
        }
        QString str = stream.readAll();
        if (!m_canRun) {
            return;
        }
        QStringList l = str.split('\n');
        if (!m_canRun) {
            return;
        }
        //   qDebug() << __FUNCTION__ << "byte" << byte.length();
        //  qDebug() << __FUNCTION__ << "str" << str;


        //            qDebug() << "ms::" << m_selectTime << output;
        QStringList a = str.split('\n');
        for (QString str : a) {
            if (!m_canRun) {
                return;
            }
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
            if (m_kernFilters.timeFilterBegin > 0 && m_kernFilters.timeFilterEnd > 0) {
                if (iTime < m_kernFilters.timeFilterBegin || iTime > m_kernFilters.timeFilterEnd)
                    continue;
            }

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
            if (!m_canRun) {
                return;
            }

        }
        if (!m_canRun) {
            return;
        }
        byte =   m_process->readLine();
    }

    emit kernFinished(kList);

}

void LogAuthThread::handleKwin()
{
    QFile file(KWIN_TREE_DATA);
    if (!m_canRun) {
        return;
    }
    QList<LOG_MSG_KWIN> kwinList;
    if (!file.exists()) {
        emit kwinFinished(kwinList);
        return;
    }
    if (!m_canRun) {
        return;
    }
    initProccess();
    m_process->start("cat", QStringList() << KWIN_TREE_DATA);
    m_process->waitForFinished(-1);
    if (!m_canRun) {
        return;
    }
    QByteArray outByte = m_process->readAllStandardOutput();
    if (!m_canRun) {
        return;
    }
    QString output = Utils::replaceEmptyByteArray(outByte);
    if (!m_canRun) {
        return;
    }
    QString filterMsgStr = m_kwinFilters.msg;
    QString filterMsgStr1 = m_kwinFilters.msg;
    for (QString str : output.split('\n')) {
//        if (!(filterMsgStr1.trimmed().isEmpty()) && !str.contains(filterMsgStr, Qt::CaseInsensitive)) {
//            continue;
//        }
        if (!m_canRun) {
            return;
        }
        if (str.trimmed().isEmpty()) {
            continue;
        }
        LOG_MSG_KWIN kwinMsg;
        kwinMsg.msg = str;
        kwinList.insert(0, kwinMsg);
    }

    if (!m_canRun) {
        return;
    }
    emit kwinFinished(kwinList);
}

void LogAuthThread::handleXorg()
{
    QFile file("/var/log/Xorg.0.log");  // if not,maybe crash
    QFile startFile("/proc/uptime");
    QList<LOG_MSG_XORG> xList;
    if (!file.exists() || !startFile.exists()) {
        emit proccessError(tr("Log file is empty"));
        emit xorgFinished(xList);
        return;
    }
    if (!m_canRun) {
        return;
    }
    QString startStr = "";
    if (startFile.open(QFile::ReadOnly)) {

        startStr = QString(startFile.readLine());
        startFile.close();
    }
    if (!m_canRun) {
        return;
    }
    qDebug() << "startStr" << startFile;
    startStr = startStr.split(" ").value(0, "");
    if (startStr.isEmpty()) {
        emit proccessError(tr("Log file is empty"));
        emit xorgFinished(xList);
        return;
    }
    if (!m_canRun) {
        return;
    }
    initProccess();
    if (!m_canRun) {
        return;
    }
    m_process->start("cat /var/log/Xorg.0.log");  // file path is fixed. so write cmd direct
    m_process->waitForFinished(-1);
    if (!m_canRun) {
        return;
    }
    QString errorStr(m_process->readAllStandardError());
    if (!m_canRun) {
        return;
    }
    Utils::CommandErrorType commandErrorType = Utils::isErroCommand(errorStr);
    if (!m_canRun) {
        return;
    }
    if (commandErrorType != Utils::NoError) {
        if (commandErrorType == Utils::PermissionError) {
            emit proccessError(errorStr + "\n" + "Please use 'sudo' run this application");
        } else if (commandErrorType == Utils::RetryError) {
            emit proccessError("The password is incorrect,please try again");
        }
        return;
    }
    if (!m_canRun) {
        return;
    }
    QByteArray outByte = m_process->readAllStandardOutput();
    QString output = Utils::replaceEmptyByteArray(outByte);
    m_process->close();
    if (!m_canRun) {
        return;
    }
    QDateTime curDt = QDateTime::currentDateTime();
    qint64 curDtSecond = curDt.toMSecsSinceEpoch() - static_cast<int>(startStr.toDouble() * 1000);
    if (!m_canRun) {
        return;
    }
    for (QString str : output.split('\n')) {
        if (!m_canRun) {
            return;
        }
        str.replace(QRegExp("\\x1B\\[\\d+(;\\d+){0,2}m"), "");

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
            if (m_xorgFilters.timeFilterBegin > 0 && m_xorgFilters.timeFilterEnd > 0) {
                if (realDt.toMSecsSinceEpoch() < m_xorgFilters.timeFilterBegin || realDt.toMSecsSinceEpoch() > m_xorgFilters.timeFilterEnd)
                    continue;
            }
            LOG_MSG_XORG msg;
            msg.dateTime = realDt.toString("yyyy-MM-dd hh:mm:ss.zzz");
            msg.msg = msgInfo;

            xList.insert(0, msg);
        } else {
            if (xList.length() > 0) {
                xList[0].msg += str;
            }
        }
        if (!m_canRun) {
            return;
        }
    }
    qDebug() << "xorg size" << xList.length();
    if (!m_canRun) {
        return;
    }
    emit xorgFinished(xList);
}

void LogAuthThread::handleDkpg()
{

    QFile file("/var/log/dpkg.log");  // if not,maybe crash
    if (!file.exists())
        return;
    if (!m_canRun) {
        return;
    }
    initProccess();
    if (!m_canRun) {
        return;
    }
    m_process->start("cat /var/log/dpkg.log");  // file path is fixed. so write cmd direct
    m_process->waitForFinished(-1);
    if (!m_canRun) {
        return;
    }
    QByteArray outByte = m_process->readAllStandardOutput();
    if (!m_canRun) {
        return;
    }
    QString output = Utils::replaceEmptyByteArray(outByte);

    m_process->close();
    if (!m_canRun) {
        return;
    }
    QList<LOG_MSG_DPKG> dList;
    for (QString str : output.split('\n')) {
        if (!m_canRun) {
            return;
        }
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
        if (m_dkpgFilters.timeFilterBegin > 0 && m_dkpgFilters.timeFilterEnd > 0) {
            if (dt.toMSecsSinceEpoch() < m_dkpgFilters.timeFilterBegin || dt.toMSecsSinceEpoch() > m_dkpgFilters.timeFilterEnd)
                continue;
        }
        dpkgLog.action = strList[2];
        dpkgLog.msg = info;

        //        dList.append(dpkgLog);
        dList.insert(0, dpkgLog);
        if (!m_canRun) {
            return;
        }
    }
    if (!m_canRun) {
        return;
    }
    qDebug() << "dkpg size" << dList.length();
    emit dpkgFinished(dList);
}

void LogAuthThread::initProccess()
{
    if (!m_process) {
        m_process = new QProcess;
        //connect(m_process, SIGNAL(finished(int)), this, SLOT(onFinished(int)), Qt::UniqueConnection);

    }
}

qint64 LogAuthThread::formatDateTime(QString m, QString d, QString t)
{
    //    QDateTime::fromString("9月 24 2019 10:32:34", "MMM d yyyy hh:mm:ss");
    // default year =2019
    QLocale local(QLocale::English, QLocale::UnitedStates);

    QDate curdt = QDate::currentDate();

    QString tStr = QString("%1 %2 %3 %4").arg(m).arg(d).arg(curdt.year()).arg(t);
    QDateTime dt = local.toDateTime(tStr, "MMM d yyyy hh:mm:ss");
    return dt.toMSecsSinceEpoch();
}



void LogAuthThread::onFinished(int exitCode)
{
    Q_UNUSED(exitCode)

//    QProcess *process = dynamic_cast<QProcess *>(sender());
//    if (!process) {
//        return;
//    }
//    if (!process->isOpen()) {
//        return;
//    }
    if (m_type != KERN && m_type != BOOT) {
        return;
    }
    QByteArray byte =   m_process->readAllStandardOutput();
    QTextStream stream(&byte);
    QByteArray encode;
    stream.setCodec(encode);
    QString str = stream.readAll();
    QStringList l = str.split('\n');
    qDebug() << __FUNCTION__ << "byte" << byte.length();
    qDebug() << __FUNCTION__ << "str" << str.length();
    //  qDebug() << __FUNCTION__ << "str" << str;

//    emit cmdFinished(m_type, str);


}

void LogAuthThread::kernDataRecived()
{

}

//void LogAuthThread::onFinishedRead()
//{
//    QProcess *process = dynamic_cast<QProcess *>(sender());
//    QString str = QString(process->readAllStandardOutput());
//    QStringList l = str.split('\n');

//}
