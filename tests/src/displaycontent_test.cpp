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
#include "displaycontent_test.h"
#include "../application/displaycontent.h"
#include "../application/utils.h"
#include <DApplication>

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
    p->initConnections();
    delete  p;
}
class DisplayContent_generateJournalFile_UT_Param
{
public:
    DisplayContent_generateJournalFile_UT_Param(int id, int Iid)
    {
        time = id;
        level = Iid;
    }
    int time;
    int level;
};

class DisplayContent_generateJournalFile_UT : public ::testing::TestWithParam<DisplayContent_generateJournalFile_UT_Param>
{
};


INSTANTIATE_TEST_SUITE_P(DisplayContent, DisplayContent_generateJournalFile_UT, ::testing::Values(DisplayContent_generateJournalFile_UT_Param(0, -1)
                                                                                                  , DisplayContent_generateJournalFile_UT_Param(0, 0)
                                                                                                  , DisplayContent_generateJournalFile_UT_Param(0, 1)
                                                                                                  , DisplayContent_generateJournalFile_UT_Param(0, 2)
                                                                                                  , DisplayContent_generateJournalFile_UT_Param(0, 3)
                                                                                                  , DisplayContent_generateJournalFile_UT_Param(0, 4)
                                                                                                  , DisplayContent_generateJournalFile_UT_Param(0, 5)
                                                                                                  , DisplayContent_generateJournalFile_UT_Param(0, 6)
                                                                                                  , DisplayContent_generateJournalFile_UT_Param(0, 7)
                                                                                                  , DisplayContent_generateJournalFile_UT_Param(1, -1)
                                                                                                  , DisplayContent_generateJournalFile_UT_Param(1, 0)
                                                                                                  , DisplayContent_generateJournalFile_UT_Param(1, 1)
                                                                                                  , DisplayContent_generateJournalFile_UT_Param(1, 2)
                                                                                                  , DisplayContent_generateJournalFile_UT_Param(1, 3)
                                                                                                  , DisplayContent_generateJournalFile_UT_Param(1, 4)
                                                                                                  , DisplayContent_generateJournalFile_UT_Param(1, 5)
                                                                                                  , DisplayContent_generateJournalFile_UT_Param(1, 6)
                                                                                                  , DisplayContent_generateJournalFile_UT_Param(1, 7)
                                                                                                  , DisplayContent_generateJournalFile_UT_Param(2, -1)
                                                                                                  , DisplayContent_generateJournalFile_UT_Param(2, 0)
                                                                                                  , DisplayContent_generateJournalFile_UT_Param(2, 1)
                                                                                                  , DisplayContent_generateJournalFile_UT_Param(2, 2)
                                                                                                  , DisplayContent_generateJournalFile_UT_Param(2, 3)
                                                                                                  , DisplayContent_generateJournalFile_UT_Param(2, 4)
                                                                                                  , DisplayContent_generateJournalFile_UT_Param(2, 5)
                                                                                                  , DisplayContent_generateJournalFile_UT_Param(2, 6)
                                                                                                  , DisplayContent_generateJournalFile_UT_Param(2, 7)
                                                                                                  , DisplayContent_generateJournalFile_UT_Param(3, -1)
                                                                                                  , DisplayContent_generateJournalFile_UT_Param(3, 0)
                                                                                                  , DisplayContent_generateJournalFile_UT_Param(3, 1)
                                                                                                  , DisplayContent_generateJournalFile_UT_Param(3, 2)
                                                                                                  , DisplayContent_generateJournalFile_UT_Param(3, 3)
                                                                                                  , DisplayContent_generateJournalFile_UT_Param(3, 4)
                                                                                                  , DisplayContent_generateJournalFile_UT_Param(3, 5)
                                                                                                  , DisplayContent_generateJournalFile_UT_Param(3, 6)
                                                                                                  , DisplayContent_generateJournalFile_UT_Param(3, 7)
                                                                                                  , DisplayContent_generateJournalFile_UT_Param(4, -1)
                                                                                                  , DisplayContent_generateJournalFile_UT_Param(4, 0)
                                                                                                  , DisplayContent_generateJournalFile_UT_Param(4, 1)
                                                                                                  , DisplayContent_generateJournalFile_UT_Param(4, 2)
                                                                                                  , DisplayContent_generateJournalFile_UT_Param(4, 3)
                                                                                                  , DisplayContent_generateJournalFile_UT_Param(4, 4)
                                                                                                  , DisplayContent_generateJournalFile_UT_Param(4, 5)
                                                                                                  , DisplayContent_generateJournalFile_UT_Param(4, 6)
                                                                                                  , DisplayContent_generateJournalFile_UT_Param(4, 7)
                                                                                                  , DisplayContent_generateJournalFile_UT_Param(5, -1)
                                                                                                  , DisplayContent_generateJournalFile_UT_Param(5, 0)
                                                                                                  , DisplayContent_generateJournalFile_UT_Param(5, 1)
                                                                                                  , DisplayContent_generateJournalFile_UT_Param(5, 2)
                                                                                                  , DisplayContent_generateJournalFile_UT_Param(5, 3)
                                                                                                  , DisplayContent_generateJournalFile_UT_Param(5, 4)
                                                                                                  , DisplayContent_generateJournalFile_UT_Param(5, 5)
                                                                                                  , DisplayContent_generateJournalFile_UT_Param(5, 6)
                                                                                                  , DisplayContent_generateJournalFile_UT_Param(5, 7)

                                                                                                 ));

