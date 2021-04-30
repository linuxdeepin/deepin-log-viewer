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

#include "logapplicationparsethread.h"
#include "utils.h"
#include "dbusproxy/dldbushandler.h"

#include <DMessageBox>

#include <QDateTime>
#include <QDebug>
#include <QProcess>

DWIDGET_USE_NAMESPACE

//std::atomic<LogApplicationParseThread *> LogApplicationParseThread::m_instance;
//std::mutex LogApplicationParseThread::m_mutex;
int LogApplicationParseThread::thread_count = 0;
/**
 * @brief LogApplicationParseThread::LogApplicationParseThread 构造函数
 * @param parent 父对象
 */
LogApplicationParseThread::LogApplicationParseThread(QObject *parent)
    : QThread(parent)
{
    qRegisterMetaType<QList<LOG_MSG_APPLICATOIN> >("QList<LOG_MSG_APPLICATOIN>");

    initMap();
    //静态计数变量加一并赋值给本对象的成员变量，以供外部判断是否为最新线程发出的数据信号
    thread_count++;
    m_threadCount = thread_count;
}

/**
 * @brief LogApplicationParseThread::~LogApplicationParseThread 析构函数，停止并销毁process指针
 */
LogApplicationParseThread::~LogApplicationParseThread()
{
    if (m_process) {
        m_process->kill();
        m_process->close();
        delete  m_process;
        m_process = nullptr;
    }
}

/**
 * @brief LogApplicationParseThread::setParam 设置获取的筛选条件
 * @param iFilter 筛选条件结构体
 */
void LogApplicationParseThread::setParam(APP_FILTERS &iFilter)
{
    m_AppFiler = iFilter;
}

/**
 * @brief LogApplicationParseThread::stopProccess 停止qprocess获取进程
 */
void LogApplicationParseThread::stopProccess()
{
    m_canRun = false;
    if (m_process && m_process->isOpen()) {
        // m_process->readAll();
        m_process->kill();
    }
}

int LogApplicationParseThread::getIndex()
{
    return m_threadCount;
}

/**
 * @brief LogApplicationParseThread::doWork 获取数据线程逻辑
 */
void LogApplicationParseThread::doWork()
{
    //此线程刚开始把可以继续变量置true，不然下面没法跑
    m_canRun = true;
    m_appList.clear();
    initProccess();
    //connect(m_process, SIGNAL(finished(int)), m_process, SLOT(deleteLater()));
    //因为筛选信息中含有日志文件路径，所以不能为空，否则无法获取
    if (m_AppFiler.path.isEmpty()) {  //modified by Airy for bug 20457::if path is empty,item is not empty
        emit appFinished(m_threadCount);
    } else {
        QStringList filePath = DLDBusHandler::instance(this)->getFileInfo(m_AppFiler.path);
        for (int i = 0; i < filePath.count(); i++) {
            //使用cat命令获取日志文件的文本
            QStringList arg;
            arg << "-c" << QString("cat %1").arg(filePath[i]);
            m_process->start("/bin/bash", arg);
            m_process->waitForFinished(-1);
            QByteArray byteOutput = m_process->readAllStandardOutput();
            m_process->close();
            arg.clear();
            if (!m_canRun) {
                return;
            }
            //替换截断空字符
            QStringList strList = QString(Utils::replaceEmptyByteArray(byteOutput)).split('\n', QString::SkipEmptyParts);
            for (int j = strList.size() - 1; j >= 0; --j) {
                LOG_MSG_APPLICATOIN msg;
                QString str = strList.at(j);
                if (!m_canRun) {
                    return;
                }

                //删除空白字符
                str.replace(QRegExp("\\s{2,}"), "");
                //删除颜色字符
                str.replace(QRegExp("\\x1B\\[\\d+(;\\d+){0,2}m"), "");
                QStringList list = str.split("]", QString::SkipEmptyParts);
                //日志格式各字段用[]和空格隔开，有等级 时间 信息体 三个字段，至少应该能分割出三个

                if (list.count() < 3||!list[0].contains("[")||!list[1].contains("["))
                    continue;

                QStringList infoList = list[0].split("[", QString::SkipEmptyParts);
                if (infoList.count() < 2) {
                    continue;
                }
                QString dateTime = infoList[0].trimmed();
                //原始文件时间日期文本为2020-07-03, 19:19:18.639 需要去除,
                if (dateTime.contains(",")) {
                    dateTime.replace(",", "");
                }
                //        if (dateTime.split(".").count() == 2) {
                //            dateTime = dateTime.split(".")[0];
                //        }

                // add by Airy
                // boot maker log can not show
                if (dateTime.split(".").count() > 2) {
                    list[1] = " " + list[1];
                    QDateTime g_time = QDateTime::fromString(dateTime, "yyyyMMdd.hh:mm:ss.zzz");
                    QString gg_time = g_time.toString("yyyy-MM-dd hh:mm:ss.zzz");
                    dateTime = gg_time;
                } // add

                qint64 dt = QDateTime::fromString(dateTime, "yyyy-MM-dd hh:mm:ss.zzz").toMSecsSinceEpoch();
                //按筛选条件筛选时间段
                if (m_AppFiler.timeFilterBegin > 0 && m_AppFiler.timeFilterEnd > 0) {
                    if (dt < m_AppFiler.timeFilterBegin || dt > m_AppFiler.timeFilterEnd)
                        continue;
                }

                msg.dateTime = dateTime;
                msg.level = infoList[1];
                //筛选日志等级
                if (m_AppFiler.lvlFilter != LVALL) {
                    if (m_levelDict.value(msg.level) != m_AppFiler.lvlFilter)
                        continue;
                }
                QStringList msgList = list[1].split("[", QString::SkipEmptyParts);
                if (msgList.count() < 2) {
                    continue;
                }
                msg.src = msgList[1];
                if (list.count() >= 4) {
                    msg.detailInfo = list.mid(2).join("]");
                    msg.msg = msg.detailInfo;
                    if (msg.detailInfo.size() > 500) {
                        msg.msg = msg.detailInfo.mid(0, 500);
                    }
                } else {
                    msg.detailInfo = list[2];
                    msg.msg = msg.detailInfo;
                    if (msg.detailInfo.size() > 500) {
                        msg.msg = msg.detailInfo.mid(0, 500);
                    }
                }

                m_appList.append(msg);
                //每获得500个数据就发出信号给控件加载
                if (m_appList.count() % SINGLE_READ_CNT == 0) {
                    emit appData(m_threadCount, m_appList);
                    m_appList.clear();
                }
            }
            if (!m_canRun) {
                return;
            }
        }
        //最后可能有余下不足500的数据
        if (m_appList.count() >= 0) {
            emit appData(m_threadCount, m_appList);
        }

        emit appFinished(m_threadCount);
    }


}

