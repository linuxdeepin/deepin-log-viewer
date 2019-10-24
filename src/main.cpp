#include <DApplication>
#include <DApplicationSettings>
#include <DMainWindow>
#include <DWidgetUtil>
#include "logcollectormain.h"
#include "logpasswordauth.h"

#include <DLog>

DWIDGET_USE_NAMESPACE
DCORE_USE_NAMESPACE

int main(int argc, char *argv[])
{
    DApplication::loadDXcbPlugin();
    DApplication a(argc, argv);
    a.setAttribute(Qt::AA_UseHighDpiPixmaps);

    a.loadTranslator();
    a.setOrganizationName("deepin");
    a.setApplicationName("dde-log-viewer");
    a.setApplicationVersion(DApplication::buildVersion("20191016"));
    a.setProductIcon(QIcon("://images/logo.svg"));
    a.setWindowIcon(QIcon("://images/logo.svg"));
    a.setProductName(DApplication::translate("Main", "Deepin Log Viewer"));
    a.setApplicationDescription(DApplication::translate(
        "Main", "Deepin Log Viewer is a small tool for viewing system logs."));
    DApplicationSettings settings;
    //    a.setTheme("dark");
    DLogManager::registerConsoleAppender();
    DLogManager::registerFileAppender();

    LogCollectorMain w;
    w.setMinimumSize(1000, 600);
    w.show();

    Dtk::Widget::moveToCenter(&w);

    return a.exec();
}
