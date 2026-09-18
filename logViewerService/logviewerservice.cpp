// SPDX-FileCopyrightText: 2019 - 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "logviewerservice.h"
#include "opslogexport.h"
#include "qtcompat.h"

#include <pwd.h>
#include <unistd.h>
#include <fstream>
#include <fcntl.h>
#include <sys/stat.h>
#include <dirent.h>

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
#include <polkit-qt5-1/PolkitQt1/Authority>
#else
#include <polkit-qt6-1/PolkitQt1/Authority>
#endif
using namespace PolkitQt1;

#include <QMutex>
#include <QUrl>
#include <QRegularExpression>
#include <QCoreApplication>
#include <QDebug>
#include <QStringList>
#include <QCryptographicHash>
#include <QTextStream>
#include <QDBusConnection>
#include <QDBusMessage>
#include <QDBusConnectionInterface>
#include <QStandardPaths>
#include <QLoggingCategory>
#include <QJsonDocument>
#include <QJsonObject>
#include <QTemporaryFile>
#include <QUuid>

#ifdef QT_DEBUG
Q_LOGGING_CATEGORY(logService, "org.deepin.log.viewer.service")
#else
Q_LOGGING_CATEGORY(logService, "org.deepin.log.viewer.service", QtInfoMsg)
#endif

// 读取崩溃应用maps信息最大行数
#define COREDUMP_MAPS_MAX_LINES 200

const QString s_Action_View = "com.deepin.pkexec.logViewerAuth";
const QString s_Action_Export = "com.deepin.pkexec.logViewerAuth.exportLogs";

/**
   @brief 移除路径 \a dirPath 下的文件，注意，不会递归删除文件夹
 */
void removeDirFiles(const QString &dirPath)
{
    qCDebug(logService) << "Removing files in directory:" << dirPath;
    QDir dir(dirPath);
    dir.setFilter(QDir::NoDotAndDotDot | QDir::Files);
    foreach(QString dirItem, dir.entryList()) {
        if (!dir.remove(dirItem)) {
            qCWarning(logService) << QString("Remove temporary dir file [%1] failed").arg(dirItem);
        }
    }
}

/**
   @brief 将压缩源文件 \a sourceFile 解压到临时文件，临时文件由模板 \a tempFileTemplate 生成，
        若文件创建异常，将返回空路径；正常解压返回临时文件路径。
 */
QString unzipToTempFile(const QString &sourceFile, const QString &tempFileTemplate)
{
    qCDebug(logService) << "Unzipping file:" << sourceFile << "to temporary file:" << tempFileTemplate;
    QProcess m_process;

    // 每次创建临时文件，销毁由 QTemporaryDir 处理
    QTemporaryFile tmpFile;
    tmpFile.setAutoRemove(false);
    tmpFile.setFileTemplate(tempFileTemplate);
    tmpFile.open();
    if (!tmpFile.open()) {
        qCWarning(logService) << QString("Create temporary file [%1](FileTemplate:%2) failed: %3")
                                 .arg(tmpFile.fileName()).arg(tempFileTemplate).arg(tmpFile.errorString());
        return QString();
    }

    QString command = "gunzip";
    QStringList args;
    args.append("-c");
    args.append(sourceFile);
    m_process.setStandardOutputFile(tmpFile.fileName());
    m_process.start(command, args);
    m_process.waitForFinished(-1);

    return tmpFile.fileName();
}

LogViewerService::LogViewerService(QObject *parent)
    : QObject(parent)
{
    qCDebug(logService) << "LogViewerService constructor called";
    m_commands.insert("dmesg", QStringList() << "dmesg" << "-r");
    m_commands.insert("last", QStringList() << "last" << "-x");
    m_commands.insert("journalctl_system", QStringList() << "journalctl" << "-r");
    m_commands.insert("journalctl_boot", QStringList() << "journalctl" << "-b" << "-r");
    m_commands.insert("journalctl_app", QStringList() << "journalctl");

    m_actionId = s_Action_View;
    qCDebug(logService) << "Commands initialized, action ID set to:" << m_actionId;

    // 监听系统总线上调用方的唯一总线名属主变化：客户端进程退出时其唯一
    // 总线名被释放，watcher 发出 serviceUnregistered，据此实现“无客户端即退出”。
    m_clientWatcher = new QDBusServiceWatcher({}, QDBusConnection::systemBus(),
                                              QDBusServiceWatcher::WatchForOwnerChange, this);
    connect(m_clientWatcher, &QDBusServiceWatcher::serviceUnregistered,
            this, &LogViewerService::onClientUnregistered);
}

LogViewerService::~LogViewerService()
{
    qCDebug(logService) << "LogViewerService destructor called";
    if(!m_logMap.isEmpty()) {
        qCDebug(logService) << "Cleaning up" << m_logMap.size() << "log map entries";
        for(auto eachPair : m_logMap) {
            delete eachPair.second;
        }
    }

    clearTempFiles();
}

/*!
 * \~chinese \brief LogViewerService::readLog 读取日志文件
 * \~chinese \param fd 文件句柄
 * \~chinese \return 读取的日志
 */
QString LogViewerService::readLog(const QDBusUnixFileDescriptor &fd)
{
    trackCurrentCaller();
    qCDebug(logService) << "Reading log from file descriptor";
    if(!checkAuth(s_Action_View)) {
        qCWarning(logService) << "Authorization check failed for readLog";
        return " ";
    }

    QString log("");
    // fd转文件
    int fdi = fd.fileDescriptor();
    if (fdi <= 0) {
        qCWarning(logService) << "Invalid file descriptor:" << fdi;
        return log;
    }

    QFile file;
    if (!file.open(fdi, QIODevice::ReadOnly | QIODevice::Text)) {
        qCWarning(logService) << "Failed to open file path cache file descriptor.";
        return log;
    }

    QTextStream in(&file);
    QString targetFilePath = in.readAll();

    if (!targetFilePath.isEmpty()) {
        qCDebug(logService) << "Reading log from path:" << targetFilePath;
        log = readLog(targetFilePath);
    } else {
        qCWarning(logService) << "target log file path is empty.";
    }

    file.close();

    return log;
}

QByteArray LogViewerService::processCatFile(const QString &filePath)
{
    qCDebug(logService) << "Processing cat file:" << filePath;
    m_process.start("cat", QStringList() << filePath);
    m_process.waitForFinished(-1);
    QByteArray byte = m_process.readAllStandardOutput();
    qCDebug(logService) << "Cat process completed, read" << byte.size() << "bytes";
    return byte;
}

static QByteArray processCmdWithArgs(const QString &cmdStr, const QStringList &args)
{
    qCDebug(logService) << "Processing command:" << cmdStr << "with arguments:" << args;
    QProcess process;
    process.start(cmdStr, args);
    process.waitForFinished(-1);
    QByteArray outByte = process.readAllStandardOutput();
    return outByte;
}

