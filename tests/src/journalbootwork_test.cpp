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
#include <gtest/gtest.h>
#include <stub.h>
#include "journalbootwork.h"
#include "stuballthread.h"
#include <systemd/sd-journal.h>

#include <DApplication>

#include <QDebug>
#include <QThreadPool>
#include <QThread>

int stub_sd_journal_get_data_bootwork(sd_journal *j, const char *field, const void **data, size_t *l)
{
    return 0;
}

QString stub_getReplaceColorStr_bootwork(const char *d)
{
    return "testaaaa";
}
TEST(JournalBootWork_Constructor_UT, JournalBootWork_Constructor_UT_001)
{
    JournalBootWork *p = new JournalBootWork(nullptr);
    EXPECT_NE(p, nullptr);
    p->deleteLater();
}
TEST(JournalBootWork_Constructor_UT, JournalBootWork_Constructor_UT_002)
{
    JournalBootWork *p = new JournalBootWork(QStringList() << "", nullptr);
    EXPECT_NE(p, nullptr);
    p->deleteLater();
}

TEST(JournalBootWork_Constructor_UT, JournalBootWork_Constructor_UT_003)
{
    JournalBootWork *p = new JournalBootWork(QStringList() << "PRIORITY=1", nullptr);
    EXPECT_NE(p, nullptr);
    EXPECT_EQ(p->m_arg.contains("PRIORITY=1"), true);
    p->deleteLater();
}

TEST(JournalBootWork_Destructor_UT, JournalBootWork_Destructor_UT_001)
{
    JournalBootWork *p = new JournalBootWork(nullptr);
    EXPECT_NE(p, nullptr);
    p->~JournalBootWork();
    EXPECT_EQ(p->logList.isEmpty(), true);
    EXPECT_EQ(p->m_map.isEmpty(), true);
    //  delete  p;
}

TEST(JournalBootWork_stopWork_UT, JournalBootWork_stopWork_UT_001)
{
    JournalBootWork *p = new JournalBootWork(nullptr);
    EXPECT_NE(p, nullptr);
    p->stopWork();
    EXPECT_EQ(p->m_canRun, false);
    p->deleteLater();
}

TEST(JournalBootWork_getIndex_UT, JournalBootWork_getIndex_UT_001)
{
    JournalBootWork *p = new JournalBootWork(nullptr);
    EXPECT_NE(p, nullptr);
    EXPECT_EQ(p->getIndex(), p->getPublicIndex());
    p->deleteLater();
}
TEST(JournalBootWork_setArg_UT, JournalBootWork_setArg_UT_001)
{
    JournalBootWork *p = new JournalBootWork(nullptr);
    EXPECT_NE(p, nullptr);
    QStringList testArg;
    testArg << "PRIORITY=1";
    p->setArg(testArg);
    EXPECT_EQ(p->m_arg.contains("PRIORITY=1"), true);
    p->deleteLater();
}

TEST(JournalBootWork_setArg_UT, JournalBootWork_setArg_UT_002)
{
    JournalBootWork *p = new JournalBootWork(nullptr);
    EXPECT_NE(p, nullptr);
    QStringList testArg;
    p->setArg(testArg);
    EXPECT_EQ(p->m_arg.isEmpty(), true);
    p->deleteLater();
}

TEST(JournalBootWork_doWork_UT, JournalBootWork_doWork_UT)
{
    Stub stub;
    stub.set(sd_journal_get_data, stub_sd_journal_get_data_bootwork);
    stub.set(ADDR(JournalBootWork, getReplaceColorStr), stub_getReplaceColorStr_bootwork);
    JournalBootWork *p = new JournalBootWork(nullptr);
    p->doWork();
    p->deleteLater();
}

TEST(JournalBootWork_getReplaceColorStr_UT, JournalBootWork_getReplaceColorStr_UT)
{
    JournalBootWork *p = new JournalBootWork(nullptr);
    EXPECT_EQ(p->getReplaceColorStr("\033[40;37m"), "");
    p->deleteLater();
}

