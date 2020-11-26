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
#ifndef STUBALLTHREAD_H
#define STUBALLTHREAD_H
#include "structdef.h"

#include <QThread>
#include <QThreadPool>
#include <QDebug>
void QThreadPool_start(QRunnable *runnable, int priority = 0);

void QThread_start(QThread::Priority = QThread::InheritPriority);
int LogFileParser_parseByJournal(QStringList arg = QStringList());
int LogFileParser_parseByJournalBoot(QStringList arg = QStringList());

void LogFileParser_parseByDpkg(DKPG_FILTERS &iDpkgFilter);
void LogFileParser_parseByXlog(XORG_FILTERS &iXorgFilter);
void LogFileParser_parseByBoot();
void LogFileParser_parseByKern(KERN_FILTERS &iKernFilter);
void LogFileParser_parseByApp(APP_FILTERS &iAPPFilter);
void LogFileParser_parseByNormal(QList<LOG_MSG_NORMAL> &nList, NORMAL_FILTERS &iNormalFiler);  // add by Airy
void LogFileParser_parseByKwin(KWIN_FILTERS iKwinfilter);
void Doc_Document_save(const QString &path);
void QRunnable_run();
#endif // STUBALLTHREAD_H
