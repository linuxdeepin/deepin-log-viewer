// SPDX-FileCopyrightText: 2019 - 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "utils.h"
#include "logsettings.h"
#include "dbusmanager.h"
#include "dbusproxy/dldbushandler.h"
#include "opslogpaths.h"

#include <math.h>
#include <pwd.h>

#include <QUrl>
#include <QDir>
#include <QFile>
#include <QDebug>
#include <QFileInfo>
#include <QFontInfo>
#include <QMimeType>
#include <QApplication>
#include <QMimeDatabase>
#include <QStandardPaths>
#include <QImageReader>
#include <QPixmap>
#include <QFile>
#include <QFontDatabase>
#include <QProcessEnvironment>
#include <QTime>
#include <QLoggingCategory>
#include <QDBusInterface>

#include <polkit-qt5-1/PolkitQt1/Authority>
using namespace PolkitQt1;

#ifdef QT_DEBUG
Q_LOGGING_CATEGORY(logUtils, "org.deepin.log.viewer.utils")
#else
Q_LOGGING_CATEGORY(logUtils, "org.deepin.log.viewer.utils", QtInfoMsg)
#endif
const QString DCONFIG_APPID = "org.deepin.log.viewer";

QHash<QString, QPixmap> Utils::m_imgCacheHash;
QHash<QString, QString> Utils::m_fontNameCache;
QMap<QString, QStringList> Utils::m_mapAuditType2EventType;
int Utils::specialComType = -1;
// 1.sudo权限运行deepin-log-viewer，QDir::homePath返回的是/root
// 此种情况下，使用freedesktop dbus接口获取当前登录用户家目录，以便能正确导出日志
// 2.systemd服务启动deepin-log-viewer，QDir::homePath返回的是/，因该方式下freedesktop dbus接口获取为空，只能将/root作为homePath
QString Utils::homePath = ((QDir::homePath() != "/root" && QDir::homePath() != "/") ? QDir::homePath() : (QDir::homePath() == "/" ? "/root" : DBusManager::getHomePathByFreeDesktop()));
bool Utils::runInCmd = false;

// 高频重复崩溃记录exe路径名单
//const QString COREDUMP_REPEAT_CONFIG_PATH = "/var/cache/deepin/deepin-log-viewer/repeatCoredumpApp.list";
const QString COREDUMP_REPEAT_CONFIG_PATH = Utils::homePath + "/.cache/deepin/deepin-log-viewer/repeatCoredumpApp.list";
const float COREDUMP_HIGH_REPETITION = 0.8f;
const int COREDUMP_TIME_THRESHOLD = 3;

Utils::Utils(QObject *parent)
    : QObject(parent)
{
}

Utils::~Utils()
{
}

QString Utils::getQssContent(const QString &filePath)
{
    QFile file(filePath);
    QString qss;

    if (file.open(QIODevice::ReadOnly)) {
        qss = file.readAll();
    }

    return qss;
}

QString Utils::getConfigPath()
{
    QDir dir(QDir(Utils::homePath + "/.config")
             .filePath(kOrgNameForDeepinLogViewer));

    return dir.filePath(kAppNameForDeepinLogViewer);
}

QString Utils::getAppDataPath()
{
    QDir dir(QDir(Utils::homePath + "/.local/share")
             .filePath(kOrgNameForDeepinLogViewer));
    return dir.filePath(kAppNameForDeepinLogViewer);
}

bool Utils::isFontMimeType(const QString &filePath)
{
    const QString mimeName = QMimeDatabase().mimeTypeForFile(filePath).name();
    ;

    if (mimeName.startsWith("font/") || mimeName.startsWith("application/x-font")) {
        return true;
    }

    return false;
}

bool Utils::isTextFileType(const QString &filePath)
{
    QMimeDatabase db;
    QMimeType mime = db.mimeTypeForFile(filePath);
    //标准文本文件和空白无后缀文件均计入文本文件范围
    if (mime.inherits("text/plain") || mime.inherits("application/x-zerosize")) {
        return true;
    }
    return false;
}

QString Utils::suffixList()
{
    return QString("Font Files (*.ttf *.ttc *.otf)");
}

