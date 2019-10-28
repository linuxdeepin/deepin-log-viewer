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

    m_logPasswdWgt = new LogPasswordAuth;
}

LogFileParser::~LogFileParser()
{
    m_thread.quit();
    m_thread.wait();
}

void LogFileParser::parseByJournal(QStringList arg)
{
    journalWork *work = new journalWork(arg);
    work->moveToThread(&m_thread);
    //    connect(work, SIGNAL(journalFinished(QList<LOG_MSG_JOURNAL>)), this,
    //            SLOT(slot_journalFinished(QList<LOG_MSG_JOURNAL>)));
    connect(work, &journalWork::journalFinished, this, [=](QList<LOG_MSG_JOURNAL> list) {
        emit journalFinished(list);
        //        work->deleteLater();
        work->deleteLater();
        m_thread.quit();
        m_thread.wait();
    });

    m_thread.start();
    QMetaObject::invokeMethod(work, "doWork", Qt::AutoConnection);
}

void LogFileParser::parseByDpkg(QList<LOG_MSG_DPKG> &dList, qint64 ms)
{
    QProcess proc;
    proc.start("cat /var/log/dpkg.log");  // file path is fixed. so write cmd direct
    proc.waitForFinished();
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

        dList.append(dpkgLog);
    }

    createFile(output, dList.count());
    emit dpkgFinished();
}

void LogFileParser::parseByXlog(QStringList &xList)
{
    QProcess proc;
    proc.start("cat /var/log/Xorg.0.log");  // file path is fixed. so write cmd direct
    proc.waitForFinished();

    if (isErroCommand(QString(proc.readAllStandardError())))
        return;

    QString output = proc.readAllStandardOutput();
    proc.close();

    for (QString str : output.split('\n')) {
        if (str.startsWith("[")) {
            xList.append(str);
        } else {
            str += " ";
            xList[xList.size() - 1] += str;
        }
    }
    createFile(output, xList.count());

    emit xlogFinished();
}

void LogFileParser::parseByBoot(QList<LOG_MSG_BOOT> &bList)
{
#ifndef USE_POLKIT
    if (m_rootPasswd.isEmpty()) {
        m_logPasswdWgt->exec();
        m_rootPasswd = m_logPasswdWgt->getPasswd();
    }
    if (m_rootPasswd.isEmpty())
        return;

    QProcess proc;
    QStringList arg;
    arg << "-c" << QString("echo '%1' | sudo -S cat /var/log/boot.log").arg(m_rootPasswd);

    proc.start("/bin/bash", arg);  // file path is fixed. So write cmd direct
    proc.waitForFinished();
#else
    QProcess proc;
    QStringList arg;
    //    arg << "/bin/bash"
    arg << "bash"
        << "-c" << QString("cat /var/log/boot.log");
    proc.start("pkexec", arg);  // file path is fixed. So write cmd direct
    proc.waitForFinished();
#endif

    if (isErroCommand(QString(proc.readAllStandardError())))
        return;

    QString output = proc.readAllStandardOutput();
    proc.close();

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
            for (int i = 3; i < retList.size(); i++) {
                bMsg.msg = bMsg.msg + retList[i] + " ";
            }
            bList.append(bMsg);
        } else {
            if (bList.size() == 0)
                continue;
            for (int i = 0; i < retList.size(); ++i) {
                bList[bList.size() - 1].msg += retList[i] + " ";
            }
        }
    }

    createFile(output, bList.count());
    emit bootFinished();
}

void LogFileParser::parseByKern(QList<LOG_MSG_JOURNAL> &kList, qint64 ms)
{
    // opt ==> "| grep "ERR"
#ifndef USE_POLKIT
    if (m_rootPasswd.isEmpty()) {
        m_logPasswdWgt->exec();
        m_rootPasswd = m_logPasswdWgt->getPasswd();
    }
    if (m_rootPasswd.isEmpty())
        return;

    QProcess proc;
    QStringList arg;

    arg << "-c" << QString("echo '%1' | sudo -S cat /var/log/kern.log").arg(m_rootPasswd);

    proc.start("/bin/bash", arg);
    proc.waitForFinished();
#else
    QProcess proc;
    QStringList arg;
    //    arg << "/bin/bash"
    arg << "bash"
        << "-c" << QString("cat /var/log/kern.log");

    proc.start("pkexec", arg);
    proc.waitForFinished();
#endif

    if (isErroCommand(QString(proc.readAllStandardError())))
        return;

    QString output = proc.readAllStandardOutput();
    proc.close();

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
        if (iTime < ms)
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

        kList.append(msg);
    }

    createFile(output, kList.count());
    emit kernFinished();
}

void LogFileParser::parseByApp(QString path, QList<LOG_MSG_APPLICATOIN> &appList, int lv, qint64 ms)
{
    QProcess proc;
    QStringList arg;
    arg << "-c" << QString("cat %1").arg(path);

    proc.start("/bin/bash", arg);
    proc.waitForFinished();

    if (isErroCommand(QString(proc.readAllStandardError())))
        return;

    QString output = proc.readAllStandardOutput();
    proc.close();

    for (QString str : output.split('\n')) {
        LOG_MSG_APPLICATOIN msg;

        str.replace(QRegExp("\\s{2,}"), "");

        QStringList list = str.split("]", QString::SkipEmptyParts);
        if (list.count() < 3)
            continue;

        QString dateTime = list[0].split("[", QString::SkipEmptyParts)[0].trimmed();
        qint64 dt = QDateTime::fromString(dateTime, "yyyy-MM-dd, hh:mm:ss.zzz").toMSecsSinceEpoch();
        if (dt < ms)
            continue;
        msg.dateTime = dateTime;
        msg.level = list[0].split("[", QString::SkipEmptyParts)[1];
        if (m_levelDict.value(msg.level) != lv)
            continue;
        msg.src = list[1].split("[", QString::SkipEmptyParts)[1];
        msg.msg = list[2];

        appList.append(msg);
    }

    createFile(output, appList.count());
    emit applicationFinished();
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
    QString month = m_dateDict.value(m);
    QString tStr = QString("%1 %2 2019 %3").arg(month).arg(d).arg(t);
    QDateTime dt = QDateTime::fromString(tStr, "MMM d yyyy hh:mm:ss");
    return dt.toMSecsSinceEpoch();
}

void LogFileParser::slot_journalFinished(QList<LOG_MSG_JOURNAL> list)
{
    emit journalFinished(list);
}
