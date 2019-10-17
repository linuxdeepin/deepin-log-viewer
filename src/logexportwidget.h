#ifndef LOGEXPORTWIDGET_H
#define LOGEXPORTWIDGET_H

#include <DWidget>
#include <QObject>
#include <QStandardItemModel>
#include "structdef.h"

class LogExportWidget : public Dtk::Widget::DWidget
{
public:
    LogExportWidget(QWidget *parent = nullptr);

    static bool exportToTxt(QString fileName, QStandardItemModel *pModel);
    static bool exportToTxt(QString fileName, QList<LOG_MSG_JOURNAL> jList);

    static bool exportToDoc(QString fileName, QStandardItemModel *pModel);
    static bool exportToDoc(QString fileName, QList<LOG_MSG_JOURNAL> jList, QStringList labels);

    static bool exportToHtml(QString fileName, QStandardItemModel *pModel);
    static bool exportToHtml(QString fileName, QList<LOG_MSG_JOURNAL> jList);

    static bool exportToXls(QString fileName, QStandardItemModel *pModel);
    static bool exportToXls(QString fileName, QList<LOG_MSG_JOURNAL> jList, QStringList labels);
};

#endif  // LOGEXPORTWIDGET_H