/*!
 * \~chinese \brief LogViewerService::readLog 读取日志文件
 * \~chinese \param filePath 文件路径
 * \~chinese \return 读取的日志
 */
QString LogViewerService::readLog(const QString &filePath)
{
    qCDebug(logService) << "Reading log from file path:" << filePath;
    if(!checkAuth(s_Action_View)) {
        qCWarning(logService) << "Authorization check failed for readLog";
        return " ";
    }

    // ProtectHome=tmpfs 遮蔽后，后端仅允许读取 /var/log/ 和 /tmp 下的系统日志文件；
    // /home、/root 下的用户日志由前端用户日志访问类在用户进程内本地读取，不再经此后端接口。
    if (!filePath.startsWith("/var/log/") && !filePath.startsWith("/tmp")) {
        qCWarning(logService) << "File path not in whitelist:" << filePath;
        return " ";
    }
    if (filePath.contains("..")) {
        qCWarning(logService) << "File path contains '..':" << filePath;
        return " ";
    }

    QByteArray byte = processCatFile(filePath);

    //QByteArray -> QString 如果遇到0x00，会导致转换终止
    //replace("\x00", "")和replace("\u0000", "")无效
    //使用remove操作，性能损耗过大，因此遇到0x00 替换为 0x20(空格符)
    int replaceTimes = 0;
    for (int i = 0; i != byte.size(); ++i) {
        if (byte.at(i) == 0x00) {
            byte[i] = 0x20;
            replaceTimes++;
        }
    }
    if (replaceTimes > 0) {
        qCDebug(logService) << "Replaced" << replaceTimes << "null bytes in log content";
    }
    return QString::fromUtf8(byte);
}

