// SPDX-FileCopyrightText: 2019 - 2022 UnionTech Software Technology Co., Ltd.
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
DWIDGET_USE_NAMESPACE
DCORE_USE_NAMESPACE

Q_LOGGING_CATEGORY(test, "main.app.test.module")

int main(int argc, char *argv[])
{
    //在root下或者非deepin/uos环境下运行不会发生异常，需要加上XDG_CURRENT_DESKTOP=Deepin环境变量；
    if (!QString(qgetenv("XDG_CURRENT_DESKTOP")).toLower().startsWith("deepin")) {
        setenv("XDG_CURRENT_DESKTOP", "Deepin", 1);
    }

    qCDebug(test) << "just test";
    // 命令参数大于1，进行命令行处理
    if (argc > 1) {
        QCoreApplication a(argc, argv);
        a.setOrganizationName("deepin");
        a.setApplicationName("deepin-log-viewer");

#if (DTK_VERSION >= DTK_VERSION_CHECK(5, 6, 5, 0))
        DLogManager::registerJournalAppender();
#ifdef QT_DEBUG
        DLogManager::registerConsoleAppender();
#endif
#else
        DLogManager::registerConsoleAppender();
        DLogManager::registerFileAppender();
#endif

        QCommandLineOption exportOption(QStringList() << "e" << "export", DApplication::translate("main", "export logs"));

        QCommandLineParser cmdParser;
        cmdParser.addPositionalArgument("outDir", DApplication::translate("main", "export logs directory. if not set, default export to ~/Document."), "[outDir...]");
        cmdParser.setApplicationDescription("deepin-log-viewer");
        cmdParser.addHelpOption();
        cmdParser.addVersionOption();
        cmdParser.addOption(exportOption);

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

            QString outDir = "";
            // 若指定有导出目录，按指定目录导出，否则按默认路径导出
            if (!args.isEmpty())
                outDir = args.first();

            // 根据当前用户名获取正确家目录路径
            Utils::homePath = Utils::getHomePath(argv[argc - 1]);
            // 设置命令行工作目录
            LogBackend::instance(&a)->setCmdWorkDir(argv[argc - 2]);

            // 全部导出日志
            LogBackend::instance(&a)->exportAllLogs(outDir);

            return a.exec();
        }

        return a.exec();
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

#if (DTK_VERSION >= DTK_VERSION_CHECK(5, 6, 5, 0))
        qCDebug(test) << "just test 2";
        DLogManager::registerJournalAppender();
#ifdef QT_DEBUG
        DLogManager::registerConsoleAppender();
#endif

        qCDebug(test) << "just test 3";
#else
        DLogManager::registerConsoleAppender();
        DLogManager::registerFileAppender();
#endif
        LogApplicationHelper::instance();

        if (!DGuiApplicationHelper::instance()->setSingleInstance(a.applicationName(),
                                                                  DGuiApplicationHelper::UserScope)) {
            qCritical() << "DGuiApplicationHelper::instance()->setSingleInstance";
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
