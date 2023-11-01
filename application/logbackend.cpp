// SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "logbackend.h"
#include "logallexportthread.h"
#include "logfileparser.h"
#include "logexportthread.h"
#include "logsettings.h"
#include "utils.h"
#include "dbusmanager.h"
#include "dbusproxy/dldbushandler.h"
#include "logapplicationhelper.h"
#include "DebugTimeManager.h"
#include <sys/utsname.h>

#include <DSysInfo>

#include <QDateTime>
#include <QStandardPaths>
#include <QThreadPool>
#include <QLoggingCategory>
#include <QCoreApplication>

#ifdef QT_DEBUG
Q_LOGGING_CATEGORY(logBackend, "org.deepin.log.viewer.backend")
#else
Q_LOGGING_CATEGORY(logBackend, "org.deepin.log.viewer.backend", QtInfoMsg)
#endif

LogBackend *LogBackend::m_staticbackend = nullptr;

LogBackend *LogBackend::instance(QObject *parent)
{
    if (parent != nullptr && m_staticbackend == nullptr) {
        m_staticbackend = new LogBackend(parent);
    }

    return m_staticbackend;
}

LogBackend::~LogBackend()
{

}

LogBackend::LogBackend(QObject *parent) : QObject(parent)
{
    getLogTypes();

    m_cmdWorkDir = QDir::currentPath();
    Utils::m_mapAuditType2EventType = LogSettings::instance()->loadAuditMap();
}

void LogBackend::setCmdWorkDir(const QString &dirPath)
{
    QDir dir(dirPath);
    if (dir.exists())
        m_cmdWorkDir = dirPath;
}

int LogBackend::exportAllLogs(const QString &outDir)
{
    if(!getOutDirPath(outDir))
        return -1;

    PERF_PRINT_BEGIN("POINT-05", "export all logs");
    qCInfo(logBackend) << "exporting all logs begin.";

    // 时间
    QString dateTime = QDateTime::currentDateTime().toString("yyyyMMddHHmmss");

    // 主机名
    utsname _utsname;
    uname(&_utsname);
    QString hostname = QString(_utsname.nodename);
    QString fileFullPath = m_outPath + "/" + QString("%1_%2_all_logs.zip").arg(dateTime).arg(hostname);

    // 添加文件后缀
    if (!fileFullPath.endsWith(".zip")) {
        fileFullPath += ".zip";
    }

    LogAllExportThread *thread = new LogAllExportThread(m_logTypes, fileFullPath);
    thread->setAutoDelete(true);
    connect(thread, &LogAllExportThread::exportFinsh, this, [ = ](bool ret) {
        if (ret) {
            qCInfo(logBackend) << "exporting all logs done.";
            PERF_PRINT_END("POINT-05", "cost");
            qApp->quit();
        } else {
            qCWarning(logBackend) << "exporting all logs stoped.";
            // 导出失败，若为用户指定的新目录，应清除
            if (m_newDir) {
                QDir odir(m_outPath);
                odir.removeRecursively();
            }
            PERF_PRINT_END("POINT-05", "cost");
            qApp->exit(-1);
        }
    });
    QThreadPool::globalInstance()->start(thread);

    Utils::resetToNormalAuth(m_outPath);

    return 0;
}

int LogBackend::exportTypeLogs(const QString &outDir, const QString &type)
{
    // 输出目录有效性验证
    if(!getOutDirPath(outDir))
        return -1;

    // 日志种类有效性验证
    QString error;
    m_flag = type2Flag(type, error);
    if (NONE == m_flag) {
        qCWarning(logBackend) << error;
        return -1;
    }

    QString dateTime = QDateTime::currentDateTime().toString("yyyyMMddHHmmss");
    QString categoryOutPath = QString("%1/%2-%3/").arg(m_outPath).arg(type).arg(dateTime);

    qCInfo(logBackend) << "exporting ... type:" << type;
    bool bSuccess = true;
    switch (m_flag) {
    case JOURNAL: {
        resetCategoryOutputPath(categoryOutPath);

        DLDBusHandler::instance()->exportLog(categoryOutPath, "journalctl_system", false);
    }
    break;
    case Dmesg: {
        resetCategoryOutputPath(categoryOutPath);

        DLDBusHandler::instance()->exportLog(categoryOutPath, "dmesg", false);
    }
    break;
    case KERN: {
        QStringList logPaths = DLDBusHandler::instance()->getFileInfo("kern", false);
        if (logPaths.size() > 0) {
            resetCategoryOutputPath(categoryOutPath);

            for (auto &file: logPaths)
                DLDBusHandler::instance()->exportLog(categoryOutPath, file, true);
        } else if (logPaths.size() == 0) {
            qCWarning(logBackend) << "/var/log has not kern.log";
            bSuccess = false;
        }
    }
    break;
    case BOOT_KLU: {
        resetCategoryOutputPath(categoryOutPath);

        DLDBusHandler::instance()->exportLog(categoryOutPath, "journalctl_boot", false);
    }
    break;
    case BOOT: {
        QStringList logPaths = DLDBusHandler::instance()->getFileInfo("boot", false);
        if (!logPaths.isEmpty()) {
            resetCategoryOutputPath(categoryOutPath);

            for (auto &file: logPaths)
                DLDBusHandler::instance()->exportLog(categoryOutPath, file, true);
        } else {
            qCWarning(logBackend) << "/var/log has not boot.log";
            bSuccess = false;
        }
    }
    break;
    case DPKG: {
        QStringList logPaths = DLDBusHandler::instance()->getFileInfo("dpkg", false);
        if (!logPaths.isEmpty()) {
            resetCategoryOutputPath(categoryOutPath);

            for (auto &file: logPaths)
                DLDBusHandler::instance()->exportLog(categoryOutPath, file, true);
        } else {
            qCWarning(logBackend) << "/var/log has not dpkg.log";
            bSuccess = false;
        }
    }
    break;
    case Dnf: {
        QStringList logPaths = DLDBusHandler::instance()->getFileInfo("dnf", false);
        if (!logPaths.isEmpty()) {
            resetCategoryOutputPath(categoryOutPath);

            for (auto &file: logPaths)
                DLDBusHandler::instance()->exportLog(categoryOutPath, file, true);
        } else {
            qCWarning(logBackend) << "/var/log has not dnf.log";
            bSuccess = false;
        }
    }
    break;
    case Kwin: {
        resetCategoryOutputPath(categoryOutPath);

        DLDBusHandler::instance()->exportLog(categoryOutPath, KWIN_TREE_DATA, true);
    }
    break;
    case XORG: {
        QStringList logPaths = DLDBusHandler::instance()->getFileInfo("Xorg", false);
        if (!logPaths.isEmpty()) {
            resetCategoryOutputPath(categoryOutPath);

            for (auto &file: logPaths)
                DLDBusHandler::instance()->exportLog(categoryOutPath, file, true);
        } else {
            qCWarning(logBackend) << "/var/log has not Xorg.log";
            bSuccess = false;
        }
    }
    break;
    case APP: {
        categoryOutPath = QString("%1/%2/").arg(m_outPath).arg("apps");
        resetCategoryOutputPath(categoryOutPath);

        QMap<QString, QString> appData = LogApplicationHelper::instance()->getMap();
        for (auto &it2 : appData.toStdMap()) {
            QString appName = Utils::appName(it2.second);
            if (appName.isEmpty())
                continue;

            AppLogConfig appLogConfig = LogApplicationHelper::instance()->appLogConfig(appName);

            // 确定解析方式
            QString parseType = "";
            if (appLogConfig.logType == "file" || !appLogConfig.isValid())
                parseType = "file";
            else if (appLogConfig.isValid() && appLogConfig.logType == "journal")
                parseType = "journal";

            QFileInfo fi(it2.second);
            QString tmpSubCategoryOutPath = QString("%1/%2/").arg(categoryOutPath).arg(fi.baseName());
            Utils::mkMutiDir(tmpSubCategoryOutPath);

            if (parseType == "file") {
                QStringList logPaths = DLDBusHandler::instance(nullptr)->getOtherFileInfo(it2.second);
                logPaths.removeDuplicates();
                for (auto &file: logPaths)
                    DLDBusHandler::instance()->exportLog(tmpSubCategoryOutPath, file, true);
            } else if (parseType == "journal") {
                DLDBusHandler::instance()->exportLog(tmpSubCategoryOutPath, "journalctl_app", false);
            }
        }
    }
    break;
    case COREDUMP: {
        QStringList logPaths = DLDBusHandler::instance()->getFileInfo("coredump", false);
        if (!logPaths.isEmpty()) {
            resetCategoryOutputPath(categoryOutPath);

            for (auto &file: logPaths)
                DLDBusHandler::instance()->exportLog(categoryOutPath, file, true);
        } else {
            qCWarning(logBackend) << "/var/log has no coredump logs";
            bSuccess = false;
        }
    }
    break;
    case Normal: {
        QFile file("/var/log/wtmp");
        if (!file.exists()) {
            qCWarning(logBackend) << "/var/log has no boot shutdown event log";
            bSuccess = false;
        }

        resetCategoryOutputPath(categoryOutPath);

        DLDBusHandler::instance()->exportLog(categoryOutPath, "last", false);
    }
    break;
    case OtherLog: {
        categoryOutPath = QString("%1/%2/").arg(m_outPath).arg("others");
        resetCategoryOutputPath(categoryOutPath);

        auto otherLogListPair = LogApplicationHelper::instance()->getOtherLogList();
        for (auto &it2 : otherLogListPair) {
            QStringList logPaths = DLDBusHandler::instance(nullptr)->getOtherFileInfo(it2.at(1));
            logPaths.removeDuplicates();
            if (logPaths.size() > 1) {
                QString tmpSubCategoryOutPath = QString("%1/%2/").arg(categoryOutPath).arg(it2.at(0));
                Utils::mkMutiDir(tmpSubCategoryOutPath);
                for (auto &file : logPaths) {
                    DLDBusHandler::instance()->exportLog(tmpSubCategoryOutPath, file, true);
                }
            }
            else if (logPaths.size() == 1)
                 DLDBusHandler::instance()->exportLog(categoryOutPath, logPaths[0], true);
        }
    }
    break;
    case CustomLog: {
        auto customLogListPair = LogApplicationHelper::instance()->getCustomLogList();
        QStringList logPaths;
        for (auto &it2 : customLogListPair) {
            logPaths.append(it2.at(1));
        }

        if (logPaths.size() > 0) {
            categoryOutPath = QString("%1/%2/").arg(m_outPath).arg("customized");
            resetCategoryOutputPath(categoryOutPath);

            for (auto &file : logPaths) {
                DLDBusHandler::instance()->exportLog(categoryOutPath, file, true);
            }
        } else {
            qCWarning(logBackend) << "no custom logs";
            bSuccess = false;
        }
    }
    break;
    case Audit: {
        QStringList logPaths = DLDBusHandler::instance()->getFileInfo("audit", false);
        if (!logPaths.isEmpty()) {
            resetCategoryOutputPath(categoryOutPath);

            for (auto &file: logPaths)
                DLDBusHandler::instance()->exportLog(categoryOutPath, file, true);
        } else {
            qCWarning(logBackend) << "/var/log has no audit logs";
            bSuccess = false;
        }
    }
    break;
    default:
        break;
    }

    Utils::resetToNormalAuth(categoryOutPath);

    if (bSuccess)
        qCInfo(logBackend) << QString("export success.");
    else
        qCInfo(logBackend) << QString("export failed.");

    return bSuccess ? 0 : -1;
}

