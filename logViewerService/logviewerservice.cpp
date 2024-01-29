// SPDX-FileCopyrightText: 2019 - 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "logviewerservice.h"

#include <pwd.h>
#include <unistd.h>

#include <dgiofile.h>
#include <dgiovolume.h>
#include <dgiovolumemanager.h>

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

#ifdef QT_DEBUG
Q_LOGGING_CATEGORY(logService, "org.deepin.log.viewer.service")
#else
Q_LOGGING_CATEGORY(logService, "org.deepin.log.viewer.service", QtInfoMsg)
#endif

LogViewerService::LogViewerService(QObject *parent)
    : QObject(parent)
{
    m_commands.insert("dmesg", "dmesg -r");
    m_commands.insert("last", "last -x");
    m_commands.insert("journalctl_system", "journalctl -r");
    m_commands.insert("journalctl_boot", "journalctl -b -r");
    m_commands.insert("journalctl_app", "journalctl");
}

LogViewerService::~LogViewerService()
{
    if(!m_logMap.isEmpty()) {
        for(auto eachPair : m_logMap) {
            delete eachPair.second;
        }
    }
}

/*!
 * \~chinese \brief LogViewerService::readLog 读取日志文件
 * \~chinese \param filePath 文件路径
 * \~chinese \return 读取的日志
 */
QString LogViewerService::readLog(const QString &filePath)
{
    if(!isValidInvoker()) {
        return " ";
    }

    //增加服务黑名单，只允许通过提权接口读取/var/log下，家目录下和临时目录下的文件
    //部分设备是直接从root账户进入，因此还需要监控/root目录
    if ((!filePath.startsWith("/var/log/") &&
         !filePath.startsWith("/tmp") &&
         !filePath.startsWith("/home") &&
         !filePath.startsWith("/root") &&
         !filePath.startsWith("coredumpctl info") &&
         !filePath.startsWith("coredumpctl dump") &&
         !filePath.startsWith("readelf") &&
         filePath != "coredump") ||
         filePath.contains(".."))  {
        return " ";
    }

    if (filePath == "coredump") {
        // 通过后端服务，读取系统下所有账户的崩溃日志信息
        m_process.start("/bin/bash", QStringList() << "-c" << "coredumpctl list --no-pager");
        m_process.waitForFinished(-1);

        return m_process.readAllStandardOutput();
    } else if (filePath.startsWith("coredumpctl info") && !filePath.contains(";")) {
        // 通过后端服务，按进程号获取崩溃信息
        m_process.start("/bin/bash", QStringList() << "-c" << filePath);
        m_process.waitForFinished(-1);

        return m_process.readAllStandardOutput();
    } else if (filePath.startsWith("coredumpctl dump") && !filePath.contains(";")) {
        // 截取对应pid的dump文件到指定目录
        m_process.start("/bin/bash", QStringList() << "-c" << filePath);
        m_process.waitForFinished(-1);

        return m_process.readAllStandardOutput();
    } else if (filePath.startsWith("readelf") && !filePath.contains(";")) {
        // 获取dump文件偏移地址信息
        m_process.start("/bin/bash", QStringList() << "-c" << filePath);
        m_process.waitForFinished(-1);

        return m_process.readAllStandardOutput();
    } else {
        m_process.start("cat", QStringList() << filePath);
        m_process.waitForFinished(-1);
        QByteArray byte = m_process.readAllStandardOutput();

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
        return QString::fromUtf8(byte);
    }
}

/*!
 * \~chinese \brief LogViewerService::openLogStream 打开一个日志文件的流式读取通道
 * \~chinese \param filePath 文件路径
 * \~chinese \return 通道token，返回空时表示文件路径无效
 */
QString LogViewerService::openLogStream(const QString &filePath)
{
    QString result = readLog(filePath);
    if(result == " ") {
        return "";
    }

    QString token = QCryptographicHash::hash(filePath.toUtf8(), QCryptographicHash::Md5).toHex();

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
    if(!m_logMap.contains(token)) {
        return "";
    }

    auto stream = m_logMap[token].second;

    QString result;
    constexpr int maxReadSize = 10 * 1024 * 1024;
    while (1) {
        auto data = stream->readLine();
        if(data.isEmpty()) {
            break;
        }

        result += data + '\n';

        if(result.size() > maxReadSize) {
            break;
        }
    }

    if(result.isEmpty()) {
        delete m_logMap[token].second;
        m_logMap.remove(token);
    }

    return result;
}

