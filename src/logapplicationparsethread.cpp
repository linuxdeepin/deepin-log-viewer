#include "logapplicationparsethread.h"
#include <DMessageBox>
#include <QDateTime>
#include <QDebug>
#include <QProcess>

DWIDGET_USE_NAMESPACE

std::atomic<LogApplicationParseThread *> LogApplicationParseThread::m_instance;
std::mutex LogApplicationParseThread::m_mutex;

LogApplicationParseThread::LogApplicationParseThread(QObject *parent)
    : QThread(parent)
{
    qRegisterMetaType<QList<LOG_MSG_APPLICATOIN> >("QList<LOG_MSG_APPLICATOIN>");

    initMap();
}

void LogApplicationParseThread::setParam(QString path, int lv, qint64 ms)
{
    m_logPath = path;
    m_level = lv;
    m_periorTime = ms;
}

void LogApplicationParseThread::doWork()
{
    m_appList.clear();

    QProcess *proc = new QProcess;
    connect(proc, SIGNAL(finished(int)), proc, SLOT(deleteLater()));
    if (m_logPath.isEmpty()) {  //modified by Airy for bug 20457::if path is empty,item is not empty
        emit appCmdFinished(m_appList);
    } else {
        QStringList arg;
        arg << "-c" << QString("cat %1").arg(m_logPath);

        proc->start("/bin/bash", arg);
        proc->waitForFinished(-1);

        QString output = proc->readAllStandardOutput();
        qDebug() << __FUNCTION__ << "---output.length()" << output.length() << "output.split('\n')" << output.split('\n').length();
        for (QString str : output.split('\n')) {
            LOG_MSG_APPLICATOIN msg;

            str.replace(QRegExp("\\s{2,}"), "");
            str.replace(QRegExp("\\x1B\\[\\d+(;\\d+){0,2}m"), "");

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

            // add by Airy
            // boot maker log can not show
            if (dateTime.split(".").count() > 2) {
                list[1] = " " + list[1];
                QDateTime g_time = QDateTime::fromString(dateTime, "yyyyMMdd.hh:mm:ss.zzz");
                QString gg_time = g_time.toString("yyyy-MM-dd hh:mm:ss.zzz");
                dateTime = gg_time;
            }  // add

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
    if (proc) {
        delete  proc;
    }

}

void LogApplicationParseThread::onProcFinished(int ret)
{
#if 0
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
#endif
}

void LogApplicationParseThread::initMap()
{
    m_levelDict.insert("Warning", WARN);
    m_levelDict.insert("Debug", DEB);
    m_levelDict.insert("Info", INF);
    m_levelDict.insert("Error", ERR);
}

void LogApplicationParseThread::run()
{
    doWork();
}