void LogApplicationParseThread::onProcFinished(int ret)
{
    Q_UNUSED(ret)
#if 0
    QProcess *proc = dynamic_cast<QProcess *>(sender());
    QString output = proc->readAllStandardOutput();
    proc->deleteLater();

    for (QString str : output.split('\n')) {
        LOG_MSG_APPLICATOIN msg;

        str.replace(QRegExp("\\s{2,}"), "");

        QStringList list = str.split("]", QString::SkipEmptyParts);
        if (list.count() < 3)
            continue;

        QString dateTime = list[0].split("[", QString::SkipEmptyParts)[0].trimmed();
        if (dateTime.contains(",")) {
            dateTime.replace(",", "");
        }
        //        if (dateTime.split(".").count() == 2) {
        //            dateTime = dateTime.split(".")[0];
        //        }
        qint64 dt = QDateTime::fromString(dateTime, "yyyy-MM-dd hh:mm:ss.zzz").toMSecsSinceEpoch();
        if (dt < m_periorTime)
            continue;
        msg.dateTime = dateTime;
        msg.level = list[0].split("[", QString::SkipEmptyParts)[1];

        if (m_level != LVALL) {
            if (m_levelDict.value(msg.level) != m_level)
                continue;
        }

        msg.src = list[1].split("[", QString::SkipEmptyParts)[1];

        if (list.count() >= 4) {
            msg.msg = list.mid(2).join("]");
        } else {
            msg.msg = list[2];
        }

        m_appList.insert(0, msg);
    }

    emit appCmdFinished(m_appList);
#endif
}

/**
 * @brief LogApplicationParseThread::initMap 初始化数据结构
 */
void LogApplicationParseThread::initMap()
{
    m_levelDict.insert("Warning", WARN);
    m_levelDict.insert("Debug", DEB);
    m_levelDict.insert("Info", INF);
    m_levelDict.insert("Error", ERR);
}

/**
 * @brief LogApplicationParseThread::initProccess 构造 QProcess成员指针
 */
void LogApplicationParseThread::initProccess()
{
    if (!m_process) {
        m_process = new QProcess;
    }
}

/**
 * @brief LogApplicationParseThread::run 线程执行虚函数重写逻辑
 */
void LogApplicationParseThread::run()
{
    qDebug() << "LogApplicationParseThread::run()---threadrun";
    doWork();
}