bool LogBackend::LogBackend::exportTypeLogsByCondition(const QString &outDir, const QString &type, const QString &period, const QString &condition, const QString &keyword)
{
    if (!getOutDirPath(outDir))
        return false;

    // 日志种类有效性验证
    QString error;
    m_flag = type2Flag(type, error);
    if (NONE == m_flag) {
        qCWarning(logBackend) << error;
        return false;
    }

    qCInfo(logBackend) << "exporting ... type:" << type;

    m_currentSearchStr = keyword;

    // 解析数据
    if (!parseData(m_flag, period, condition)) {
        qCWarning(logBackend) << QString("parse data failed.");
        return false;
    }

    m_bNeedExport = true;
    return true;
}

int LogBackend::exportAppLogs(const QString &outDir, const QString &appName)
{
    if(!getOutDirPath(outDir))
        return -1;

    if (appName.isEmpty())
        return -1;

    // 先查找是否有该应用相关日志
    if (!LogApplicationHelper::instance()->isValidAppName(appName)) {
        qCWarning(logBackend) << QString("unknown app:%1 is invalid.").arg(appName);
        return -1;
    }

    QString logPath = getApplogPath(appName);
    if (logPath.isEmpty()) {
        qCWarning(logBackend) << QString("app:%1 log path is null.").arg(appName);
        return -1;
    }

    qCInfo(logBackend) << QString("exporting %1 logs...").arg(appName);

    bool bSuccess = true;
    AppLogConfig appLogConfig = LogApplicationHelper::instance()->appLogConfig(appName);
    // 确定解析方式
    QString parseType = "";
    if (appLogConfig.logType == "file" || !appLogConfig.isValid())
        parseType = "file";
    else if (appLogConfig.isValid() && appLogConfig.logType == "journal")
        parseType = "journal";

    QString categoryOutPath = QString("%1/%2").arg(m_outPath).arg(appName);
    if (parseType == "file") {
        QStringList logPaths = DLDBusHandler::instance(nullptr)->getOtherFileInfo(logPath);
        logPaths.removeDuplicates();

        if (logPaths.size() > 0) {
            resetCategoryOutputPath(categoryOutPath);

            for (auto &file: logPaths)
                DLDBusHandler::instance()->exportLog(categoryOutPath, file, true);
        } else {
            qCWarning(logBackend) << QString("app:%1 not found log files.").arg(appName);
            bSuccess = false;
        }
    } else if (parseType == "journal") {
        resetCategoryOutputPath(categoryOutPath);

        DLDBusHandler::instance()->exportLog(categoryOutPath, "journalctl_app", false);
    }

    Utils::resetToNormalAuth(categoryOutPath);

    if (bSuccess)
        qCInfo(logBackend) << QString("export success.");
    else
        qCInfo(logBackend) << QString("export failed.");

    return bSuccess ? 0 : -1;
}

bool LogBackend::exportAppLogsByCondition(const QString &outDir, const QString &appName, const QString &period, const QString &level, const QString &keyword)
{
    if(!getOutDirPath(outDir))
        return false;

    if (appName.isEmpty())
        return false;

    // 周期类型有效性验证
    BUTTONID periodId = ALL;
    if (!period.isEmpty()) {
        periodId = period2Enum(period);
        if (INVALID == periodId) {
            qCWarning(logBackend) << "invalid 'period' parameter: " << period << "\nUSEAGE: all(export all), today(export today), 3d(export past 3 days), 1w(export past week), 1m(export past month), 3m(export past 3 months)";
            return false;
        }
    }

    // 级别有效性判断
    if (!level.isEmpty() && level2Id(level) == -2) {
        qCWarning(logBackend) << "invalid 'level' parameter: " << level << "\nUSEAGE: 0(emerg), 1(alert), 2(crit), 3(error), 4(warning), 5(notice), 6(info), 7(debug)";
        return false;
    }

    qCInfo(logBackend) << "appName:" << appName << "period:" << period << "level:" << level << "keyword:" << keyword;

    TIME_RANGE timeRange = getTimeRange(periodId);

    // 先查找是否有该应用相关日志
    if (!LogApplicationHelper::instance()->isValidAppName(appName)) {
        qCWarning(logBackend) << QString("unknown app:%1 is invalid.").arg(appName);
        return false;
    }

    QString logPath = getApplogPath(appName);
    if (logPath.isEmpty()) {
        qCWarning(logBackend) << QString("app:%1 log path is null.").arg(appName);
        return false;
    }

    // 解析器准备工作
    initParser();
    if (!m_pParser)
        return false;

    m_flag = APP;
    m_curAppLog = logPath;
    m_bNeedExport = true;
    m_currentSearchStr = keyword;

    APP_FILTERS appFilter;
    appFilter.path = logPath;
    appFilter.lvlFilter = level2Id(level);
    appFilter.timeFilterBegin = timeRange.begin;
    appFilter.timeFilterEnd = timeRange.end;
    m_appCurrentIndex = m_pParser->parseByApp(appFilter);

    return true;
}

