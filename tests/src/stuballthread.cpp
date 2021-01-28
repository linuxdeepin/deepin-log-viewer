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

#include "stuballthread.h"
void QThread_start(QThread::Priority)
{
    qDebug() << "QThread_start--";
}

void QThreadPool_start(QRunnable *runnable, int priority)
{
    qDebug() << "QThreadPool_start--";
}

int LogFileParser_parseByJournal(QStringList arg)
{
    return 0;
}

int LogFileParser_parseByJournalBoot(QStringList arg)
{
    return 0;
}

void LogFileParser_parseByDpkg(DKPG_FILTERS &iDpkgFilter)
{
}

void LogFileParser_parseByXlog(XORG_FILTERS &iXorgFilter)
{
}

void LogFileParser_parseByBoot()
{
}

void LogFileParser_parseByKern(KERN_FILTERS &iKernFilter)
{
}

void LogFileParser_parseByApp(APP_FILTERS &iAPPFilter)
{
}

void LogFileParser_parseByNormal(QList<LOG_MSG_NORMAL> &nList, NORMAL_FILTERS &iNormalFiler)
{
}

void LogFileParser_parseByKwin(KWIN_FILTERS iKwinfilter)
{
}

void QRunnable_run()
{
}

void Doc_Document_save(const QString &path)
{
}
