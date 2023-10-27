// SPDX-FileCopyrightText: 2019 - 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "logapplicationhelper.h"
#include "logcollectormain.h"
#include "logapplication.h"
#include "environments.h"
#include "dbusmanager.h"
#include "utils.h"
#include "eventlogutils.h"
#include "DebugTimeManager.h"
#include "logbackend.h"
#include "cliapplicationhelper.h"
#include "accessible.h"

#include <DApplication>
#include <DApplicationSettings>
#include <DMainWindow>
#include <DWidgetUtil>
#include <DLog>

#include <QDateTime>
#include <QSurfaceFormat>
#include <QDebug>
#include <QLoggingCategory>

DWIDGET_USE_NAMESPACE
DCORE_USE_NAMESPACE

#ifdef QT_DEBUG
Q_LOGGING_CATEGORY(logAppMain, "org.deepin.log.viewer.main")
#else
Q_LOGGING_CATEGORY(logAppMain, "org.deepin.log.viewer.main", QtInfoMsg)
#endif

int main(int argc, char *argv[])
{
    //在root下或者非deepin/uos环境下运行不会发生异常，需要加上XDG_CURRENT_DESKTOP=Deepin环境变量；
    if (!QString(qgetenv("XDG_CURRENT_DESKTOP")).toLower().startsWith("deepin")) {
        setenv("XDG_CURRENT_DESKTOP", "Deepin", 1);
    }

    // 命令参数大于1，进行命令行处理
    if (argc > 1) {
        QCoreApplication a(argc, argv);
        a.setOrganizationName("deepin");
        a.setApplicationName("deepin-log-viewer");

        DLogManager::registerConsoleAppender();

#if (DTK_VERSION >= DTK_VERSION_CHECK(5, 6, 8, 0))
        DLogManager::registerJournalAppender();
#else
        DLogManager::registerFileAppender();
#endif

        QCommandLineOption exportOption(QStringList() << "e" << "export", DApplication::translate("main", "Export logs to the specified path"), DApplication::translate("main", "PATH"));
        QCommandLineOption typeOption(QStringList() << "t" << "type", DApplication::translate("main", "Export logs of specified types"), DApplication::translate("main", "TYPE"));
        QCommandLineOption appOption(QStringList() << "d" << "deepin-application", DApplication::translate("main", "Export logs of specified self-developed applications"), DApplication::translate("main", "SELF APPNAME"));
        QCommandLineOption periodOption(QStringList() << "p" << "period", DApplication::translate("main", "Export logs within a specified time period"), DApplication::translate("main", "PERIOD"));
        QCommandLineOption levelOption(QStringList() << "l" << "level", DApplication::translate("main", "Export logs within a specified debug level"), DApplication::translate("main", "LEVEL"));
        QCommandLineOption statusOption(QStringList() << "s" << "status", DApplication::translate("main", "Export boot(no-klu) logs within a specified status"), DApplication::translate("main", "BOOT STATUS"));
        QCommandLineOption eventOption(QStringList() << "E" << "event", DApplication::translate("main", "Export boot-shutdown-event or audit logs within a specified event type"), DApplication::translate("main", "EVENT TYPE"));
        QCommandLineOption keywordOption(QStringList() << "k" << "search", DApplication::translate("main", "Export logs based on keywords search results"), DApplication::translate("main", "KEY WORD"));

        QCommandLineParser cmdParser;
        cmdParser.setApplicationDescription("deepin-log-viewer");
        cmdParser.addHelpOption();
        cmdParser.addVersionOption();
        cmdParser.addOption(exportOption);
        cmdParser.addOption(typeOption);
        cmdParser.addOption(appOption);
        cmdParser.addOption(periodOption);
        cmdParser.addOption(levelOption);
        cmdParser.addOption(statusOption);
        cmdParser.addOption(eventOption);
        cmdParser.addOption(keywordOption);

        if (!cmdParser.parse(qApp->arguments())) {
            cmdParser.showHelp();
        }

        cmdParser.process(a);

        // cli命令处理
        QStringList args = cmdParser.positionalArguments();
        if (cmdParser.isSet(exportOption)) {

            if (!CliApplicationHelper::instance()->setSingleInstance(a.applicationName(),
                                                                      CliApplicationHelper::UserScope)) {
                return 0;
            }

            Utils::runInCmd = true;

            // 若指定有导出目录，按指定目录导出
            QString outDir = cmdParser.value(exportOption);
            if (outDir.isEmpty()) {
                qCWarning(logAppMain) << "plseae input outpath.";
                return -1;
            }

            QString type = "";
            QString appName = "";
            QString period = "";
            QString level = "";
            QString status = "";
            QString event = "";
            QString keyword = "";
            if (cmdParser.isSet(typeOption))
                type = cmdParser.value(typeOption);
            if (cmdParser.isSet(appOption))
                appName = cmdParser.value(appOption);
            if (cmdParser.isSet(periodOption))
                period = cmdParser.value(periodOption);
            if (cmdParser.isSet(levelOption))
                level = cmdParser.value(levelOption);
            if (cmdParser.isSet(statusOption))
                status = cmdParser.value(statusOption);
            if (cmdParser.isSet(eventOption))
                event = cmdParser.value(eventOption);
            if (cmdParser.isSet(keywordOption))
                keyword = cmdParser.value(keywordOption);

            if (!type.isEmpty()) {
                // 按类型导出日志
                if (period.isEmpty() &&
                    level.isEmpty() &&
                    status.isEmpty() &&
                    event.isEmpty() &&
                    appName.isEmpty() &&
                    keyword.isEmpty()) {
                    int nRet = LogBackend::instance(&a)->exportTypeLogs(outDir, type);
                    return nRet;
                } else {
                    // 按筛选条件导出指定类型的日志
                    bool bRet = false;
                    QString error("");
                    LOG_FLAG flag = LogBackend::type2Flag(type, error);
                    if (TYPE_SYSTEM == type || Dmesg == flag || BOOT_KLU == flag || TYPE_DNF == type){
                        // system、dmesg(centos下内核日志)、boot_klu、dnf 可按周期或级别导出
                        if (!status.isEmpty() || !event.isEmpty())
                            qCWarning(logAppMain) << QString("Export logs by %1, can only be filtered using 'period' or 'level' parameter.").arg(type);
                        else
                            bRet = LogBackend::instance(&a)->exportTypeLogsByCondition(outDir, type, period, level, keyword);
                    } else if (KERN == flag || TYPE_DPKG == type || TYPE_COREDUMP == type) {
                        // 内核、dpkg、崩溃日志 可按周期导出
                        if (!level.isEmpty() || !status.isEmpty() || !event.isEmpty())
                            qCWarning(logAppMain) << QString("Export logs by %1, can only be filtered using 'period' condition.").arg(type);
                        else
                            bRet = LogBackend::instance(&a)->exportTypeLogsByCondition(outDir, type, period, "", keyword);
                    } else if (BOOT == flag) {
                        // 启动日志 可按状态导出
                        if (!period.isEmpty() || !level.isEmpty() || !event.isEmpty())
                            qCWarning(logAppMain) << QString("Export logs by %1, can only be filtered using 'status' parameter.").arg(type);
                        else
                            bRet = LogBackend::instance(&a)->exportTypeLogsByCondition(outDir, type, "", status, keyword);
                    } else if (TYPE_APP == type) {
                        if (!status.isEmpty() || !event.isEmpty()) {
                            qCWarning(logAppMain) << QString("Export logs by %1, can only be filtered using 'period' or 'level' parameter.").arg(type);
                        } else if (!appName.isEmpty()) {
                            bRet = LogBackend::instance(&a)->exportAppLogsByCondition(outDir, appName, period, level, keyword);
                        } else {
                            qCWarning(logAppMain) << QString("Export logs by %1, filterd by 'period' or 'level' or 'keyword', currently not supported.").arg(type).arg(appName);
                        }
                    } else if (TYPE_BSE == type || TYPE_AUDIT == type) {
                        // 开关机事件、审计日志 可按周期和事件类型导出
                        if (!level.isEmpty() || !status.isEmpty())
                            qCWarning(logAppMain) << QString("Export logs by %1, can only be filtered using 'period' or 'event' parameter.").arg(type);
                        else
                            bRet = LogBackend::instance(&a)->exportTypeLogsByCondition(outDir, type, period, event, keyword);
                    } else if (TYPE_XORG == type || TYPE_OTHER == type || TYPE_CUSTOM == type || TYPE_KWIN == type) {
                        // Xorg、Kwin 只能按关键字导出
                        if (!period.isEmpty() || !level.isEmpty() || !status.isEmpty() || !event.isEmpty())
                            qCWarning(logAppMain) << QString("Export logs by %1, can only be filtered using 'keyword' parameters.").arg(type);
                        else
                            bRet = LogBackend::instance(&a)->exportTypeLogsByCondition(outDir, type, period, event, keyword);
                    } else if (TYPE_OTHER == type || TYPE_CUSTOM == type) {
                        // 其他、自定义日志 不能按条件导出
                        if (!period.isEmpty() || !level.isEmpty() || !status.isEmpty() || !event.isEmpty())
                            qCWarning(logAppMain) << QString("Export logs by %1, cannot be filtered by any parameters.").arg(type);
                    } else {
                        qCWarning(logAppMain) << error;
                    }

                    if (!bRet)
                        return -1;
                }
            } else if (!appName.isEmpty()) {
                if (period.isEmpty() &&
                    level.isEmpty() &&
                    status.isEmpty() &&
                    event.isEmpty() &&
                    keyword.isEmpty()) {
                    int nRet = LogBackend::instance(&a)->exportAppLogs(outDir, appName);
                    return nRet;
                } else {
                    bool bRet = false;
                    if (!status.isEmpty() || !event.isEmpty())
                        qCWarning(logAppMain) << QString("Export app logs, can only be filtered using 'period' or 'level' parameter.");
                    else if (!period.isEmpty() || !level.isEmpty() || !keyword.isEmpty())
                        bRet = LogBackend::instance(&a)->exportAppLogsByCondition(outDir, appName, period, level, keyword);
                    if (!bRet)
                        return -1;
                }
            } else {
                if (!period.isEmpty() || !level.isEmpty() || !status.isEmpty() || !event.isEmpty() || !keyword.isEmpty()) {
                    qCWarning(logAppMain) << "Export all logs by conditons currently is not supported.";
                    return -1;
                }
                // 未指定类型，默认导出所有日志
                int nRet = LogBackend::instance(&a)->exportAllLogs(outDir);
                if (nRet != 0)
                    return nRet;
            }

            return a.exec();
        } else {
            qCWarning(logAppMain) <<"Missing export path, please enter the '-e' parameter.";
            return -1;
        }
    } else {

        PERF_PRINT_BEGIN("POINT-01", "");

        //klu下不使用opengl 使用OpenGLES,因为opengl基于x11 现在全面换wayland了
        QCoreApplication::setAttribute(Qt::AA_UseOpenGLES);
        //klu下不使用opengl 使用OpenGLES,因为opengl基于x11 现在全面换wayland了,这个真正有效
        qputenv("QT_WAYLAND_SHELL_INTEGRATION", "kwayland-shell");
        if (Utils::isWayland()) {
            qputenv("QT_WAYLAND_SHELL_INTEGRATION", "kwayland-shell");
        }
        setenv("PULSE_PROP_media.role", "video", 1);
        QSurfaceFormat format;
        format.setRenderableType(QSurfaceFormat::OpenGLES);
        format.setDefaultFormat(format);
        LogApplication a(argc, argv);

        qputenv("DTK_USE_SEMAPHORE_SINGLEINSTANCE", "1");

        //高分屏支持
        a.setAttribute(Qt::AA_UseHighDpiPixmaps);
        a.setAutoActivateWindows(true);
        a.loadTranslator();
        a.setOrganizationName("deepin");
        a.setApplicationName("deepin-log-viewer");
        a.setApplicationVersion(VERSION);
        a.setProductIcon(QIcon::fromTheme("deepin-log-viewer"));
        a.setWindowIcon(QIcon::fromTheme("deepin-log-viewer"));
        a.setProductName(DApplication::translate("Main", "Log Viewer"));
        a.setApplicationDisplayName(DApplication::translate("Main", "Log Viewer"));
        a.setApplicationDescription(
                    DApplication::translate("Main", "Log Viewer is a useful tool for viewing system logs."));

#if (DTK_VERSION >= DTK_VERSION_CHECK(5, 6, 8, 0))
        DLogManager::registerJournalAppender();
#ifdef QT_DEBUG
        DLogManager::registerConsoleAppender();
#endif
#else
        DLogManager::registerConsoleAppender();
        DLogManager::registerFileAppender();
#endif
        LogApplicationHelper::instance();

        if (!DGuiApplicationHelper::instance()->setSingleInstance(a.applicationName(),
                                                                  DGuiApplicationHelper::UserScope)) {
            qCCritical(logAppMain) << "DGuiApplicationHelper::instance()->setSingleInstance";
            a.activeWindow();
            return 0;
        }

        // 显示GUI
        LogCollectorMain w;
        a.setMainWindow(&w);

        // 自动化标记由此开始
        QAccessible::installFactory(accessibleFactory);

        //埋点记录启动数据
        QJsonObject objStartEvent{
            {"tid", Eventlogutils::StartUp},
            {"vsersion", VERSION},
            {"mode", 1},
        };
        Eventlogutils::GetInstance()->writeLogs(objStartEvent);

        w.show();
        Dtk::Widget::moveToCenter(&w);
        bool result = a.exec();
        PERF_PRINT_END("POINT-02", "");

        return  result;

    }
}
