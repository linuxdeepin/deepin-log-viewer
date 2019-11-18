#include "logauththread.h"
#include <QDebug>
#include <QProcess>

static LogAuthThread *INSTANCE = nullptr;
LogAuthThread::LogAuthThread(QObject *parent)
    : QThread(parent)
{
}

LogAuthThread::~LogAuthThread()
{
    this->wait();
    this->terminate();
}

LogAuthThread *LogAuthThread::instance()
{
    if (!INSTANCE) {
        INSTANCE = new LogAuthThread;
    }

    return INSTANCE;
}

void LogAuthThread::setParam(QStringList list)
{
    m_list = list;
}

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
    QProcess *proc = new QProcess;
    connect(proc, SIGNAL(finished(int)), this, SLOT(onFinished(int)));
    proc->setProcessChannelMode(QProcess::MergedChannels);
    proc->start("pkexec", m_list);
    proc->waitForFinished(-1);
}

void LogAuthThread::onFinished(int exitCode)
{
    Q_UNUSED(exitCode)

    QProcess *process = dynamic_cast<QProcess *>(sender());
    //    qDebug() << process->processId();
    //    qDebug() << exitCode << endl;
    emit cmdFinished(process->readAllStandardOutput());
    process->deleteLater();
}
