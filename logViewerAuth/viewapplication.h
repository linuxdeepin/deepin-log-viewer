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

#ifndef VIEWAPPLICATION_H
#define VIEWAPPLICATION_H
#include <QCoreApplication>
class QProcess;
class QSharedMemory;
class ViewApplication : public QCoreApplication
{
    struct ShareMemoryInfo {
        bool isStart = true ;
    };
public:
    ViewApplication(int &argc, char **argv);
    ~ViewApplication();
    QProcess *m_proc;
    QSharedMemory *m_commondM;
public slots:
    void releaseMemery();
    ShareMemoryInfo getControlInfo();

};

#endif // VIEWAPPLICATION_H
