/*
 * Copyright (C) 2019 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     LZ <zhou.lu@archermind.com>
 *
 * Maintainer: LZ <zhou.lu@archermind.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

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

    static bool exportToTxt(QString fileName, QStandardItemModel *pModel, LOG_FLAG flag);
    static bool exportToTxt(QString fileName, QList<LOG_MSG_JOURNAL> jList);
    static bool exportToTxt(QString fileName, QList<LOG_MSG_JOURNAL> jList, QStringList labels);
    static bool exportToTxt(QString fileName, QList<LOG_MSG_APPLICATOIN> jList, QStringList labels);
    static bool exportToTxt(QString fileName, QList<LOG_MSG_DPKG> jList, QStringList labels);
    static bool exportToTxt(QString fileName, QList<LOG_MSG_BOOT> jList, QStringList labels);
    static bool exportToTxt(QString fileName, QList<LOG_MSG_XORG> jList, QStringList labels);
    static bool exportToTxt(QString fileName, QList<LOG_MSG_NORMAL> jList, QStringList labels);



    static bool exportToDoc(QString fileName, QStandardItemModel *pModel, LOG_FLAG flag);
    static bool exportToDoc(QString fileName, QList<LOG_MSG_JOURNAL> jList, QStringList labels, LOG_FLAG iFlag);
    static bool exportToDoc(QString fileName, QList<LOG_MSG_APPLICATOIN> jList, QStringList labels);
    static bool exportToDoc(QString fileName, QList<LOG_MSG_DPKG> jList, QStringList labels);
    static bool exportToDoc(QString fileName, QList<LOG_MSG_BOOT> jList, QStringList labels);
    static bool exportToDoc(QString fileName, QList<LOG_MSG_XORG> jList, QStringList labels);
    static bool exportToDoc(QString fileName, QList<LOG_MSG_NORMAL> jList, QStringList labels);

    static bool exportToHtml(QString fileName, QStandardItemModel *pModel, LOG_FLAG flag);
    static bool exportToHtml(QString fileName, QList<LOG_MSG_JOURNAL> jList);
    static bool exportToHtml(QString fileName, QList<LOG_MSG_JOURNAL> jList, QStringList labels);
    static bool exportToHtml(QString fileName, QList<LOG_MSG_APPLICATOIN> jList, QStringList labels);
    static bool exportToHtml(QString fileName, QList<LOG_MSG_DPKG> jList, QStringList labels);
    static bool exportToHtml(QString fileName, QList<LOG_MSG_BOOT> jList, QStringList labels);
    static bool exportToHtml(QString fileName, QList<LOG_MSG_XORG> jList, QStringList labels);
    static bool exportToHtml(QString fileName, QList<LOG_MSG_NORMAL> jList, QStringList labels);

    static bool exportToXls(QString fileName, QStandardItemModel *pModel, LOG_FLAG flag);
    static bool exportToXls(QString fileName, QList<LOG_MSG_JOURNAL> jList, QStringList labels, LOG_FLAG iFlag);
    static bool exportToXls(QString fileName, QList<LOG_MSG_APPLICATOIN> jList, QStringList labels);
    static bool exportToXls(QString fileName, QList<LOG_MSG_DPKG> jList, QStringList labels);
    static bool exportToXls(QString fileName, QList<LOG_MSG_BOOT> jList, QStringList labels);
    static bool exportToXls(QString fileName, QList<LOG_MSG_XORG> jList, QStringList labels);
    static bool exportToXls(QString fileName, QList<LOG_MSG_NORMAL> jList, QStringList labels);
};

#endif  // LOGEXPORTWIDGET_H
