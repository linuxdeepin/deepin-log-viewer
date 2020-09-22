/*
* Copyright (C) 2019 ~ 2020 UnionTech Software Technology Co.,Ltd
*
* Author:     zyc <zyc@uniontech.com>
* Maintainer:  zyc <zyc@uniontech.com>
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* any later version.
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "logapplication.h"
#include "logcollectormain.h"

#include <DWidgetUtil>

#include <QKeyEvent>
#include <QDebug>
#include <QEvent>

LogApplication::LogApplication(int &argc, char **argv): DApplication(argc, argv)
{

}

void LogApplication::setMainWindow(LogCollectorMain *iMainWindow)
{
    m_mainWindow = iMainWindow;
}

bool LogApplication::notify(QObject *obj, QEvent *evt)
{
    switch (evt->type()) {
    case QEvent::KeyPress: {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(evt);
        if (keyEvent->key() == Qt::Key_Tab || keyEvent->key() == Qt::Key_Backtab) {
            if (m_mainWindow) {
                bool rs =  m_mainWindow->handleApplicationTabEventNotify(obj, keyEvent);
                if (rs) {
                    return rs;
                } else {
                    return  DApplication::notify(obj, evt) ;
                }
            }
        }
        break;
    }
    }
    return  DApplication::notify(obj, evt) ;
}
