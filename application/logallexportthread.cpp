// SPDX-FileCopyrightText: 2021 - 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "logallexportthread.h"
#include "dbusproxy/dldbushandler.h"
#include "logapplicationhelper.h"
#include "utils.h"

#include <QLoggingCategory>
#include <QFileInfo>
#include <QDir>
#include <QProcess>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDateTime>
#include <QSet>
#include <QStandardPaths>
#include <QTemporaryDir>

Q_DECLARE_LOGGING_CATEGORY(logApp)

constexpr int ZIP_BUFFER_SIZE = 64 * 1024; // 64KB buffer for better I/O performance
constexpr int ZIP_COMPRESSION_LEVEL = Z_BEST_SPEED; // Fast compression for logs

// Helper function to convert QDateTime to tm_zip structure
static tm_zip dateTimeToTmZip(const QDateTime &dateTime)
{
    tm_zip tmz = {};
    if (dateTime.isValid()) {
        QDate date = dateTime.date();
        QTime time = dateTime.time();
        
        tmz.tm_year = date.year();
        tmz.tm_mon = date.month() - 1;  // tm_mon is 0-based (0-11)
        tmz.tm_mday = date.day();
        tmz.tm_hour = time.hour();
        tmz.tm_min = time.minute();
        tmz.tm_sec = time.second();
    }
    return tmz;
}

LogAllExportThread::LogAllExportThread(const QStringList &types, const QString &outfile, QObject *parent)
    : QObject(parent)
    , m_types(types)
    , m_outfile(outfile)
    , m_cancel(false)
{
    qCDebug(logApp) << "LogAllExportThread created with types:" << types << "output file:" << outfile;
}

bool LogAllExportThread::addFileToZip(const QString &filePath, const QString &zipEntryName)
{
    if (m_cancel.load() || !m_zipFile) return false;

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qCWarning(logApp) << "Failed to open file for zipping:" << filePath;
        return false;
    }

    // Get file modification time and set it in zip_fileinfo
    zip_fileinfo zfi = {};
    QFileInfo fileInfo(filePath);
    if (fileInfo.exists()) {
        QDateTime lastModified = fileInfo.lastModified();
        zfi.tmz_date = dateTimeToTmZip(lastModified);
        zfi.dosDate = 0; // Use tmz_date instead of dosDate
    }
    
    zipOpenNewFileInZip64(m_zipFile, zipEntryName.toUtf8().constData(), &zfi, nullptr, 0, nullptr, 0, nullptr, Z_DEFLATED, ZIP_COMPRESSION_LEVEL, 1);

    char buf[ZIP_BUFFER_SIZE];
    qint64 bytesRead;
    while ((bytesRead = file.read(buf, ZIP_BUFFER_SIZE)) > 0) {
        if (m_cancel.load()) {
            zipCloseFileInZip(m_zipFile);
            return false;
        }
        if (zipWriteInFileInZip(m_zipFile, buf, bytesRead) != ZIP_OK) {
            qCCritical(logApp) << "Failed to write to zip stream for file:" << filePath;
            zipCloseFileInZip(m_zipFile);
            return false;
        }
    }

    zipCloseFileInZip(m_zipFile);
    return true;
}

bool LogAllExportThread::addProcessOutputToZip(const QString &command, const QStringList &args, const QString &zipEntryName)
{
    if (m_cancel.load() || !m_zipFile) return false;

    QProcess process;
    process.start(command, args);
    if (!process.waitForStarted()) {
        qCWarning(logApp) << "Failed to start process for command:" << command << args;
        return false;
    }

    // Set current time for process output
    zip_fileinfo zfi = {};
    QDateTime currentTime = QDateTime::currentDateTime();
    zfi.tmz_date = dateTimeToTmZip(currentTime);
    zfi.dosDate = 0; // Use tmz_date instead of dosDate
    
    zipOpenNewFileInZip64(m_zipFile, zipEntryName.toUtf8().constData(), &zfi, nullptr, 0, nullptr, 0, nullptr, Z_DEFLATED, ZIP_COMPRESSION_LEVEL, 1);

    char buf[ZIP_BUFFER_SIZE];
    while (process.waitForReadyRead(100)) {
        if (m_cancel.load()) {
            process.terminate();
            zipCloseFileInZip(m_zipFile);
            return false;
        }
        qint64 bytesRead = process.read(buf, ZIP_BUFFER_SIZE);
        if (bytesRead > 0) {
            if (zipWriteInFileInZip(m_zipFile, buf, bytesRead) != ZIP_OK) {
                qCCritical(logApp) << "Failed to write process output to zip stream for command:" << command;
                process.terminate();
                zipCloseFileInZip(m_zipFile);
                return false;
            }
        }
    }

    process.waitForFinished();
    zipCloseFileInZip(m_zipFile);
    return true;
}