QStringList LogBackend::getLogTypes()
{
    Dtk::Core::DSysInfo::UosEdition edition = Dtk::Core::DSysInfo::uosEditionType();
    //等于服务器行业版或欧拉版(centos)
    bool isCentos = Dtk::Core::DSysInfo::UosEuler == edition || Dtk::Core::DSysInfo::UosEnterpriseC == edition || Dtk::Core::DSysInfo::UosMilitaryS == edition;
    if (QFile::exists("/var/log/journal") || isCentos) {
        m_logTypes.push_back(JOUR_TREE_DATA);
    }

    if (isCentos) {
        m_logTypes.push_back(DMESG_TREE_DATA);
    } else {
        if (QFile::exists("/var/log/kern.log")) {
            m_logTypes.push_back(KERN_TREE_DATA);
        }
    }
    if (DBusManager::isSpecialComType()) {
        m_logTypes.push_back(BOOT_KLU_TREE_DATA);
    } else {
        m_logTypes.push_back(BOOT_TREE_DATA);
    }
    if (isCentos) {
        m_logTypes.push_back(DNF_TREE_DATA);
    } else {
        if (QFile::exists("/var/log/dpkg.log")) {
            m_logTypes.push_back(DPKG_TREE_DATA);
        }
    }
    //w515是新版本内核的panguv返回值  panguV是老版本
    if (DBusManager::isSpecialComType()) {
        m_logTypes.push_back(KWIN_TREE_DATA);
    } else {
        m_logTypes.push_back(XORG_TREE_DATA);
    }
    auto *appHelper = LogApplicationHelper::instance();
    QMap<QString, QString> appMap = appHelper->getMap();
    if (!appMap.isEmpty()) {
        m_logTypes.push_back(APP_TREE_DATA);
    }

    m_logTypes.push_back(COREDUMP_TREE_DATA);

    // add by Airy
    if (QFile::exists("/var/log/wtmp")) {
        m_logTypes.push_back(LAST_TREE_DATA);
    }

    //other
    m_logTypes.push_back(OTHER_TREE_DATA);

    //custom
    if (LogApplicationHelper::instance()->getCustomLogList().size()) {
        m_logTypes.push_back(CUSTOM_TREE_DATA);
    }

    // 审计日志文件存在，才加载和显示审计日志模块（审计日志文件需要root权限访问，因此在service服务中判断审计日志文件是否存在）
    if (DLDBusHandler::instance(qApp)->isFileExist(AUDIT_TREE_DATA)) {
        m_logTypes.push_back(AUDIT_TREE_DATA);
    }

    return m_logTypes;
}

void LogBackend::slot_dpkgFinished(int index)
{
    if (m_flag != DPKG || index != m_dpkgCurrentIndex)
        return;
    m_isDataLoadComplete = true;

    if (m_bNeedExport) {
        exportData();
    }
}

void LogBackend::slot_dpkgData(int index, QList<LOG_MSG_DPKG> list)
{
    if (m_flag != DPKG || index != m_dpkgCurrentIndex)
        return;

    dListOrigin.append(list);
    dList.append(filterDpkg(m_currentSearchStr, list));
}

void LogBackend::slot_XorgFinished(int index)
{
    if (m_flag != XORG || index != m_xorgCurrentIndex)
        return;
    m_isDataLoadComplete = true;

    if (m_bNeedExport) {
        exportData();
    }
}

void LogBackend::slot_xorgData(int index, QList<LOG_MSG_XORG> list)
{
    if (m_flag != XORG || index != m_xorgCurrentIndex)
        return;

    xListOrigin.append(list);
    xList.append(filterXorg(m_currentSearchStr, list));
}

void LogBackend::slot_bootFinished(int index)
{
    if (m_flag != BOOT || index != m_bootCurrentIndex)
        return;
    m_isDataLoadComplete = true;

    if (m_bNeedExport) {
        exportData();
    }
}

void LogBackend::slot_bootData(int index, QList<LOG_MSG_BOOT> list)
{
    if (m_flag != BOOT || index != m_bootCurrentIndex)
        return;

    bList.append(list);
    currentBootList.append(filterBoot(m_bootFilter, list));
}

void LogBackend::slot_kernFinished(int index)
{
    if (m_flag != KERN || index != m_kernCurrentIndex)
        return;
    m_isDataLoadComplete = true;

    if (m_bNeedExport) {
        exportData();
    }
}

void LogBackend::slot_kernData(int index, QList<LOG_MSG_JOURNAL> list)
{
    if (m_flag != KERN || index != m_kernCurrentIndex)
        return;

    kListOrigin.append(list);
    kList.append(filterKern(m_currentSearchStr, list));
}

void LogBackend::slot_kwinFinished(int index)
{
    if (m_flag != Kwin || index != m_kwinCurrentIndex)
        return;
    m_isDataLoadComplete = true;

    if (m_bNeedExport) {
        exportData();
    }
}

void LogBackend::slot_kwinData(int index, QList<LOG_MSG_KWIN> list)
{
    if (m_flag != Kwin || index != m_kwinCurrentIndex)
        return;
    m_kwinList.append(list);
    m_currentKwinList.append(filterKwin(m_currentSearchStr, list));
}

void LogBackend::slot_dnfFinished(const QList<LOG_MSG_DNF> &list)
{
    if (m_flag != Dnf)
        return;
    dnfList = filterDnf(m_currentSearchStr, list);
    dnfListOrigin = list;

    m_isDataLoadComplete = true;

    if (m_bNeedExport) {
        exportData();
    }
}

void LogBackend::slot_dmesgFinished(const QList<LOG_MSG_DMESG> &list)
{
    if (m_flag != Dmesg)
        return;

    dmesgList = filterDmesg(m_currentSearchStr,list);
    dmesgListOrigin = list;

    m_isDataLoadComplete = true;

    if (m_bNeedExport) {
        exportData();
    }
}

void LogBackend::slot_journalFinished(int index)
{
    if (m_flag != JOURNAL || index != m_journalCurrentIndex)
        return;
    m_isDataLoadComplete = true;

    if (m_bNeedExport) {
        exportData();
    }
}

void LogBackend::slot_journalBootFinished(int index)
{
    if (m_flag != BOOT_KLU || index != m_journalBootCurrentIndex)
        return;
    m_isDataLoadComplete = true;

    if (m_bNeedExport) {
        exportData();
    }
}

void LogBackend::slot_journalBootData(int index, QList<LOG_MSG_JOURNAL> list)
{
    if (m_flag != BOOT_KLU || index != m_journalBootCurrentIndex)
        return;

    jBootListOrigin.append(list);
    jBootList.append(filterJournalBoot(m_currentSearchStr, list));
}

void LogBackend::slot_journalData(int index, QList<LOG_MSG_JOURNAL> list)
{
    //判断最近一次获取数据线程的标记量,和信号曹发来的sender的标记量作对比,如果相同才可以刷新,因为会出现上次的获取线程就算停下信号也发出来了
    if (m_flag != JOURNAL || index != m_journalCurrentIndex)
        return;

    jListOrigin.append(list);
    jList.append(filterJournal(m_currentSearchStr, list));
}

void LogBackend::slot_applicationFinished(int index)
{
    if (m_flag != APP || index != m_appCurrentIndex)
        return;
    m_isDataLoadComplete = true;

    if (m_bNeedExport) {
        exportData();
    }
}

void LogBackend::slot_applicationData(int index, QList<LOG_MSG_APPLICATOIN> list)
{
    if (m_flag != APP || index != m_appCurrentIndex)
        return;

    appListOrigin.append(list);
    appList.append(filterApp(m_currentSearchStr, list));
}

