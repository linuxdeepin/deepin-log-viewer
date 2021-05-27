/*
* Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
*
* Author:     zyc <zyc@uniontech.com>
*
* Maintainer:  zyc <zyc@uniontech.com>
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

#include "logauththread.h"
#include "utils.h"
#include "sharedmemorymanager.h"
#include "sys/utsname.h"
#include <wtmpparse.h>
#include <DGuiApplicationHelper>
#include "dbusproxy/dldbushandler.h"

#include <DApplication>

#include <QDebug>
#include <QDateTime>
#include <time.h>
#include <utmp.h>
#include <utmpx.h>

DGUI_USE_NAMESPACE
std::atomic<LogAuthThread *> LogAuthThread::m_instance;
std::mutex LogAuthThread::m_mutex;
int LogAuthThread::thread_count = 0;

/**
 * @brief LogAuthThread::LogAuthThread 构造函数
 * @param parent 父对象
 */
LogAuthThread::LogAuthThread(QObject *parent)
    :  QObject(parent)
    , QRunnable()
    , m_type(NONE)

{
    //使用线程池启动该线程，跑完自己删自己
    setAutoDelete(true);
    //静态计数变量加一并赋值给本对象的成员变量，以供外部判断是否为最新线程发出的数据信号
    thread_count++;
    m_threadCount = thread_count;
    initDnfLevelMap();
    initLevelMap();
}

/**
 * @brief LogAuthThread::~LogAuthThread 析构函数，停止并销毁process指针
 */
LogAuthThread::~LogAuthThread()
{
    stopProccess();
}
void LogAuthThread::initDnfLevelMap()
{
    m_dnfLevelDict.insert("TRACE", TRACE);
    m_dnfLevelDict.insert("SUBDEBUG", DEBUG);
    m_dnfLevelDict.insert("DDEBUG", DEBUG);
    m_dnfLevelDict.insert("DEBUG", DEBUG);
    m_dnfLevelDict.insert("INFO", INFO);
    m_dnfLevelDict.insert("WARNING", WARNING);
    m_dnfLevelDict.insert("ERROR", ERROR);
    m_dnfLevelDict.insert("CRITICAL", CRITICAL);
    m_dnfLevelDict.insert("SUPERCRITICAL", SUPERCRITICAL);

    m_transDnfDict.insert("TRACE", Dtk::Widget::DApplication::translate("Level", "Trace"));
    m_transDnfDict.insert("SUBDEBUG", Dtk::Widget::DApplication::translate("Level", "Debug"));
    m_transDnfDict.insert("DDEBUG", Dtk::Widget::DApplication::translate("Level", "Debug"));
    m_transDnfDict.insert("DEBUG", Dtk::Widget::DApplication::translate("Level", "Debug"));
    m_transDnfDict.insert("INFO", Dtk::Widget::DApplication::translate("Level", "Info"));
    m_transDnfDict.insert("WARNING", Dtk::Widget::DApplication::translate("Level", "Warning"));
    m_transDnfDict.insert("ERROR", Dtk::Widget::DApplication::translate("Level", "Error"));
    m_transDnfDict.insert("CRITICAL", Dtk::Widget::DApplication::translate("Level", "Critical"));
    m_transDnfDict.insert("SUPERCRITICAL", Dtk::Widget::DApplication::translate("Level", "Super critical"));
}

void LogAuthThread::initLevelMap()
{
    m_levelMap.clear();
    m_levelMap.insert(0, Dtk::Widget::DApplication::translate("Level", "Emergency"));
    m_levelMap.insert(1, Dtk::Widget::DApplication::translate("Level", "Alert"));
    m_levelMap.insert(2, Dtk::Widget::DApplication::translate("Level", "Critical"));
    m_levelMap.insert(3, Dtk::Widget::DApplication::translate("Level", "Error"));
    m_levelMap.insert(4, Dtk::Widget::DApplication::translate("Level", "Warning"));
    m_levelMap.insert(5, Dtk::Widget::DApplication::translate("Level", "Notice"));
    m_levelMap.insert(6, Dtk::Widget::DApplication::translate("Level", "Info"));
    m_levelMap.insert(7, Dtk::Widget::DApplication::translate("Level", "Debug"));
}

/**
 * @brief LogAuthThread::stopProccess 停止日志数据获取进程并销毁
 */
