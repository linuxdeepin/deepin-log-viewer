// SPDX-FileCopyrightText: 2019 - 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "dldbushandler.h"
#include "userlogaccess.h"
#include "../utils.h"

#include <QDebug>
#include <QStandardPaths>
#include <QLoggingCategory>
#include <QFile>
#include <QDir>
#include <QFileInfo>
#include <QUrl>
#include <QJsonDocument>
#include <QJsonObject>
#include <QHash>

#include <dgiomount.h>
#include <dgiofile.h>
#include <dgiovolumemanager.h>

#include <fcntl.h>
#include <unistd.h>

Q_DECLARE_LOGGING_CATEGORY(logApp)

// 标准的 Meyers 单例：利用 C++11 魔法静态变量（函数局部静态对象），
// 其初始化由编译器保证线程安全且仅执行一次，多线程首次调用即创建。
// 单例无父对象，自行管理生命周期，进程退出时由 OS 回收。
DLDBusHandler *DLDBusHandler::instance()
{
    static DLDBusHandler instance;
    return &instance;
}

DLDBusHandler::~DLDBusHandler()
{
    qCDebug(logApp) << "DLDBusHandler destructor called";
    // 不再主动调用后端 quit()：后端会在所有 D-Bus 客户端断开后自行退出。
}

DLDBusHandler::DLDBusHandler(QObject *parent)
    : QObject(parent)
{
    qCDebug(logApp) << "DLDBusHandler constructor called with parent:" << parent;
    // 注意：D-Bus 接口对象以 this 为父对象，随单例一起销毁。
    m_dbus = new DeepinLogviewerInterface("com.deepin.logviewer",
                                          "/com/deepin/logviewer",
                                          QDBusConnection::systemBus(),
                                          this);
    //Note: when dealing with remote objects, it is not always possible to determine if it exists when creating a QDBusInterface.
    if (!m_dbus->isValid() && !m_dbus->lastError().message().isEmpty()) {
        qCCritical(logApp) << "dbus com.deepin.logviewer isValid false error:" << m_dbus->lastError() << m_dbus->lastError().message();
    }
    qCDebug(logApp) << "dbus com.deepin.logviewer isValid true";
}

/*!
 * \~chinese \brief DLDBusHandler::isHomePath 判断路径是否属于当前用户 home 目录
 * \~chinese ProtectHome=tmpfs 后，home 下的日志由 UserLogAccess 本地读取，不再走 DBus。
 */
bool DLDBusHandler::isHomePath(const QString &filePath) const
{
    if (filePath.isEmpty() || Utils::homePath.isEmpty()) {
        return false;
    }
    // 精确匹配 home 前缀：path == home 或 path 以 home + "/" 开头
    if (filePath == Utils::homePath) {
        return true;
    }
    return filePath.startsWith(Utils::homePath + "/");
}

/*!
 * \~chinese \brief DLDBusHandler::readLog 读取日志文件
 * \~chinese \param filePath 文件路径
 * \~chinese \return 读取的日志
 */
QString DLDBusHandler::readLog(const QString &filePath)
{
    qCDebug(logApp) << "DLDBusHandler::readLog called with filePath:" << filePath;

    // home 目录下日志由用户日志访问类本地读取
    if (isHomePath(filePath)) {
        return UserLogAccess::instance()->readLog(filePath);
    }

    // 非_home 路径走后端 DBus（需 polkit 提权，后端以 root 读取 /var/log 等）
    QString tempFilePath = createFilePathCacheFile(filePath);
    QFile file(tempFilePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Failed to open filePath cache file:" << tempFilePath;
        return QString("");
    }
    const int fd = file.handle();
    if (fd <= 0) {
        qWarning() << "originPath file fd error. filePath cache file:" << tempFilePath;
        return QString("");
    }

    qCDebug(logApp) << "Creating DBus file descriptor for reading log";
    QDBusUnixFileDescriptor dbusFd(fd);
    QString log = m_dbus->readLog(dbusFd);

    file.close();
    releaseFilePathCacheFile(tempFilePath);
    qCDebug(logApp) << "DLDBusHandler::readLog completed, log length:" << log.length();

    return log;
}

/*!
 * \~chinese \brief DLDBusHandler::readLogLinesInRange 获取指定行数范围的日志内容，默认读取500条数据
 * \~chinese \param filePath 文件路径
 * \~chinese \param startLine 起始行
 * \~chinese \param lineCount 获取行数
 * \~chinese \return 读取的日志
 */