void LogBackend::slot_normalFinished(int index)
{
    if (m_flag != Normal || index != m_normalCurrentIndex)
        return;
    m_isDataLoadComplete = true;

    if (m_bNeedExport) {
        exportData();
    }
}

void LogBackend::slot_normalData(int index, QList<LOG_MSG_NORMAL> list)
{
    if (m_flag != Normal || index != m_normalCurrentIndex)
        return;
    norList.append(list);
    nortempList.append(filterNomal(m_normalFilter, list));
}

void LogBackend::slot_auditFinished(int index, bool bShowTip)
{
    Q_UNUSED(bShowTip);

    if (m_flag != Audit || index != m_auditCurrentIndex)
        return;
    m_isDataLoadComplete = true;


    if (m_bNeedExport) {
        exportData();
    }
}

void LogBackend::slot_auditData(int index, QList<LOG_MSG_AUDIT> list)
{
    if (m_flag != Audit || index != m_auditCurrentIndex)
        return;

    aListOrigin.append(list);
    aList.append(filterAudit(m_auditFilter, list));
}

void LogBackend::slot_coredumpFinished(int index)
{
    if (m_flag != COREDUMP || index != m_coredumpCurrentIndex)
        return;

    m_isDataLoadComplete = true;

    if (m_bNeedExport) {
        exportData();
    }
}

void LogBackend::slot_coredumpData(int index, QList<LOG_MSG_COREDUMP> list)
{
    if (m_flag != COREDUMP || index != m_coredumpCurrentIndex)
        return;

    m_coredumpList.append(list);
    m_currentCoredumpList.append(filterCoredump(m_currentSearchStr, list));
}

void LogBackend::slot_logLoadFailed(const QString &iError)
{
    qCWarning(logBackend) << "parse data failed. error: " << iError;
    qApp->exit(-1);
}

void LogBackend::onExportProgress(int nCur, int nTotal)
{
    Q_UNUSED(nCur);
    Q_UNUSED(nTotal);
}

void LogBackend::onExportResult(bool isSuccess)
{
    Utils::resetToNormalAuth(m_outPath);

    if (isSuccess) {
        qCInfo(logBackend) << "export success.";
        qApp->quit();
    } else {
        qCWarning(logBackend) << "export failed.";
        qApp->exit(-1);
    }
}

QList<LOG_MSG_BOOT> LogBackend::filterBoot(BOOT_FILTERS ibootFilter, const QList<LOG_MSG_BOOT> &iList)
{
    QList<LOG_MSG_BOOT> rsList;
    bool isStatusFilterEmpty = ibootFilter.statusFilter.isEmpty();
    if (isStatusFilterEmpty && ibootFilter.searchstr.isEmpty()) {
        return iList;
    } else {
        for (int i = 0; i < iList.size(); i++) {
            LOG_MSG_BOOT msg = iList.at(i);
            QString _statusStr = msg.status;
            if ((_statusStr.compare(ibootFilter.statusFilter, Qt::CaseInsensitive) != 0) && !isStatusFilterEmpty)
                continue;
            if ((msg.status.contains(ibootFilter.searchstr, Qt::CaseInsensitive)) || (msg.msg.contains(ibootFilter.searchstr, Qt::CaseInsensitive))) {
                rsList.append(iList[i]);
            }
        }
    }
    return rsList;
}

QList<LOG_MSG_NORMAL> LogBackend::filterNomal(NORMAL_FILTERS inormalFilter, QList<LOG_MSG_NORMAL> &iList)
{
    QList<LOG_MSG_NORMAL> rsList;
    if (inormalFilter.searchstr.isEmpty() && inormalFilter.eventTypeFilter < 0) {
        return iList;
    }
    int tcbx = inormalFilter.eventTypeFilter;
    if (0 == tcbx) {
        for (auto i = 0; i < iList.size(); i++) {
            LOG_MSG_NORMAL msg = iList.at(i);
            if (msg.eventType.contains(inormalFilter.searchstr, Qt::CaseInsensitive) || msg.userName.contains(inormalFilter.searchstr, Qt::CaseInsensitive) || msg.dateTime.contains(inormalFilter.searchstr, Qt::CaseInsensitive) || msg.msg.contains(inormalFilter.searchstr, Qt::CaseInsensitive)) {
                rsList.append(msg);
            }
        }
    } else if (1 == tcbx) {
        for (auto i = 0; i < iList.size(); i++) {
            LOG_MSG_NORMAL msg = iList.at(i);
            if (msg.eventType.contains(inormalFilter.searchstr, Qt::CaseInsensitive) || msg.userName.contains(inormalFilter.searchstr, Qt::CaseInsensitive) || msg.dateTime.contains(inormalFilter.searchstr, Qt::CaseInsensitive) || msg.msg.contains(inormalFilter.searchstr, Qt::CaseInsensitive)) {
                if (msg.eventType.compare("Boot", Qt::CaseInsensitive) != 0 && msg.eventType.compare("shutdown", Qt::CaseInsensitive) != 0 && msg.eventType.compare("runlevel", Qt::CaseInsensitive) != 0)
                    rsList.append(msg);
            }
        }
    } else if (2 == tcbx) {
        for (auto i = 0; i < iList.size(); i++) {
            LOG_MSG_NORMAL msg = iList.at(i);
            if (msg.eventType.contains(inormalFilter.searchstr, Qt::CaseInsensitive) || msg.userName.contains(inormalFilter.searchstr, Qt::CaseInsensitive) || msg.dateTime.contains(inormalFilter.searchstr, Qt::CaseInsensitive) || msg.msg.contains(inormalFilter.searchstr, Qt::CaseInsensitive)) {
                if (iList[i].eventType.compare("Boot", Qt::CaseInsensitive) == 0)
                    rsList.append(iList[i]);
            }
        }
    } else if (3 == tcbx) {
        for (auto i = 0; i < iList.size(); i++) {
            LOG_MSG_NORMAL msg = iList.at(i);
            if (msg.eventType.contains(inormalFilter.searchstr, Qt::CaseInsensitive) || msg.userName.contains(inormalFilter.searchstr, Qt::CaseInsensitive) || msg.dateTime.contains(inormalFilter.searchstr, Qt::CaseInsensitive) || msg.msg.contains(inormalFilter.searchstr, Qt::CaseInsensitive)) {
                if (iList[i].eventType.compare("shutdown", Qt::CaseInsensitive) == 0)
                    rsList.append(iList[i]);
            }
        }
    }
    return rsList;
}

QList<LOG_MSG_DPKG> LogBackend::filterDpkg(const QString &iSearchStr, const QList<LOG_MSG_DPKG> &iList)
{
    QList<LOG_MSG_DPKG> rsList;
    if (iSearchStr.isEmpty()) {
        return iList;
    }

    for (int i = 0; i < iList.size(); i++) {
        LOG_MSG_DPKG msg = iList.at(i);
        if (msg.dateTime.contains(iSearchStr, Qt::CaseInsensitive) || msg.msg.contains(iSearchStr, Qt::CaseInsensitive)) {
            rsList.append(msg);
        }
    }
    return rsList;
}

QList<LOG_MSG_JOURNAL> LogBackend::filterKern(const QString &iSearchStr, const QList<LOG_MSG_JOURNAL> &iList)
{
    QList<LOG_MSG_JOURNAL> rsList;
    if (iSearchStr.isEmpty()) {
        return iList;
    }
    for (int i = 0; i < iList.size(); i++) {
        LOG_MSG_JOURNAL msg = iList.at(i);
        if (msg.dateTime.contains(iSearchStr, Qt::CaseInsensitive) || msg.hostName.contains(iSearchStr, Qt::CaseInsensitive) || msg.daemonName.contains(iSearchStr, Qt::CaseInsensitive) || msg.msg.contains(iSearchStr, Qt::CaseInsensitive)) {
            rsList.append(msg);
        }
    }
    return rsList;
}