TEST_P(DisplayContent_generateJournalFile_UT, DisplayContent_generateJournalFile_UT_001)
{
    DisplayContent_generateJournalFile_UT_Param param = GetParam();
    DisplayContent *p = new DisplayContent(nullptr);
    EXPECT_NE(p, nullptr);
    p->generateJournalFile(param.time, param.level);
    p->deleteLater();
}
TEST(DisplayContent_createJournalTableStart_UT, DisplayContent_createJournalTableStart_UT_001)
{
    DisplayContent *p = new DisplayContent(nullptr);
    EXPECT_NE(p, nullptr);
    QList<LOG_MSG_JOURNAL> list;
    for (int i = 0; i < 100; ++i) {
        LOG_MSG_JOURNAL item;
        item.msg = "";
        item.level = "";
        item.daemonId = "";
        item.dateTime = "";
        item.hostName = "";
        item.daemonName = "";
        list.append(item);
    }
    p->createJournalTableStart(list);
    delete  p;
}
TEST(DisplayContent_createJournalTableForm_UT, DisplayContent_createJournalTableForm_UT_001)
{
    DisplayContent *p = new DisplayContent(nullptr);
    EXPECT_NE(p, nullptr);

    p->createJournalTableForm();
    QStringList  a;
    a << Dtk::Widget::DApplication::translate("Table", "Level")
      << Dtk::Widget::DApplication::translate("Table", "Process")  // modified by Airy
      << Dtk::Widget::DApplication::translate("Table", "Date and Time")
      << Dtk::Widget::DApplication::translate("Table", "Info")
      << Dtk::Widget::DApplication::translate("Table", "User")
      << Dtk::Widget::DApplication::translate("Table", "PID");
    bool rs = true;
    for (int i = 0; i < a.size(); ++i) {
        if (p->m_pModel->horizontalHeaderItem(i)->text() != a.value(i)) {
            rs = false;
        }
    }
    EXPECT_EQ(rs, true);
    delete  p;
}



class DisplayContent_generateDpkgFile_UT_Param
{
public:
    DisplayContent_generateDpkgFile_UT_Param(int id)
    {
        time = id;

    }
    int time;

};

class DisplayContent_generateDpkgFile_UT : public ::testing::TestWithParam<DisplayContent_generateDpkgFile_UT_Param>
{
};