void LogAuthThread::stopProccess()
{
    //防止正在执行时重复执行
    if (m_isStopProccess) {
        return;
    }
    m_isStopProccess = true;
    //停止获取线程执行，标记量置false
    m_canRun = false;
    //共享内存数据结构，用于和获取进程共享内存，数据为是否可执行进程，用于控制数据获取进程停止，因为这里会出现需要提权执行的进程，主进程没有权限停止提权进程，所以使用共享内存变量标记量控制提权进程停止
    ShareMemoryInfo   shareInfo ;
    //设置进程为不可执行
    shareInfo.isStart = false;
    //把数据付给共享内存中对应的变量
    SharedMemoryManager::instance()->setRunnableTag(shareInfo);
    if (m_process) {
        m_process->kill();

    }
}

void LogAuthThread::setFilePath(const QStringList &filePath)
{
    m_FilePath = filePath;
}

int LogAuthThread::getIndex()
{
    return  m_threadCount;
}
QString LogAuthThread::startTime()
{
    QString startStr = "";
    QFile startFile("/proc/uptime");
    if (!startFile.exists()) {
        return "";
    }
    if (startFile.open(QFile::ReadOnly)) {
        startStr = QString(startFile.readLine());
        startFile.close();
    }

    qDebug() << "startStr" << startFile;
    startStr = startStr.split(" ").value(0, "");
    if (startStr.isEmpty()) {
        return "";
    }
    return startStr;
}

/**
 * @brief LogAuthThread::run 线程执行虚函数
 */
void LogAuthThread::run()
{
    //此线程刚开始把可以继续变量置true，不然下面没法跑
    m_canRun = true;
    //根据类型成员变量执行对应日志的获取逻辑
    switch (m_type) {
    case KERN:
        handleKern();
        break;
    case BOOT:
        handleBoot();
        break;
    case Kwin:
        handleKwin();
        break;
    case XORG:
        handleXorg();
        break;
    case DPKG:
        handleDkpg();
        break;
    case Normal:
        handleNormal();
        break;
    case Dnf:
        handleDnf();
        break;
    case Dmesg:
        handleDmesg();
        break;
    default:
        break;
    }

    m_canRun = false;
}

#include <QFile>
/**
 * @brief LogAuthThread::handleBoot 处理启动日志
 */
void LogAuthThread::handleBoot()
{
    QList<LOG_MSG_BOOT> bList;
    for (int i = 0; i < m_FilePath.count(); i++) {
        if (!m_FilePath.at(i).contains("txt")) {
            QFile file(m_FilePath.at(i)); // add by Airy
            if (!file.exists()) {
                emit bootFinished(m_threadCount);
                return;
            }
        }
        if (!m_canRun) {
            return;
        }
        initProccess();
        m_process->setProcessChannelMode(QProcess::MergedChannels);
        //共享内存对应变量置true，允许进程内部逻辑运行
        ShareMemoryInfo shareInfo;
        shareInfo.isStart = true;
        SharedMemoryManager::instance()->setRunnableTag(shareInfo);
        //启动日志需要提权获取，运行的时候把对应共享内存的名称传进去，方便获取进程拿标记量判断是否继续运行
        m_process->start("pkexec", QStringList() << "logViewerAuth"
                                                 << m_FilePath.at(i) << SharedMemoryManager::instance()->getRunnableKey());
        m_process->waitForFinished(-1);
        if (m_process->exitCode() != 0) {
            emit bootFinished(m_threadCount);
            return;
        }
        QByteArray byte = m_process->readAllStandardOutput();
        QStringList strList = QString(Utils::replaceEmptyByteArray(byte)).split('\n', QString::SkipEmptyParts);

        //按换行分割
        //    qInfo()<<strList.size()<<"_________________________";
        for (int j = strList.size() - 1; j >= 0; --j) {
            QString lineStr = strList.at(j);
            if (lineStr.startsWith("/dev") || lineStr.isEmpty())
                continue;
            //删除颜色格式字符
            lineStr.replace(QRegExp("\\#033\\[\\d+(;\\d+){0,2}m"), "");
            // remove Useless characters
            lineStr.replace(QRegExp("\\x1B\\[\\d+(;\\d+){0,2}m"), "");
            Utils::replaceColorfulFont(&lineStr);
            QStringList retList;
            LOG_MSG_BOOT bMsg;
            retList = lineStr.split(" ", QString::SkipEmptyParts);
            if (retList.size() == 1) {
                bMsg.msg = lineStr.trimmed();
                bList.append(bMsg);
            } else {
                if (retList[1].compare("OK", Qt::CaseInsensitive) == 0 || retList[1].compare("Failed", Qt::CaseInsensitive) == 0) {
                    //如果是以ok/failed开头，则认为是一条记录的开头，否则认为此行为上一条日志的信息体的后续
                    bMsg.status = retList[1];
                    QStringList leftList = retList.mid(3);
                    bMsg.msg += leftList.join(" ");
                    bList.append(bMsg);
                } else {
                    bMsg.msg = lineStr.trimmed();
                    bList.append(bMsg);
                }
            }

            //每获得500个数据就发出信号给控件加载
            if (bList.count() % SINGLE_READ_CNT == 0) {
                emit bootData(m_threadCount, bList);
                bList.clear();
            }
        }
    }
    //最后可能有余下不足500的数据
    if (bList.count() >= 0) {
        emit bootData(m_threadCount, bList);
    }
    emit bootFinished(m_threadCount);
}

