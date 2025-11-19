// SPDX-FileCopyrightText: 2019 - 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "dldbushandler.h"
#include <QDebug>
#include <QStandardPaths>
#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(logApp)

DLDBusHandler *DLDBusHandler::m_statichandeler = nullptr;

DLDBusHandler *DLDBusHandler::instance(QObject *parent)
{
    // qCDebug(logApp) << "DLDBusHandler::instance called with parent:" << parent;
    if (parent != nullptr && m_statichandeler == nullptr) {
        qCDebug(logApp) << "Creating new DLDBusHandler instance";
        m_statichandeler = new DLDBusHandler(parent);
    }
    return m_statichandeler;
}

DLDBusHandler::~DLDBusHandler()
{
    qCDebug(logApp) << "DLDBusHandler destructor called";
    quit();
}

DLDBusHandler::DLDBusHandler(QObject *parent)
    : QObject(parent)
{
    qCDebug(logApp) << "DLDBusHandler constructor called with parent:" << parent;
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
 * \~chinese \brief DLDBusHandler::readLog 读取日志文件
 * \~chinese \param filePath 文件路径
 * \~chinese \return 读取的日志
 */
QString DLDBusHandler::readLog(const QString &filePath)
{
    qCDebug(logApp) << "DLDBusHandler::readLog called with filePath:" << filePath;
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
    return m_dbus->openLogStream(filePath);
}

QString DLDBusHandler::readLogInStream(const QString &token)
{
    qCDebug(logApp) << "DLDBusHandler::readLogInStream called with token:" << token;
    return m_dbus->readLogInStream(token);
}

QStringList DLDBusHandler::whiteListOutPaths()
{
    qCDebug(logApp) << "DLDBusHandler::whiteListOutPaths called";
    return m_dbus->whiteListOutPaths();
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

QStringList DLDBusHandler::getFileInfo(const QString &flag, bool unzip)
{
    qCDebug(logApp) << "DLDBusHandler::getFileInfo called with flag:" << flag << "unzip:" << unzip;
    QDBusPendingReply<QStringList> reply = m_dbus->getFileInfo(flag, unzip);
    reply.waitForFinished();
    if (reply.isError()) {
        qCWarning(logApp) << "call dbus iterface 'getFileInfo()' failed. error info:" << reply.error().message();
    } else {
        qCDebug(logApp) << "getFileInfo succeeded, file count:" << reply.value().size();
        filePath = reply.value();
    }
    return filePath;
}

QStringList DLDBusHandler::getOtherFileInfo(const QString &flag, bool unzip)
{
    qCDebug(logApp) << "DLDBusHandler::getOtherFileInfo called with flag:" << flag << "unzip:" << unzip;
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

bool DLDBusHandler::exportOpsLog(const QString &outDir, const QString &userHomeDir)
{
    qCDebug(logApp) << "DLDBusHandler::exportOpsLog called:" << outDir << userHomeDir;

    m_dbus->setTimeout(1200000);
    QDBusPendingReply<bool> reply = m_dbus->exportOpsLog(outDir, userHomeDir);
    reply.waitForFinished();
    m_dbus->setTimeout(-1);

    if (reply.isError()) {
        qCWarning(logApp) << "call dbus iterface 'exportOpsLog()' failed. error info:" << reply.error().message();
        return false;
    } else {
        qCDebug(logApp) << "exportOpsLog succeeded:" << reply.value();
        return reply.value();
    }
}

bool DLDBusHandler::exportLog(const QString &outDir, const QString &in, bool isFile)
{
    qCDebug(logApp) << "DLDBusHandler::exportLog called with outDir:" << outDir << "in:" << in << "isFile:" << isFile;
    return m_dbus->exportLog(outDir, in, isFile);
}

bool DLDBusHandler::isFileExist(const QString &filePath)
{
    qCDebug(logApp) << "DLDBusHandler::isFileExist called with filePath:" << filePath;
    QString ret = m_dbus->isFileExist(filePath);
    qCDebug(logApp) << "isFileExist result:" << ret;
    return ret == "exist";
}

quint64 DLDBusHandler::getFileSize(const QString &filePath)
{
    qCDebug(logApp) << "DLDBusHandler::getFileSize called with filePath:" << filePath;
    return m_dbus->getFileSize(filePath);
}

qint64 DLDBusHandler::getLineCount(const QString &filePath)
{
    qCDebug(logApp) << "DLDBusHandler::getLineCount called with filePath:" << filePath;
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