QPixmap Utils::renderSVG(const QString &filePath, const QSize &size)
{
    if (m_imgCacheHash.contains(filePath)) {
        return m_imgCacheHash.value(filePath);
    }

    QImageReader reader;
    QPixmap pixmap;

    reader.setFileName(filePath);

    if (reader.canRead()) {
        const qreal ratio = qApp->devicePixelRatio();
        reader.setScaledSize(size * ratio);
        pixmap = QPixmap::fromImage(reader.read());
        pixmap.setDevicePixelRatio(ratio);
    } else {
        pixmap.load(filePath);
    }

    m_imgCacheHash.insert(filePath, pixmap);

    return pixmap;
}

QString Utils::loadFontFamilyFromFiles(const QString &fontFileName)
{
    if (m_fontNameCache.contains(fontFileName)) {
        return m_fontNameCache.value(fontFileName);
    }

    QString fontFamilyName = "";

    QFile fontFile(fontFileName);
    if (!fontFile.open(QIODevice::ReadOnly)) {
        return fontFamilyName;
    }

    int loadedFontID = QFontDatabase::addApplicationFontFromData(fontFile.readAll());
    QStringList loadedFontFamilies = QFontDatabase::applicationFontFamilies(loadedFontID);
    if (!loadedFontFamilies.empty()) {
        fontFamilyName = loadedFontFamilies.at(0);
    }
    fontFile.close();

    m_fontNameCache.insert(fontFileName, fontFamilyName);
    return fontFamilyName;
}

QString Utils::getReplaceColorStr(const char *d)
{
    QByteArray byteChar(d);
    byteChar = replaceEmptyByteArray(byteChar);
    QString d_str = QString(byteChar);
    d_str.replace(QRegExp("\\x1B\\[\\d+(;\\d+){0,2}m"), "");
    d_str.replace(QRegExp("\\002"), "");
    return  d_str;
}

/**
 * @brief Utils::replaceEmptyByteArray 替换从qproccess获取的日志信息的字符中的空字符串 替换 \u0000,不然QByteArray会忽略这以后的内容
 * @param iReplaceStr要替换的字符串
 * @return 替换过的字符串
 */
QByteArray Utils::replaceEmptyByteArray(const QByteArray &iReplaceStr)
{
    QByteArray byteOutput = iReplaceStr;
    //\u0000是空字符，\x01是标题开始，出现于系统日志部分进程进程名称最后一个字符，不替换英文情况下显示错误
    return byteOutput.replace('\u0000', "").replace("\x01", "");
}
/**
 * @brief Utils::isErroCommand 判断qproccess获取日志的返回值是否为报错
 * @param str 要判断的字符串结果
 * @return 见头文件中CommandErrorType的定义
 */
Utils::CommandErrorType Utils::isErroCommand(const QString &str)
{
    if (str.contains("权限") || str.contains("permission", Qt::CaseInsensitive)) {
        return PermissionError;
    }
    if (str.contains("请重试") || str.contains("retry", Qt::CaseInsensitive)) {
        return RetryError;
    }
    return NoError;
}

bool Utils::checkAndDeleteDir(const QString &iFilePath)
{
    QFileInfo tempFileInfo(iFilePath);

    if (tempFileInfo.isDir()) {
        deleteDir(iFilePath);
        return true;
    } else if (tempFileInfo.isFile()) {
        QFile deleteFile(iFilePath);
        return deleteFile.remove();
    }
    return false;
}

bool Utils::deleteDir(const QString &iFilePath)
{
    QDir directory(iFilePath);
    if (!directory.exists()) {
        return true;
    }

    QString srcPath = QDir::toNativeSeparators(iFilePath);
    if (!srcPath.endsWith(QDir::separator()))
        srcPath += QDir::separator();

    QStringList fileNames = directory.entryList(QDir::AllEntries | QDir::NoDotAndDotDot | QDir::Hidden);
    bool error = false;
    for (QStringList::size_type i = 0; i != fileNames.size(); ++i) {
        QString filePath = srcPath + fileNames.at(i);
        QFileInfo fileInfo(filePath);
        if (fileInfo.isFile() || fileInfo.isSymLink()) {
            QFile::setPermissions(filePath, QFile::WriteOwner);
            if (!QFile::remove(filePath)) {
                error = true;
            }
        } else if (fileInfo.isDir()) {
            if (!deleteDir(filePath)) {
                error = true;
            }
        }
    }

    if (!directory.rmdir(QDir::toNativeSeparators(directory.path()))) {
        error = true;
    }

    return !error;
}

void Utils::replaceColorfulFont(QString *iStr)
{
    iStr->replace(QRegExp("[[0-9]{1,2}m"), "");
}

