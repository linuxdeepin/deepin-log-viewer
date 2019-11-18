#ifndef LOGAUTHTHREAD_H
#define LOGAUTHTHREAD_H
#include <QThread>

class LogAuthThread : public QThread
{
    Q_OBJECT
public:
    LogAuthThread(QObject *parent = nullptr);
    ~LogAuthThread();

    static LogAuthThread *instance();

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
};

#endif  // LOGAUTHTHREAD_H