QList<LOG_MSG_XORG> LogBackend::filterXorg(const QString &iSearchStr, const QList<LOG_MSG_XORG> &iList)
{
    QList<LOG_MSG_XORG> rsList;
    if (iSearchStr.isEmpty()) {
        return iList;
    }
    for (int i = 0; i < iList.size(); i++) {
        LOG_MSG_XORG msg = iList.at(i);
        if (msg.offset.contains(iSearchStr, Qt::CaseInsensitive) || msg.msg.contains(iSearchStr, Qt::CaseInsensitive))
            rsList.append(msg);
    }
    return rsList;
}

QList<LOG_MSG_KWIN> LogBackend::filterKwin(const QString &iSearchStr, const QList<LOG_MSG_KWIN> &iList)
{
    QList<LOG_MSG_KWIN> rsList;
    if (iSearchStr.isEmpty()) {
        return iList;
    }
    for (int i = 0; i < iList.size(); i++) {
        LOG_MSG_KWIN msg = iList.at(i);
        if (msg.msg.contains(iSearchStr, Qt::CaseInsensitive))
            rsList.append(msg);
    }
    return rsList;
}

QList<LOG_MSG_APPLICATOIN> LogBackend::filterApp(const QString &iSearchStr, const QList<LOG_MSG_APPLICATOIN> &iList)
{
    QList<LOG_MSG_APPLICATOIN> rsList;
    if (iSearchStr.isEmpty()) {
        return iList;
    }
    for (int i = 0; i < iList.size(); i++) {
        LOG_MSG_APPLICATOIN msg = iList.at(i);
        if (msg.dateTime.contains(iSearchStr, Qt::CaseInsensitive) || msg.level.contains(iSearchStr, Qt::CaseInsensitive) || msg.src.contains(iSearchStr, Qt::CaseInsensitive) || msg.msg.contains(iSearchStr, Qt::CaseInsensitive))
            rsList.append(msg);
    }
    return rsList;
}

QList<LOG_MSG_DNF> LogBackend::filterDnf(const QString &iSearchStr, const QList<LOG_MSG_DNF> &iList)
{
    QList<LOG_MSG_DNF> rsList;
    if (iSearchStr.isEmpty()) {
        return iList;
    }
    for (int i = 0; i < iList.size(); i++) {
        LOG_MSG_DNF msg = iList.at(i);
        if (msg.dateTime.contains(m_currentSearchStr, Qt::CaseInsensitive)
                || msg.msg.contains(m_currentSearchStr, Qt::CaseInsensitive)
                || msg.level.contains(m_currentSearchStr, Qt::CaseInsensitive))
            rsList.append(msg);
    }
    return rsList;
}

QList<LOG_MSG_DMESG> LogBackend::filterDmesg(const QString &iSearchStr, const QList<LOG_MSG_DMESG> &iList)
{
    QList<LOG_MSG_DMESG> rsList;
    if (iSearchStr.isEmpty()) {
        return iList;
    }

    for (int i = 0; i < iList.size(); i++) {
        LOG_MSG_DMESG msg = iList.at(i);
        if (msg.dateTime.contains(m_currentSearchStr, Qt::CaseInsensitive) || msg.msg.contains(m_currentSearchStr, Qt::CaseInsensitive))
            rsList.append(msg);
    }

    return rsList;
}

QList<LOG_MSG_JOURNAL> LogBackend::filterJournal(const QString &iSearchStr, const QList<LOG_MSG_JOURNAL> &iList)
{
    QList<LOG_MSG_JOURNAL> rsList;
    if (iSearchStr.isEmpty()) {
        return iList;
    }
    for (int i = 0; i < iList.size(); i++) {
        LOG_MSG_JOURNAL msg = iList.at(i);
        if (msg.dateTime.contains(iSearchStr, Qt::CaseInsensitive) || msg.hostName.contains(iSearchStr, Qt::CaseInsensitive) || msg.daemonName.contains(iSearchStr, Qt::CaseInsensitive) || msg.daemonId.contains(iSearchStr, Qt::CaseInsensitive) || msg.level.contains(iSearchStr, Qt::CaseInsensitive) || msg.msg.contains(iSearchStr, Qt::CaseInsensitive))
            rsList.append(msg);
    }
    return rsList;
}

QList<LOG_MSG_JOURNAL> LogBackend::filterJournalBoot(const QString &iSearchStr, const QList<LOG_MSG_JOURNAL> &iList)
{
    QList<LOG_MSG_JOURNAL> rsList;
    if (iSearchStr.isEmpty()) {
        return iList;
    }
    for (int i = 0; i < iList.size(); i++) {
        LOG_MSG_JOURNAL msg = iList.at(i);
        if (msg.dateTime.contains(iSearchStr, Qt::CaseInsensitive) || msg.hostName.contains(iSearchStr, Qt::CaseInsensitive) || msg.daemonName.contains(iSearchStr, Qt::CaseInsensitive) || msg.daemonId.contains(iSearchStr, Qt::CaseInsensitive) || msg.level.contains(iSearchStr, Qt::CaseInsensitive) || msg.msg.contains(iSearchStr, Qt::CaseInsensitive))
            rsList.append(msg);
    }
    return rsList;
}

QList<LOG_MSG_AUDIT> LogBackend::filterAudit(AUDIT_FILTERS auditFilter, const QList<LOG_MSG_AUDIT> &iList)
{
    QList<LOG_MSG_AUDIT> rsList;
    if (auditFilter.searchstr.isEmpty() && auditFilter.auditTypeFilter < -1) {
        return iList;
    }
    int nAuditType = auditFilter.auditTypeFilter - 1;
    for (int i = 0; i < iList.size(); i++) {
        LOG_MSG_AUDIT msg = iList.at(i);
        if (msg.contains(auditFilter.searchstr) && (nAuditType == -1 || msg.filterAuditType(nAuditType))) {
            rsList.append(msg);
        }
    }

    return rsList;
}
QList<LOG_MSG_COREDUMP> LogBackend::filterCoredump(const QString &iSearchStr, const QList<LOG_MSG_COREDUMP> &iList)
{
    QList<LOG_MSG_COREDUMP> rsList;
    if (iSearchStr.isEmpty()) {
        return iList;
    }
    for (int i = 0; i < iList.size(); i++) {
        LOG_MSG_COREDUMP msg = iList.at(i);
        if (msg.sig.contains(iSearchStr, Qt::CaseInsensitive)
                || msg.dateTime.contains(iSearchStr, Qt::CaseInsensitive)
                || msg.coreFile.contains(iSearchStr, Qt::CaseInsensitive)
                || msg.uid.contains(iSearchStr, Qt::CaseInsensitive)
                || msg.exe.contains(iSearchStr, Qt::CaseInsensitive)) {
            rsList.append(msg);
        }
    }

    return rsList;
}