bool LogViewerService::isFileExist(const QString &filePath)
{
    QFile file(filePath);
    return file.exists();
}

quint64 LogViewerService::getFileSize(const QString &filePath)
{
    QFileInfo fi(filePath);
    if (fi.exists())
        return static_cast<quint64>(fi.size());

    return 0;
}

// 获取白名单导出路径
QStringList LogViewerService::whiteListOutPaths()
{
    QStringList paths;
    // 获取当前用户家目录
    QString homePath = getAppUserHomePath();
    if (!homePath.isEmpty())
        paths.push_back(homePath);
    // 获取外设挂载可写路径(包括smb路径)
    paths << getExternalDevPaths();
    // 获取临时目录
    paths.push_back("/tmp");
    return paths;
}

// 获取应用当前登录用户家目录
QString LogViewerService::getAppUserHomePath()
{
    if (!calledFromDBus()) {
        return "";
    }

    uint uid = connection().interface()->serviceUid(message().service()).value();
    struct passwd* pwd = getpwuid(uid);
    QString homePath = "/home/" + QString(pwd->pw_name);
    return homePath;
}

// 获取外设挂载路径
QStringList LogViewerService::getExternalDevPaths()
{
    QStringList devPaths;
    const QList<QExplicitlySharedDataPointer<DGioMount> > mounts = getMounts_safe();
    for (auto mount : mounts) {
        QString uri = mount->getRootFile()->uri();
        QString scheme = QUrl(mount->getRootFile()->uri()).scheme();

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
    static QMutex mutex;
    mutex.lock();
    auto result = DGioVolumeManager::getMounts();
    mutex.unlock();
    return result;
}

/*!
 * \~chinese \brief LogViewerService::exitCode 返回进程状态
 * \~chinese \return 进程返回值
 */
int LogViewerService::exitCode()
{
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
    int fileNum = 0;
    if (tmpDir.isValid()) {
        tmpDirPath = tmpDir.path();
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
        dir.setPath("/var/log/audit");
        nameFilter = file;
    } else if (file == "coredump") {
        QProcess process;
        QStringList args = {"-c", ""};
        args[1].append("coredumpctl list");
        process.start("/bin/bash", args);
        process.waitForFinished(-1);
        QByteArray outByte = process.readAllStandardOutput();
        QStringList strList = QString(outByte.replace('\u0000', "").replace("\x01", "")).split('\n', QString::SkipEmptyParts);

        QRegExp re("(Storage: )\\S+");
        for (int i = strList.size() - 1; i >= 0; --i) {
            QString str = strList.at(i);
            if (str.trimmed().isEmpty())
                continue;

            QStringList tmpList = str.split(" ", QString::SkipEmptyParts);
            if (tmpList.count() < 10)
                continue;

            QString coreFile = tmpList[8];
            QString pid = tmpList[4];
            QString storagePath = "";
            // 解析coredump文件保存位置
            if (coreFile != "missing") {
                args[1] = QString("coredumpctl info %1").arg(pid);
                process.start("/bin/bash", args);
                process.waitForFinished(-1);
                QByteArray outInfoByte = process.readAllStandardOutput();
                re.indexIn(outInfoByte);
                storagePath = re.cap(0).replace("Storage: ", "");
            }

            if (!storagePath.isEmpty()) {
                fileNamePath.append(storagePath);
            }
        }

        return fileNamePath;
    } else {
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
    for (int i = 0; i < fileList.count(); i++) {
        if (QString::compare(fileList[i].suffix(), "gz", Qt::CaseInsensitive) == 0 && unzip) {
            QProcess m_process;

            QString command = "gunzip";
            QStringList args;
            args.append("-c");
            args.append(fileList[i].absoluteFilePath());
            m_process.setStandardOutputFile(tmpDirPath + "/" + QString::number(fileNum) + ".txt");
            m_process.start(command, args);
            m_process.waitForFinished(-1);
            fileNamePath.append(tmpDirPath + "/" + QString::number(fileNum) + ".txt");
            fileNum++;
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
    int fileNum = 0;
    if (tmpDir.isValid()) {
        tmpDirPath = tmpDir.path();
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

    for (int i = 0; i < fileList.count(); i++) {
        if (QString::compare(fileList[i].suffix(), "gz", Qt::CaseInsensitive) == 0 && unzip) {
            QProcess m_process;

            QString command = "gunzip";
            QStringList args;
            args.append("-c");
            args.append(fileList[i].absoluteFilePath());
            m_process.setStandardOutputFile(tmpDirPath + "/" + QString::number(fileNum) + ".txt");
            m_process.start(command, args);
            m_process.waitForFinished(-1);
            fileNamePath.append(tmpDirPath + "/" + QString::number(fileNum) + ".txt");
            fileNum++;
        }
        else {
            fileNamePath.append(fileList[i].absoluteFilePath());
        }
    }
    return fileNamePath;
}

bool LogViewerService::exportLog(const QString &outDir, const QString &in, bool isFile)
{
    if(!isValidInvoker()) { //非法调用
        return false;
    }

    QFileInfo outDirInfo;
    if(!outDir.endsWith("/")) {
        outDirInfo.setFile(outDir + "/");
    } else {
        outDirInfo.setFile(outDir);
    }

    if (!outDirInfo.isDir() || in.isEmpty()) {
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
        return false;
    }

    QString outFullPath = "";
    QStringList arg = {"-c", ""};
    if (isFile) {
        //增加服务黑名单，只允许通过提权接口读取/var/log、/var/lib/systemd/coredump下，家目录下和临时目录下的文件
        if ((!in.startsWith("/var/log/") && !in.startsWith("/tmp") && !in.startsWith("/home") && !in.startsWith("/var/lib/systemd/coredump"))
                || in.contains("..")) {
            return false;
        }
        QFileInfo filein(in);
        if (!filein.isFile()) {
            qCWarning(logService) << "in not file:" << in;
            return false;
        }
        outFullPath = outDirInfo.absoluteFilePath() + filein.fileName();
        //复制文件
        arg[1].append(QString("cp %1 \"%2\";").arg(in, outDirInfo.absoluteFilePath()));
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

        //结果重定向到文件
        arg[1].append(QString("%1 >& \"%2\";").arg(cmd, outFullPath));
    }
    //设置文件权限
    arg[1].append(QString("chmod 777 \"%1\";").arg(outFullPath));
    QProcess process;
    process.start("/bin/bash", arg);
    if (!process.waitForFinished()) {
        qCWarning(logService) << "command error:" << arg;
        return false;
    }
    return true;
}

bool LogViewerService::isValidInvoker()
{
    if (!calledFromDBus()) {
        return false;
    }

    bool valid = false;
    QDBusConnection conn = connection();
    QDBusMessage msg = message();

    //判断是否存在执行路径
    uint pid = conn.interface()->servicePid(msg.service()).value();
    QFileInfo f(QString("/proc/%1/exe").arg(pid));
    if (!f.exists()) {
        valid = false;
    } else {
        valid = true;
    }

    //是否存在于可调用者名单中
    QStringList ValidInvokerExePathList;
    QString invokerPath = f.canonicalFilePath();
    QStringList findPaths;//合法调用者查找目录列表
    findPaths << "/usr/bin";
    ValidInvokerExePathList << QStandardPaths::findExecutable("deepin-log-viewer", findPaths);

    if (valid)
        valid = ValidInvokerExePathList.contains(invokerPath);

    //非法调用
    if (!valid) {
        sendErrorReply(QDBusError::ErrorType::Failed,
                       QString("(pid: %1)[%2] is not allowed to configrate firewall")
                       .arg(pid)
                       .arg((invokerPath)));
        return false;
    }
    return true;
}
