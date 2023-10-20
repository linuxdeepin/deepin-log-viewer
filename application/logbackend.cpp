// SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "logbackend.h"
#include "logallexportthread.h"
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
}

void LogBackend::setCmdWorkDir(const QString &dirPath)
{
    QDir dir(dirPath);
    if (dir.exists())
        m_cmdWorkDir = dirPath;
}

void LogBackend::exportAllLogs(const QString &outDir)
{
    if(!getOutDirPath(outDir))
        return;

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

    resetToNormalAuth(m_outPath);
}

void LogBackend::exportTypeLogs(const QString &outDir, const QString &type)
{
    // 输出目录有效性验证
    if(!getOutDirPath(outDir))
        return;

    // 日志种类有效性验证
    QString error;
    m_flag = type2Flag(type, error);
    if (NONE == m_flag) {
        qCInfo(logBackend) << error;
        return;
    }

    QString tmpCategoryOutPath("");

    switch (m_flag) {
    case JOURNAL: {
        DLDBusHandler::instance()->exportLog(m_outPath, "journalctl_system", false);
    }
    break;
    case Dmesg: {
        DLDBusHandler::instance()->exportLog(m_outPath, "dmesg", false);
    }
    break;
    case KERN: {
        QStringList logPaths = DLDBusHandler::instance()->getFileInfo("kern", false);
        if (logPaths.size() > 0) {
            tmpCategoryOutPath = QString("%1/%2/").arg(m_outPath).arg("kernel");
            // 先清空原有路径中的kernel日志文件
            QDir dir(tmpCategoryOutPath);
            dir.removeRecursively();

            Utils::mkMutiDir(tmpCategoryOutPath);
            for (auto &file: logPaths)
                DLDBusHandler::instance()->exportLog(tmpCategoryOutPath, file, true);
        } else if (logPaths.size() == 0) {
            qCInfo(logBackend) << "/var/log has not kern.log";
        }
    }
    break;
    case BOOT_KLU: {
        DLDBusHandler::instance()->exportLog(m_outPath, "journalctl_boot", false);
    }
    break;
    case BOOT: {
        QStringList logPaths = DLDBusHandler::instance()->getFileInfo("boot", false);
        if (!logPaths.isEmpty()) {
            tmpCategoryOutPath = QString("%1/%2/").arg(m_outPath).arg("boot");
            // 先清空原有路径中的boot日志文件
            QDir dir(tmpCategoryOutPath);
            dir.removeRecursively();

            Utils::mkMutiDir(tmpCategoryOutPath);
            for (auto &file: logPaths)
                DLDBusHandler::instance()->exportLog(tmpCategoryOutPath, file, true);
        } else {
            qCInfo(logBackend) << "/var/log has not boot.log";
        }
    }
    break;
    case DPKG: {
        QStringList logPaths = DLDBusHandler::instance()->getFileInfo("dpkg", false);
        if (!logPaths.isEmpty()) {
            tmpCategoryOutPath = QString("%1/%2/").arg(m_outPath).arg("dpkg");
            // 先清空原有路径中的dpkg日志文件
            QDir dir(tmpCategoryOutPath);
            dir.removeRecursively();

            Utils::mkMutiDir(tmpCategoryOutPath);
            for (auto &file: logPaths)
                DLDBusHandler::instance()->exportLog(tmpCategoryOutPath, file, true);
        } else
            qCInfo(logBackend) << "/var/log has not dpkg.log";
    }
    break;
    case Dnf: {
        QStringList logPaths = DLDBusHandler::instance()->getFileInfo("dnf", false);
        if (!logPaths.isEmpty()) {
            tmpCategoryOutPath = QString("%1/%2/").arg(m_outPath).arg("dnf");
            // 先清空原有路径中的dnf日志文件
            QDir dir(tmpCategoryOutPath);
            dir.removeRecursively();

            Utils::mkMutiDir(tmpCategoryOutPath);
            for (auto &file: logPaths)
                DLDBusHandler::instance()->exportLog(tmpCategoryOutPath, file, true);
        } else
            qCInfo(logBackend) << "/var/log has not dnf.log";
    }
    break;
    case Kwin: {
        DLDBusHandler::instance()->exportLog(m_outPath, KWIN_TREE_DATA, true);
    }
    break;
    case XORG: {
        QStringList logPaths = DLDBusHandler::instance()->getFileInfo("Xorg", false);
        if (!logPaths.isEmpty()) {
            tmpCategoryOutPath = QString("%1/%2/").arg(m_outPath).arg("xorg");
            // 先清空原有路径中的xorg日志文件
            QDir dir(tmpCategoryOutPath);
            dir.removeRecursively();

            Utils::mkMutiDir(tmpCategoryOutPath);
            for (auto &file: logPaths)
                DLDBusHandler::instance()->exportLog(tmpCategoryOutPath, file, true);
        } else
            qCInfo(logBackend) << "/var/log has not Xorg.log";
    }
    break;
    case APP: {
        tmpCategoryOutPath = QString("%1/%2/").arg(m_outPath).arg("apps");

        // 先清空原有路径中的app日志文件
        QDir dir(tmpCategoryOutPath);
        dir.removeRecursively();

        Utils::mkMutiDir(tmpCategoryOutPath);

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
            QString tmpSubCategoryOutPath = QString("%1/%2/").arg(tmpCategoryOutPath).arg(fi.baseName());
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
            tmpCategoryOutPath = QString("%1/%2/").arg(m_outPath).arg("coredump");
            // 先清空原有路径中的coredump日志文件
            QDir dir(tmpCategoryOutPath);
            dir.removeRecursively();

            Utils::mkMutiDir(tmpCategoryOutPath);
            for (auto &file: logPaths)
                DLDBusHandler::instance()->exportLog(tmpCategoryOutPath, file, true);
        } else
            qCInfo(logBackend) << "/var/log has no coredump logs";
    }
    break;
    case Normal: {
        QFile file("/var/log/wtmp");
        if (!file.exists())
            qCInfo(logBackend) << "/var/log has no boot shutdown event log";

        DLDBusHandler::instance()->exportLog(m_outPath, "last", false);
    }
    break;
    case OtherLog: {
        tmpCategoryOutPath = QString("%1/%2/").arg(m_outPath).arg("others");

        // 先清空原有路径中的other日志文件
        QDir dir(tmpCategoryOutPath);
        dir.removeRecursively();

        Utils::mkMutiDir(tmpCategoryOutPath);

        auto otherLogListPair = LogApplicationHelper::instance()->getOtherLogList();
        for (auto &it2 : otherLogListPair) {
            QStringList logPaths = DLDBusHandler::instance(nullptr)->getOtherFileInfo(it2.at(1));
            logPaths.removeDuplicates();
            if (logPaths.size() > 1) {
                QString tmpSubCategoryOutPath = QString("%1/%2/").arg(tmpCategoryOutPath).arg(it2.at(0));
                Utils::mkMutiDir(tmpSubCategoryOutPath);
                for (auto &file : logPaths) {
                    DLDBusHandler::instance()->exportLog(tmpSubCategoryOutPath, file, true);
                }
            }
            else if (logPaths.size() == 1)
                 DLDBusHandler::instance()->exportLog(tmpCategoryOutPath, logPaths[0], true);
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
            tmpCategoryOutPath = QString("%1/%2/").arg(m_outPath).arg("customized");
            // 先清空原有路径中的other日志文件
            QDir dir(tmpCategoryOutPath);
            dir.removeRecursively();

            Utils::mkMutiDir(tmpCategoryOutPath);

            for (auto &file : logPaths) {
                DLDBusHandler::instance()->exportLog(tmpCategoryOutPath, file, true);
            }
        } else {
            qCInfo(logBackend) << "no custom logs";
        }
    }
    break;
    case Audit: {
        QStringList logPaths = DLDBusHandler::instance()->getFileInfo("audit", false);
        if (!logPaths.isEmpty()) {
            tmpCategoryOutPath = QString("%1/%2/").arg(m_outPath).arg("audit");
            // 先清空原有路径中的audit日志文件
            QDir dir(tmpCategoryOutPath);
            dir.removeRecursively();

            Utils::mkMutiDir(tmpCategoryOutPath);
            for (auto &file: logPaths)
                DLDBusHandler::instance()->exportLog(tmpCategoryOutPath, file, true);
        } else
            qCInfo(logBackend) << "/var/log has no audit logs";
    }
    break;
    default:
        break;
    }

    resetToNormalAuth(tmpCategoryOutPath);
}

