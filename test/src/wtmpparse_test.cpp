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
#include "wtmpparse.h"

#include <gtest/gtest.h>
#include <stub.h>

#include <QDebug>

TEST(wtmpparse_wtmp_open_UT, wtmpparse_wtmp_open_UT)
{
    int result = wtmp_open(QString(WTMP_FILE).toLatin1().data());
    EXPECT_NE(result, -1);
}

TEST(wtmpparse_wtmp_next_UT, wtmp_open_back_UT)
{
    int result = wtmp_open_back("test_test");
    EXPECT_EQ(result, -1);
}

TEST(wtmpparse_wtmp_next_UT, seek_end_UT)
{
    int result = seek_end();
    EXPECT_EQ(result, 0);
}

TEST(wtmpparse_wtmp_next_UT, wtmp_reload_back_UT)
{
    int result = wtmp_reload_back();
    EXPECT_NE(result, 0);
}

TEST(wtmpparse_wtmp_next_UT, wtmp_back_UT)
{
    struct utmp *test = wtmp_back();
    EXPECT_EQ(test, nullptr);
}

TEST(wtmpparse_wtmp_next_UT, show_end_time_UT)
{
    show_end_time(233);
}

TEST(wtmpparse_wtmp_next_UT, show_start_time_UT)
{
    show_start_time(233);
}
//TEST(wtmpparse_wtmp_reload_UT, wtmpparse_wtmp_reload_UT_001)
//{
//    int result = wtmp_reload();
//    EXPECT_NE(result, 0);
//}

//TEST(wtmpparse_wtmp_reload_UT, wtmpparse_wtmp_reload_UT_002)
//{
//    int result = wtmp_open(QString(WTMP_FILE).toLatin1().data());
//    EXPECT_NE(result, -1);
//    result = wtmp_reload();
//    EXPECT_NE(result, 0);
//}

TEST(wtmpparse_wtmp_next_UT, wtmpparse_wtmp_next_UT_001)
{
    wtmp_next();
}

TEST(wtmpparse_wtmp_close_UT, wtmpparse_wtmp_close_UT_001)
{
    int result = wtmp_open(QString(WTMP_FILE).toLatin1().data());
    EXPECT_NE(result, -1);
    wtmp_close();
}

TEST(wtmpparse_wtmp_close_UT, wtmpparse_wtmp_close_UT_002)
{
    wtmp_close();
}

TEST(wtmpparse_st_list_init_UT, wtmpparse_st_list_init_UT_001)
{
    st_list_init();
}

TEST(wtmpparse_st_utmp_init_UT, wtmpparse_st_utmp_init_UT_001)
{
    st_utmp_init();
}

TEST(wtmpparse_list_insert_UT, wtmpparse_list_insert_UT_001)
{
    QList<utmp *> plist;
    utmp *info = (struct utmp *)malloc(sizeof(struct utmp));
    list_insert(plist, info);
    EXPECT_EQ(plist.size(), 1);
    free(info);
}

TEST(wtmpparse_list_delete_UT, wtmpparse_list_delete_UT)
{
    utmp_list *list = st_list_init();
    utmp info;
    list->value = info;
    utmp_list *list2 = st_list_init();
    list2->value = info;
    list->next = list2;
    list_delete(list);
}

TEST(wtmpparse_list_get_ele_and_del_UT, wtmpparse_list_get_ele_and_del_UT)
{
    QList<utmp> list;
    utmp info1;
    strcpy(info1.ut_line, "test1");
    list.append(info1);
    utmp info2;
    strcpy(info2.ut_line, "test2");
    list.append(info2);
    int result = -1;

    utmp info3 = list_get_ele_and_del(list, "test1", result);
    qDebug() << "info3.ut_line" << info3.ut_line;
    EXPECT_EQ(result, 0);
    EXPECT_EQ(QString(info3.ut_line) == QString("test1"), true);
    EXPECT_EQ(list.size(), 1);
}

//TEST(wtmpparse_show_end_time_UT, wtmpparse_show_end_time_UT_001)
//{
//    EXPECT_EQ(QString(show_end_time(1603705035)) == QString("Mon Oct 26 17:37:15 2020\n"), true);
//}

//TEST(wtmpparse_show_start_time_UT, wtmpparse_show_start_time_UT)
//{
//    EXPECT_EQ(QString(show_start_time(1603705164)) == QString("Mon Oct 26 17:39:24 2020\n"), true);
//}

TEST(wtmpparse_show_base_info_UT, wtmpparse_show_base_info_UT)
{
    utmp info1;
    strcpy(info1.ut_line, "test");
    show_base_info(&info1);
}
