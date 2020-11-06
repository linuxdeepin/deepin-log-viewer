
#include "../application/logapplicationhelper.h"
#include "../application/logcollectormain.h"
#include "../application/logapplication.h"
#include "../application/environments.h"
#include "../application/accessible.h"
#include "../application/dbusmanager.h"
#include "../application/utils.h"

#include <DApplication>
#include <DApplicationSettings>
#include <DMainWindow>
#include <DWidgetUtil>
#include <DLog>

#include <QTest>
#include <QApplication>
#include <QDateTime>
#include <QSurfaceFormat>

#include <gtest/gtest.h>



QT_BEGIN_NAMESPACE
QTEST_ADD_GPU_BLACKLIST_SUPPORT_DEFS
QT_END_NAMESPACE

int main(int argc, char *argv[])
{
    //klu下不使用opengl 使用OpenGLES,因为opengl基于x11 现在全面换wayland了
    QCoreApplication::setAttribute(Qt::AA_UseOpenGLES);
    //klu下不使用opengl 使用OpenGLES,因为opengl基于x11 现在全面换wayland了,这个真正有效
    qputenv("QT_WAYLAND_SHELL_INTEGRATION", "kwayland-shell");
    QString  systemName =   DBusManager::getSystemInfo();
    qDebug() << "systemName" << systemName;
    if (systemName == "klu" || systemName == "panguV") {
        qputenv("_d_disableDBusFileDialog", "true");
    }
    if (Utils::isWayland()) {
        qputenv("QT_WAYLAND_SHELL_INTEGRATION", "kwayland-shell");
    }


    setenv("PULSE_PROP_media.role", "video", 1);
    QSurfaceFormat format;
    format.setRenderableType(QSurfaceFormat::OpenGLES);
    format.setDefaultFormat(format);
    LogApplication a(argc, argv);

    //  wayland环境判断
    auto systemEnv = QProcessEnvironment::systemEnvironment();
    QString XDG_SESSION_TYPE = systemEnv.value(QStringLiteral("XDG_SESSION_TYPE"));
    QString WAYLAND_DISPLAY = systemEnv.value(QStringLiteral("WAYLAND_DISPLAY"));
    qDebug() << "XDG_SESSION_TYPE:" << XDG_SESSION_TYPE << "---WAYLAND_DISPLAY:" << WAYLAND_DISPLAY;
    qputenv("DTK_USE_SEMAPHORE_SINGLEINSTANCE", "1");
    if (!DGuiApplicationHelper::instance()->setSingleInstance(a.applicationName(),
                                                              DGuiApplicationHelper::UserScope)) {
        qDebug() << "DGuiApplicationHelper::instance()->setSingleInstance";
        exit(0);
    }
    //高分屏支持
    a.setAttribute(Qt::AA_UseHighDpiPixmaps);
    a.loadTranslator();
    a.setOrganizationName("deepin");
    a.setApplicationName("deepin-log-viewer");
    a.setApplicationVersion(VERSION);
    a.setProductIcon(QIcon::fromTheme("deepin-log-viewer"));
    a.setWindowIcon(QIcon::fromTheme("deepin-log-viewer"));
    a.setProductName(DApplication::translate("Main", "Log Viewer"));
    //  a.setAttribute(Qt::AA_SynthesizeTouchForUnhandledMouseEvents, 0);
    //a.setAttribute(Qt::AA_SynthesizeMouseForUnhandledTouchEvents, 0);
    a.setApplicationDescription(
        DApplication::translate("Main", "Log Viewer is a useful tool for viewing system logs."));
    DApplicationSettings settings;
    LogApplicationHelper::instance();
    QAccessible::installFactory(accessibleFactory);

//    LogCollectorMain w;
//    a.setMainWindow(&w);


//    w.show();
    //  Dtk::Widget::moveToCenter(&w);
//    for (int i = 0; i < 120000; ++i) {
//        qDebug() << "fake log" << i;
//    }


//   QTEST_DISABLE_KEYPAD_NAVIGATION
    //  QTEST_ADD_GPU_BLACKLIST_SUPPORT
    // a.exec();
    testing::InitGoogleTest();
    int ret = RUN_ALL_TESTS();

#ifdef ENABLE_UI_TEST
//    app.exec();
    QTest::qExec(&app, argc, argv);
#endif

    QTEST_SET_MAIN_SOURCE_PATH

    return ret;
}
