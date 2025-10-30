// SPDX-FileCopyrightText: 2019 - 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "utils.h"
#include "logsettings.h"
#include "dbusmanager.h"
#include "dbusproxy/dldbushandler.h"
#include "qtcompat.h"

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

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
#include <polkit-qt5-1/PolkitQt1/Authority>
#else
#include <polkit-qt6-1/PolkitQt1/Authority>
#endif
using namespace PolkitQt1;

Q_DECLARE_LOGGING_CATEGORY(logApp)

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
    qCDebug(logApp) << "Utils constructor";
}

Utils::~Utils()
{
    qCDebug(logApp) << "Utils destructor called";
}

QString Utils::getQssContent(const QString &filePath)
{
    qCDebug(logApp) << "Loading QSS content from:" << filePath;
    QFile file(filePath);
    QString qss;

    if (file.open(QIODevice::ReadOnly)) {
        qss = file.readAll();
        qCDebug(logApp) << "Successfully loaded QSS content";
    } else {
        qCWarning(logApp) << "Failed to open QSS file:" << filePath << file.errorString();
    }

    return qss;
}

QString Utils::getConfigPath()
{
    qCDebug(logApp) << "Getting config path";
    QDir dir(QDir(Utils::homePath + "/.config")
             .filePath(qApp->organizationName()));

    QString path = dir.filePath(qApp->applicationName());
    qCDebug(logApp) << "Config path:" << path;
    return path;
}

QString Utils::getAppDataPath()
{
    qCDebug(logApp) << "Getting app data path";
    QDir dir(QDir(Utils::homePath + "/.local/share")
             .filePath(qApp->organizationName()));

    QString path = dir.filePath(qApp->applicationName());
    qCDebug(logApp) << "App data path:" << path;
    return path;
}

bool Utils::isFontMimeType(const QString &filePath)
{
    qCDebug(logApp) << "Checking font MIME type for:" << filePath;
    const QString mimeName = QMimeDatabase().mimeTypeForFile(filePath).name();
    
    bool isFont = mimeName.startsWith("font/") || mimeName.startsWith("application/x-font");
    qCDebug(logApp) << "MIME type:" << mimeName << "isFont:" << isFont;
    
    return isFont;
}

bool Utils::isTextFileType(const QString &filePath)
{
    qCDebug(logApp) << "Checking text file type for:" << filePath;
    QMimeDatabase db;
    QMimeType mime = db.mimeTypeForFile(filePath);
    //标准文本文件和空白无后缀文件均计入文本文件范围
    bool isText = mime.inherits("text/plain") || mime.inherits("application/x-zerosize");
    qCDebug(logApp) << "MIME type:" << mime.name() << "isText:" << isText;
    return isText;
}

QString Utils::suffixList()
{
    qCDebug(logApp) << "Getting suffix list";
    return QString("Font Files (*.ttf *.ttc *.otf)");
}

QPixmap Utils::renderSVG(const QString &filePath, const QSize &size)
{
    qCDebug(logApp) << "Rendering SVG:" << filePath << "size:" << size;
    if (m_imgCacheHash.contains(filePath)) {
        qCDebug(logApp) << "Using cached image";
        return m_imgCacheHash.value(filePath);
    }

    QImageReader reader;
    QPixmap pixmap;

    reader.setFileName(filePath);

    if (reader.canRead()) {
        qCDebug(logApp) << "Reading image with scaled size";
        const qreal ratio = qApp->devicePixelRatio();
        reader.setScaledSize(size * ratio);
        pixmap = QPixmap::fromImage(reader.read());
        pixmap.setDevicePixelRatio(ratio);
    } else {
        qCDebug(logApp) << "Loading image directly";
        pixmap.load(filePath);
    }

    m_imgCacheHash.insert(filePath, pixmap);
    qCDebug(logApp) << "Image cached and returned";

    return pixmap;
}

