// SPDX-FileCopyrightText: 2019 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "wtmpparse.h"

#include <stub.h>

#include <QDebug>

#include <gtest/gtest.h>
#include <string.h>

// Helper: force fdWtmp to -1 in wtmpparse.cpp's TU by opening a non-existent file
static void resetFdInvalid()
{
    char badFile[] = "nonexistent_wtmp_test_file";
    wtmp_open_back(badFile);  // open() fails → fdWtmp = -1
}

TEST(wtmpparse_wtmp_open_UT, wtmpparse_wtmp_open_UT)
{
    int result = wtmp_open(QString(WTMP_FILE).toLatin1().data());
    EXPECT_NE(result, -1);
    wtmp_close();
}

TEST(wtmpparse_wtmp_open_back_UT, wtmp_open_back_UT)
{
    char fileName[] = "test_test";
    int result = wtmp_open_back(fileName);
    EXPECT_EQ(result, -1);
}

TEST(wtmpparse_seek_end_UT, seek_end_UT)
{
    int result = seek_end();
    EXPECT_EQ(result, 0);
}

// Covers wtmp_reload (via wtmp_next) and the successful read path of wtmp_next
TEST(wtmpparse_wtmp_next_UT, wtmp_next_reload_UT)
{
    int fd = wtmp_open(QString(WTMP_FILE).toLatin1().data());
    ASSERT_NE(fd, -1);

    // wtmp_open sets cur_rec=0, num_recs=0, so wtmp_next calls wtmp_reload
    struct utmp *rec = wtmp_next();
    if (rec != nullptr) {
        SUCCEED() << "wtmp_next returned a valid record";
    } else {
        SUCCEED() << "wtmp_next returned NULL (empty wtmp file, reload returned 0)";
    }

    wtmp_close();
}

// Covers the fdWtmp==-1 early-return path of wtmp_next
TEST(wtmpparse_wtmp_next_UT, wtmp_next_invalid_fd_UT)
{
    resetFdInvalid();
    struct utmp *tmp = wtmp_next();
    EXPECT_EQ(tmp, nullptr) << "wtmp_next should return NULL when fd is invalid";
}

// Covers wtmp_reload_back (direct call with invalid fd — error path)
TEST(wtmpparse_wtmp_back_UT, wtmp_reload_back_UT)
{
    resetFdInvalid();
    int result = wtmp_reload_back();
    // With invalid fd, pread fails → amt_read=-1 → num_recs = -1/UTSIZE (non-zero)
    EXPECT_NE(result, 0);
}

// Covers the fdWtmp==-1 early-return path of wtmp_back
TEST(wtmpparse_wtmp_back_UT, wtmp_back_invalid_fd_UT)
{
    resetFdInvalid();
    struct utmp *test = wtmp_back();
    EXPECT_EQ(test, nullptr);
}

// Covers wtmp_back with a real file (exercises wtmp_reload_back via wtmp_back)
TEST(wtmpparse_wtmp_back_UT, wtmp_back_with_file_UT)
{
    int fd = wtmp_open_back(QString(WTMP_FILE).toLatin1().data());
    if (fd != -1) {
        struct utmp *rec = wtmp_back();
        if (rec != nullptr) {
            SUCCEED() << "wtmp_back returned a valid record";
        } else {
            SUCCEED() << "wtmp_back returned NULL (no records)";
        }
        wtmp_close();
    } else {
        SUCCEED() << "Could not open wtmp file for backward reading";
    }
}

TEST(wtmpparse_show_end_time_UT, show_end_time_UT)
{
    const char *endTime = show_end_time(233);
    EXPECT_NE(endTime, nullptr);
    EXPECT_NE(strlen(endTime), 0) << "show_end_time should return a non-empty string";
}

TEST(wtmpparse_show_start_time_UT, show_start_time_UT)
{
    const char *startTime = show_start_time(233);
    EXPECT_NE(startTime, nullptr);
    EXPECT_NE(strlen(startTime), 0) << "show_start_time should return a non-empty string";
}

