/*
* Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
*
* Author:     zyc <zyc@uniontech.com>
*
* Maintainer:  zyc <zyc@uniontech.com>
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

#include "logapplication.h"
#include "logcollectormain.h"

#include <DWidgetUtil>

#include <QKeyEvent>
#include <QDebug>
#include <QEvent>

/**
 * @brief LogApplication::LogApplication 构造函数
 * @param argc 参数个数
 * @param argv 参数数组指针
 */
LogApplication::LogApplication(int &argc, char **argv): DApplication(argc, argv)
{

}

/**
 * @brief LogApplication::setMainWindow 设置mainwindow
 * @param iMainWindow mainwindow指针
 */
void LogApplication::setMainWindow(LogCollectorMain *iMainWindow)
{
    m_mainWindow = iMainWindow;
}