QStringList DLDBusHandler::readLogLinesInRange(const QString &filePath, qint64 startLine, qint64 lineCount, bool bReverse)
{
    qCDebug(logApp) << "DLDBusHandler::readLogLinesInRange called with filePath:" << filePath << "startLine:" << startLine << "lineCount:" << lineCount << "bReverse:" << bReverse;

    // home 目录下日志由用户日志访问类本地读取
    if (isHomePath(filePath)) {
        return UserLogAccess::instance()->readLogLinesInRange(filePath, startLine, lineCount, bReverse);
    }

    QString tempFilePath = createFilePathCacheFile(filePath);
    QFile file(tempFilePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Failed to open filePath cache file:" << tempFilePath;
        return QStringList();
    }
    const int fd = file.handle();
    if (fd <= 0) {
        qWarning() << "originPath file fd error. filePath cache file:" << tempFilePath;
        return QStringList();
    }

    qCDebug(logApp) << "Creating DBus file descriptor for reading log lines in range";
    QDBusUnixFileDescriptor dbusFd(fd);
    QStringList lines = m_dbus->readLogLinesInRange(dbusFd, startLine, lineCount, bReverse);

    file.close();
    releaseFilePathCacheFile(tempFilePath);
    qCDebug(logApp) << "DLDBusHandler::readLogLinesInRange completed, lines count:" << lines.size();

    return lines;
}

QString DLDBusHandler::openLogStream(const QString &filePath)
{
    qCDebug(logApp) << "DLDBusHandler::openLogStream called with filePath:" << filePath;

    // home 目录下日志由用户日志访问类本地读取，返回 local- 前缀 token
    if (isHomePath(filePath)) {
        return UserLogAccess::instance()->openLogStream(filePath);
    }

    return m_dbus->openLogStream(filePath);
}

QString DLDBusHandler::readLogInStream(const QString &token)
{
    qCDebug(logApp) << "DLDBusHandler::readLogInStream called with token:" << token;

    // local- 前缀 token 路由到用户日志访问类
    if (token.startsWith(QStringLiteral("local-"))) {
        return UserLogAccess::instance()->readLogInStream(token);
    }

    return m_dbus->readLogInStream(token);
}

QStringList DLDBusHandler::whiteListOutPaths()
{
    qCDebug(logApp) << "DLDBusHandler::whiteListOutPaths called";
    // ProtectHome=tmpfs 后后端无法列举 /home 和 /run/user，前端自行实现白名单。
    QStringList paths;

    // 用户家目录
    if (!Utils::homePath.isEmpty() && Utils::homePath != "/" && Utils::homePath != "/root") {
        paths << Utils::homePath;
    }

    // 外设挂载路径（包括 SMB/gvfs）
    const QList<QExplicitlySharedDataPointer<DGioMount>> mounts = DGioVolumeManager::getMounts();
    for (auto mount : mounts) {
        QString uri = mount->getRootFile()->uri();
        QString scheme = QUrl(uri).scheme();

        // SMB 路径判断
        QRegularExpression recifs("^file:///media/(.*)/smbmounts");
        QRegularExpression regvfs("^file:///run/user/(.*)/gvfs|^/root/.gvfs");
        if (recifs.match(uri).hasMatch() || regvfs.match(uri).hasMatch()) {
            QString path = QUrl(uri).toLocalFile();
            QFlags<QFileDevice::Permission> power = QFile::permissions(path);
            if (power.testFlag(QFile::WriteUser))
                paths << path;
        }

        // 外设路径判断
        if ((scheme == "file") ||
                (scheme == "gphoto2") ||
                (scheme == "mtp")) {
            QExplicitlySharedDataPointer<DGioFile> locationFile = mount->getDefaultLocationFile();
            QString path = locationFile->path();
            if (path.startsWith("/media/")) {
                QFlags<QFileDevice::Permission> power = QFile::permissions(path);
                if (power.testFlag(QFile::WriteUser)) {
                    paths << path;
                }
            }
        }
    }

    // 临时目录
    paths << "/tmp";

    return paths;
}

/*!
 * \~chinese \brief DLDBusHandler::exitCode 返回进程状态
 * \~chinese \return 进程返回值
 */
int DLDBusHandler::exitCode()
{
    qCDebug(logApp) << "DLDBusHandler::exitCode called";
    return m_dbus->exitCode();
}

/*!
 * \~chinese \brief DLDBusHandler::quit 退出服务端程序
 */
void DLDBusHandler::quit()
{
    qCDebug(logApp) << "DLDBusHandler::quit called";
    m_dbus->quit();
}

bool DLDBusHandler::isGetFileInfoError() const
{
    return m_bGetFileInfoError;
}

