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
#include <gtest/src/stub.h>
#include <DApplication>
#include <QDebug>
#include <QFileDialog>
#include <QPaintEvent>
#include <QThreadPool>
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
void parseByKern(void *obj, KERN_FILTERS &iKernFilter)
{
    qDebug() << "parseByKern---";
}
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
    Stub stub;
    stub.set(ADDR(LogFileParser, parseByKern), parseByKern);
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


TEST(DisplayContent_createKernTable_UT, DisplayContent_createKernTable_UT_001)
{
    DisplayContent *p = new DisplayContent(nullptr);
    EXPECT_NE(p, nullptr);
    QList<LOG_MSG_JOURNAL> list;
    LOG_MSG_JOURNAL item;
    for (int i = 0; i < 100; ++i) {
        item.msg = QString("msg%1").arg(i);
        item.level = "Debug";
        item.daemonId = "1";
        item.dateTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
        item.hostName = "test_host";
        item.daemonName = "test_daemon";
        list.append(item);
    }
    p->createKernTable(list);
    p->deleteLater();
}


TEST(DisplayContent_insertKernTable_UT, DisplayContent_insertKernTable_UT_001)
{
    DisplayContent *p = new DisplayContent(nullptr);
    EXPECT_NE(p, nullptr);
    QList<LOG_MSG_JOURNAL> list;
    LOG_MSG_JOURNAL item;
    for (int i = 0; i < 100; ++i) {
        item.msg = QString("msg%1").arg(i);
        item.level = "Debug";
        item.daemonId = "1";
        item.dateTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
        item.hostName = "test_host";
        item.daemonName = "test_daemon";
        list.append(item);
    }
    p->insertKernTable(list, 0, list.size());
    p->deleteLater();
}

class DisplayContent_generateAppFile_UT_Param
{
public:
    DisplayContent_generateAppFile_UT_Param(int id)
    {
        time = id;

    }
    int time;

};

class DisplayContent_generateAppFile_UT : public ::testing::TestWithParam<DisplayContent_generateAppFile_UT_Param>
{
};


INSTANTIATE_TEST_SUITE_P(DisplayContent, DisplayContent_generateAppFile_UT, ::testing::Values(DisplayContent_generateAppFile_UT_Param(0)
                                                                                              , DisplayContent_generateAppFile_UT_Param(1)
                                                                                              , DisplayContent_generateAppFile_UT_Param(2)
                                                                                              , DisplayContent_generateAppFile_UT_Param(3)
                                                                                              , DisplayContent_generateAppFile_UT_Param(4)
                                                                                              , DisplayContent_generateAppFile_UT_Param(5)
                                                                                             ));

TEST_P(DisplayContent_generateAppFile_UT, DisplayContent_generateAppFile_UT_001)
{
    DisplayContent_generateAppFile_UT_Param param = GetParam();
    DisplayContent *p = new DisplayContent(nullptr);
    EXPECT_NE(p, nullptr);
    Utils::sleep(200);
    p->generateAppFile("", param.time, 7, "");
    p->deleteLater();
}

