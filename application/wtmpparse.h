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

struct utmp_list {
    struct utmp value;
    struct utmp_list *next;
};

int wtmp_open(char *filename)
{
    fdWtmp = open(filename, O_RDONLY);
    cur_rec = num_recs = 0;
    return fdWtmp;
}

int wtmp_reload(void)
{
    int amt_read;
    amt_read = read(fdWtmp, utmpbuf, NRECS * UTSIZE);
    num_recs = amt_read / UTSIZE;

    cur_rec = 0;

    return num_recs;
}

struct utmp *wtmp_next(void)
{
    struct utmp *recp;

    if (fdWtmp == -1)
        return NULLUT;

    if (cur_rec == num_recs && wtmp_reload() == 0)
        return NULLUT;

    recp = (struct utmp *)&utmpbuf[cur_rec * UTSIZE];
    cur_rec++;

    return recp;
}

void wtmp_close(void)
{
    if (fdWtmp != -1)
        close(fdWtmp);
}

struct utmp_list *st_list_init(void)
{
    struct utmp_list *list = (struct utmp_list *)malloc(sizeof(struct utmp_list));
    if (!list) {
        printf("struct utmp_list malloc failed\n");
        return NULL;
    }

    memset(list, 0, sizeof(struct utmp_list));

    return list;
}

struct utmp *st_utmp_init(void)
{
    struct utmp *stUTMP = (struct utmp *)malloc(sizeof(struct utmp));
    if (!stUTMP) {
        printf("struct utmp malloc failed\n");
        return NULL;
    }

    memset(stUTMP, 0, sizeof(struct utmp));

    return stUTMP;
}
void list_delete(struct utmp_list *list)
{
    struct utmp_list *pList = list;
    while (pList->next) {
        struct utmp_list *p = pList;
        pList = pList->next;
        free(p);
    }
}
void list_insert(QList<utmp *> plist, struct utmp *value)
{
    utmp v = *value;
    utmp *value_ = &(v);
    plist.append(value_);

}

utmp list_get_ele_and_del(QList<utmp > &list, char *value, int &rs)
{
    utmp temp = {};
    for (int i = 0; i < list.length(); ++i) {
        utmp itemValue = list.value(i);
        QString a(itemValue.ut_line);
        QString b(value);
        int result = QString::compare(a, b);
        if (result == 0) {
            list.removeAt(i);
            rs = 0;
            return itemValue;
        }
    }
    rs = -1;
    return temp;
}

char *show_end_time(long timeval)
{
    struct tm *t;
    char tt[12] = {0};

    t = localtime(&timeval);
    strftime(tt, 100, "%R", t);
    //printf("%s", tt);
    return asctime(t);
}

char *show_start_time(long timeval)
{
    struct tm *t;
    char tt[32] = {0};

    t = localtime(&timeval);
    strftime(tt, 100, "%a %b %e %R", t);
//    printf("%s", tt);
//    printf(" - ");

    return asctime(t);
}

void show_base_info(struct utmp *uBuf)
{
    printf("%-9.8s", uBuf->ut_name);

    if (uBuf->ut_type == BOOT_TIME)
        printf("%-13s", "system boot");
    else
        printf("%-13.8s", uBuf->ut_line);

    printf("%-17.16s", uBuf->ut_host);
}

#endif  // WTMPPARSE_H