INSTANTIATE_TEST_SUITE_P(DisplayContent, DisplayContent_generateDpkgFile_UT, ::testing::Values(DisplayContent_generateDpkgFile_UT_Param(0)
                                                                                               , DisplayContent_generateDpkgFile_UT_Param(1)
                                                                                               , DisplayContent_generateDpkgFile_UT_Param(2)
                                                                                               , DisplayContent_generateDpkgFile_UT_Param(3)
                                                                                               , DisplayContent_generateDpkgFile_UT_Param(4)
                                                                                               , DisplayContent_generateDpkgFile_UT_Param(5)
                                                                                              ));

TEST_P(DisplayContent_generateDpkgFile_UT, DisplayContent_generateDpkgFile_UT_001)
{
    DisplayContent_generateDpkgFile_UT_Param param = GetParam();
    DisplayContent *p = new DisplayContent(nullptr);
    EXPECT_NE(p, nullptr);
    Utils::sleep(200);
    p->generateDpkgFile(param.time);
    p->deleteLater();
}
TEST(DisplayContent_createDpkgTable_UT, DisplayContent_createDpkgTable_UT_001)
{
    DisplayContent *p = new DisplayContent(nullptr);
    EXPECT_NE(p, nullptr);

    QList<LOG_MSG_DPKG> list;
    for (int i = 0; i < 100; ++i) {
        LOG_MSG_DPKG item;
        item.msg = "";
        item.dateTime = "";
        item.action = "";
        list.append(item);
    }
    p->createDpkgTable(list);
    delete  p;
}

class DisplayContent_generateKernFile_UT_Param
{
public:
    DisplayContent_generateKernFile_UT_Param(int id)
    {
        time = id;

    }
    int time;

};

class DisplayContent_generateKernFile_UT : public ::testing::TestWithParam<DisplayContent_generateKernFile_UT_Param>
{
};


INSTANTIATE_TEST_SUITE_P(DisplayContent, DisplayContent_generateKernFile_UT, ::testing::Values(DisplayContent_generateKernFile_UT_Param(0)
                                                                                               , DisplayContent_generateKernFile_UT_Param(1)
                                                                                               , DisplayContent_generateKernFile_UT_Param(2)
                                                                                               , DisplayContent_generateKernFile_UT_Param(3)
                                                                                               , DisplayContent_generateKernFile_UT_Param(4)
                                                                                               , DisplayContent_generateKernFile_UT_Param(5)
                                                                                              ));

TEST_P(DisplayContent_generateKernFile_UT, DisplayContent_generateKernFile_UT_001)
{
    DisplayContent_generateKernFile_UT_Param param = GetParam();
    DisplayContent *p = new DisplayContent(nullptr);
    EXPECT_NE(p, nullptr);
    Utils::sleep(200);
    p->generateKernFile(param.time);
    p->deleteLater();
}


TEST(DisplayContent_createKernTableForm_UT, DisplayContent_createKernTableForm_UT_001)
{
    DisplayContent *p = new DisplayContent(nullptr);
    EXPECT_NE(p, nullptr);

    p->createKernTableForm();
    QStringList  a;
    a << Dtk::Widget::DApplication::translate("Table", "Date and Time")
      << Dtk::Widget::DApplication::translate("Table", "User")  // modified by Airy
      << Dtk::Widget::DApplication::translate("Table", "Process")
      << Dtk::Widget::DApplication::translate("Table", "Info");
    bool rs = true;
    for (int i = 0; i < a.size(); ++i) {
        if (p->m_pModel->horizontalHeaderItem(i)->text() != a.value(i)) {
            rs = false;
        }
    }
    EXPECT_EQ(rs, true);
    delete  p;
}




displaycontent_test::displaycontent_test()
{

}

void displaycontent_test::SetUp()
{

}

void displaycontent_test::TearDown()
{

}