TEST(DisplayContent_createAppTableForm_UT, DisplayContent_createAppTableForm_UT_001)
{
    DisplayContent *p = new DisplayContent(nullptr);
    EXPECT_NE(p, nullptr);

    p->createAppTableForm();
    QStringList  a;
    a << Dtk::Widget::DApplication::translate("Table", "Level")
      << Dtk::Widget::DApplication::translate("Table", "Date and Time")  // modified by Airy
      << Dtk::Widget::DApplication::translate("Table", "Source")
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



TEST(DisplayContent_createAppTable_UT, DisplayContent_createAppTable_UT_001)
{
    DisplayContent *p = new DisplayContent(nullptr);
    EXPECT_NE(p, nullptr);
    QList<LOG_MSG_APPLICATOIN> list;
    LOG_MSG_APPLICATOIN item;
    for (int i = 0; i < 100; ++i) {
        item.msg = QString("msg%1").arg(i);
        item.level = "Debug";
        item.dateTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
        item.src = "test_src";
        list.append(item);
    }
    p->createAppTable(list);
    p->deleteLater();
}
TEST(DisplayContent_createBootTable_UT, DisplayContent_createBootTable_UT_001)
{
    DisplayContent *p = new DisplayContent(nullptr);
    EXPECT_NE(p, nullptr);
    QList<LOG_MSG_BOOT> list;
    LOG_MSG_BOOT item;
    for (int i = 0; i < 100; ++i) {
        item.msg = QString("msg%1").arg(i);
        item.status = "OK";
        list.append(item);
    }
    p->createBootTable(list);
    p->deleteLater();
}

TEST(DisplayContent_createXorgTable_UT, DisplayContent_createXorgTable_UT_001)
{
    DisplayContent *p = new DisplayContent(nullptr);
    EXPECT_NE(p, nullptr);
    QList<LOG_MSG_XORG> list;
    LOG_MSG_XORG item;
    for (int i = 0; i < 100; ++i) {
        item.msg = QString("msg%1").arg(i);
        item.dateTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
        list.append(item);
    }
    p->createXorgTable(list);
    p->deleteLater();
}

class DisplayContent_generateXorgFile_UT_Param
{
public:
    DisplayContent_generateXorgFile_UT_Param(int id)
    {
        time = id;

    }
    int time;

};

class DisplayContent_generateXorgFile_UT : public ::testing::TestWithParam<DisplayContent_generateXorgFile_UT_Param>
{
};


INSTANTIATE_TEST_SUITE_P(DisplayContent, DisplayContent_generateXorgFile_UT, ::testing::Values(DisplayContent_generateXorgFile_UT_Param(0)
                                                                                               , DisplayContent_generateXorgFile_UT_Param(1)
                                                                                               , DisplayContent_generateXorgFile_UT_Param(2)
                                                                                               , DisplayContent_generateXorgFile_UT_Param(3)
                                                                                               , DisplayContent_generateXorgFile_UT_Param(4)
                                                                                               , DisplayContent_generateXorgFile_UT_Param(5)
                                                                                              ));

TEST_P(DisplayContent_generateXorgFile_UT, DisplayContent_generateXorgFile_UT_001)
{
    DisplayContent_generateXorgFile_UT_Param param = GetParam();
    DisplayContent *p = new DisplayContent(nullptr);
    EXPECT_NE(p, nullptr);
    Utils::sleep(200);
    p->generateXorgFile(param.time);
    p->deleteLater();
}

TEST(DisplayContent_creatKwinTable_UT, DisplayContent_creatKwinTable_UT_001)
{
    DisplayContent *p = new DisplayContent(nullptr);
    EXPECT_NE(p, nullptr);
    QList<LOG_MSG_KWIN> list;
    LOG_MSG_KWIN item;
    for (int i = 0; i < 100; ++i) {
        item.msg = QString("msg%1").arg(i);
        list.append(item);
    }
    p->creatKwinTable(list);
    p->deleteLater();
}

void QThreadPool_start(QRunnable *runnable, int priority = 0)
{
    qDebug() << "QThreadPool_sstart";
}
TEST(DisplayContent_generateKwinFile_UT, DisplayContent_generateKwinFile_UT_001)
{
    DisplayContent *p = new DisplayContent(nullptr);
    EXPECT_NE(p, nullptr);
    KWIN_FILTERS f;
    f.msg = "";
    Stub *stub = new Stub;
    stub->set(ADDR(QThreadPool, start), QThreadPool_start);
    p->generateKwinFile(f);
    p->deleteLater();
}


TEST(DisplayContent_createNormalTable_UT, DisplayContent_createNormalTable_UT_001)
{
    DisplayContent *p = new DisplayContent(nullptr);
    EXPECT_NE(p, nullptr);
    QList<LOG_MSG_NORMAL> list;
    LOG_MSG_NORMAL item;
    for (int i = 0; i < 100; ++i) {
        item.msg = QString("msg%1").arg(i);
        item.dateTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
        item.userName = "test_user";
        item.eventType = "Login";
        list.append(item);
    }
    p->createNormalTable(list);
    p->deleteLater();
}

class DisplayContent_generateNormalFile_UT_Param
{
public:
    DisplayContent_generateNormalFile_UT_Param(int id)
    {
        time = id;

    }
    int time;

};

class DisplayContent_generateNormalFile_UT : public ::testing::TestWithParam<DisplayContent_generateNormalFile_UT_Param>
{
};


INSTANTIATE_TEST_SUITE_P(DisplayContent, DisplayContent_generateNormalFile_UT, ::testing::Values(DisplayContent_generateNormalFile_UT_Param(0)
                                                                                                 , DisplayContent_generateNormalFile_UT_Param(1)
                                                                                                 , DisplayContent_generateNormalFile_UT_Param(2)
                                                                                                 , DisplayContent_generateNormalFile_UT_Param(3)
                                                                                                 , DisplayContent_generateNormalFile_UT_Param(4)
                                                                                                 , DisplayContent_generateNormalFile_UT_Param(5)
                                                                                                ));

TEST_P(DisplayContent_generateNormalFile_UT, DisplayContent_generateNormalFile_UT_001)
{
    DisplayContent_generateNormalFile_UT_Param param = GetParam();
    DisplayContent *p = new DisplayContent(nullptr);
    EXPECT_NE(p, nullptr);
    Utils::sleep(200);
    p->generateNormalFile(param.time);
    p->deleteLater();
}



TEST(DisplayContent_insertJournalTable_UT, DisplayContent_insertJournalTable_UT_001)
{
    DisplayContent *p = new DisplayContent(nullptr);
    EXPECT_NE(p, nullptr);
    QList<LOG_MSG_JOURNAL> list;
    LOG_MSG_JOURNAL item;
    for (int i = 0; i < 100; ++i) {
        item.msg = QString("msg%1").arg(i);
        item.level = "Debug";
        item.daemonId = "1";
        item.dateTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
        item.hostName = "test_host";
        item.daemonName = "test_daemon";
        list.append(item);
    }
    p->insertJournalTable(list, 0, list.size());
    p->deleteLater();
}



class DisplayContent_getAppName_UT_Param
{
public:
    DisplayContent_getAppName_UT_Param(const QString &iPath, const QString &iResult)
    {
        path = iPath;
        rs = iResult;

    }
    QString path;
    QString rs;

};

class DisplayContent_getAppName_UT : public ::testing::TestWithParam<DisplayContent_getAppName_UT_Param>
{
};


INSTANTIATE_TEST_SUITE_P(DisplayContent, DisplayContent_getAppName_UT, ::testing::Values(DisplayContent_getAppName_UT_Param("", "")
                                                                                         , DisplayContent_getAppName_UT_Param("deepin-log-viewer.log", "日志收集工具")
                                                                                         , DisplayContent_getAppName_UT_Param("deepin-log-viewer", "日志收集工具")
                                                                                         , DisplayContent_getAppName_UT_Param("/test/deepin-log-viewer.log", "日志收集工具")
                                                                                        ));

TEST_P(DisplayContent_getAppName_UT, DisplayContent_getAppName_UT_001)
{
    DisplayContent_getAppName_UT_Param param = GetParam();
    DisplayContent *p = new DisplayContent(nullptr);
    EXPECT_NE(p, nullptr);
    QString result = p->getAppName(param.path);
    qDebug() << param.path << "result" << result;
    EXPECT_EQ(result, param.rs);
    p->deleteLater();
}



TEST(DisplayContent_isAuthProcessAlive_UT, DisplayContent_isAuthProcessAlive_UT_001)
{
    DisplayContent *p = new DisplayContent(nullptr);
    EXPECT_NE(p, nullptr);
    p->isAuthProcessAlive();
    p->deleteLater();
}


class DisplayContent_generateJournalBootFile_UT_Param
{
public:
    DisplayContent_generateJournalBootFile_UT_Param(int id)
    {
        level = id;

    }
    int level;

};

class DisplayContent_generateJournalBootFile_UT : public ::testing::TestWithParam<DisplayContent_generateJournalBootFile_UT_Param>
{
};


INSTANTIATE_TEST_SUITE_P(DisplayContent, DisplayContent_generateJournalBootFile_UT, ::testing::Values(DisplayContent_generateJournalBootFile_UT_Param(0)
                                                                                                      , DisplayContent_generateJournalBootFile_UT_Param(1)
                                                                                                      , DisplayContent_generateJournalBootFile_UT_Param(2)
                                                                                                      , DisplayContent_generateJournalBootFile_UT_Param(3)
                                                                                                      , DisplayContent_generateJournalBootFile_UT_Param(4)
                                                                                                      , DisplayContent_generateJournalBootFile_UT_Param(5)
                                                                                                      , DisplayContent_generateJournalBootFile_UT_Param(6)
                                                                                                      , DisplayContent_generateJournalBootFile_UT_Param(7)
                                                                                                     ));

TEST_P(DisplayContent_generateJournalBootFile_UT, DisplayContent_generateJournalBootFile_UT)
{
    DisplayContent_generateJournalBootFile_UT_Param param = GetParam();
    DisplayContent *p = new DisplayContent(nullptr);
    EXPECT_NE(p, nullptr);
    p->generateJournalBootFile(param.level);
    p->deleteLater();
}

TEST(DisplayContent_createJournalBootTableStart_UT, DisplayContent_createJournalBootTableStart_UT_001)
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
    p->createJournalBootTableStart(list);
    delete  p;
}
TEST(DisplayContent_createJournalBootTableForm_UT, DisplayContent_createJournalBootTableForm_UT_001)
{
    DisplayContent *p = new DisplayContent(nullptr);
    EXPECT_NE(p, nullptr);

    p->createJournalBootTableForm();
    QStringList  a;
    a << Dtk::Widget::DApplication::translate("Table", "Level")
      << Dtk::Widget::DApplication::translate("Table", "Process")
      << Dtk::Widget::DApplication::translate("Table", "Date and Time")  // modified by Airy
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


TEST(DisplayContent_insertJournalBootTable_UT, DisplayContent_insertJournalBootTable_UT_001)
{
    DisplayContent *p = new DisplayContent(nullptr);
    EXPECT_NE(p, nullptr);
    QList<LOG_MSG_JOURNAL> list;
    LOG_MSG_JOURNAL item;
    for (int i = 0; i < 100; ++i) {
        item.msg = QString("msg%1").arg(i);
        item.level = "Debug";
        item.daemonId = "1";
        item.dateTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
        item.hostName = "test_host";
        item.daemonName = "test_daemon";
        list.append(item);
    }
    p->insertJournalBootTable(list, 0, list.size());
    p->deleteLater();
}

TEST(DisplayContent_slot_tableItemClicked_UT, DisplayContent_slot_tableItemClicked_UT_001)
{
    DisplayContent *p = new DisplayContent(nullptr);
    EXPECT_NE(p, nullptr);

    p->slot_tableItemClicked(QModelIndex());
    p->deleteLater();
}

class DisplayContent_slot_BtnSelected_UT_Param
{
public:
    DisplayContent_slot_BtnSelected_UT_Param(int iIndex)
    {
        index = iIndex;

    }
    int index;



};

class DisplayContent_slot_BtnSelected_UT : public ::testing::TestWithParam<DisplayContent_slot_BtnSelected_UT_Param>
{
};

char *slot_BtnSelected_ModelIndex_data = "";
INSTANTIATE_TEST_SUITE_P(DisplayContent, DisplayContent_slot_BtnSelected_UT, ::testing::Values(DisplayContent_slot_BtnSelected_UT_Param(0)
                                                                                               , DisplayContent_slot_BtnSelected_UT_Param(1)
                                                                                               , DisplayContent_slot_BtnSelected_UT_Param(2)
                                                                                               , DisplayContent_slot_BtnSelected_UT_Param(3)
                                                                                               , DisplayContent_slot_BtnSelected_UT_Param(4)
                                                                                               , DisplayContent_slot_BtnSelected_UT_Param(5)
                                                                                               , DisplayContent_slot_BtnSelected_UT_Param(6)
                                                                                               , DisplayContent_slot_BtnSelected_UT_Param(7)
                                                                                               , DisplayContent_slot_BtnSelected_UT_Param(8)
                                                                                              ));
QVariant slot_BtnSelected_ModelIndex_data_Func(void *obj, int arole)
{
    return QString(slot_BtnSelected_ModelIndex_data);
}
void DisplayContent_slot_BtnSelected_UT_DisplayContent_generateKernFile(int id, const QString &iSearchStr = "")
{
    qDebug() << "DisplayContent_slot_BtnSelected_UT_DisplayContent_generateKernFile--";
}
TEST_P(DisplayContent_slot_BtnSelected_UT, DisplayContent_slot_BtnSelected_UT_001)
{
    DisplayContent_slot_BtnSelected_UT_Param param = GetParam();
    DisplayContent *p = new DisplayContent(nullptr);
    EXPECT_NE(p, nullptr);
    Stub *stub = new Stub;

    switch (param.index) {
    case 0:
        slot_BtnSelected_ModelIndex_data = JOUR_TREE_DATA;
        break;
    case 1:
        slot_BtnSelected_ModelIndex_data = BOOT_KLU_TREE_DATA;
        break;
    case 2:
        slot_BtnSelected_ModelIndex_data = DPKG_TREE_DATA;
        break;
    case 3:
        slot_BtnSelected_ModelIndex_data = KERN_TREE_DATA;
        break;
    case 4:
        slot_BtnSelected_ModelIndex_data = APP_TREE_DATA;
        break;
    case 5:
        slot_BtnSelected_ModelIndex_data = XORG_TREE_DATA;
        break;
    case 6:
        slot_BtnSelected_ModelIndex_data = LAST_TREE_DATA;
        break;
    case 7:
        slot_BtnSelected_ModelIndex_data = ".cache";
        break;
    case 8:
        slot_BtnSelected_ModelIndex_data = "";
        break;
    default:
        break;
    }

    stub->set(ADDR(QModelIndex, data), slot_BtnSelected_ModelIndex_data_Func);
    stub->set(ADDR(DisplayContent, generateKernFile), DisplayContent_slot_BtnSelected_UT_DisplayContent_generateKernFile);
    p->slot_BtnSelected(0, 0, QModelIndex());
    p->deleteLater();
}


TEST(DisplayContent_slot_appLogs_UT, DisplayContent_slot_appLogs_UT_001)
{
    DisplayContent *p = new DisplayContent(nullptr);
    EXPECT_NE(p, nullptr);

    p->slot_appLogs(QDir::homePath() + "/.cache/deepin/deepin-log-viewer/deepin-log-viewer.log");
    p->deleteLater();
}

class DisplayContent_slot_logCatelogueClicked_UT_Param
{
public:
    DisplayContent_slot_logCatelogueClicked_UT_Param(int iIndex)
    {
        index = iIndex;

    }
    int index;



};

class DisplayContent_slot_logCatelogueClicked_UT : public ::testing::TestWithParam<DisplayContent_slot_logCatelogueClicked_UT_Param>
{
};

QString slot_logCatelogueClicked_ModelIndex_data = "";
INSTANTIATE_TEST_SUITE_P(DisplayContent, DisplayContent_slot_logCatelogueClicked_UT, ::testing::Values(DisplayContent_slot_logCatelogueClicked_UT_Param(0)
                                                                                                       , DisplayContent_slot_logCatelogueClicked_UT_Param(1)
                                                                                                       , DisplayContent_slot_logCatelogueClicked_UT_Param(2)
                                                                                                       , DisplayContent_slot_logCatelogueClicked_UT_Param(3)
                                                                                                       , DisplayContent_slot_logCatelogueClicked_UT_Param(4)
                                                                                                       , DisplayContent_slot_logCatelogueClicked_UT_Param(5)
                                                                                                       , DisplayContent_slot_logCatelogueClicked_UT_Param(6)
                                                                                                       , DisplayContent_slot_logCatelogueClicked_UT_Param(7)
                                                                                                       , DisplayContent_slot_logCatelogueClicked_UT_Param(8)
                                                                                                       , DisplayContent_slot_logCatelogueClicked_UT_Param(9)
                                                                                                      ));
QVariant slot_logCatelogueClicked_ModelIndex_data_Func(void *obj, int arole)
{
    return QString(slot_logCatelogueClicked_ModelIndex_data);
}
bool slot_logCatelogueClicked_ModelIndex_isValid_Func(void *obj)
{
    return true;
}

TEST_P(DisplayContent_slot_logCatelogueClicked_UT, DisplayContent_slot_logCatelogueClicked_UT_001)
{
    DisplayContent_slot_logCatelogueClicked_UT_Param param = GetParam();
    DisplayContent *p = new DisplayContent(nullptr);
    EXPECT_NE(p, nullptr);
    Stub *stub = new Stub;

    switch (param.index) {
    case 0:
        slot_logCatelogueClicked_ModelIndex_data = JOUR_TREE_DATA;
        break;
    case 1:
        slot_logCatelogueClicked_ModelIndex_data = DPKG_TREE_DATA;
        break;
    case 2:
        slot_logCatelogueClicked_ModelIndex_data = XORG_TREE_DATA;
        break;
    case 3:
        slot_logCatelogueClicked_ModelIndex_data = BOOT_TREE_DATA;
        break;
    case 4:
        slot_logCatelogueClicked_ModelIndex_data = KERN_TREE_DATA;
        break;
    case 5:
        slot_logCatelogueClicked_ModelIndex_data = APP_TREE_DATA;
        break;
    case 6:
        slot_logCatelogueClicked_ModelIndex_data = LAST_TREE_DATA;
        break;
    case 7:
        slot_logCatelogueClicked_ModelIndex_data = KWIN_TREE_DATA;
        break;
    case 8:
        slot_logCatelogueClicked_ModelIndex_data = BOOT_KLU_TREE_DATA;
        break;
    case 9:
        slot_logCatelogueClicked_ModelIndex_data = "";
        break;
    default:
        break;
    }

    stub->set(ADDR(QModelIndex, data), slot_logCatelogueClicked_ModelIndex_data_Func);
    stub->set(ADDR(QModelIndex, isValid), slot_logCatelogueClicked_ModelIndex_isValid_Func);
    p->slot_logCatelogueClicked(QModelIndex());
    p->deleteLater();
}

//class DisplayContent_slot_exportClicked_UT_Param
//{
//public:
//    DisplayContent_slot_exportClicked_UT_Param(int iIndex)
//    {
//        index = iIndex;

//    }
//    int index;



//};

//class DisplayContent_slot_exportClicked_UT : public ::testing::TestWithParam<DisplayContent_slot_exportClicked_UT_Param>
//{
//};


//INSTANTIATE_TEST_SUITE_P(DisplayContent, DisplayContent_slot_exportClicked_UT, ::testing::Values(DisplayContent_slot_exportClicked_UT_Param(0)
//                                                                                                 , DisplayContent_slot_exportClicked_UT_Param(1)
//                                                                                                 , DisplayContent_slot_exportClicked_UT_Param(2)
//                                                                                                 , DisplayContent_slot_exportClicked_UT_Param(3)
//                                                                                                 , DisplayContent_slot_exportClicked_UT_Param(4)
//                                                                                                 , DisplayContent_slot_exportClicked_UT_Param(5)
//                                                                                                 , DisplayContent_slot_exportClicked_UT_Param(6)
//                                                                                                 , DisplayContent_slot_exportClicked_UT_Param(7)
//                                                                                                 , DisplayContent_slot_exportClicked_UT_Param(8)
//                                                                                                 , DisplayContent_slot_exportClicked_UT_Param(9)
//                                                                                                ));
QString slot_exportClicked_ModelIndex_data = "";
QVariant slot_exportClicked_ModelIndex_data_Func(void *obj, int arole)
{
    return QString(slot_exportClicked_ModelIndex_data);
}

QString exportClicked_getSaveFileName(QWidget *parent,
                                      const QString &caption,
                                      const QString &dir,
                                      const QString &filter,
                                      QString *selectedFilter,
                                      QFileDialog::Options options)
{
    return  "";
}
TEST(DisplayContent_slot_exportClicked_UT, DisplayContent_slot_exportClicked_UT_001)
{
//   DisplayContent_slot_exportClicked_UT_Param param = GetParam();
    DisplayContent *p = new DisplayContent(nullptr);
    EXPECT_NE(p, nullptr);
    Stub *stub = new Stub;

//    switch (param.index) {
//    case 0:

//        break;
//    case 1:
//        slot_exportClicked_ModelIndex_data = DPKG_TREE_DATA;
//        break;
//    case 2:
//        slot_exportClicked_ModelIndex_data = XORG_TREE_DATA;
//        break;
//    case 3:
//        slot_exportClicked_ModelIndex_data = BOOT_TREE_DATA;
//        break;
//    case 4:
//        slot_exportClicked_ModelIndex_data = KERN_TREE_DATA;
//        break;
//    case 5:
//        slot_exportClicked_ModelIndex_data = APP_TREE_DATA;
//        break;
//    case 6:
//        slot_exportClicked_ModelIndex_data = LAST_TREE_DATA;
//        break;
//    case 7:
//        slot_exportClicked_ModelIndex_data = KWIN_TREE_DATA;
//        break;
//    case 8:
//        slot_exportClicked_ModelIndex_data = BOOT_KLU_TREE_DATA;
//        break;
//    case 9:
//        slot_exportClicked_ModelIndex_data = "";
//        break;
//    default:
//        break;
//    }

    stub->set(ADDR(QFileDialog, getSaveFileName), exportClicked_getSaveFileName);
    p->slot_exportClicked();
    p->deleteLater();
}
TEST(DisplayContent_slot_statusChagned_UT, DisplayContent_slot_statusChagned_UT_001)
{
    DisplayContent *p = new DisplayContent(nullptr);
    EXPECT_NE(p, nullptr);
    p->slot_statusChagned("OK");
    p->deleteLater();
}


TEST(DisplayContent_slot_dpkgFinished_UT, DisplayContent_slot_dpkgFinished_UT_001)
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
    p->m_flag = DPKG;
    p->slot_dpkgFinished(list);
    p->m_flag = NONE;
    p->slot_dpkgFinished(list);
    delete  p;
}