qint64 LogViewerService::readFileAndReturnIndex(const QString &filePath, qint64 startLine, QList<uint64_t>& lineIndexes, bool reverseOrder) {
    qCDebug(logService) << "Reading file:" << filePath << "and returning index with start line:" << startLine << "and reverse order:" << reverseOrder;
    std::ifstream file(filePath.toStdString());
    if (!file.is_open()) {
        qCDebug(logService) << "File does not exist";
        // 文件不存在，返回错误
        return -1;
    }

    std::string line;
    uint64_t lineNumber = 0;
    uint64_t startIndex = 0;

    // TODO(pengfeixxx): If the index storage takes up too much memory, you can use differential
    // encoding to encode the array where the index is stored, and if it is still large,
    // you can continue to use Huffman encoding for the encoded array.
    if (lineIndexes.empty()) {
        qCDebug(logService) << "Line indexes are empty, reading from file";
        // 从文件开头开始读取
        while (std::getline(file, line)) {
            lineNumber++;
            lineIndexes.push_back(startIndex);
            startIndex = file.tellg();
            if (lineNumber > startLine && !reverseOrder) {
                break;
            }
        }
    } else {
        qCDebug(logService) << "Line indexes are not empty, reading from file";
        if (startLine < lineIndexes.size() && !reverseOrder) {
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

    qCDebug(logService) << "Returning index:" << lineIndexes.at(startLine);
    if (reverseOrder) {
        startLine = lineIndexes.size() - startLine - 1;
        if (startLine < 0)
            return -1;
        else
            return lineIndexes.at(startLine);
    }

    return lineIndexes.at(startLine); // 返回给定起始行的索引
}

QString LogViewerService::getCallerHomeDir()
{
    if (!calledFromDBus()) {
        qWarning() << "getCallerHomeDir: called not from dbus.";
        return QString();
    }

    QString busName = message().service();
    auto reply = connection().interface()->serviceUid(busName);
    if (!reply.isValid()) {
        qCWarning(logService) << "getCallerHomeDir: failed to get caller UID via D-Bus:"
                              << reply.error().message();
        return QString();
    }

    uint callerUid = reply.value();

    // 使用线程安全版本的 getpwuid_r 替代非线程安全的 getpwuid
    struct passwd pwd;
    struct passwd *result = nullptr;
    long bufSize = sysconf(_SC_GETPW_R_SIZE_MAX);
    if (bufSize <= 0 || bufSize > 1024 * 1024) {
        bufSize = 16384;  // 默认缓冲区大小
    }
    QByteArray buf(static_cast<qsizetype>(bufSize), '\0');

    int ret = getpwuid_r(static_cast<uid_t>(callerUid), &pwd, buf.data(), buf.size(), &result);
    if (ret == 0 && result) {
        return QString::fromLocal8Bit(pwd.pw_dir);
    }

    qCCritical(logService) << "getCallerHomeDir: failed to resolve uid:" << callerUid << "error:" << ret;
    return QString();
}

// 基于 fd 的 TOCTOU 安全方式删除 exportOpsLog 产生的 /var/log 临时目录（opsDir）。
// 由 exportOpsLog 在写入 fd 完成后自动调用，确保 /var/log 下不残留含系统日志的目录。
bool LogViewerService::removeOpsTempDirByPathInternal(const QString &path)
{
    // 防御性前缀校验：路径由本服务创建，删除前再确认前缀。
    if (!path.startsWith("/var/log/deepin-log-viewer-ops-log.")) {
        qCWarning(logService) << "removeOpsTempDirByPathInternal: path not under expected prefix, refuse:" << path;
        return false;
    }

    // 先打开可信父目录 /var/log 的 fd，再以 basename 进行 fd-relative 操作，
    // 目标路径自始至终不再被重新解析，真正消除“检查-使用”竞态。
    const QByteArray pathBytes = QFile::encodeName(path);
    const int slash = pathBytes.lastIndexOf('/');
    if (slash <= 0) {
        qCWarning(logService) << "removeOpsTempDirByPathInternal: invalid path (no parent dir):" << path;
        return false;
    }
    const QByteArray parentPath = pathBytes.left(slash);      // /var/log
    const QByteArray baseName = pathBytes.mid(slash + 1);     // deepin-log-viewer-ops-log.XXX

    const int parentFd = open(parentPath.constData(), O_RDONLY | O_DIRECTORY | O_CLOEXEC);
    if (parentFd < 0) {
        qCWarning(logService) << "removeOpsTempDirByPathInternal: failed to open parent dir:" << parentPath << "errno:" << errno;
        return false;
    }
    const bool removed = safeRemoveDirRecursive(parentFd, baseName.constData());
    close(parentFd);

    if (!removed) {
        qCWarning(logService) << "removeOpsTempDirByPathInternal: failed to remove safely:" << path;
        return false;
    }

    qCDebug(logService) << "removeOpsTempDirByPathInternal: removed:" << path;
    return true;
}

// 基于 fd 的 TOCTOU 安全递归删除目录。
// 全程相对父目录 fd 操作（openat/fstatat/unlinkat），不重新解析路径，消除「检查-使用」竞态。
// - O_NOFOLLOW：若 name 是符号链接则 openat 直接失败，不跟随目标；
// - O_DIRECTORY：仅当为目录时打开；
// - fstatat(..., AT_SYMLINK_NOFOLLOW) 取条目自身 stat，符号链接为 S_ISLNK（非 S_ISDIR），
//   走 unlinkat 删链接本身而非跟随目标。
bool LogViewerService::safeRemoveDirRecursive(int parentFd, const char *name)
{
    int fd = openat(parentFd, name, O_RDONLY | O_NOFOLLOW | O_DIRECTORY | O_CLOEXEC);
    if (fd < 0) {
        // 非目录或符号链接：作为文件/链接直接 unlink（不跟随目标）。
        return unlinkat(parentFd, name, 0) == 0;
    }

    DIR *dir = fdopendir(fd);
    if (!dir) {
        close(fd);
        return false;
    }

    // 先收集所有条目名，再处理，避免在迭代过程中修改目录导致遗漏。
    QVector<QByteArray> entries;
    struct dirent *entry = nullptr;
    while ((entry = readdir(dir)) != nullptr) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;
        entries.append(QByteArray(entry->d_name));
    }
    // 此时不再调用 readdir，可安全修改目录。

    bool ok = true;
    for (const QByteArray &childName : entries) {
        struct stat st;
        if (fstatat(fd, childName.constData(), &st, AT_SYMLINK_NOFOLLOW) != 0)
            continue;  // 条目已消失，跳过

        if (S_ISDIR(st.st_mode)) {
            if (!safeRemoveDirRecursive(fd, childName.constData()))
                ok = false;
        } else {
            // 普通文件或符号链接——unlink 链接本身，不跟随。
            if (unlinkat(fd, childName.constData(), 0) != 0)
                ok = false;
        }
    }

    closedir(dir);  // 同时关闭 fd
    // 目录已清空，移除目录项本身。
    if (unlinkat(parentFd, name, AT_REMOVEDIR) != 0)
        ok = false;
    return ok;
}

/**
   @brief Polkit action authorization check.
        Use com.deepin.pkexec.logViewerAuth.policy config file.
        Default action id: "com.deepin.pkexec.logViewerAuth"
   @note Available on linux/unix/macos platform.
   @return check passed.
 */
bool LogViewerService::checkAuthorization(const QString &actionId)
{
    QString appBusName = message().service();
    qCDebug(logService) << "Checking authorization for action:" << actionId << "and app bus name:" << appBusName;
#if defined (Q_OS_LINUX) || defined (Q_OS_UNIX) ||  defined (Q_OS_MAC)
    PolkitQt1::Authority::Result ret = PolkitQt1::Authority::instance()->checkAuthorizationSync(
        actionId, PolkitQt1::SystemBusNameSubject(appBusName), PolkitQt1::Authority::AllowUserInteraction);
    if (PolkitQt1::Authority::Yes == ret) {
        qCDebug(logService) << "Authorization check passed";
        return true;
    } else {
        qCDebug(logService) << "Authorization check failed";
        qCWarning(logService) << qPrintable("Policy authorization check failed!");
        return false;
    }
#else
    qCDebug(logService) << "Authorization check passed in non-linux/unix/macos platform";
    return true;
#endif
}

/*!
 * \~chinese \brief LogViewerService::readLogLinesInRange 分段读取日志文件
 * \~chinese \param fd 文件句柄
 * \~chinese \return 读取的日志
 */
QStringList LogViewerService::readLogLinesInRange(const QDBusUnixFileDescriptor &fd, qint64 startLine, qint64 lineCount, bool bReverse)
{
    trackCurrentCaller();
    qCDebug(logService) << "Reading log lines in range with file descriptor, start line:" << startLine << "line count:" << lineCount << "reverse order:" << bReverse;
    if(!checkAuth(s_Action_View)) {
        qCDebug(logService) << "Authorization check failed for readLogLinesInRange";
        return QStringList();
    }

    QStringList lines;
    // fd转文件
    int fdi = fd.fileDescriptor();
    if (fdi <= 0) {
        qCDebug(logService) << "Invalid file descriptor:" << fdi;
        return lines;
    }

    QFile file;
    if (!file.open(fdi, QIODevice::ReadOnly | QIODevice::Text)) {
        qCWarning(logService) << "Failed to open file path cache file descriptor.";
        return lines;
    }

    QTextStream in(&file);
    QString targetFilePath = in.readAll();

    if (!targetFilePath.isEmpty())
        lines = readLogLinesInRange(targetFilePath, startLine, lineCount, bReverse);
    else
        qCWarning(logService) << "target log file path is empty.";

    file.close();

    return lines;
}

QStringList LogViewerService::readLogLinesInRange(const QString &filePath, qint64 startLine, qint64 lineCount, bool bReverse)
{
    qCDebug(logService) << "Reading log lines in range with file path, start line:" << startLine << "line count:" << lineCount << "reverse order:" << bReverse;
    QStringList lines;

    // 开启鉴权
    if (!checkAuth(s_Action_View))
        return lines;

    // ProtectHome=tmpfs 遮蔽后，后端仅允许读取 /var/log/ 和 /tmp 下的系统日志文件；
    // /home、/root 下的用户日志由前端用户日志访问类在用户进程内本地读取，不再经此后端接口。
    if (!filePath.startsWith("/var/log/") && !filePath.startsWith("/tmp")) {
        qCDebug(logService) << "File path not in whitelist for readLogLinesInRange:" << filePath;
        return lines;
    }
    if (filePath.contains("..")) {
        qCDebug(logService) << "File path contains '..':" << filePath;
        return lines;
    }

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qCDebug(logService) << "Failed to open file for readLogLinesInRange:" << filePath;
        QString token = QCryptographicHash::hash(filePath.toUtf8(), QCryptographicHash::Md5).toHex();
        if (m_logLineIndex.contains(token))
            m_logLineIndex.remove(token);
        return lines;
    }

    qint64 startLineIndex = 0;
    QString token = QCryptographicHash::hash(filePath.toUtf8(), QCryptographicHash::Md5).toHex();
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

    if (startLineIndex < 0)
        return lines;

    if (!file.seek(m_logLineIndex[token].at(startLineIndex))) {
        qCDebug(logService) << "Failed to seek file for readLogLinesInRange:" << filePath;
        file.close();
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

    file.close();

    return lines;
}

qint64 LogViewerService::findLineStartOffsetWithCaching(const QString &filePath, qint64 targetLine) {
    qCDebug(logService) << "Finding line start offset with caching for file:" << filePath << "and target line:" << targetLine;

    const int blockSize = 4096; // 设置块大小，可以根据实际情况调整
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qCDebug(logService) << "Failed to open file for findLineStartOffsetWithCaching:" << filePath;
        return -1; // 无法打开文件
    }

    QByteArray block;
    qint64 offset = 0;
    qint64 currentLine = 0;
    char *blockData = nullptr;
    int blockPosition = 0;
    int lineLength = 0;

    while (currentLine <= targetLine) {
        block = file.read(blockSize);
        if (block.isEmpty()) {
            qCDebug(logService) << "Reached file end while reading block";
            break; // 到达文件末尾
        }

        blockData = block.data();
        blockPosition = 0;
        while (blockPosition < block.size()) {
            char c = blockData[blockPosition++];
            lineLength++;
            if (c == '\n') { // 检测到换行符，增加行数
                if (currentLine == targetLine) {
                    // 如果找到目标行，返回当前偏移量减去换行符的字节数
                    return offset + blockPosition - lineLength;
                }
                currentLine++;
                lineLength = 0;
            }
        }

        offset += block.size(); // 更新总偏移量
    }

    // 处理目标行为文本最后一行的情况
    if (currentLine >= targetLine) {
        if (offset > 0 && lineLength > 0)
            return offset - lineLength;
    }

    file.close();

    return -1; // 没有找到目标行
}

