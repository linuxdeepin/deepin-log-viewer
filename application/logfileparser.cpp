// SPDX-FileCopyrightText: 2019 - 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include "logfileparser.h"
#include "journalwork.h"
#include "sharedmemorymanager.h"
#include "utils.h"// add by Airy
#include "wtmpparse.h"
#include "logapplicationhelper.h"

#include <DMessageManager>

#include <QDateTime>
#include <QDebug>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QMessageBox>
#include <QProcess>
#include <QtConcurrent>
#include <QLoggingCategory>

#include <time.h>
#include <utmp.h>
#include <utmpx.h>

#ifdef QT_DEBUG
Q_LOGGING_CATEGORY(logFileParser, "org.deepin.log.viewer.parser")
#else
Q_LOGGING_CATEGORY(logFileParser, "org.deepin.log.viewer.parser", QtInfoMsg)
#endif

int journalWork::thread_index = 0;
int JournalBootWork::thread_index = 0;
DWIDGET_USE_NAMESPACE

LogFileParser::LogFileParser(QWidget *parent)
    : QObject(parent)
{
    m_dateDict.clear();
    m_dateDict.insert("Jan", "1月");
    m_dateDict.insert("Feb", "2月");
    m_dateDict.insert("Mar", "3月");
    m_dateDict.insert("Apr", "4月");
    m_dateDict.insert("May", "5月");
    m_dateDict.insert("Jun", "6月");
    m_dateDict.insert("Jul", "7月");
    m_dateDict.insert("Aug", "8月");
    m_dateDict.insert("Sep", "9月");
    m_dateDict.insert("Oct", "10月");
    m_dateDict.insert("Nov", "11月");
    m_dateDict.insert("Dec", "12月");

    // TODO::
    m_levelDict.insert("Warning", WARN);
    m_levelDict.insert("Debug", DEB);
    qRegisterMetaType<QList<LOG_MSG_KWIN> > ("QList<LOG_MSG_KWIN>");
    qRegisterMetaType<QList<LOG_MSG_XORG> > ("QList<LOG_MSG_XORG>");
    qRegisterMetaType<QList<LOG_MSG_DPKG> > ("QList<LOG_MSG_DPKG>");
    qRegisterMetaType<QList<LOG_MSG_BOOT> > ("QList<LOG_MSG_BOOT>");
    qRegisterMetaType<QList<LOG_MSG_NORMAL> > ("QList<LOG_MSG_NORMAL>");
    qRegisterMetaType<QList<LOG_MSG_DNF>>("QList<LOG_MSG_DNF>");
    qRegisterMetaType<QList<LOG_MSG_DMESG>>("QList<LOG_MSG_DMESG>");
    qRegisterMetaType<QList<LOG_MSG_AUDIT>>("QList<LOG_MSG_AUDIT>");
    qRegisterMetaType<QList<LOG_MSG_JOURNAL>>("QList<LOG_MSG_JOURNAL>");
    qRegisterMetaType<QList<LOG_MSG_COREDUMP>>("QList<LOG_MSG_COREDUMP>");
    qRegisterMetaType<LOG_FLAG> ("LOG_FLAG");

}

LogFileParser::~LogFileParser()
{
    stopAllLoad();
    //释放共享内存
    SharedMemoryManager::instance()->releaseMemory();
}

