#include "logapplicationparsethread.h"
#include "utils.h"
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

LogApplicationParseThread::~LogApplicationParseThread()
{
    if (m_process) {
        m_process->kill();
        m_process->close();
        delete  m_process;
        m_process = nullptr;
    }
}

void LogApplicationParseThread::setParam(APP_FILTERS &iFilter)
{
    m_AppFiler = iFilter;
}

void LogApplicationParseThread::stopProccess()
{
    m_canRun = false;
    if (m_process && m_process->isOpen()) {
        // m_process->readAll();
        m_process->kill();
    }
}

void LogApplicationParseThread::doWork()
{
    m_canRun = true;
    m_appList.clear();
    initProccess();

    //connect(m_process, SIGNAL(finished(int)), m_process, SLOT(deleteLater()));
    if (m_AppFiler.path.isEmpty()) {  //modified by Airy for bug 20457::if path is empty,item is not empty
        emit appCmdFinished(m_appList);
    } else {
        QStringList arg;
        arg << "-c" << QString("cat %1").arg(m_AppFiler.path);

        m_process->start("/bin/bash", arg);
        m_process->waitForFinished(-1);
        QByteArray byteOutput = m_process->readAllStandardOutput();
        m_process->close();
        if (!m_canRun) {
            return;
        }
        QString output(Utils::replaceEmptyByteArray(byteOutput));
        for (QString str : output.split('\n')) {
            if (!m_canRun) {
                return;
            }
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
            if (m_AppFiler.timeFilterBegin > 0 && m_AppFiler.timeFilterEnd > 0) {
                if (dt < m_AppFiler.timeFilterBegin || dt > m_AppFiler.timeFilterEnd)
                    continue;
            }

            msg.dateTime = dateTime;
            msg.level = list[0].split("[", QString::SkipEmptyParts)[1];

            if (m_AppFiler.lvlFilter != LVALL) {
                if (m_levelDict.value(msg.level) != m_AppFiler.lvlFilter)
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
        if (!m_canRun) {
            return;
        }
        emit appCmdFinished(m_appList);
    }


}


void LogApplicationParseThread::onProcFinished(int ret)
{
    Q_UNUSED(ret)
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

void LogApplicationParseThread::initProccess()
{
    if (!m_process) {
        m_process = new QProcess;
    }
}

void LogApplicationParseThread::run()
{
    doWork();
}
