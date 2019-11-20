#include "logapplicationparsethread.h"
#include <DMessageBox>
#include <QDateTime>
#include <QDebug>
#include <QProcess>

DWIDGET_USE_NAMESPACE

LogApplicationParseThread::LogApplicationParseThread(QObject *parent)
    : QThread(parent)
{
    qRegisterMetaType<QList<LOG_MSG_APPLICATOIN> >("QList<LOG_MSG_APPLICATOIN>");

    //    m_thread.start();
    //    this->moveToThread(&m_thread);
}

LogApplicationParseThread::LogApplicationParseThread(QString path, int lv, qint64 ms,
                                                     QObject *parent)
    : QThread(parent)
{
    qRegisterMetaType<QList<LOG_MSG_APPLICATOIN> >("QList<LOG_MSG_APPLICATOIN>");

    m_logPath = path;
    m_level = lv;
    m_periorTime = ms;

    initMap();

    //    m_thread.start();
    //    this->moveToThread(&m_thread);
}

void LogApplicationParseThread::doWork()
{
    m_appList.clear();

    QProcess *proc = new QProcess;
    connect(proc, SIGNAL(finished(int)), this, SLOT(onProcFinished(int)));

    QStringList arg;
    arg << "-c" << QString("cat %1").arg(m_logPath);

    proc->start("/bin/bash", arg);
    proc->waitForFinished(-1);
}

void LogApplicationParseThread::onProcFinished(int ret)
{
    QProcess *proc = dynamic_cast<QProcess *>(sender());
    QString output = proc->readAllStandardOutput();
    proc->deleteLater();

    for (QString str : output.split('\n')) {
        LOG_MSG_APPLICATOIN msg;

        str.replace(QRegExp("\\s{2,}"), "");

        QStringList list = str.split("]", QString::SkipEmptyParts);
        if (list.count() < 3)
            continue;

        QString dateTime = list[0].split("[", QString::SkipEmptyParts)[0].trimmed();
        if (dateTime.contains(",")) {
            dateTime.replace(",", "");
        }
        //        if (dateTime.split(".").count() == 2) {
        //            dateTime = dateTime.split(".")[0];
        //        }
        qint64 dt = QDateTime::fromString(dateTime, "yyyy-MM-dd hh:mm:ss.zzz").toMSecsSinceEpoch();
        if (dt < m_periorTime)
            continue;
        msg.dateTime = dateTime;
        msg.level = list[0].split("[", QString::SkipEmptyParts)[1];

        if (m_level != LVALL) {
            if (m_levelDict.value(msg.level) != m_level)
                continue;
        }

        msg.src = list[1].split("[", QString::SkipEmptyParts)[1];

        if (list.count() >= 4) {
            msg.msg = list.mid(2).join("]");
        } else {
            msg.msg = list[2];
        }

        m_appList.insert(0, msg);
    }

    emit appCmdFinished(m_appList);
}

void LogApplicationParseThread::initMap()
{
    m_levelDict.insert("Warning", WARN);
    m_levelDict.insert("Debug", DEB);
}

void LogApplicationParseThread::run()
{
    doWork();
}
