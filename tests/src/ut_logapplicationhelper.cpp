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
#include "logapplicationhelper.h"
#include <QDebug>

QByteArray stub_readLine(qint64 maxlen = 0)
{
    return "Hidden testX-Deepin-Vendor=deepin";
}

QByteArray stub_readLine001(qint64 maxlen = 0)
{
    return "NotShowIn testX-Deepin-Vendor=deepin";
}

void stub_parseField(QString path, QString name, bool isDeepin, bool isGeneric,
                     bool isName)
{
}

TEST(LogApplicationHelper_Constructor_UT, LogApplicationHelper_Constructor_UT)
{
    LogApplicationHelper *p = new LogApplicationHelper(nullptr);
    EXPECT_NE(p, nullptr);
    p->deleteLater();
}

TEST(LogApplicationHelper_init_UT, LogApplicationHelper_init_UT)
{
    LogApplicationHelper *p = new LogApplicationHelper(nullptr);
    EXPECT_NE(p, nullptr);
    p->init();
    p->deleteLater();
}
TEST(LogApplicationHelper_createDesktopFiles_UT, LogApplicationHelper_createDesktopFiles_UT)
{
    LogApplicationHelper *p = new LogApplicationHelper(nullptr);
    EXPECT_NE(p, nullptr);
    p->createDesktopFiles();
    p->deleteLater();
}

TEST(LogApplicationHelper_createDesktopFiles_UT, LogApplicationHelper_createDesktopFiles_UT001)
{
    LogApplicationHelper *p = new LogApplicationHelper(nullptr);
    EXPECT_NE(p, nullptr);
    Stub stub;
    stub.set((QByteArray(QIODevice::*)(qint64))ADDR(QIODevice, readLine), stub_readLine);
    stub.set(ADDR(LogApplicationHelper, parseField), stub_parseField);
    p->createDesktopFiles();
    p->deleteLater();
}

TEST(LogApplicationHelper_createDesktopFiles_UT, LogApplicationHelper_createDesktopFiles_UT002)
{
    LogApplicationHelper *p = new LogApplicationHelper(nullptr);
    EXPECT_NE(p, nullptr);
    Stub stub;
    stub.set((QByteArray(QIODevice::*)(qint64))ADDR(QIODevice, readLine), stub_readLine001);
    stub.set(ADDR(LogApplicationHelper, parseField), stub_parseField);
    p->createDesktopFiles();
    p->deleteLater();
}

TEST(LogApplicationHelper_createLogFiles_UT, LogApplicationHelper_createLogFiles_UT)
{
    LogApplicationHelper *p = new LogApplicationHelper(nullptr);
    EXPECT_NE(p, nullptr);
    p->createLogFiles();
    p->deleteLater();
}

class LogApplicationHelper_parseField_UT_Param
{
public:
    LogApplicationHelper_parseField_UT_Param(bool iIsPathEmpty, bool iIsDeepin, bool iIsGeneric, bool iIsName)
    {
        isPathEmpty = iIsPathEmpty;
        isDeepin = iIsDeepin;
        isGeneric = iIsGeneric;
        isName = iIsName;
    }
    bool isPathEmpty;
    bool isDeepin;
    bool isGeneric;
    bool isName;
};

class LogApplicationHelper_parseField_UT : public ::testing::TestWithParam<LogApplicationHelper_parseField_UT_Param>
{
};

INSTANTIATE_TEST_CASE_P(LogApplication, LogApplicationHelper_parseField_UT, ::testing::Values(LogApplicationHelper_parseField_UT_Param(false, true, true, true), LogApplicationHelper_parseField_UT_Param(true, true, true, true), LogApplicationHelper_parseField_UT_Param(true, true, false, true), LogApplicationHelper_parseField_UT_Param(true, false, false, true)));

TEST_P(LogApplicationHelper_parseField_UT, LogApplicationHelper_parseField_UT)
{
    LogApplicationHelper *p = new LogApplicationHelper(nullptr);
    EXPECT_NE(p, nullptr);
    LogApplicationHelper_parseField_UT_Param param = GetParam();
    QString path = param.isPathEmpty ? "" : "../sources/dde-calendar.log";
    p->parseField(path, "dde-calendar.log", param.isDeepin, param.isGeneric, param.isName);

    p->deleteLater();
}

TEST(LogApplicationHelper_getLogFile_UT, LogApplicationHelper_getLogFile_UT_001)
{
    LogApplicationHelper *p = new LogApplicationHelper(nullptr);
    EXPECT_NE(p, nullptr);
    QString path = p->getLogFile("../sources/dde-calendar.log");
    p->deleteLater();
}
TEST(LogApplicationHelper_getLogFile_UT, LogApplicationHelper_getLogFile_UT_002)
{
    LogApplicationHelper *p = new LogApplicationHelper(nullptr);
    EXPECT_NE(p, nullptr);
    QString path = p->getLogFile("");
    EXPECT_EQ(path.isEmpty(), true);
    p->deleteLater();
}

TEST(LogApplicationHelper_getMap_UT, LogApplicationHelper_getMape_UT)
{
    LogApplicationHelper *p = new LogApplicationHelper(nullptr);
    EXPECT_NE(p, nullptr);
    p->getMap();
    p->deleteLater();
}

TEST(LogApplicationHelper_transName_UT, LogApplicationHelper_transName_UT)
{
    LogApplicationHelper *p = new LogApplicationHelper(nullptr);
    EXPECT_NE(p, nullptr);
    p->transName("");
    p->deleteLater();
}