QString Utils::loadFontFamilyFromFiles(const QString &fontFileName)
{
    qCDebug(logApp) << "Loading font family from file:" << fontFileName;
    if (m_fontNameCache.contains(fontFileName)) {
        qCDebug(logApp) << "Using cached font name";
        return m_fontNameCache.value(fontFileName);
    }

    QString fontFamilyName = "";

    QFile fontFile(fontFileName);
    if (!fontFile.open(QIODevice::ReadOnly)) {
        qCWarning(logApp) << "Failed to open font file:" << fontFileName;
        return fontFamilyName;
    }

    int loadedFontID = QFontDatabase::addApplicationFontFromData(fontFile.readAll());
    QStringList loadedFontFamilies = QFontDatabase::applicationFontFamilies(loadedFontID);
    if (!loadedFontFamilies.empty()) {
        fontFamilyName = loadedFontFamilies.at(0);
        qCDebug(logApp) << "Font family loaded:" << fontFamilyName;
    } else {
        qCWarning(logApp) << "No font families found in file";
    }
    fontFile.close();

    m_fontNameCache.insert(fontFileName, fontFamilyName);
    return fontFamilyName;
}

QString Utils::getReplaceColorStr(const char *d)
{
    qCDebug(logApp) << "Replacing color strings in text";
    QByteArray byteChar(d);
    byteChar = replaceEmptyByteArray(byteChar);
    QString d_str = QString(byteChar);
    d_str.replace(REG_EXP("\\x1B\\[\\d+(;\\d+){0,2}m"), "");
    d_str.replace(REG_EXP("\\002"), "");
    return  d_str;
}

/**
 * @brief Utils::replaceEmptyByteArray 替换从qproccess获取的日志信息的字符中的空字符串 替换 \u0000,不然QByteArray会忽略这以后的内容
 * @param iReplaceStr要替换的字符串
 * @return 替换过的字符串
 */
QByteArray Utils::replaceEmptyByteArray(const QByteArray &iReplaceStr)
{
    // qCDebug(logApp) << "Replacing empty bytes in array";
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
    qCDebug(logApp) << "Checking command error type for:" << str;
    if (str.contains("权限") || str.contains("permission", Qt::CaseInsensitive)) {
        qCDebug(logApp) << "Permission error detected";
        return PermissionError;
    }
    if (str.contains("请重试") || str.contains("retry", Qt::CaseInsensitive)) {
        qCDebug(logApp) << "Retry error detected";
        return RetryError;
    }
    qCDebug(logApp) << "No error detected";
    return NoError;
}

bool Utils::checkAndDeleteDir(const QString &iFilePath)
{
    qCDebug(logApp) << "Checking and deleting directory/file:" << iFilePath;
    QFileInfo tempFileInfo(iFilePath);

    if (tempFileInfo.isDir()) {
        bool result = deleteDir(iFilePath);
        qCDebug(logApp) << "Directory deletion result:" << result;
        return result;
    } else if (tempFileInfo.isFile()) {
        QFile deleteFile(iFilePath);
        bool result = deleteFile.remove();
        qCDebug(logApp) << "File deletion result:" << result;
        return result;
    }
    
    qCWarning(logApp) << "Path is neither directory nor file:" << iFilePath;
    return false;
}

bool Utils::deleteDir(const QString &iFilePath)
{
    qCDebug(logApp) << "Deleting directory:" << iFilePath;
    QDir directory(iFilePath);
    if (!directory.exists()) {
        qCDebug(logApp) << "Directory does not exist";
        return true;
    }

    QString srcPath = QDir::toNativeSeparators(iFilePath);
    if (!srcPath.endsWith(QDir::separator())) {
        qCDebug(logApp) << "Adding separator to path";
        srcPath += QDir::separator();
    }

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
    // qCDebug(logApp) << "Replacing colorful font in string";
    iStr->replace(REG_EXP("[[0-9]{1,2}m"), "");
}

bool Utils::isWayland()
{
    // qCDebug(logApp) << "Checking if Wayland is enabled";
    auto e = QProcessEnvironment::systemEnvironment();
    QString XDG_SESSION_TYPE = e.value(QStringLiteral("XDG_SESSION_TYPE"));
    QString WAYLAND_DISPLAY = e.value(QStringLiteral("WAYLAND_DISPLAY"));

    if (XDG_SESSION_TYPE == QLatin1String("wayland") || WAYLAND_DISPLAY.contains(QLatin1String("wayland"), Qt::CaseInsensitive)) {
        qCDebug(logApp) << "Wayland is enabled";
        return true;
    } else {
        qCDebug(logApp) << "Wayland is not enabled";
        return false;
    }
}