qint64 LogViewerService::getLineCount(const QString &filePath)
{
    trackCurrentCaller();
    qCDebug(logService) << "Getting line count for file:" << filePath;
    if (!checkAuth(s_Action_View)) {
        return -1;
    }

    // ProtectHome=tmpfs 遮蔽后，后端仅允许读取 /var/log/ 和 /tmp 下的系统日志文件。
    if (!filePath.startsWith("/var/log/") && !filePath.startsWith("/tmp")) {
        qCWarning(logService) << "File path not in whitelist for getLineCount:" << filePath;
        return -1;
    }
    if (filePath.contains("..")) {
        qCWarning(logService) << "File path contains '..':" << filePath;
        return -1;
    }

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qCWarning(logService) << "Failed to open file for line count:" << filePath;
        // 文件打开失败处理
        return -1;
    }

    qint64 lineCount = -1;
    QString result = processCmdWithArgs("wc", QStringList() << "-l" << filePath);
    QStringList splitResult = result.split(' ');
    if (splitResult.size() > 0) {
        lineCount = splitResult.first().toLongLong();
        qCDebug(logService) << "Line count result:" << lineCount;
    } else {
        qCWarning(logService) << "Failed to parse line count result:" << result;
    }

    return lineCount;
}

void LogViewerService::processCmdArgs(const QString &cmdStr, const QStringList &args)
{
    m_process.start(cmdStr, args);
}

QString LogViewerService::executeCmd(const QString &cmd)
{
    trackCurrentCaller();
    qCDebug(logService) << "Executing command:" << cmd;
    QString result("");

    if (!checkAuth(s_Action_View)) {
        return result;
    }

    QString cmdStr;
    QStringList args;
    if (cmd.startsWith("coredumpctl-list")) {
        // 通过后端服务，读取系统下所有账户的崩溃日志信息
        cmdStr = "coredumpctl";
        args << "list" << "--no-pager";
    } else if (cmd.startsWith("coredumpctl info")) {
        // 通过后端服务，按进程号获取崩溃信息
        cmdStr = "coredumpctl";
        args = cmd.mid(QString("coredumpctl").size() + 1).split(' ');
    } else if (cmd.startsWith("read-coredump-maps")) {
        // 合并原 dump+readelf 两步：在后端私有命名空间内完成 coredumpctl dump →
        // readelf -n → 自动清理，路径不再经 D-Bus 传递，PrivateTmp 下依然有效。
        const QString pid = cmd.mid(QString("read-coredump-maps").size()).trimmed();
        bool pidOk = false;
        pid.toInt(&pidOk);
        if (!pidOk || pid.isEmpty()) {
            qCWarning(logService) << "read-coredump-maps: invalid pid:" << pid;
            return result;
        }
        return extractCoredumpMaps(pid);
    }

    if (!cmdStr.isEmpty()) {
        qCDebug(logService) << "Executing command:" << cmdStr << "with args:" << args;
        processCmdArgs(cmdStr, args);

        if (!m_process.waitForFinished(-1)) {
            qCWarning(logService()) << "invalid command:" << QString("%1 %2").arg(cmdStr).arg(args.join(' '));
            return "";
        }

        if (cmd == "coredumpctl-list-count") {
            result = m_process.readAllStandardOutput();
            QStringList rows = result.split('\n');
            int nCnt = rows.size();
            // 去掉表头和表尾行
            if (nCnt > 2)
                nCnt -= 2;
            else
                nCnt = 0;
            result = QString::number(nCnt);
        } else {
            result = m_process.readAllStandardOutput();
        }
    }

    return result;
}

/*!
 * \~chinese \brief LogViewerService::extractCoredumpMaps 在后端私有命名空间内完成
 *            coredumpctl dump + readelf -n 并截取前 COREDUMP_MAPS_MAX_LINES 行 maps 信息。
 * \~chinese \param pid 崩溃进程号
 * \~chinese \return maps 文本（前 200 行），失败返回空串
 * \note 临时 dump 文件落在后端私有 /tmp（QTemporaryFile，autoRemove），作用域结束自动清理。
 *       原实现由前端拼 /tmp 路径经 D-Bus 传后端，PrivateTmp 下后端不可达该路径；
 *       现改为后端自建临时文件闭环处理，路径不跨进程传递。
 */
QString LogViewerService::extractCoredumpMaps(const QString &pid)
{
    // 仅需单个临时文件，无需目录：open() 以 O_EXCL 独占创建，fileName() 即路径。
    // open 后立即 close，让 coredumpctl dump -o 以 O_TRUNC 覆写该文件。
    QTemporaryFile tmpFile(QDir::tempPath() + "/deepin-log-viewer-core-XXXXXX.dump");
    if (!tmpFile.open()) {
        qCWarning(logService) << "extractCoredumpMaps: failed to create temp file:" << tmpFile.errorString();
        return QString();
    }
    const QString corePath = tmpFile.fileName();
    tmpFile.close();

    // coredumpctl dump: 导出 core dump 到后端私有 /tmp 下的临时文件
    m_process.start("coredumpctl", QStringList() << "dump" << pid << "-o" << corePath);
    if (!m_process.waitForFinished(-1) || m_process.exitCode() != 0) {
        qCWarning(logService) << "extractCoredumpMaps: coredumpctl dump failed for pid:" << pid
                              << "exitCode:" << m_process.exitCode()
                              << "stderr:" << m_process.readAllStandardError();
        return QString();
    }

    // readelf -n: 读取 ELF notes（含 maps 信息）
    m_process.start("readelf", QStringList() << "-n" << corePath);
    if (!m_process.waitForFinished(-1)) {
        qCWarning(logService) << "extractCoredumpMaps: readelf failed for:" << corePath;
        return QString();
    }

    // 因原始 maps 信息过大，仅取前 COREDUMP_MAPS_MAX_LINES 行
    QTextStream in(m_process.readAllStandardOutput());
    QStringList lines;
    while (!in.atEnd()) {
        const QString str = in.readLine();
        if (!str.isEmpty())
            lines.push_back(str);
        if (lines.count() >= COREDUMP_MAPS_MAX_LINES)
            break;
    }

    return lines.join('\n');
}