bool Utils::isWayland()
{
    auto e = QProcessEnvironment::systemEnvironment();
    QString XDG_SESSION_TYPE = e.value(QStringLiteral("XDG_SESSION_TYPE"));
    QString WAYLAND_DISPLAY = e.value(QStringLiteral("WAYLAND_DISPLAY"));

    if (XDG_SESSION_TYPE == QLatin1String("wayland") || WAYLAND_DISPLAY.contains(QLatin1String("wayland"), Qt::CaseInsensitive)) {
        return true;
    } else {
        return false;
    }
}

bool Utils::sleep(unsigned int msec)
{
    QTime dieTime = QTime::currentTime().addMSecs(static_cast<int>(msec));

    while (QTime::currentTime() < dieTime) {
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
    }

    return true;
}

QString Utils::mkMutiDir(const QString &path)
{
    QDir dir(path);
    if (path.isEmpty() || dir.exists()) {
        return path;
    }
    QString parentDir = mkMutiDir(path.mid(0, path.lastIndexOf('/')));
    QString dirname = path.mid(path.lastIndexOf('/') + 1);
    QDir parentPath(parentDir);
    if (!dirname.isEmpty()) {
        bool bRet = parentPath.mkpath(dirname);
        if (!bRet)
            qCWarning(logUtils) << QString("mkpath: unable to create directory '%1' in path: '%2/', please confirm if the file exists.").arg(dirname).arg(parentDir);
    }
    return parentDir + "/" + dirname;
}

bool Utils::checkAuthorization(const QString &actionId, qint64 applicationPid)
{
    Authority::Result result;

    result = Authority::instance()->checkAuthorizationSync(actionId, SystemBusNameSubject(QDBusConnection::systemBus().baseService()),
                                                           Authority::AllowUserInteraction);
    return result == Authority::Yes ? true : false;
}

QString Utils::osVersion()
{
    QProcess *unlock = new QProcess;
    unlock->start("lsb_release", {"-r"});
    unlock->waitForFinished();
    auto output = unlock->readAllStandardOutput();
    auto str = QString::fromUtf8(output);
    QRegExp re("\t.+\n");
    QString osVerStr;
    if (re.indexIn(str) > -1) {
        auto result = re.cap(0);
        osVerStr = result.remove(0, 1).remove(result.size() - 1, 1);
    }
    unlock->deleteLater();
    return osVerStr;
}

QString Utils::auditType(const QString &eventType)
{
    QMapIterator<QString, QStringList> it(m_mapAuditType2EventType);
    while (it.hasNext()) {
        it.next();
        if (it.value().indexOf(eventType) != -1)
            return it.key();
    }

    return "";
}

double Utils::convertToMB(quint64 cap, const int size/* = 1024*/)
{
    static QString type[] = {" B", " KB", " MB"};

    double dc = cap;
    double ds = size;

    for (size_t p = 0; p < sizeof(type); ++p) {
        if (cap < pow(size, p + 1) || p == sizeof(type) - 1)
            return dc / pow(ds, p);
    }

    return 0.0;
}

QString Utils::getUserNamebyUID(uint uid)
{
    struct passwd * pwd;
    pwd = getpwuid(uid);
    if (nullptr == pwd) {
        qCWarning(logUtils) << QString("unknown uid:%1").arg(uid);
        return "";
    }

    return pwd->pw_name;
}

QString Utils::getUserHomePathByUID(uint uid)
{
    struct passwd * pwd;
    pwd = getpwuid(uid);
    if (nullptr == pwd) {
        qCWarning(logUtils) << QString("unknown uid:%1").arg(uid);
        return "";
    }

    return pwd->pw_dir;
}

QString Utils::getCurrentUserName()
{
    // 获取当前系统用户名
    struct passwd* pwd = getpwuid(getuid());
    return pwd->pw_name;
}

bool Utils::isValidUserName(const QString &userName)
{
    bool bValidUserName = false;
    QDBusInterface interface("com.deepin.daemon.Accounts", "/com/deepin/daemon/Accounts", "com.deepin.daemon.Accounts", QDBusConnection::systemBus());
    QStringList userList = qvariant_cast< QStringList >(interface.property("UserList"));
    for (auto strUser : userList) {
        uint uid = strUser.mid(strUser.lastIndexOf("User") + 4).toUInt();
        QString tempUserName = getUserNamebyUID(uid);
        if(tempUserName == userName) {
            bValidUserName = true;
            break;
        }
    }

    return bValidUserName;
}

