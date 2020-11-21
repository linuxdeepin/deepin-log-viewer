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
#include "logapplication.h"
#include "logcollectormain.h"
#include <QDebug>
#include <QKeyEvent>
TEST(LogApplication_Constructor_UT, LogApplication_Constructor_UT)
{
    int argc = 0;
    char **argv;
    LogApplication *p = new LogApplication(argc, argv);
    EXPECT_NE(p, nullptr);
    p->deleteLater();
}

TEST(LogApplication_setMainWindow_UT, LogApplication_setMainWindow_UT)
{
    int argc = 0;
    char **argv;
    LogApplication *p = new LogApplication(argc, argv);
    EXPECT_NE(p, nullptr);
    LogCollectorMain *m = new LogCollectorMain(nullptr);
    p->setMainWindow(m);
    EXPECT_EQ(m, p->m_mainWindow);
    p->deleteLater();
}

class LogApplication_notify_UT_Param
{
public:
    LogApplication_notify_UT_Param(bool iIsKeyPress, bool iIsKeyTab, bool iIsMainWindowb, bool iIsSucces)
    {
        isKeyPress = iIsKeyPress;
        isKeyTab = iIsKeyTab;
        isMainWindow = iIsMainWindowb;
        isKeyPress = iIsSucces;
    }
    bool isKeyPress;
    bool isKeyTab;
    bool isMainWindow;
    bool isSucces;
};

class LogApplication_notify_UT : public ::testing::TestWithParam<LogApplication_notify_UT_Param>
{
};

static bool LogApplication_notify_mainWindow_handleApplicationTabEventNotify_result = false;
std::vector<LogApplication_notify_UT_Param> vec1 {LogApplication_notify_UT_Param(false, true, true, true),
                                                  LogApplication_notify_UT_Param(true, true, true, true),
                                                  LogApplication_notify_UT_Param(true, false, true, true),
                                                  LogApplication_notify_UT_Param(true, true, false, true),
                                                  LogApplication_notify_UT_Param(true, true, true, false)};

INSTANTIATE_TEST_CASE_P(LogApplication, LogApplication_notify_UT, ::testing::ValuesIn(vec1));
bool LogApplication_notify_mainWindow_handleApplicationTabEventNotify_result_Func(void *obj, QObject *obj1, QKeyEvent *evt)
{
    return LogApplication_notify_mainWindow_handleApplicationTabEventNotify_result;
}

//TEST_P(LogApplication_notify_UT, LogApplication_notify_UT)
//{
//    int argc = 0;
//    char **argv;
//    Stub *stub = new Stub;
//    stub->set(ADDR(LogCollectorMain, handleApplicationTabEventNotify), LogApplication_notify_mainWindow_handleApplicationTabEventNotify_result_Func);

//    LogApplication *p = new LogApplication(argc, argv);
//    EXPECT_NE(p, nullptr);
//    LogCollectorMain *m = new  LogCollectorMain(nullptr);
//    LogApplication_notify_UT_Param param = GetParam();
//    if (param.isKeyPress) {
//        int key = param.isKeyTab ? Qt::Key_Tab : Qt::Key_Left;
//        QKeyEvent   *keyEvent = new QKeyEvent(QEvent::KeyPress, key, Qt::NoModifier);
//        LogApplication_notify_mainWindow_handleApplicationTabEventNotify_result = param.isSucces;
//        p->m_mainWindow = param.isMainWindow ? p->m_mainWindow : nullptr;
//        p->notify(nullptr, keyEvent);
//    } else {
//        QEvent *e = new QEvent(QEvent::None);
//        p->notify(nullptr, e);
//    }

//    p->deleteLater();
//}