bool LogBackend::parseData(const LOG_FLAG &flag, const QString &period, const QString &condition)
{
    // 周期类型有效性验证
    BUTTONID periodId = ALL;
    if (!period.isEmpty()) {
        periodId = period2Enum(period);
        if (INVALID == periodId) {
            qCWarning(logBackend) << "invalid 'period' parameter: " << period << "\nUSEAGE: all(export all), today(export today), 3d(export past 3 days), 1w(export past week), 1m(export past month), 3m(export past 3 months)";
            return false;
        }
    }

    // 常规级别有效性判断
    if (flag == JOURNAL || flag == Dmesg || flag == BOOT_KLU || flag == APP) {
        if (!condition.isEmpty() && level2Id(condition) == -2) {
            qCWarning(logBackend) << "invalid 'level' parameter: " << condition << "\nUSEAGE: 0(emerg), 1(alert), 2(crit), 3(error), 4(warning), 5(notice), 6(info), 7(debug)";
            return false;
        }

    }

    // dnf级别有效性判断
    if (flag == Dnf) {
        if (!condition.isEmpty() && dnfLevel2Id(condition) == DNFINVALID) {
            qCWarning(logBackend) << "invalid 'level' parameter: " << condition << "\nUSEAGE: 0(supercrit), 1(crit), 2(error), 3(warning), 4(info), 5(debug), 6(trace)";
            return false;
        }
    }

    // boot status有效性判断
    QString statusFilter = "";
    if (flag == BOOT) {
        if (condition == "0")
            statusFilter = "";
        else if (condition == "ok" || condition == "1")
            statusFilter = "OK";
        else if (condition == "failed" || condition == "2")
            statusFilter = "Failed";
        else {
            qCWarning(logBackend) << "invalid 'status' parameter: " << condition << "\nUSEAGE: 0(export all), 1(export ok), 2(export failed)";
            return false;
        }
    }

    // boot-shutdown-event event类型有效性判断
    if (flag == Normal) {
        if (normal2eventType(condition) == -1) {
            qCWarning(logBackend) << "invalid 'event' parameter: " << condition << "\nUSEAGE: 0(export all), 1(export login), 2(export boot), 3(shutdown)";
            return false;
        }
    }

    // audit event有效性判断
    if (flag == Audit) {
        if (audit2eventType(condition) == -1) {
            qCWarning(logBackend) << "invalid 'event' parameter: " << condition << "\nUSEAGE: 0(all), 1(ident auth), 2(discretionary access Contro), 3(mandatory access control), 4(remote), 5(doc audit), 6(other)";
            return false;
        }
    }

    qCInfo(logBackend) << "parsing ..." << "period:" << period << "condition:" << condition << "keyword:" << m_currentSearchStr;

    TIME_RANGE timeRange = getTimeRange(periodId);
    int lId = level2Id(condition);

    // 解析器准备工作
    initParser();
    if (!m_pParser)
        return false;

    // 设置筛选条件，解析数据
    switch (flag) {
    case JOURNAL: {
        QStringList arg;
        if (lId != LVALL) {
            QString prio = QString("PRIORITY=%1").arg(lId);
            arg.append(prio);
        } else {
            arg.append("all");
        }

        if (timeRange.begin != -1 && timeRange.end != -1) {
            arg << QString::number(timeRange.begin * 1000) << QString::number(timeRange.end *1000);
        }

        m_journalCurrentIndex = m_pParser->parseByJournal(arg);
    }
    break;
    case Dmesg: {
        DMESG_FILTERS dmesgfilter;
        dmesgfilter.levelFilter = static_cast<PRIORITY>(lId);
        dmesgfilter.timeFilter = timeRange.begin;
        if (periodId == ALL)
            dmesgfilter.timeFilter = 0;

        m_pParser->parseByDmesg(dmesgfilter);
    }
    break;
    case KERN: {
        KERN_FILTERS kernFilter;
        kernFilter.timeFilterBegin = timeRange.begin;
        kernFilter.timeFilterEnd = timeRange.end;

        m_kernCurrentIndex = m_pParser->parseByKern(kernFilter);
    }
    break;
    case BOOT_KLU: {
        QStringList arg;
        if (lId != LVALL) {
            QString prio = QString("PRIORITY=%1").arg(lId);
            arg.append(prio);
        } else {
            arg.append("all");
        }

        m_journalBootCurrentIndex = m_pParser->parseByJournalBoot(arg);
    }
    break;
    case BOOT: {
        m_bootFilter.searchstr = m_currentSearchStr;
        m_bootFilter.statusFilter = statusFilter;

        m_bootCurrentIndex = m_pParser->parseByBoot();
    }
    break;
    case DPKG: {
        DKPG_FILTERS dpkgFilter;
        dpkgFilter.timeFilterBegin = timeRange.begin;
        dpkgFilter.timeFilterEnd = timeRange.end;
        m_dpkgCurrentIndex = m_pParser->parseByDpkg(dpkgFilter);
    }
    break;
    case Dnf: {
        DNF_FILTERS dnffilter;
        dnffilter.levelfilter = dnfLevel2Id(condition);
        dnffilter.timeFilter = timeRange.begin;
        if (periodId == ALL)
            dnffilter.timeFilter = 0;

        m_pParser->parseByDnf(dnffilter);
    }
    break;
    case Kwin: {
        KWIN_FILTERS filter;
        filter.msg = "";
        m_kwinCurrentIndex = m_pParser->parseByKwin(filter);
    }
    break;
    case XORG: {
        XORG_FILTERS xorgFilter;
        m_xorgCurrentIndex = m_pParser->parseByXlog(xorgFilter);
    }
    break;
    case APP: {
        // TODO 待实现
    }
    break;
    case COREDUMP: {
        COREDUMP_FILTERS coreFilter;
        coreFilter.timeFilterBegin = timeRange.begin;
        coreFilter.timeFilterEnd = timeRange.end;
        m_coredumpCurrentIndex = m_pParser->parseByCoredump(coreFilter);
    }
    break;
    case Normal: {
        m_normalFilter.searchstr = m_currentSearchStr;
        m_normalFilter.timeFilterBegin = timeRange.begin;
        m_normalFilter.timeFilterEnd = timeRange.end;
        m_normalFilter.eventTypeFilter = normal2eventType(condition);
        m_normalCurrentIndex = m_pParser->parseByNormal(m_normalFilter);
    }
    break;
    case Audit: {
        m_auditFilter.timeFilterBegin = timeRange.begin;
        m_auditFilter.timeFilterEnd = timeRange.end;
        m_auditFilter.auditTypeFilter = audit2eventType(condition);
        m_auditCurrentIndex = m_pParser->parseByAudit(m_auditFilter);
    }
    break;
    default:
        break;
    }

    return true;
}

