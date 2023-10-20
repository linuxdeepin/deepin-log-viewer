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
}

void LogBackend::setCmdWorkDir(const QString &dir)
{
    m_cmdWorkDir = dir;
}

void LogBackend::exportAllLogs(const QString &outDir)
{
    PERF_PRINT_BEGIN("POINT-05", "export all logs");
    qCInfo(logBackend) << "exporting all logs begin.";
    // 时间
    QString dateTime = QDateTime::currentDateTime().toString("yyyyMMddHHmmss");

    // 主机名
    utsname _utsname;
    uname(&_utsname);
    QString hostname = QString(_utsname.nodename);

    QString outPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    if (!outDir.isEmpty())
        outPath = QDir::isRelativePath(outDir) ? (m_cmdWorkDir + "/" + outDir) : outDir;

    // 若指定目录不存在，先创建目录
    QDir odir(outPath);
    if (!odir.exists()) {
         odir.mkpath(odir.absolutePath());
         m_newDir = odir.exists();
         if (m_newDir)
             outPath = odir.absolutePath();
    } else {
        outPath = odir.absolutePath();
    }

    m_outPath = outPath;

    QString fileFullPath = outPath + "/" + QString("%1_%2_all_logs.zip").arg(dateTime).arg(hostname);

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
                QDir odir(outPath);
                odir.removeRecursively();
            }
            PERF_PRINT_END("POINT-05", "cost");
            qApp->exit(-1);
        }
    });
    QThreadPool::globalInstance()->start(thread);
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
