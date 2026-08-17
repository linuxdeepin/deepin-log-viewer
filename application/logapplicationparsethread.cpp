// SPDX-FileCopyrightText: 2019 - 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "logapplicationparsethread.h"
#include "utils.h"
#include "dbusproxy/dldbushandler.h"

#include <DMessageBox>
#include <DApplication>

#include <QDateTime>
#include <QDebug>
#include <QProcess>
#include <QJsonDocument>
#include <QJsonObject>

#include <QLoggingCategory>

#ifdef QT_DEBUG
Q_LOGGING_CATEGORY(logApp, "org.deepin.log.viewer.parse.app.work")
#else
Q_LOGGING_CATEGORY(logApp, "org.deepin.log.viewer.parse.app.work", QtInfoMsg)
#endif

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
    //初始化等级数字对应显示文本的map
    initJournalMap();
    //静态计数变量加一并赋值给本对象的成员变量，以供外部判断是否为最新线程发出的数据信号
    thread_count++;
    m_threadCount = thread_count;
}

/**
 * @brief LogApplicationParseThread::~LogApplicationParseThread 析构函数，停止并销毁process指针
 */
LogApplicationParseThread::~LogApplicationParseThread()
{
    m_appList.clear();
    m_levelDict.clear();
    m_journalMap.clear();

    if (m_process) {
        m_process->kill();
        m_process->close();
        delete  m_process;
        m_process = nullptr;
    }
}

void LogApplicationParseThread::setFilters(const APP_FILTERSList &iFilters)
{
    m_AppFilers = iFilters;
}

/**
 * @brief LogApplicationParseThread::stopProccess 停止qprocess获取进程
 */
