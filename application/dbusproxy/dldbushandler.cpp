// SPDX-FileCopyrightText: 2019 - 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "dldbushandler.h"
#include <QDebug>
#include <QStandardPaths>
#include <QLoggingCategory>

#ifdef QT_DEBUG
Q_LOGGING_CATEGORY(logDBusHandler, "org.deepin.log.viewer.dbus.handler")
#else
Q_LOGGING_CATEGORY(logDBusHandler, "org.deepin.log.viewer.dbus.handler", QtInfoMsg)
#endif

DLDBusHandler *DLDBusHandler::m_statichandeler = nullptr;

DLDBusHandler *DLDBusHandler::instance(QObject *parent)
{
    if (parent != nullptr && m_statichandeler == nullptr) {
        m_statichandeler = new DLDBusHandler(parent);
    }
    return m_statichandeler;
}

DLDBusHandler::~DLDBusHandler()
{
    quit();
}

DLDBusHandler::DLDBusHandler(QObject *parent)
    : QObject(parent)
{
    m_dbus = new DeepinLogviewerInterface("com.deepin.logviewer",
                                          "/com/deepin/logviewer",
                                          QDBusConnection::systemBus(),
                                          this);
    //Note: when dealing with remote objects, it is not always possible to determine if it exists when creating a QDBusInterface.
    if (!m_dbus->isValid() && !m_dbus->lastError().message().isEmpty()) {
        qCCritical(logDBusHandler) << "dbus com.deepin.logviewer isValid false error:" << m_dbus->lastError() << m_dbus->lastError().message();
    }
    qCDebug(logDBusHandler) << "dbus com.deepin.logviewer isValid true";
}

/*!
 * \~chinese \brief DLDBusHandler::readLog 读取日志文件
 * \~chinese \param filePath 文件路径
 * \~chinese \return 读取的日志
 */
QString DLDBusHandler::readLog(const QString &filePath)
{
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

    QDBusUnixFileDescriptor dbusFd(fd);
    QString log = m_dbus->readLog(dbusFd);

    file.close();
    releaseFilePathCacheFile(tempFilePath);

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

    QDBusUnixFileDescriptor dbusFd(fd);
    QStringList lines = m_dbus->readLogLinesInRange(dbusFd, startLine, lineCount, bReverse);

    file.close();
    releaseFilePathCacheFile(tempFilePath);

    return lines;
}

QString DLDBusHandler::openLogStream(const QString &filePath)
{
    return m_dbus->openLogStream(filePath);
}

QString DLDBusHandler::readLogInStream(const QString &token)
{
    return m_dbus->readLogInStream(token);
}

QStringList DLDBusHandler::whiteListOutPaths()
{
    return m_dbus->whiteListOutPaths();
}

/*!
 * \~chinese \brief DLDBusHandler::exitCode 返回进程状态
 * \~chinese \return 进程返回值
 */
int DLDBusHandler::exitCode()
{
    return m_dbus->exitCode();
}

/*!
 * \~chinese \brief DLDBusHandler::quit 退出服务端程序
 */
void DLDBusHandler::quit()
{
    m_dbus->quit();
}

QStringList DLDBusHandler::getFileInfo(const QString &flag, bool unzip)
{
    QDBusPendingReply<QStringList> reply = m_dbus->getFileInfo(flag, unzip);
    reply.waitForFinished();
    if (reply.isError()) {
        qCWarning(logDBusHandler) << "call dbus iterface 'getFileInfo()' failed. error info:" << reply.error().message();
        m_getFileInfoError = true;
    } else {
        filePath = reply.value();
        m_getFileInfoError = false;
    }
    return filePath;
}

bool DLDBusHandler::isGetFileInfoError() const
{
    return m_getFileInfoError;
}

QStringList DLDBusHandler::getOtherFileInfo(const QString &flag, bool unzip)
{
    QDBusPendingReply<QStringList> reply = m_dbus->getOtherFileInfo(flag, unzip);
    reply.waitForFinished();
    QStringList filePathList;
    if (reply.isError()) {
        qCWarning(logDBusHandler) << "call dbus iterface 'getOtherFileInfo()' failed. error info:" << reply.error().message();
    } else {
        filePathList = reply.value();
    }
    return filePathList;
}


bool DLDBusHandler::exportLog(const QString &outDir, const QString &in, bool isFile)
{
    return m_dbus->exportLog(outDir, in, isFile);
}

bool DLDBusHandler::exportOpsLog(const QString &zipFilePath)
{
    // 前端创建压缩包目标文件并以写方式打开，将 fd 通过 D-Bus 传给后端。
    // 后端在 root 权限下收集 /var/log 等运维日志，整体压缩后写入该 fd，
    // 并自行清理 /var/log 下的随机临时目录，前端无需再感知后端临时目录路径。
    QFile zipFile(zipFilePath);
    if (!zipFile.open(QIODevice::WriteOnly)) {
        qCritical(logDBusHandler) << "exportOpsLog: failed to open zip file for writing:" << zipFilePath
                                  << "error:" << zipFile.errorString();
        return false;
    }

    const int fd = zipFile.handle();
    if (fd <= 0) {
        qCritical(logDBusHandler) << "exportOpsLog: invalid file descriptor for:" << zipFilePath;
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
        qCritical(logDBusHandler) << "call dbus interface 'exportOpsLog' failed. error info:" << reply.error().message();
        return false;
    }

    const bool ok = reply.value();
    qCDebug(logDBusHandler) << "exportOpsLog finished, result:" << ok << "zip:" << zipFilePath;
    return ok;
}

bool DLDBusHandler::isFileExist(const QString &filePath)
{
    QDBusPendingReply<QString> reply = m_dbus->isFileExist(filePath);
    reply.waitForFinished();
    bool bRet = false;
    if (reply.isError()) {
        qCWarning(logDBusHandler) << "call dbus iterface 'isFileExist()' failed. error info:" << reply.error().message();
    } else {
        bRet = (reply.value() == "exist");
    }
    return bRet;
}

quint64 DLDBusHandler::getFileSize(const QString &filePath)
{
    return m_dbus->getFileSize(filePath);
}

qint64 DLDBusHandler::getLineCount(const QString &filePath)
{
    return m_dbus->getLineCount(filePath);
}

QString DLDBusHandler::executeCmd(const QString &cmd)
{
    return m_dbus->executeCmd(cmd);
}

QString DLDBusHandler::createFilePathCacheFile(const QString &logFilePath)
{
    QString tempFilePath = m_tempDir.path() + QDir::separator() + "Log_file_path.txt";

    QFile tmpFile(tempFilePath);
    if (!tmpFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "Failed to open temp file:" << tmpFile;
        return QString("");
    }

    QTextStream in(&tmpFile);
    in << logFilePath;
    tmpFile.close();

    return tempFilePath;
}

void DLDBusHandler::releaseFilePathCacheFile(const QString &cacheFilePath)
{
    if (!cacheFilePath.isEmpty())
        QFile::remove(cacheFilePath);
}
