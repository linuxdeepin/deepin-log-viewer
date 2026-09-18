// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "userlogaccess.h"
#include "../utils.h"

#include <QFile>
#include <QDir>
#include <QFileInfo>
#include <QTemporaryFile>
#include <QTextStream>
#include <QProcess>
#include <QCryptographicHash>
#include <QUuid>
#include <QStandardPaths>
#include <QLoggingCategory>
#include <QRegularExpression>

#include <fstream>

Q_DECLARE_LOGGING_CATEGORY(logApp)
Q_LOGGING_CATEGORY(logUserLogAccess, "org.deepin.log.viewer.userlogaccess", QtInfoMsg)

// 流式读取单次最大返回大小
static constexpr int kMaxReadSize = 10 * 1024 * 1024;

// 读取崩溃应用 maps 信息最大行数
static constexpr int kCoredumpMapsMaxLines = 200;

// 流式读取单次最大行数
static constexpr int kStreamMaxLines = 500;

UserLogAccess *UserLogAccess::instance()
{
    static UserLogAccess s_instance;
    return &s_instance;
}

UserLogAccess::UserLogAccess(QObject *parent)
    : QObject(parent)
{
    // 初始化解压根目录：~/.cache/deepin/deepin-log-viewer/user-log
    m_extractRoot = Utils::homePath + QStringLiteral("/.cache/deepin/deepin-log-viewer/user-log");
    QDir().mkpath(m_extractRoot);

    // 清理上次残留的解压子目录
    QDir rootDir(m_extractRoot);
    rootDir.setFilter(QDir::Dirs | QDir::NoDotAndDotDot);
    for (const QString &subdir : rootDir.entryList()) {
        QDir sub(m_extractRoot + QDir::separator() + subdir);
        sub.removeRecursively();
    }
}

UserLogAccess::~UserLogAccess()
{
    QMutexLocker locker(&m_mutex);
    for (auto &pair : m_logMap) {
        delete pair.second;
    }

    // 清理解压子目录，避免残留
    QDir rootDir(m_extractRoot);
    rootDir.setFilter(QDir::Dirs | QDir::NoDotAndDotDot);
    for (const QString &subdir : rootDir.entryList()) {
        QDir sub(m_extractRoot + QDir::separator() + subdir);
        sub.removeRecursively();
    }
}

QString UserLogAccess::createExtractSubdir()
{
    // 每次调用创建独立子目录，避免并发互删
    const QString subdir = m_extractRoot + QDir::separator()
                           + QStringLiteral("extract-") + QUuid::createUuid().toString(QUuid::WithoutBraces);
    QDir().mkpath(subdir);
    return subdir;
}

void UserLogAccess::removeDirFiles(const QString &dirPath)
{
    QDir dir(dirPath);
    dir.setFilter(QDir::NoDotAndDotDot | QDir::Files);
    for (const QString &item : dir.entryList()) {
        dir.remove(item);
    }
}

QString UserLogAccess::unzipToTempFile(const QString &sourceFile, const QString &tempFileTemplate)
{
    QTemporaryFile tmpFile;
    tmpFile.setAutoRemove(false);
    tmpFile.setFileTemplate(tempFileTemplate);
    if (!tmpFile.open()) {
        qCWarning(logUserLogAccess) << "Create temporary file failed:" << tmpFile.errorString();
        return QString();
    }

    QProcess proc;
    proc.setStandardOutputFile(tmpFile.fileName());
    proc.start(QStringLiteral("gunzip"), QStringList() << QStringLiteral("-c") << sourceFile);
    proc.waitForFinished(-1);

    return tmpFile.fileName();
}

QString UserLogAccess::readLog(const QString &filePath)
{
    qCDebug(logUserLogAccess) << "readLog:" << filePath;
    if (filePath.isEmpty()) {
        return QString();
    }

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qCWarning(logUserLogAccess) << "Failed to open file:" << filePath << file.errorString();
        return QString();
    }

    QByteArray byte = file.readAll();
    file.close();

    // 0x00 替换为 0x20（与后端实现一致）
    for (int i = 0; i < byte.size(); ++i) {
        if (byte.at(i) == 0x00) {
            byte[i] = 0x20;
        }
    }
    return QString::fromUtf8(byte);
}

qint64 UserLogAccess::readFileAndReturnIndex(const QString &filePath, qint64 startLine,
                                             QList<uint64_t> &lineIndexes, bool reverseOrder)
{
    std::ifstream file(filePath.toStdString());
    if (!file.is_open()) {
        return -1;
    }

    std::string line;
    uint64_t lineNumber = 0;
    uint64_t startIndex = 0;

    if (lineIndexes.empty()) {
        while (std::getline(file, line)) {
            lineNumber++;
            lineIndexes.push_back(startIndex);
            startIndex = file.tellg();
            if (lineNumber > startLine && !reverseOrder) {
                break;
            }
        }
    } else {
        if (startLine < static_cast<qint64>(lineIndexes.size()) && !reverseOrder) {
            return lineIndexes[startLine];
        } else {
            file.seekg(lineIndexes.last());
            lineNumber = lineIndexes.size();
            while (std::getline(file, line)) {
                lineNumber++;
                startIndex = file.tellg();
                lineIndexes.push_back(startIndex);
                if (lineNumber > startLine && !reverseOrder) {
                    break;
                }
            }
            if (reverseOrder)
                lineIndexes.removeLast();
        }
    }

    file.close();

    if (reverseOrder) {
        startLine = lineIndexes.size() - startLine - 1;
        if (startLine < 0)
            return -1;
        return lineIndexes.at(startLine);
    }

    return lineIndexes.at(startLine);
}

