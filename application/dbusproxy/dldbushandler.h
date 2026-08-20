// SPDX-FileCopyrightText: 2019 - 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef DLDBUSHANDLER_H
#define DLDBUSHANDLER_H

#include "dldbusinterface.h"
#include <QObject>

class DLDBusHandler : public QObject
{
    Q_OBJECT
public:
    /**
     * @brief 获取单例对象。线程安全（C++11 魔法静态变量保证）。
     * @note 旧实现要求 parent 非空才会创建实例，导致在子线程中首次调用
     *       （如不传 this 的 DLDBusHandler::instance()->executeCmd(...)）
     *       会返回 nullptr，从而引发空指针崩溃。现改为标准的 Meyers 单例，
     *       移除 parent 参数，单例自行管理生命周期，多线程下首次调用即创建。
     */
    static DLDBusHandler *instance();
    ~DLDBusHandler();
    QString readLog(const QString &filePath);
    QStringList readLogLinesInRange(const QString &filePath, qint64 startLine = 0, qint64 lineCount = 500, bool bReverse = true);
    QStringList getFileInfo(const QString &flag, bool unzip = true);
    bool isGetFileInfoError() const;
    QStringList getOtherFileInfo(const QString &flag, bool unzip = true);
    int exitCode();
    void quit();
    bool exportLog(const QString &outDir, const QString &in, bool isFile);
    bool isFileExist(const QString &filePath);
    quint64 getFileSize(const QString &filePath);
    qint64 getLineCount(const QString &filePath);
    QString executeCmd(const QString &cmd);
    QString openLogStream(const QString &filePath);
    QString readLogInStream(const QString &token);
    QStringList whiteListOutPaths();
    bool exportOpsLog(const QString &zipFilePath);

private:
    explicit DLDBusHandler(QObject *parent = nullptr);

private:
    QString createFilePathCacheFile(const QString& logFilePath);
    void releaseFilePathCacheFile(const QString &cacheFilePath);

private:
    DeepinLogviewerInterface *m_dbus;
    QStringList filePath;
    bool m_bGetFileInfoError = false;

    QTemporaryDir m_tempDir;
};

#endif // DLDBUSHANDLER_H