TEST(DisplayContent_slot_XorgFinished_UT, DisplayContent_slot_XorgFinished_UT_001)
{
    DisplayContent *p = new DisplayContent(nullptr);
    EXPECT_NE(p, nullptr);
    QList<LOG_MSG_XORG> list;
    LOG_MSG_XORG item;
    for (int i = 0; i < 100; ++i) {
        item.msg = QString("msg%1").arg(i);
        item.dateTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
        list.append(item);
    }
    p->m_flag = XORG;
    p->slot_XorgFinished(list);
    p->m_flag = NONE;
    p->slot_XorgFinished(list);
    p->deleteLater();
}

TEST(DisplayContent_slot_bootFinished_UT, DisplayContent_slot_bootFinished_UT_001)
{
    DisplayContent *p = new DisplayContent(nullptr);
    EXPECT_NE(p, nullptr);
    QList<LOG_MSG_BOOT> list;
    LOG_MSG_BOOT item;
    for (int i = 0; i < 100; ++i) {
        item.msg = QString("msg%1").arg(i);
        item.status = "OK";
        list.append(item);
    }
    p->m_flag = BOOT;
    p->slot_bootFinished(list);
    p->m_flag = NONE;
    p->slot_bootFinished(list);
    p->deleteLater();
}
TEST(DisplayContent_slot_kernFinished_UT, DisplayContent_slot_kernFinished_UT_001)
{
    DisplayContent *p = new DisplayContent(nullptr);
    EXPECT_NE(p, nullptr);
    QList<LOG_MSG_JOURNAL> list;
    LOG_MSG_JOURNAL item;
    for (int i = 0; i < 100; ++i) {
        item.msg = QString("msg%1").arg(i);
        item.level = "Debug";
        item.daemonId = "1";
        item.dateTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
        item.hostName = "test_host";
        item.daemonName = "test_daemon";
        list.append(item);
    }
    p->m_flag = KERN;
    p->slot_kernFinished(list);
    p->m_flag = NONE;
    p->slot_kernFinished(list);
    p->deleteLater();
}
TEST(DisplayContent_slot_kwinFinished_UT, DisplayContent_slot_kwinFinished_UT_001)
{
    DisplayContent *p = new DisplayContent(nullptr);
    EXPECT_NE(p, nullptr);
    QList<LOG_MSG_KWIN> list;
    LOG_MSG_KWIN item;
    for (int i = 0; i < 100; ++i) {
        item.msg = QString("msg%1").arg(i);
        list.append(item);
    }
    p->m_flag = Kwin;
    p->slot_kwinFinished(list);
    p->m_flag = NONE;
    p->slot_kwinFinished(list);
    p->deleteLater();
}

TEST(DisplayContent_slot_journalBootFinished_UT, DisplayContent_slot_journalBootFinished_UT_001)
{
    DisplayContent *p = new DisplayContent(nullptr);
    EXPECT_NE(p, nullptr);
    p->slot_journalBootFinished();
    p->deleteLater();
}


class DisplayContent_slot_journalData_UT_Param
{
public:
    DisplayContent_slot_journalData_UT_Param(LOG_FLAG iFlag, bool iIndexEqual, bool iListIsEmpty, bool iFirstLoadPageData)
    {
        m_flag = iFlag;
        indexEqual = iIndexEqual;
        listIsEmpty = iListIsEmpty;
        m_firstLoadPageData = iFirstLoadPageData;
    }
    LOG_FLAG m_flag;
    bool indexEqual;
    bool listIsEmpty;
    bool m_firstLoadPageData;


};

class DisplayContent_slot_journalData_UT : public ::testing::TestWithParam<DisplayContent_slot_journalData_UT_Param>
{
};

INSTANTIATE_TEST_SUITE_P(DisplayContent, DisplayContent_slot_journalData_UT, ::testing::Values(DisplayContent_slot_journalData_UT_Param(JOURNAL, true, true, true)
                                                                                               , DisplayContent_slot_journalData_UT_Param(JOURNAL, true, true, false)
                                                                                               , DisplayContent_slot_journalData_UT_Param(JOURNAL, true, false, true)
                                                                                               , DisplayContent_slot_journalData_UT_Param(JOURNAL, true, false, false)
                                                                                               , DisplayContent_slot_journalData_UT_Param(JOURNAL, false, true, true)
                                                                                               , DisplayContent_slot_journalData_UT_Param(JOURNAL, false, true, false)
                                                                                               , DisplayContent_slot_journalData_UT_Param(JOURNAL, false, false, true)
                                                                                               , DisplayContent_slot_journalData_UT_Param(JOURNAL, false, false, false)
                                                                                               , DisplayContent_slot_journalData_UT_Param(NONE, true, true, true)
                                                                                               , DisplayContent_slot_journalData_UT_Param(NONE, true, true, false)
                                                                                               , DisplayContent_slot_journalData_UT_Param(NONE, true, false, true)
                                                                                               , DisplayContent_slot_journalData_UT_Param(NONE, true, false, false)
                                                                                               , DisplayContent_slot_journalData_UT_Param(NONE, false, true, true)
                                                                                               , DisplayContent_slot_journalData_UT_Param(NONE, false, true, false)
                                                                                               , DisplayContent_slot_journalData_UT_Param(NONE, false, false, true)
                                                                                               , DisplayContent_slot_journalData_UT_Param(NONE, false, false, false)
                                                                                              ));