void LogBackend::exportData()
{
    if (!m_isDataLoadComplete)
        return;

    QString outPath = m_outPath;

    LogExportThread *exportThread = new LogExportThread(m_isDataLoadComplete, this);
    connect(exportThread, &LogExportThread::sigResult, this, &LogBackend::onExportResult);
    connect(exportThread, &LogExportThread::sigProgress, this, &LogBackend::onExportProgress);

    QString fileName = "";
    QStringList labels;
    bool bMatchedData = false;
    switch (m_flag) {
    case JOURNAL: {
        if (!jList.isEmpty()) {
            bMatchedData = true;
            fileName = outPath + "/system.txt";
            labels << QCoreApplication::translate("Table", "Level")
                   << QCoreApplication::translate("Table", "Process") // modified by Airy
                   << QCoreApplication::translate("Table", "Date and Time")
                   << QCoreApplication::translate("Table", "Info")
                   << QCoreApplication::translate("Table", "User")
                   << QCoreApplication::translate("Table", "PID");
            exportThread->exportToTxtPublic(fileName, jList, labels, m_flag);
        }
    }
    break;
    case Dmesg: {
        if (!dmesgList.isEmpty()) {
            bMatchedData = true;
            fileName = outPath + "/dmesg.txt";
            labels  << QCoreApplication::translate("Table", "Level")
                    << QCoreApplication::translate("Table", "Date and Time")
                    << QCoreApplication::translate("Table", "Info");
            exportThread->exportToTxtPublic(fileName, dmesgList, labels);
        }
    }
    break;
    case KERN: {
        if (!kList.isEmpty()) {
            bMatchedData = true;
            fileName = outPath + "/kernel.txt";
            labels << QCoreApplication::translate("Table", "Date and Time")
                   << QCoreApplication::translate("Table", "User")
                   << QCoreApplication::translate("Table", "Process")
                   << QCoreApplication::translate("Table", "Info");
            exportThread->exportToTxtPublic(fileName, kList, labels, m_flag);
        }
    }
    break;
    case BOOT_KLU: {
        if (!jBootList.isEmpty()) {
            bMatchedData = true;
            fileName = outPath + "/boot_klu.txt";
            labels << QCoreApplication::translate("Table", "Level")
                   << QCoreApplication::translate("Table", "Process")
                   << QCoreApplication::translate("Table", "Date and Time")
                   << QCoreApplication::translate("Table", "Info")
                   << QCoreApplication::translate("Table", "User")
                   << QCoreApplication::translate("Table", "PID");
            exportThread->exportToTxtPublic(fileName, jBootList, labels, JOURNAL);
        }
    }
    break;
    case BOOT: {
        if (!currentBootList.isEmpty()) {
            bMatchedData = true;
            fileName = outPath + "/boot.txt";
            labels << QCoreApplication::translate("Table", "Status")
                   << QCoreApplication::translate("Table", "Info");
            exportThread->exportToTxtPublic(fileName, currentBootList, labels);
        }
    }
    break;
    case DPKG: {
        if (!dList.isEmpty()) {
            bMatchedData = true;
            fileName = outPath + "/dpkg.txt";
            labels << QCoreApplication::translate("Table", "Date and Time")
                   << QCoreApplication::translate("Table", "Info")
                   << QCoreApplication::translate("Table", "Action");
            exportThread->exportToTxtPublic(fileName, dList, labels);
        }
    }
    break;
    case Dnf: {
        if (!dnfList.isEmpty()) {
            bMatchedData = true;
            fileName = outPath + "/dnf.txt";
            labels << QCoreApplication::translate("Table", "Level")
                   << QCoreApplication::translate("Table", "Date and Time")
                   << QCoreApplication::translate("Table", "Info");
            exportThread->exportToTxtPublic(fileName, dnfList, labels);
        }
    }
    break;
    case Kwin: {
        if (!m_currentKwinList.isEmpty()) {
            bMatchedData = true;
            fileName = outPath + "/kwin.txt";
            labels << QCoreApplication::translate("Table", "Info");
            exportThread->exportToTxtPublic(fileName, m_currentKwinList, labels);
        }
    }
    break;
    case XORG: {
        if (!xList.isEmpty()) {
            bMatchedData = true;
            fileName = outPath + "/xorg.txt";
            labels << QCoreApplication::translate("Table", "Offset")
                   << QCoreApplication::translate("Table", "Info");
            exportThread->exportToTxtPublic(fileName, xList, labels);
        }
    }
    break;
    case APP: {
        if (!appList.isEmpty()) {
            bMatchedData = true;
            QString appName = Utils::appName(m_curAppLog);
            QString transAppName = LogApplicationHelper::instance()->transName(appName);
            fileName = outPath + QString("/%1.txt").arg(appName);
            labels << QCoreApplication::translate("Table", "Level")
                   << QCoreApplication::translate("Table", "Date and Time")
                   << QCoreApplication::translate("Table", "Source")
                   << QCoreApplication::translate("Table", "Info");
            exportThread->exportToTxtPublic(fileName, appList, labels, transAppName);
        }
    }
    break;
    case COREDUMP: {
        if (!m_currentCoredumpList.isEmpty()) {
            bMatchedData = true;
            fileName = outPath + "/coredump.zip";
            labels << QCoreApplication::translate("Table", "SIG")
                   << QCoreApplication::translate("Table", "Date and Time")
                   << QCoreApplication::translate("Table", "Core File")
                   << QCoreApplication::translate("Table", "User Name ")
                   << QCoreApplication::translate("Table", "EXE");
            exportThread->exportToZipPublic(fileName, m_currentCoredumpList, labels);
        }
    }
    break;
    case Normal: {
        if (!nortempList.isEmpty()) {
            bMatchedData = true;
            fileName = outPath + "/boot-shutdown-event.txt";
            labels << QCoreApplication::translate("Table", "Event Type")
                   << QCoreApplication::translate("Table", "Username")
                   << QCoreApplication::translate("Tbble", "Date and Time")
                   << QCoreApplication::translate("Table", "Info");
            exportThread->exportToTxtPublic(fileName, nortempList, labels);
        }
    }
    break;
    case Audit: {
        if (!aList.isEmpty()) {
            bMatchedData = true;
            fileName = outPath + "/audit.txt";
            labels << QCoreApplication::translate("Table", "Event Type")
                   << QCoreApplication::translate("Table", "Date and Time")
                   << QCoreApplication::translate("Table", "Process")
                   << QCoreApplication::translate("Table", "Status")
                   << QCoreApplication::translate("Table", "Info");
            exportThread->exportToTxtPublic(fileName, aList, labels);
        }
    }
    break;
    default:
    break;
    }

    if (!bMatchedData) {
        qCWarning(logBackend) << "No matching data..";
        qApp->exit(-1);
        return;
    }

    QThreadPool::globalInstance()->start(exportThread);
    qCInfo(logBackend) << "exporting ...";
}

void LogBackend::resetCategoryOutputPath(const QString &path)
{
    // 先清空原有路径中的kernel日志文件
    QDir dir(path);
    dir.removeRecursively();

    // 创建目录
    Utils::mkMutiDir(path);
}

void LogBackend::initParser()
{
    if (m_pParser)
        return;

    m_pParser = new LogFileParser();

    connect(m_pParser, &LogFileParser::dpkgFinished, this, &LogBackend::slot_dpkgFinished,
            Qt::QueuedConnection);
    connect(m_pParser, &LogFileParser::dpkgData, this, &LogBackend::slot_dpkgData,
            Qt::QueuedConnection);
    connect(m_pParser, &LogFileParser::xlogFinished, this, &LogBackend::slot_XorgFinished,
            Qt::QueuedConnection);
    connect(m_pParser, &LogFileParser::xlogData, this, &LogBackend::slot_xorgData,
            Qt::QueuedConnection);

    connect(m_pParser, &LogFileParser::bootFinished, this, &LogBackend::slot_bootFinished,
            Qt::QueuedConnection);
    connect(m_pParser, &LogFileParser::bootData, this, &LogBackend::slot_bootData,
            Qt::QueuedConnection);

    connect(m_pParser, &LogFileParser::kernFinished, this, &LogBackend::slot_kernFinished,
            Qt::QueuedConnection);
    connect(m_pParser, &LogFileParser::kernData, this, &LogBackend::slot_kernData,
            Qt::QueuedConnection);

    connect(m_pParser, &LogFileParser::journalFinished, this, &LogBackend::slot_journalFinished,
            Qt::QueuedConnection);
    connect(m_pParser, &LogFileParser::journalData, this, &LogBackend::slot_journalData,
            Qt::QueuedConnection);
    connect(m_pParser, &LogFileParser::journaBootlData, this, &LogBackend::slot_journalBootData,
            Qt::QueuedConnection);
    connect(m_pParser, &LogFileParser::appFinished, this,
            &LogBackend::slot_applicationFinished);
    connect(m_pParser, &LogFileParser::appData, this,
            &LogBackend::slot_applicationData);

    connect(m_pParser, &LogFileParser::kwinFinished, this, &LogBackend::slot_kwinFinished,
            Qt::QueuedConnection);
    connect(m_pParser, &LogFileParser::kwinData, this, &LogBackend::slot_kwinData,
            Qt::QueuedConnection);

    connect(m_pParser, &LogFileParser::normalData, this, &LogBackend::slot_normalData,
            Qt::QueuedConnection);
    connect(m_pParser, &LogFileParser::normalFinished, this, &LogBackend::slot_normalFinished,
            Qt::QueuedConnection);
    connect(m_pParser, &LogFileParser::journalBootFinished, this, &LogBackend::slot_journalBootFinished);

    connect(m_pParser, &LogFileParser::proccessError, this, &LogBackend::slot_logLoadFailed,
            Qt::QueuedConnection);
    connect(m_pParser, SIGNAL(dnfFinished(QList<LOG_MSG_DNF>)), this, SLOT(slot_dnfFinished(QList<LOG_MSG_DNF>)));
    connect(m_pParser, &LogFileParser::dmesgFinished, this, &LogBackend::slot_dmesgFinished,
            Qt::QueuedConnection);

    connect(m_pParser, &LogFileParser::auditData, this, &LogBackend::slot_auditData,
            Qt::QueuedConnection);
    connect(m_pParser, &LogFileParser::auditFinished, this, &LogBackend::slot_auditFinished,
            Qt::QueuedConnection);

    connect(m_pParser, &LogFileParser::coredumpData, this, &LogBackend::slot_coredumpData,
            Qt::QueuedConnection);
    connect(m_pParser, &LogFileParser::coredumpFinished, this, &LogBackend::slot_coredumpFinished,
            Qt::QueuedConnection);
}

QString LogBackend::getOutDirPath() const
{
    return m_outPath;
}

