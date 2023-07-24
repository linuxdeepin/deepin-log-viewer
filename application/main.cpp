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

int main(int argc, char *argv[])
{
    //在root下或者非deepin/uos环境下运行不会发生异常，需要加上XDG_CURRENT_DESKTOP=Deepin环境变量；
    if (!QString(qgetenv("XDG_CURRENT_DESKTOP")).toLower().startsWith("deepin")) {
        setenv("XDG_CURRENT_DESKTOP", "Deepin", 1);
    }

    PERF_PRINT_BEGIN("POINT-01", "");

    //klu下不使用opengl 使用OpenGLES,因为opengl基于x11 现在全面换wayland了
    QCoreApplication::setAttribute(Qt::AA_UseOpenGLES);
    //klu下不使用opengl 使用OpenGLES,因为opengl基于x11 现在全面换wayland了,这个真正有效
    qputenv("QT_WAYLAND_SHELL_INTEGRATION", "kwayland-shell");
    QString  systemName =   DBusManager::getSystemInfo();
    qDebug() << "systemName" << systemName;
    if (Utils::isWayland()) {
        qputenv("QT_WAYLAND_SHELL_INTEGRATION", "kwayland-shell");
    }
    setenv("PULSE_PROP_media.role", "video", 1);
    QSurfaceFormat format;
    format.setRenderableType(QSurfaceFormat::OpenGLES);
    format.setDefaultFormat(format);
    LogApplication a(argc, argv);

    //wayland环境判断
    auto systemEnv = QProcessEnvironment::systemEnvironment();
    QString XDG_SESSION_TYPE = systemEnv.value(QStringLiteral("XDG_SESSION_TYPE"));
    QString WAYLAND_DISPLAY = systemEnv.value(QStringLiteral("WAYLAND_DISPLAY"));
    qDebug() << "XDG_SESSION_TYPE:" << XDG_SESSION_TYPE << "---WAYLAND_DISPLAY:" << WAYLAND_DISPLAY;
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
    DApplicationSettings settings;
    DLogManager::registerConsoleAppender();
    DLogManager::registerFileAppender();
    LogApplicationHelper::instance();

    //命令行参数的解析

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

        if (!DGuiApplicationHelper::instance()->setSingleInstance("deepin-logger",
                                                                  DGuiApplicationHelper::UserScope)) {
            qInfo() << "DGuiApplicationHelper::instance()->setSingleInstance";
            a.activeWindow();
            return 0;
        }

        QString outDir = "";

        // 若指定有导出目录，按指定目录导出，否则按默认路径导出
        if (!args.isEmpty())
            outDir = args.first();

        // 全部导出日志
        LogBackend::instance(&a)->exportAllLogs(outDir);

        return a.exec();
    } else {

        if (!DGuiApplicationHelper::instance()->setSingleInstance(a.applicationName(),
                                                                  DGuiApplicationHelper::UserScope)) {
            qInfo() << "DGuiApplicationHelper::instance()->setSingleInstance";
            a.activeWindow();
            return 0;
        }

        // 显示GUI
        LogCollectorMain w;
        a.setMainWindow(&w);

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