void LogBackend::exportLogsByApp(const QString &outDir, const QString &appName)
{
    if(!getOutDirPath(outDir))
        return;

    if (appName.isEmpty())
        return;

    // 先查找是否有该应用相关日志
    QString logPath;
    QMap<QString, QString> appData = LogApplicationHelper::instance()->getMap();
    for (auto &it2 : appData.toStdMap()) {
        if (it2.second.contains(appName)) {
            logPath = it2.second;
            break;
        }
    }

    if (logPath.isEmpty()) {
        qCInfo(logBackend) << QString("unknown app:%1 is invalid.").arg(appName);
        return;
    }

    AppLogConfig appLogConfig = LogApplicationHelper::instance()->appLogConfig(appName);
    // 确定解析方式
    QString parseType = "";
    if (appLogConfig.logType == "file" || !appLogConfig.isValid())
        parseType = "file";
    else if (appLogConfig.isValid() && appLogConfig.logType == "journal")
        parseType = "journal";


    QString tmpCategoryOutPath = QString("%1/%2").arg(m_outPath).arg(appName);
    if (parseType == "file") {
        QStringList logPaths = DLDBusHandler::instance(nullptr)->getOtherFileInfo(logPath);
        logPaths.removeDuplicates();

        if (logPaths.size() > 0) {
            // 先清空原有路径中的app日志文件
            QDir dir(tmpCategoryOutPath);
            dir.removeRecursively();

            Utils::mkMutiDir(tmpCategoryOutPath);
            for (auto &file: logPaths)
                DLDBusHandler::instance()->exportLog(tmpCategoryOutPath, file, true);
        } else {
            qCInfo(logBackend) << QString("app:%1 not found log files.").arg(appName);
        }
    } else if (parseType == "journal") {
        // 先清空原有路径中的app日志文件
        QDir dir(tmpCategoryOutPath);
        dir.removeRecursively();

        Utils::mkMutiDir(tmpCategoryOutPath);

        DLDBusHandler::instance()->exportLog(tmpCategoryOutPath, "journalctl_app", false);
    }

    resetToNormalAuth(tmpCategoryOutPath);
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
        m_outPath = tmpPath;
        qCDebug(logBackend) << "outPath:" << m_outPath;
        return true;
    }
    else {
        m_outPath = "";
        qCInfo(logBackend) << QString("outpath:%1 is not exist.").arg(path);
    }

    return false;
}

