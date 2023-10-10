// SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef JOURNALAPPWORK_H
#define JOURNALAPPWORK_H

#include "structdef.h"

#include <QMap>
#include <QObject>
#include <QRunnable>
#include <QMutex>

#include <mutex>
#include <systemd/sd-journal.h>
/**
 * @brief The journalAppWork class jouranl应用日志获取进程
 */
class JournalAppWork :  public QObject, public QRunnable
{
    Q_OBJECT

public:
    explicit JournalAppWork(QStringList arg, QObject *parent = nullptr);
    explicit JournalAppWork(QObject *parent = nullptr);
    ~JournalAppWork();

    void setArg(QStringList arg);
    void run() override;

signals:
    /**
     * @brief journalData 把获取到的一部分数据传出去的信号
     * @param index 当前线程的数字标号
     * @param list 数据list
     */
    void journalAppData(int index, QList<LOG_MSG_APPLICATOIN> list);
    /**
     * @brief journalFinished 获取数据结束
     */
    void journalAppFinished(int index);

public slots:
    void doWork();
    QString getReplaceColorStr(const char *d);
    void stopWork();
    int getIndex();
    static int getPublicIndex();
public:
    /**
     * @brief logList 每次发出的数据list
     */
    QList<LOG_MSG_APPLICATOIN> logList;
    QMutex mutex;
    /**
     * @brief thread_index 静态成员变量，用来每次构造时标记新的当前线程对象 m_threadIndex
     */
    static int thread_index ;
private:
    QString getDateTimeFromStamp(const QString &str);
    void initMap();
    QString i2str(int prio);
    /**
     * @brief m_arg 获取数据筛选参数
     */
    QStringList m_arg;
    /**
     * @brief m_map 等级数字对应字符串
     */
    QMap<int, QString> m_map;
    static std::mutex m_mutex;
    /**
     * @brief m_canRun  是否允许标记量，用于停止该线程
     */
    std::atomic_bool m_canRun = false;
    /**
     * @brief m_threadIndex 当前线程标号
     */
    int m_threadIndex;


};

#endif  // journalAppWork_H
