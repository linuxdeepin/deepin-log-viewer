#ifndef LOGAUTHTHREAD_H
#define LOGAUTHTHREAD_H
#include <QProcess>
#include <QThread>
#include <mutex>
#include "structdef.h"

class LogAuthThread : public QThread
{
    Q_OBJECT
public:
    LogAuthThread(QObject *parent = nullptr);
    ~LogAuthThread();

    static LogAuthThread *instance()
    {
        LogAuthThread *sin = m_instance.load();
        if (!sin) {
            std::lock_guard<std::mutex> lock(m_mutex);
            sin = m_instance.load();
            if (!sin) {
                sin = new LogAuthThread();
                m_instance.store(sin);
            }
        }
        return sin;
    }

    QString getStandardOutput();
    QString getStandardError();

    void setType(LOG_FLAG flag) { m_type = flag; }

protected:
    void run() override;

    void handleBoot();
    void handleKern();

signals:
    void cmdFinished(LOG_FLAG, QString output);

public slots:
    void onFinished(int exitCode);

private:
    QStringList m_list;
    QString m_output;
    QString m_error;
    LOG_FLAG m_type;

    static std::atomic<LogAuthThread *> m_instance;
    static std::mutex m_mutex;
};

#endif  // LOGAUTHTHREAD_H
