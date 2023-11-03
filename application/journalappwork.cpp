// SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "journalappwork.h"
#include "utils.h"

#include <DApplication>

#include <QDateTime>
#include <QDebug>
#include <QLoggingCategory>

#ifdef QT_DEBUG
Q_LOGGING_CATEGORY(logJournalApp, "org.deepin.log.viewer.parse.app.journal.work")
#else
Q_LOGGING_CATEGORY(logJournalApp, "org.deepin.log.viewer.parse.app.journal.work", QtInfoMsg)
#endif

DWIDGET_USE_NAMESPACE

int JournalAppWork::thread_index;
/**
 * @brief journalAppWork::journalAppWork 线程构造函数
 * @param arg 筛选参数
 * @param parent 父对象
 */
JournalAppWork::JournalAppWork(QStringList arg, QObject *parent)
    :  QObject(parent),
       QRunnable()

{
    //注册QList<LOG_MSG_APPLICATOIN>类型以让信号可以发出数据并能连接信号槽
    qRegisterMetaType<QList<LOG_MSG_APPLICATOIN> >("QList<LOG_MSG_APPLICATOIN>");
    //使用线程池启动该线程，跑完自己删自己
    setAutoDelete(true);
    //初始化等级数字对应显示文本的map
    initMap();
    //增加获取参数
    m_arg.append("-o");
    m_arg.append("json");
    if (!arg.isEmpty())
        m_arg.append(arg);
    //静态计数变量加一并赋值给本对象的成员变量，以供外部判断是否为最新线程发出的数据信号
    thread_index++;
    m_threadIndex = thread_index;
}

/**
 * @brief journalAppWork::journalAppWork 线程构造函数
 * @param parent 父对象
 */
JournalAppWork::JournalAppWork(QObject *parent)
    :  QObject(parent),
       QRunnable()

{
    qRegisterMetaType<QList<LOG_MSG_APPLICATOIN> >("QList<LOG_MSG_APPLICATOIN>");
    initMap();
    setAutoDelete(true);
    thread_index++;
    m_threadIndex = thread_index;
}

/**
 * @brief journalAppWork::~journalAppWork 析构时清空数据结构
 */
JournalAppWork::~JournalAppWork()
{
    logList.clear();
    m_map.clear();
}

/**
 * @brief journalAppWork::stopWork 停止该线程
 */
void JournalAppWork::stopWork()
{
    qCDebug(logJournalApp) << "stopWork";
    m_canRun = false;
}

/**
 * @brief journalAppWork::getIndex 获取当前对象的计数
 * @return 当前对象的计数标号
 */
int JournalAppWork::getIndex()
{
    return m_threadIndex;
}

/**
 * @brief journalAppWork::getPublicIndex 获取现在此类产生对象的个数
 * @return 此类产生对象的个数，静态成员变量
 */
int JournalAppWork::getPublicIndex()
{
    return thread_index;
}


/**
 * @brief journalAppWork::setArg 设置筛选参数
 * @param arg 筛选参数
 */
void JournalAppWork::setArg(QStringList arg)
{
    m_arg.clear();
    if (!arg.isEmpty())
        m_arg.append(arg);
}

/**a
 * @brief journalAppWork::run 线程执行函数
 */
void JournalAppWork::run()
{
    qCDebug(logJournalApp) << "threadrun";
    doWork();
}

/**
 * @brief journalAppWork::doWork 实际的获取数据逻辑
 */