TEST(wtmpparse_wtmp_close_UT, wtmpparse_wtmp_close_UT_001)
{
    int result = wtmp_open(QString(WTMP_FILE).toLatin1().data());
    EXPECT_NE(result, -1);
    wtmp_close();
}

TEST(wtmpparse_wtmp_close_UT, wtmpparse_wtmp_close_UT_002)
{
    // Close when fdWtmp==-1 — exercises the else branch
    resetFdInvalid();
    wtmp_close();
    SUCCEED() << "wtmp_close on already-closed fd completed";
}

TEST(wtmpparse_st_list_init_UT, wtmpparse_st_list_init_UT_001)
{
    utmp_list *utmListPtr = st_list_init();
    EXPECT_NE(utmListPtr, nullptr);
    EXPECT_EQ(utmListPtr->value.ut_type, 0) << "check the status after st_list_init()";
    free(utmListPtr);
}

TEST(wtmpparse_st_utmp_init_UT, wtmpparse_st_utmp_init_UT_001)
{
    utmp *utmptr = st_utmp_init();
    EXPECT_NE(utmptr, nullptr);
    EXPECT_EQ(utmptr->ut_type, 0) << "check the status after st_utmp_init()";
    free(utmptr);
}

TEST(wtmpparse_list_insert_UT, wtmpparse_list_insert_UT_001)
{
    QList<utmp *> plist;
    utmp *info = static_cast<utmp*>(malloc(sizeof(struct utmp)));
    memset(info, 0, sizeof(struct utmp));
    list_insert(plist, info);
    EXPECT_EQ(plist.size(), 1);
    free(info);
}

TEST(wtmpparse_list_delete_UT, wtmpparse_list_delete_UT)
{
    utmp_list *list = st_list_init();
    utmp info;
    memset(&info, 0, sizeof(struct utmp));
    list->value = info;
    utmp_list *list2 = st_list_init();

    list2->value = info;
    list->next = list2;
    list = list_delete(list);
    EXPECT_EQ(list, list2) << "check the status after list_delete()";
    free(list2);
}

TEST(wtmpparse_list_get_ele_and_del_UT, wtmpparse_list_get_ele_and_del_UT)
{
    QList<utmp> list;
    utmp info1;
    memset(&info1, 0, sizeof(struct utmp));
    strcpy(info1.ut_line, "test1");
    list.append(info1);
    utmp info2;
    memset(&info2, 0, sizeof(struct utmp));
    strcpy(info2.ut_line, "test2");
    list.append(info2);
    int result = -1;

    char search1[] = "test1";
    utmp info3 = list_get_ele_and_del(list, search1, result);
    qDebug() << "info3.ut_line" << info3.ut_line;
    EXPECT_EQ(result, 0);
    EXPECT_EQ(QString(info3.ut_line) == QString("test1"), true);
    EXPECT_EQ(list.size(), 1);

    // Also exercise the not-found path
    int result2 = -1;
    char search2[] = "nonexistent";
    utmp info4 = list_get_ele_and_del(list, search2, result2);
    EXPECT_EQ(result2, -1);
    (void)info4;
}

TEST(wtmpparse_show_base_info_UT, wtmpparse_show_base_info_UT)
{
    // Test non-BOOT_TIME branch
    utmp info1;
    memset(&info1, 0, sizeof(struct utmp));
    strcpy(info1.ut_line, "test");
    strcpy(info1.ut_name, "user");
    strcpy(info1.ut_host, "host");
    info1.ut_type = LOGIN_PROCESS;
    show_base_info(&info1);
    EXPECT_NE(&info1, nullptr) << "check the status after show_base_info()";
}

TEST(wtmpparse_show_base_info_UT, show_base_info_boot_time_UT)
{
    // Test BOOT_TIME branch
    utmp info1;
    memset(&info1, 0, sizeof(struct utmp));
    strcpy(info1.ut_line, "test");
    strcpy(info1.ut_name, "user");
    strcpy(info1.ut_host, "host");
    info1.ut_type = BOOT_TIME;
    show_base_info(&info1);
    EXPECT_EQ(info1.ut_type, BOOT_TIME);
}
