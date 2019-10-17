#ifndef LOGFILEPARSER_H
#define LOGFILEPARSER_H

#include <QMap>
#include <QObject>
#include <QThread>
#include "logpasswordauth.h"
#include "structdef.h"

class LogFileParser : public QObject
{
    Q_OBJECT
public:
    explicit LogFileParser(QWidget *parent = nullptr);
    ~LogFileParser();

    void parseByJournal(QStringList arg = QStringList());
    void parseByDpkg(QList<LOG_MSG_DPKG> &dList, qint64 ms = 0);
    void parseByXlog(QStringList &xList);
    void parseByBoot(QList<LOG_MSG_BOOT> &bList);
    void parseByKern(QList<LOG_MSG_JOURNAL> &kList, qint64 ms = 0, QString opt = "");
    void parseByApp(QString path, QList<LOG_MSG_APPLICATOIN> &appList, int lv = 6, qint64 ms = 0);

    void createFile(QString output, int count);

signals:
    void dpkgFinished();
    void xlogFinished();
    void bootFinished();
    void kernFinished();
    void journalFinished(QList<LOG_MSG_JOURNAL>);
    void applicationFinished();

private:
    bool isErroCommand(QString str);
    qint64 formatDateTime(QString m, QString d, QString t);
signals:

public slots:
    void slot_journalFinished(QList<LOG_MSG_JOURNAL> list);

private:
    QThread m_thread;
    QString m_rootPasswd;
    LogPasswordAuth *m_logPasswdWgt;

    QMap<QString, QString> m_dateDict;
    QMap<QString, int> m_levelDict;  // example:warning=>4
};

#endif  // LOGFILEPARSER_H
