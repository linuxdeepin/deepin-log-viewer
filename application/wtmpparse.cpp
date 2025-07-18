// SPDX-FileCopyrightText: 2019 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "wtmpparse.h"
#include <QDebug>
#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(logApp)

int wtmp_open(char *filename)
{
    qCDebug(logApp) << "Opening wtmp file:" << filename;
    fdWtmp = open(filename, O_RDONLY);
    if (fdWtmp == -1) {
        qCWarning(logApp) << "Failed to open wtmp file:" << filename << "error:" << strerror(errno);
    }
    cur_rec = num_recs = 0;
    return fdWtmp;
}


int wtmp_open_back(char *filename)
{
    qCDebug(logApp) << "Opening wtmp file for backward reading:" << filename;
    fdWtmp = open(filename, O_RDONLY);
    bpos = cur_rec = num_recs = 0;
    fpos = lseek(fdWtmp, 0, SEEK_END);
    qCDebug(logApp) << "File position set to end:" << fpos;
    return fdWtmp;
}

int seek_end(void)
{
    // qCDebug(logApp) << "Seeking to end of file";
    return 0;
}

int wtmp_reload(void)
{
    qCDebug(logApp) << "Reloading wtmp data";
    int amt_read;
    amt_read = read(fdWtmp, utmpbuf, NRECS * UTSIZE);
    if (amt_read == -1) {
        qCWarning(logApp) << "Failed to read wtmp data, error:" << strerror(errno);
        return 0;
    }
    num_recs = amt_read / UTSIZE;
    qCDebug(logApp) << "Read" << num_recs << "wtmp records";

    cur_rec = 0;

    return num_recs;
}


struct utmp *wtmp_next(void)
{
    qCDebug(logApp) << "Getting next wtmp record";
    struct utmp *recp;

    if (fdWtmp == -1) {
        qCWarning(logApp) << "wtmp file descriptor is invalid";
        return NULLUT;
    }

    if (cur_rec == num_recs && wtmp_reload() == 0) {
        qCDebug(logApp) << "No more records to read";
        return NULLUT;
    }

    recp = (struct utmp *)&utmpbuf[cur_rec * UTSIZE];
    cur_rec++;

    return recp;
}


int wtmp_reload_back(void)
{
    qCDebug(logApp) << "Reloading wtmp data backwards";
    int amt_read;

    off_t o;
    o = ((fpos - 1) / (NRECS * UTSIZE)) * (NRECS * UTSIZE);
    bpos = (int)(fpos - o);
    fpos -= NRECS * UTSIZE;
    amt_read = pread(fdWtmp, utmpbuf, NRECS * UTSIZE, fpos);
    num_recs = amt_read / UTSIZE;
    qCDebug(logApp) << "Read" << num_recs << "wtmp records backwards";

    cur_rec = 0;

    return num_recs;
}
struct utmp *wtmp_back(void)
{
    qCDebug(logApp) << "Getting previous wtmp record";
    struct utmp *recp;

    if (fdWtmp == -1) {
        qCWarning(logApp) << "wtmp file descriptor is invalid";
        return NULLUT;
    }

    if (cur_rec == 0 && wtmp_reload_back() == 0) {
        qCDebug(logApp) << "No more records to read backwards";
        return NULLUT;
    }

    recp = (struct utmp *)&utmpbuf[cur_rec * UTSIZE];
    cur_rec--;

    return recp;
}

void wtmp_close(void)
{
    qCDebug(logApp) << "Closing wtmp file";
    if (fdWtmp != -1) {
        close(fdWtmp);
        qCDebug(logApp) << "wtmp file closed successfully";
    } else {
        qCDebug(logApp) << "wtmp file was already closed";
    }
}

struct utmp_list *st_list_init(void)
{
    qCDebug(logApp) << "Initializing utmp list";
    struct utmp_list *list = static_cast<struct utmp_list *>(malloc(sizeof(struct utmp_list)));
    if (!list) {
        qCCritical(logApp) << "Failed to allocate memory for utmp_list";
        return NULL;
    }

    memset(list, 0, sizeof(struct utmp_list));

    return list;
}

struct utmp *st_utmp_init(void)
{
    qCDebug(logApp) << "Initializing utmp structure";
    struct utmp *stUTMP = static_cast<struct utmp *>(malloc(sizeof(struct utmp)));
    if (!stUTMP) {
        qCCritical(logApp) << "Failed to allocate memory for utmp structure";
        printf("struct utmp malloc failed\n");
        return NULL;
    }

    memset(stUTMP, 0, sizeof(struct utmp));
    qCDebug(logApp) << "utmp structure initialized successfully";

    return stUTMP;
}

utmp_list * list_delete(struct utmp_list *list)
{
    qCDebug(logApp) << "Deleting utmp list";
    int count = 0;
    while (list->next) {
        struct utmp_list *p = list;
        list = p->next;
        free(p);
        count++;
    }
    qCDebug(logApp) << "Deleted" << count << "list items";
    return list;
}

void list_insert(QList<utmp *> &plist, struct utmp *value)
{
    qCDebug(logApp) << "Inserting utmp entry into list";
    utmp v = *value;
    utmp *value_ = &(v);
    plist.append(value_);
    qCDebug(logApp) << "List size after insert:" << plist.size();
}

utmp list_get_ele_and_del(QList<utmp > &list, char *value, int &rs)
{
    qCDebug(logApp) << "Getting and deleting element from list, searching for:" << value;
    utmp temp = {};
    for (int i = 0; i < list.length(); ++i) {
        utmp itemValue = list.value(i);
        QString a(itemValue.ut_line);
        QString b(value);
        int result = QString::compare(a, b);
        if (result == 0) {
            qCDebug(logApp) << "Found matching element at index:" << i;
            list.removeAt(i);
            rs = 0;
            return itemValue;
        }
    }
    qCDebug(logApp) << "Element not found in list";
    rs = -1;
    return temp;
}

char *show_end_time(time_t timeval)
{
    qCDebug(logApp) << "Formatting end time for timestamp:" << timeval;
    struct tm *t;
    char tt[256] = {0};

    t = localtime(&timeval);
    strftime(tt, sizeof(tt) - 1, "%R", t);
    //printf("%s", tt);
    return asctime(t);
}

char *show_start_time(time_t timeval)
{
    qCDebug(logApp) << "Formatting start time for timestamp:" << timeval;
    struct tm *t;
    char tt[256] = {0};

    t = localtime(&timeval);
    strftime(tt, sizeof(tt) - 1, "%a %b %e %R", t);
    //    printf("%s", tt);
    //    printf(" - ");

    return asctime(t);
}
void show_base_info(struct utmp *uBuf)
{
    qCDebug(logApp) << "Showing base info for utmp entry:"
                    << "user:" << uBuf->ut_name
                    << "line:" << uBuf->ut_line
                    << "host:" << uBuf->ut_host
                    << "type:" << uBuf->ut_type;
                    
    printf("%-9.8s", uBuf->ut_name);

    if (uBuf->ut_type == BOOT_TIME)
        printf("%-13s", "system boot");
    else
        printf("%-13.8s", uBuf->ut_line);

    printf("%-17.16s", uBuf->ut_host);
}