/**
 * @brief LogAuthThread::handleKern 处理获取内核日志
 */
void LogAuthThread::handleKern()
{
    QList<LOG_MSG_JOURNAL> kList;
    for (int i = 0; i < m_FilePath.count(); i++) {
        if (!m_FilePath.at(i).contains("txt")) {
            QFile file(m_FilePath.at(i)); // add by Airy
            if (!file.exists()) {
                emit kernFinished(m_threadCount);
                return;
            }
        }
        if (!m_canRun) {
            return;
        }
        initProccess();
        if (!m_canRun) {
            return;
        }
        m_process->setProcessChannelMode(QProcess::MergedChannels);
        if (!m_canRun) {
            return;
        }
        //共享内存对应变量置true，允许进程内部逻辑运行
        ShareMemoryInfo shareInfo;
        shareInfo.isStart = true;
        SharedMemoryManager::instance()->setRunnableTag(shareInfo);
        //启动日志需要提权获取，运行的时候把对应共享内存的名称传进去，方便获取进程拿标记量判断是否继续运行
        m_process->start("pkexec", QStringList() << "logViewerAuth"
                                                 << m_FilePath.at(i) << SharedMemoryManager::instance()->getRunnableKey());
        m_process->waitForFinished(-1);
        //有错则传出空数据
        if (m_process->exitCode() != 0) {
            emit kernFinished(m_threadCount);
            return;
        }
        if (!m_canRun) {
            return;
        }
        QByteArray outByte = m_process->readAllStandardOutput();
        if (!m_canRun) {
            return;
        }

        QStringList strList = QString(Utils::replaceEmptyByteArray(outByte)).split('\n', QString::SkipEmptyParts);
        for (int j = strList.size() - 1; j >= 0; --j) {
            if (!m_canRun) {
                return;
            }
            QString str = strList.at(j);
            LOG_MSG_JOURNAL msg;
            //删除颜色格式字符
            str.replace(QRegExp("\\#033\\[\\d+(;\\d+){0,2}m"), "");
            QStringList list = str.split(" ", QString::SkipEmptyParts);
            if (list.size() < 5)
                continue;
            //获取内核年份接口已添加，等待系统接口添加年份改变相关日志
            QStringList timeList;
            if (list[0].contains("-")) {
                timeList.append(list[0]);
                timeList.append(list[1]);
                iTime = formatDateTime(list[0], list[1]);
            } else {
                timeList.append(list[0]);
                timeList.append(list[1]);
                timeList.append(list[2]);
                iTime = formatDateTime(list[0], list[1], list[2]);
            }

            //对时间筛选
            if (m_kernFilters.timeFilterBegin > 0 && m_kernFilters.timeFilterEnd > 0) {
                if (iTime < m_kernFilters.timeFilterBegin || iTime > m_kernFilters.timeFilterEnd)
                    continue;
            }

            msg.dateTime = timeList.join(" ");
            QStringList tmpList;
            utsname _utsname;
            uname(&_utsname);
            if (list[0].contains("-")) {
                // get hostname.
                msg.hostName = QString(_utsname.nodename);
                tmpList = list[3].split("[");
            } else {
                // get hostname.
                msg.hostName = QString(_utsname.nodename);
                tmpList = list[4].split("[");
            }

            int m = 0;
            //内核日志存在年份，解析用户名和进程id
            if (list[0].contains("-")) {
                if (tmpList.size() != 2) {
                    msg.daemonName = list[3].split(":")[0];
                } else {
                    msg.daemonName = list[3].split("[")[0];
                    QString id = list[3].split("[")[1];
                    id.chop(2);
                    msg.daemonId = id;
                }
                m = 4;
            } else {//内核日志不存在年份,解析用户名和进程id
                if (tmpList.size() != 2) {
                    msg.daemonName = list[4].split(":")[0];
                } else {
                    msg.daemonName = list[4].split("[")[0];
                    QString id = list[4].split("[")[1];
                    id.chop(2);
                    msg.daemonId = id;
                }
                m = 5;
            }

            QString msgInfo;
            for (int k = m; k < list.size(); k++) {
                msgInfo.append(list[k] + " ");
            }
            msg.msg = msgInfo;

            //            kList.append(msg);
            kList.append(msg);
            if (!m_canRun) {
                return;
            }
            //每获得500个数据就发出信号给控件加载
            if (kList.count() % SINGLE_READ_CNT == 0) {
                emit kernData(m_threadCount, kList);
                kList.clear();
            }
            if (!m_canRun) {
                return;
            }
        }
    }
    //最后可能有余下不足500的数据
    if (kList.count() >= 0) {
        emit kernData(m_threadCount, kList);
    }
    emit kernFinished(m_threadCount);
}

