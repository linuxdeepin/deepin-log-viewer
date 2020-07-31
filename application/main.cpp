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
#include "logapplicationhelper.h"
#include "logcollectormain.h"
#include "environments.h"
#include "accessible.h"

#include <DApplication>
#include <DApplicationSettings>
#include <DMainWindow>
#include <DWidgetUtil>
#include <DLog>

#include <QDateTime>

DWIDGET_USE_NAMESPACE
DCORE_USE_NAMESPACE

int main(int argc, char *argv[])
{
    DApplication a(argc, argv);

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
    DLogManager::registerConsoleAppender();
    DLogManager::registerFileAppender();
    LogApplicationHelper::instance();
    LogCollectorMain w;
    w.show();
    Dtk::Widget::moveToCenter(&w);
//    for (int i = 0; i < 120000; ++i) {
//        qDebug() << "fake log" << i;
//    }
    return a.exec();
}
