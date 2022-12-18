// SPDX-FileCopyrightText: 2019 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef LOGAPPLICATIONPARSETHREAD_H
#define LOGAPPLICATIONPARSETHREAD_H
#include "../structdef.h"

#include <QMap>
#include <QObject>
#include <QThread>

#include <mutex>

class QProcess;
/**
 * @brief The LogApplicationParseThread class 应用日志获取线程
 */
class LogApplicationParseThread : public QThread
{
    Q_OBJECT
public:
    explicit LogApplicationParseThread(QObject *parent = nullptr);
    ~LogApplicationParseThread() override;
    void setParam(APP_FILTERS &iFilter);
    static int thread_count;
signals:
    /**
     * @brief appFinished 获取数据结束信号
     */
    void appFinished(int index);
    void appData(int index, QList<LOG_MSG_APPLICATOIN> iDataList);
public slots:
    void doWork();

    void onProcFinished(int ret);
    void stopProccess();
    int getIndex();
protected:
    void initMap();
    void initProccess();
    void run() override;

private:
    /**
     * @brief m_AppFiler 筛选条件结构体
     */
    APP_FILTERS m_AppFiler;
    //获取数据用的cat命令的process
    QProcess *m_process = nullptr;
    /**
     * @brief m_levelDict example:warning=>4 等级字符串到等级数字的键值对
     */
    QMap<QString, int> m_levelDict;
    /**
     * @brief m_appList 获取的数据结果
     */
    QList<LOG_MSG_APPLICATOIN> m_appList;
    /**
     * @brief m_canRun 是否可以继续运行的标记量，用于停止运行线程
     */
    bool m_canRun = false;
    /**
     * @brief m_threadIndex 当前线程标号
     */
    int m_threadCount;
    /**
     * @brief totalSize 获取的数据总量
     */
    int totalSize = 0;
};

#endif  // LOGAPPLICATIONPARSETHREAD_H