/**
 * @brief LogAuthThread::handleKwin 获取kwin日志逻辑
 */
void LogAuthThread::handleKwin()
{
    QFile file(KWIN_TREE_DATA);
    if (!m_canRun) {
        return;
    }
    QList<LOG_MSG_KWIN> kwinList;
    if (!file.exists()) {
        emit kwinFinished(m_threadCount);
        return;
    }
    if (!m_canRun) {
        return;
    }
    initProccess();
    m_process->start("cat", QStringList() << KWIN_TREE_DATA);
    m_process->waitForFinished(-1);
    if (!m_canRun) {
        return;
    }
    QByteArray outByte = m_process->readAllStandardOutput();
    if (!m_canRun) {
        return;
    }

    QStringList strList =  QString(Utils::replaceEmptyByteArray(outByte)).split('\n', QString::SkipEmptyParts);

    for (int i = strList.size() - 1; i >= 0 ; --i)  {
        QString str = strList.at(i);
        if (!m_canRun) {
            return;
        }
        if (str.trimmed().isEmpty()) {
            continue;
        }
        LOG_MSG_KWIN kwinMsg;
        kwinMsg.msg = str;
        kwinList.append(kwinMsg);
        //每获得500个数据就发出信号给控件加载
        if (kwinList.count() % SINGLE_READ_CNT == 0) {
            emit kwinData(m_threadCount, kwinList);
            kwinList.clear();
        }
    }

    if (!m_canRun) {
        return;
    }
    //最后可能有余下不足500的数据
    if (kwinList.count() >= 0) {
        emit kwinData(m_threadCount, kwinList);
    }
    emit kwinFinished(m_threadCount);
}

/**
 * @brief LogAuthThread::handleXorg 处理xorg日志获取逻辑
 */