/*!
 * \~chinese \brief LogViewerService::openLogStream 打开一个日志文件的流式读取通道
 * \~chinese \param filePath 文件路径
 * \~chinese \return 通道token，返回空时表示文件路径无效
 */
QString LogViewerService::openLogStream(const QString &filePath)
{
    trackCurrentCaller();
    qCDebug(logService) << "Opening log stream for file:" << filePath;
    QString result = readLog(filePath);
    if(result == " ") {
        qCWarning(logService) << "Failed to read log file for stream";
        return "";
    }

    // 使用随机 UUID 作为 token，替代可预测的 MD5(filePath)。
    // 旧方案下攻击者只需知道文件路径即可计算 token，无需经过 openLogStream，
    // 进而通过无鉴权的 readLogInStream 窃取其他用户缓存的日志。
    QString token = QUuid::createUuid().toString(QUuid::WithoutBraces);
    qCDebug(logService) << "Generated token for log stream:" << token;

    auto stream = new QTextStream;

    m_logMap[token] = std::make_pair(result, stream);
    stream->setString(&(m_logMap[token].first), QIODevice::ReadOnly);

    return token;
}

/*!
 * \~chinese \brief LogViewerService::readLogInStream 从刚刚打开的传输通道中读取日志数据
 * \~chinese \param token 通道token
 * \~chinese \return 读取的日志，返回为空的时候表示读取结束或token无效
 */
QString LogViewerService::readLogInStream(const QString &token)
{
    trackCurrentCaller();
    qCDebug(logService) << "Reading log in stream with token:" << token;
    if (!checkAuth(s_Action_View)) {
        qCWarning(logService) << "Authorization check failed for readLogInStream";
        return "";
    }

    if(!m_logMap.contains(token)) {
        qCWarning(logService) << "Token not found in log map:" << token;
        return "";
    }

    auto stream = m_logMap[token].second;

    QString result;
    constexpr int maxReadSize = 10 * 1024 * 1024;
    int linesRead = 0;
    while (1) {
        auto data = stream->readLine();
        if(data.isEmpty()) {
            break;
        }

        result += data + '\n';
        linesRead++;

        if(result.size() > maxReadSize) {
            qCDebug(logService) << "Reached max read size, stopping at" << linesRead << "lines";
            break;
        }
    }

    if(result.isEmpty()) {
        qCDebug(logService) << "Stream finished, cleaning up token:" << token;
        delete m_logMap[token].second;
        m_logMap.remove(token);
    } else {
        qCDebug(logService) << "Read" << linesRead << "lines from stream";
    }

    return result;
}

QString LogViewerService::isFileExist(const QString &filePath)
{
    trackCurrentCaller();
    qCDebug(logService) << "Checking if file exists:" << filePath;
    if (!checkAuth(s_Action_View))
        return QString("");

    if (QFile::exists(filePath))
        return QString("exist");

    return QString("");
}

quint64 LogViewerService::getFileSize(const QString &filePath)
{
    trackCurrentCaller();
    qCDebug(logService) << "Getting file size for:" << filePath;
    if (!checkAuth(s_Action_View)) {
        return 0;
    }

    QFileInfo fi(filePath);
    if (fi.exists())
        return static_cast<quint64>(fi.size());

    return 0;
}


void LogViewerService::clearTempFiles()
{
    qCDebug(logService) << "Clearing temporary files";
    // 清除/tmp目录下lz4.dump文件
    QDir dirTemp(QDir::tempPath());
    dirTemp.setFilter(QDir::Files);
    dirTemp.setNameFilters(QStringList() << "*.lz4.dump");
    QFileInfoList fiList = dirTemp.entryInfoList();
    int removedCount = 0;
    for (auto fi : fiList) {
        if (QFile::remove(fi.absoluteFilePath())) {
            removedCount++;
        }
    }
    qCDebug(logService) << "Removed" << removedCount << "temporary files";
}

/*!
 * \~chinese \brief LogViewerService::exitCode 返回进程状态
 * \~chinese \return 进程返回值
 */
int LogViewerService::exitCode()
{
    trackCurrentCaller();
    if (!checkAuth(s_Action_View)) {
        return -1;
    }
    // qCDebug(logService) << "Getting exit code";
    return m_process.exitCode();
}

/*!
 * \~chinese \brief LogViewerService::quit 退出服务端程序
 *
 * 仅允许 root 调用（systemd/管理员显式停服）。非 root 调用直接返回
 * AccessDenied，不触发 polkit 认证（不弹框），避免“请求退出却提示
 * 访问日志需要认证”的语义错位。普通用户的正常退出由服务端在所有
 * 客户端断开后自动执行（见 onClientUnregistered）。
 */
void LogViewerService::quit()
{
    qCDebug(logService) << "LogViewService::Quit called";
    trackCurrentCaller();
    if (!calledFromDBus()) {
        return;
    }
    // 显式校验 D-Bus 回复有效性：serviceUid() 失败时 QDBusReply::value() 会静默
    // 返回默认构造值 0，若直接当作 UID 会与 root(0) 混淆，构成 fail-open。此处
    // 对无效回复 fail-closed，拒绝退出并回 AccessDenied，绝不回退为 0。
    auto reply = connection().interface()->serviceUid(message().service());
    if (!reply.isValid()) {
        qCWarning(logService) << "quit denied: failed to get caller UID via D-Bus:"
                              << reply.error().message();
        sendErrorReply(QDBusError::ErrorType::AccessDenied,
                       "quit is only allowed for root");
        return;
    }
    const uint callerUid = reply.value();
    if (callerUid != 0) {
        qCWarning(logService) << "quit denied: non-root caller uid" << callerUid;
        sendErrorReply(QDBusError::ErrorType::AccessDenied, "quit is only allowed for root");
        return;
    }
    QCoreApplication::quit();
}

/*!
 * \~chinese \brief LogViewerService::getFileInfo 获取想到读取日志文件的路径
 * \~chinese \param file 日志文件的类型
 * \~chinese \return 所有日志文件路径列表
 */