QStringList DLDBusHandler::getFileInfo(const QString &flag, bool unzip)
{
    qCDebug(logApp) << "DLDBusHandler::getFileInfo called with flag:" << flag << "unzip:" << unzip;

    // home 目录下日志由用户日志访问类本地列举
    if (isHomePath(flag)) {
        return UserLogAccess::instance()->getFileInfo(flag, unzip);
    }

    QDBusPendingReply<QStringList> reply = m_dbus->getFileInfo(flag, unzip);
    reply.waitForFinished();
    if (reply.isError()) {
        qCWarning(logApp) << "call dbus iterface 'getFileInfo()' failed. error info:" << reply.error().message();
        m_bGetFileInfoError = true;
    } else {
        qCDebug(logApp) << "getFileInfo succeeded, file count:" << reply.value().size();
        filePath = reply.value();
        m_bGetFileInfoError = false;
    }
    return filePath;
}

QStringList DLDBusHandler::getOtherFileInfo(const QString &flag, bool unzip)
{
    qCDebug(logApp) << "DLDBusHandler::getOtherFileInfo called with flag:" << flag << "unzip:" << unzip;

    // home 目录下日志由用户日志访问类本地列举
    if (isHomePath(flag)) {
        return UserLogAccess::instance()->getOtherFileInfo(flag, unzip);
    }

    QDBusPendingReply<QStringList> reply = m_dbus->getOtherFileInfo(flag, unzip);
    reply.waitForFinished();
    QStringList filePathList;
    if (reply.isError()) {
        qCWarning(logApp) << "call dbus iterface 'getOtherFileInfo()' failed. error info:" << reply.error().message();
    } else {
        qCDebug(logApp) << "getOtherFileInfo succeeded, file count:" << reply.value().size();
        filePathList = reply.value();
    }
    return filePathList;
}

// 计算导出目标文件名：与后端原 openat(dirFd, safeName) 逻辑对齐。
// isFile=true: 取源文件 basename；isFile=false: 取命令名/子模块名 + ".log"。
static QString computeExportFileName(const QString &in, bool isFile)
{
    if (isFile) {
        return QFileInfo(in).fileName();
    }

    // JSON 分支：解析 submoduleName 作为目标文件名
    QJsonParseError parseError;
    QJsonDocument document = QJsonDocument::fromJson(in.toUtf8(), &parseError);
    if (parseError.error == QJsonParseError::NoError && document.isObject()) {
        QJsonObject object = document.object();
        if (object.contains("name")) {
            return object.value("name").toString() + ".log";
        }
    }

    // 硬编码白名单命令分支：命令名 + ".log"
    static const QHash<QString, QString> commandNames = {
        {"dmesg", "dmesg.log"},
        {"last", "last.log"},
        {"journalctl_system", "journalctl_system.log"},
        {"journalctl_boot", "journalctl_boot.log"},
        {"journalctl_app", "journalctl_app.log"}
    };
    return commandNames.value(in, in + ".log");
}

bool DLDBusHandler::exportLog(const QString &outDir, const QString &in, bool isFile)
{
    qCDebug(logApp) << "DLDBusHandler::exportLog called with outDir:" << outDir << "in:" << in << "isFile:" << isFile;

    if (in.isEmpty()) {
        qCWarning(logApp) << "exportLog: empty input";
        return false;
    }

    // 计算目标文件名并拼接完整目标文件路径
    const QString fileName = computeExportFileName(in, isFile);
    if (fileName.isEmpty()) {
        qCWarning(logApp) << "exportLog: failed to compute file name for:" << in;
        return false;
    }
    // 规范化目录路径：去掉末尾多余的 '/'，再拼接文件名
    QString dir = outDir;
    while (dir.endsWith('/'))
        dir.chop(1);
    const QString outFilePath = dir + QDir::separator() + fileName;

    // 源文件在 home 下：前端本地复制到目标文件
    if (isFile && isHomePath(in)) {
        QFile sourceFile(in);
        if (!sourceFile.open(QIODevice::ReadOnly)) {
            qCWarning(logApp) << "exportLog: failed to open source file:" << in;
            return false;
        }
        QFile targetFile(outFilePath);
        if (!targetFile.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
            qCWarning(logApp) << "exportLog: failed to open target file:" << outFilePath;
            sourceFile.close();
            return false;
        }
        const qint64 chunkSize = 1024 * 1024;
        QScopedPointer<char> buffer(new char[chunkSize]);
        bool error = false;
        while (!sourceFile.atEnd()) {
            qint64 bytesRead = sourceFile.read(buffer.data(), chunkSize);
            if (bytesRead > 0) {
                if (targetFile.write(buffer.data(), bytesRead) != bytesRead) {
                    qCWarning(logApp) << "exportLog: failed to write all bytes to target";
                    error = true;
                    break;
                }
            } else if (bytesRead < 0) {
                qCWarning(logApp) << "exportLog: error reading from source:" << in;
                error = true;
                break;
            }
        }
        targetFile.flush();
        sourceFile.close();
        targetFile.close();
        return !error;
    }

    // 非_home 源路径走后端 DBus（后端以 root 读取 /var/log 等）
    // 前端打开目标文件（写方式），将 fd 通过 D-Bus 传给后端。
    QDBusPendingReply<bool> reply = m_dbus->exportLog(outFilePath, in, isFile);
    reply.waitForFinished();
    if (reply.isError()) {
        qCWarning(logApp) << "call dbus interface 'exportLog' failed. error info:" << reply.error().message();
        return false;
    }
    return reply.value();
}

