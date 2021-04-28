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
#ifndef LOGAPPLICATION_H
#define LOGAPPLICATION_H
#include <DApplication>

DWIDGET_USE_NAMESPACE
class QKeyEvent;
class LogCollectorMain;
/**
 * @brief The LogApplication class 主application类
 */
class LogApplication: public DApplication
{
public:
    LogApplication(int &argc, char **argv);
    void setMainWindow(LogCollectorMain *iMainWindow);
protected:
    bool notify(QObject *obj, QEvent *event) Q_DECL_OVERRIDE;
private:
    //mainwindow指针
    LogCollectorMain *m_mainWindow{nullptr};
};

#endif // LOGAPPLICATION_H