void LogAuthThread::handleXorg()
{
    qint64 curDtSecond = 0;

    QList<LOG_MSG_XORG> xList;
    for (int i = 0; i < m_FilePath.count(); i++) {
        if (!m_FilePath.at(i).contains("txt")) {
            QFile file(m_FilePath.at(i)); // add by Airy
            if (!file.exists()) {
                emit proccessError(tr("Log file is empty"));
                emit xorgFinished(m_threadCount);
                return;
            }
        }
        if (!m_canRun) {
            return;
        }
        QString m_Log = DLDBusHandler::instance(this)->readLog(m_FilePath.at(i));
        QByteArray outByte = m_Log.toUtf8();
        if (!m_canRun) {
            return;
        }
        //计算文件生成的时间加上文件时间偏移量
        QFileInfo fileInfo(m_FilePath.at(i));
        QDateTime creatTime = fileInfo.birthTime();
        curDtSecond = creatTime.toMSecsSinceEpoch();
        if (!m_canRun) {
            return;
        }
        QStringList fileInfoList = QString(Utils::replaceEmptyByteArray(outByte)).split('\n', QString::SkipEmptyParts);
        QString tempStr = "";
        for (QStringList::Iterator k = fileInfoList.end() - 1; k != fileInfoList.begin() - 1; --k) {
            QString &str = *k;
            //清除颜色格式字符
            str.replace(QRegExp("\\x1B\\[\\d+(;\\d+){0,2}m"), "");
            if (str.startsWith("[")) {
                QStringList list = str.split("]", QString::SkipEmptyParts);
                if (list.count() < 2)
                    continue;
                QString timeStr = list[0];
                QString msgInfo = list.mid(1, list.length() - 1).join("]");
                // 把文件生成时间加上日志记录的毫秒数则为日志记录的时间
                QString tStr = timeStr.split("[", QString::SkipEmptyParts)[0].trimmed();
                qint64 tStrMesc = qint64(tStr.toDouble() * 1000);
                qint64 realT = 0;
                realT = curDtSecond + tStrMesc;
                QDateTime realDt = QDateTime::fromMSecsSinceEpoch(realT);
                if (m_xorgFilters.timeFilterBegin > 0 && m_xorgFilters.timeFilterEnd > 0) {
                    if (realDt.toMSecsSinceEpoch() < m_xorgFilters.timeFilterBegin || realDt.toMSecsSinceEpoch() > m_xorgFilters.timeFilterEnd)
                        continue;
                }
                LOG_MSG_XORG msg;
                msg.dateTime = realDt.toString("yyyy-MM-dd hh:mm:ss.zzz");
                msg.msg = msgInfo + tempStr;
                tempStr.clear();
                xList.append(msg);
                //每获得500个数据就发出信号给控件加载
                if (xList.count() % SINGLE_READ_CNT == 0) {
                    emit xorgData(m_threadCount, xList);
                    xList.clear();
                }
            } else {
                tempStr.prepend(" " + str);
            }
        }
    }
    if (!m_canRun) {
        return;
    }
    //最后可能有余下不足500的数据
    if (xList.count() >= 0) {
        emit xorgData(m_threadCount, xList);
    }
    emit xorgFinished(m_threadCount);
}

/**
 * @brief LogAuthThread::handleDkpg 获取dpkg逻辑
 */
void LogAuthThread::handleDkpg()
{
    QList<LOG_MSG_DPKG> dList;
    for (int i = 0; i < m_FilePath.count(); i++) {
        if (!m_FilePath.at(i).contains("txt")) {
            QFile file(m_FilePath.at(i)); // if not,maybe crash
            if (!file.exists())
                return;
        }
        if (!m_canRun) {
            return;
        }

        QString m_Log = DLDBusHandler::instance(this)->readLog(m_FilePath.at(i));
        QByteArray outByte = m_Log.toUtf8();
        if (!m_canRun) {
            return;
        }
        QStringList strList = QString(Utils::replaceEmptyByteArray(outByte)).split('\n', QString::SkipEmptyParts);
        for (int j = strList.size() - 1; j >= 0; --j) {
            QString str = strList.at(j);
            if (!m_canRun) {
                return;
            }
            str.replace(QRegExp("\\x1B\\[\\d+(;\\d+){0,2}m"), "");
            QStringList m_strList = str.split(" ", QString::SkipEmptyParts);
            if (m_strList.size() < 3)
                continue;

            QString info;
            for (auto k = 3; k < m_strList.size(); k++) {
                info = info + m_strList[k] + " ";
            }

            LOG_MSG_DPKG dpkgLog;
            dpkgLog.dateTime = m_strList[0] + " " + m_strList[1];
            QDateTime dt = QDateTime::fromString(dpkgLog.dateTime, "yyyy-MM-dd hh:mm:ss");
            //筛选时间
            if (m_dkpgFilters.timeFilterBegin > 0 && m_dkpgFilters.timeFilterEnd > 0) {
                if (dt.toMSecsSinceEpoch() < m_dkpgFilters.timeFilterBegin || dt.toMSecsSinceEpoch() > m_dkpgFilters.timeFilterEnd)
                    continue;
            }
            dpkgLog.action = m_strList[2];
            dpkgLog.msg = info;

            //        dList.append(dpkgLog);
            dList.append(dpkgLog);
            if (!m_canRun) {
                return;
            }
            //每获得500个数据就发出信号给控件加载
            if (dList.count() % SINGLE_READ_CNT == 0) {
                emit dpkgData(m_threadCount, dList);
                dList.clear();
            }
            if (!m_canRun) {
                return;
            }
        }
    }

    //最后可能有余下不足500的数据
    if (dList.count() >= 0) {
        emit dpkgData(m_threadCount, dList);
    }
    emit dpkgFinished(m_threadCount);
}