bool DLDBusHandler::isFileExist(const QString &filePath)
{
    qCDebug(logApp) << "DLDBusHandler::isFileExist called with filePath:" << filePath;

    // home 目录下日志由用户日志访问类本地检查
    if (isHomePath(filePath)) {
        return UserLogAccess::instance()->isFileExist(filePath);
    }

    QString ret = m_dbus->isFileExist(filePath);
    qCDebug(logApp) << "isFileExist result:" << ret;
    return ret == "exist";
}

quint64 DLDBusHandler::getFileSize(const QString &filePath)
{
    qCDebug(logApp) << "DLDBusHandler::getFileSize called with filePath:" << filePath;

    // home 目录下日志由用户日志访问类本地获取
    if (isHomePath(filePath)) {
        return UserLogAccess::instance()->getFileSize(filePath);
    }

    return m_dbus->getFileSize(filePath);
}

qint64 DLDBusHandler::getLineCount(const QString &filePath)
{
    qCDebug(logApp) << "DLDBusHandler::getLineCount called with filePath:" << filePath;

    // home 目录下日志由用户日志访问类本地统计
    if (isHomePath(filePath)) {
        return UserLogAccess::instance()->getLineCount(filePath);
    }

    return m_dbus->getLineCount(filePath);
}

QString DLDBusHandler::executeCmd(const QString &cmd)
{
    qCDebug(logApp) << "DLDBusHandler::executeCmd called with cmd:" << cmd;
    return m_dbus->executeCmd(cmd);
}

QString DLDBusHandler::createFilePathCacheFile(const QString &logFilePath)
{
    qCDebug(logApp) << "DLDBusHandler::createFilePathCacheFile called with logFilePath:" << logFilePath;
    QString tempFilePath = m_tempDir.path() + QDir::separator() + "Log_file_path.txt";

    QFile tmpFile(tempFilePath);
    if (!tmpFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "Failed to open temp file:" << tempFilePath;
        return QString("");
    }

    qCDebug(logApp) << "Writing to temp file:" << tempFilePath;
    QTextStream in(&tmpFile);
    in << logFilePath;
    tmpFile.close();

    return tempFilePath;
}

void DLDBusHandler::releaseFilePathCacheFile(const QString &cacheFilePath)
{
    qCDebug(logApp) << "DLDBusHandler::releaseFilePathCacheFile called with cacheFilePath:" << cacheFilePath;
    if (!cacheFilePath.isEmpty()) {
        qCDebug(logApp) << "Removing cache file:" << cacheFilePath;
        QFile::remove(cacheFilePath);
    }
}

bool DLDBusHandler::exportOpsLog(const QString &zipFilePath)
{
    // 前端创建压缩包目标文件并以写方式打开，将 fd 通过 D-Bus 传给后端。
    // 后端在 root 权限下收集 /var/log 等运维日志，整体压缩后写入该 fd，
    // 并自行清理 /var/log 下的随机临时目录，前端无需再感知后端临时目录路径。
    QFile zipFile(zipFilePath);
    if (!zipFile.open(QIODevice::WriteOnly)) {
        qCritical() << "exportOpsLog: failed to open zip file for writing:" << zipFilePath
                    << "error:" << zipFile.errorString();
        return false;
    }

    const int fd = zipFile.handle();
    if (fd <= 0) {
        qCritical() << "exportOpsLog: invalid file descriptor for:" << zipFilePath;
        zipFile.close();
        return false;
    }

    QDBusUnixFileDescriptor dbusFd(fd);

    m_dbus->setTimeout(1200000);
    QDBusPendingReply<bool> reply = m_dbus->exportOpsLog(dbusFd);
    reply.waitForFinished();
    m_dbus->setTimeout(-1);

    // 后端写完后会关闭其 dup 的 fd，但前端打开的 QFile 仍需由前端关闭。
    zipFile.close();

    if (reply.isError()) {
        qCritical() << "call dbus interface 'exportOpsLog' failed. error info:" << reply.error().message();
        return false;
    }

    const bool ok = reply.value();
    return ok;
}