void LogAllExportThread::ensureDirectoriesExist(const QString &filePath)
{
    if (m_cancel.load() || !m_zipFile) return;
    
    // Use static set to track created directories, clear on new zip file
    static QSet<QString> createdDirs;
    static zipFile lastZipFile = nullptr;
    
    // Clear directory tracking when starting a new export
    if (m_zipFile != lastZipFile) {
        createdDirs.clear();
        lastZipFile = m_zipFile;
    }
    
    // Extract directory parts from file path
    QStringList pathParts = filePath.split('/', Qt::SkipEmptyParts);
    if (pathParts.size() <= 1) return; // No directories to create
    
    QString currentPath;
    for (int i = 0; i < pathParts.size() - 1; ++i) { // Exclude the filename
        if (!currentPath.isEmpty()) {
            currentPath += "/";
        }
        currentPath += pathParts[i];
        
        // Skip if directory already created
        if (createdDirs.contains(currentPath)) {
            continue;
        }
        
        // Create directory entry with current time
        zip_fileinfo zfi = {};
        QDateTime currentTime = QDateTime::currentDateTime();
        zfi.tmz_date = dateTimeToTmZip(currentTime);
        zfi.dosDate = 0;
        
        // Directory names in ZIP should end with '/'
        QString dirName = currentPath + '/';
        
        // Create empty directory entry
        if (zipOpenNewFileInZip64(m_zipFile, dirName.toUtf8().constData(), &zfi, nullptr, 0, nullptr, 0, nullptr, Z_DEFLATED, ZIP_COMPRESSION_LEVEL, 1) == ZIP_OK) {
            zipCloseFileInZip(m_zipFile);
            createdDirs.insert(currentPath);
        }
    }
}

