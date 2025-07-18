// SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "cliapplicationhelper.h"

#include <unistd.h>

#include <QLoggingCategory>
#include <QLockFile>
#include <QLocalServer>
#include <QLocalSocket>
#include <QDir>
#include <QDataStream>

using HelperCreator = CliApplicationHelper::HelperCreator;

Q_DECLARE_LOGGING_CATEGORY(logApp)

Q_GLOBAL_STATIC(QLocalServer, _d_singleServer)

/*!
 @brief cli程序单实例类
 @private
 */
class _CliApplicationHelper
{
public:
#define INVALID_HELPER reinterpret_cast<CliApplicationHelper*>(1)
    inline CliApplicationHelper *helper()
    {
        // 临时存储一个无效的指针值, 用于此处条件变量的竞争
        if (m_helper.testAndSetRelaxed(nullptr, INVALID_HELPER)) {
#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
            m_helper.storeRelaxed(creator());
        }
        return m_helper.loadRelaxed();
#else
            m_helper.store(creator());
        }
        return m_helper.load();
#endif
    }

    inline void clear()
    {
        if (m_helper != INVALID_HELPER)
            delete m_helper.fetchAndStoreRelaxed(nullptr);
    }

    static CliApplicationHelper *defaultCreator()
    {
        return new CliApplicationHelper();
    }

    QAtomicPointer<CliApplicationHelper> m_helper;
    static HelperCreator creator;
};

HelperCreator _CliApplicationHelper::creator = _CliApplicationHelper::defaultCreator;
static quint8 _d_singleServerVersion = 1;
Q_GLOBAL_STATIC(_CliApplicationHelper, _globalHelper)
int CliApplicationHelper::waitTime = 3000;


