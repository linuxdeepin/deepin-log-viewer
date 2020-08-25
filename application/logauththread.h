#ifndef LOGAUTHTHREAD_H
#define LOGAUTHTHREAD_H
#include <QProcess>
#include <QRunnable>
#include <mutex>
#include "structdef.h"

class LogAuthThread :  public QObject, public QRunnable
{
    Q_OBJECT
public:
    LogAuthThread(QObject *parent = nullptr);
    ~LogAuthThread() override;

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
    void setFileterParam(KWIN_FILTERS iFIlters) { m_kwinFilters = iFIlters; }
    void setFileterParam(XORG_FILTERS iFIlters) { m_xorgFilters = iFIlters; }
    void setFileterParam(DKPG_FILTERS iFIlters) { m_dkpgFilters = iFIlters; }
    void setFileterParam(KERN_FILTERS iFIlters) { m_kernFilters = iFIlters; }
    void stopProccess();

protected:
    void run() override;

    void handleBoot();
    void handleKern();
    void handleKwin();
    void handleXorg();
    void handleDkpg();
    void initProccess();
    qint64 formatDateTime(QString m, QString d, QString t);



signals:
    void kernFinished(QList<LOG_MSG_JOURNAL>);
    void bootFinished(QList<LOG_MSG_BOOT>);
    void kwinFinished(QList<LOG_MSG_KWIN> iKwinList);
    void xorgFinished(QList<LOG_MSG_XORG> iKwinList);
    void dpkgFinished(QList<LOG_MSG_DPKG> iKwinList);
    void proccessError(const QString &iError);
public slots:
    void onFinished(int exitCode);
private:
    QStringList m_list;
    QString m_output;
    QString m_error;
    LOG_FLAG m_type;
    KWIN_FILTERS m_kwinFilters;
    XORG_FILTERS m_xorgFilters;
    DKPG_FILTERS m_dkpgFilters;
    KERN_FILTERS m_kernFilters;
    static std::atomic<LogAuthThread *> m_instance;
    static std::mutex m_mutex;
    QProcess *m_process = nullptr;
    bool m_canRun = false;
};

#endif  // LOGAUTHTHREAD_H
