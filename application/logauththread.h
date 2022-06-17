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
#include "structdef.h"

#include <QProcess>
#include <QRunnable>
#include <QMap>

#include <mutex>
/**
 * @brief The LogAuthThread class 启动日志 内核日志 kwin日志 xorg日志 dpkg日志获取线程
 */
class LogAuthThread :  public QObject, public QRunnable
{
    Q_OBJECT
public:
    explicit LogAuthThread(QObject *parent = nullptr);
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
    void initDnfLevelMap();
    void initLevelMap();
    QString getStandardOutput();
    QString getStandardError();
    void setType(LOG_FLAG flag) { m_type = flag; }
    void setFileterParam(KWIN_FILTERS iFIlters) { m_kwinFilters = iFIlters; }
    void setFileterParam(XORG_FILTERS iFIlters) { m_xorgFilters = iFIlters; }
    void setFileterParam(DKPG_FILTERS iFIlters) { m_dkpgFilters = iFIlters; }
    void setFileterParam(KERN_FILTERS iFIlters) { m_kernFilters = iFIlters; }
    void setFileterParam(NORMAL_FILTERS iFIlters) { m_normalFilters = iFIlters; }
    void setFileterParam(DNF_FILTERS iFIlters) { m_dnfFilters = iFIlters; }
    void setFileterParam(DMESG_FILTERS iFIlters) { m_dmesgFilters = iFIlters; }
    void stopProccess();
    void setFilePath(const QStringList &filePath);
    int getIndex();
    QString startTime();
    /**
     * @brief thread_index 静态成员变量，用来每次构造时标记新的当前线程对象 m_threadIndex
     */
    static int thread_count;
protected:
    void run() override;

    void handleBoot();
    void handleKern();
    void handleKwin();
    void handleXorg();
    void handleDkpg();
    void handleNormal();
    /**
     * @brief NormalInfoTime 开关机事件信息段解析
     */
    void NormalInfoTime();
    void handleDnf();
    void handleDmesg();
    void initProccess();
    qint64 formatDateTime(QString m, QString d, QString t);
    qint64 formatDateTime(QString y, QString t);

signals:
    void kernFinished(int index);
    /**
     * @brief kernData
     * @param index 当前线程的数字标号
     * @param iDataList　数据list
     */
    void kernData(int index, QList<LOG_MSG_JOURNAL> iDataList);
    void bootFinished(int index);
    /**
     * @brief bootData
     * @param index 当前线程的数字标号
     * @param iDataList　数据list
     */
    void bootData(int index, QList<LOG_MSG_BOOT> iDataList);
    void kwinFinished(int index);
    void kwinData(int index, QList<LOG_MSG_KWIN> iDataList);
    void xorgFinished(int index);
    void xorgData(int index, QList<LOG_MSG_XORG> iDataList);
    void dpkgFinished(int index);
    void dpkgData(int index, QList<LOG_MSG_DPKG> iDataList);
    void normalFinished(int index);
    void normalData(int index, QList<LOG_MSG_NORMAL> iDataList);
    void dnfFinished(QList<LOG_MSG_DNF> iKwinList);
    void dmesgFinished(QList<LOG_MSG_DMESG> iKwinList);
    void proccessError(const QString &iError);
public slots:
    //    void onFinished(int exitCode);
    //    void kernDataRecived();
private:

    QStringList m_list;
    QString m_output;
    QString m_error;
    /**
     * @brief m_type 当前线程获取日志数据的类型，用来指定不同的获取逻辑和返回结果
     */
    LOG_FLAG m_type;
    /**
     * @brief m_kwinFilters kwin日志筛选条件
     */
    KWIN_FILTERS m_kwinFilters;
    /**
     * @brief m_xorgFilters xorg日志筛选条件
     */
    XORG_FILTERS m_xorgFilters;
    /**
     * @brief m_dkpgFilters dpkg日志筛选条件
     */
    DKPG_FILTERS m_dkpgFilters;
    /**
     * @brief m_kernFilters 内核日志筛选条件
     */
    KERN_FILTERS m_kernFilters;
    /**
     * @brief m_normalFilters 开关机日志筛选条件
     */
    NORMAL_FILTERS  m_normalFilters;
    DNF_FILTERS m_dnfFilters;
    DMESG_FILTERS m_dmesgFilters;
    static std::atomic<LogAuthThread *> m_instance;
    static std::mutex m_mutex;
    //获取数据用的cat命令的process
    QScopedPointer<QProcess> m_process;
    /**
     * @brief m_canRun 是否可以继续运行的标记量，用于停止运行线程
     */
    bool m_canRun = false;
    /**
     * @brief m_threadIndex 当前线程标号
     */
    int m_threadCount;
    //正在执行停止进程的变量，防止重复执行停止逻辑
    bool m_isStopProccess = false;
    //日志显示时间(毫秒)
    qint64 iTime;
    //所有日志文件路径
    QStringList m_FilePath;
    QMap<int, QString> m_levelMap;
    QMap<QString, int> m_dnfLevelDict;
    QMap<QString, QString> m_transDnfDict;
    QStringList TimeList;
};

#endif  // LOGAUTHTHREAD_H
