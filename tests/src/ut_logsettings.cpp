// SPDX-FileCopyrightText: 2019-2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "logsettings.h"

#include <gtest/gtest.h>
#include <stub.h>

#include <QDebug>
#include <QSize>
TEST(LogSettings_Constructor_UT, LogSettings_Constructor_UT_001)
{
    LogSettings *p = new LogSettings(nullptr);
    EXPECT_NE(p, nullptr);

    p->deleteLater();
}

TEST(LogSettings_getConfigWinSize_UT, LogSettings_getConfigWinSize_UT_001)
{
    LogSettings *p = new LogSettings(nullptr);
    EXPECT_NE(p, nullptr);
    EXPECT_EQ(p->getConfigWinSize().isValid(), true);
    p->deleteLater();
}

TEST(LogSettings_saveConfigWinSize_UT, LogSettings_saveConfigWinSize_UT_001)
{
    LogSettings *p = new LogSettings(nullptr);
    EXPECT_NE(p, nullptr);
    p->saveConfigWinSize(100, 200);
    p->deleteLater();
}

TEST(LogSettings_instance_UT, LogSettings_instance_UT_001)
{
    LogSettings *p1 = LogSettings::instance();
    LogSettings *p2 = LogSettings::instance();
    EXPECT_NE(p1, nullptr);
    EXPECT_EQ(p1, p2);
}

TEST(LogSettings_loadAuditMap_UT, LogSettings_loadAuditMap_UT_001)
{
    auto auditMap = LogSettings::loadAuditMap();
    SUCCEED();
}
