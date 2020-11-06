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

#ifndef VIEWAPPLICATION_H
#define VIEWAPPLICATION_H
#include <QCoreApplication>
class QTimer;
class QProcess;
class QSharedMemory;
class QFile;
class ViewApplication : public QCoreApplication
{

public:
    ViewApplication(int &argc, char **argv);
    ~ViewApplication();

public slots:
    void dataRecived();
    bool doReadFileWork(const QString &iFilePath);
    void close();
    void checkClose();
private:

    QProcess *m_proc;


    QFile *mFile;
    char *mMem;
    qint64 mSize;
    QTimer *m_timer;

    QList<QFile *> mFiles;
    QList<char *> mMems;
    QList<qint64> mSizes;


};

#endif // VIEWAPPLICATION_H