int LogFileParser::parseByJournal(const QStringList &arg)
{
    stopAllLoad();
    m_isJournalLoading = true;

#if 0
    m_currentJournalWork = journalWork::instance();

    m_currentJournalWork->stopWork();
//    journalWork   *work = new journalWork();
//    m_currentJournalWork = work;
    disconnect(m_currentJournalWork, SIGNAL(journalFinished()), this, SLOT(slot_journalFinished()));
    disconnect(m_currentJournalWork, &journalWork::journalData, this, &LogFileParser::journalData);
    m_currentJournalWork->setArg(arg);
    connect(m_currentJournalWork, SIGNAL(journalFinished()), this,  SLOT(slot_journalFinished()),
            Qt::QueuedConnection);
    connect(m_currentJournalWork, &journalWork::journalData, this, &LogFileParser::slot_journalData,
            Qt::QueuedConnection);
    connect(this, &LogFileParser::stopJournal, this, [ = ] {
        if (m_currentJournalWork)
        {
            disconnect(m_currentJournalWork, SIGNAL(journalFinished()), this,  SLOT(slot_journalFinished()));
            disconnect(m_currentJournalWork, &journalWork::journalData, this, &LogFileParser::slot_journalData);
        }
    });
    m_currentJournalWork->start();
    //QtConcurrent::run(work, &journalWork::doWork);
    // QThreadPool::globalInstance()->start(work);

#endif
#if 1
    emit stopJournal();
    journalWork *work = new journalWork(this);

    work->setArg(arg);
    auto a = connect(work, &journalWork::journalFinished, this, &LogFileParser::journalFinished,
                     Qt::QueuedConnection);
    auto b = connect(work, &journalWork::journalData, this, &LogFileParser::journalData,
                     Qt::QueuedConnection);

    connect(this, &LogFileParser::stopJournal, work, &journalWork::stopWork);

    int index = work->getIndex();
    QThreadPool::globalInstance()->start(work);
    return index;
#endif
}

int LogFileParser::parseByJournalBoot(const QStringList &arg)
{
    stopAllLoad();
    JournalBootWork *work = new JournalBootWork(this);

    work->setArg(arg);
    auto a = connect(work, &JournalBootWork::journalBootFinished, this, &LogFileParser::journalBootFinished,
                     Qt::QueuedConnection);
    auto b = connect(work, &JournalBootWork::journaBootlData, this, &LogFileParser::journaBootlData,
                     Qt::QueuedConnection);

    connect(this, &LogFileParser::stopJournalBoot, work, &JournalBootWork::stopWork);

    int index = work->getIndex();
    QThreadPool::globalInstance()->start(work);
    return index;
}

int LogFileParser::parseByDpkg(const DKPG_FILTERS &iDpkgFilter)
{

    stopAllLoad();
    LogAuthThread   *authThread = new LogAuthThread(this);
    authThread->setType(DPKG);
    QStringList filePath = DLDBusHandler::instance(this)->getFileInfo("dpkg");
    //    const QString&str="/var/log/kern";
    authThread->setFilePath(filePath);
    authThread->setFileterParam(iDpkgFilter);
    connect(authThread, &LogAuthThread::proccessError, this,
            &LogFileParser::slog_proccessError, Qt::UniqueConnection);
    connect(authThread, &LogAuthThread::dpkgFinished, this,
            &LogFileParser::dpkgFinished, Qt::UniqueConnection);
    connect(authThread, &LogAuthThread::dpkgData, this,
            &LogFileParser::dpkgData, Qt::UniqueConnection);
    connect(this, &LogFileParser::stopDpkg, authThread, &LogAuthThread::stopProccess);
    int index = authThread->getIndex();
    QThreadPool::globalInstance()->start(authThread);
    return index;
}

int LogFileParser::parseByXlog(const XORG_FILTERS &iXorgFilter)    // modifed by Airy
{
    stopAllLoad();
    LogAuthThread   *authThread = new LogAuthThread(this);
    authThread->setType(XORG);
    QStringList filePath = DLDBusHandler::instance(this)->getFileInfo("Xorg");
    authThread->setFilePath(filePath);
    authThread->setFileterParam(iXorgFilter);
    connect(authThread, &LogAuthThread::proccessError, this,
            &LogFileParser::slog_proccessError, Qt::UniqueConnection);
    connect(authThread, &LogAuthThread::xorgFinished, this,
            &LogFileParser::xlogFinished, Qt::UniqueConnection);
    connect(authThread, &LogAuthThread::xorgData, this,
            &LogFileParser::xlogData, Qt::UniqueConnection);
    connect(this, &LogFileParser::stopXlog, authThread, &LogAuthThread::stopProccess);
    int index = authThread->getIndex();
    QThreadPool::globalInstance()->tryStart(authThread);
    return index;
}

