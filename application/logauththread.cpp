#include "logauththread.h"
#include "utils.h"
#include <QDebug>

std::atomic<LogAuthThread *> LogAuthThread::m_instance;
std::mutex LogAuthThread::m_mutex;

LogAuthThread::LogAuthThread(QObject *parent)
    : QThread(parent)
{
}

LogAuthThread::~LogAuthThread() {}

QString LogAuthThread::getStandardOutput()
{
    return m_output;
}

QString LogAuthThread::getStandardError()
{
    return m_error;
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
    default:
        break;
    }
}

#include <QFile>
void LogAuthThread::handleBoot()
{
    QFile file("/var/log/boot.log");  // add by Airy
    if (!file.exists()) {
        emit cmdFinished(m_type, "");
        return;
    }
    QProcess *proc = new QProcess;
    connect(proc, SIGNAL(finished(int)), this, SLOT(onFinished(int)));
    proc->setProcessChannelMode(QProcess::MergedChannels);
    proc->start("pkexec", QStringList() << "logViewerAuth"
                << "/var/log/boot.log");
    proc->waitForFinished(-1);
}

void LogAuthThread::handleKern()
{
    QFile file("/var/log/kern.log"); // add by Airy
    if (!file.exists()) {
        emit cmdFinished(m_type, "");
        return;
    }
    QProcess *proc = new QProcess;
    connect(proc, SIGNAL(finished(int)), this, SLOT(onFinished(int)));
    // connect(proc, &QProcess::readyRead, this, &LogAuthThread::onFinishedRead);
    proc->setProcessChannelMode(QProcess::MergedChannels);
    proc->start("pkexec", QStringList() << "logViewerAuth"
                << "/var/log/kern.log");
    //proc->start("pkexec", QStringList() << "/bin/bash" << "-c" << QString("cat %1").arg("/var/log/kern.log"));
    // proc->start("pkexec", QStringList() << QString("cat") << QString("/var/log/kern.log"));
    proc->waitForFinished(-1);
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

void LogAuthThread::onFinished(int exitCode)
{
    Q_UNUSED(exitCode)

    QProcess *process = dynamic_cast<QProcess *>(sender());
    QByteArray byte =   process->readAllStandardOutput();
    QTextStream stream(&byte);
    QByteArray encode;
    stream.setCodec(encode);
    QString str = stream.readAll();
    QStringList l = str.split('\n');
    qDebug() << __FUNCTION__ << "byte" << byte.length();
    qDebug() << __FUNCTION__ << "str" << str.length();
    //  qDebug() << __FUNCTION__ << "str" << str;
    emit cmdFinished(m_type, str);
    process->deleteLater();
}

void LogAuthThread::onFinishedRead()
{
    QProcess *process = dynamic_cast<QProcess *>(sender());
    QString str = QString(process->readAllStandardOutput());
    QStringList l = str.split('\n');

    process->deleteLater();
}