TEST_P(DisplayContent_slot_journalData_UT, DisplayContent_slot_journalData_UT_001)
{
    DisplayContent_slot_journalData_UT_Param param = GetParam();
    DisplayContent *p = new DisplayContent(nullptr);
    EXPECT_NE(p, nullptr);
    QList<LOG_MSG_JOURNAL> list;
    if (!param.listIsEmpty) {
        LOG_MSG_JOURNAL item;
        for (int i = 0; i < 100; ++i) {
            item.msg = QString("msg%1").arg(i);
            item.level = "Debug";
            item.daemonId = "1";
            item.dateTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
            item.hostName = "test_host";
            item.daemonName = "test_daemon";
            list.append(item);
        }
    }

    p->m_flag = param.m_flag;
    p->m_journalCurrentIndex = 0;
    p->m_firstLoadPageData = param.m_firstLoadPageData;
    int index = param.indexEqual ? 0 : 1;
    p->slot_journalData(index, list);
    p->deleteLater();
}




class DisplayContent_slot_journalBootData_UT_Param
{
public:
    DisplayContent_slot_journalBootData_UT_Param(LOG_FLAG iFlag, bool iIndexEqual, bool iListIsEmpty, bool iFirstLoadPageData)
    {
        m_flag = iFlag;
        indexEqual = iIndexEqual;
        listIsEmpty = iListIsEmpty;
        m_firstLoadPageData = iFirstLoadPageData;
    }
    LOG_FLAG m_flag;
    bool indexEqual;
    bool listIsEmpty;
    bool m_firstLoadPageData;


};

class DisplayContent_slot_journalBootData_UT : public ::testing::TestWithParam<DisplayContent_slot_journalBootData_UT_Param>
{
};

INSTANTIATE_TEST_SUITE_P(DisplayContent, DisplayContent_slot_journalBootData_UT, ::testing::Values(DisplayContent_slot_journalBootData_UT_Param(BOOT_KLU, true, true, true)
                                                                                                   , DisplayContent_slot_journalBootData_UT_Param(BOOT_KLU, true, true, false)
                                                                                                   , DisplayContent_slot_journalBootData_UT_Param(BOOT_KLU, true, false, true)
                                                                                                   , DisplayContent_slot_journalBootData_UT_Param(BOOT_KLU, true, false, false)
                                                                                                   , DisplayContent_slot_journalBootData_UT_Param(BOOT_KLU, false, true, true)
                                                                                                   , DisplayContent_slot_journalBootData_UT_Param(BOOT_KLU, false, true, false)
                                                                                                   , DisplayContent_slot_journalBootData_UT_Param(BOOT_KLU, false, false, true)
                                                                                                   , DisplayContent_slot_journalBootData_UT_Param(BOOT_KLU, false, false, false)
                                                                                                   , DisplayContent_slot_journalBootData_UT_Param(NONE, true, true, true)
                                                                                                   , DisplayContent_slot_journalBootData_UT_Param(NONE, true, true, false)
                                                                                                   , DisplayContent_slot_journalBootData_UT_Param(NONE, true, false, true)
                                                                                                   , DisplayContent_slot_journalBootData_UT_Param(NONE, true, false, false)
                                                                                                   , DisplayContent_slot_journalBootData_UT_Param(NONE, false, true, true)
                                                                                                   , DisplayContent_slot_journalBootData_UT_Param(NONE, false, true, false)
                                                                                                   , DisplayContent_slot_journalBootData_UT_Param(NONE, false, false, true)
                                                                                                   , DisplayContent_slot_journalBootData_UT_Param(NONE, false, false, false)
                                                                                                  ));


TEST_P(DisplayContent_slot_journalBootData_UT, DisplayContent_slot_journalBootData_UT_001)
{
    DisplayContent_slot_journalBootData_UT_Param param = GetParam();
    DisplayContent *p = new DisplayContent(nullptr);
    EXPECT_NE(p, nullptr);
    QList<LOG_MSG_JOURNAL> list;
    if (!param.listIsEmpty) {
        LOG_MSG_JOURNAL item;
        for (int i = 0; i < 100; ++i) {
            item.msg = QString("msg%1").arg(i);
            item.level = "Debug";
            item.daemonId = "1";
            item.dateTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
            item.hostName = "test_host";
            item.daemonName = "test_daemon";
            list.append(item);
        }
    }

    p->m_flag = param.m_flag;
    p->m_journalBootCurrentIndex = 0;
    p->m_firstLoadPageData = param.m_firstLoadPageData;
    int index = param.indexEqual ? 0 : 1;
    p->slot_journalBootData(index, list);
    p->deleteLater();
}
TEST(DisplayContent_slot_applicationFinished_UT, DisplayContent_slot_applicationFinished_UT_001)
{
    DisplayContent *p = new DisplayContent(nullptr);
    EXPECT_NE(p, nullptr);
    QList<LOG_MSG_APPLICATOIN> list;
    LOG_MSG_APPLICATOIN item;
    for (int i = 0; i < 100; ++i) {
        item.msg = QString("msg%1").arg(i);
        item.level = "Debug";
        item.dateTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
        item.src = "test_src";
        list.append(item);
    }
    p->m_flag = APP;
    p->slot_applicationFinished(list);
    p->m_flag = NONE;
    p->slot_applicationFinished(list);
    p->deleteLater();
}

TEST(DisplayContent_slot_NormalFinished_UT, DisplayContent_NormalFinished_UT_001)
{
    DisplayContent *p = new DisplayContent(nullptr);
    EXPECT_NE(p, nullptr);
    p->m_flag = Normal;
    p->slot_NormalFinished();
    p->m_flag = NONE;
    p->slot_NormalFinished();
    p->deleteLater();
}
TEST(DisplayContent_slot_themeChanged_UT, DisplayContent_slot_themeChanged_UT_001)
{
    DisplayContent *p = new DisplayContent(nullptr);
    EXPECT_NE(p, nullptr);
    p->slot_themeChanged(DGuiApplicationHelper::LightType);
    p->deleteLater();
}

TEST(DisplayContent_resizeEvent_UT, DisplayContent_resizeEvent_UT_001)
{
    DisplayContent *p = new DisplayContent(nullptr);
    EXPECT_NE(p, nullptr);
    p->resizeEvent(nullptr);
    p->deleteLater();
}

TEST(DisplayContent_paintEvent_UT, DisplayContent_paintEvent_UT_001)
{
    DisplayContent *p = new DisplayContent(nullptr);
    EXPECT_NE(p, nullptr);
    p->paintEvent(new QPaintEvent(p->rect()));
    p->deleteLater();
}
class DisplayContent_slot_vScrollValueChanged_UT_Param
{
public:
    DisplayContent_slot_vScrollValueChanged_UT_Param(LOG_FLAG iFlag, bool iIsTreeView, bool iIsTreeViewHeight, bool iIsLastHeight, bool iIsLoadData, bool iIsOutLoad)
    {
        m_flag = iFlag;
        isTreeView = iIsTreeView;
        isTreeViewHeight = iIsTreeViewHeight;
        isLastHeight = iIsLastHeight;
        isLoadData = iIsLoadData;
        isOutLoad = iIsOutLoad;
    }
    LOG_FLAG m_flag;
    bool isTreeView;
    bool isTreeViewHeight;
    bool isLastHeight;
    bool isLoadData;
    bool isOutLoad;

};

class DisplayContent_slot_vScrollValueChanged_UT : public ::testing::TestWithParam<DisplayContent_slot_vScrollValueChanged_UT_Param>
{
};
int DisplayContent_slot_vScrollValueChanged_treeView_singleRowHeight = -1;
int DisplayContent_slot_vScrollValueChanged_treeView_singleRowHeight_Func(void *obj)
{
    return DisplayContent_slot_vScrollValueChanged_treeView_singleRowHeight;
}

INSTANTIATE_TEST_SUITE_P(DisplayContent, DisplayContent_slot_vScrollValueChanged_UT, ::testing::Values(DisplayContent_slot_vScrollValueChanged_UT_Param(JOURNAL, false, true, true, true, true)
                                                                                                       , DisplayContent_slot_vScrollValueChanged_UT_Param(JOURNAL, true, false, true, true, true)
                                                                                                       , DisplayContent_slot_vScrollValueChanged_UT_Param(JOURNAL, true, true, false, true, true)
                                                                                                       , DisplayContent_slot_vScrollValueChanged_UT_Param(JOURNAL, true, true, true, false, true)
                                                                                                       , DisplayContent_slot_vScrollValueChanged_UT_Param(JOURNAL, true, true, true, true, true)
                                                                                                       , DisplayContent_slot_vScrollValueChanged_UT_Param(JOURNAL, true, true, true, true, false)
                                                                                                       , DisplayContent_slot_vScrollValueChanged_UT_Param(BOOT_KLU, false, true, true, true, true)
                                                                                                       , DisplayContent_slot_vScrollValueChanged_UT_Param(BOOT_KLU, true, false, true, true, true)
                                                                                                       , DisplayContent_slot_vScrollValueChanged_UT_Param(BOOT_KLU, true, true, false, true, true)
                                                                                                       , DisplayContent_slot_vScrollValueChanged_UT_Param(BOOT_KLU, true, true, true, false, true)
                                                                                                       , DisplayContent_slot_vScrollValueChanged_UT_Param(BOOT_KLU, true, true, true, true, true)
                                                                                                       , DisplayContent_slot_vScrollValueChanged_UT_Param(BOOT_KLU, true, true, true, true, false)
                                                                                                       , DisplayContent_slot_vScrollValueChanged_UT_Param(APP, false, true, true, true, true)
                                                                                                       , DisplayContent_slot_vScrollValueChanged_UT_Param(APP, true, false, true, true, true)
                                                                                                       , DisplayContent_slot_vScrollValueChanged_UT_Param(APP, true, true, false, true, true)
                                                                                                       , DisplayContent_slot_vScrollValueChanged_UT_Param(APP, true, true, true, false, true)
                                                                                                       , DisplayContent_slot_vScrollValueChanged_UT_Param(APP, true, true, true, true, true)
                                                                                                       , DisplayContent_slot_vScrollValueChanged_UT_Param(APP, true, true, true, true, false)
                                                                                                       , DisplayContent_slot_vScrollValueChanged_UT_Param(KERN, false, true, true, true, true)
                                                                                                       , DisplayContent_slot_vScrollValueChanged_UT_Param(KERN, true, false, true, true, true)
                                                                                                       , DisplayContent_slot_vScrollValueChanged_UT_Param(KERN, true, true, false, true, true)
                                                                                                       , DisplayContent_slot_vScrollValueChanged_UT_Param(KERN, true, true, true, false, true)
                                                                                                       , DisplayContent_slot_vScrollValueChanged_UT_Param(KERN, true, true, true, true, true)
                                                                                                       , DisplayContent_slot_vScrollValueChanged_UT_Param(KERN, true, true, true, true, false)
                                                                                                      ));