int LogFileParser::parseByNormal(const NORMAL_FILTERS &iNormalFiler)
{
    stopAllLoad();
    LogAuthThread   *authThread = new LogAuthThread(this);
    authThread->setType(Normal);
    authThread->setFileterParam(iNormalFiler);
    connect(authThread, &LogAuthThread::proccessError, this,
            &LogFileParser::slog_proccessError, Qt::UniqueConnection);
    connect(authThread, &LogAuthThread::normalFinished, this,
            &LogFileParser::normalFinished, Qt::UniqueConnection);
    connect(authThread, &LogAuthThread::normalData, this,
            &LogFileParser::normalData, Qt::UniqueConnection);
    connect(this, &LogFileParser::stopNormal, authThread, &LogAuthThread::stopProccess);
    int index = authThread->getIndex();
    QThreadPool::globalInstance()->tryStart(authThread);
    return index;
}

int LogFileParser::parseByKwin(const KWIN_FILTERS &iKwinfilter)
{
    stopAllLoad();
    LogAuthThread   *authThread = new LogAuthThread(this);
    authThread->setType(Kwin);
    authThread->setFileterParam(iKwinfilter);
    connect(authThread, &LogAuthThread::kwinFinished, this,
            &LogFileParser::kwinFinished);
    connect(authThread, &LogAuthThread::kwinData, this,
            &LogFileParser::kwinData);
    connect(this, &LogFileParser::stopKwin, authThread, &LogAuthThread::stopProccess);

    int index = authThread->getIndex();
    QThreadPool::globalInstance()->start(authThread);
    return index;
}
#if 0
void LogFileParser::parseByXlog(QStringList &xList)
{
    QProcess proc;
    proc.start("cat /var/log/Xorg.0.log");  // file path is fixed. so write cmd direct
    proc.waitForFinished(-1);

    if (isErroCommand(QString(proc.readAllStandardError())))
        return;

    QString output = proc.readAllStandardOutput();
    proc.close();

    for (QString str : output.split('\n')) {
        if (str.startsWith("[")) {
            //            xList.append(str);
            xList.insert(0, str);
        } else {
            str += " ";
            //            xList[xList.size() - 1] += str;
            xList[0] += str;
        }
    }
    createFile(output, xList.count());

    emit xlogFinished();
}
#endif

int LogFileParser::parseByBoot()
{
    stopAllLoad();
    m_isBootLoading = true;
    LogAuthThread   *authThread = new LogAuthThread(this);
    authThread->setType(BOOT);

    QStringList filePath = DLDBusHandler::instance(this)->getFileInfo("boot");
    authThread->setFilePath(filePath);
    connect(authThread, &LogAuthThread::bootFinished, this,
            &LogFileParser::bootFinished);
    connect(authThread, &LogAuthThread::bootData, this,
            &LogFileParser::bootData);
    connect(this, &LogFileParser::stopBoot, authThread,
            &LogAuthThread::stopProccess);
    int index = authThread->getIndex();
    QThreadPool::globalInstance()->start(authThread);
    return index;
}

int LogFileParser::parseByKern(const KERN_FILTERS &iKernFilter)
{
    stopAllLoad();
    m_isKernLoading = true;
    LogAuthThread   *authThread = new LogAuthThread(this);
    authThread->setType(KERN);
    QStringList filePath = DLDBusHandler::instance(this)->getFileInfo("kern", false);
    authThread->setFileterParam(iKernFilter);
    authThread->setFilePath(filePath);
    connect(authThread, &LogAuthThread::kernFinished, this,
            &LogFileParser::kernFinished);
    connect(authThread, &LogAuthThread::kernData, this,
            &LogFileParser::kernData);
    connect(this, &LogFileParser::stopKern, authThread,
            &LogAuthThread::stopProccess);
    int index = authThread->getIndex();
    QThreadPool::globalInstance()->start(authThread);
    return index;
}