QStringList LogViewerService::getFileInfo(const QString &file, bool unzip)
{
    trackCurrentCaller();
    qCDebug(logService) << "Getting file info for:" << file << "and unzip:" << unzip;
    // 判断非法调用
    if(!checkAuth(s_Action_View)) {
        return {};
    }

    if (tmpDir.isValid()) {
        qCDebug(logService) << "Tmp dir is valid";
        m_tmpDirPath = tmpDir.path();
        // 每次解压前移除旧有的文件
        if (unzip) {
            qCDebug(logService) << "Removing old files in tmp dir";
            removeDirFiles(m_tmpDirPath);
        }
    }

    QStringList fileNamePath;
    QString nameFilter;
    QDir dir;
    if (file.contains("deepin", Qt::CaseInsensitive) || file.contains("uos", Qt::CaseInsensitive)) {
        QFileInfo appFileInfo(file);
        QString appDir;
        if (appFileInfo.isFile()) {
            appDir = appFileInfo.absolutePath();
        } else if (appFileInfo.isDir()) {
            appDir = appFileInfo.absoluteFilePath();
        } else {
            qCDebug(logService) << "App file info is not a file or dir";
            return QStringList();
        }

        nameFilter = appDir.mid(appDir.lastIndexOf("/") + 1, appDir.size() - 1);
        dir.setPath(appDir);
        dir.setFilter(QDir::Files | QDir::NoSymLinks); //实现对文件的过滤
        dir.setNameFilters(QStringList() << nameFilter + ".*"); //设置过滤
        dir.setSorting(QDir::Time);

        // 若该路径下未找到日志，则按日志文件名称来检索相关日志文件
        QFileInfoList fileList = dir.entryInfoList();
        if (fileList.size() == 0)
            nameFilter = appFileInfo.completeBaseName();
    } else if (file == "audit"){
        qCDebug(logService) << "Audit file";
        dir.setPath("/var/log/audit");
        nameFilter = file;
    } else if (file == "coredump") {
        qCDebug(logService) << "Coredump file";
        QByteArray outByte = processCmdWithArgs("coredumpctl", QStringList() << "list");
        QStringList strList = QString(outByte.replace('\u0000', "").replace("\x01", "")).split('\n', SKIP_EMPTY_PARTS);

        REG_EXP re("(Storage: )\\S+");
        for (int i = strList.size() - 1; i >= 0; --i) {
            QString str = strList.at(i);
            if (str.trimmed().isEmpty())
                continue;

            QStringList tmpList = str.split(" ", SKIP_EMPTY_PARTS);
            if (tmpList.count() < 10)
                continue;

            QString coreFile = tmpList[8];
            QString pid = tmpList[4];
            QString storagePath = "";
            // 解析coredump文件保存位置
            if (coreFile != "missing") {
                QByteArray outInfoByte = processCmdWithArgs("coredumpctl", QStringList() << "info" << pid);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
                re.indexIn(outInfoByte);
                storagePath = re.cap(0).replace("Storage: ", "");
#else
                QRegularExpressionMatch match = re.match(outInfoByte);
                if (match.hasMatch()) {
                    storagePath = match.captured(0).replace("Storage: ", "");
                }
#endif
            }

            if (!storagePath.isEmpty()) {
                fileNamePath.append(storagePath);
            }
        }

        return fileNamePath;
    } else {
        qCDebug(logService) << "Other file";
        dir.setPath("/var/log");
        nameFilter = file;
    }
    //要判断路径是否存在
    if (!dir.exists()) {
        qCWarning(logService) << "it is not true path";
        return QStringList() << "";
    }

    dir.setFilter(QDir::Files | QDir::NoSymLinks); //实现对文件的过滤
    dir.setNameFilters(QStringList() << nameFilter + ".*"); //设置过滤
    dir.setSorting(QDir::Time);
    QFileInfoList fileList = dir.entryInfoList();
    QString tempFileTemplate = m_tmpDirPath + QDir::separator() + "Log_extract_XXXXXX.txt";

    for (int i = 0; i < fileList.count(); i++) {
        if (QString::compare(fileList[i].suffix(), "gz", Qt::CaseInsensitive) == 0 && unzip) {
            QString unzipFile = unzipToTempFile(fileList[i].absoluteFilePath(), tempFileTemplate);
            if (!unzipFile.isEmpty()) {
                fileNamePath.append(unzipFile);
            }
        }
        else {
            fileNamePath.append(fileList[i].absoluteFilePath());
        }
    }
    return fileNamePath;
}

/*!
 * \~chinese \brief LogViewerService::getOtherFileInfo 获取其他日志文件的路径
 * \~chinese \param file 日志文件的类型
 * \~chinese \return 所有日志文件路径列表
 */
QStringList LogViewerService::getOtherFileInfo(const QString &file, bool unzip)
{
    trackCurrentCaller();
    qCDebug(logService) << "Getting other file info for:" << file << "and unzip:" << unzip;
    // 判断非法调用
    if(!checkAuth(s_Action_View)) {
        return {};
    }

    if (tmpDir.isValid()) {
        qCDebug(logService) << "Tmp dir is valid";
        m_tmpDirPath = tmpDir.path();
        // 每次解压前移除旧有的文件
        if (unzip) {
            qCDebug(logService) << "Removing old files in tmp dir";
            removeDirFiles(m_tmpDirPath);
        }
    }

    QStringList fileNamePath;
    QString nameFilter;
    QDir dir;
    QFileInfo appFileInfo(file);
    QFileInfoList fileList;
    //判断路径是否存在
    if (!appFileInfo.exists()) {
        qCWarning(logService) << QString("path:[%1] it is not true path").arg(file);
        return QStringList();
    }
    //如果是文件
    if (appFileInfo.isFile()) {
        QString appDir = appFileInfo.absolutePath();
        nameFilter = appFileInfo.fileName();
        dir.setPath(appDir);
        dir.setNameFilters(QStringList() << nameFilter + "*"); //设置过滤
    } else if (appFileInfo.isDir()) {
        //如果是目录
        dir.setPath(file);
    }

    dir.setFilter(QDir::Files | QDir::NoSymLinks | QDir::Hidden); //实现对文件的过滤
    dir.setSorting(QDir::Time);
    fileList = dir.entryInfoList();
    QString tempFileTemplate = m_tmpDirPath + QDir::separator() + "Log_extract_XXXXXX.txt";

    for (int i = 0; i < fileList.count(); i++) {
        if (QString::compare(fileList[i].suffix(), "gz", Qt::CaseInsensitive) == 0 && unzip) {
            QString unzipFile = unzipToTempFile(fileList[i].absoluteFilePath(), tempFileTemplate);
            if (!unzipFile.isEmpty()) {
                fileNamePath.append(unzipFile);
            }
        }
        else {
            fileNamePath.append(fileList[i].absoluteFilePath());
        }
    }
    return fileNamePath;
}