TEST_P(DisplayContent_slot_vScrollValueChanged_UT, DisplayContent_slot_vScrollValueChanged_UT_001)
{
    DisplayContent_slot_vScrollValueChanged_UT_Param param = GetParam();
    DisplayContent *p = new DisplayContent(nullptr);
    EXPECT_NE(p, nullptr);
    int valuePixel = 0;


    p->m_flag = param.m_flag;
    if (!param.isTreeView) {
        p->m_treeView->deleteLater();
        p->m_treeView = nullptr;
    }
    DisplayContent_slot_vScrollValueChanged_treeView_singleRowHeight = param.isTreeViewHeight ? 20 : -1;
    if (param.isLoadData) {
        valuePixel = 190 * DisplayContent_slot_vScrollValueChanged_treeView_singleRowHeight;
    } else {
        valuePixel = 5 * DisplayContent_slot_vScrollValueChanged_treeView_singleRowHeight;
    }
    if (param.isOutLoad) {
        p->m_limitTag = -1;
    } else {
        p->m_limitTag = (valuePixel / DisplayContent_slot_vScrollValueChanged_treeView_singleRowHeight + 25) / 200;
    }
    Stub *stub = new Stub;
    stub->set(ADDR(LogTreeView, singleRowHeight), DisplayContent_slot_vScrollValueChanged_treeView_singleRowHeight_Func);
    if (!param.isLastHeight) {
        p->m_treeViewLastScrollValue = valuePixel;
    } else {
        p->m_treeViewLastScrollValue = -1;
    }
    p->slot_vScrollValueChanged(valuePixel);
    p->deleteLater();
}


class DisplayContent_slot_searchResult_UT_Param
{
public:
    DisplayContent_slot_searchResult_UT_Param(LOG_FLAG iFlag, bool iIsContains, bool iIsSearchEmpty)
    {
        m_flag = iFlag;
        isContains = iIsContains;
        isSearchEmpty = iIsSearchEmpty;
    }
    LOG_FLAG m_flag;
    bool isContains;
    bool isSearchEmpty;


};
bool DisplayContent_slot_searchResult_QString_contains = false;
bool DisplayContent_slot_searchResult_QString_contains_Func(const QString &s, Qt::CaseSensitivity cs)
{
    Q_UNUSED(s)
    Q_UNUSED(cs)
    return DisplayContent_slot_searchResult_QString_contains;
}
class DisplayContent_slot_searchResult_UT : public ::testing::TestWithParam<DisplayContent_slot_searchResult_UT_Param>
{
};


INSTANTIATE_TEST_SUITE_P(DisplayContent, DisplayContent_slot_searchResult_UT, ::testing::Values(DisplayContent_slot_searchResult_UT_Param(JOURNAL, true, true)
                                                                                                , DisplayContent_slot_searchResult_UT_Param(JOURNAL, false, true)
                                                                                                , DisplayContent_slot_searchResult_UT_Param(JOURNAL, true, false)
                                                                                                , DisplayContent_slot_searchResult_UT_Param(NONE, true, true)
                                                                                                , DisplayContent_slot_searchResult_UT_Param(BOOT_KLU, true, true)
                                                                                                , DisplayContent_slot_searchResult_UT_Param(BOOT_KLU, false, true)
                                                                                                , DisplayContent_slot_searchResult_UT_Param(BOOT_KLU, true, false)
                                                                                                , DisplayContent_slot_searchResult_UT_Param(KERN, true, true)
                                                                                                , DisplayContent_slot_searchResult_UT_Param(KERN, false, true)
                                                                                                , DisplayContent_slot_searchResult_UT_Param(KERN, true, false)
                                                                                                , DisplayContent_slot_searchResult_UT_Param(BOOT, true, true)
                                                                                                , DisplayContent_slot_searchResult_UT_Param(BOOT, false, true)
                                                                                                , DisplayContent_slot_searchResult_UT_Param(BOOT, true, false)
                                                                                                , DisplayContent_slot_searchResult_UT_Param(XORG, true, true)
                                                                                                , DisplayContent_slot_searchResult_UT_Param(XORG, false, true)
                                                                                                , DisplayContent_slot_searchResult_UT_Param(XORG, true, false)
                                                                                                , DisplayContent_slot_searchResult_UT_Param(DPKG, true, true)
                                                                                                , DisplayContent_slot_searchResult_UT_Param(DPKG, false, true)
                                                                                                , DisplayContent_slot_searchResult_UT_Param(DPKG, true, false)
                                                                                                , DisplayContent_slot_searchResult_UT_Param(APP, true, true)
                                                                                                , DisplayContent_slot_searchResult_UT_Param(APP, false, true)
                                                                                                , DisplayContent_slot_searchResult_UT_Param(APP, true, false)
                                                                                                , DisplayContent_slot_searchResult_UT_Param(Normal, true, true)
                                                                                                , DisplayContent_slot_searchResult_UT_Param(Normal, false, true)
                                                                                                , DisplayContent_slot_searchResult_UT_Param(Normal, true, false)
                                                                                                , DisplayContent_slot_searchResult_UT_Param(Kwin, true, true)
                                                                                                , DisplayContent_slot_searchResult_UT_Param(Kwin, false, true)
                                                                                                , DisplayContent_slot_searchResult_UT_Param(Kwin, true, false)
                                                                                               ));


TEST_P(DisplayContent_slot_searchResult_UT, DisplayContent_slot_searchResult_UT_001)
{
    DisplayContent_slot_searchResult_UT_Param param = GetParam();
    DisplayContent *p = new DisplayContent(nullptr);
    EXPECT_NE(p, nullptr);
    p->m_flag = param.m_flag;
    switch (p->m_flag) {
    case JOURNAL: {
        QList<LOG_MSG_JOURNAL> list;
        LOG_MSG_JOURNAL item;
        for (int i = 0; i < 100; ++i) {
            item.msg = QString("msg%1").arg(i);
            item.level = "Debug";
            item.daemonId = "1";
            item.dateTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
            item.hostName = "test_host";
            item.daemonName = "test_daemon";
            list.append(item);
        }
        p->jBootListOrigin.append(list);
        break;
    }
    case BOOT_KLU: {
        QList<LOG_MSG_JOURNAL> list;
        LOG_MSG_JOURNAL item;
        for (int i = 0; i < 100; ++i) {
            item.msg = QString("msg%1").arg(i);
            item.level = "Debug";
            item.daemonId = "1";
            item.dateTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
            item.hostName = "test_host";
            item.daemonName = "test_daemon";
            list.append(item);
        }
        p->jBootListOrigin.append(list);
        break;
    }
    case KERN: {
        QList<LOG_MSG_JOURNAL> list;
        LOG_MSG_JOURNAL item;
        for (int i = 0; i < 100; ++i) {
            item.msg = QString("msg%1").arg(i);
            item.level = "Debug";
            item.daemonId = "1";
            item.dateTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
            item.hostName = "test_host";
            item.daemonName = "test_daemon";
            list.append(item);
        }
        p->kListOrigin.append(list);
        break;
    }
    case BOOT: {

        break;
    }
    case XORG: {
        QList<LOG_MSG_XORG> list;
        LOG_MSG_XORG item;
        for (int i = 0; i < 100; ++i) {
            item.msg = QString("msg%1").arg(i);
            item.dateTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
            list.append(item);
        }
        p->xListOrigin.append(list);
        break;
    }
    case DPKG: {
        QList<LOG_MSG_DPKG> list;
        for (int i = 0; i < 100; ++i) {
            LOG_MSG_DPKG item;
            item.msg = "";
            item.dateTime = "";
            item.action = "";
            list.append(item);
        }
        p->dListOrigin.append(list);
        break;
    } case APP: {
        QList<LOG_MSG_APPLICATOIN> list;
        LOG_MSG_APPLICATOIN item;
        for (int i = 0; i < 100; ++i) {
            item.msg = QString("msg%1").arg(i);
            item.level = "Debug";
            item.dateTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
            item.src = "test_src";
            list.append(item);
        }
        p->appListOrigin.append(list);
        break;
    }
    case Normal: {

        break;
    }
    case Kwin: {
        QList<LOG_MSG_KWIN> list;
        LOG_MSG_KWIN item;
        for (int i = 0; i < 100; ++i) {
            item.msg = QString("msg%1").arg(i);
            list.append(item);
        }
        p->m_kwinList.append(list);
        break;
    }
    default:
        break;
    }
    DisplayContent_slot_searchResult_QString_contains = param.isContains;
    Stub *stub = new Stub;
    stub->set((bool(QString::*)(const QString &, Qt::CaseSensitivity) const)ADDR(QString, contains), DisplayContent_slot_searchResult_QString_contains_Func);
    QString searchstr = param.isSearchEmpty ? "" : "testsearchstr";
    p->slot_searchResult(searchstr);
    p->deleteLater();
}

TEST(DisplayContent_slot_getLogtype_UT, DisplayContent_slot_getLogtype_UT_001)
{
    DisplayContent *p = new DisplayContent(nullptr);
    EXPECT_NE(p, nullptr);
    p->slot_getLogtype(0);
    p->deleteLater();
}


class DisplayContent_parseListToModel_DPKG_UT_Param
{
public:
    DisplayContent_parseListToModel_DPKG_UT_Param(bool iIsEmptyList, bool iIsEmptyModel)
    {

        isEmptyList = iIsEmptyList;
        isEmptyList = iIsEmptyModel;
    }
    bool isEmptyList;
    bool isEmptyModel;


};

class DisplayContent_parseListToModel_DPKG_UT : public ::testing::TestWithParam<DisplayContent_parseListToModel_DPKG_UT_Param>
{
};


INSTANTIATE_TEST_SUITE_P(DisplayContent, DisplayContent_parseListToModel_DPKG_UT, ::testing::Values(DisplayContent_parseListToModel_DPKG_UT_Param(true, true)
                                                                                                    , DisplayContent_parseListToModel_DPKG_UT_Param(true, false)
                                                                                                    , DisplayContent_parseListToModel_DPKG_UT_Param(false, false)
                                                                                                   ));