void JournalAppWork::doWork()
{
    //此线程刚开始把可以继续变量置true，不然下面没法跑
    m_canRun = true;
    mutex.lock();
    logList.clear();
    mutex.unlock();
    if ((!m_canRun)) {
        mutex.unlock();
        return;
    }

    int r;

    sd_journal *j ;
    if ((!m_canRun)) {
        mutex.unlock();
        return;
    }
    //打开日志文件
    r = sd_journal_open(&j, SD_JOURNAL_LOCAL_ONLY);
    if ((!m_canRun)) {
        mutex.unlock();
        sd_journal_close(j);
        return;
    }
    //r为系统借口返回值，小于0则表示失败，直接返回
    if (r < 0) {
        fprintf(stderr, "Failed to open journal: %s\n", strerror(-r));
        return;
    }
    //从尾部开始读，这样出来数据是倒叙，符合需求
    sd_journal_seek_tail(j);
    if ((!m_canRun)) {
        mutex.unlock();
        sd_journal_close(j);
        return;
    }

    if (!m_arg.isEmpty()) {
        QString _priority = m_arg.at(0);
        //增加日志等级筛选
        if (_priority != "all")
            sd_journal_add_match(j, m_arg.at(0).toStdString().c_str(), 0);

        sd_journal_add_match(j, QString("SYSLOG_IDENTIFIER=%1").arg(m_arg.last()).toStdString().c_str(), 0);
    }
    if ((!m_canRun)) {
        mutex.unlock();
        sd_journal_close(j);
        return;
    }
    int cnt = 0;
    //调用宏开始迭代
    SD_JOURNAL_FOREACH_BACKWARDS(j) {
        if ((!m_canRun)) {
            mutex.unlock();
            sd_journal_close(j);
            return;
        }
        const char *d;
        size_t l;

        LOG_MSG_APPLICATOIN logMsg;
        //获取时间
        r = sd_journal_get_data(j, "_SOURCE_REALTIME_TIMESTAMP", reinterpret_cast<const void **>(&d), &l);
        if (r < 0) {
            r = sd_journal_get_data(j, "__REALTIME_TIMESTAMP", reinterpret_cast<const void **>(&d), &l);
            if (r < 0) {
                continue;
            }
        }
        uint64_t t;
        sd_journal_get_realtime_usec(j, &t);
        //解锁返回字符串长度上限，默认是64k，写0为无限
        // sd_journal_set_data_threshold(j, 0);
        QString dt = getReplaceColorStr(d).split("=").value(1);
        if (m_arg.size() == 4) {
            if (t < static_cast<uint64_t>(m_arg.at(1).toLongLong()) || t > static_cast<uint64_t>(m_arg.at(2).toLongLong()))
                continue;
        }
        logMsg.dateTime = getDateTimeFromStamp(dt);

        //获取信息体
        r = sd_journal_get_data(j, "MESSAGE", reinterpret_cast<const void **>(&d), &l);
        if (r < 0) {
            logMsg.msg = "";
        } else {
            //出来的数据格式为 字段名= 信息体，但是因为信息体中也可能有=号，所以要把第一个去掉，后面的用=号拼起来
            QStringList strList =    getReplaceColorStr(d).split("=");
            strList.removeFirst();
            logMsg.msg = strList.join("=");
        }
        logMsg.detailInfo = logMsg.msg;

        //如果日志太长就显示一部分
        if (logMsg.detailInfo.size() > 500) {
            logMsg.msg = logMsg.detailInfo.mid(0, 500);
        }

        //获取等级
        r = sd_journal_get_data(j, "PRIORITY", reinterpret_cast<const void **>(&d), &l);
        if (r < 0) {
            //有些时候的确会产生没有等级的日志，按照需求此时一律按调试处理，和journalctl 的筛选行为一致
            logMsg.level = i2str(7);
        } else {
            //获取等级为字段名= 数字 ，数字为0-7 ，对应紧急到调试，需要转换
            logMsg.level = i2str(getReplaceColorStr(d).split("=").value(1).toInt());
        }
        cnt++;
        mutex.lock();
        logList.append(logMsg);
        mutex.unlock();

        //每获得500个数据就发出信号给控件加载
        if (cnt % 500 == 0) {
            mutex.lock();
            emit journalAppData(m_threadIndex, logList);
            logList.clear();
            //sleep(100);
            mutex.unlock();
        }
    }
    //最后可能有余下不足500的数据
    if (logList.count() >= 0) {
        emit journalAppData(m_threadIndex, logList);
    }

    emit journalAppFinished(m_threadIndex);
    //第一次加载时这个之后的代码都不执行?故放到最后
    sd_journal_close(j);
}

/**
 * @brief journalAppWork::getReplaceColorStr 替换掉获取字符的颜色字符和特殊符号
 * @param d 原字符
 * @return  替换后的字符
 */
QString JournalAppWork::getReplaceColorStr(const char *d)
{
    QByteArray byteChar(d);
    byteChar = Utils::replaceEmptyByteArray(byteChar);
    QString d_str = QString(byteChar);
    d_str.replace(QRegExp("\\x1B\\[\\d+(;\\d+){0,2}m"), "");
    d_str.replace(QRegExp("\\002"), "");
    return  d_str;
}


/**
 * @brief journalAppWork::getDateTimeFromStamp 通过获取的时间戳转换为格式化的时间显示文本
 * @param str 接口获取的原始时间字符
 * @return  格式化的时间显示文本
 */
QString JournalAppWork::getDateTimeFromStamp(const QString &str)
{
    QString ret = "";
    QString dtstr = str.left(str.length() - 6);
    QDateTime dt = QDateTime::fromTime_t(dtstr.toUInt());
    ret = dt.toString("yyyy-MM-dd hh:mm:ss");  // + QString(".%1").arg(ums);
    return ret;
}

/**
 * @brief journalAppWork::initMap 初始化等级数字和等级显示文本的map
 */
void JournalAppWork::initMap()
{
    m_map.clear();
    m_map.insert(0, DApplication::translate("Level", "Emergency"));
    m_map.insert(1, DApplication::translate("Level", "Alert"));
    m_map.insert(2, DApplication::translate("Level", "Critical"));
    m_map.insert(3, DApplication::translate("Level", "Error"));
    m_map.insert(4, DApplication::translate("Level", "Warning"));
    m_map.insert(5, DApplication::translate("Level", "Notice"));
    m_map.insert(6, DApplication::translate("Level", "Info"));
    m_map.insert(7, DApplication::translate("Level", "Debug"));
}

/**
 * @brief journalAppWork::i2str 日志等级到等级显示文本的转换
 * @param prio 日志等级数字
 * @return 等级显示文
 */
QString JournalAppWork::i2str(int prio)
{
    return m_map.value(prio);
}