bool Utils::isCoredumpctlExist()
{
    bool isCoredumpctlExist = false;
    QDir dir("/usr/bin");
    QStringList list = dir.entryList(QStringList() << (QString("coredumpctl") + "*"), QDir::NoDotAndDotDot | QDir::Files);
    for (int i = 0; i < list.count(); i++) {
        if("coredumpctl" == list[i]) {
            isCoredumpctlExist = true;
            break;
        }
    }
    return isCoredumpctlExist;
}

QString Utils::appName(const QString &filePath)
{
    QString ret;
    if (filePath.isEmpty())
        return ret;

    QStringList strList = filePath.split("/");
    if (strList.count() < 2) {
        if (filePath.contains("."))
            ret = filePath.section(".", 0, 0);
        else {
            ret = filePath;
        }
        return ret;
    }

    QString tmpPath = filePath;
    if (tmpPath.endsWith('/'))
        tmpPath = tmpPath.remove(tmpPath.size() - 1, 1);
    QString desStr = tmpPath.section("/", -1);
    ret = desStr.mid(0, desStr.lastIndexOf("."));
    return ret;
}

void Utils::resetToNormalAuth(const QString &path)
{
    QFileInfo fi(path);
    if (!path.isEmpty() && fi.exists()) {
        qInfo() << "resetToNormalAuth: " << path;
        QString tmpPath = path;
        if (fi.isDir())
            tmpPath = path;
        else
            tmpPath = fi.absolutePath();

        executeCmd("chmod", QStringList() << "-R" << "777" << tmpPath);
    }
}

QList<LOG_REPEAT_COREDUMP_INFO> Utils::countRepeatCoredumps(qint64 timeBegin, qint64 timeEnd)
{
    QList<LOG_REPEAT_COREDUMP_INFO> result;
    QString data = DLDBusHandler::instance()->executeCmd("coredumpctl-list");
    QStringList strList = data.split('\n', QString::SkipEmptyParts);
    int total = 0;
    for (int i = strList.size() - 1; i >= 0; --i) {
        QString str = strList.at(i);
        if (str.trimmed().isEmpty())
            continue;
        QStringList tmpList = str.split(" ", QString::SkipEmptyParts);
        if (tmpList.count() < 10)
            continue;

        QDateTime dt = QDateTime::fromString(tmpList[1] + " " + tmpList[2], "yyyy-MM-dd hh:mm:ss");
        if (timeBegin > 0 && timeEnd > 0) {
            if (dt.toMSecsSinceEpoch() < timeBegin || dt.toMSecsSinceEpoch() > timeEnd)
                continue;
        }

        total++;
        QString exePath = tmpList[9];

        auto it = std::find_if(result.begin(), result.end(), [&exePath](const LOG_REPEAT_COREDUMP_INFO& item){
            return exePath == item.exePath;
        });
        if (it == result.end()) {
            LOG_REPEAT_COREDUMP_INFO info;
            info.exePath = exePath;
            info.times = 1;
            info.repetitionRate = 0.0;
            result.push_back(info);
        } else {
            it->times++;
        }
    }

    // 计算重复率
    for (auto &info : result) {
        if (info.times > 1) {
            info.repetitionRate = static_cast<float>(info.times) / total;
        }
    }

    return result;
}

QStringList Utils::getRepeatCoredumpExePaths()
{
    QFile file(COREDUMP_REPEAT_CONFIG_PATH);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return QStringList();

    QString data = file.readAll();

    file.close();

    return data.split(' ');
}

void Utils::updateRepeatCoredumpExePaths(const QList<LOG_REPEAT_COREDUMP_INFO> &infos)
{
    // 每隔24小时强制清空一次名单内容，以便重复的崩溃exe路径是最新的
    QFileInfo fi(COREDUMP_REPEAT_CONFIG_PATH);
    if (fi.birthTime().daysTo(QDateTime::currentDateTime()) >= 1) {
        if (QFile::exists(COREDUMP_REPEAT_CONFIG_PATH))
                QFile::remove(COREDUMP_REPEAT_CONFIG_PATH);
    }

    QDateTime bt = fi.birthTime();
    // 目录不存在，则创建目录
    if (!QFileInfo::exists(fi.absolutePath())) {
        QDir dir;
        dir.mkpath(fi.absolutePath());
    }

    // 确定当前时间范围的崩溃数据高频重复路径
    QStringList newRepatExePaths;
    for (auto info : infos) {
        if (info.repetitionRate > COREDUMP_HIGH_REPETITION || info.times >= COREDUMP_TIME_THRESHOLD)
            newRepatExePaths.push_back(info.exePath);
    }

    if (newRepatExePaths.size() == 0)
        return;

    QStringList curReaptExePaths = getRepeatCoredumpExePaths();
    for (auto exePath : newRepatExePaths) {
        if (curReaptExePaths.indexOf(exePath) == -1)
            curReaptExePaths.push_back(exePath);
    }

    QFile file(COREDUMP_REPEAT_CONFIG_PATH);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qCWarning(logUtils) << "failed to open coredump repeat config file:" << COREDUMP_REPEAT_CONFIG_PATH;
        return;
    }

    QTextStream out(&file);
    out << curReaptExePaths.join(' ');

    file.close();
}