TEST_P(DisplayContent_parseListToModel_DPKG_UT, DisplayContent_parseListToModel_DPKG_UT)
{
    DisplayContent *p = new DisplayContent(nullptr);
    EXPECT_NE(p, nullptr);
    DisplayContent_parseListToModel_DPKG_UT_Param param = GetParam();
    QList<LOG_MSG_DPKG> list;
    if (!param.isEmptyList) {
        for (int i = 0; i < 100; ++i) {
            LOG_MSG_DPKG item;
            item.msg = "";
            item.dateTime = "";
            item.action = "";
            list.append(item);
        }
    }

    p->parseListToModel(list, (param.isEmptyModel ? nullptr : p->m_pModel));
    p->deleteLater();
}

class DisplayContent_parseListToModel_BOOT_UT_Param
{
public:
    DisplayContent_parseListToModel_BOOT_UT_Param(bool iIsEmptyList, bool iIsEmptyModel)
    {

        isEmptyList = iIsEmptyList;
        isEmptyList = iIsEmptyModel;
    }
    bool isEmptyList;
    bool isEmptyModel;


};

class DisplayContent_parseListToModel_BOOT_UT : public ::testing::TestWithParam<DisplayContent_parseListToModel_BOOT_UT_Param>
{
};


INSTANTIATE_TEST_SUITE_P(DisplayContent, DisplayContent_parseListToModel_BOOT_UT, ::testing::Values(DisplayContent_parseListToModel_BOOT_UT_Param(true, true)
                                                                                                    , DisplayContent_parseListToModel_BOOT_UT_Param(true, false)
                                                                                                    , DisplayContent_parseListToModel_BOOT_UT_Param(false, false)
                                                                                                   ));


TEST_P(DisplayContent_parseListToModel_BOOT_UT, DisplayContent_parseListToModel_BOOT_UT)
{
    DisplayContent *p = new DisplayContent(nullptr);
    EXPECT_NE(p, nullptr);
    DisplayContent_parseListToModel_BOOT_UT_Param param = GetParam();
    QList<LOG_MSG_BOOT> list;

    if (!param.isEmptyList) {
        LOG_MSG_BOOT item;
        for (int i = 0; i < 100; ++i) {
            item.msg = QString("msg%1").arg(i);
            item.status = "OK";
            list.append(item);
        }
    }

    p->parseListToModel(list, (param.isEmptyModel ? nullptr : p->m_pModel));
    p->deleteLater();
}

class DisplayContent_parseListToModel_APP_UT_Param
{
public:
    DisplayContent_parseListToModel_APP_UT_Param(bool iIsEmptyList, bool iIsEmptyModel)
    {

        isEmptyList = iIsEmptyList;
        isEmptyList = iIsEmptyModel;
    }
    bool isEmptyList;
    bool isEmptyModel;


};

class DisplayContent_parseListToModel_APP_UT : public ::testing::TestWithParam<DisplayContent_parseListToModel_APP_UT_Param>
{
};


INSTANTIATE_TEST_SUITE_P(DisplayContent, DisplayContent_parseListToModel_APP_UT, ::testing::Values(DisplayContent_parseListToModel_APP_UT_Param(true, true)
                                                                                                   , DisplayContent_parseListToModel_APP_UT_Param(true, false)
                                                                                                   , DisplayContent_parseListToModel_APP_UT_Param(false, false)
                                                                                                  ));


TEST_P(DisplayContent_parseListToModel_APP_UT, DisplayContent_parseListToModel_APP_UT)
{
    DisplayContent *p = new DisplayContent(nullptr);
    EXPECT_NE(p, nullptr);
    DisplayContent_parseListToModel_APP_UT_Param param = GetParam();
    QList<LOG_MSG_APPLICATOIN> list;

    if (!param.isEmptyList) {
        LOG_MSG_APPLICATOIN item;
        for (int i = 0; i < 100; ++i) {
            item.msg = QString("msg%1").arg(i);
            item.level = "Debug";
            item.dateTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
            item.src = "test_src";
            list.append(item);
        }
    }

    p->parseListToModel(list, (param.isEmptyModel ? nullptr : p->m_pModel));
    p->deleteLater();
}

class DisplayContent_parseListToModel_XORG_UT_Param
{
public:
    DisplayContent_parseListToModel_XORG_UT_Param(bool iIsEmptyList, bool iIsEmptyModel)
    {

        isEmptyList = iIsEmptyList;
        isEmptyList = iIsEmptyModel;
    }
    bool isEmptyList;
    bool isEmptyModel;


};

class DisplayContent_parseListToModel_XORG_UT : public ::testing::TestWithParam<DisplayContent_parseListToModel_XORG_UT_Param>
{
};


INSTANTIATE_TEST_SUITE_P(DisplayContent, DisplayContent_parseListToModel_XORG_UT, ::testing::Values(DisplayContent_parseListToModel_XORG_UT_Param(true, true)
                                                                                                    , DisplayContent_parseListToModel_XORG_UT_Param(true, false)
                                                                                                    , DisplayContent_parseListToModel_XORG_UT_Param(false, false)
                                                                                                   ));


TEST_P(DisplayContent_parseListToModel_XORG_UT, DisplayContent_parseListToModel_XORG_UT)
{
    DisplayContent *p = new DisplayContent(nullptr);
    EXPECT_NE(p, nullptr);
    DisplayContent_parseListToModel_XORG_UT_Param param = GetParam();
    QList<LOG_MSG_XORG> list;

    if (!param.isEmptyList) {
        LOG_MSG_XORG item;
        for (int i = 0; i < 100; ++i) {
            item.msg = QString("msg%1").arg(i);
            item.dateTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
            list.append(item);
        }
    }

    p->parseListToModel(list, (param.isEmptyModel ? nullptr : p->m_pModel));
    p->deleteLater();
}

class DisplayContent_parseListToModel_NORMAL_UT_Param
{
public:
    DisplayContent_parseListToModel_NORMAL_UT_Param(bool iIsEmptyList, bool iIsEmptyModel)
    {

        isEmptyList = iIsEmptyList;
        isEmptyList = iIsEmptyModel;
    }
    bool isEmptyList;
    bool isEmptyModel;


};

class DisplayContent_parseListToModel_NORMAL_UT : public ::testing::TestWithParam<DisplayContent_parseListToModel_NORMAL_UT_Param>
{
};


INSTANTIATE_TEST_SUITE_P(DisplayContent, DisplayContent_parseListToModel_NORMAL_UT, ::testing::Values(DisplayContent_parseListToModel_NORMAL_UT_Param(true, true)
                                                                                                      , DisplayContent_parseListToModel_NORMAL_UT_Param(true, false)
                                                                                                      , DisplayContent_parseListToModel_NORMAL_UT_Param(false, false)
                                                                                                     ));


TEST_P(DisplayContent_parseListToModel_NORMAL_UT, DisplayContent_parseListToModel_NORMAL_UT)
{
    DisplayContent *p = new DisplayContent(nullptr);
    EXPECT_NE(p, nullptr);
    DisplayContent_parseListToModel_NORMAL_UT_Param param = GetParam();
    QList<LOG_MSG_NORMAL> list;

    if (!param.isEmptyList) {
        LOG_MSG_NORMAL item;
        for (int i = 0; i < 100; ++i) {
            item.msg = QString("msg%1").arg(i);
            item.dateTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
            item.userName = "test_user";
            item.eventType = "Login";
            list.append(item);
        }
    }

    p->parseListToModel(list, (param.isEmptyModel ? nullptr : p->m_pModel));
    p->deleteLater();
}

class DisplayContent_parseListToModel_KWIN_UT_Param
{
public:
    DisplayContent_parseListToModel_KWIN_UT_Param(bool iIsEmptyList, bool iIsEmptyModel)
    {

        isEmptyList = iIsEmptyList;
        isEmptyList = iIsEmptyModel;
    }
    bool isEmptyList;
    bool isEmptyModel;


};

class DisplayContent_parseListToModel_KWIN_UT : public ::testing::TestWithParam<DisplayContent_parseListToModel_KWIN_UT_Param>
{
};


INSTANTIATE_TEST_SUITE_P(DisplayContent, DisplayContent_parseListToModel_KWIN_UT, ::testing::Values(DisplayContent_parseListToModel_KWIN_UT_Param(true, true)
                                                                                                    , DisplayContent_parseListToModel_KWIN_UT_Param(true, false)
                                                                                                    , DisplayContent_parseListToModel_KWIN_UT_Param(false, false)
                                                                                                   ));


TEST_P(DisplayContent_parseListToModel_KWIN_UT, DisplayContent_parseListToModel_KWIN_UT)
{
    DisplayContent *p = new DisplayContent(nullptr);
    EXPECT_NE(p, nullptr);
    DisplayContent_parseListToModel_KWIN_UT_Param param = GetParam();
    QList<LOG_MSG_KWIN> list;

    if (!param.isEmptyList) {
        LOG_MSG_KWIN item;
        for (int i = 0; i < 100; ++i) {
            item.msg = QString("msg%1").arg(i);
            list.append(item);
        }
    }

    p->parseListToModel(list, (param.isEmptyModel ? nullptr : p->m_pModel));
    p->deleteLater();
}

class DisplayContent_setLoadState_UT_Param
{
public:
    DisplayContent_setLoadState_UT_Param(DisplayContent::LOAD_STATE iLoadState, bool iSpinnerWgtIsHide,
                                         bool iSpinnerWgtKIsHide,
                                         bool iNoResultLabelIsHide,
                                         bool iTreeViewIsHide,
                                         bool iSpinnerWgtIsShowResult,
                                         bool iSpinnerWgtKIsShowResult,
                                         bool iNoResultLabelIsShowResult,
                                         bool iTreeViewIsShowResult)
    {

        m_loadState = iLoadState;
        m_spinnerWgtIsHide = iSpinnerWgtIsHide;
        m_spinnerWgtKIsHide = iSpinnerWgtKIsHide;
        m_noResultLabelIsHide = iNoResultLabelIsHide;
        m_treeViewIsHide = iTreeViewIsHide;
        m_spinnerWgtIsShowResult = iSpinnerWgtIsShowResult;
        m_spinnerWgtKIsShowResult = iSpinnerWgtKIsShowResult;
        m_noResultLabelIsShowResult = iNoResultLabelIsShowResult;
        m_treeViewIsShowResult = iTreeViewIsShowResult;
    }
    DisplayContent::LOAD_STATE m_loadState;
    bool m_spinnerWgtIsHide;
    bool m_spinnerWgtKIsHide;
    bool m_noResultLabelIsHide;
    bool m_treeViewIsHide;
    bool m_spinnerWgtIsShowResult;
    bool m_spinnerWgtKIsShowResult;
    bool m_noResultLabelIsShowResult;
    bool m_treeViewIsShowResult;
};

