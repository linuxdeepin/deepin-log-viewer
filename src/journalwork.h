#ifndef JOURNALWORK_H
#define JOURNALWORK_H

#include <QMap>
#include <QObject>
#include "structdef.h"

class journalWork : public QObject
{
    Q_OBJECT
public:
    explicit journalWork(QStringList arg, QObject *parent = nullptr);
    ~journalWork();

signals:
    void journalFinished(QList<LOG_MSG_JOURNAL> list);

public slots:
    void doWork();

private:
    QString getDateTimeFromStamp(QString str);
    void initMap();
    QString i2str(int prio);

    QStringList m_arg;
    QList<LOG_MSG_JOURNAL> logList;
    QMap<int, QString> m_map;
};

#endif  // JOURNALWORK_H
