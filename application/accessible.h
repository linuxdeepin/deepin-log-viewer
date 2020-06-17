#ifndef ACCESSIBLE_H
#define ACCESSIBLE_H

#include "accessibledefine.h"
#include "logcollectormain.h"

DWIDGET_USE_NAMESPACE
//using namespace DCC_NAMESPACE;

// 主窗口
SET_FORM_ACCESSIBLE(LogCollectorMain, "LogCollectorMain")

QAccessibleInterface *accessibleFactory(const QString &classname, QObject *object)
{
    QAccessibleInterface *interface = nullptr;
    // 应用主窗口
    USE_ACCESSIBLE(QString(classname).replace("dccV20::", ""), LogCollectorMain);

    return interface;
}
#endif // ACCESSIBLE_H