bool LogBackend::getOutDirPath(const QString &path)
{
    QString tmpPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    if (!path.isEmpty())
        tmpPath = QDir::isRelativePath(path) ? (m_cmdWorkDir + "/" + path) : path;

    // 若指定目录不存在，先创建目录
    QDir dir(tmpPath);
    if (!dir.exists()) {
         dir.mkpath(dir.absolutePath());
         m_newDir = dir.exists();
         if (m_newDir)
             tmpPath = dir.absolutePath();
    } else {
        tmpPath = dir.absolutePath();
    }

    if (dir.exists()) {
        QFileInfo fi(tmpPath);
        if (!fi.isWritable()) {
            qCWarning(logBackend) << QString("outPath: %1 is not writable.").arg(tmpPath);
            return false;
        }

        m_outPath = tmpPath;
        qCInfo(logBackend) << "outPath:" << m_outPath;
        return true;
    } else {
        m_outPath = "";
        qCWarning(logBackend) << QString("outpath:%1 is not exist.").arg(path);
    }

    return false;
}

LOG_FLAG LogBackend::type2Flag(const QString &type, QString& error)
{
    Dtk::Core::DSysInfo::UosEdition edition = Dtk::Core::DSysInfo::uosEditionType();
    bool isCentos = Dtk::Core::DSysInfo::UosEuler == edition || Dtk::Core::DSysInfo::UosEnterpriseC == edition || Dtk::Core::DSysInfo::UosMilitaryS == edition;

    LOG_FLAG flag = NONE;
    if (type == TYPE_SYSTEM)
        flag = JOURNAL;
    else if (type == TYPE_KERNEL) {
        if (isCentos)
            flag = Dmesg;
        else
            flag = KERN;
    } else if (type == TYPE_BOOT) {
        if (DBusManager::isSpecialComType())
            flag = BOOT_KLU;
        else
            flag = BOOT;
    } else if (type == TYPE_DPKG) {
        if (!isCentos)
            flag = DPKG;
        else
            error = "Server industry edition has no dpkg.log";
    } else if (type == TYPE_DNF) {
        if (isCentos)
            flag = Dnf;
        else
            error = "Only server industry edition has dnf.log";
    } else if (type == TYPE_KWIN) {
        if (DBusManager::isSpecialComType())
            flag = Kwin;
        else
            error = "Only wayland platform has kwin.log";
    } else if (type == TYPE_XORG) {
        if (!DBusManager::isSpecialComType())
            flag = XORG;
        else
            error = "Wayland platform has no Xorg.log";
    } else if (type == TYPE_APP) {
        flag = APP;
    } else if (type == TYPE_COREDUMP) {
        flag = COREDUMP;
    } else if (type == TYPE_BSE) {
        flag = Normal;
    } else if (type == TYPE_OTHER) {
        flag = OtherLog;
    } else if (type == TYPE_CUSTOM) {
        flag = CustomLog;
    } else if (type == TYPE_AUDIT) {
        flag = Audit;
    } else {
        flag = NONE;
        error = QString("Unknown type: %1.").arg(type) + "USEAGE: system(journal log), kernel(kernel log), boot(boot log), dpkg(dpkg log), dnf(dnf log), kwin(Kwin log), xorg(Xorg log), app(deepin app log), coredump(coredump log)、boot-shutdown-event(boot shutdown event log)、other(other log)、custom(custom log)、audit(audit log)";
    }

    return flag;
}

BUTTONID LogBackend::period2Enum(const QString &period)
{
    BUTTONID id = INVALID;
    if (period == "all")
        id = ALL;
    else if (period == "today")
        id = ONE_DAY;
    else if (period == "3d")
        id = THREE_DAYS;
    else if (period == "1w")
        id = ONE_WEEK;
    else if (period == "1m")
        id = ONE_MONTH;
    else if (period == "3m")
        id = THREE_MONTHS;

    return id;
}

int LogBackend::level2Id(const QString &level)
{
    int lId = -2;
    if (level == "debug" || level == "7")
        lId = 7;
    if (level == "info" || level == "6")
        lId = 6;
    else if (level == "notice" || level == "5")
        lId = 5;
    else if (level == "warning" || level == "4")
        lId = 4;
    else if (level == "err" || level == "error" || level == "3")
        lId = 3;
    else if (level == "critical" || level == "crit" || level == "2")
        lId = 2;
    else if (level == "alert" || level == "1")
        lId = 1;
    else if (level == "emerg" || level == "0")
        lId = 0;
    else if (level == "all")
        lId = -1;
    else if (level.isEmpty())
        lId = -1;

    return lId;
}

DNFPRIORITY LogBackend::dnfLevel2Id(const QString &level)
{
    DNFPRIORITY eId = DNFINVALID;
    if (level == "trace" || level == "6")
        eId = TRACE;
    else if (level == "debug" || level == "5")
        eId = DEBUG;
    else if (level == "info" || level == "4")
        eId = INFO;
    else if (level == "warning" || level == "3")
        eId = WARNING;
    else if (level == "error" || level == "err" || level == "2")
        eId = ERROR;
    else if (level == "critical" || level == "crit" || level == "1")
        eId = CRITICAL;
    else if (level == "supercritical" || level == "supercrit" || level == "0")
        eId = SUPERCRITICAL;
    else if (level == "all")
        eId = DNFLVALL;
    else if (level.isEmpty())
        eId = DNFLVALL;

    return eId;
}

int LogBackend::normal2eventType(const QString &eventType)
{
    int type = -1;
    if (eventType == "all" || eventType == "0")
        type = 0;
    else if (eventType == "login" || eventType == "1")
        type = 1;
    else if (eventType == "boot" || eventType == "2")
        type = 2;
    else if (eventType == "shutdown" || eventType == "3")
        type = 3;

    return type;
}

int LogBackend::audit2eventType(const QString &eventType)
{
    int type = -1;
    if (eventType == "all" || eventType == "0")
        type = 0;
    else if (eventType == "identauth" || eventType == "1")
        type = 1;
    else if (eventType == "dac" || eventType == "2")
        type = 2;
    else if (eventType == "mac" || eventType == "3")
        type = 3;
    else if (eventType == "remote" || eventType == "4")
        type = 4;
    else if (eventType == "docaudit" || eventType == "5")
        type = 5;
    else if (eventType == "other" || eventType == "6")
        type = 6;

    return type;
}

TIME_RANGE LogBackend::getTimeRange(const BUTTONID &periodId)
{
    TIME_RANGE tr;

    QDateTime dtStart = QDateTime::currentDateTime();
    QDateTime dtEnd = dtStart;
    dtStart.setTime(QTime());
    dtEnd.setTime(QTime(23, 59, 59, 999));
    tr.end = dtEnd.toMSecsSinceEpoch();

    switch (periodId) {
    case ALL: {
        tr.begin = -1;
        tr.end = -1;
    }
    break;
    case ONE_DAY: {
        tr.begin = dtStart.toMSecsSinceEpoch();
    }
    break;
    case THREE_DAYS: {
        tr.begin = dtStart.addDays(-2).toMSecsSinceEpoch();
    }
    break;
    case ONE_WEEK: {
        tr.begin = dtStart.addDays(-6).toMSecsSinceEpoch();
    }
    break;
    case ONE_MONTH: {
        tr.begin = dtStart.addMonths(-1).toMSecsSinceEpoch();
        if (m_flag == Dmesg || m_flag == Dnf) {
            tr.begin = dtStart.addDays(-29).toMSecsSinceEpoch();
        }
    }
    break;
    case THREE_MONTHS: {
        tr.begin = dtStart.addMonths(-3).toMSecsSinceEpoch();
        if (m_flag == Dmesg || m_flag == Dnf) {
            tr.begin = dtStart.addDays(-89).toMSecsSinceEpoch();
        }
    }
    break;
    default: {
        tr.begin = -1;
        tr.end = -1;
    }
    break;
    }

    return tr;
}

QString LogBackend::getApplogPath(const QString &appName)
{
    QString logPath;
    QMap<QString, QString> appData = LogApplicationHelper::instance()->getMap();
    for (auto &it2 : appData.toStdMap()) {
        if (it2.second.contains(appName)) {
            logPath = it2.second;
            break;
        }
    }

    if (logPath.isEmpty())
        logPath = LogApplicationHelper::instance()->getPathByAppId(appName);

    return logPath;
}
