// SPDX-FileCopyrightText: 2019 - 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef LOGVIEWERSERVICE_H
#define LOGVIEWERSERVICE_H

#include <dgiomount.h>

#include <QObject>
#include <QDBusContext>
#include <QScopedPointer>
#include <QProcess>
#include <QTemporaryDir>
#include <QDBusUnixFileDescriptor>

class QTextStream;
class DGioVolumeManager;
class LogViewerService : public QObject
    , protected QDBusContext
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "com.deepin.logviewer")
public:
    explicit LogViewerService(QObject *parent = nullptr);
    ~LogViewerService();

Q_SIGNALS:

public Q_SLOTS:
    Q_SCRIPTABLE QString readLog(const QDBusUnixFileDescriptor &fd);
    // 获取指定行数范围的日志内容
    Q_SCRIPTABLE QStringList readLogLinesInRange(const QDBusUnixFileDescriptor &fd, qint64 startLine, qint64 lineCount, bool bReverse);
    Q_SCRIPTABLE int exitCode();
    Q_SCRIPTABLE void quit();
    Q_SCRIPTABLE QStringList getFileInfo(const QString &file, bool unzip = true);
    Q_SCRIPTABLE QStringList getOtherFileInfo(const QString &file, bool unzip = true);
    Q_SCRIPTABLE bool exportLog(const QString &outDir, const QString &in, bool isFile);
    Q_SCRIPTABLE QString openLogStream(const QString &filePath);
    Q_SCRIPTABLE QString readLogInStream(const QString &token);
    Q_SCRIPTABLE QString isFileExist(const QString &filePath);
    Q_SCRIPTABLE quint64 getFileSize(const QString &filePath);
    Q_SCRIPTABLE qint64 getLineCount(const QString &filePath);
    // 仅能执行特定合法命令
    Q_SCRIPTABLE QString executeCmd(const QString &cmd);
    Q_SCRIPTABLE QStringList whiteListOutPaths();
    // 通过前端传入的文件描述符导出运维日志：后端在 /var/log 下创建随机临时目录收集日志，
    // 整体压缩后写入 fd，随后自行清理临时目录，不再向调用方返回路径。
    Q_SCRIPTABLE bool exportOpsLog(const QDBusUnixFileDescriptor &fd);

public:
    // 获取用户家目录
    QStringList getHomePaths();
    // 获取外设挂载路径(包括smb路径)
    QStringList getExternalDevPaths();
    QList<QExplicitlySharedDataPointer<DGioMount>> getMounts_safe();

private:
    QString readLog(const QString &filePath);
    Q_SCRIPTABLE QStringList readLogLinesInRange(const QString &filePath, qint64 startLine, qint64 lineCount, bool bReverse);
    // 清理临时目录下一些缓存文件，如解压后dump文件等（前端可能没权限删除，因此统一放到后端清理）
    void clearTempFiles();

    // 通过缓存块方式快速定位到指定行行首
    qint64 findLineStartOffsetWithCaching(const QString &filePath, qint64 targetLine);

    qint64 readFileAndReturnIndex(const QString &filePath, qint64 startLine, QList<uint64_t>& lineIndexes, bool reverseOrder);

    // 获取当前调用该 DBus 接口的用户家目录路径
    QString getCallerHomeDir();
    // 基于 fd 的 TOCTOU 安全方式删除 exportOpsLog 产生的 /var/log 临时目录（opsDir）。
    // 由 exportOpsLog 写入 fd 完成后自动调用回收，成功返回 true。
    bool removeOpsTempDirByPathInternal(const QString &path);
    // 基于 fd 的 TOCTOU 安全递归删除目录。
    bool safeRemoveDirRecursive(int parentFd, const char *name);

private:
    bool checkAuthorization(const QString &actionId);
private:
    QTemporaryDir tmpDir;
    QProcess m_process;
    QString m_tmpDirPath;
    QString m_actionId;
    QMap<QString, QStringList> m_commands;
    QMap<QString, std::pair<QString, QTextStream*>> m_logMap;
    QMap<QString, QList<uint64_t>> m_logLineIndex;

    bool checkAuth(const QString &actionId);
    QByteArray processCatFile(const QString &filePath);
    void processCmdArgs(const QString &cmdStr, const QStringList &args);
};

#endif // LOGVIEWERSERVICE_H