int LogFileParser::parseByApp(const APP_FILTERS &iAPPFilter)
{
    // 根据应用名获取应用日志类型，缺省为log文件类型
    QString appName = Utils::appName(iAPPFilter.path);
    AppLogConfig appLogConfig = LogApplicationHelper::instance()->appLogConfig(appName);

    qCDebug(logFileParser) << QString("parsing app log, appName:%1 applogType:%2 path:%3").arg(appName).arg(appLogConfig.logType).arg(iAPPFilter.path);

    // 确定解析方式
    QString parseType = "file";
    if (appLogConfig.logType == "file" || !appLogConfig.isValid())
        parseType = "file";
    else if (appLogConfig.isValid() && appLogConfig.logType == "journal")
        parseType = "journal";

// DTKCore 5.6.8以下，不支持journal方式解析，指定按file方式解析应用日志
#if (DTK_VERSION < DTK_VERSION_CHECK(5, 6, 8, 0))
    parseType = "file";
#endif

    if (parseType == "file") {
        // file方式解析应用日志(老流程)
        stopAllLoad();
        m_isAppLoading = true;

        m_appThread = new LogApplicationParseThread(this);
        quitLogAuththread(m_appThread);

        disconnect(m_appThread, &LogApplicationParseThread::appFinished, this,
                   &LogFileParser::appFinished);
        disconnect(m_appThread, &LogApplicationParseThread::appData, this,
                   &LogFileParser::appData);
        disconnect(this, &LogFileParser::stopApp, m_appThread,
                   &LogApplicationParseThread::stopProccess);
        m_appThread->setParam(iAPPFilter);
        connect(m_appThread, &LogApplicationParseThread::appFinished, this,
                &LogFileParser::appFinished);
        connect(m_appThread, &LogApplicationParseThread::appData, this,
                &LogFileParser::appData);
        connect(this, &LogFileParser::stopApp, m_appThread,
                &LogApplicationParseThread::stopProccess);
        connect(m_appThread, &LogApplicationParseThread::finished, m_appThread,
                &QObject::deleteLater);
        int index = m_appThread->getIndex();
        m_appThread->start();
        return index;
    } else if (parseType == "journal") {
        // journal方式解析应用日志
        stopAllLoad();
        emit stopJournalApp();

        // 级别筛选
        QStringList arg;
        if (iAPPFilter.lvlFilter != LVALL) {
            QString prio = QString("PRIORITY=%1").arg(iAPPFilter.lvlFilter);
            arg.append(prio);
        } else {
            arg.append("all");
        }

        // 时间筛选
        if (iAPPFilter.timeFilterBegin != -1) {
            arg << QString::number(iAPPFilter.timeFilterBegin * 1000) << QString::number(iAPPFilter.timeFilterEnd * 1000);
        }

        // 应用筛选
        arg << appName;

        JournalAppWork* work = new JournalAppWork(this);

        // 设置筛选条件参数
        work->setArg(arg);

        connect(work, &JournalAppWork::journalAppFinished, this, &LogFileParser::appFinished, Qt::QueuedConnection);
        connect(work, &JournalAppWork::journalAppData, this, &LogFileParser::appData, Qt::QueuedConnection);
        connect(this, &LogFileParser::stopJournalApp, work, &JournalAppWork::stopWork);

        int index = work->getIndex();
        QThreadPool::globalInstance()->start(work);
        return index;
    }

    return -1;
}

void LogFileParser::parseByDnf(DNF_FILTERS iDnfFilter)
{
    stopAllLoad();
    LogAuthThread *authThread = new LogAuthThread(this);
    authThread->setType(Dnf);
    QStringList filePath = DLDBusHandler::instance(this)->getFileInfo("dnf");
    authThread->setFilePath(filePath);
    authThread->setFileterParam(iDnfFilter);
    connect(authThread, &LogAuthThread::proccessError, this,
            &LogFileParser::slog_proccessError, Qt::UniqueConnection);
    connect(authThread, &LogAuthThread::dnfFinished, this,
            &LogFileParser::dnfFinished, Qt::UniqueConnection);
    connect(this, &LogFileParser::stopDnf, authThread,
            &LogAuthThread::stopProccess);
    QThreadPool::globalInstance()->start(authThread);
}

void LogFileParser::parseByDmesg(DMESG_FILTERS iDmesgFilter)
{
    stopAllLoad();
    LogAuthThread *authThread = new LogAuthThread(this);
    authThread->setType(Dmesg);
    QStringList filePath = DLDBusHandler::instance(this)->getFileInfo("dmesg");
    authThread->setFilePath(filePath);
    authThread->setFileterParam(iDmesgFilter);
    connect(authThread, &LogAuthThread::proccessError, this,
            &LogFileParser::slog_proccessError, Qt::UniqueConnection);
    connect(authThread, &LogAuthThread::dmesgFinished, this,
            &LogFileParser::dmesgFinished, Qt::UniqueConnection);
    connect(this, &LogFileParser::stopDmesg, authThread,
            &LogAuthThread::stopProccess);
    QThreadPool::globalInstance()->start(authThread);
}