// 查找所有物理网络接口
QStringList Utils::getPhysicalInterfaces()
{
    QStringList physicalInterfaces;
    QDir netDir("/sys/class/net/");

    if (!netDir.exists()) {
        qWarning("Directory /sys/class/net/ does not exist. This program is for Linux only.");
        return physicalInterfaces;
    }

    // 设置过滤器，只列出目录
    netDir.setFilter(QDir::Dirs | QDir::NoDotAndDotDot);
    QStringList interfaceList = netDir.entryList();

    for (const QString &iface : interfaceList) {
        // 物理网卡在 /sys/class/net/ 下会有一个指向 device 的符号链接或目录
        QFileInfo deviceInfo(netDir.filePath(iface + "/device"));
        if (deviceInfo.exists() && deviceInfo.isSymLink()) {
            physicalInterfaces.append(iface);
        }
    }

    return physicalInterfaces;
}

QStringList Utils::expandPathWithWildcardIterator(const QString &pathWithWildcard)
{
    // 1. 分离目录和文件名模式
    QFileInfo fileInfo(pathWithWildcard);
    QString dirPath = fileInfo.path();
    QString fileNamePattern = fileInfo.fileName();

    QStringList matchedFiles;
    QStringList nameFilters;
    nameFilters << fileNamePattern;

    // 2. 创建 QDirIterator
    // 构造函数参数：目录, 名称过滤器, 实体类型过滤器, 迭代器标志
    QDirIterator it(dirPath, nameFilters, QDir::Files | QDir::NoDotAndDotDot, QDirIterator::NoIteratorFlags);

    // 3. 循环遍历所有匹配项
    while (it.hasNext()) {
        // it.next() 直接返回下一个匹配项的完整路径
        matchedFiles.append(it.next());
    }

    return matchedFiles;
}

void Utils::appendToFile(const QString &filePath, const QByteArray &content)
{
    QFile file(filePath);

    if (file.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
        file.write(content);
    } else {
        qWarning() << "Error opening file for writing:" << file.errorString();
    }
}

QByteArray Utils::processCmdWithArgs(const QString &cmdStr, const QString &workPath, const QStringList &args)
{
    QProcess process;
    if (!workPath.isEmpty())
        process.setWorkingDirectory(workPath);

    process.setProgram(cmdStr);
    process.setArguments(args);
    process.setEnvironment({"LANG=en_US.UTF-8", "LANGUAGE=en_US"});
    process.start();
    // 设定超时上限，防止因特殊文件（如 /dev/zero、无写端的 FIFO）或异常子进程导致
    // 无限阻塞。超时后主动 kill 并回收，避免僵尸进程与导出线程被永久卡死（拒绝服务）。
    static constexpr int kCmdTimeoutMs = 300000;
    if (!process.waitForFinished(kCmdTimeoutMs)) {
        qWarning() << "cmd timed out after" << kCmdTimeoutMs << "ms, killing:" << cmdStr << args;
        process.kill();
        process.waitForFinished(3000);
        return QByteArray();
    }
    QByteArray outPut = process.readAllStandardOutput();
    int nExitCode = process.exitCode();
    bool bRet = (process.exitStatus() == QProcess::NormalExit && nExitCode == 0);
    if (!bRet) {
        qWarning() << "run cmd error, caused by:" << process.errorString() << "output:" << outPut;
        return QByteArray();
    }
    return outPut;
}

QByteArray Utils::executeCmd(const QString &cmdStr, const QStringList &args, const QString &workPath)
{
    return processCmdWithArgs(cmdStr, workPath,  args);
}

