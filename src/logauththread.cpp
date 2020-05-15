#include "logauththread.h"
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
    default:
        break;
    }
}

#include <QFile>
void LogAuthThread::handleBoot()
{
    QFile file("/var/log/boot.log");  // add by Airy
    if (!file.exists())
        return;

    QProcess *proc = new QProcess;
    connect(proc, SIGNAL(finished(int)), this, SLOT(onFinished(int)));
    proc->setProcessChannelMode(QProcess::MergedChannels);
    proc->start("pkexec", QStringList() << "logViewerAuth"
                << "/var/log/boot.log");
    proc->waitForFinished(-1);
}

void LogAuthThread::handleKern()
{
    QFile file("/var/log/kern.log");  // add by Airy
    if (!file.exists())
        return;
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
