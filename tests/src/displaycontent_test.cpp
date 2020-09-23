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
#include "../application/displaycontent.h"
#include <DApplication>
#include <gtest/src/stub.h>
TEST(DisplayContent_Constructor_UT, DisplayContent_Constructor_UT_001)
{
    DisplayContent *p = new DisplayContent(nullptr);
    EXPECT_NE(p, nullptr);
    delete  p;
}


TEST(DisplayContent_Destructor_UT, DisplayContent_Destructor_UT_001)
{
    DisplayContent *p = new DisplayContent(nullptr);
    EXPECT_NE(p, nullptr);
    p->~DisplayContent();
    EXPECT_EQ(p->m_treeView, nullptr);
    EXPECT_EQ(p->m_pModel, nullptr);
    //  delete  p;
}


TEST(DisplayContent_MainLogTableView_UT, DisplayContent_MainLogTableView_UT_001)
{
    DisplayContent *p = new DisplayContent(nullptr);
    EXPECT_NE(p, nullptr);
    LogTreeView *t =  p->mainLogTableView();
    EXPECT_NE(t, nullptr);
    delete  p;
}

TEST(DisplayContent_initUI_UT, DisplayContent_initUI_UT_001)
{
    DisplayContent *p = new DisplayContent(nullptr);
    EXPECT_NE(p, nullptr);
    p->initUI();
    EXPECT_NE(p->noResultLabel, nullptr);
    delete  p;
}


TEST(DisplayContent_initMap_UT, DisplayContent_initMap_UT_001)
{
    DisplayContent *p = new DisplayContent(nullptr);
    EXPECT_NE(p, nullptr);
    p->initMap();
    QMap<QString, QString> map1;
    map1.insert("Warning", Dtk::Widget::DApplication::translate("Level", "Warning"));  //add by Airy for bug 19167 and 19161
    map1.insert("Debug", Dtk::Widget::DApplication::translate("Level", "Debug")); //add by Airy for bug 19167 and 19161
    map1.insert("Info", Dtk::Widget::DApplication::translate("Level", "Info"));
    map1.insert("Error", Dtk::Widget::DApplication::translate("Level", "Error"));
    QMap<QString, QString> map2;
    map2.insert(Dtk::Widget::DApplication::translate("Level", "Emergency"), "warning2.svg");
    map2.insert(Dtk::Widget::DApplication::translate("Level", "Alert"), "warning3.svg");
    map2.insert(Dtk::Widget::DApplication::translate("Level", "Critical"), "warning2.svg");
    map2.insert(Dtk::Widget::DApplication::translate("Level", "Error"), "wrong.svg");
    map2.insert(Dtk::Widget::DApplication::translate("Level", "Warning"), "warning.svg");
    map2.insert(Dtk::Widget::DApplication::translate("Level", "Notice"), "warning.svg");
    map2.insert(Dtk::Widget::DApplication::translate("Level", "Info"), "");
    map2.insert(Dtk::Widget::DApplication::translate("Level", "Debug"), "");
    map2.insert("Warning", "warning.svg");
    map2.insert("Debug", "");
    map2.insert("Error", "wrong.svg");
    EXPECT_EQ(p->m_transDict, map1);
    EXPECT_EQ(p->m_icon_name_map, map2);
    delete  p;
}
TEST(DisplayContent_initTableView_UT, DisplayContent_initTableView_UT_001)
{
    DisplayContent *p = new DisplayContent(nullptr);
    EXPECT_NE(p, nullptr);
    p->initTableView();
    EXPECT_NE(p->m_treeView, nullptr);
    EXPECT_NE(p->m_pModel, nullptr);
    EXPECT_EQ(p->m_treeView->model(), p->m_pModel);
    EXPECT_EQ(p->m_treeView->objectName(), "mainLogTable");
    EXPECT_EQ(p->m_treeView->accessibleName(), "main_log_table");
    delete  p;
}

TEST(DisplayContent_initConnections_UT, DisplayContent_initConnections_UT_001)
{
    DisplayContent *p = new DisplayContent(nullptr);
    EXPECT_NE(p, nullptr);
    p->generateJournalFile(0, 0);
    bool rs = p->m_journalCurrentIndex >= 0;
    EXPECT_EQ(rs, true);
    delete  p;
}