void LogApplicationParseThread::stopProccess()
{
    qCDebug(logApp) << "stopWork";
    m_canRun = false;
    if (m_process && m_process->isOpen()) {
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
    mutex.lock();
    m_appList.clear();
    mutex.unlock();

    // 遍历每个子模块对应的日志过滤配置项
    for (auto appFilter : m_AppFilers) {
        if (appFilter.logType == "file") {
            if (!parseByFile(appFilter))
                return;
        } else if (appFilter.logType == "journal") {
            if (!parseByJournal(appFilter))
                return;
        }
    }

    //最后可能有余下不足500的数据
    if (m_appList.count() >= 0) {
        emit appData(m_threadCount, m_appList);
    }

    emit appFinished(m_threadCount);
}

bool LogApplicationParseThread::parseByFile(const APP_FILTERS &app_filter)
{
    m_AppFiler = app_filter;

    initProccess();
    //connect(m_process, SIGNAL(finished(int)), m_process, SLOT(deleteLater()));
    //因为筛选信息中含有日志文件路径，所以不能为空，否则无法获取
    if (m_AppFiler.path.isEmpty()) {  //modified by Airy for bug 20457::if path is empty,item is not empty
        emit appFinished(m_threadCount);
    } else {
        QStringList filePath = DLDBusHandler::instance(this)->getFileInfo(m_AppFiler.path);
        // 如果getFileInfo的dbus调用失败(如用户取消授权)，直接返回false以中止后续子模块处理
        if (DLDBusHandler::instance(this)->isGetFileInfoError()) {
            qCWarning(logApp) << "D-Bus getFileInfo failed (authorization canceled) for submodule:"
                              << app_filter.submodule << ", aborting remaining submodules";
            emit appFinished(m_threadCount);
            return false;
        }
        for (int i = 0; i < filePath.count(); i++) {
            if (!m_canRun) {
                return false;
            }
            //按行解析
            QByteArray outByte = DLDBusHandler::instance(this)->readLog(filePath[i]).toUtf8();
            // dbus鉴权失败，不再继续解析
            if (outByte.endsWith("is not allowed to configrate firewall. checkAuthorization failed.")) {
                emit appFinished(m_threadCount);
                return false;
            }
            QString output = Utils::replaceEmptyByteArray(outByte);
            QStringList strList = QString(output ).split('\n', QString::SkipEmptyParts);
            //开启贪婪匹配
            QRegularExpression re("^(\\d{4}-[0-2]\\d-[0-3]\\d)\\D*([0-2]\\d:[0-5]\\d:[0-5]\\d.\\d*)[^A-Za-z]*([A-Za-z]*)[^\\[]*[^\\]]*\\]*\\s*(.*)$");

            for (int j = strList.size() - 1; j >= 0; --j) {
                if (!m_canRun) {
                    return false;
                }
                LOG_MSG_APPLICATOIN msg;
                QString str = strList[j];

                QRegularExpressionMatch match = re.match(str);
                bool matchRes = match.hasMatch();
                if(!matchRes){
                    continue;
                }

                QString dateTime = match.captured(1)+" "+match.captured(2);
                qint64 dt = QDateTime::fromString(dateTime, "yyyy-MM-dd hh:mm:ss.zzz").toMSecsSinceEpoch();
                //按筛选条件筛选时间段
                if (m_AppFiler.timeFilterBegin > 0 && m_AppFiler.timeFilterEnd > 0) {
                    if (dt < m_AppFiler.timeFilterBegin || dt > m_AppFiler.timeFilterEnd)
                        continue;
                }

                msg.subModule = m_AppFiler.submodule;
                msg.dateTime = dateTime;
                msg.level = match.captured(3);
                //筛选日志等级
                if (m_AppFiler.lvlFilter != LVALL) {
                    if (m_levelDict.value(msg.level) != m_AppFiler.lvlFilter)
                        continue;
                }
                //获取信息
                msg.msg=match.captured(4);
                msg.detailInfo=match.captured(4);

                //如果日志太长就显示一部分
                if (msg.detailInfo.size() > 500) {
                    msg.msg = msg.detailInfo.mid(0, 500);
                }
                mutex.lock();
                m_appList.append(msg);
                mutex.unlock();
                //每获得500个数据就发出信号给控件加载
                if (m_appList.count() % SINGLE_READ_CNT == 0) {
                    mutex.lock();
                    emit appData(m_threadCount, m_appList);
                    m_appList.clear();
                    mutex.unlock();
                }
            }
            if (!m_canRun) {
                return false;
            }
        }
    }

    return true;
}

bool LogApplicationParseThread::parseByJournal(const APP_FILTERS &app_filter)
{
    m_AppFiler = app_filter;

    if ((!m_canRun)) {
        return false;
    }

    // journal 型应用日志由非特权前端 sd_journal_open 只能读到本进程可见的 journal 子集，
    // 无法获取以 root/sudo 运行的服务（如 uos-service-support-agent）写入系统 journal 的日志。
    // 改为经 root 后端 com.deepin.logviewer 以 journalctl 取数（-o json -r），前端按结构化
    // 字段解析；过滤条件由后端安全构建参数列表（与 exportLog 一致，杜绝参数注入）。
    QJsonObject conditionsObj;
    conditionsObj["name"] = m_AppFiler.app;
    conditionsObj["filter"] = m_AppFiler.filter;
    conditionsObj["execPath"] = m_AppFiler.execPath;
    const QString conditions = QString::fromUtf8(
        QJsonDocument(conditionsObj).toJson(QJsonDocument::Compact));

    const QString output = DLDBusHandler::instance(this)->getJournalLog(conditions);
    if ((!m_canRun)) {
        return false;
    }
    // 后端无有效匹配条件或鉴权失败时返回空，视作无日志
    if (output.isEmpty())
        return true;

    // 查看是否开启通配符匹配（journalctl 无法精确过滤通配符，前端按 CODE_CATEGORY 前缀过滤）
    bool bWildcardMatch = m_AppFiler.filter.endsWith("*");
    QString wildcard_CodeCategory = "";
    if (bWildcardMatch)
        wildcard_CodeCategory = m_AppFiler.filter.split("*").first();

    // 通配符匹配规则为空，不开启通配符匹配
    if (wildcard_CodeCategory.isEmpty())
        bWildcardMatch = false;

    uint64_t beginTime = 0;
    uint64_t endTime = 0;
    if (m_AppFiler.timeFilterBegin != -1) {
        beginTime = static_cast<uint64_t>(m_AppFiler.timeFilterBegin * 1000);
        endTime = static_cast<uint64_t>(m_AppFiler.timeFilterEnd * 1000);
    }

    int cnt = 0;
    // journalctl -r -o json 输出每行一个 JSON 对象，最新在前（与原 SD_JOURNAL_FOREACH_BACKWARDS 顺序一致）
    const QStringList lines = output.split('\n', QString::SkipEmptyParts);
    for (int i = 0; i < lines.size(); ++i) {
        if ((!m_canRun)) {
            return false;
        }

        const QJsonObject obj = QJsonDocument::fromJson(lines[i].toUtf8()).object();
        if (obj.isEmpty())
            continue;

        // 获取时间：优先 _SOURCE_REALTIME_TIMESTAMP，回退 __REALTIME_TIMESTAMP（与原 sd_journal 行为一致）
        const QString srcTs = obj.value("_SOURCE_REALTIME_TIMESTAMP").toString();
        const QString recvTs = obj.value("__REALTIME_TIMESTAMP").toString();
        if (srcTs.isEmpty() && recvTs.isEmpty())
            continue;

        LOG_MSG_APPLICATOIN logMsg;
        logMsg.subModule = m_AppFiler.submodule;
        // 显示时间优先 _SOURCE_REALTIME_TIMESTAMP，回退 __REALTIME_TIMESTAMP
        logMsg.dateTime = getDateTimeFromStamp(srcTs.isEmpty() ? recvTs : srcTs);
        // 过滤时间用 __REALTIME_TIMESTAMP（与原 sd_journal_get_realtime_usec 一致），缺失时回退 source
        const uint64_t t = (recvTs.isEmpty() ? srcTs : recvTs).toULongLong();
        if (m_AppFiler.timeFilterBegin != -1) {
            if (t < beginTime || t > endTime)
                continue;
        }

        // 根据filter进行通配符匹配查找
        if (bWildcardMatch) {
            const QString code_category = obj.value("CODE_CATEGORY").toString();
            if (!code_category.startsWith(wildcard_CodeCategory))
                continue;
        }

        //获取信息体
        logMsg.msg = obj.value("MESSAGE").toString();
        logMsg.detailInfo = logMsg.msg;

        //如果日志太长就显示一部分
        if (logMsg.detailInfo.size() > 500) {
            logMsg.msg = logMsg.detailInfo.mid(0, 500);
        }

        //获取等级：0-7 对应紧急到调试，与 journalctl 筛选行为一致；缺省按调试(7)处理
        const QJsonValue prioVal = obj.value("PRIORITY");
        int prio = 7;
        if (!prioVal.isUndefined())
            prio = prioVal.toString().toInt();
        else if (m_AppFiler.lvlFilter != LVALL)
            continue;  // 缺少 PRIORITY 的条目在原 journal PRIORITY= 匹配下会被排除
        //日志等级筛选（原由 sd_journal_add_match PRIORITY= 在 journal 侧过滤，现前端过滤）
        if (m_AppFiler.lvlFilter != LVALL && prio != m_AppFiler.lvlFilter)
            continue;
        logMsg.level = i2str(prio);

        cnt++;
        mutex.lock();
        m_appList.append(logMsg);
        mutex.unlock();

        //每获得500个数据就发出信号给控件加载
        if (cnt % SINGLE_READ_CNT == 0) {
            mutex.lock();
            emit appData(m_threadCount, m_appList);
            m_appList.clear();
            mutex.unlock();
        }
    }

    return true;
}

void LogApplicationParseThread::onProcFinished(int ret)
{
    Q_UNUSED(ret)
}

QString LogApplicationParseThread::getDateTimeFromStamp(const QString &str)
{
    QString ret = "";
    QString dtstr = str.left(str.length() - 3);
    QDateTime dt = QDateTime::fromMSecsSinceEpoch(dtstr.toLongLong());
    ret = dt.toString("yyyy-MM-dd hh:mm:ss.zzz");  // + QString(".%1").arg(ums);
    return ret;
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
 * @brief LogApplicationParseThread::initMap 初始化等级数字和等级显示文本的map
 */
void LogApplicationParseThread::initJournalMap()
{
    m_journalMap.clear();
    m_journalMap.insert(0, DApplication::translate("Level", "Emergency"));
    m_journalMap.insert(1, DApplication::translate("Level", "Alert"));
    m_journalMap.insert(2, DApplication::translate("Level", "Critical"));
    m_journalMap.insert(3, DApplication::translate("Level", "Error"));
    m_journalMap.insert(4, DApplication::translate("Level", "Warning"));
    m_journalMap.insert(5, DApplication::translate("Level", "Notice"));
    m_journalMap.insert(6, DApplication::translate("Level", "Info"));
    m_journalMap.insert(7, DApplication::translate("Level", "Debug"));
}

/**
 * @brief journalAppWork::i2str 日志等级到等级显示文本的转换
 * @param prio 日志等级数字
 * @return 等级显示文
 */
QString LogApplicationParseThread::i2str(int prio)
{
    return m_journalMap.value(prio);
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
    qCDebug(logApp) << "threadrun";
    doWork();
}
