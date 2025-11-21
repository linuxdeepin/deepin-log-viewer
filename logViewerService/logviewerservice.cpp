// SPDX-FileCopyrightText: 2019 - 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "logviewerservice.h"
#include "opslogexport.h"
#include "qtcompat.h"

#include <pwd.h>
#include <unistd.h>
#include <fstream>

#include <dgiofile.h>
#include <dgiovolume.h>
#include <dgiovolumemanager.h>
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
    m_commands.insert("dmesg", "dmesg -r");
    m_commands.insert("last", "last -x");
    m_commands.insert("journalctl_system", "journalctl -r");
    m_commands.insert("journalctl_boot", "journalctl -b -r");
    m_commands.insert("journalctl_app", "journalctl");

    m_actionId = s_Action_View;
    qCDebug(logService) << "Commands initialized, action ID set to:" << m_actionId;
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

    //增加服务黑名单，只允许通过提权接口读取/var/log下，家目录下和临时目录下的文件
    //部分设备是直接从root账户进入，因此还需要监控/root目录
    if ((!filePath.startsWith("/var/log/") &&
         !filePath.startsWith("/tmp") &&
         !filePath.startsWith("/home") &&
         !filePath.startsWith("/root")) ||
         filePath.contains(".."))  {
        qCWarning(logService) << "File path not in whitelist:" << filePath;
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

    //增加服务黑名单，只允许通过提权接口读取/var/log下，家目录下和临时目录下的文件
    //部分设备是直接从root账户进入，因此还需要监控/root目录
    if ((!filePath.startsWith("/var/log/") &&
         !filePath.startsWith("/tmp") &&
         !filePath.startsWith("/home") &&
         !filePath.startsWith("/root")) ||
         filePath.contains("..")) {
        qCDebug(logService) << "File path not in whitelist for readLogLinesInRange:" << filePath;
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
    qCDebug(logService) << "Getting line count for file:" << filePath;
    if (!checkAuth(s_Action_View)) {
        return -1;
    }

    //增加服务黑名单，只允许通过提权接口读取/var/log下，家目录下和临时目录下的文件
    //部分设备是直接从root账户进入，因此还需要监控/root目录
    if ((!filePath.startsWith("/var/log/") &&
         !filePath.startsWith("/tmp") &&
         !filePath.startsWith("/home") &&
         !filePath.startsWith("/root")) ||
            filePath.contains("..")) {
        qCWarning(logService) << "File path not in whitelist for getLineCount:" << filePath;
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
    qCDebug(logService) << "Executing command:" << cmd;
    QString result("");

    if (!checkAuth(s_Action_Export)) {
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
    } else if (cmd.startsWith("coredumpctl dump")) {
        // 截取对应pid的dump文件到指定目录
        cmdStr = "coredumpctl";
        args = cmd.mid(QString("coredumpctl").size() + 1).split(' ');
    } else if (cmd.startsWith("readelf")) {
        // 获取dump文件偏移地址信息
        cmdStr = "readelf";
        args = cmd.mid(QString("readelf").size() + 1).split(' ');
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
        } else if (cmd.startsWith("readelf")) {
            // 因原始maps信息过大，基本几百KB，埋点平台并不需要全量数据，仅取前200行maps信息即可
            QTextStream in(m_process.readAllStandardOutput());
            QStringList lines;
            QString str;
            while (!in.atEnd()) {
                str  = in.readLine();

                if (!str.isEmpty())
                    lines.push_back(str);
                if (lines.count() >= COREDUMP_MAPS_MAX_LINES)
                    break;
            }

            result = lines.join('\n').toUtf8();
        } else {
            result = m_process.readAllStandardOutput();
        }
    }

    return result;
}

/*!
 * \~chinese \brief LogViewerService::openLogStream 打开一个日志文件的流式读取通道
 * \~chinese \param filePath 文件路径
 * \~chinese \return 通道token，返回空时表示文件路径无效
 */
QString LogViewerService::openLogStream(const QString &filePath)
{
    qCDebug(logService) << "Opening log stream for file:" << filePath;
    QString result = readLog(filePath);
    if(result == " ") {
        qCWarning(logService) << "Failed to read log file for stream";
        return "";
    }

    QString token = QCryptographicHash::hash(filePath.toUtf8(), QCryptographicHash::Md5).toHex();
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
    qCDebug(logService) << "Reading log in stream with token:" << token;
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
    qCDebug(logService) << "Checking if file exists:" << filePath;
    if (!checkAuth(s_Action_View))
        return QString("");

    if (QFile::exists(filePath))
        return QString("exist");

    return QString("");
}

quint64 LogViewerService::getFileSize(const QString &filePath)
{
    qCDebug(logService) << "Getting file size for:" << filePath;
    QFileInfo fi(filePath);
    if (fi.exists())
        return static_cast<quint64>(fi.size());

    return 0;
}

// 获取白名单导出路径
QStringList LogViewerService::whiteListOutPaths()
{
    qCDebug(logService) << "Getting white list out paths";
    QStringList paths;
    // 获取用户家目录
    QStringList homeList = getHomePaths();
    if (!homeList.isEmpty())
        paths << homeList;
    // 获取外设挂载可写路径(包括smb路径)
    paths << getExternalDevPaths();
    // 获取临时目录
    paths.push_back("/tmp");
    return paths;
}

// 获取用户家目录
QStringList LogViewerService::getHomePaths()
{
    qCDebug(logService) << "Getting home paths";
    QStringList homeList;

    if (!calledFromDBus()) {
        return homeList;
    }

    QFileInfoList infoList = QDir("/home").entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
    for (auto info : infoList) {
        if (info.isDir())
            homeList.push_back(info.absoluteFilePath());
    }

    return homeList;
}

// 获取外设挂载路径
QStringList LogViewerService::getExternalDevPaths()
{
    qCDebug(logService) << "Getting external dev paths";
    QStringList devPaths;
    const QList<QExplicitlySharedDataPointer<DGioMount> > mounts = getMounts_safe();
    for (auto mount : mounts) {
        QString uri = mount->getRootFile()->uri();
        QString scheme = QUrl(uri).scheme();

        // sbm路径判断，分为gvfs挂载和cifs挂载两种
        QRegularExpression recifs("^file:///media/(.*)/smbmounts");
        QRegularExpression regvfs("^file:///run/user/(.*)/gvfs|^/root/.gvfs");
        if (recifs.match(uri).hasMatch() || regvfs.match(uri).hasMatch()) {
            QString path = QUrl(uri).toLocalFile();
            QFlags <QFileDevice::Permission> power = QFile::permissions(path);
            if (power.testFlag(QFile::WriteUser))
                devPaths.push_back(path);
        }

        // 外设路径判断
        if ((scheme == "file") ||  //usb device
                (scheme == "gphoto2") ||        //phone photo
                (scheme == "mtp")) {            //android file
            QExplicitlySharedDataPointer<DGioFile> locationFile = mount->getDefaultLocationFile();
            QString path = locationFile->path();
            if (path.startsWith("/media/")) {
                QFlags <QFileDevice::Permission> power = QFile::permissions(path);
                if (power.testFlag(QFile::WriteUser)) {
                    devPaths.push_back(path);
                }
            }
        }
    }

    return devPaths;
}

//可重入版本的getMounts
QList<QExplicitlySharedDataPointer<DGioMount> > LogViewerService::getMounts_safe()
{
    qCDebug(logService) << "Getting mounts safely";
    static QMutex mutex;
    mutex.lock();
    auto result = DGioVolumeManager::getMounts();
    mutex.unlock();
    return result;
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
    // qCDebug(logService) << "Getting exit code";
    return m_process.exitCode();
}

/*!
 * \~chinese \brief LogViewerService::quit 退出服务端程序
 */
void LogViewerService::quit()
{
    qCDebug(logService) << "LogViewService::Quit called";
    QCoreApplication::exit(0);
}

/*!
 * \~chinese \brief LogViewerService::getFileInfo 获取想到读取日志文件的路径
 * \~chinese \param file 日志文件的类型
 * \~chinese \return 所有日志文件路径列表
 */
QStringList LogViewerService::getFileInfo(const QString &file, bool unzip)
{
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

static bool processExportLog(const QString &cmdStr, const QString &outFullPath,const QStringList &args)
{
    qCDebug(logService) << "Processing export log for:" << cmdStr << "and outFullPath:" << outFullPath << "and args:" << args;
    QProcess process;
    if (cmdStr != "cp") {
        process.setStandardOutputFile(outFullPath, QIODevice::WriteOnly);
    }

    process.start(cmdStr, args);
    if (!process.waitForFinished(-1)) {
        qCDebug(logService) << "Failed to wait for process to finish";
        return false;
    }
    return true;
}

bool LogViewerService::exportLog(const QString &outDir, const QString &in, bool isFile)
{
    qCDebug(logService) << "Exporting log to:" << outDir << "with input:" << in << "and isFile:" << isFile;
    if(!checkAuth(s_Action_Export)) { //非法调用
        qCDebug(logService) << "Invalid authorization for export log";
        return false;
    }

    QFileInfo outDirInfo;
    if(!outDir.endsWith("/")) {
        outDirInfo.setFile(outDir + "/");
    } else {
        outDirInfo.setFile(outDir);
    }

    if (!outDirInfo.isDir() || in.isEmpty()) {
        qCDebug(logService) << "Invalid output directory or input";
        return false;
    }

    // 导出路径白名单检查
    QString outPath = outDirInfo.absoluteFilePath();
    QStringList availablePaths = whiteListOutPaths();
    bool bAvailable = false;
    for (auto path : availablePaths) {
        if (outPath.startsWith(path)) {
            bAvailable = true;
            break;
        }
    }
    if (!bAvailable) {
        qCDebug(logService) << "Output path not in whitelist";
        return false;
    }

    QString outFullPath = "";
    QString cmdExec = "";
    if (isFile) {
        //增加服务黑名单，只允许通过提权接口读取/var/log、/var/lib/systemd/coredump下，家目录下和临时目录下的文件
        if ((!in.startsWith("/var/log/") && !in.startsWith("/tmp") && !in.startsWith("/home") && !in.startsWith("/var/lib/systemd/coredump"))
                || in.contains("..")) {
            qCDebug(logService) << "Input path not in allowed paths";
            return false;
        }
        QFileInfo filein(in);
        if (!filein.isFile()) {
            qCWarning(logService) << "in not file:" << in;
            qCDebug(logService) << "Input is not a file";
            return false;
        }

        outFullPath = outDirInfo.absoluteFilePath() + filein.fileName();
        
        // 使用更高效的文件复制方法替代cp命令
        QFile sourceFile(in);
        QFile targetFile(outFullPath);
        
        // 使用块复制提高大文件复制效率
        if (sourceFile.open(QIODevice::ReadOnly) && targetFile.open(QIODevice::WriteOnly)) {
            const qint64 chunkSize = 1024 * 1024; // 1MB chunks
            QScopedPointer<char> buffer(new char[chunkSize]);
            
            while (!sourceFile.atEnd()) {
                qint64 bytesRead = sourceFile.read(buffer.data(), chunkSize);
                if (bytesRead > 0) {
                    if (targetFile.write(buffer.data(), bytesRead) != bytesRead) {
                        qCWarning(logService) << "Failed to write all bytes to target file:" << outFullPath;
                        sourceFile.close();
                        targetFile.close();
                        return false;
                    }
                } else if (bytesRead < 0) {
                    qCWarning(logService) << "Error reading from source file:" << in;
                    sourceFile.close();
                    targetFile.close();
                    return false;
                }
            }
            
            sourceFile.close();
            targetFile.close();
            
            // 设置文件权限
            QFile::setPermissions(outFullPath, QFileDevice::ReadOwner | QFileDevice::WriteOwner | 
                                             QFileDevice::ReadGroup | QFileDevice::WriteGroup |
                                             QFileDevice::ReadOther | QFileDevice::WriteOther);
            
            return true;
        } else {
            qCWarning(logService) << "Failed to open source or target file. Source:" << in << "Target:" << outFullPath;
            return false;
        }
    } else {
        QString cmd;

        // 判断输入是否为json字串
        QJsonParseError parseError;
        QJsonDocument document = QJsonDocument::fromJson(in.toUtf8(), &parseError);
        if (parseError.error == QJsonParseError::NoError) {
            if (document.isObject()) {
                QJsonObject object = document.object();

                QString submoduleName;
                QString filter;
                QString execPath;

                if (object.contains("name"))
                    submoduleName = object.value("name").toString();
                if (object.contains("filter"))
                    filter = object.value("filter").toString();
                if (object.contains("execPath"))
                    execPath = object.value("execPath").toString();

                QString condition;
                if (!execPath.isEmpty())
                    condition += QString(" _EXE=%1").arg(execPath);
                if (!filter.isEmpty())
                    condition += QString(" CODE_CATEGORY=%1").arg(filter);
                if (execPath.isEmpty() && filter.isEmpty())
                    condition += QString(" SYSLOG_IDENTIFIER=%1").arg(submoduleName);

                if (!condition.isEmpty()) {
                    cmd = "journalctl" + condition;
                    cmd += " -r";
                    outFullPath = outDirInfo.absoluteFilePath() + submoduleName + ".log";
                }
            }
        }

        // 判断输入是否为cmd命令
        if (cmd.isEmpty()) {
            auto it = m_commands.find(in);
            if (it != m_commands.end()) {
                cmd = it.value();
                outFullPath = outDirInfo.absoluteFilePath() + in + ".log";
            }
        }

        // 未解析出有效命令，返回
        if (cmd.isEmpty()) {
            qCWarning(logService) << "unknown command:" << in;
            return false;
        }

        cmdExec = cmd;
    }

    QString cmdStr = cmdExec.mid(0, cmdExec.indexOf(' '));
    QStringList args;
    args << cmdExec.mid(cmdExec.indexOf(' ') + 1).split(' ');

    if (cmdStr != "cp") {
        if (!QFile::exists(outFullPath)) {
            qCInfo(logService) << "outFullPath:" << outFullPath << "not exist;";
            QFile file(outFullPath);
        }
    }

    if (!processExportLog(cmdStr,outFullPath, args)) {
        qCWarning(logService) << "command error:" << cmdExec;
        return false;
    }

    //设置文件权限
    QProcess newProcess;
    newProcess.start("chmod", QStringList() << "777" << outFullPath);
    if (!newProcess.waitForFinished()) {
        qCWarning(logService) << QString("chmod 777 %1 failed.").arg(outFullPath);
        return false;
    }
    return true;
}

bool LogViewerService::checkAuth(const QString &actionId)
{
    qCDebug(logService) << "Checking auth for:" << actionId;
    if (!calledFromDBus()) {
        qCWarning(logService) << "called not from dbus.";
        return false;
    }

    bool isRoot = connection().interface()->serviceUid(message().service()).value() == 0;
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

bool LogViewerService::exportOpsLog(const QString &outDir, const QString &homeDir)
{
    qCDebug(logService) << "exportOpsLog for outDir:" << outDir << "and homeDir:" << homeDir;
    if(!checkAuth(s_Action_Export)) {
        qCDebug(logService) << "Invalid authorization for export log";
        return false;
    }

    OpsLogExport ops(outDir.toStdString(), homeDir.toStdString());
    ops.run();

    return true;
}