class DisplayContent_setLoadState_UT : public ::testing::TestWithParam<DisplayContent_setLoadState_UT_Param>
{
};


INSTANTIATE_TEST_SUITE_P(DisplayContent, DisplayContent_setLoadState_UT, ::testing::Values(DisplayContent_setLoadState_UT_Param(DisplayContent::DATA_LOADING, true, true, true, true, true, false, false, false)
                                                                                           , DisplayContent_setLoadState_UT_Param(DisplayContent::DATA_COMPLETE, true, true, true, true, false, false, false, true)
                                                                                           , DisplayContent_setLoadState_UT_Param(DisplayContent::DATA_LOADING_K, true, true, true, true, false, true, false, false)
                                                                                           , DisplayContent_setLoadState_UT_Param(DisplayContent::DATA_NO_SEARCH_RESULT, true, true, true, true, false, false, true, true)
                                                                                           , DisplayContent_setLoadState_UT_Param(DisplayContent::DATA_LOADING, false, false, false, false, true, false, false, false)
                                                                                           , DisplayContent_setLoadState_UT_Param(DisplayContent::DATA_COMPLETE, false, false, false, false, false, false, false, true)
                                                                                           , DisplayContent_setLoadState_UT_Param(DisplayContent::DATA_LOADING_K, false, false, false, false, false, true, false, false)
                                                                                           , DisplayContent_setLoadState_UT_Param(DisplayContent::DATA_NO_SEARCH_RESULT, false, false, false, false, false, false, true, true)
                                                                                          ));


TEST_P(DisplayContent_setLoadState_UT, DisplayContent_setLoadState_UT)
{
    DisplayContent *p = new DisplayContent(nullptr);
    EXPECT_NE(p, nullptr);
    DisplayContent_setLoadState_UT_Param param = GetParam();

    p->m_spinnerWgt->setHidden(param.m_spinnerWgtIsHide);
    p->m_spinnerWgt_K->setHidden(param.m_spinnerWgtKIsHide);
    p->noResultLabel->setHidden(param.m_noResultLabelIsHide);
    p->m_treeView->setHidden(param.m_treeViewIsHide);


    p->setLoadState(param.m_loadState);
    EXPECT_EQ(param.m_spinnerWgtIsShowResult, !p->m_spinnerWgt->isHidden());
    EXPECT_EQ(param.m_spinnerWgtKIsShowResult, !p->m_spinnerWgt_K->isHidden());
    EXPECT_EQ(param.m_noResultLabelIsShowResult, !p->noResultLabel->isHidden());
    EXPECT_EQ(param.m_treeViewIsShowResult, !p->m_treeView->isHidden());
    p->deleteLater();
}

class DisplayContent_onExportResult_UT_Param
{
public:
    DisplayContent_onExportResult_UT_Param(bool iIsSuccess, bool iNeedHide)
    {

        m_isSuccess = iIsSuccess;
        m_NeedHide = iNeedHide;

    }
    bool m_isSuccess;
    bool m_NeedHide;

};

class DisplayContent_onExportResult_UT : public ::testing::TestWithParam<DisplayContent_onExportResult_UT_Param>
{
};


INSTANTIATE_TEST_SUITE_P(DisplayContent, DisplayContent_onExportResult_UT, ::testing::Values(DisplayContent_onExportResult_UT_Param(false, true)
                                                                                             , DisplayContent_onExportResult_UT_Param(true, true)
                                                                                             , DisplayContent_onExportResult_UT_Param(true, false)
                                                                                            ));

#include "exportprogressdlg.h"
TEST_P(DisplayContent_onExportResult_UT, DisplayContent_onExportResult_UT)
{
    DisplayContent *p = new DisplayContent(nullptr);
    EXPECT_NE(p, nullptr);
    DisplayContent_onExportResult_UT_Param param = GetParam();
    if (param.m_NeedHide) {
        p->m_exportDlg->show();
    } else {
        p->m_exportDlg->hide();
    }
    p->onExportResult(param.m_isSuccess);
    p->deleteLater();
}

TEST(DisplayContent_onExportFakeCloseDlg_UT, DisplayContent_onExportFakeCloseDlg_UT)
{
    DisplayContent *p = new DisplayContent(nullptr);
    EXPECT_NE(p, nullptr);
    p->m_exportDlg->show();
    p->onExportFakeCloseDlg();
    EXPECT_EQ(p->m_exportDlg->isHidden(), true);
    p->m_exportDlg->hide();
    p->onExportFakeCloseDlg();
    EXPECT_EQ(p->m_exportDlg->isHidden(), true);
    p->deleteLater();
}

TEST(DisplayContent_clearAllFilter_UT, DisplayContent_clearAllFilter_UT)
{
    DisplayContent *p = new DisplayContent(nullptr);
    EXPECT_NE(p, nullptr);

    p->clearAllFilter();

    EXPECT_EQ(p->m_bootFilter.searchstr, "");
    EXPECT_EQ(p->m_bootFilter.statusFilter, "");
    EXPECT_EQ(p->m_currentSearchStr.isEmpty(), true);
    EXPECT_EQ(p->m_currentKwinFilter.msg, "");
    EXPECT_EQ(p->m_normalFilter.searchstr, "");
    EXPECT_EQ(p->m_normalFilter.timeFilterEnd, -1);
    EXPECT_EQ(p->m_normalFilter.timeFilterBegin, -1);
    EXPECT_EQ(p->m_normalFilter.eventTypeFilter, 0);
    p->deleteLater();
}

#include <QStandardItemModel>
TEST(DisplayContent_clearAllDatalist_UT, DisplayContent_clearAllDatalist_UT)
{
    DisplayContent *p = new DisplayContent(nullptr);
    EXPECT_NE(p, nullptr);

    p->clearAllDatalist();

    EXPECT_EQ(p->m_pModel->rowCount(), 0);
    EXPECT_EQ(p->m_pModel->columnCount(), 0);
    EXPECT_EQ(p->jList.size(), 0);
    EXPECT_EQ(p->jListOrigin.size(), 0);
    EXPECT_EQ(p->dListOrigin.size(), 0);
    EXPECT_EQ(p->xList.size(), 0);
    EXPECT_EQ(p->xListOrigin.size(), 0);
    EXPECT_EQ(p->bList.size(), 0);
    EXPECT_EQ(p->currentBootList.size(), 0);
    EXPECT_EQ(p->kList.size(), 0);
    EXPECT_EQ(p->kListOrigin.size(), 0);
    EXPECT_EQ(p->appList.size(), 0);
    EXPECT_EQ(p->appListOrigin.size(), 0);
    EXPECT_EQ(p->norList.size(), 0);
    EXPECT_EQ(p->nortempList.size(), 0);
    EXPECT_EQ(p->m_currentKwinList.size(), 0);
    EXPECT_EQ(p->m_kwinList.size(), 0);
    EXPECT_EQ(p->jBootList.size(), 0);
    EXPECT_EQ(p->jBootListOrigin.size(), 0);
    p->deleteLater();
}

class DisplayContent_filterBoot_UT_Param
{
public:
    DisplayContent_filterBoot_UT_Param(bool iIsStatusFilerEmpty, bool iIsMsgFilerEmpty)
    {

        m_isStatusFilerEmpty = iIsStatusFilerEmpty;
        m_isMsgFilerEmpty = iIsMsgFilerEmpty;

    }
    bool m_isStatusFilerEmpty;
    bool m_isMsgFilerEmpty;


};

class DisplayContent_filterBoot_UT : public ::testing::TestWithParam<DisplayContent_filterBoot_UT_Param>
{
};


INSTANTIATE_TEST_SUITE_P(DisplayContent, DisplayContent_filterBoot_UT, ::testing::Values(DisplayContent_filterBoot_UT_Param(false, false)
                                                                                         , DisplayContent_filterBoot_UT_Param(true, true)
                                                                                         , DisplayContent_filterBoot_UT_Param(true, false)
                                                                                        ));

TEST_P(DisplayContent_filterBoot_UT, DisplayContent_filterBoot_UT)
{
    DisplayContent *p = new DisplayContent(nullptr);
    EXPECT_NE(p, nullptr);
    DisplayContent_filterBoot_UT_Param param = GetParam();
    BOOT_FILTERS filter;
    filter.searchstr = param.m_isMsgFilerEmpty ? "" : "msg";
    filter.statusFilter = param.m_isStatusFilerEmpty ? "" : "OK";
    QList<LOG_MSG_BOOT> list;
    LOG_MSG_BOOT item;
    for (int i = 0; i < 100; ++i) {
        item.msg = QString("msg%1").arg(i);
        item.status = "OK";
        list.append(item);
    }
    p->bList.append(list);
    p->filterBoot(filter);
    EXPECT_EQ(p->currentBootList.size(), 100);
    p->deleteLater();
}


class DisplayContent_filterNomal_UT_Param
{
public:
    DisplayContent_filterNomal_UT_Param(bool iIsEventTypeFilterEmpty, bool iIsMsgFilerEmpty)
    {

        m_isEventTypeFilterEmpty = iIsEventTypeFilterEmpty;
        m_isMsgFilerEmpty = iIsMsgFilerEmpty;

    }
    bool m_isEventTypeFilterEmpty;
    bool m_isMsgFilerEmpty;


};

class DisplayContent_filterNomal_UT : public ::testing::TestWithParam<DisplayContent_filterNomal_UT_Param>
{
};


INSTANTIATE_TEST_SUITE_P(DisplayContent, DisplayContent_filterNomal_UT, ::testing::Values(DisplayContent_filterNomal_UT_Param(false, false)
                                                                                          , DisplayContent_filterNomal_UT_Param(true, true)
                                                                                          , DisplayContent_filterNomal_UT_Param(true, false)
                                                                                          , DisplayContent_filterNomal_UT_Param(false, true)
                                                                                         ));

