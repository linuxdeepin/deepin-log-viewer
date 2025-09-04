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

#include "parsethread/parsethreadkern.h"
#include "parsethread/parsethreadkwin.h"

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
#include <QThreadPool>
#include <QThread>
#include <QLoggingCategory>

#include <time.h>
#include <utmp.h>
#include <utmpx.h>

Q_DECLARE_LOGGING_CATEGORY(logApp)

int journalWork::thread_index = 0;
int JournalBootWork::thread_index = 0;
DWIDGET_USE_NAMESPACE

LogFileParser::LogFileParser(QWidget *parent)
    : QObject(parent)
{
    qCDebug(logApp) << "LogFileParser constructor called";
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
    qCDebug(logApp) << "LogFileParser destructor called";
    stopAllLoad();
    
    // Give threads more time to respond to stop signals
    QThread::msleep(200);
    
    // Check if application is still valid before accessing QThreadPool
    if (QCoreApplication::instance() && !QCoreApplication::closingDown()) {
        qCDebug(logApp) << "Checking thread pool status...";
        // Use non-blocking approach to avoid deadlock
        QThreadPool *pool = QThreadPool::globalInstance();
        if (pool && pool->activeThreadCount() > 0) {
            qCWarning(logApp) << "Active threads detected:" << pool->activeThreadCount();
            // Clear queued tasks immediately, but don't wait for running tasks
            pool->clear();
            
            // Use shorter timeout, proceed with destruction if wait fails
            if (!pool->waitForDone(100)) {
                qCWarning(logApp) << "Thread pool cleanup timeout, proceeding with destruction";
                // Don't perform second wait to avoid deadlock
            }
        }
    } else {
        qCDebug(logApp) << "Application is closing down, skipping thread pool cleanup";
    }
    
    if (SharedMemoryManager::getInstance()) {
        qCDebug(logApp) << "Releasing shared memory";
        SharedMemoryManager::instance()->releaseMemory();
    }
}

int LogFileParser::parseByJournal(const QStringList &arg)
{
    qCDebug(logApp) << "Starting journal log parsing";
    stopAllLoad();

    emit stopJournal();
    journalWork *work = new journalWork(this);

    qCDebug(logApp) << "Setting journal parser arguments";
    work->setArg(arg);
    auto a = connect(work, &journalWork::journalFinished, this, &LogFileParser::journalFinished,
                     Qt::QueuedConnection);
    auto b = connect(work, &journalWork::journalData, this, &LogFileParser::journalData,
                     Qt::QueuedConnection);

    connect(this, &LogFileParser::stopJournal, work, &journalWork::stopWork);

    int index = work->getIndex();
    qCDebug(logApp) << "Starting journal parser thread with index:" << index;
    QThreadPool::globalInstance()->start(work);
    return index;
}

