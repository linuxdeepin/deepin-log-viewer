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
    proc->setProcessChannelMode(QProcess::MergedChannels);
    proc->start("pkexec", QStringList() << "logViewerAuth"
                                        << "/var/log/kern.log");
    proc->waitForFinished(-1);
}

void LogAuthThread::onFinished(int exitCode)
{
    Q_UNUSED(exitCode)

    QProcess *process = dynamic_cast<QProcess *>(sender());
    emit cmdFinished(m_type, process->readAllStandardOutput());
    process->deleteLater();
}