void LogAuthThread::handleNormal()
{
    qDebug() << "ogAuthThread::handleNormal()";
    if (!m_canRun) {
        emit normalFinished(m_threadCount);
        return;
    }

    int ret = -2;
    struct utmp *utbufp;

    if (wtmp_open(QString(WTMP_FILE).toLatin1().data()) == -1) {
        printf("open WTMP_FILE file error\n");
        return;  // exit(1) will exit this application
    }
    QList<utmp > normalList;
    QList<utmp > deadList;
    QList<LOG_MSG_NORMAL> nList;
    while ((utbufp = wtmp_next()) != (static_cast<struct utmp *>(nullptr))) {
        utmp value_ = *utbufp;
        if (utbufp->ut_type != DEAD_PROCESS) {
            normalList.append(value_);
        } else {
            deadList.append(value_);
        }
    }
    QString a_name = "root";
    foreach (utmp value, normalList) {
        QString strtmp = value.ut_name;
        if (strtmp.compare("runlevel") == 0 || (value.ut_type == RUN_LVL && strtmp != "shutdown") || value.ut_type == INIT_PROCESS) { // clear the runlevel
            continue;
        }
        struct utmp nodeUTMP   = list_get_ele_and_del(deadList, value.ut_line, ret);
        LOG_MSG_NORMAL Nmsg;
        if (value.ut_type == USER_PROCESS) {
            Nmsg.eventType = "Login";
            Nmsg.userName = value.ut_name;
            a_name = Nmsg.userName;
        } else {
            Nmsg.eventType = value.ut_name;
            if (strtmp.compare("reboot") == 0) {
                Nmsg.eventType = "Boot";
            }
            Nmsg.userName = a_name;
        }
        QString end_str;
        if (deadList.length() > 0 && ret != -1)
            end_str = show_end_time(nodeUTMP.ut_time);
        else if (ret == -1 && value.ut_type == USER_PROCESS)
            end_str = "still logged in";
        else if (ret == -1 && value.ut_type == BOOT_TIME)
            end_str = "system boot";
        QString start_str = show_start_time(value.ut_time);
        QString n_time = QDateTime::fromTime_t(static_cast<uint>(value.ut_time)).toString("yyyy-MM-dd hh:mm:ss");
        end_str = end_str.remove(QChar('\n'), Qt::CaseInsensitive);
        start_str = start_str.remove(QChar('\n'), Qt::CaseInsensitive);
        Nmsg.dateTime = n_time;
        QDateTime nn_time = QDateTime::fromString(Nmsg.dateTime, "yyyy-MM-dd hh:mm:ss");
        if (m_normalFilters.timeFilterEnd > 0 && m_normalFilters.timeFilterBegin > 0) {
            if (nn_time.toMSecsSinceEpoch() < m_normalFilters.timeFilterBegin || nn_time.toMSecsSinceEpoch() > m_normalFilters.timeFilterEnd) { // add by Airy
                continue;
            }
        }

        Nmsg.msg = start_str + "  ~  " + end_str;
        printf("\n");
        nList.insert(0, Nmsg);

    }
    wtmp_close();

    if (nList.count() >= 0) {
        emit normalData(m_threadCount, nList);
    }
    emit normalFinished(m_threadCount);
}