QStringList UserLogAccess::readLogLinesInRange(const QString &filePath, qint64 startLine,
                                               qint64 lineCount, bool bReverse)
{
    qCDebug(logUserLogAccess) << "readLogLinesInRange:" << filePath << startLine << lineCount << bReverse;
    QStringList lines;

    if (filePath.isEmpty()) {
        return lines;
    }

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qCWarning(logUserLogAccess) << "Failed to open file:" << filePath;
        return lines;
    }

    QMutexLocker locker(&m_mutex);

    QString token = QCryptographicHash::hash(filePath.toUtf8(), QCryptographicHash::Md5).toHex();
    qint64 startLineIndex = 0;
    if (m_logLineIndex.contains(token)) {
        startLineIndex = readFileAndReturnIndex(filePath, startLine, m_logLineIndex[token], bReverse);
    } else {
        QList<uint64_t> indexList;
        startLineIndex = readFileAndReturnIndex(filePath, startLine, indexList, bReverse);
        m_logLineIndex.insert(token, indexList);
    }

    if (bReverse) {
        int startLineCount = m_logLineIndex[token].size() - lineCount;
        startLineIndex = startLineCount > 0 ? startLineCount : 0;
    }

    if (startLineIndex < 0) {
        return lines;
    }

    if (!file.seek(m_logLineIndex[token].at(startLineIndex))) {
        qCWarning(logUserLogAccess) << "Failed to seek file:" << filePath;
        return lines;
    }

    QTextStream in(&file);
    while (!in.atEnd() && lines.size() < lineCount) {
        QString line = in.readLine();
        if (line.contains('\x00'))
            lines.append(line.replace(QChar('\x00'), ""));
        else
            lines.append(line);
    }

    return lines;
}

QStringList UserLogAccess::getFileInfo(const QString &file, bool unzip)
{
    qCDebug(logUserLogAccess) << "getFileInfo:" << file << unzip;
    QStringList fileNamePath;

    if (file.isEmpty()) {
        return fileNamePath;
    }

    // 应用日志分支：file 包含 deepin/uos，视为应用日志路径
    if (file.contains("deepin", Qt::CaseInsensitive) || file.contains("uos", Qt::CaseInsensitive)) {
        QFileInfo appFileInfo(file);
        QString appDir;
        if (appFileInfo.isFile()) {
            appDir = appFileInfo.absolutePath();
        } else if (appFileInfo.isDir()) {
            appDir = appFileInfo.absoluteFilePath();
        } else {
            return fileNamePath;
        }

        QString nameFilter = appDir.mid(appDir.lastIndexOf("/") + 1);
        QDir dir(appDir);
        dir.setFilter(QDir::Files | QDir::NoSymLinks);
        dir.setNameFilters(QStringList() << nameFilter + ".*");
        dir.setSorting(QDir::Time);

        QFileInfoList fileList = dir.entryInfoList();
        if (fileList.size() == 0)
            nameFilter = appFileInfo.completeBaseName();

        if (!dir.exists()) {
            return fileNamePath;
        }
        dir.setFilter(QDir::Files | QDir::NoSymLinks);
        dir.setNameFilters(QStringList() << nameFilter + ".*");
        dir.setSorting(QDir::Time);
        fileList = dir.entryInfoList();

        // 独立解压子目录
        QString extractDir = createExtractSubdir();
        QString tempFileTemplate = extractDir + QDir::separator() + "Log_extract_XXXXXX.txt";

        for (int i = 0; i < fileList.count(); i++) {
            if (fileList[i].suffix().compare("gz", Qt::CaseInsensitive) == 0 && unzip) {
                QString unzipFile = unzipToTempFile(fileList[i].absoluteFilePath(), tempFileTemplate);
                if (!unzipFile.isEmpty())
                    fileNamePath.append(unzipFile);
            } else {
                fileNamePath.append(fileList[i].absoluteFilePath());
            }
        }
    } else {
        // 其他类型在用户 home 下的通用文件列举
        QFileInfo fi(file);
        if (!fi.exists()) {
            return fileNamePath;
        }
        QDir dir;
        QString nameFilter;
        if (fi.isFile()) {
            dir.setPath(fi.absolutePath());
            nameFilter = fi.fileName();
            dir.setNameFilters(QStringList() << nameFilter + "*");
        } else if (fi.isDir()) {
            dir.setPath(file);
        }
        dir.setFilter(QDir::Files | QDir::NoSymLinks | QDir::Hidden);
        dir.setSorting(QDir::Time);
        QFileInfoList fileList = dir.entryInfoList();

        QString extractDir = createExtractSubdir();
        QString tempFileTemplate = extractDir + QDir::separator() + "Log_extract_XXXXXX.txt";

        for (int i = 0; i < fileList.count(); i++) {
            if (fileList[i].suffix().compare("gz", Qt::CaseInsensitive) == 0 && unzip) {
                QString unzipFile = unzipToTempFile(fileList[i].absoluteFilePath(), tempFileTemplate);
                if (!unzipFile.isEmpty())
                    fileNamePath.append(unzipFile);
            } else {
                fileNamePath.append(fileList[i].absoluteFilePath());
            }
        }
    }

    return fileNamePath;
}