bool Utils::sleep(unsigned int msec)
{
    // qCDebug(logApp) << "Sleeping for:" << msec << "milliseconds";
    QTime dieTime = QTime::currentTime().addMSecs(static_cast<int>(msec));

    while (QTime::currentTime() < dieTime) {
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
    }

    return true;
}

QString Utils::mkMutiDir(const QString &path)
{
    qCDebug(logApp) << "Creating multi-level directory:" << path;
    QDir dir(path);
    if (path.isEmpty() || dir.exists()) {
        qCDebug(logApp) << "Directory already exists";
        return path;
    }
    QString parentDir = mkMutiDir(path.mid(0, path.lastIndexOf('/')));
    QString dirname = path.mid(path.lastIndexOf('/') + 1);
    QDir parentPath(parentDir);
    if (!dirname.isEmpty()) {
        bool bRet = parentPath.mkpath(dirname);
        if (!bRet)
            qCWarning(logApp) << QString("mkpath: unable to create directory '%1' in path: '%2/', please confirm if the file exists.").arg(dirname).arg(parentDir);
    }
    return parentDir + "/" + dirname;
}

bool Utils::checkAuthorization(const QString &actionId, qint64 applicationPid)
{
    qCDebug(logApp) << "Checking authorization for action:" << actionId << "pid:" << applicationPid;
    Authority::Result result;

    result = Authority::instance()->checkAuthorizationSync(actionId, UnixProcessSubject(applicationPid),
                                                           Authority::AllowUserInteraction);
    bool authorized = result == Authority::Yes;
    qCDebug(logApp) << "Authorization result:" << authorized;
    
    return authorized;
}