int LogFileParser::parseByOOC(const QString &path)
{
    stopAllLoad();
    m_isOOCLoading = true;

    m_OOCThread = new LogOOCFileParseThread(this);
    m_OOCThread->setParam(path);
    connect(m_OOCThread, &LogOOCFileParseThread::sigFinished, this,
            &LogFileParser::OOCFinished);
    connect(m_OOCThread, &LogOOCFileParseThread::sigData, this,
            &LogFileParser::OOCData);
    connect(this, &LogFileParser::stopOOC, m_OOCThread,
            &LogOOCFileParseThread::stopProccess);
    connect(m_OOCThread, &LogOOCFileParseThread::finished, m_OOCThread,
            &QObject::deleteLater);
    int index = m_OOCThread->getIndex();
    m_OOCThread->start();
    return index;
}

int LogFileParser::parseByAudit(const AUDIT_FILTERS &iAuditFilter)
{
    stopAllLoad();
    m_isAuditLoading = true;
    LogAuthThread   *authThread = new LogAuthThread(this);
    authThread->setType(Audit);
    QStringList filePath = DLDBusHandler::instance(this)->getFileInfo("audit", false);
    authThread->setFileterParam(iAuditFilter);
    authThread->setFilePath(filePath);
    connect(authThread, &LogAuthThread::auditFinished, this,
            &LogFileParser::auditFinished);
    connect(authThread, &LogAuthThread::auditData, this,
            &LogFileParser::auditData);
    connect(this, &LogFileParser::stopKern, authThread,
            &LogAuthThread::stopProccess);
    int index = authThread->getIndex();
    QThreadPool::globalInstance()->start(authThread);
    return index;
}

int LogFileParser::parseByCoredump(const COREDUMP_FILTERS &iCoredumpFilter)
{
    stopAllLoad();
    m_isCoredumpLoading = true;
    //qRegisterMetaType<QList<quint16>>("QList<LOG_MSG_COREDUMP>");
    LogAuthThread   *authThread = new LogAuthThread(this);
    authThread->setType(COREDUMP);
    authThread->setFileterParam(iCoredumpFilter);
    connect(authThread, &LogAuthThread::coredumpFinished, this,
            &LogFileParser::coredumpFinished);
    connect(authThread, &LogAuthThread::coredumpData, this,
            &LogFileParser::coredumpData);
    connect(this, &LogFileParser::stopCoredump, authThread, &LogAuthThread::stopProccess);
    int index = authThread->getIndex();
    QThreadPool::globalInstance()->start(authThread);
    return index;
}

void LogFileParser::createFile(const QString &output, int count)
{
#if 1
    Q_UNUSED(output)
    Q_UNUSED(count)
#else
    // this is for test parser.
    QFile fi("tempFile");
    if (!fi.open(QIODevice::ReadWrite | QIODevice::Truncate))
        return;
    fi.write(output.toLatin1());
    fi.write(QString::number(count).toLatin1());
    fi.close();
#endif
}

void LogFileParser::stopAllLoad()
{
    emit stopKern();
    emit stopBoot();
    emit stopDpkg();
    emit stopXlog();
    emit stopKwin();
    emit stopApp();
    emit stopJournalApp();
    emit stopJournal();
    emit stopJournalBoot();
    emit stopNormal();
    emit stopDnf();
    emit stopDmesg();
    emit stopOOC();
    emit stopCoredump();
    return;
}

void LogFileParser::quitLogAuththread(QThread *iThread)
{
    if (iThread && iThread->isRunning()) {
        iThread->quit();
        iThread->wait();
    }
}






#include <unistd.h>
#include <QApplication>

/**
 * @brief LogFileParser::slog_proccessError 处理转发日志获取线程的错误信息信号
 * @param iError 错误信息
 */
void LogFileParser::slog_proccessError(const QString &iError)
{
    emit proccessError(iError);
}




