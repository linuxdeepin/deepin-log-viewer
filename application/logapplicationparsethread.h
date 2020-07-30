#ifndef LOGAPPLICATIONPARSETHREAD_H
#define LOGAPPLICATIONPARSETHREAD_H

#include <QMap>
#include <QObject>
#include <QThread>
#include <mutex>
#include "structdef.h"
class QProcess;
class LogApplicationParseThread : public QThread
{
    Q_OBJECT
public:
    explicit LogApplicationParseThread(QObject *parent = nullptr);

    static LogApplicationParseThread *instance()
    {
        LogApplicationParseThread *sin = m_instance.load();
        if (!sin) {
            std::lock_guard<std::mutex> lock(m_mutex);
            sin = m_instance.load();
            if (!sin) {
                sin = new LogApplicationParseThread();
                m_instance.store(sin);
            }
        }
        return sin;
    }
    ~LogApplicationParseThread() override;
    void setParam(QString path, int lv, qint64 ms);

signals:
    void appCmdFinished(QList<LOG_MSG_APPLICATOIN>);

public slots:
    void doWork();

    void onProcFinished(int ret);
    void stopProccess();

protected:
    void initMap();
    void initProccess();
    void run() override;

private:
    QString m_logPath;
    qint64 m_periorTime;

    int m_level;
    int padding {0};
    QProcess *m_process = nullptr;
    QMap<QString, int> m_levelDict;  // example:warning=>4

    QList<LOG_MSG_APPLICATOIN> m_appList;
    bool m_canRun = false;
    static std::atomic<LogApplicationParseThread *> m_instance;
    static std::mutex m_mutex;
};

#endif  // LOGAPPLICATIONPARSETHREAD_H
