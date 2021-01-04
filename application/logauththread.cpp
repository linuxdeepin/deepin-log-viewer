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

#include <DApplication>

#include <QDebug>
#include <QDateTime>

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
    if (m_process) {
        //   m_process->kill();
        m_process->close();
        m_process->deleteLater();
        m_process = nullptr;
    }


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

    // kill(qvariant_cast<pid_t>(m_process->pid()), SIGKILL);


    // m_process->close();
//        delete  m_process;
//        m_process = nullptr;
    //m_process->terminate();

    //  m_process->close();
    //m_process->waitForFinished(-1);

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
        return  "";
    }
    return startStr;
}

/**
 * @brief LogAuthThread::run 线程执行虚函数
 */
void LogAuthThread::run()
{
    qDebug() << " LogAuthThread::run-----threadrun" << m_type;
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
    QFile file("/var/log/boot.log");  // add by Airy
    if (!file.exists()) {
        emit bootFinished(bList);
        return;
    }
    if (!m_canRun) {
        return;
    }
    initProccess();
    m_process->setProcessChannelMode(QProcess::MergedChannels);
    //共享内存对应变量置true，允许进程内部逻辑运行
    ShareMemoryInfo   shareInfo ;
    shareInfo.isStart = true;
    SharedMemoryManager::instance()->setRunnableTag(shareInfo);
    //启动日志需要提权获取，运行的时候把对应共享内存的名称传进去，方便获取进程拿标记量判断是否继续运行
    m_process->start("pkexec", QStringList() << "logViewerAuth"
                     << "/var/log/boot.log" << SharedMemoryManager::instance()->getRunnableKey());
    m_process->waitForFinished(-1);

    QByteArray byte =   m_process->readAllStandardOutput();
    QTextStream stream(&byte);
    QByteArray encode;
    stream.setCodec(encode);
    QString str = stream.readAll();
    QStringList l = str.split('\n');
    qDebug() << __FUNCTION__ << "byte" << byte.length();
    qDebug() << __FUNCTION__ << "str" << str.length();

    //按换行分割
    for (QString lineStr : str.split('\n')) {
        if (lineStr.startsWith("/dev"))
            continue;

        // remove Useless characters
        lineStr.replace(QRegExp("\\x1B\\[\\d+(;\\d+){0,2}m"), "");
        Utils::replaceColorfulFont(&lineStr);
        QStringList retList;
        LOG_MSG_BOOT bMsg;
        retList = lineStr.split(" ", QString::SkipEmptyParts);
        if (lineStr.startsWith("[")) {
            //如果是以ok/failed开头，则认为是一条记录的开头，否则认为此行为上一条日志的信息体的后续
            bMsg.status = retList[1];
            QStringList leftList = retList.mid(3);
            bMsg.msg += leftList.join(" ");
            bList.insert(0, bMsg);
        } else {
            if (bList.size() == 0)
                continue;

            bList[0].msg += retList.join(" ");
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
    // connect(proc, &QProcess::readyRead, this, &LogAuthThread::onFinishedRead);
    m_process->setProcessChannelMode(QProcess::MergedChannels);
    if (!m_canRun) {
        return;
    }
    //如果共享内存没有初始化绑定好，则无法开始，因为不能开启一个可能无法停止的进程
    if (!SharedMemoryManager::instance()->isAttached()) {
        return;
    }
    //共享内存对应变量置true，允许进程内部逻辑运行
    ShareMemoryInfo   shareInfo ;
    shareInfo.isStart = true;
    SharedMemoryManager::instance()->setRunnableTag(shareInfo);
    //启动日志需要提权获取，运行的时候把对应共享内存的名称传进去，方便获取进程拿标记量判断是否继续运行
    m_process->start("pkexec", QStringList() << "logViewerAuth"
                     //         << "/home/zyc/Documents/tech/同方内核日志没有/kern.log" << SharedMemoryManager::instance()->getRunnableKey());
                     //   << "/home/zyc/Documents/tech/klu内核日志读取崩溃日志/kern.log" << SharedMemoryManager::instance()->getRunnableKey());
                     << "/var/log/kern.log" << SharedMemoryManager::instance()->getRunnableKey());
    //proc->start("pkexec", QStringList() << "/bin/bash" << "-c" << QString("cat %1").arg("/var/log/kern.log"));
    // proc->start("pkexec", QStringList() << QString("cat") << QString("/var/log/kern.log"));
    m_process->waitForFinished(-1);
    qDebug() << " m_process->exitCode() " << m_process->exitCode();
    //有错则传出空数据
    if (m_process->exitCode() != 0) {
        emit kernFinished(kList);
        return;
    }
    if (!m_canRun) {
        return;
    }
    QByteArray byte =   m_process->readLine();
//一行行读，不能直接1readAllStandardOutput，因为日志文本可能很大，有可能超过QByteArray最大大小
    while (!byte.isNull()) {
        if (!m_canRun) {
            return;
        }
        QTextStream stream(&byte);
        if (!m_canRun) {
            return;
        }
        QByteArray encode;
        if (!m_canRun) {
            //停止标记量，外部把置false则停止运行
            return;
        }
        stream.setCodec(encode);
        if (!m_canRun) {
            return;
        }
        QString str = stream.readAll();
        if (!m_canRun) {
            return;
        }
        QStringList l = str.split('\n');
        if (!m_canRun) {
            return;
        }

        QStringList a = str.split('\n');
        for (QString str : a) {
            if (!m_canRun) {
                return;
            }
            LOG_MSG_JOURNAL msg;
            //删除颜色格式字符
            str.replace(QRegExp("\\#033\\[\\d+(;\\d+){0,2}m"), "");
            QStringList list = str.split(" ", QString::SkipEmptyParts);
            if (list.size() < 5)
                continue;

            QStringList timeList;
            timeList.append(list[0]);
            timeList.append(list[1]);
            timeList.append(list[2]);
            qint64 iTime = formatDateTime(list[0], list[1], list[2]);
            //对时间筛选
            if (m_kernFilters.timeFilterBegin > 0 && m_kernFilters.timeFilterEnd > 0) {
                if (iTime < m_kernFilters.timeFilterBegin || iTime > m_kernFilters.timeFilterEnd)
                    continue;
            }

            msg.dateTime = timeList.join(" ");
            msg.hostName = list[3];

            QStringList tmpList = list[4].split("[");
            if (tmpList.size() != 2) {
                msg.daemonName = list[4].split(":")[0];
            } else {
                msg.daemonName = list[4].split("[")[0];
                QString id = list[4].split("[")[1];
                id.chop(2);
                msg.daemonId = id;
            }

            QString msgInfo;
            for (auto i = 5; i < list.size(); i++) {
                msgInfo.append(list[i] + " ");
            }
            msg.msg = msgInfo;

            //            kList.append(msg);
            kList.insert(0, msg);
            if (!m_canRun) {
                return;
            }

        }
        if (!m_canRun) {
            return;
        }
        byte =   m_process->readLine();
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

void LogAuthThread::handleDnf()
{
    QFile file("/var/log/dnf.log");  // if not,maybe crash
    QList<LOG_MSG_DNF> dList;
    if (!file.exists()) {
        emit dnfFinished(dList);
        return;
    }
    if (!m_canRun) {
        return;
    }
    initProccess();
    m_process->start("cat /var/log/dnf.log");
    m_process->waitForFinished(-1);
    QByteArray outByte = m_process->readAllStandardOutput();
    QString output = Utils::replaceEmptyByteArray(outByte);
    m_process->close();
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
    for (int i = 0; i <  allLog.count(); ++i) {
        if (!m_canRun) {
            return;
        }
        QString str = allLog.value(i);

        str.replace(QRegExp("\\x1B\\[\\d+(;\\d+){0,2}m"), "");
        QStringList strList = str.split(" ", QString::SkipEmptyParts);
        if (ipRegExp.exactMatch(strList.value(0, "")) && strList.size() >= 2) {

        } else {
            //如果当前行数等于上次筛选插入进结果的日志行数加1(在下一行)或者在上次增加过的为纯信息体的行数的下一行,则认为这一行为上一个日志的信息体
            if (dList.length() > 0  && ((lastMsgAddRow == i - 1) || (lastLogAddRow == i - 1))) {
                if (!str.isEmpty()) {
                    //上一个日志的信息体如果是空行,不换行并把空行清空
                    if (dList.first().msg.trimmed().isEmpty()) {
                        dList.first().msg = "";
                    } else {
                        dList.first().msg +=  "\n";
                    }

                    dList.first().msg += str ;
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
        if (dt.toMSecsSinceEpoch() < m_dnfFilters.timeFilter)
            continue;
        if (m_dnfFilters.levelfilter != DNFLVALL) {
            if (m_dnfLevelDict.value(strList[1]) != m_dnfFilters.levelfilter)
                continue;
        }
        dnfLog.dateTime = dt.toString("yyyy-MM-dd hh:mm:ss");
        dnfLog.level = m_transDnfDict.value(strList[1]);
        dnfLog.msg = info;
        dList.insert(0, dnfLog);
        //此行为日志行
        lastLogAddRow = i;
        if (!m_canRun) {
            return;
        }
    }
    qDebug() << "dnf size" << dList.length();
    emit dnfFinished(dList);
}

void LogAuthThread::handleDmesg()
{
    if (!m_canRun) {
        return;
    }
    QString startStr = startTime();
    QDateTime curDt = QDateTime::currentDateTime();
    QList<LOG_MSG_DMESG> dmesgList;
    if (startStr.isEmpty()) {
        emit dmesgFinished(dmesgList);
        return;
    }
    if (!m_canRun) {
        return;
    }

    initProccess();
    //如果共享内存没有初始化绑定好，则无法开始，因为不能开启一个可能无法停止的进程
    if (!SharedMemoryManager::instance()->isAttached()) {
        return;
    }
    //共享内存对应变量置true，允许进程内部逻辑运行
    ShareMemoryInfo   shareInfo ;
    shareInfo.isStart = true;
    SharedMemoryManager::instance()->setRunnableTag(shareInfo);
    // connect(proc, &QProcess::readyRead, this, &LogAuthThread::onFinishedRead);
    m_process->setProcessChannelMode(QProcess::MergedChannels);
    m_process->start("pkexec", QStringList() << "logViewerAuth"
                     << "dmesg" << SharedMemoryManager::instance()->getRunnableKey());
    //proc->start("pkexec", QStringList() << "/bin/bash" << "-c" << QString("cat %1").arg("/var/log/kern.log"));
    // proc->start("pkexec", QStringList() << QString("cat") << QString("/var/log/kern.log"));
    m_process->waitForFinished(-1);
    QString errorStr(m_process->readAllStandardError());
    Utils::CommandErrorType commandErrorType = Utils::isErroCommand(errorStr);
    if (!m_canRun) {
        return;
    }
    if (commandErrorType != Utils::NoError) {
        if (commandErrorType == Utils::PermissionError) {
            emit proccessError(errorStr + "\n" + "Please use 'sudo' run this application");
//            DMessageBox::information(nullptr, tr("information"),
//                                     errorStr + "\n" + "Please use 'sudo' run this application");
        } else if (commandErrorType == Utils::RetryError) {
            emit proccessError("The password is incorrect,please try again");
//            DMessageBox::information(nullptr, tr("information"),
//                                     "The password is incorrect,please try again");
        }
        m_process->close();
        return;
    }
    if (!m_canRun) {
        return;
    }
    QByteArray outByte =   m_process->readAllStandardOutput();
    QByteArray byte = Utils::replaceEmptyByteArray(outByte);
    QTextStream stream(&byte);
    QByteArray encode;
    stream.setCodec(encode);
    QString output = stream.readAll();
    QStringList l = QString(byte).split('\n');
    qDebug() << __FUNCTION__ << "byte" << byte.length();
//    qDebug() << __FUNCTION__ << "str" << outByte;
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
        // qDebug()  << pos <<  dmesgExp.capturedTexts();    qDebug() << str;
        if (pos >= 0) {

            QStringList list = dmesgExp.capturedTexts();
            if (list.count() < 6)
                continue;
//            if (list.count() > 4) {
//                qDebug() << "match wrong" << list;
//            }

            QString timeStr = list[3]  + list[4];
            QString msgInfo = list[5].simplified();
            int levelOrigin = list[1].toInt();
            // qDebug() << "match wrong" << timeStr << levelOrigin << list;
            // get time
            QString tStr = timeStr.split("[", QString::SkipEmptyParts)[0].trimmed();
            qint64 realT = curDtSecond + qint64(tStr.toDouble() * 1000);
            QDateTime realDt = QDateTime::fromMSecsSinceEpoch(realT);
            if (realDt.toMSecsSinceEpoch() < m_dmesgFilters.timeFilter)  // add by Airy
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
    qDebug() << "dmesg size" << dmesgList.length();
    emit dmesgFinished(dmesgList);
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



void LogAuthThread::onFinished(int exitCode)
{
    Q_UNUSED(exitCode)

//    QProcess *process = dynamic_cast<QProcess *>(sender());
//    if (!process) {
//        return;
//    }
//    if (!process->isOpen()) {
//        return;
//    }
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
    //  qDebug() << __FUNCTION__ << "str" << str;

//    emit cmdFinished(m_type, str);


}

void LogAuthThread::kernDataRecived()
{

}

//void LogAuthThread::onFinishedRead()
//{
//    QProcess *process = dynamic_cast<QProcess *>(sender());
//    QString str = QString(process->readAllStandardOutput());
//    QStringList l = str.split('\n');

//}
