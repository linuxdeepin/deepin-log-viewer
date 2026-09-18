// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef USERLOGACCESS_H
#define USERLOGACCESS_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QMutex>
#include <QMap>
#include <QTemporaryDir>

class QTextStream;

/**
 * @brief 用户日志访问类——在前端进程内本地读取用户家目录下的日志文件。
 *
 * ProtectHome=tmpfs 遮蔽后，后端守护进程无法访问 /home、/root、/run/user，
 * 用户日志读取从 root 后端 DBus 接口下沉到前端用户进程内本地完成。
 *
 * 线程安全：前端多个 LogAuthThread/ParseThread 并发调用此类，内部用 QMutex
 * 保护共享状态（行索引缓存、日志流 map、解压临时目录）。
 * 解压临时文件落在 ~/.cache/deepin/deepin-log-viewer/user-log/extract-XXXXXX/ 下，
 * 每次调用使用独立子目录避免并发互删。
 */
class UserLogAccess : public QObject
{
    Q_OBJECT
public:
    static UserLogAccess *instance();
    ~UserLogAccess();

    // 与后端 DBus 接口对齐的读取方法
    QString readLog(const QString &filePath);
    QStringList readLogLinesInRange(const QString &filePath, qint64 startLine, qint64 lineCount, bool bReverse);
    QStringList getFileInfo(const QString &file, bool unzip = true);
    QStringList getOtherFileInfo(const QString &file, bool unzip = true);
    bool isFileExist(const QString &filePath);
    quint64 getFileSize(const QString &filePath);
    qint64 getLineCount(const QString &filePath);
    QString openLogStream(const QString &filePath);
    QString readLogInStream(const QString &token);

private:
    explicit UserLogAccess(QObject *parent = nullptr);

    // 在 ~/.cache/deepin/deepin-log-viewer/user-log 下创建独立解压子目录
    QString createExtractSubdir();
    // 清理解压子目录下的文件
    void removeDirFiles(const QString &dirPath);
    // 解压 .gz 到指定子目录
    QString unzipToTempFile(const QString &sourceFile, const QString &tempFileTemplate);
    // 行索引缓存辅助
    qint64 readFileAndReturnIndex(const QString &filePath, qint64 startLine,
                                  QList<uint64_t> &lineIndexes, bool reverseOrder);

private:
    QMutex m_mutex;
    // 行索引缓存：token(filePath md5) -> 行偏移列表
    QMap<QString, QList<uint64_t>> m_logLineIndex;
    // 流式读取：token -> {内容, QTextStream}
    QMap<QString, std::pair<QString, QTextStream *>> m_logMap;
    // 解压根目录 ~/.cache/deepin/deepin-log-viewer/user-log
    QString m_extractRoot;
};

#endif // USERLOGACCESS_H
