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

#ifndef WTMPPARSE_H
#define WTMPPARSE_H

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include <utmp.h>
#include <QDebug>
#define NRECS 16
#define NULLUT ((struct utmp *)NULL)
#define UTSIZE (sizeof(struct utmp))

static char utmpbuf[NRECS * UTSIZE];
static int num_recs;
static int cur_rec;
static int fdWtmp = -1;
static int bpos;
static __off_t fpos;
struct utmp_list {
    struct utmp value;
    struct utmp_list *next;
};
int wtmp_open_back(char *filename);
int wtmp_open(char *filename);
int seek_end(void);
int wtmp_reload(void);
int wtmp_reload_back(void);
struct utmp *wtmp_next(void);
struct utmp *wtmp_back(void);
void wtmp_close(void);

struct utmp_list *st_list_init(void);

struct utmp *st_utmp_init(void);

void list_delete(struct utmp_list *list);

void list_insert(QList<utmp *> &plist, struct utmp *value);

utmp list_get_ele_and_del(QList<utmp > &list, char *value, int &rs);

char *show_end_time(long timeval);

char *show_start_time(long timeval);

void show_base_info(struct utmp *uBuf);

#endif  // WTMPPARSE_H
