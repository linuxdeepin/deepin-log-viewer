// SPDX-FileCopyrightText: 2022-2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "DebugTimeManager.h"
#include <stub.h>
#include <gtest/gtest.h>
#include <time.h>

// --- clock_gettime stub helpers ---
// Controls whether the stubbed clock_gettime reports failure.
static int g_ut_clock_fail = 0;
static int ut_stub_clock_gettime(clockid_t, struct timespec *tp)
{
    if (g_ut_clock_fail) {
        return -1;
    }
    tp->tv_sec = 100;
    tp->tv_nsec = 200;
    return 0;
}

// getInstance: returns the same non-null singleton each call.
TEST(UT_DebugTimeManager, getInstance_returnsSameSingleton)
{
    DebugTimeManager *first = DebugTimeManager::getInstance();
    DebugTimeManager *second = DebugTimeManager::getInstance();
    ASSERT_NE(first, nullptr);
    EXPECT_EQ(first, second);
}

// Protected constructor (accessible via -fno-access-control): constructs without crash.
TEST(UT_DebugTimeManager, constructor_constructsInstance)
{
    DebugTimeManager *manager = new DebugTimeManager();
    ASSERT_NE(manager, nullptr);
    manager->clear();
    delete manager;
}

// clear: removes entries previously inserted into the linux point map.
TEST(UT_DebugTimeManager, clear_emptiesPointMaps)
{
    DebugTimeManager *manager = new DebugTimeManager();
    manager->beginPointLinux("clearPoint", "clearStatus");
    ASSERT_TRUE(manager->m_MapLinuxPoint.contains("clearPoint"));
    manager->clear();
    EXPECT_TRUE(manager->m_MapLinuxPoint.isEmpty());
    EXPECT_TRUE(manager->m_MapPoint.isEmpty());
    delete manager;
}

// beginPointLinux: success path inserts the point with the given status.
TEST(UT_DebugTimeManager, beginPointLinux_insertsPointOnSuccess)
{
    DebugTimeManager *manager = new DebugTimeManager();
    manager->beginPointLinux("beginOk", "okStatus");
    EXPECT_TRUE(manager->m_MapLinuxPoint.contains("beginOk"));
    EXPECT_EQ(manager->m_MapLinuxPoint.value("beginOk").desc, QString("okStatus"));
    delete manager;
}

// beginPointLinux: clock_gettime failure path skips insertion.
TEST(UT_DebugTimeManager, beginPointLinux_skipsInsertionOnClockFail)
{
    Stub stub;
    g_ut_clock_fail = 1;
    stub.set(clock_gettime, ut_stub_clock_gettime);
    DebugTimeManager *manager = new DebugTimeManager();
    manager->beginPointLinux("beginFail", "failStatus");
    EXPECT_FALSE(manager->m_MapLinuxPoint.contains("beginFail"));
    delete manager;
    g_ut_clock_fail = 0;
}

// endPointLinux: normal path removes the previously begun point.
TEST(UT_DebugTimeManager, endPointLinux_removesPointAfterBegin)
{
    DebugTimeManager *manager = new DebugTimeManager();
    manager->beginPointLinux("endOk", "endStatus");
    ASSERT_TRUE(manager->m_MapLinuxPoint.contains("endOk"));
    manager->endPointLinux("endOk", "finished");
    EXPECT_FALSE(manager->m_MapLinuxPoint.contains("endOk"));
    delete manager;
}

// endPointLinux: unknown point is a no-op (no crash, nothing inserted).
TEST(UT_DebugTimeManager, endPointLinux_unknownPointIsNoop)
{
    DebugTimeManager *manager = new DebugTimeManager();
    manager->endPointLinux("notBegun", "status");
    EXPECT_FALSE(manager->m_MapLinuxPoint.contains("notBegun"));
    delete manager;
}

// endPointLinux: clock_gettime failure keeps the point (not removed).
TEST(UT_DebugTimeManager, endPointLinux_keepsPointOnClockFail)
{
    DebugTimeManager *manager = new DebugTimeManager();
    manager->beginPointLinux("endFail", "endFailStatus");
    ASSERT_TRUE(manager->m_MapLinuxPoint.contains("endFail"));
    Stub stub;
    g_ut_clock_fail = 1;
    stub.set(clock_gettime, ut_stub_clock_gettime);
    manager->endPointLinux("endFail", "finished");
    EXPECT_TRUE(manager->m_MapLinuxPoint.contains("endFail"));
    g_ut_clock_fail = 0;
    delete manager;
}

// diff: computes the difference between two timespecs.
TEST(UT_DebugTimeManager, diff_computesDifference)
{
    DebugTimeManager *manager = new DebugTimeManager();
    timespec start;
    start.tv_sec = 10;
    start.tv_nsec = 100;
    timespec end;
    end.tv_sec = 35;
    end.tv_nsec = 500;
    timespec result = manager->diff(start, end);
    EXPECT_EQ(result.tv_sec, 25);
    EXPECT_EQ(result.tv_nsec, 400);
    delete manager;
}
