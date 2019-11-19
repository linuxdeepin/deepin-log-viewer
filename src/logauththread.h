#ifndef LOGAUTHTHREAD_H
#define LOGAUTHTHREAD_H
#include <QProcess>
#include <QThread>
#include "structdef.h"

class LogAuthThread : public QThread
{
    Q_OBJECT
public:
    LogAuthThread(QObject *parent = nullptr);
    ~LogAuthThread();

    static LogAuthThread *instance();

    void setType(LOG_FLAG type);
    void setParam(QStringList list);

    QString getStandardOutput();
    QString getStandardError();

protected:
    void run() override;

signals:
    void cmdFinished(QString output);

public slots:
    void onFinished(int exitCode);

private:
    QStringList m_list;
    QString m_output;
    QString m_error;
    LOG_FLAG m_type;
    QProcess *proc {nullptr};
};

#endif  // LOGAUTHTHREAD_H
