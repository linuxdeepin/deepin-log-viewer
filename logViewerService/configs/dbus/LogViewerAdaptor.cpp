// SPDX-FileCopyrightText: 2024 - 2025 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "LogViewerAdaptor.h"
#include <QtCore/QMetaObject>
#include <QtCore/QByteArray>
#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QVariant>

/*
 * Implementation of adaptor class LogViewerAdaptor
 */

LogViewerAdaptor::LogViewerAdaptor(QObject *parent)
    : QDBusAbstractAdaptor(parent)
{
    // constructor
    setAutoRelaySignals(true);
}

LogViewerAdaptor::~LogViewerAdaptor()
{
    // destructor
}

QString LogViewerAdaptor::executeCmd(const QString &cmd)
{
    // handle method call com.deepin.logviewer.executeCmd
    QString out0;
    QMetaObject::invokeMethod(parent(), "executeCmd", Q_RETURN_ARG(QString, out0), Q_ARG(QString, cmd));
    return out0;
}

int LogViewerAdaptor::exitCode()
{
    // handle method call com.deepin.logviewer.exitCode
    int out0;
    QMetaObject::invokeMethod(parent(), "exitCode", Q_RETURN_ARG(int, out0));
    return out0;
}

bool LogViewerAdaptor::exportLog(const QString &outDir, const QString &in, bool isFile)
{
    // handle method call com.deepin.logviewer.exportLog
    bool out0;
    QMetaObject::invokeMethod(parent(), "exportLog", Q_RETURN_ARG(bool, out0), Q_ARG(QString, outDir), Q_ARG(QString, in), Q_ARG(bool, isFile));
    return out0;
}

QStringList LogViewerAdaptor::getFileInfo(const QString &file, bool unzip)
{
    // handle method call com.deepin.logviewer.getFileInfo
    QStringList out0;
    QMetaObject::invokeMethod(parent(), "getFileInfo", Q_RETURN_ARG(QStringList, out0), Q_ARG(QString, file), Q_ARG(bool, unzip));
    return out0;
}

qulonglong LogViewerAdaptor::getFileSize(const QString &filePath)
{
    // handle method call com.deepin.logviewer.getFileSize
    qulonglong out0;
    QMetaObject::invokeMethod(parent(), "getFileSize", Q_RETURN_ARG(qulonglong, out0), Q_ARG(QString, filePath));
    return out0;
}

qlonglong LogViewerAdaptor::getLineCount(const QString &filePath)
{
    // handle method call com.deepin.logviewer.getLineCount
    qlonglong out0;
    QMetaObject::invokeMethod(parent(), "getLineCount", Q_RETURN_ARG(qlonglong, out0), Q_ARG(QString, filePath));
    return out0;
}

QString LogViewerAdaptor::isFileExist(const QString &filePath)
{
    // handle method call com.deepin.logviewer.isFileExist
    QString out0;
    QMetaObject::invokeMethod(parent(), "isFileExist", Q_RETURN_ARG(QString, out0), Q_ARG(QString, filePath));
    return out0;
}

QString LogViewerAdaptor::openLogStream(const QString &filePath)
{
    // handle method call com.deepin.logviewer.openLogStream
    QString out0;
    QMetaObject::invokeMethod(parent(), "openLogStream", Q_RETURN_ARG(QString, out0), Q_ARG(QString, filePath));
    return out0;
}

void LogViewerAdaptor::quit()
{
    // handle method call com.deepin.logviewer.quit
    QMetaObject::invokeMethod(parent(), "quit");
}

QString LogViewerAdaptor::readLog(int fd)
{
    // handle method call com.deepin.logviewer.readLog
    QString out0;
    QMetaObject::invokeMethod(parent(), "readLog", Q_RETURN_ARG(QString, out0), Q_ARG(int, fd));
    return out0;
}

QString LogViewerAdaptor::readLogInStream(const QString &token)
{
    // handle method call com.deepin.logviewer.readLogInStream
    QString out0;
    QMetaObject::invokeMethod(parent(), "readLogInStream", Q_RETURN_ARG(QString, out0), Q_ARG(QString, token));
    return out0;
}

QStringList LogViewerAdaptor::readLogLinesInRange(int fd, qlonglong startLine, qlonglong lineCount)
{
    // handle method call com.deepin.logviewer.readLogLinesInRange
    QStringList out0;
    QMetaObject::invokeMethod(parent(), "readLogLinesInRange", Q_RETURN_ARG(QStringList, out0), Q_ARG(int, fd), Q_ARG(qlonglong, startLine), Q_ARG(qlonglong, lineCount));
    return out0;
}

QStringList LogViewerAdaptor::whiteListOutPaths()
{
    // handle method call com.deepin.logviewer.whiteListOutPaths
    QStringList out0;
    QMetaObject::invokeMethod(parent(), "whiteListOutPaths", Q_RETURN_ARG(QStringList, out0));
    return out0;
}