// 安全拷贝辅助：对齐 root 侧 opslogexport.cpp 的符号链接防护策略。
// 前端以普通用户权限收集家目录日志时，恶意用户可在对应路径下预埋指向
// /dev/zero 或命名管道（FIFO）的符号链接。默认 cp 会跟随符号链接读取目标，
// 叠加 processCmdWithArgs 的超时机制前曾为无限等待，可永久阻塞导出线程（拒绝服务）。
// 防护两层：
//   1. 拷贝前用 QFileInfo::isSymLink() 过滤源路径，符号链接源直接跳过；
//   2. 强制传入 -P，使递归拷贝目录时也不跟随树内符号链接（复制链接本身）。
// sources 为源路径列表，dest 为目标路径，recursive 控制是否递归拷贝目录。
static void safeCpSkipSymlinks(const QStringList &sources, const QString &dest, bool recursive)
{
    QStringList args;
    args << "-P";  // 不跟随源参数自身及递归树内的符号链接
    if (recursive)
        args << "-r";
    for (const QString &src : sources) {
        if (QFileInfo(src).isSymLink()) {
            qCWarning(logUtils) << "skip symlink source to avoid DoS:" << src;
            continue;
        }
        args << src;
    }
    args << dest;
    Utils::executeCmd("cp", args);
}

