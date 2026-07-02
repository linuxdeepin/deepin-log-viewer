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
#include <QMap>

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
    Q_SCRIPTABLE QString exportOpsLog();
    // 清理 exportOpsLog 在 /var/log 下创建的临时导出目录。无参：清理的是服务端缓存
    // 的本次创建路径，避免调用方传参带来的路径替换/越权删除风险。
    Q_SCRIPTABLE bool removeOpsLogTempDir();
    Q_SCRIPTABLE QString openLogStream(const QString &filePath);
    Q_SCRIPTABLE QString readLogInStream(const QString &token);
    Q_SCRIPTABLE QString isFileExist(const QString &filePath);
    Q_SCRIPTABLE quint64 getFileSize(const QString &filePath);
    Q_SCRIPTABLE qint64 getLineCount(const QString &filePath);
    // 仅能执行特定合法命令
    Q_SCRIPTABLE QString executeCmd(const QString &cmd);
    Q_SCRIPTABLE QStringList whiteListOutPaths();

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

private:
    bool checkAuthorization(const QString &actionId, const QString &busName);
private:
    QTemporaryDir tmpDir;
    QProcess m_process;
    QString m_tmpDirPath;
    // exportOpsLog 创建的 /var/log 临时目录路径，按 D-Bus 调用方 unique bus name 隔离存储，
    // 供 removeOpsLogTempDir 按当前调用者身份清理。避免单一成员变量在多用户/多窗口并发
    // 导出时被覆盖，导致临时目录泄露或误清理他人目录。
    QMap<QString, QString> m_opsTempDirs;
    QString m_actionId;
    QMap<QString, QStringList> m_commands;
    QMap<QString, std::pair<QString, QTextStream*>> m_logMap;
    QMap<QString, QList<uint64_t>> m_logLineIndex;
    bool checkAuth(const QString &actionId);
    // 按 fd-relative 安全方式删除 exportOpsLog 产生的 /var/log 临时目录（path 为缓存路径）。
    // 成功返回 true；path 为空或非预期前缀/删除失败返回 false。
    bool removeOpsTempDirByPath(const QString &path);
    // 获取当前调用该 DBus 接口的用户家目录路径与主组 gid
    bool getCallerHomeAndGid(QString &outHome, uint &outGid);
    QByteArray processCatFile(const QString &filePath);
    void processCmdArgs(const QString &cmdStr, const QStringList &args);
};

#endif // LOGVIEWERSERVICE_H