void LogBackend::resetToNormalAuth(const QString &path)
{
    QDir dir(path);
    if (!path.isEmpty() && dir.exists()) {
        QDir dir(path);
        if (dir.exists()) {
            QProcess procss;
            procss.setWorkingDirectory(path);
            QStringList arg = {"-c"};
            arg.append(QString("chmod -R 777 '%1'").arg(path));
            procss.start("/bin/bash", arg);
            procss.waitForFinished(-1);
        }
    }
}

LOG_FLAG LogBackend::type2Flag(const QString &type, QString& error)
{
    Dtk::Core::DSysInfo::UosEdition edition = Dtk::Core::DSysInfo::uosEditionType();
    bool isCentos = Dtk::Core::DSysInfo::UosEuler == edition || Dtk::Core::DSysInfo::UosEnterpriseC == edition || Dtk::Core::DSysInfo::UosMilitaryS == edition;

    LOG_FLAG flag = NONE;
    if (type == "system")
        flag = JOURNAL;
    else if (type == "kernel") {
        if (isCentos)
            flag = Dmesg;
        else
            flag = KERN;
    } else if (type == "boot") {
        if (DBusManager::isSpecialComType())
            flag = BOOT_KLU;
        else
            flag = BOOT;
    } else if (type == "dpkg") {
        if (!isCentos)
            flag = DPKG;
        else
            error = "Server industry edition has no dpkg.log";
    } else if (type == "dnf") {
        if (isCentos)
            flag = Dnf;
        else
            error = "Only server industry edition has dnf.log";
    } else if (type == "kwin") {
        if (DBusManager::isSpecialComType())
            flag = Kwin;
        else
            error = "Only wayland platform has kwin.log";
    } else if (type == "xorg") {
        if (!DBusManager::isSpecialComType())
            flag = XORG;
        else
            error = "Wayland platform has no Xorg.log";
    } else if (type == "app") {
        flag = APP;
    } else if (type == "coredump") {
        flag = COREDUMP;
    } else if (type == "boot-shutdown-event") {
        flag = Normal;
    } else if (type == "other") {
        flag = OtherLog;
    } else if (type == "custom") {
        flag = CustomLog;
    } else if (type == "audit") {
        flag = Audit;
    } else {
        flag = NONE;
        error = QString("Unknown type: %1.").arg(type);
    }

    return flag;
}
