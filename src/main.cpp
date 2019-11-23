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

#include <DApplication>
#include <DApplicationSettings>
#include <DMainWindow>
#include <DWidgetUtil>
#include "logapplicationhelper.h"
#include "logcollectormain.h"
#include "logpasswordauth.h"

#include <DLog>
#include <QDateTime>

DWIDGET_USE_NAMESPACE
DCORE_USE_NAMESPACE

int main(int argc, char *argv[])
{
    DApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    DApplication::loadDXcbPlugin();
    DApplication a(argc, argv);
    a.setAttribute(Qt::AA_UseHighDpiPixmaps);

    a.loadTranslator();
    a.setOrganizationName("deepin");
    a.setApplicationName("deepin-log-viewer");
    a.setApplicationVersion(DApplication::buildVersion(""));
    a.setProductIcon(QIcon::fromTheme("deepin-log-viewer"));
    a.setWindowIcon(QIcon::fromTheme("deepin-log-viewer"));
    a.setProductName(DApplication::translate("Main", "Log Viewer"));
    a.setApplicationDescription(
        DApplication::translate("Main", "Log Viewer is a useful tool for viewing system logs."));
    DApplicationSettings settings;
    DLogManager::registerConsoleAppender();
    DLogManager::registerFileAppender();

    if (!DGuiApplicationHelper::instance()->setSingleInstance("deepin-log-viewer"))
        return 0;

    LogApplicationHelper::instance();

    LogCollectorMain w;
    w.setMinimumSize(958, 736);
    w.show();

    Dtk::Widget::moveToCenter(&w);

    return a.exec();
}