TEST(JournalBootWork_getDateTimeFromStamp_UT, JournalBootWork_getDateTimeFromStamp_UT)
{
    JournalBootWork *p = new JournalBootWork(nullptr);
    p->getDateTimeFromStamp("1603160995838863") == QString("2020-10-20 10:29:55");
    //  EXPECT_EQ(rs, true);
    p->deleteLater();
}

TEST(JournalBootWork_initMap_UT, JournalBootWork_initMap_UT)
{
    JournalBootWork *p = new JournalBootWork(nullptr);
    p->initMap();
    QMap<int, QString> map;
    map.insert(0, Dtk::Widget::DApplication::translate("Level", "Emergency"));
    map.insert(1, Dtk::Widget::DApplication::translate("Level", "Alert"));
    map.insert(2, Dtk::Widget::DApplication::translate("Level", "Critical"));
    map.insert(3, Dtk::Widget::DApplication::translate("Level", "Error"));
    map.insert(4, Dtk::Widget::DApplication::translate("Level", "Warning"));
    map.insert(5, Dtk::Widget::DApplication::translate("Level", "Notice"));
    map.insert(6, Dtk::Widget::DApplication::translate("Level", "Info"));
    map.insert(7, Dtk::Widget::DApplication::translate("Level", "Debug"));
    EXPECT_EQ(map, p->m_map);
    p->deleteLater();
}

TEST(JournalBootWork_i2str_UT, JournalBootWork_i2str_UT)
{
    JournalBootWork *p = new JournalBootWork(nullptr);
    QMap<int, QString> map;
    map.insert(0, Dtk::Widget::DApplication::translate("Level", "Emergency"));
    map.insert(1, Dtk::Widget::DApplication::translate("Level", "Alert"));
    map.insert(2, Dtk::Widget::DApplication::translate("Level", "Critical"));
    map.insert(3, Dtk::Widget::DApplication::translate("Level", "Error"));
    map.insert(4, Dtk::Widget::DApplication::translate("Level", "Warning"));
    map.insert(5, Dtk::Widget::DApplication::translate("Level", "Notice"));
    map.insert(6, Dtk::Widget::DApplication::translate("Level", "Info"));
    map.insert(7, Dtk::Widget::DApplication::translate("Level", "Debug"));
    foreach (int key, map.keys()) {
        EXPECT_EQ(p->i2str(key), map.value(key));
    }

    p->deleteLater();
}

int stub_sd_journal_open(sd_journal **ret, int flags)
{
    return -1;
}

//int stub_sd_journal_seek_tail(sd_journal *j)
//{
//}

class JournalBootWork_UT : public testing::Test
{
public:
    //添加日志
    static void SetUpTestCase()
    {
        qDebug() << "SetUpTestCase" << endl;
    }
    static void TearDownTestCase()
    {
        qDebug() << "TearDownTestCase" << endl;
    }
    void SetUp() //TEST跑之前会执行SetUp
    {
        m_bootWork = new JournalBootWork(nullptr);
        qDebug() << "SetUp" << endl;
    }
    void TearDown() //TEST跑完之后会执行TearDown
    {
        delete m_bootWork;
    }
    JournalBootWork *m_bootWork;
};

//TEST_F(JournalBootWork_UT, testBootWork_UT)
//{
//    m_bootWork->doWork();
//}

TEST_F(JournalBootWork_UT, testBootWork_UT001)
{
    Stub stub;
    stub.set(sd_journal_open, stub_sd_journal_open);
    m_bootWork->doWork();
    m_bootWork->m_canRun = false;
    m_bootWork->doWork();
}

//TEST_F(JournalBootWork_UT, testBootWork_UT002)
//{
//    Stub stub;
//    stub.set(sd_journal_seek_tail, stub_sd_journal_seek_tail);
//    m_bootWork->doWork();
//}