QString Utils::osVersion()
{
    qCDebug(logApp) << "Getting OS version";
    QProcess *unlock = new QProcess;
    unlock->start("lsb_release", {"-r"});
    unlock->waitForFinished();
    auto output = unlock->readAllStandardOutput();
    auto str = QString::fromUtf8(output);
    REG_EXP re("\t.+\n");
    QString osVerStr;
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    if (re.indexIn(str) > -1) {
        auto result = re.cap(0);
#else
    QRegularExpressionMatch match = re.match(str);
    if (match.hasMatch()) {
        auto result = match.captured(0);
#endif
        osVerStr = result.remove(0, 1).remove(result.size() - 1, 1);
    }
    unlock->deleteLater();
    return osVerStr;
}

QString Utils::auditType(const QString &eventType)
{
    // qCDebug(logApp) << "Getting audit type for event type:" << eventType;
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
    // qCDebug(logApp) << "Converting capacity to MB:" << cap;
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
    qCDebug(logApp) << "Getting user name by UID:" << uid;
    struct passwd * pwd;
    pwd = getpwuid(uid);
    if (nullptr == pwd) {
        qCWarning(logApp) << QString("unknown uid:%1").arg(uid);
        return "";
    }

    return pwd->pw_name;
}

QString Utils::getUserHomePathByUID(uint uid)
{
    qCDebug(logApp) << "Getting user home path by UID:" << uid;
    struct passwd * pwd;
    pwd = getpwuid(uid);
    if (nullptr == pwd) {
        qCWarning(logApp) << QString("unknown uid:%1").arg(uid);
        return "";
    }

    return pwd->pw_dir;
}

QString Utils::getCurrentUserName()
{
    // qCDebug(logApp) << "Getting current user name";
    // 获取当前系统用户名
    struct passwd* pwd = getpwuid(getuid());
    return pwd->pw_name;
}

bool Utils::isCoredumpctlExist()
{
    qCDebug(logApp) << "Checking if coredumpctl exists";
    bool isCoredumpctlExist = false;
    QDir dir("/usr/bin");
    QStringList list = dir.entryList(QStringList() << (QString("coredumpctl") + "*"), QDir::NoDotAndDotDot | QDir::Files);
    
    for (int i = 0; i < list.count(); i++) {
        if("coredumpctl" == list[i]) {
            isCoredumpctlExist = true;
            qCDebug(logApp) << "Found coredumpctl";
            break;
        }
    }
    
    qCDebug(logApp) << "coredumpctl exists:" << isCoredumpctlExist;
    return isCoredumpctlExist;
}

QString Utils::appName(const QString &filePath)
{
    qCDebug(logApp) << "Getting app name for file path:" << filePath;
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
    qCDebug(logApp) << "Resetting to normal auth for path:" << path;
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
    qCDebug(logApp) << "Counting repeat coredumps from:" << timeBegin << "to:" << timeEnd;
    QList<LOG_REPEAT_COREDUMP_INFO> result;
    QString data = DLDBusHandler::instance()->executeCmd("coredumpctl-list");
    QStringList strList = data.split('\n', SKIP_EMPTY_PARTS);
    int total = 0;
    for (int i = strList.size() - 1; i >= 0; --i) {
        QString str = strList.at(i);
        if (str.trimmed().isEmpty())
            continue;
        QStringList tmpList = str.split(" ", SKIP_EMPTY_PARTS);
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
    qCDebug(logApp) << "Getting repeat coredump exe paths";
    QFile file(COREDUMP_REPEAT_CONFIG_PATH);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return QStringList();

    QString data = file.readAll();

    file.close();

    return data.split(' ');
}

void Utils::updateRepeatCoredumpExePaths(const QList<LOG_REPEAT_COREDUMP_INFO> &infos)
{
    qCDebug(logApp) << "Updating repeat coredump exe paths";
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
        qCWarning(logApp) << "failed to open coredump repeat config file:" << COREDUMP_REPEAT_CONFIG_PATH;
        return;
    }

    QTextStream out(&file);
    out << curReaptExePaths.join(' ');

    file.close();
}

static QByteArray processCmdWithArgs(const QString &cmdStr, const QString &workPath, const QStringList &args)
{
    qCDebug(logApp) << "Executing command:" << cmdStr << "with arguments:" << args << "in working directory:" << workPath;
    QProcess process;
    if (!workPath.isEmpty())
        process.setWorkingDirectory(workPath);

    process.setProgram(cmdStr);
    process.setArguments(args);
    process.setEnvironment({"LANG=en_US.UTF-8", "LANGUAGE=en_US"});
    process.start();
    // Wait for process to finish without timeout.
    process.waitForFinished(-1);
    QByteArray outPut = process.readAllStandardOutput();
    int nExitCode = process.exitCode();
    bool bRet = (process.exitStatus() == QProcess::NormalExit && nExitCode == 0);
    if (!bRet) {
        qCDebug(logApp) << "run cmd error, caused by:" << process.errorString() << "output:" << outPut;
        return QByteArray();
    }
    return outPut;
}

QByteArray Utils::executeCmd(const QString &cmdStr, const QStringList &args, const QString &workPath)
{
    qCDebug(logApp) << "Executing command:" << cmdStr << "with arguments:" << args << "in working directory:" << workPath;
    return processCmdWithArgs(cmdStr, workPath,  args);
}

void Utils::exportSomeOpsLogs(const QString &outDir, const QString &userHomeDir)
{
    Q_UNUSED(userHomeDir)

    std::string tmpCmd;

    // app
    tmpCmd = ("glxinfo -B >> " + outDir + "/app/kwin/glxinfo.log").toStdString();
    system(tmpCmd.c_str());
    tmpCmd = ("cp -rf /tmp/fcitx*.log " + outDir + "/app/fcitx/").toStdString();
    system(tmpCmd.c_str());

    // kernel
    tmpCmd = ("aplay -l >> " + outDir + "/kernel/aplay.log").toStdString();
    system(tmpCmd.c_str());
    tmpCmd = ("ethtool -i $(ifconfig | grep --max-count=1 ^en | awk -F ':' '{print $1}') >> " + outDir + "/kernel/eth_info.log").toStdString();
    system(tmpCmd.c_str());
    tmpCmd = ("ifconfig >> " + outDir + "/kernel/ifconfig.log").toStdString();
    system(tmpCmd.c_str());
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
          qCDebug(logApp) << "value changed:" << key;
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
