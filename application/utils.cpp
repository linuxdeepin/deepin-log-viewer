/*
* Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
*
* Author:    LZ <zhou.lu@archermind.com>
*
* Maintainer:  LZ <zhou.lu@archermind.com>
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "utils.h"

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
QHash<QString, QPixmap> Utils::m_imgCacheHash;
QHash<QString, QString> Utils::m_fontNameCache;

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
    QDir dir(QDir(QStandardPaths::standardLocations(QStandardPaths::ConfigLocation).first())
             .filePath(qApp->organizationName()));

    return dir.filePath(qApp->applicationName());
}

bool Utils::isFontMimeType(const QString &filePath)
{
    const QString mimeName = QMimeDatabase().mimeTypeForFile(filePath).name();;

    if (mimeName.startsWith("font/") ||
            mimeName.startsWith("application/x-font")) {
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
        //        qDebug() << "Open font file error";
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
QByteArray Utils::replaceEmptyByteArray(QByteArray &iReplaceStr)
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
        return  deleteFile.remove();
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
                //                qDebug() << "remove file" << filePath << " faild!";
                error = true;
            }
        } else if (fileInfo.isDir()) {
            if (!deleteDir(filePath)) {
                error = true;
            }
        }
    }

    if (!directory.rmdir(QDir::toNativeSeparators(directory.path()))) {
        //        qDebug() << "remove dir" << directory.path() << " faild!";
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
