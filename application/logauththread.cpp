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
#include "logauththread.h"
#include "utils.h"
#include "sharedmemorymanager.h"
#include "dbusproxy/dldbushandler.h"
#include "sys/utsname.h"
#include <wtmpparse.h>

#include <QDebug>
#include <QDateTime>
#include <time.h>
#include <utmp.h>
#include <utmpx.h>

std::atomic<LogAuthThread *> LogAuthThread::m_instance;
std::mutex LogAuthThread::m_mutex;
int LogAuthThread::thread_count = 0;

/**
 * @brief LogAuthThread::LogAuthThread 构造函数
 * @param parent 父对象
 */
LogAuthThread::LogAuthThread(QObject *parent)
    :  QObject(parent),
       QRunnable()

{
    //使用线程池启动该线程，跑完自己删自己
    setAutoDelete(true);
    //静态计数变量加一并赋值给本对象的成员变量，以供外部判断是否为最新线程发出的数据信号
    thread_count++;
    m_threadCount = thread_count;


}

/**
 * @brief LogAuthThread::~LogAuthThread 析构函数，停止并销毁process指针
 */
LogAuthThread::~LogAuthThread()
{
    stopProccess();
    if (m_process) {
        m_process->close();
        m_process->deleteLater();
        m_process = nullptr;
    }


}

/**
 * @brief LogAuthThread::getStandardOutput 返回m_process执行输出的信息
 * @return m_process执行输出的信息
 */
QString LogAuthThread::getStandardOutput()
{
    return m_output;
}

/**
 * @brief LogAuthThread::getStandardError 返回m_process执行输出的错误信息
 * @return m_process执行输出的错误信息
 */
QString LogAuthThread::getStandardError()
{
    return m_error;
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

/**
 * @brief LogAuthThread::run 线程执行虚函数
 */
void LogAuthThread::run()
{
    qDebug() << " LogAuthThread::run" << m_type;
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
    QFile file("/var/log/boot.log");  // add by Airy
    if (!file.exists()) {
        emit bootFinished(bList);
        return;
    }
    if (!m_canRun) {
        return;
    }
    initProccess();

    QString m_Log = DLDBusHandler::instance(this)->readLog("/var/log/boot.log");
    if (DLDBusHandler::instance(this)->exitCode() != 0) {
        emit bootFinished(bList);
        return;
    }
    QByteArray byte = m_Log.toUtf8();
    QString tempStr = "";
    QStringList strList = QString(Utils::replaceEmptyByteArray(byte)).split('\n', QString::SkipEmptyParts);

    //按换行分割
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
    }
    emit bootFinished(bList);
}

/**
 * @brief LogAuthThread::handleKern 处理获取内核日志
 */
void LogAuthThread::handleKern()
{
    QList<LOG_MSG_JOURNAL> kList;
    QFile file("/var/log/kern.log"); // add by Airy
    if (!file.exists()) {
        emit kernFinished(kList);
        return;
    }
    if (!m_canRun) {
        return;
    }
    initProccess();
    if (!m_canRun) {
        return;
    }
    QString m_Log = DLDBusHandler::instance(this)->readLog("/var/log/kern.log");

    //有错则传出空数据
    if (DLDBusHandler::instance(this)->exitCode() != 0) {
        emit kernFinished(kList);
        return;
    }
    QByteArray outByte = m_Log.toUtf8();
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
        } else { //内核日志不存在年份,解析用户名和进程id
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
    }

    emit kernFinished(kList);
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
        emit kwinFinished(kwinList);
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
    QString output = Utils::replaceEmptyByteArray(outByte);
    if (!m_canRun) {
        return;
    }
    QString filterMsgStr = m_kwinFilters.msg;
    QString filterMsgStr1 = m_kwinFilters.msg;
    for (QString str : output.split('\n')) {
        if (!m_canRun) {
            return;
        }
        if (str.trimmed().isEmpty()) {
            continue;
        }
        LOG_MSG_KWIN kwinMsg;
        kwinMsg.msg = str;
        kwinList.insert(0, kwinMsg);
    }

    if (!m_canRun) {
        return;
    }
    emit kwinFinished(kwinList);
}

/**
 * @brief LogAuthThread::handleXorg 处理xorg日志获取逻辑
 */