void LogAllExportThread::run()
{
    qCDebug(logApp) << "Export thread started with new zip stream logic";

    QFileInfo info(m_outfile);
    if (!QFileInfo(info.path()).isWritable()) {
        qCCritical(logApp) << QString("outdir:%1 it not writable or is not exist.").arg(info.absolutePath());
        emit exportFinsh(false);
        return;
    }

    if (info.exists() && !QFile::remove(m_outfile)) {
        qCCritical(logApp) << "Failed to remove existing output file:" << m_outfile;
        emit exportFinsh(false);
        return;
    }

    m_zipFile = zipOpen64(m_outfile.toUtf8().constData(), 0);
    if (!m_zipFile) {
        qCCritical(logApp) << "Failed to create zip file:" << m_outfile;
        emit exportFinsh(false);
        return;
    }
    emit updatecurrentProcess(1); //update process started

    QList<EXPORTALL_DATA> eList;
    int totalTasks = 0;

    // --- Data Gathering (same as before) ---
    for (auto &it : m_types) {
        EXPORTALL_DATA data;
        if (it.contains(JOUR_TREE_DATA, Qt::CaseInsensitive)) {
            data.logCategory = "system";
            data.commands.push_back("journalctl_system");
        } else if (it.contains(BOOT_KLU_TREE_DATA, Qt::CaseInsensitive)) {
            data.logCategory = "boot";
            data.commands.push_back("journalctl_boot");
        } else if (it.contains(DMESG_TREE_DATA, Qt::CaseInsensitive)) {
            data.logCategory = "kernel";
            data.commands.push_back("dmesg");
        } else if (it.contains(LAST_TREE_DATA, Qt::CaseInsensitive)) {
            data.logCategory = "boot-shutdown-event";
            data.commands.push_back("last");
        } else if (it.contains(DPKG_TREE_DATA, Qt::CaseInsensitive)) {
            data.logCategory = "dpkg";
            data.files.append(DLDBusHandler::instance(nullptr)->getFileInfo("dpkg", false));
        } else if (it.contains(KERN_TREE_DATA, Qt::CaseInsensitive)) {
            data.logCategory = "kernel";
            data.files.append(DLDBusHandler::instance(nullptr)->getFileInfo("kern", false));
        } else if (it.contains(XORG_TREE_DATA, Qt::CaseInsensitive)) {
            data.logCategory = "xorg";
            data.files.append(DLDBusHandler::instance(nullptr)->getFileInfo("Xorg", false));
        } else if (it.contains(DNF_TREE_DATA, Qt::CaseInsensitive)) {
            data.logCategory = "dnf";
            data.files.append(DLDBusHandler::instance(nullptr)->getFileInfo("dnf", false));
        } else if (it.contains(BOOT_TREE_DATA, Qt::CaseInsensitive)) {
            data.logCategory = "boot";
            data.files.append(DLDBusHandler::instance(nullptr)->getFileInfo("boot", false));
        } else if (it.contains(KWIN_TREE_DATA, Qt::CaseInsensitive)) {
            data.logCategory = "kwin";
            data.files.append(KWIN_TREE_DATA);
        } else if (it.contains(APP_TREE_DATA, Qt::CaseInsensitive)) {
            data.logCategory = "apps";
            AppLogConfigList appConfigs = LogApplicationHelper::instance()->getAppLogConfigs();
            for (auto appLogConfig : appConfigs) {
                QString appName = appLogConfig.name;
                if(appName.isEmpty() || !appLogConfig.visible)
                    continue;

                for (int i = 0; i < appLogConfig.subModules.size(); i++) {
                    SubModuleConfig& submodule = appLogConfig.subModules[i];
                    QString dir = appLogConfig.name;
                    QString subDir = QString("%1/%2").arg(dir).arg(submodule.name);
                    if (appLogConfig.subModules.size() == 1 &&  submodule.name == appLogConfig.name)
                        subDir = dir;
                    if (submodule.logType == "file") {
                        QStringList logPaths = DLDBusHandler::instance(nullptr)->getFileInfo(submodule.logPath);
                        logPaths.removeDuplicates();
                        if (logPaths.size() > 0) {
                            data.dir2Files[subDir] = logPaths;
                        } else {
                            qCWarning(logApp) << QString("app:%1 submodule:%2, logPath:%3 not found log files.").arg(appName).arg(submodule.name).arg(submodule.logPath);
                        }
                    } else if (submodule.logType == "journal") {
                        if (submodule.filter.endsWith("*"))
                            qCWarning(logApp) << QString("app:%1 submodule:%2, Export journal logs with wildcard not supported.").arg(appName).arg(submodule.name);
                        else {
                            QJsonObject obj = submodule.toJson();
                            data.dir2Cmds[subDir] = QStringList() << QJsonDocument(obj).toJson(QJsonDocument::Compact);
                        }
                    }
                }
            }
        } else if (it.contains(COREDUMP_TREE_DATA, Qt::CaseInsensitive)) {
            data.logCategory = "coredump";
            data.files.append(DLDBusHandler::instance(nullptr)->getFileInfo("coredump", false));
        } else if (it.contains(OTHER_TREE_DATA, Qt::CaseInsensitive)) {
            data.logCategory = "others";
            auto otherLogListPair = LogApplicationHelper::instance()->getOtherLogList();
            for (auto &it2 : otherLogListPair) {
                QStringList paths = DLDBusHandler::instance(nullptr)->getOtherFileInfo(it2.at(1));
                paths.removeDuplicates();
                if (paths.size() > 1)
                    data.dir2Files[it2.at(0)] = paths;
                else if (paths.size() == 1)
                    data.files.append(paths);
            }
        } else if (it.contains(CUSTOM_TREE_DATA, Qt::CaseInsensitive)) {
            data.logCategory = "customized";
            auto customLogListPair = LogApplicationHelper::instance()->getCustomLogList();
            for (auto &it2 : customLogListPair) {
                data.files.append(it2.at(1));
            }
        } else if (it.contains(AUDIT_TREE_DATA, Qt::CaseInsensitive)) {
            data.logCategory = "audit";
            data.files.append(DLDBusHandler::instance(nullptr)->getFileInfo("audit", false));
        }

        data.files.removeDuplicates();
        data.commands.removeDuplicates();
        totalTasks += data.files.size() + data.commands.size() + data.dir2FilesCount() + data.dir2CmdsCount();
        eList.push_back(data);
    }

    if (eList.isEmpty()) {
        qCWarning(logApp) << "No log types specified for export";
        zipClose(m_zipFile, nullptr);
        QFile::remove(m_outfile);
        emit exportFinsh(false);
        return;
    }

    // Add task for ops log
    totalTasks += 10;

    emit updateTolProcess(totalTasks);
    int completedTasks = 0;

    // --- Processing Logic ---
    for (auto &data : eList) {
        if (m_cancel.load()) break;

        // Files in category root
        for (const auto &file : data.files) {
            if (m_cancel.load()) break;
            // Optimize: extract filename without constructing QFileInfo
            int lastSlash = file.lastIndexOf('/');
            QString fileName = (lastSlash >= 0) ? file.mid(lastSlash + 1) : file;
            QString entryName = "log/" + data.logCategory + '/' + fileName;
            ensureDirectoriesExist(entryName);
            if (!addFileToZip(file, entryName)) {
                 qCWarning(logApp) << "Failed to add file to zip:" << file;
            }
            emit updatecurrentProcess(++completedTasks);
        }
        if (m_cancel.load()) break;

        // Files in subdirectories
        QMapIterator<QString, QStringList> fileMapIt(data.dir2Files);
        while (fileMapIt.hasNext()) {
            if (m_cancel.load()) break;
            fileMapIt.next();
            for (const auto &path : fileMapIt.value()) {
                if (m_cancel.load()) break;
                // Optimize: extract filename without constructing QFileInfo
                int lastSlash = path.lastIndexOf('/');
                QString fileName = (lastSlash >= 0) ? path.mid(lastSlash + 1) : path;
                QString entryName = "log/" + data.logCategory + '/' + fileMapIt.key() + '/' + fileName;
                ensureDirectoriesExist(entryName);
                if (!addFileToZip(path, entryName)) {
                    qCWarning(logApp) << "Failed to add file to zip:" << path;
                }
                emit updatecurrentProcess(++completedTasks);
            }
        }
        if (m_cancel.load()) break;

        // Commands in category root
        for (const auto &command : data.commands) {
            if (m_cancel.load()) break;
            QString entryName = "log/" + data.logCategory + '/' + command + ".log";
            ensureDirectoriesExist(entryName);
            
            if (command == "journalctl_system") {
                addProcessOutputToZip("journalctl", QStringList(), entryName);
            } else if (command == "journalctl_boot") {
                addProcessOutputToZip("journalctl", QStringList() << "-b", entryName);
            } else if (command == "dmesg") {
                addProcessOutputToZip("dmesg", QStringList(), entryName);
            } else if (command == "last") {
                addProcessOutputToZip("last", QStringList(), entryName);
            } else {
                // Generic command handling
                addProcessOutputToZip(command, QStringList(), entryName);
            }
            emit updatecurrentProcess(++completedTasks);
        }
        if (m_cancel.load()) break;

        // Commands in subdirectories
        QMapIterator<QString, QStringList> cmdMapIt(data.dir2Cmds);
        while (cmdMapIt.hasNext()) {
            if (m_cancel.load()) break;
            cmdMapIt.next();
            for (const auto &cmdJson : cmdMapIt.value()) {
                if (m_cancel.load()) break;
                
                // Parse JSON command configuration
                QJsonDocument doc = QJsonDocument::fromJson(cmdJson.toUtf8());
                if (doc.isObject()) {
                    QJsonObject obj = doc.object();
                    QString logType = obj["logType"].toString();
                    QString filter = obj["filter"].toString();
                    
                    if (logType == "journal" && !filter.isEmpty()) {
                        QString entryName = "log/" + data.logCategory + '/' + cmdMapIt.key() + '/' + filter + ".log";
                        ensureDirectoriesExist(entryName);
                        addProcessOutputToZip("journalctl", QStringList() << "-u" << filter, entryName);
                    }
                } else {
                    // Fallback for malformed JSON
                    QString entryName = "log/" + data.logCategory + '/' + cmdMapIt.key() + "/journal.log";
                    ensureDirectoriesExist(entryName);
                    addProcessOutputToZip("journalctl", QStringList(), entryName);
                }
                emit updatecurrentProcess(++completedTasks);
            }
        }
    }

    // --- Export additional ops logs ---
    if (!m_cancel.load()) {
        // Create temporary directory for ops logs
        QString userHomePath = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
        QString opsLogPath = QDir::tempPath() + "/deepin-log-viewer-ops-logs-" + QString::number(QDateTime::currentMSecsSinceEpoch());

        if (QDir().mkpath(opsLogPath)) {
            DLDBusHandler::instance(this)->exportOpsLog(opsLogPath, userHomePath);
            completedTasks += 5;
            emit updatecurrentProcess(completedTasks);
            Utils::exportSomeOpsLogs(opsLogPath, userHomePath);
            completedTasks += 2;
            emit updatecurrentProcess(completedTasks);

            // Add ops logs to zip file with directory structure preserved
            QDir opsDir(opsLogPath);
            if (opsDir.exists()) {
                // Recursive function to add directory structure to zip
                std::function<void(const QDir&, const QString&)> addDirToZip = [&](const QDir& currentDir, const QString& basePath) {
                    if (m_cancel.load()) return;

                    // First, add all files in current directory
                    QStringList files = currentDir.entryList(QDir::Files | QDir::NoDotAndDotDot);
                    for (const QString &file : files) {
                        if (m_cancel.load()) break;

                        QString filePath = currentDir.filePath(file);
                        QString entryName = "log-ops/" + basePath + file;
                        ensureDirectoriesExist(entryName);

                        if (!addFileToZip(filePath, entryName)) {
                            qCWarning(logApp) << "Failed to add ops log file to zip:" << entryName;
                        }
                    }

                    // Create directory entry for current directory if it's empty or has subdirectories
                    if (files.isEmpty()) {
                        QString dirEntryName = "log-ops/" + basePath;
                        if (!dirEntryName.endsWith('/')) {
                            dirEntryName += '/';
                        }

                        // Create empty directory entry
                        zip_fileinfo zfi = {};
                        QDateTime currentTime = QDateTime::currentDateTime();
                        zfi.tmz_date = dateTimeToTmZip(currentTime);
                        zfi.dosDate = 0;

                        if (zipOpenNewFileInZip64(m_zipFile, dirEntryName.toUtf8().constData(), &zfi, nullptr, 0, nullptr, 0, nullptr, Z_DEFLATED, ZIP_COMPRESSION_LEVEL, 1) == ZIP_OK) {
                            zipCloseFileInZip(m_zipFile);
                        }
                    }

                    // Then, recursively process subdirectories
                    QStringList subDirs = currentDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
                    for (const QString &subDir : subDirs) {
                        if (m_cancel.load()) break;

                        QDir subDirectory = currentDir;
                        if (subDirectory.cd(subDir)) {
                            QString subPath = basePath + subDir + "/";
                            addDirToZip(subDirectory, subPath);
                        }
                    }
                };

                // Start recursive addition from the ops directory
                addDirToZip(opsDir, "");
            }
            completedTasks += 2;
            emit updatecurrentProcess(completedTasks);
            // Clean up temporary directory
            if (!Utils::deleteDir(opsLogPath)) {
                qCWarning(logApp) << "Failed to remove temporary ops log directory:" << opsLogPath;
            }
            completedTasks += 1;
            emit updatecurrentProcess(completedTasks);
        } else {
            qCCritical(logApp) << "Failed to create temporary ops log directory:" << opsLogPath;
        }
    }

    zipClose(m_zipFile, "Exported by Deepin Log Viewer");
    m_zipFile = nullptr;

    if (m_cancel.load()) {
        qCInfo(logApp) << "Export cancelled, removing output file";
        QFile::remove(m_outfile);
        emit exportFinsh(false);
    } else {
        qCDebug(logApp) << "Export finished successfully";
        emit exportFinsh(true);
    }
}
