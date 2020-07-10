#include "logauththread.h"
#include "utils.h"
#include <QDebug>
#include <QDateTime>

std::atomic<LogAuthThread *> LogAuthThread::m_instance;
std::mutex LogAuthThread::m_mutex;

LogAuthThread::LogAuthThread(QObject *parent)
    :  QObject(parent),
       QRunnable()

{
    setAutoDelete(true);
}

LogAuthThread::~LogAuthThread()
{
    if (m_process) {
        m_process->kill();
        m_process->close();
        delete  m_process;
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
    if (m_process && m_process->isOpen()) {
        m_process->readAll();
        m_process->kill();
        //m_process->terminate();

        //  m_process->close();
        //m_process->waitForFinished(-1);
    }
}

void LogAuthThread::run()
{
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
}

#include <QFile>
void LogAuthThread::handleBoot()
{
    QFile file("/var/log/boot.log");  // add by Airy
    if (!file.exists()) {
        emit bootFinished(m_type, "");
        return;
    }
    initProccess();
    m_process->setProcessChannelMode(QProcess::MergedChannels);
    m_process->start("pkexec", QStringList() << "logViewerAuth"
                     << "/var/log/boot.log");
    m_process->waitForFinished(-1);
    QByteArray byte =   m_process->readAllStandardOutput();
    QTextStream stream(&byte);
    QByteArray encode;
    stream.setCodec(encode);
    QString str = stream.readAll();
    QStringList l = str.split('\n');
    qDebug() << __FUNCTION__ << "byte" << byte.length();
    qDebug() << __FUNCTION__ << "str" << str.length();
    emit bootFinished(m_type, str);
    //  qDebug() << __FUNCTION__ << "str" << str;
}

void LogAuthThread::handleKern()
{
    QFile file("/var/log/kern.log"); // add by Airy
    if (!file.exists()) {
        emit kernFinished(m_type, "");
        return;
    }
    initProccess();
    // connect(proc, &QProcess::readyRead, this, &LogAuthThread::onFinishedRead);
    m_process->setProcessChannelMode(QProcess::MergedChannels);
    m_process->start("pkexec", QStringList() << "logViewerAuth"
                     << "/var/log/kern.log");
    //proc->start("pkexec", QStringList() << "/bin/bash" << "-c" << QString("cat %1").arg("/var/log/kern.log"));
    // proc->start("pkexec", QStringList() << QString("cat") << QString("/var/log/kern.log"));
    m_process->waitForFinished(-1);
    QByteArray byte =   m_process->readAllStandardOutput();
    QTextStream stream(&byte);
    QByteArray encode;
    stream.setCodec(encode);
    QString str = stream.readAll();
    QStringList l = str.split('\n');
    qDebug() << __FUNCTION__ << "byte" << byte.length();
    qDebug() << __FUNCTION__ << "str" << str.length();
    emit kernFinished(m_type, str);
}

void LogAuthThread::handleKwin()
{
    QFile file(KWIN_TREE_DATA);
    QList<LOG_MSG_KWIN> kwinList;
    if (!file.exists()) {
        emit kwinFinished(kwinList);
        return;
    }
    QProcess proc ;
    proc.start("cat", QStringList() << KWIN_TREE_DATA);
    proc.waitForFinished(-1);
    QByteArray outByte = proc.readAllStandardOutput();
    QString output = Utils::replaceEmptyByteArray(outByte);
    QString filterMsgStr = m_kwinFilters.msg;
    QString filterMsgStr1 = m_kwinFilters.msg;
    for (QString str : output.split('\n')) {
//        if (!(filterMsgStr1.trimmed().isEmpty()) && !str.contains(filterMsgStr, Qt::CaseInsensitive)) {
//            continue;
//        }
        if (str.trimmed().isEmpty()) {
            continue;
        }
        LOG_MSG_KWIN kwinMsg;
        kwinMsg.msg = str;
        kwinList.insert(0, kwinMsg);
    }
    proc.close();
    emit kwinFinished(kwinList);
}

void LogAuthThread::handleXorg()
{
    QFile file("/var/log/Xorg.0.log");  // if not,maybe crash
    QFile startFile("/proc/uptime");
    QList<LOG_MSG_XORG> xList;
    if (!file.exists() || !startFile.exists()) {
        emit proccessError(tr("Log file is empty!"));
        emit xorgFinished(xList);
        return;
    }
    QString startStr = "";
    if (startFile.open(QFile::ReadOnly)) {

        startStr = QString(startFile.readLine());
        startFile.close();
    }

    qDebug() << "startStr" << startFile;
    startStr = startStr.split(" ").value(0, "");
    if (startStr.isEmpty()) {
        emit proccessError(tr("Log file is empty!"));
        emit xorgFinished(xList);
        return;
    }
    initProccess();
    m_process->start("cat /var/log/Xorg.0.log");  // file path is fixed. so write cmd direct
    m_process->waitForFinished(-1);
    QString errorStr(m_process->readAllStandardError());
    Utils::CommandErrorType commandErrorType = Utils::isErroCommand(errorStr);
    if (commandErrorType != Utils::NoError) {
        if (commandErrorType == Utils::PermissionError) {
            emit proccessError(errorStr + "\n" + "Please use 'sudo' run this application");
        } else if (commandErrorType == Utils::RetryError) {
            emit proccessError("The password is incorrect,please try again");
        }
        return;
    }
    QByteArray outByte = m_process->readAllStandardOutput();
    QString output = Utils::replaceEmptyByteArray(outByte);
    m_process->close();
    QDateTime curDt = QDateTime::currentDateTime();
    qint64 curDtSecond = curDt.toMSecsSinceEpoch() - static_cast<int>(startStr.toDouble() * 1000);

    for (QString str : output.split('\n')) {
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
            if (realDt.toMSecsSinceEpoch() < m_xorgFilters.timeFilter)  // add by Airy
                continue;

            LOG_MSG_XORG msg;
            msg.dateTime = realDt.toString("yyyy-MM-dd hh:mm:ss.zzz");
            msg.msg = msgInfo;

            xList.insert(0, msg);
        } else {
            if (xList.length() > 0) {
                xList[0].msg += str;
            }
        }
    }
    qDebug() << "xorg size" << xList.length();
    emit xorgFinished(xList);
}

void LogAuthThread::handleDkpg()
{

    QFile file("/var/log/dpkg.log");  // if not,maybe crash
    if (!file.exists())
        return;
    initProccess();
    m_process->start("cat /var/log/dpkg.log");  // file path is fixed. so write cmd direct
    m_process->waitForFinished(-1);
    QByteArray outByte = m_process->readAllStandardOutput();
    QString output = Utils::replaceEmptyByteArray(outByte);
    m_process->close();
    QList<LOG_MSG_DPKG> dList;
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
        if (dt.toMSecsSinceEpoch() < m_dkpgFilters.timeFilter)
            continue;
        dpkgLog.action = strList[2];
        dpkgLog.msg = info;

        //        dList.append(dpkgLog);
        dList.insert(0, dpkgLog);
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

//void LogAuthThread::onFinishedRead()
//{
//    QProcess *process = dynamic_cast<QProcess *>(sender());
//    QString str = QString(process->readAllStandardOutput());
//    QStringList l = str.split('\n');

//}
