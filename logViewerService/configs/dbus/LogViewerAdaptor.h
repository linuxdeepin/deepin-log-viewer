// SPDX-FileCopyrightText: 2024 - 2025 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef LOGVIEWERADAPTOR_H
#define LOGVIEWERADAPTOR_H

#include <QtCore/QObject>
#include <QtDBus/QtDBus>
QT_BEGIN_NAMESPACE
class QByteArray;
template<class T> class QList;
template<class Key, class Value> class QMap;
class QString;
class QStringList;
class QVariant;
QT_END_NAMESPACE

/*
 * Adaptor class for interface com.deepin.logviewer
 */
class LogViewerAdaptor: public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "com.deepin.logviewer")
    Q_CLASSINFO("D-Bus Introspection", ""
"  <interface name=\"com.deepin.logviewer\">\n"
"    <method name=\"readLog\">\n"
"      <arg direction=\"out\" type=\"s\"/>\n"
"      <arg direction=\"in\" type=\"i\" name=\"fd\"/>\n"
"    </method>\n"
"    <method name=\"readLogLinesInRange\">\n"
"      <arg direction=\"out\" type=\"as\"/>\n"
"      <arg direction=\"in\" type=\"i\" name=\"fd\"/>\n"
"      <arg direction=\"in\" type=\"x\" name=\"startLine\"/>\n"
"      <arg direction=\"in\" type=\"x\" name=\"lineCount\"/>\n"
"    </method>\n"
"    <method name=\"exitCode\">\n"
"      <arg direction=\"out\" type=\"i\"/>\n"
"    </method>\n"
"    <method name=\"quit\"/>\n"
"    <method name=\"exportLog\">\n"
"      <arg direction=\"out\" type=\"b\"/>\n"
"      <arg direction=\"in\" type=\"s\" name=\"outDir\"/>\n"
"      <arg direction=\"in\" type=\"s\" name=\"in\"/>\n"
"      <arg direction=\"in\" type=\"b\" name=\"isFile\"/>\n"
"    </method>\n"
"    <method name=\"getFileInfo\">\n"
"      <arg direction=\"out\" type=\"as\"/>\n"
"      <arg direction=\"in\" type=\"s\" name=\"file\"/>\n"
"      <arg direction=\"in\" type=\"b\" name=\"unzip\"/>\n"
"    </method>\n"
"    <method name=\"openLogStream\">\n"
"      <arg direction=\"out\" type=\"s\"/>\n"
"      <arg direction=\"in\" type=\"s\" name=\"filePath\"/>\n"
"    </method>\n"
"    <method name=\"readLogInStream\">\n"
"      <arg direction=\"out\" type=\"s\"/>\n"
"      <arg direction=\"in\" type=\"s\" name=\"token\"/>\n"
"    </method>\n"
"    <method name=\"isFileExist\">\n"
"      <arg direction=\"out\" type=\"s\"/>\n"
"      <arg direction=\"in\" type=\"s\" name=\"filePath\"/>\n"
"    </method>\n"
"    <method name=\"getFileSize\">\n"
"      <arg direction=\"out\" type=\"t\"/>\n"
"      <arg direction=\"in\" type=\"s\" name=\"filePath\"/>\n"
"    </method>\n"
"    <method name=\"getLineCount\">\n"
"      <arg direction=\"out\" type=\"x\"/>\n"
"      <arg direction=\"in\" type=\"s\" name=\"filePath\"/>\n"
"    </method>\n"
"    <method name=\"executeCmd\">\n"
"      <arg direction=\"out\" type=\"s\"/>\n"
"      <arg direction=\"in\" type=\"s\" name=\"cmd\"/>\n"
"    </method>\n"
"    <method name=\"whiteListOutPaths\">\n"
"      <arg direction=\"out\" type=\"as\"/>\n"
"    </method>\n"
"  </interface>\n"
        "")
public:
    LogViewerAdaptor(QObject *parent);
    virtual ~LogViewerAdaptor();

public: // PROPERTIES
public Q_SLOTS: // METHODS
    QString executeCmd(const QString &cmd);
    int exitCode();
    bool exportLog(const QString &outDir, const QString &in, bool isFile);
    QStringList getFileInfo(const QString &file, bool unzip);
    qulonglong getFileSize(const QString &filePath);
    qlonglong getLineCount(const QString &filePath);
    QString isFileExist(const QString &filePath);
    QString openLogStream(const QString &filePath);
    void quit();
    QString readLog(int fd);
    QString readLogInStream(const QString &token);
    QStringList readLogLinesInRange(int fd, qlonglong startLine, qlonglong lineCount);
    QStringList whiteListOutPaths();
Q_SIGNALS: // SIGNALS
};

#endif