int LogFileParser::parseByJournalBoot(const QStringList &arg)
{
    qCDebug(logApp) << "Starting journal boot log parsing";
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
    qCDebug(logApp) << "Starting dpkg log parsing";
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
    qCDebug(logApp) << "Starting xorg log parsing";
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
    qCDebug(logApp) << "Starting normal log parsing";
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
    qCDebug(logApp) << "Starting kwin log parsing";
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

int LogFileParser::parseByBoot()
{
    qCDebug(logApp) << "Starting boot log parsing";
    stopAllLoad();
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

int LogFileParser::parse(LOG_FILTER_BASE &filter)
{
    qCDebug(logApp) << "Starting log parsing";
    stopAllLoad();

    ParseThreadBase *parseWork = nullptr;
    if (filter.type == KERN) {
        qCDebug(logApp) << "Starting kern log parsing";
        parseWork = new ParseThreadKern(this);
    } else if (filter.type == Kwin) {
        qCDebug(logApp) << "Starting kwin log parsing";
        parseWork = new ParseThreadKwin(this);
    }
    if (parseWork) {
        qCDebug(logApp) << "Setting filter for parse work";
        parseWork->setFilter(filter);
        int index = parseWork->getIndex();
        QThreadPool::globalInstance()->start(parseWork);
        return index;
    }
    qCDebug(logApp) << "No parse work found, returning -1";
    return -1;
}

int LogFileParser::parseByKern(const KERN_FILTERS &iKernFilter)
{
    qCDebug(logApp) << "Starting kern log parsing";
    stopAllLoad();
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
    qCDebug(logApp) << "Starting app log parsing for:" << iAPPFilter.app;
    // 根据应用名获取应用日志配置信息
    QString appName = iAPPFilter.app;
    AppLogConfig appLogConfig = LogApplicationHelper::instance()->appLogConfig(appName);

    APP_FILTERSList appFilterList;
    if (appLogConfig.subModules.size() == 1) {
        qCDebug(logApp) << "App log config has only one submodule";
        APP_FILTERS appFilter = iAPPFilter;
        // 子模块名称与应用名称显示一致，并且仅有一个子模块，
        // 则认为该应用与子模块同名称，来源列表显示为应用名称
        if (appLogConfig.subModules[0].name == appLogConfig.name)
            appFilter.submodule = "";
        else
            appFilter.submodule = appLogConfig.subModules[0].name;
        appFilter.logType = appLogConfig.subModules[0].logType;
        appFilter.filter = appLogConfig.subModules[0].filter;
        appFilter.path = appLogConfig.subModules[0].logPath;
        appFilter.execPath = appLogConfig.subModules[0].execPath;
        appFilterList.push_back(appFilter);
    } else if (appLogConfig.subModules.size() > 1) {
        qCDebug(logApp) << "App log config has multiple submodules";
        for (auto submodule : appLogConfig.subModules) {
            APP_FILTERS appFilter = iAPPFilter;
            appFilter.submodule = submodule.name;
            appFilter.logType = submodule.logType;
            appFilter.filter = submodule.filter;
            appFilter.path = submodule.logPath;
            appFilter.execPath = submodule.execPath;
            appFilterList.push_back(appFilter);
        }
    }

    if (appFilterList.size() > 0) {
        qCDebug(logApp) << "App log config has submodules, starting parsing";
        stopAllLoad();

        m_appThread = new LogApplicationParseThread(this);
        quitLogAuththread(m_appThread);

        disconnect(m_appThread, &LogApplicationParseThread::appFinished, this,
                   &LogFileParser::appFinished);
        disconnect(m_appThread, &LogApplicationParseThread::appData, this,
                   &LogFileParser::appData);
        disconnect(this, &LogFileParser::stopApp, m_appThread,
                   &LogApplicationParseThread::stopProccess);
        m_appThread->setFilters(appFilterList);
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
    }
    qCDebug(logApp) << "App log config has no submodules, returning -1";
    return -1;
}

void LogFileParser::parseByDnf(DNF_FILTERS iDnfFilter)
{
    qCDebug(logApp) << "Starting dnf log parsing";
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
    qCDebug(logApp) << "Starting dmesg log parsing";
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
    qCDebug(logApp) << "Starting ooc log parsing";
    stopAllLoad();

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
    qCDebug(logApp) << "Starting audit log parsing";
    stopAllLoad();
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

int LogFileParser::parseByCoredump(const COREDUMP_FILTERS &iCoredumpFilter, bool parseMap)
{
    qCDebug(logApp) << "Starting coredump log parsing";
    stopAllLoad();
    //qRegisterMetaType<QList<quint16>>("QList<LOG_MSG_COREDUMP>");
    LogAuthThread   *authThread = new LogAuthThread(this);
    authThread->setType(COREDUMP);
    authThread->setParseMap(parseMap);
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

void LogFileParser::stopAllLoad()
{
    qCDebug(logApp) << "Stopping all log parsers";
    emit stop();
    emit stopKern();
    emit stopBoot();
    emit stopDpkg();
    emit stopXlog();
    emit stopKwin();
    emit stopApp();
    emit stopJournal();
    emit stopJournalBoot();
    emit stopNormal();
    emit stopDnf();
    emit stopDmesg();
    emit stopOOC();
    emit stopCoredump();
    
    // 给线程一些时间来响应停止信号
    qCDebug(logApp) << "Waiting for threads to respond to stop signals";
    QThread::msleep(100);
    return;
}

void LogFileParser::quitLogAuththread(QThread *iThread)
{
    // qCDebug(logApp) << "LogFileParser::quitLogAuththread called with iThread:" << iThread;
    if (iThread && iThread->isRunning()) {
        qCDebug(logApp) << "Stopping auth thread";
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
    qCWarning(logApp) << "Log processing error:" << iError;
    emit proccessError(iError);
}




