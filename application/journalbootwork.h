/*
 * Copyright (C) 2019 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     LZ <zhou.lu@archermind.com>
 *
 * Maintainer: LZ <zhou.lu@archermind.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef JOURNALBOOTWORK_H
#define JOURNALBOOTWORK_H

#include "structdef.h"

#include <QMap>
#include <QObject>
#include <QProcess>
#include <QRunnable>
#include <QMutexLocker>
#include <QEventLoop>
#include <QMutex>

#include <systemd/sd-journal.h>
#include <mutex>
/**
 * @brief The JournalBootWork class klu下启动日志获取线程,klu下没有/var/log/boot.log 所以使用journalctl -b 对应的系统接口获取,和系统日志类似
 */
class JournalBootWork :  public QObject, public QRunnable
{
    Q_OBJECT
public:
    explicit JournalBootWork(QStringList arg, QObject *parent = nullptr);
    explicit JournalBootWork(QObject *parent = nullptr);
    ~JournalBootWork();

    static JournalBootWork *instance()
    {
        JournalBootWork *sin = m_instance.load();
        if (!sin) {
            std::lock_guard<std::mutex> lock(m_mutex);
            sin = m_instance.load();
            if (!sin) {
                sin = new JournalBootWork();
                m_instance.store(sin);
            }
        }
        return sin;
    }


    void setArg(QStringList arg);
    void deleteSd();
    void run() override;

signals:
    /**
     * @brief journalBootFinished 获取数据结束
     */
    void journalBootFinished(int index);
    /**
     * @brief journaBootlData 把获取到的一部分数据传出去的信号
     * @param index 当前线程的数字标号
     * @param list 数据list
     */
    void journaBootlData(int index, QList<LOG_MSG_JOURNAL> list);
    /**
     * @brief journalBootError 获取失败信号
     * @param iError 失败信息
     */
    void journalBootError(QString &iError);

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
    QList<LOG_MSG_JOURNAL> logList;
    QMutex mutex;
    /**
     * @brief thread_index 静态成员变量，用来每次构造时标记新的当前线程对象 m_threadIndex
     */
    static int thread_index ;
private:
    QString getDateTimeFromStamp(QString str);
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
    static std::atomic<JournalBootWork *> m_instance;
    static std::mutex m_mutex;
    QEventLoop loop;
    /**
     * @brief m_canRun  是否允许标记量，用于停止该线程
     */
    bool m_canRun = false;
    /**
     * @brief m_threadIndex 当前线程标号
     */
    int m_threadIndex;

};

#endif  // JOURNALBOOTWORK_H