// 通过 QProcess 运行命令，将标准输出重定向到父进程持有的 fd。
// 子进程无法直接继承父进程的 fd（Qt 的 closeOpenFiles 会关闭继承的 fd，
// 且 O_CLOEXEC 也不跨 exec），故通过 /proc/<pid>/fd/<N> magic symlink
// 让子进程重新 open 同一文件描述符指向的文件。
// 依赖：父子进程同 uid（ptrace 访问检查要求），服务以 root 运行满足。
static bool runCommandRedirectToFd(const QString &cmdStr, const QStringList &args, int outFd)
{
    const QString outFullPath = QStringLiteral("/proc/%1/fd/%2")
                                    .arg(QCoreApplication::applicationPid())
                                    .arg(outFd);
    QProcess process;
    process.setStandardOutputFile(outFullPath, QIODevice::WriteOnly);
    process.start(cmdStr, args);
    if (!process.waitForFinished(-1)) {
        qCWarning(logService) << "command timed out or failed:" << cmdStr << args;
        process.kill();
        return false;
    }
    if (process.exitCode() != 0) {
        qCWarning(logService) << "command exited with code:" << process.exitCode()
                              << "cmd:" << cmdStr << args;
        return false;
    }
    return true;
}

bool LogViewerService::exportLog(const QDBusUnixFileDescriptor &fd, const QString &in, bool isFile)
{
    trackCurrentCaller();
    qCDebug(logService) << "Exporting log with target fd, input:" << in << "isFile:" << isFile;
    if (!checkAuth(s_Action_View)) {
        qCDebug(logService) << "Invalid authorization for export log";
        return false;
    }

    // 目标文件 fd 由前端打开（用户自己可写的文件），后端只需检查可写即可。
    // fd 即精确授权：前端打开的文件必然是用户有权限写入的，无需白名单或路径校验。
    const int outFd = fd.fileDescriptor();
    if (outFd <= 0) {
        qCWarning(logService) << "exportLog: invalid target fd";
        return false;
    }

    // 必须是普通文件，防止写入设备/套接字等特殊 fd
    struct stat fdSt;
    if (fstat(outFd, &fdSt) != 0 || !S_ISREG(fdSt.st_mode)) {
        qCWarning(logService) << "exportLog: target fd is not a regular file";
        return false;
    }

    if (in.isEmpty()) {
        qCWarning(logService) << "exportLog: empty input";
        return false;
    }

    // 命令白名单分支（非文件模式）
    QString cmdStr;
    QStringList args;

    if (isFile) {
        // 输入路径白名单：仅允许 /var/log/、/tmp、/var/lib/systemd/coredump
        // /home 下的应用日志由前端用户日志访问类本地导出，不再经此后端接口。
        if ((!in.startsWith("/var/log/") && !in.startsWith("/tmp") && !in.startsWith("/var/lib/systemd/coredump"))
                || in.contains("..")) {
            qCWarning(logService) << "Input path not in allowed paths:" << in;
            return false;
        }
        QFileInfo filein(in);
        if (!filein.isFile()) {
            qCWarning(logService) << "in not file:" << in;
            return false;
        }
    } else {
        // JSON 分支：解析 submoduleName 构造 journalctl 命令
        QJsonParseError parseError;
        QJsonDocument document = QJsonDocument::fromJson(in.toUtf8(), &parseError);
        if (parseError.error == QJsonParseError::NoError && document.isObject()) {
            QJsonObject object = document.object();
            QString submoduleName;
            if (object.contains("name"))
                submoduleName = object.value("name").toString();

            QString filter;
            QString execPath;
            if (object.contains("filter"))
                filter = object.value("filter").toString();
            if (object.contains("execPath"))
                execPath = object.value("execPath").toString();

            cmdStr = "journalctl";
            if (!execPath.isEmpty())
                args << QString("_EXE=%1").arg(execPath);
            if (!filter.isEmpty())
                args << QString("CODE_CATEGORY=%1").arg(filter);
            if (execPath.isEmpty() && filter.isEmpty())
                args << QString("SYSLOG_IDENTIFIER=%1").arg(submoduleName);
            args << "-r";
        }

        // 硬编码白名单命令分支
        if (cmdStr.isEmpty()) {
            auto it = m_commands.find(in);
            if (it != m_commands.end()) {
                args = it.value();
                cmdStr = args.takeFirst();
            }
        }

        if (cmdStr.isEmpty()) {
            qCWarning(logService) << "unknown command:" << in;
            return false;
        }
    }

    bool ret = false;
    if (isFile) {
        // 块复制源文件到目标 fd
        QFile sourceFile(in);
        if (!sourceFile.open(QIODevice::ReadOnly)) {
            qCWarning(logService) << "Failed to open source file:" << in;
        } else {
            QFile targetFile;
            if (!targetFile.open(outFd, QIODevice::WriteOnly)) {
                qCWarning(logService) << "Failed to open target fd for writing";
                sourceFile.close();
            } else {
                const qint64 chunkSize = 1024 * 1024;  // 1MB
                QScopedPointer<char> buffer(new char[chunkSize]);
                bool error = false;
                while (!sourceFile.atEnd()) {
                    qint64 bytesRead = sourceFile.read(buffer.data(), chunkSize);
                    if (bytesRead > 0) {
                        if (targetFile.write(buffer.data(), bytesRead) != bytesRead) {
                            qCWarning(logService) << "Failed to write all bytes to target";
                            error = true;
                            break;
                        }
                    } else if (bytesRead < 0) {
                        qCWarning(logService) << "Error reading from source:" << in;
                        error = true;
                        break;
                    }
                }
                targetFile.flush();
                sourceFile.close();
                ret = !error;
            }
        }
    } else {
        // 命令输出重定向到目标 fd（通过 /proc/<pid>/fd/N magic symlink）
        ret = runCommandRedirectToFd(cmdStr, args, outFd);
    }

    // 不对前端打开的 fd 做 fchmod（权限由前端文件决定），也不 close（fd 由调用者管理）。
    return ret;
}

void LogViewerService::trackCurrentCaller()
{
    // 仅 D-Bus 调用才记录；内部调用（calledFromDBus() 为 false）直接 no-op，
    // 因此在私有 helper 等非 D-Bus 入口调用也无副作用。
    if (!calledFromDBus())
        return;
    const QString caller = message().service();
    if (caller.isEmpty() || m_clientBusNames.contains(caller))
        return;
    m_clientBusNames.insert(caller);
    m_clientWatcher->addWatchedService(caller);
}

void LogViewerService::onClientUnregistered(const QString &serviceName)
{
    if (!m_clientBusNames.remove(serviceName))
        return;
    qCInfo(logService) << "Client disconnected:" << serviceName
                       << "remaining:" << m_clientBusNames.size();
    // 仅在“有客户端 → 全部断开”的转换时退出；启动空集不退出。
    if (m_clientBusNames.isEmpty()) {
        qCInfo(logService) << "All clients disconnected, exiting";
        QCoreApplication::quit();
    }
}