bool CliApplicationHelper::setSingleInstance(const QString &key, CliApplicationHelper::SingleScope singleScope)
{
    qCDebug(logApp) << "Setting up single instance with key:" << key << "and scope:" << singleScope;
    bool new_server = !_d_singleServer.exists();

    if (_d_singleServer->isListening()) {
        qCDebug(logApp) << "Closing existing server instance";
        _d_singleServer->close();
    }

    QString socket_key = "_d_deepin_logger_single_instance_";

    switch (singleScope) {
    case GroupScope:
        qCDebug(logApp) << "Setting up group scope";
        _d_singleServer->setSocketOptions(QLocalServer::GroupAccessOption);
#ifdef Q_OS_LINUX
        socket_key += QString("%1_").arg(getgid());
#endif
        break;
    case WorldScope:
        qCDebug(logApp) << "Setting up world scope";
        _d_singleServer->setSocketOptions(QLocalServer::WorldAccessOption);
        break;
    default:
        qCDebug(logApp) << "Setting up user scope";
        _d_singleServer->setSocketOptions(QLocalServer::UserAccessOption);
#ifdef Q_OS_LINUX
        socket_key += QString("%1_").arg(getuid());
#endif
        break;
    }

    socket_key += key;
    qCDebug(logApp) << "Generated socket key:" << socket_key;
    QString lockfile = socket_key;
    if (!lockfile.startsWith(QLatin1Char('/'))) {
        qCDebug(logApp) << "Lock file does not start with /, using temp path";
        lockfile = QDir::cleanPath(QDir::tempPath());
        lockfile += QLatin1Char('/') + socket_key;
    }
    lockfile += QStringLiteral(".lock");
    qCDebug(logApp) << "Using lock file:" << lockfile;
    static QScopedPointer <QLockFile> lock(new QLockFile(lockfile));
    // 同一个进程多次调用本接口使用最后一次设置的 key
    // FIX dcc 使用不同的 key 两次调用 setSingleInstance 后无法启动的问题
    qint64 tpid = -1;
    QString hostname, appname;
    if (lock->isLocked() && lock->getLockInfo(&tpid, &hostname, &appname) && tpid == getpid()) {
        qCWarning(logApp) << "call setSingleInstance again within the same process";
        lock->unlock();
        lock.reset(new QLockFile(lockfile));
    }

    if (!lock->tryLock()) {
        qCInfo(logApp) << "Instance already running (pid:" << getpid() << "), connecting to existing instance";
        // 通知别的实例
        QLocalSocket socket;
        qCDebug(logApp) << "Connecting to server at:" << socket_key;
        socket.connectToServer(socket_key);

        // 等待到有效数据时认为server实例有效
        qCDebug(logApp) << "Waiting for server connection (timeout:" << CliApplicationHelper::waitTime << "ms)";
        if (socket.waitForConnected(CliApplicationHelper::waitTime) &&
                socket.waitForReadyRead(CliApplicationHelper::waitTime)) {
            qCDebug(logApp) << "Server connection established";
            // 读取数据
            qint8 version;
            qint64 pid;
            QStringList arguments;

            QDataStream ds(&socket);
            ds >> version >> pid >> arguments;
            qCInfo(logApp) << "Process is started: pid=" << pid << "arguments=" << arguments;

            // 把自己的信息告诉第一个实例
            ds << _d_singleServerVersion << qApp->applicationPid() << qApp->arguments();
            socket.flush();
        }

        qCDebug(logApp) << "Server connection failed";
        return false;
    }

    qCInfo(logApp) << "Attempting to listen on socket:" << socket_key;
    if (!_d_singleServer->listen(socket_key)) {
        qCCritical(logApp) << "Failed to listen on socket:" << socket_key
                               << "- Error:" << _d_singleServer->errorString();
        return false;
    }

    if (new_server) {
        qCInfo(logApp) << "Setting up new server connection handler for socket:" << socket_key;
        QObject::connect(_d_singleServer, &QLocalServer::newConnection, qApp, [] {
            QLocalSocket *instance = _d_singleServer->nextPendingConnection();
            // 先发送数据告诉新的实例自己收到了它的请求
            QDataStream ds(instance);
            ds << _d_singleServerVersion // 协议版本
               << qApp->applicationPid() // 进程id
               << qApp->arguments(); // 启动时的参数

            qCDebug(logApp) << "New connection received, setting up readyRead handler";
            QObject::connect(instance, &QLocalSocket::readyRead, qApp, [instance] {
                // 读取数据
                QDataStream ds(instance);

                qint8 version;
                qint64 pid;
                QStringList arguments;

                ds >> version >> pid >> arguments;
                instance->close();

                qCInfo(logApp) << "New instance connected - pid:" << pid << "args:" << arguments;

                // 通知新进程的信息
                if (_globalHelper.exists() && _globalHelper->helper()) {
                    qCDebug(logApp) << "Emitting newProcessInstance signal";
                    Q_EMIT _globalHelper->helper()->newProcessInstance(pid, arguments);
                } else {
                    qCWarning(logApp) << "Global helper not available to emit signal";
                }
            });

            qCDebug(logApp) << "Flushing data to new instance";
            instance->flush(); //发送数据给新的实例
        });
    }

    qCDebug(logApp) << "Single instance set up successfully";
    return true;
}

CliApplicationHelper::CliApplicationHelper()
    : QObject(nullptr)
{
    qCDebug(logApp) << "Creating CliApplicationHelper instance";
    // 跟随application销毁
    qAddPostRoutine(staticCleanApplication);
}

CliApplicationHelper::~CliApplicationHelper()
{
    // qCDebug(logApp) << "Destroying CliApplicationHelper instance";
    _globalHelper->m_helper = nullptr;
}


void CliApplicationHelper::staticCleanApplication()
{
    qCDebug(logApp) << "Cleaning up application helper resources";
    if (_globalHelper.exists()) {
        qCDebug(logApp) << "Found global helper instance, clearing it";
        _globalHelper->clear();
    }
}

CliApplicationHelper *CliApplicationHelper::instance()
{
    // qCDebug(logApp) << "Getting CliApplicationHelper instance";
    return _globalHelper->helper();
}