TEST_P(DisplayContent_filterNomal_UT, DisplayContent_filterNomal_UT)
{
    DisplayContent *p = new DisplayContent(nullptr);
    EXPECT_NE(p, nullptr);
    DisplayContent_filterNomal_UT_Param param = GetParam();
    NORMAL_FILTERS filter;
    filter.searchstr = param.m_isMsgFilerEmpty ? "" : "msg";
    filter.eventTypeFilter = param.m_isEventTypeFilterEmpty ? -1 : 2;
    QList<LOG_MSG_NORMAL> list;
    LOG_MSG_NORMAL item;
    for (int i = 0; i < 100; ++i) {
        item.msg = QString("msg%1").arg(i);
        item.dateTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
        item.userName = "test_user";
        item.eventType = "Boot";
        list.append(item);
    }
    p->norList.append(list);
    p->filterNomal(filter);
    int resultCount = (param.m_isEventTypeFilterEmpty && (!param.m_isMsgFilerEmpty)) ? 0 : 100;

    EXPECT_EQ(p->nortempList.size(), resultCount);
    p->deleteLater();
}

TEST(DisplayContent_onExportProgress_UT, DisplayContent_onExportProgress_UT)
{
    DisplayContent *p = new DisplayContent(nullptr);
    EXPECT_NE(p, nullptr);
    p->onExportProgress(5, 100);
    p->deleteLater();
}



class DisplayContent_parseListToModel_JOURNAL_UT_Param
{
public:
    DisplayContent_parseListToModel_JOURNAL_UT_Param(bool iIsEmptyList, bool iIsEmptyModel)
    {

        isEmptyList = iIsEmptyList;
        isEmptyList = iIsEmptyModel;
    }
    bool isEmptyList;
    bool isEmptyModel;


};

class DisplayContent_parseListToModel_JOURNAL_UT : public ::testing::TestWithParam<DisplayContent_parseListToModel_JOURNAL_UT_Param>
{
};


INSTANTIATE_TEST_SUITE_P(DisplayContent, DisplayContent_parseListToModel_JOURNAL_UT, ::testing::Values(DisplayContent_parseListToModel_JOURNAL_UT_Param(true, true)
                                                                                                       , DisplayContent_parseListToModel_JOURNAL_UT_Param(true, false)
                                                                                                       , DisplayContent_parseListToModel_JOURNAL_UT_Param(false, false)
                                                                                                      ));


TEST_P(DisplayContent_parseListToModel_JOURNAL_UT, DisplayContent_parseListToModel_JOURNAL_UT)
{
    DisplayContent *p = new DisplayContent(nullptr);
    EXPECT_NE(p, nullptr);
    DisplayContent_parseListToModel_JOURNAL_UT_Param param = GetParam();
    QList<LOG_MSG_JOURNAL> list;
    if (!param.isEmptyList) {
        LOG_MSG_JOURNAL item;
        for (int i = 0; i < 100; ++i) {
            item.msg = QString("msg%1").arg(i);
            item.level = "Debug";
            item.daemonId = "1";
            item.dateTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
            item.hostName = "test_host";
            item.daemonName = "test_daemon";
            list.append(item);
        }
    }

    p->parseListToModel(list, (param.isEmptyModel ? nullptr : p->m_pModel));
    p->deleteLater();
}

class DisplayContent_getIconByname_UT_Param
{
public:
    DisplayContent_getIconByname_UT_Param(const QString &iKey, const QString &iValue)
    {

        key = iKey;
        value = iValue;
    }
    QString key;
    QString value;


};

class DisplayContent_getIconByname_UT : public ::testing::TestWithParam<DisplayContent_getIconByname_UT_Param>
{
};


INSTANTIATE_TEST_SUITE_P(DisplayContent, DisplayContent_getIconByname_UT, ::testing::Values(DisplayContent_getIconByname_UT_Param(DApplication::translate("Level", "Emergency"), "warning2.svg")
                                                                                            , DisplayContent_getIconByname_UT_Param(DApplication::translate("Level", "Alert"), "warning3.svg")
                                                                                            , DisplayContent_getIconByname_UT_Param(DApplication::translate("Level", "Critical"), "warning2.svg")
                                                                                            , DisplayContent_getIconByname_UT_Param(DApplication::translate("Level", "Error"), "wrong.svg")
                                                                                            , DisplayContent_getIconByname_UT_Param(DApplication::translate("Level", "Warning"), "warning.svg")
                                                                                            , DisplayContent_getIconByname_UT_Param(DApplication::translate("Level", "Notice"), "warning.svg")
                                                                                            , DisplayContent_getIconByname_UT_Param(DApplication::translate("Level", "Info"), "")
                                                                                            , DisplayContent_getIconByname_UT_Param(DApplication::translate("Level", "Debug"), "")
                                                                                            , DisplayContent_getIconByname_UT_Param("Warning", "warning.svg")
                                                                                            , DisplayContent_getIconByname_UT_Param("Debug", "")
                                                                                            , DisplayContent_getIconByname_UT_Param("Error", "wrong.svg")
                                                                                           ));


TEST_P(DisplayContent_getIconByname_UT, DisplayContent_getIconByname_UT)
{
    DisplayContent *p = new DisplayContent(nullptr);
    EXPECT_NE(p, nullptr);
    DisplayContent_getIconByname_UT_Param param = GetParam();
    EXPECT_EQ(p->getIconByname(param.key), param.value);
    p->deleteLater();
}

TEST(DisplayContent_createApplicationTable_UT, DisplayContent_createApplicationTable_UT_001)
{
    DisplayContent *p = new DisplayContent(nullptr);
    EXPECT_NE(p, nullptr);
    QList<LOG_MSG_APPLICATOIN> list;
    LOG_MSG_APPLICATOIN item;
    for (int i = 0; i < 100; ++i) {
        item.msg = QString("msg%1").arg(i);
        item.level = "Debug";
        item.dateTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
        item.src = "test_src";
        list.append(item);
    }
    p->createApplicationTable(list);
    p->deleteLater();
}

class DisplayContent_insertApplicationTable_UT_Param
{
public:
    DisplayContent_insertApplicationTable_UT_Param(bool iIsEndMore)
    {

        m_isEndMore = iIsEndMore;
    }
    bool m_isEndMore;
};

class DisplayContent_insertApplicationTable_UT : public ::testing::TestWithParam<DisplayContent_insertApplicationTable_UT_Param>
{
};


INSTANTIATE_TEST_SUITE_P(DisplayContent, DisplayContent_insertApplicationTable_UT, ::testing::Values(DisplayContent_insertApplicationTable_UT_Param(true),
                                                                                                     DisplayContent_insertApplicationTable_UT_Param(false)
                                                                                                    ));

TEST_P(DisplayContent_insertApplicationTable_UT, DisplayContent_insertApplicationTable_UT_001)
{
    DisplayContent *p = new DisplayContent(nullptr);
    EXPECT_NE(p, nullptr);
    DisplayContent_insertApplicationTable_UT_Param param = GetParam();
    QList<LOG_MSG_APPLICATOIN> list;
    LOG_MSG_APPLICATOIN item;
    for (int i = 0; i < 100; ++i) {
        item.msg = QString("msg%1").arg(i);
        item.level = "Debug";
        item.dateTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
        item.src = "test_src";
        list.append(item);
    }
    int end = 0;
    int start = 0;
    if (param.m_isEndMore) {
        qDebug() << "param.m_isEndMore";
        start = 0;
        end = 20;
    } else {
        qDebug() << "param.m_noEndMore";
        start = 20;
        end = 0;
    }
    p->insertApplicationTable(list, start, end);
    p->deleteLater();
}

class DisplayContent_slot_refreshClicked_UT_Param
{
public:
    DisplayContent_slot_refreshClicked_UT_Param(int iIndex)
    {
        index = iIndex;

    }
    int index;



};

class DisplayContent_slot_refreshClicked_UT : public ::testing::TestWithParam<DisplayContent_slot_refreshClicked_UT_Param>
{
};

static QString slot_refreshClicked_ModelIndex_data = "";
INSTANTIATE_TEST_SUITE_P(DisplayContent, DisplayContent_slot_refreshClicked_UT, ::testing::Values(DisplayContent_slot_refreshClicked_UT_Param(0)
                                                                                                  , DisplayContent_slot_refreshClicked_UT_Param(1)
                                                                                                  , DisplayContent_slot_refreshClicked_UT_Param(2)
                                                                                                  , DisplayContent_slot_refreshClicked_UT_Param(3)
                                                                                                  , DisplayContent_slot_refreshClicked_UT_Param(4)
                                                                                                  , DisplayContent_slot_refreshClicked_UT_Param(5)
                                                                                                  , DisplayContent_slot_refreshClicked_UT_Param(6)
                                                                                                  , DisplayContent_slot_refreshClicked_UT_Param(7)
                                                                                                  , DisplayContent_slot_refreshClicked_UT_Param(8)
                                                                                                  , DisplayContent_slot_refreshClicked_UT_Param(9)
                                                                                                 ));
QVariant slot_refreshClicked_ModelIndex_data_Func(void *obj, int arole)
{
    return QString(slot_refreshClicked_ModelIndex_data);
}
bool slot_refreshClicked_ModelIndex_isValid_Func(void *obj)
{
    return true;
}

TEST_P(DisplayContent_slot_refreshClicked_UT, DisplayContent_slot_refreshClicked_UT_001)
{
    DisplayContent_slot_refreshClicked_UT_Param param = GetParam();
    DisplayContent *p = new DisplayContent(nullptr);
    EXPECT_NE(p, nullptr);
    Stub *stub = new Stub;

    switch (param.index) {
    case 0:
        slot_refreshClicked_ModelIndex_data = JOUR_TREE_DATA;
        break;
    case 1:
        slot_refreshClicked_ModelIndex_data = DPKG_TREE_DATA;
        break;
    case 2:
        slot_refreshClicked_ModelIndex_data = XORG_TREE_DATA;
        break;
    case 3:
        slot_refreshClicked_ModelIndex_data = BOOT_TREE_DATA;
        break;
    case 4:
        slot_refreshClicked_ModelIndex_data = KERN_TREE_DATA;
        break;
    case 5:
        slot_refreshClicked_ModelIndex_data = APP_TREE_DATA;
        break;
    case 6:
        slot_refreshClicked_ModelIndex_data = LAST_TREE_DATA;
        break;
    case 7:
        slot_refreshClicked_ModelIndex_data = KWIN_TREE_DATA;
        break;
    case 8:
        slot_refreshClicked_ModelIndex_data = BOOT_KLU_TREE_DATA;
        break;
    case 9:
        slot_refreshClicked_ModelIndex_data = "";
        break;
    default:
        break;
    }

    stub->set(ADDR(QModelIndex, data), slot_refreshClicked_ModelIndex_data_Func);
    stub->set(ADDR(QModelIndex, isValid), slot_refreshClicked_ModelIndex_isValid_Func);

    p->slot_refreshClicked(QModelIndex());
    p->deleteLater();
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