void LogAuthThread::handleXorg()
{
    QFile file("/var/log/Xorg.0.log");  // if not,maybe crash
    //读取现在到本次开机经过的时间，xorg日志文件的时间为从本次开机到日志记录产生时的时间差值
    QFile startFile("/proc/uptime");
    QList<LOG_MSG_XORG> xList;
    if (!file.exists() || !startFile.exists()) {
        emit proccessError(tr("Log file is empty"));
        emit xorgFinished(xList);
        return;
    }
    if (!m_canRun) {
        return;
    }
    QString startStr = "";
    if (startFile.open(QFile::ReadOnly)) {

        startStr = QString(startFile.readLine());
        startFile.close();
    }
    if (!m_canRun) {
        return;
    }
    qDebug() << "startStr" << startFile;
    startStr = startStr.split(" ").value(0, "");
    if (startStr.isEmpty()) {
        emit proccessError(tr("Log file is empty"));
        emit xorgFinished(xList);
        return;
    }
    if (!m_canRun) {
        return;
    }
    initProccess();
    if (!m_canRun) {
        return;
    }
    m_process->start("cat /var/log/Xorg.0.log");  // file path is fixed. so write cmd direct
    m_process->waitForFinished(-1);
    if (!m_canRun) {
        return;
    }
    QString errorStr(m_process->readAllStandardError());
    if (!m_canRun) {
        return;
    }
    //进程返回输出不是预期的则报错返回
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
        return;
    }
    if (!m_canRun) {
        return;
    }
    QByteArray outByte = m_process->readAllStandardOutput();
    QString output = Utils::replaceEmptyByteArray(outByte);
    m_process->close();
    if (!m_canRun) {
        return;
    }
    //当前时间减去开机到现在过去的毫秒数则为开机时间
    QDateTime curDt = QDateTime::currentDateTime();
    qint64 curDtSecond = curDt.toMSecsSinceEpoch() - static_cast<int>(startStr.toDouble() * 1000);
    if (!m_canRun) {
        return;
    }
    for (QString str : output.split('\n')) {
        if (!m_canRun) {
            return;
        }
        //清除颜色格式字符
        str.replace(QRegExp("\\x1B\\[\\d+(;\\d+){0,2}m"), "");

        if (str.startsWith("[")) {
            QStringList list = str.split("]", QString::SkipEmptyParts);
            if (list.count() != 2)
                continue;

            QString timeStr = list[0];
            QString msgInfo = list[1];

            // 把开机时间加上日志记录的毫秒数则为日志记录的时间
            QString tStr = timeStr.split("[", QString::SkipEmptyParts)[0].trimmed();
            qint64 realT = curDtSecond + qint64(tStr.toDouble() * 1000);
            QDateTime realDt = QDateTime::fromMSecsSinceEpoch(realT);
            if (m_xorgFilters.timeFilterBegin > 0 && m_xorgFilters.timeFilterEnd > 0) {
                if (realDt.toMSecsSinceEpoch() < m_xorgFilters.timeFilterBegin || realDt.toMSecsSinceEpoch() > m_xorgFilters.timeFilterEnd)
                    continue;
            }
            LOG_MSG_XORG msg;
            msg.dateTime = realDt.toString("yyyy-MM-dd hh:mm:ss.zzz");
            msg.msg = msgInfo;

            xList.insert(0, msg);
        } else {
            if (xList.length() > 0) {
                xList[0].msg += str;
            }
        }
        if (!m_canRun) {
            return;
        }
    }
    qDebug() << "xorg size" << xList.length();
    if (!m_canRun) {
        return;
    }
    emit xorgFinished(xList);
}

/**
 * @brief LogAuthThread::handleDkpg 获取dpkg逻辑
 */
void LogAuthThread::handleDkpg()
{

    QFile file("/var/log/dpkg.log");  // if not,maybe crash
    if (!file.exists())
        return;
    if (!m_canRun) {
        return;
    }
    initProccess();
    if (!m_canRun) {
        return;
    }
    m_process->start("cat /var/log/dpkg.log");  // file path is fixed. so write cmd direct
    m_process->waitForFinished(-1);
    if (!m_canRun) {
        return;
    }
    QByteArray outByte = m_process->readAllStandardOutput();
    if (!m_canRun) {
        return;
    }
    QString output = Utils::replaceEmptyByteArray(outByte);

    m_process->close();
    if (!m_canRun) {
        return;
    }
    QList<LOG_MSG_DPKG> dList;
    for (QString str : output.split('\n')) {
        if (!m_canRun) {
            return;
        }
        str.replace(QRegExp("\\x1B\\[\\d+(;\\d+){0,2}m"), "");
        QStringList strList = str.split(" ", QString::SkipEmptyParts);
        if (strList.size() < 3)
            continue;

        QString info;
        for (auto i = 3; i < strList.size(); i++) {
            info = info + strList[i] + " ";
        }

        LOG_MSG_DPKG dpkgLog;
        dpkgLog.dateTime = strList[0] + " " + strList[1];
        QDateTime dt = QDateTime::fromString(dpkgLog.dateTime, "yyyy-MM-dd hh:mm:ss");
        //筛选时间
        if (m_dkpgFilters.timeFilterBegin > 0 && m_dkpgFilters.timeFilterEnd > 0) {
            if (dt.toMSecsSinceEpoch() < m_dkpgFilters.timeFilterBegin || dt.toMSecsSinceEpoch() > m_dkpgFilters.timeFilterEnd)
                continue;
        }
        dpkgLog.action = strList[2];
        dpkgLog.msg = info;

        //        dList.append(dpkgLog);
        dList.insert(0, dpkgLog);
        if (!m_canRun) {
            return;
        }
    }
    if (!m_canRun) {
        return;
    }
    qDebug() << "dkpg size" << dList.length();
    emit dpkgFinished(dList);
}

/**
 * @brief LogAuthThread::initProccess 初始化QProcess指针
 */
void LogAuthThread::initProccess()
{
    if (!m_process) {
        m_process = new QProcess;
        //connect(m_process, SIGNAL(finished(int)), this, SLOT(onFinished(int)), Qt::UniqueConnection);
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
    //    QDateTime::fromString("9月 24 2019 10:32:34", "MMM d yyyy hh:mm:ss");
    // default year =2019
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

void LogAuthThread::onFinished(int exitCode)
{
    Q_UNUSED(exitCode)

    if (m_type != KERN && m_type != BOOT) {
        return;
    }
    QByteArray byte =   m_process->readAllStandardOutput();
    QTextStream stream(&byte);
    QByteArray encode;
    stream.setCodec(encode);
    QString str = stream.readAll();
    QStringList l = str.split('\n');
    qDebug() << __FUNCTION__ << "byte" << byte.length();
    qDebug() << __FUNCTION__ << "str" << str.length();

}

void LogAuthThread::kernDataRecived()
{

}
