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

#include <stub.h>
#include "journalwork.h"
#include "stuballthread.h"

#include <gtest/gtest.h>

#include <DApplication>

#include <QDebug>
TEST(journalWork_Constructor_UT, journalWork_Constructor_UT_001)
{
    journalWork *p = new journalWork(nullptr);
    EXPECT_NE(p, nullptr);
    p->deleteLater();
}
TEST(journalWork_Constructor_UT, journalWork_Constructor_UT_002)
{
    journalWork *p = new journalWork(QStringList() << "", nullptr);
    EXPECT_NE(p, nullptr);
    p->deleteLater();
}

TEST(journalWork_Constructor_UT, journalWork_Constructor_UT_003)
{
    journalWork *p = new journalWork(QStringList() << "PRIORITY=1", nullptr);
    EXPECT_NE(p, nullptr);
    EXPECT_EQ(p->m_arg.contains("PRIORITY=1"), true);
    p->deleteLater();
}

TEST(journalWork_Destructor_UT, journalWork_Destructor_UT_001)
{
    journalWork *p = new journalWork(nullptr);
    EXPECT_NE(p, nullptr);
    p->~journalWork();
    EXPECT_EQ(p->logList.isEmpty(), true);
    EXPECT_EQ(p->m_map.isEmpty(), true);
    //  delete  p;
}

TEST(journalWork_stopWork_UT, journalWork_stopWork_UT_001)
{
    journalWork *p = new journalWork(nullptr);
    EXPECT_NE(p, nullptr);
    p->stopWork();
    EXPECT_EQ(p->m_canRun, false);
    p->deleteLater();
}

TEST(journalWork_getIndex_UT, journalWork_getIndex_UT_001)
{
    journalWork *p = new journalWork(nullptr);
    EXPECT_NE(p, nullptr);
    EXPECT_EQ(p->getIndex(), p->getPublicIndex());
    p->deleteLater();
}
TEST(journalWork_setArg_UT, journalWork_setArg_UT_001)
{
    journalWork *p = new journalWork(nullptr);
    EXPECT_NE(p, nullptr);
    QStringList testArg;
    testArg << "PRIORITY=1";
    p->setArg(testArg);
    EXPECT_EQ(p->m_arg.contains("PRIORITY=1"), true);
    p->deleteLater();
}

TEST(journalWork_setArg_UT, journalWork_setArg_UT_002)
{
    journalWork *p = new journalWork(nullptr);
    EXPECT_NE(p, nullptr);
    QStringList testArg;
    p->setArg(testArg);
    EXPECT_EQ(p->m_arg.isEmpty(), true);
    p->deleteLater();
}

TEST(journalWork_doWork_UT, journalWork_doWork_UT)
{
    journalWork *p = new journalWork(nullptr);
    p->doWork();
    p->deleteLater();
}

TEST(journalWork_getReplaceColorStr_UT, journalWork_getReplaceColorStr_UT)
{
    journalWork *p = new journalWork(nullptr);
    EXPECT_EQ(p->getReplaceColorStr("\033[40;37m"), "");
    p->deleteLater();
}

TEST(journalWork_getDateTimeFromStamp_UT, journalWork_getDateTimeFromStamp_UT)
{
    journalWork *p = new journalWork(nullptr);
    p->getDateTimeFromStamp("1603160995838863") == QString("2020-10-20 10:29:55");
    //  EXPECT_EQ(rs, true);
    p->deleteLater();
}

TEST(journalWork_initMap_UT, journalWork_initMap_UT)
{
    journalWork *p = new journalWork(nullptr);
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

TEST(journalWork_i2str_UT, journalWork_i2str_UT)
{
    journalWork *p = new journalWork(nullptr);
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
