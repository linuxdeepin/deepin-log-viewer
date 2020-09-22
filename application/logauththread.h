/*
* Copyright (C) 2019 ~ 2020 UnionTech Software Technology Co.,Ltd
*
* Author:     zyc <zyc@uniontech.com>
* Maintainer:  zyc <zyc@uniontech.com>
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* any later version.
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef LOGAUTHTHREAD_H
#define LOGAUTHTHREAD_H
#include <QProcess>
#include <QRunnable>
#include <QVector>
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
    static int thread_count;
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
    bool doReadFileWork();
    void close();
    void splitLines();
    void splitLine(QVector<qint64> *enters, char *ptr, bool progress);
    void detectTextCodec();
    void setCodec(QTextCodec *codec) {mCodec = codec;}
signals:
    void kernFinished(QList<LOG_MSG_JOURNAL>);
    void bootFinished(QList<LOG_MSG_BOOT>);
    void kwinFinished(QList<LOG_MSG_KWIN> iKwinList);
    void xorgFinished(QList<LOG_MSG_XORG> iKwinList);
    void dpkgFinished(QList<LOG_MSG_DPKG> iKwinList);
    void kernFinished(LOG_DATA_BASE_INFO *info);
    void proccessError(const QString &iError);
public slots:
    void onFinished(int exitCode);
    void kernDataRecived();
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
    int m_threadCount;

    bool m_isStopProccess = false;
    GET_FIILE_DATA_FLAG m_flag;
    char *mMem{nullptr};
    qint64 mSize{0};
    QVector<qint64> mEnters;
    int mLineCnt{0};
    QFile *mFile{nullptr};
    int mEnterCharOffset;
    QTextCodec *mCodec;
};

#endif  // LOGAUTHTHREAD_H