void Utils::exportUserPermissionAppLogs(const QString &outDir, const QString &userHomeDir)
{
    // 安全中心
    safeCpSkipSymlinks({userHomeDir + "/.cache/deepin/deepin-defender/deepin-defender.log",
                        userHomeDir + "/.cache/deepin/deepin-defender-daemon/deepin-defender-daemon.log",
                        userHomeDir + "/.cache/deepin/deepin-defender-datainterface/deepin-defender-datainterface.log"},
                       outDir + kDefenderPath, false);

    // 云打印
    safeCpSkipSymlinks({userHomeDir + "/.cache/uniontech/deepin-cloud-print/deepin-cloud-print.log",
                        userHomeDir + "/.cache/uniontech/deepin-cloud-print-configurator/deepin-cloud-print-configurator.log"},
                       outDir + kCloudPrintPath, false);

    // 云扫描
    safeCpSkipSymlinks({userHomeDir + "/.cache/deepin/deepin-cloud-scan/deepin-cloud-scan.log"},
                       outDir + kCloudScanPath, false);

    // 打印管理器
    safeCpSkipSymlinks({userHomeDir + "/.cache/deepin/dde-printer/dde-printer.log"},
                       outDir + kPrinterPath, false);

    // 显卡驱动管理器
    safeCpSkipSymlinks({userHomeDir + "/.cache/deepin/deepin-graphics-driver-manager/deepin-graphics-driver-manager.log"},
                       outDir + kGraphicsDriverManagerPath, false);

    // 启动盘制作工具
    safeCpSkipSymlinks({userHomeDir + "/.cache/deepin/deepin-boot-maker/deepin-boot-maker.log"},
                       outDir + kBootMakerPath, false);

    // 扫描管理
    safeCpSkipSymlinks({userHomeDir + "/.cache/deepin/org.deepin.scanner/org.deepin.scanner"},
                       outDir + kScanerPath, true);

    // KMS项目
    safeCpSkipSymlinks({userHomeDir + "/.cache/deepin/kmsclient",
                        userHomeDir + "/.cache/deepin/kmstools"},
                       outDir + kKMSPath, true);

    // 归档管理器
    safeCpSkipSymlinks({userHomeDir + "/.cache/deepin/deepin-compressor/deepin-compressor.log"},
                       outDir + kCompressorPath, false);

    // 日历
    safeCpSkipSymlinks({userHomeDir + "/.cache/deepin/dde-calendar-service/dde-calendar-service.log",
                        userHomeDir + "/.cache/deepin/dde-calendar/dde-calendar.log"},
                       outDir + kCalendarPath, false);

    // 帮助手册
    safeCpSkipSymlinks({userHomeDir + "/.cache/deepin/deepin-manual/deepin-manual.log"},
                       outDir + kManualPath, false);

    // 文档查看器
    safeCpSkipSymlinks({userHomeDir + "/.cache/deepin/deepin-reader/deepin-reader.log"},
                       outDir + kReaderPath, false);

    // 字体管理器
    safeCpSkipSymlinks({userHomeDir + "/.cache/deepin/deepin-font-manager/deepin-font-manager.log"},
                       outDir + kFontManagerPath, false);

    // 软件包安装器
    safeCpSkipSymlinks({userHomeDir + "/.cache/deepin/deepin-deb-installer/deepin-deb-installer.log"},
                       outDir + kDebInstallerPath, false);

    // 终端
    safeCpSkipSymlinks({userHomeDir + "/.cache/deepin/deepin-terminal/deepin-terminal.log"},
                       outDir + kTerminalPath, false);

    // 语音记事本
    safeCpSkipSymlinks({userHomeDir + "/.cache/deepin/deepin-voice-note/deepin-voice-note.log"},
                       outDir + kVoiceNotPath, false);

    // 设备管理器
    safeCpSkipSymlinks({userHomeDir + "/.cache/deepin/deepin-devicemanager/deepin-devicemanager.log"},
                       outDir + kDevicemanagerPath, false);

    // 服务与支持
    safeCpSkipSymlinks({userHomeDir + "/.cache/deepin/uos-service-support/uos-service-support.log"},
                       outDir + kServiceSupportPath, false);

    // 远程协助
    safeCpSkipSymlinks({userHomeDir + "/.cache/deepin/uos-remote-assistance/uos-remote-assistance.log"},
                       outDir + kRemoteAssistancePath, false);

    // 系统监视器
    safeCpSkipSymlinks({userHomeDir + "/.cache/deepin/deepin-system-monitor/deepin-system-monitor.log"},
                       outDir + kSystemMonitorPath, false);

    // 文本编辑器
    safeCpSkipSymlinks({userHomeDir + "/.cache/deepin/deepin-editor/deepin-editor.log"},
                       outDir + kEditorPath, false);

    // 计算器
    safeCpSkipSymlinks({userHomeDir + "/.cache/deepin/deepin-calculator/deepin-calculator.log"},
                       outDir + kCalculatorPath, false);

    // 邮箱
    safeCpSkipSymlinks({userHomeDir + "/.cache/deepin/deepin-mail/deepin-mail.log"},
                       outDir + kMailPath, false);

    // 截图录屏
    safeCpSkipSymlinks({userHomeDir + "/.cache/deepin/deepin-screen-recorder/deepin-screen-recorder.log"},
                       outDir + kScreenRecorderPath, false);

    // 画板
    safeCpSkipSymlinks({userHomeDir + "/.cache/deepin/deepin-draw/deepin-draw.log"},
                       outDir + kDrawPath, false);

    // 音乐
    safeCpSkipSymlinks({userHomeDir + "/.cache/deepin/deepin-music/deepin-music.log"},
                       outDir + kMusicPath, false);

    // 看图
    safeCpSkipSymlinks({userHomeDir + "/.cache/deepin/deepin-image-viewer/deepin-image-viewer.log"},
                       outDir + kImageViewerPath, false);

    // 相册
    safeCpSkipSymlinks({userHomeDir + "/.cache/deepin/deepin-album/deepin-album.log"},
                       outDir + kAlbumPath, false);

    // 影院
    safeCpSkipSymlinks({userHomeDir + "/.cache/deepin/deepin-movie/deepin-movie.log"},
                       outDir + kMoviePath, false);

    // 相机
    safeCpSkipSymlinks({userHomeDir + "/.cache/deepin/deepin-camera/deepin-camera.log"},
                       outDir + kCameraPath, false);

    // 中文输入法
    safeCpSkipSymlinks({userHomeDir + "/.cache/org.deepin.chineseime/ime/chineseime-qimpanel.log",
                        userHomeDir + "/.cache/org.deepin.chineseime/ime/fcitx-iflyime.log",
                        userHomeDir + "/.cache/org.deepin.chineseime/ime/ossp.log"},
                       outDir + kChineseImePath, false);

    // 授权管理客户端
    safeCpSkipSymlinks({userHomeDir + "/.cache/uos/uos-activator",
                        userHomeDir + "/.cache/uos/uos-activator-cmd",
                        userHomeDir + "/.cache/uos-agent/uos-license-agent",
                        userHomeDir + "/.cache/uos-agent/uos-activator-kms"},
                       outDir + kUosActivatorPath, true);

    // 输入法配置
    safeCpSkipSymlinks(expandPathWithWildcardIterator("/tmp/fcitx*.log"),
                       outDir + "/app/fcitx/", true);

    // 下载器
    safeCpSkipSymlinks({userHomeDir + "/.config/uos/downloader/Log"},
                       outDir + kDownloaderPath, true);

    // 窗口管理器
    appendToFile(outDir + "/app/kwin/glxinfo.log", executeCmd("glxinfo", { "-display", qEnvironmentVariable("DISPLAY"), "-B" }));
    appendToFile(outDir + "/app/kwin/kwin_info.log", executeCmd("apt", { "policy", "kwin-x11", "dde-kwin" }));

    // 安卓容器
    safeCpSkipSymlinks({userHomeDir + "/log/AospLog.log",
                        userHomeDir + "/log/KboxServer.log"},
                       outDir + kKboxPath, false);

    // 日志收集工具
    safeCpSkipSymlinks({userHomeDir + "/.cache/deepin/deepin-log-viewer/deepin-log-viewer.log"},
                       outDir + kDeepinLogViewerPath, false);
}