void LogAuthThread::handleDnf()
{
    QList<LOG_MSG_DNF> dList;
    for (int i = 0; i < m_FilePath.count(); i++) {
        if (!m_FilePath.at(i).contains("txt")) {
            QFile file(m_FilePath.at(i)); // if not,maybe crash
            if (!file.exists())
                return;
        }
        if (!m_canRun) {
            return;
        }
        QString m_Log = DLDBusHandler::instance(this)->readLog(m_FilePath.at(i));
        QByteArray outByte = m_Log.toUtf8();
        QString output = Utils::replaceEmptyByteArray(outByte);
        if (!m_canRun) {
            return;
        }
        //上一次成功筛选出的日志
        int lastLogAddRow = -99;
        //上一次增加的换行的日志信息体的行数
        int lastMsgAddRow = -99;
        QStringList allLog = output.split('\n');
        QRegExp ipRegExp = QRegExp("[0-9]{4}-(0[1-9]|1[0-2])-(0[1-9]|[12][0-9]|3[01])T([0-9][0-9])\\:([0-5][0-9])\\:([0-5][0-9])Z");
        // for (QString str : output.split('\n')) {
        for (int i = 0; i < allLog.count(); ++i) {
            if (!m_canRun) {
                return;
            }
            QString str = allLog.value(i);

            str.replace(QRegExp("\\x1B\\[\\d+(;\\d+){0,2}m"), "");
            QStringList strList = str.split(" ", QString::SkipEmptyParts);
            if (ipRegExp.exactMatch(strList.value(0, "")) && strList.size() >= 2) {
            } else {
                //如果当前行数等于上次筛选插入进结果的日志行数加1(在下一行)或者在上次增加过的为纯信息体的行数的下一行,则认为这一行为上一个日志的信息体
                if (dList.length() > 0 && ((lastMsgAddRow == i - 1) || (lastLogAddRow == i - 1))) {
                    if (!str.isEmpty()) {
                        //上一个日志的信息体如果是空行,不换行并把空行清空
                        if (dList.first().msg.trimmed().isEmpty()) {
                            dList.first().msg = "";
                        } else {
                            dList.first().msg += "\n";
                        }

                        dList.first().msg += str;
                    }
                    //此行为信息体行
                    lastMsgAddRow = i;
                }
                continue;
            }
            QString info;
            for (auto i = 2; i < strList.size(); i++) {
                info = info + strList[i] + " ";
            }

            LOG_MSG_DNF dnfLog;

            QDateTime dt = QDateTime::fromString(strList[0], "yyyy-MM-ddThh:mm:ssZ");
            dt.setTimeSpec(Qt::UTC);
            QDateTime localdt = dt.toLocalTime();
            if (dt.toMSecsSinceEpoch() < m_dnfFilters.timeFilter)
                continue;
            if (m_dnfFilters.levelfilter != DNFLVALL) {
                if (m_dnfLevelDict.value(strList[1]) != m_dnfFilters.levelfilter)
                    continue;
            }
            dnfLog.dateTime = localdt.toString("yyyy-MM-dd hh:mm:ss");
            dnfLog.level = m_transDnfDict.value(strList[1]);
            dnfLog.msg = info;
            dList.insert(0, dnfLog);
            //此行为日志行
            //每获得500个数据就发出信号给控件加载
            lastLogAddRow = i;
            if (!m_canRun) {
                return;
            }
        }
    }
    emit dnfFinished(dList);
}

