// SPDX-FileCopyrightText: 2019 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "utils.h"
#include "logsettings.h"

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
#include <polkit-qt5-1/PolkitQt1/Authority>
using namespace PolkitQt1;

QHash<QString, QPixmap> Utils::m_imgCacheHash;
QHash<QString, QString> Utils::m_fontNameCache;
QMap<QString, QStringList> Utils::m_mapAuditType2EventType;
int Utils::specialComType = -1;
QString Utils::homePath = QDir::homePath();

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
             .filePath(qApp->organizationName()));

    return dir.filePath(qApp->applicationName());
}

QString Utils::getAppDataPath()
{
    QDir dir(QDir(Utils::homePath + "/.local/share")
             .filePath(qApp->organizationName()));

    return dir.filePath(qApp->applicationName());
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
    if (!dirname.isEmpty())
        parentPath.mkpath(dirname);
    return parentDir + "/" + dirname;
}

bool Utils::checkAuthorization(const QString &actionId, qint64 applicationPid)
{
    Authority::Result result;

    result = Authority::instance()->checkAuthorizationSync(actionId, UnixProcessSubject(applicationPid),
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
    return pwd->pw_name;
}

QString Utils::getCurrentUserName()
{
    // 获取当前系统用户名
    struct passwd* pwd = getpwuid(getuid());
    return pwd->pw_name;
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

QString Utils::getHomePath(const QString &userName)
{
    QString uName("");
    if (!userName.isEmpty())
        uName = userName;
    else
        uName = getCurrentUserName();


    QProcess *unlock = new QProcess;
    unlock->start("sh", QStringList() << "-c" << QString("cat /etc/passwd | grep %1").arg(uName));
    unlock->waitForFinished();
    auto output = unlock->readAllStandardOutput();
    auto str = QString::fromUtf8(output);
    str = str.mid(str.indexOf("::") + 2).split(":").first();
    qInfo() << "userName: " << uName << "homePath:" << str;
    return str;
}