bool LogViewerService::checkAuth(const QString &actionId)
{
    qCDebug(logService) << "Checking auth for:" << actionId;
    if (!calledFromDBus()) {
        qCWarning(logService) << "called not from dbus.";
        return false;
    }

    // 显式校验 D-Bus 回复有效性：serviceUid() 失败时 QDBusReply::value() 会静默
    // 返回默认构造值 0，若直接当作 UID 会与 root(0) 混淆，构成 fail-open——
    // 任何 D-Bus 调用者都能借此绕过 Polkit 被当作 root 放行。此处对无效回复
    // fail-closed，拒绝访问并回 Failed，绝不回退为 root。
    auto reply = connection().interface()->serviceUid(message().service());
    if (!reply.isValid()) {
        qCWarning(logService) << "checkAuth denied: failed to get caller UID via D-Bus:"
                              << reply.error().message();
        sendErrorReply(QDBusError::ErrorType::Failed, "failed to get caller UID");
        return false;
    }
    bool isRoot = reply.value() == 0;
    if (isRoot) {
        qCInfo(logService) << "dbus caller is root progress.";
        return  true;
    }

    bool bAuthVaild = false;
    bAuthVaild = checkAuthorization(actionId);
    if (!bAuthVaild) {
        qCWarning(logService) << "checkAuthorization failed.";
        sendErrorReply(QDBusError::ErrorType::Failed, "checkAuthorization failed.");
    }

    return  bAuthVaild;
}

bool LogViewerService::exportOpsLog(const QDBusUnixFileDescriptor &fd)
{
    trackCurrentCaller();
    if(!checkAuth(s_Action_View)) {
        qCWarning(logService) << "Invalid authorization for export log";
        return false;
    }

    QString callerHomeDir = getCallerHomeDir();
    if (callerHomeDir.isEmpty()) {
        qCWarning(logService) << "Failed to get caller home directory for export log";
        return false;
    }

    // 不支持导出 sudo 权限的日志，且导出日志功能主要面向普通用户使用场景，
    // 因此当获取到的用户家目录为根目录时，认为是异常情况，不执行导出操作
    if (callerHomeDir == "/" || callerHomeDir == "/root") {
        qCWarning(logService) << "Invalid caller home directory for export log: " << callerHomeDir;
        return false;
    }

    // 前端创建压缩包目标文件并以写方式打开，将 fd 通过 D-Bus 传入。
    // 后端在 root 权限下于 /var/log 创建随机临时目录收集运维日志，整体压缩后写入该 fd，
    // 随后自行清理临时目录，全程不向调用方暴露 /var/log 路径，前端也无需再调用清理接口。
    int fdi = fd.fileDescriptor();
    if (fdi <= 0) {
        qCWarning(logService) << "exportOpsLog: invalid file descriptor from caller";
        return false;
    }

    QTemporaryDir tmpOpsDir("/var/log/deepin-log-viewer-ops-log.XXXXXX");
    tmpOpsDir.setAutoRemove(false);
    if (!tmpOpsDir.isValid()) {
        qCWarning(logService) << "exportOpsLog: failed to create temporary dir under /var/log:" << tmpOpsDir.errorString();
        return false;
    }

    const QString opsDir = tmpOpsDir.path();
    // 在 opsDir 内创建子目录 log-ops，专用于 OpsLogExport 收集日志。
    // 收集完成后压缩该子目录，压缩包同样落在 opsDir 内，
    // 全程不向 /var/log 暴露压缩包或日志内容，无需调整目录权限。
    const QString logCollectDir = opsDir + QStringLiteral("/log-ops");
    if (!QDir().mkpath(logCollectDir)) {
        qCWarning(logService) << "exportOpsLog: failed to create log collect dir:" << logCollectDir;
        removeOpsTempDirByPathInternal(opsDir);
        return false;
    }

    OpsLogExport ops(logCollectDir.toStdString());
    ops.run();

    // 将收集到的 log-ops 子目录内容整体压缩。压缩包放在 opsDir 内（log-ops.zip），
    // 与被压缩内容同处一个随机目录，不暴露在 /var/log 下，避免被其它用户短暂读取。
    // 直接以最终路径调用 zip 创建新压缩包，无需先创建空文件再删除。
    const QString tmpZipPath = opsDir + QStringLiteral("/log-ops.zip");
    QProcess zipProc;
    zipProc.setWorkingDirectory(logCollectDir);
    zipProc.start(QStringLiteral("zip"), QStringList() << QStringLiteral("-r")
                                                       << tmpZipPath << QStringLiteral("."));
    static constexpr int kZipTimeoutMs = 600000;
    if (!zipProc.waitForFinished(kZipTimeoutMs) || zipProc.exitCode() != 0) {
        qCWarning(logService) << "exportOpsLog: zip failed, exitCode:" << zipProc.exitCode()
        << "stderr:" << zipProc.readAllStandardError();
        zipProc.kill();
        QFile::remove(tmpZipPath);
        removeOpsTempDirByPathInternal(opsDir);
        return false;
    }

    // 将压缩包内容写入前端传入的 fd（内核经 SCM_RIGHTS 复制了句柄，root 写入即落入前端文件）。
    bool writeOk = false;
    {
        QFile zipIn(tmpZipPath);
        if (!zipIn.open(QIODevice::ReadOnly)) {
            qCWarning(logService) << "exportOpsLog: failed to open temp zip for reading:" << tmpZipPath;
        } else {
            QFile fdOut;
            if (!fdOut.open(fdi, QIODevice::WriteOnly)) {
                qCWarning(logService) << "exportOpsLog: failed to open caller fd for writing";
            } else {
                constexpr qint64 bufSize = 1 << 20;  // 1 MiB
                // 缓冲区较大，改用堆分配，避免占用过多线程栈空间；
                // 与本文件 exportLog() 中 1 MiB 缓冲区的处理方式保持一致。
                QScopedPointer<char> buf(new char[bufSize]);
                qint64 n = 0;
                bool error = false;
                while ((n = zipIn.read(buf.data(), bufSize)) > 0) {
                    qint64 written = 0;
                    while (written < n) {
                        qint64 w = fdOut.write(buf.data() + written, n - written);
                        if (w < 0) {
                            error = true;
                            break;
                        }
                        written += w;
                    }
                    if (error)
                        break;
                }
                if (error || n < 0) {
                    qCWarning(logService) << "exportOpsLog: write to caller fd failed";
                } else {
                    fdOut.flush();
                    writeOk = true;
                }
                fdOut.close();  // 关闭后端持有的 fd 句柄，前端 QFile 仍保留自己的句柄
            }
            zipIn.close();
        }
    }

    // 清理压缩包与 /var/log 临时目录，无论写入是否成功都需回收。
    QFile::remove(tmpZipPath);
    removeOpsTempDirByPathInternal(opsDir);

    if (!writeOk) {
        qCWarning(logService) << "exportOpsLog: aborted, failed to write zip to caller fd";
        return false;
    }

    qCDebug(logService) << "exportOpsLog: ops logs zipped and written to caller fd successfully";
    return true;
}