void LogAuthThread::handleDmesg()
{
    QList<LOG_MSG_DMESG> dmesgList;
    if (!m_canRun) {
        return;
    }
    QString startStr = startTime();
    QDateTime curDt = QDateTime::currentDateTime();

    if (startStr.isEmpty()) {
        emit dmesgFinished(dmesgList);
        return;
    }
    if (!m_canRun) {
        return;
    }

    initProccess();
    //共享内存对应变量置true，允许进程内部逻辑运行
    ShareMemoryInfo shareInfo;
    shareInfo.isStart = true;
    SharedMemoryManager::instance()->setRunnableTag(shareInfo);
    m_process->setProcessChannelMode(QProcess::MergedChannels);
    m_process->start("pkexec", QStringList() << "logViewerAuth"
                                             << "dmesg" << SharedMemoryManager::instance()->getRunnableKey());
    m_process->waitForFinished(-1);
    QString errorStr(m_process->readAllStandardError());
    Utils::CommandErrorType commandErrorType = Utils::isErroCommand(errorStr);
    if (!m_canRun) {
        return;
    }
    if (commandErrorType != Utils::NoError) {
        if (commandErrorType == Utils::PermissionError) {
            emit proccessError(errorStr + "\n" + "Please use 'sudo' run this application");
        } else if (commandErrorType == Utils::RetryError) {
            emit proccessError("The password is incorrect,please try again");
        }
        m_process->close();
        return;
    }
    if (!m_canRun) {
        return;
    }
    QByteArray outByte = m_process->readAllStandardOutput();
    QByteArray byte = Utils::replaceEmptyByteArray(outByte);
    QTextStream stream(&byte);
    QByteArray encode;
    stream.setCodec(encode);
    QString output = stream.readAll();
    QStringList l = QString(byte).split('\n');
    m_process->close();
    if (!m_canRun) {
        return;
    }
    qint64 curDtSecond = curDt.toMSecsSinceEpoch() - static_cast<int>(startStr.toDouble() * 1000);
    for (QString str : l) {
        if (!m_canRun) {
            return;
        }
        str.replace(QRegExp("\\x1B\\[\\d+(;\\d+){0,2}m"), "");
        QRegExp dmesgExp("^\\<([0-7])\\>\\[\\s*[+-]?(0|([1-9]\\d*))(\\.\\d+)?\\](.*)");
        //启用贪婪匹配
        dmesgExp.setMinimal(false);
        int pos = dmesgExp.indexIn(str);
        if (pos >= 0) {
            QStringList list = dmesgExp.capturedTexts();
            if (list.count() < 6)
                continue;
            QString timeStr = list[3] + list[4];
            QString msgInfo = list[5].simplified();
            int levelOrigin = list[1].toInt();
            QString tStr = timeStr.split("[", QString::SkipEmptyParts)[0].trimmed();
            qint64 realT = curDtSecond + qint64(tStr.toDouble() * 1000);
            QDateTime realDt = QDateTime::fromMSecsSinceEpoch(realT);
            if (realDt.toMSecsSinceEpoch() < m_dmesgFilters.timeFilter) // add by Airy
                continue;
            if (m_dmesgFilters.levelFilter != LVALL) {
                if (levelOrigin != m_dmesgFilters.levelFilter)
                    continue;
            }
            LOG_MSG_DMESG msg;
            msg.dateTime = realDt.toString("yyyy-MM-dd hh:mm:ss.zzz");
            msg.msg = msgInfo;
            msg.level = m_levelMap.value(levelOrigin);
            dmesgList.insert(0, msg);
        } else {
            if (dmesgList.length() > 0) {
                dmesgList[0].msg += str;
            }
        }
        if (!m_canRun) {
            return;
        }
    }
    emit dmesgFinished(dmesgList);
}

/**
 * @brief LogAuthThread::initProccess 初始化QProcess指针
 */
void LogAuthThread::initProccess()
{
    if (!m_process) {
        m_process.reset(new QProcess);
    }
}

/**
 * @brief LogAuthThread::formatDateTime 内核日志没有年份 格式为Sep 29 15:53:34 所以需要特殊转换
 * @param m 月份字符串
 * @param d 日期字符串
 * @param t 时间字符串
 * @return 时间毫秒数
 */
qint64 LogAuthThread::formatDateTime(QString m, QString d, QString t)
{
    QLocale local(QLocale::English, QLocale::UnitedStates);

    QDate curdt = QDate::currentDate();

    QString tStr = QString("%1 %2 %3 %4").arg(m).arg(d).arg(curdt.year()).arg(t);
    QDateTime dt = local.toDateTime(tStr, "MMM d yyyy hh:mm:ss");
    return dt.toMSecsSinceEpoch();
}

/**
 * @brief LogAuthThread::formatDateTime 内核日志有年份 格式为2020-01-05 所以需要特殊转换
 * @param y 年月日
 * @param t 时间字符串
 * @return 时间毫秒数
 */
qint64 LogAuthThread::formatDateTime(QString y, QString t)
{
    //when /var/kern.log have the year
    QLocale local(QLocale::English, QLocale::UnitedStates);
    QString tStr = QString("%1 %2").arg(y).arg(t);
    QDateTime dt = local.toDateTime(tStr, "yyyy-MM-dd hh:mm:ss");
    return dt.toMSecsSinceEpoch();
}
