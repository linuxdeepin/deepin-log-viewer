#ifndef LOGAPPLICATIONHELPER_H
#define LOGAPPLICATIONHELPER_H

#include <QMap>
#include <QObject>

#include <mutex>

class LogApplicationHelper : public QObject
{
    Q_OBJECT
public:
    static LogApplicationHelper *instance()
    {
        LogApplicationHelper *sin = m_instance.load();
        if (!sin) {
            std::lock_guard<std::mutex> lock(m_mutex);
            sin = m_instance.load();
            if (!sin) {
                sin = new LogApplicationHelper();
                m_instance.store(sin);
            }
        }
        return sin;
    }

    QMap<QString, QString> getMap();

    QString transName(QString str);

private:
    explicit LogApplicationHelper(QObject *parent = nullptr);

    void init();

    void createDesktopFiles();
    void createLogFiles();

    void parseField(QString path, QString name, bool isDeepin, bool isGeneric, bool isName);

    QString getLogFile(QString path);

signals:

public slots:

private:
    QMap<QString, QString> m_en_log_map;
    QMap<QString, QString> m_en_trans_map;

    QMap<QString, QString> m_trans_log_map;

    QStringList m_desktop_files;
    QStringList m_log_files;

    QString m_current_system_language;

    static std::atomic<LogApplicationHelper *> m_instance;
    static std::mutex m_mutex;
};

#endif  // LOGAPPLICATIONHELPER_H
