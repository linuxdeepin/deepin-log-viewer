// SPDX-FileCopyrightText: 2019-2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

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

TEST(LogApplicationHelper_initAppLog_UT, LogApplicationHelper_initAppLog_UT)
{
    LogApplicationHelper *p = new LogApplicationHelper(nullptr);
    EXPECT_NE(p, nullptr);
    p->initAppLog();
    p->deleteLater();
}

TEST(LogApplicationHelper_initOtherLog_UT, LogApplicationHelper_initOtherLog_UT)
{
    LogApplicationHelper *p = new LogApplicationHelper(nullptr);
    EXPECT_NE(p, nullptr);
    p->initOtherLog();
    p->deleteLater();
}

TEST(LogApplicationHelper_initCustomLog_UT, LogApplicationHelper_initCustomLog_UT)
{
    LogApplicationHelper *p = new LogApplicationHelper(nullptr);
    EXPECT_NE(p, nullptr);
    p->initCustomLog();
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
    stub.set(ADDR(LogApplicationHelper, generateTransName), stub_parseField);
    p->createDesktopFiles();
    p->deleteLater();
}

TEST(LogApplicationHelper_createDesktopFiles_UT, LogApplicationHelper_createDesktopFiles_UT002)
{
    LogApplicationHelper *p = new LogApplicationHelper(nullptr);
    EXPECT_NE(p, nullptr);
    Stub stub;
    stub.set((QByteArray(QIODevice::*)(qint64))ADDR(QIODevice, readLine), stub_readLine001);
    stub.set(ADDR(LogApplicationHelper, generateTransName), stub_parseField);
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
    p->generateTransName(path, "dde-calendar.log", param.isDeepin, param.isGeneric, param.isName);

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

TEST(LogApplicationHelper_getOtherLogList_UT, LogApplicationHelper_getOtherLogList_UT)
{
    LogApplicationHelper *p = new LogApplicationHelper(nullptr);
    EXPECT_NE(p, nullptr);
    p->getOtherLogList();
    p->deleteLater();
}

TEST(LogApplicationHelper_getCustomLogList_UT, LogApplicationHelper_getCustomLogList_UT)
{
    LogApplicationHelper *p = new LogApplicationHelper(nullptr);
    EXPECT_NE(p, nullptr);
    p->getCustomLogList();
    p->deleteLater();
}

TEST(LogApplicationHelper_getPathByAppId_UT, LogApplicationHelper_getPathByAppId_UT)
{
    LogApplicationHelper *p = new LogApplicationHelper(nullptr);
    EXPECT_NE(p, nullptr);
    p->getPathByAppId("deepin-log-viewer");
    p->deleteLater();
}


// ===== Tests for previously uncovered methods =====

TEST(LogApplicationHelper_createTransLogFiles_UT, LogApplicationHelper_createTransLogFiles_UT_001)
{
    LogApplicationHelper *p = new LogApplicationHelper(nullptr);
    EXPECT_NE(p, nullptr);
    p->createTransLogFiles();
    p->deleteLater();
}

TEST(LogApplicationHelper_getAppLogConfigs_UT, LogApplicationHelper_getAppLogConfigs_UT_001)
{
    LogApplicationHelper *p = new LogApplicationHelper(nullptr);
    EXPECT_NE(p, nullptr);
    auto configs = p->getAppLogConfigs();
    EXPECT_GE(configs.size(), 0);
    p->deleteLater();
}

TEST(LogApplicationHelper_getAuthLogList_UT, LogApplicationHelper_getAuthLogList_UT_001)
{
    LogApplicationHelper *p = new LogApplicationHelper(nullptr);
    EXPECT_NE(p, nullptr);
    auto list = p->getAuthLogList();
    EXPECT_GE(list.size(), 0);
    p->deleteLater();
}

TEST(LogApplicationHelper_getLastReportTime_UT, LogApplicationHelper_getLastReportTime_UT_001)
{
    LogApplicationHelper *p = new LogApplicationHelper(nullptr);
    EXPECT_NE(p, nullptr);
    QDateTime dt = p->getLastReportTime();
    (void)dt;
    p->deleteLater();
}

TEST(LogApplicationHelper_getMaxReportCoredump_UT, LogApplicationHelper_getMaxReportCoredump_UT_001)
{
    LogApplicationHelper *p = new LogApplicationHelper(nullptr);
    EXPECT_NE(p, nullptr);
    int max = p->getMaxReportCoredump();
    EXPECT_GE(max, 0);
    p->deleteLater();
}

TEST(LogApplicationHelper_initAuthLog_UT, LogApplicationHelper_initAuthLog_UT_001)
{
    LogApplicationHelper *p = new LogApplicationHelper(nullptr);
    EXPECT_NE(p, nullptr);
    p->initAuthLog();
    p->deleteLater();
}

TEST(LogApplicationHelper_isAppLogConfigExist_UT, LogApplicationHelper_isAppLogConfigExist_UT_001)
{
    LogApplicationHelper *p = new LogApplicationHelper(nullptr);
    EXPECT_NE(p, nullptr);
    p->getAppLogConfigs();
    bool ret = p->isAppLogConfigExist("");
    EXPECT_FALSE(ret);
    p->deleteLater();
}

TEST(LogApplicationHelper_isAppLogConfigExist_UT, LogApplicationHelper_isAppLogConfigExist_UT_002)
{
    LogApplicationHelper *p = new LogApplicationHelper(nullptr);
    EXPECT_NE(p, nullptr);
    p->getAppLogConfigs();
    bool ret = p->isAppLogConfigExist("deepin-log-viewer");
    (void)ret;
    p->deleteLater();
}

TEST(LogApplicationHelper_isJsonAppLogConfigExist_UT, LogApplicationHelper_isJsonAppLogConfigExist_UT_001)
{
    LogApplicationHelper *p = new LogApplicationHelper(nullptr);
    EXPECT_NE(p, nullptr);
    bool ret = p->isJsonAppLogConfigExist("");
    EXPECT_FALSE(ret);
    p->deleteLater();
}

TEST(LogApplicationHelper_isJsonAppLogConfigExist_UT, LogApplicationHelper_isJsonAppLogConfigExist_UT_002)
{
    LogApplicationHelper *p = new LogApplicationHelper(nullptr);
    EXPECT_NE(p, nullptr);
    bool ret = p->isJsonAppLogConfigExist("deepin-log-viewer");
    (void)ret;
    p->deleteLater();
}

TEST(LogApplicationHelper_isValidAppName_UT, LogApplicationHelper_isValidAppName_UT_001)
{
    LogApplicationHelper *p = new LogApplicationHelper(nullptr);
    EXPECT_NE(p, nullptr);
    bool ret = p->isValidAppName("");
    EXPECT_FALSE(ret);
    p->deleteLater();
}

TEST(LogApplicationHelper_isValidAppName_UT, LogApplicationHelper_isValidAppName_UT_002)
{
    LogApplicationHelper *p = new LogApplicationHelper(nullptr);
    EXPECT_NE(p, nullptr);
    bool ret = p->isValidAppName("nonexistent_app");
    EXPECT_FALSE(ret);
    p->deleteLater();
}

TEST(LogApplicationHelper_jsonAppLogConfig_UT, LogApplicationHelper_jsonAppLogConfig_UT_001)
{
    LogApplicationHelper *p = new LogApplicationHelper(nullptr);
    EXPECT_NE(p, nullptr);
    auto config = p->jsonAppLogConfig("");
    EXPECT_TRUE(config.name.isEmpty());
    p->deleteLater();
}

TEST(LogApplicationHelper_jsonAppLogConfig_UT, LogApplicationHelper_jsonAppLogConfig_UT_002)
{
    LogApplicationHelper *p = new LogApplicationHelper(nullptr);
    EXPECT_NE(p, nullptr);
    auto config = p->jsonAppLogConfig("nonexistent_app");
    EXPECT_TRUE(config.name.isEmpty());
    p->deleteLater();
}

TEST(LogApplicationHelper_loadAppLogConfigsByJson_UT, LogApplicationHelper_loadAppLogConfigsByJson_UT_001)
{
    LogApplicationHelper *p = new LogApplicationHelper(nullptr);
    EXPECT_NE(p, nullptr);
    p->loadAppLogConfigsByJson();
    p->deleteLater();
}

TEST(LogApplicationHelper_refreshAuthLogList_UT, LogApplicationHelper_refreshAuthLogList_UT_001)
{
    LogApplicationHelper *p = new LogApplicationHelper(nullptr);
    EXPECT_NE(p, nullptr);
    p->refreshAuthLogList();
    p->deleteLater();
}

TEST(LogApplicationHelper_saveLastRerportTime_UT, LogApplicationHelper_saveLastRerportTime_UT_001)
{
    LogApplicationHelper *p = new LogApplicationHelper(nullptr);
    EXPECT_NE(p, nullptr);
    QDateTime dt = QDateTime::currentDateTime();
    p->saveLastRerportTime(dt);
    p->deleteLater();
}

TEST(LogApplicationHelper_validityJsonLogPath_UT, LogApplicationHelper_validityJsonLogPath_UT_001)
{
    LogApplicationHelper *p = new LogApplicationHelper(nullptr);
    EXPECT_NE(p, nullptr);
    SubModuleConfig submodule;
    submodule.name = "testapp";
    submodule.logType = "file";
    submodule.logPath = "";
    p->validityJsonLogPath(submodule);
    p->deleteLater();
}

TEST(LogApplicationHelper_validityJsonLogPath_UT, LogApplicationHelper_validityJsonLogPath_UT_002)
{
    LogApplicationHelper *p = new LogApplicationHelper(nullptr);
    EXPECT_NE(p, nullptr);
    SubModuleConfig submodule;
    submodule.name = "testapp";
    submodule.logType = "file";
    submodule.logPath = "~/nonexistent_path/test.log";
    p->validityJsonLogPath(submodule);
    p->deleteLater();
}

TEST(LogApplicationHelper_validityJsonLogPath_UT, LogApplicationHelper_validityJsonLogPath_UT_003)
{
    LogApplicationHelper *p = new LogApplicationHelper(nullptr);
    EXPECT_NE(p, nullptr);
    SubModuleConfig submodule;
    submodule.name = "testapp";
    submodule.logType = "journal";
    submodule.logPath = "";
    p->validityJsonLogPath(submodule);
    p->deleteLater();
}

TEST(LogApplicationHelper_appLogConfig_UT, LogApplicationHelper_appLogConfig_UT_001)
{
    LogApplicationHelper *p = new LogApplicationHelper(nullptr);
    EXPECT_NE(p, nullptr);
    auto config = p->appLogConfig("");
    EXPECT_TRUE(config.name.isEmpty());
    p->deleteLater();
}

TEST(LogApplicationHelper_appLogConfig_UT, LogApplicationHelper_appLogConfig_UT_002)
{
    LogApplicationHelper *p = new LogApplicationHelper(nullptr);
    EXPECT_NE(p, nullptr);
    auto config = p->appLogConfig("nonexistent_app");
    EXPECT_TRUE(config.name.isEmpty());
    p->deleteLater();
}
