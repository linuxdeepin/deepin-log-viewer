// SPDX-FileCopyrightText: 2019 - 2024 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "parsethreadkern.h"
#include "qtcompat.h"

#include <QDebug>
#include <QDateTime>
#include <time.h>
using namespace std;

Q_DECLARE_LOGGING_CATEGORY(logApp)

/**
 * @brief ParseThreadKern::ParseThreadKern 构造函数
 * @param parent 父对象
 */
ParseThreadKern::ParseThreadKern(QObject *parent)
    : ParseThreadBase(parent)
{
    qCDebug(logApp) << "ParseThreadKern constructor called";
}

/**
 * @brief ParseThreadKern::~ParseThreadKern 析构函数，停止并销毁process指针
 */
ParseThreadKern::~ParseThreadKern()
{
    qCDebug(logApp) << "ParseThreadKern destructor called";
}

/**
 * @brief ParseThreadKern::run 线程执行虚函数
 */
void ParseThreadKern::run()
{
    qCDebug(logApp) << "ParseThreadKern run started";
    //此线程刚开始把可以继续变量置true，不然下面没法跑
    m_canRun = true;

    handleKern();
}

/**
 * @brief ParseThreadKern::handleKern 处理获取内核日志
 */
void ParseThreadKern::handleKern()
{
    qCDebug(logApp) << "Starting kernel log handling";
    QList<QString> dataList;
    qint64 gStartLine = m_filter.segementIndex * SEGEMENT_SIZE;
    qCDebug(logApp) << "Global start line:" << gStartLine;
    m_FilePath = DLDBusHandler::instance(this)->getFileInfo(m_filter.filePath, false);
    qCDebug(logApp) << "Found" << m_FilePath.count() << "files to process";
    for (int i = 0; i < m_FilePath.count(); i++) {
        qCDebug(logApp) << "Processing file" << i << ":" << m_FilePath.at(i);
        if (!m_FilePath.at(i).contains("txt")) {
            QFile file(m_FilePath.at(i)); // add by Airy
            if (!file.exists()) {
                qCWarning(logApp) << "File does not exist:" << m_FilePath.at(i);
                emit parseFinished(m_threadCount, m_type);
                return;
            }
        }
        if (!m_canRun) {
            qCDebug(logApp) << "Thread stopped, returning";
            return;
        }

        if (!Utils::runInCmd) {
            qCDebug(logApp) << "Running with authentication";
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
            qCDebug(logApp) << "Starting pkexec process for file:" << m_FilePath.at(i);
            m_process->start("pkexec", QStringList() << "logViewerAuth"
                             << m_FilePath.at(i) << SharedMemoryManager::instance()->getRunnableKey());
            m_process->waitForFinished(-1);
            //有错则传出空数据
            if (m_process->exitCode() != 0) {
                qCWarning(logApp) << "Process failed with exit code:" << m_process->exitCode();
                emit parseFinished(m_threadCount, m_type, CancelAuth);
                return;
            }
            qCDebug(logApp) << "Process completed successfully";
        }

        if (!m_canRun) {
            return;
        }

        //如果是压缩文件，对其解压缩
        QString filePath = m_FilePath.at(i);
        if(QString::compare(QFileInfo(filePath).suffix(), "gz", Qt::CaseInsensitive) == 0){
            QStringList filePathList = DLDBusHandler::instance(this)->getFileInfo(filePath);
            if(filePathList.size()){
                filePath = filePathList.at(0);
            }else {
                filePath = "";
            }
        }

        qint64 lineCount = DLDBusHandler::instance(this)->getLineCount(filePath);
        qCDebug(logApp) << "File line count:" << lineCount;

        // 获取全局起始行在当前文件的相对起始行位置
        if (gStartLine >= lineCount) {
            qCDebug(logApp) << "Global start line exceeds file line count, skipping";
            gStartLine -= lineCount;
            continue;
        }

        qint64 startLine = gStartLine;
        qCDebug(logApp) << "Reading lines from" << startLine << "count:" << SEGEMENT_SIZE;

        QStringList strList = DLDBusHandler::instance(this)->readLogLinesInRange(filePath, startLine, SEGEMENT_SIZE);
        for (int j = strList.size() - 1; j >= 0; --j) {
            if (!m_canRun) {
                return;
            }
            QString str = strList.at(j);
            LOG_MSG_BASE msg;
            //删除颜色格式字符
            str.replace(REG_EXP("\\#033\\[\\d+(;\\d+){0,2}m"), "");

            // 以 "kernel:" 开始分割
            int kernelIndex = str.indexOf("kernel:");
            if (kernelIndex == -1) {
                continue; // 如果没有找到 "kernel:"，则跳过该行
            }
            QString msgContent = str.mid(kernelIndex + 7); // 去掉 "kernel:" 及其前面的部分
            str = str.left(kernelIndex); // 取 "kernel:" 前面的部分

            QStringList list = str.split(" ", SKIP_EMPTY_PARTS);
            if (list.size() < 2)
                continue;
            //获取内核年份接口已添加，等待系统接口添加年份改变相关日志
            QStringList timeList;
            QString dateTimeStr = list[0];
            bool isNewFormat = dateTimeStr.contains("T");
            if (isNewFormat) {
                QDateTime dateTime = QDateTime::fromString(dateTimeStr, Qt::ISODate);
                if (!dateTime.isValid()) {
                    dateTime = QDateTime::fromString(dateTimeStr, "yyyy-MM-ddTHH:mm:ss.zzzzzz+hh:mm");
                }
                timeList << dateTime.toString("yyyy-MM-dd") << dateTime.toString("HH:mm:ss");
                iTime = dateTime.toMSecsSinceEpoch();

                msg.hostName = list[1];
            } else {
                if (list[0].contains("-")) {
                    timeList << list[0] << list[1];
                    iTime = formatDateTime(list[0], list[1]);
                    msg.hostName = list[2];
                } else {
                    timeList << list[0] << list[1] << list[2];
                    iTime = formatDateTime(list[0], list[1], list[2]);
                    msg.hostName = list[3];
                }
            }

            // 对时间筛选
            if (m_filter.timeFilterBegin > 0 && m_filter.timeFilterEnd > 0) {
                if (iTime < m_filter.timeFilterBegin || iTime > m_filter.timeFilterEnd) {
                    qCDebug(logApp) << "Log entry filtered out by time range";
                    continue;
                }
            }

            msg.dateTime = timeList.join(" ");

            msg.daemonName = "kernel";
            msg.daemonId = "0";
            msg.msg = msgContent.trimmed();

            dataList.append(QJsonDocument(msg.toJson()).toJson(QJsonDocument::Compact));
            if (!m_canRun) {
                return;
            }
            //每获得500个数据就发出信号给控件加载
            if (dataList.count() % SINGLE_READ_CNT == 0) {
                qCDebug(logApp) << "Emitting" << dataList.count() << "log entries";
                emit logData(m_threadCount, dataList, m_type);
                dataList.clear();
            }
            if (!m_canRun) {
                return;
            }
        }
    }
    //最后可能有余下不足500的数据
    if (dataList.count() >= 0) {
        qCDebug(logApp) << "Emitting final" << dataList.count() << "log entries";
        emit logData(m_threadCount, dataList, m_type);
    }
    qCDebug(logApp) << "Kernel log parsing finished";
    emit parseFinished(m_threadCount, m_type);
}