void Utils::exportUserPermissionSystemLogs(const QString &outDir, const QString &userHomeDir)
{
    // pulse　audio /home/uos/pulse.log
    safeCpSkipSymlinks({userHomeDir + "/pulse.log"}, outDir + kSystemPulseaudioPath, false);
}

void Utils::exportUserPermissionKernelLogs(const QString &outDir)
{
    // ⽆法识别声卡问题⽇志
    appendToFile(outDir + "/kernel/aplay.log", executeCmd("aplay", { "-l" }));

    for (const QString &iface : getPhysicalInterfaces()) {
        appendToFile(outDir + "/kernel/eth_info.log", executeCmd("ethtool", { "-i", iface }));
    }

    appendToFile(outDir + "/kernel/ifconfig.log", executeCmd("ifconfig", { }));
}

void Utils::exportUserPermissionDDELogs(const QString &outDir, const QString &userHomeDir)
{
    // 文件管理器
    safeCpSkipSymlinks({userHomeDir + "/.cache/deepin/dde-desktop/dde-desktop.log"},
                       outDir + kDdeDesktopPath, false);

    safeCpSkipSymlinks({userHomeDir + "/.cache/deepin/dde-file-manager/dde-file-manager.log"},
                       outDir + kDdeFileManagerPath, false);

    // 任务栏
    safeCpSkipSymlinks({userHomeDir + "/.cache/deepin/dde-dock/dde-dock.log"},
                       outDir + kDdeDockPath, false);

    //DDE
    safeCpSkipSymlinks({userHomeDir + "/Desktop/DDE_LOG.zip"},
                       outDir + kDDEPath, false);
}

void Utils::exportUserPermissionOpsLogs(const QString &outDir, const QString &userHomeDir)
{
    Utils::exportUserPermissionAppLogs(outDir, userHomeDir);
    Utils::exportUserPermissionSystemLogs(outDir, userHomeDir);
    Utils::exportUserPermissionKernelLogs(outDir);
    Utils::exportUserPermissionDDELogs(outDir, userHomeDir);

    // deb version
    QStringList args;
    args.clear();
    args << "-l";
    appendToFile(outDir + "/deb-version.txt", executeCmd("dpkg", args));
}

#ifdef DTKCORE_CLASS_DConfigFile
LoggerRules::LoggerRules(QObject *parent)
    : QObject(parent), m_rules(""), m_config(nullptr) {
}

LoggerRules::~LoggerRules() { m_config->deleteLater(); }

void LoggerRules::initLoggerRules()
{
    QByteArray logRules = qgetenv("QT_LOGGING_RULES");
    qunsetenv("QT_LOGGING_RULES");

    // set env
    m_rules = logRules;

    // set dconfig
    m_config = Dtk::Core::DConfig::create(DCONFIG_APPID, DCONFIG_APPID);
    logRules = m_config->value("log_rules").toByteArray();
    appendRules(logRules);
    setRules(m_rules);

    // watch dconfig
    connect(m_config, &Dtk::Core::DConfig::valueChanged, this, [this](const QString &key) {
      if (key == "log_rules") {
          setRules(m_config->value(key).toByteArray());
          qCDebug(logUtils) << "value changed:" << key;
      }
    });
}

void LoggerRules::setRules(const QString &rules) {
  auto tmpRules = rules;
  m_rules = tmpRules.replace(";", "\n");
  QLoggingCategory::setFilterRules(m_rules);
}

void LoggerRules::appendRules(const QString &rules) {
  QString tmpRules = rules;
  tmpRules = tmpRules.replace(";", "\n");
  auto tmplist = tmpRules.split('\n');
  for (int i = 0; i < tmplist.count(); i++)
    if (m_rules.contains(tmplist.at(i))) {
      tmplist.removeAt(i);
      i--;
    }
  if (tmplist.isEmpty())
    return;
  m_rules.isEmpty() ? m_rules = tmplist.join("\n")
                    : m_rules += "\n" + tmplist.join("\n");
}
#endif