QStringList UserLogAccess::getOtherFileInfo(const QString &file, bool unzip)
{
    qCDebug(logUserLogAccess) << "getOtherFileInfo:" << file << unzip;
    QStringList fileNamePath;

    QFileInfo appFileInfo(file);
    if (!appFileInfo.exists()) {
        qCWarning(logUserLogAccess) << "path not exists:" << file;
        return fileNamePath;
    }

    QDir dir;
    QString nameFilter;
    if (appFileInfo.isFile()) {
        dir.setPath(appFileInfo.absolutePath());
        nameFilter = appFileInfo.fileName();
        dir.setNameFilters(QStringList() << nameFilter + "*");
    } else if (appFileInfo.isDir()) {
        dir.setPath(file);
    }

    dir.setFilter(QDir::Files | QDir::NoSymLinks | QDir::Hidden);
    dir.setSorting(QDir::Time);
    QFileInfoList fileList = dir.entryInfoList();

    QString extractDir = createExtractSubdir();
    QString tempFileTemplate = extractDir + QDir::separator() + "Log_extract_XXXXXX.txt";

    for (int i = 0; i < fileList.count(); i++) {
        if (fileList[i].suffix().compare("gz", Qt::CaseInsensitive) == 0 && unzip) {
            QString unzipFile = unzipToTempFile(fileList[i].absoluteFilePath(), tempFileTemplate);
            if (!unzipFile.isEmpty())
                fileNamePath.append(unzipFile);
        } else {
            fileNamePath.append(fileList[i].absoluteFilePath());
        }
    }

    return fileNamePath;
}

bool UserLogAccess::isFileExist(const QString &filePath)
{
    return QFile::exists(filePath);
}

quint64 UserLogAccess::getFileSize(const QString &filePath)
{
    QFileInfo fi(filePath);
    if (fi.exists()) {
        return static_cast<quint64>(fi.size());
    }
    return 0;
}

qint64 UserLogAccess::getLineCount(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qCWarning(logUserLogAccess) << "Failed to open file for line count:" << filePath;
        return -1;
    }

    qint64 lineCount = 0;
    QByteArray block;
    static constexpr qint64 kBlockSize = 1024 * 1024;
    while (!(block = file.read(kBlockSize)).isEmpty()) {
        lineCount += block.count('\n');
    }
    // 若文件非空且不以换行结尾，补计最后一行
    if (lineCount == 0 && file.size() > 0) {
        lineCount = 1;
    }
    return lineCount;
}

QString UserLogAccess::openLogStream(const QString &filePath)
{
    qCDebug(logUserLogAccess) << "openLogStream:" << filePath;
    QString result = readLog(filePath);
    if (result.isEmpty()) {
        qCWarning(logUserLogAccess) << "Failed to read log file for stream";
        return QString();
    }

    // 使用随机 UUID 作为 token，加 local- 前缀以便 DLDBusHandler 路由识别
    QString token = QStringLiteral("local-") + QUuid::createUuid().toString(QUuid::WithoutBraces);

    QMutexLocker locker(&m_mutex);
    auto stream = new QTextStream;
    m_logMap[token] = std::make_pair(result, stream);
    stream->setString(&(m_logMap[token].first), QIODevice::ReadOnly);

    return token;
}

QString UserLogAccess::readLogInStream(const QString &token)
{
    qCDebug(logUserLogAccess) << "readLogInStream:" << token;

    QMutexLocker locker(&m_mutex);
    if (!m_logMap.contains(token)) {
        qCWarning(logUserLogAccess) << "Token not found:" << token;
        return QString();
    }

    auto stream = m_logMap[token].second;
    QString result;
    int linesRead = 0;
    while (true) {
        auto data = stream->readLine();
        if (data.isEmpty()) {
            break;
        }
        result += data + '\n';
        linesRead++;
        if (result.size() > kMaxReadSize) {
            break;
        }
    }

    if (result.isEmpty()) {
        delete m_logMap[token].second;
        m_logMap.remove(token);
    }

    return result;
}