/**
 * @brief ParseThreadKern::formatDateTime 内核日志没有年份 格式为Sep 29 15:53:34 所以需要特殊转换
 * @param m 月份字符串
 * @param d 日期字符串
 * @param t 时间字符串
 * @return 时间毫秒数
 */
qint64 ParseThreadKern::formatDateTime(QString m, QString d, QString t)
{
    qCDebug(logApp) << "Formatting date time without year:" << m << d << t;
    QLocale local(QLocale::English, QLocale::UnitedStates);

    QDate curdt = QDate::currentDate();

    QString tStr = QString("%1 %2 %3 %4").arg(m).arg(d).arg(curdt.year()).arg(t);
    QDateTime dt = local.toDateTime(tStr, "MMM d yyyy hh:mm:ss");
    qint64 result = dt.toMSecsSinceEpoch();
    qCDebug(logApp) << "Formatted time result:" << result;
    return result;
}

/**
 * @brief ParseThreadKern::formatDateTime 内核日志有年份 格式为2020-01-05 所以需要特殊转换
 * @param y 年月日
 * @param t 时间字符串
 * @return 时间毫秒数
 */
qint64 ParseThreadKern::formatDateTime(QString y, QString t)
{
    qCDebug(logApp) << "Formatting date time with year:" << y << t;
    //when /var/kern.log have the year
    QLocale local(QLocale::English, QLocale::UnitedStates);
    QString tStr = QString("%1 %2").arg(y).arg(t);
    QDateTime dt = local.toDateTime(tStr, "yyyy-MM-dd hh:mm:ss");
    qint64 result = dt.toMSecsSinceEpoch();
    qCDebug(logApp) << "Formatted time result:" << result;
    return result;
}
