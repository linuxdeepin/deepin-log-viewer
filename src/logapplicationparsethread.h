#ifndef LOGAPPLICATIONPARSETHREAD_H
#define LOGAPPLICATIONPARSETHREAD_H

#include <QMap>
#include <QObject>
#include <QThread>
#include "structdef.h"

class LogApplicationParseThread : public QThread
{
    Q_OBJECT
public:
    explicit LogApplicationParseThread(QObject *parent = nullptr);
    explicit LogApplicationParseThread(QString path, int lv, qint64 ms, QObject *parent = nullptr);

signals:
    void appCmdFinished(QList<LOG_MSG_APPLICATOIN>);

public slots:
    void doWork();

    void onProcFinished(int ret);

protected:
    void initMap();

    void run() override;

private:
    QThread m_thread;

    QString m_logPath;
    int m_level;
    qint64 m_periorTime;

    QMap<QString, int> m_levelDict;  // example:warning=>4

    QList<LOG_MSG_APPLICATOIN> m_appList;
};

#endif  // LOGAPPLICATIONPARSETHREAD_H
