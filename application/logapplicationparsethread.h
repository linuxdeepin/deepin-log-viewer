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
    void setParam(APP_FILTERS &iFilter);

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
    APP_FILTERS m_AppFiler;
    int padding {0};
    QProcess *m_process = nullptr;
    QMap<QString, int> m_levelDict;  // example:warning=>4

    QList<LOG_MSG_APPLICATOIN> m_appList;
    bool m_canRun = false;
    static std::atomic<LogApplicationParseThread *> m_instance;
    static std::mutex m_mutex;
};

#endif  // LOGAPPLICATIONPARSETHREAD_H
