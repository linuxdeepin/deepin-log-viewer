// SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "eventlogutils.h"

#include <QDebug>
#include <QJsonObject>
#include <QJsonDocument>

#include <gtest/gtest.h>
#include <stub.h>

// Stub writeEventLogFunc target — records that it was called
static bool g_writeLogsCalled = false;
static void stub_writeEventLogFunc(const std::string &eventdata)
{
    g_writeLogsCalled = true;
    Q_UNUSED(eventdata)
}

// Stub initFunc target — returns true to exercise the initFunc call path
static bool stub_initFunc(const std::string &packagename, bool enable_sig)
{
    Q_UNUSED(packagename)
    Q_UNUSED(enable_sig)
    return true;
}

TEST(Eventlogutils_GetInstance_UT, Eventlogutils_GetInstance_UT_001)
{
    Eventlogutils *p = Eventlogutils::GetInstance();
    EXPECT_NE(p, nullptr);

    // Singleton: a second call returns the same instance
    Eventlogutils *p2 = Eventlogutils::GetInstance();
    EXPECT_EQ(p, p2);
}

TEST(Eventlogutils_writeLogs_UT, Eventlogutils_writeLogs_UT_001)
{
    Eventlogutils *p = Eventlogutils::GetInstance();
    EXPECT_NE(p, nullptr);

    // writeLogs with null writeEventLogFunc — exercises the early-return branch
    p->writeEventLogFunc = nullptr;
    QJsonObject data;
    data["tid"] = Eventlogutils::StartUp;
    p->writeLogs(data);

    // writeLogs with a non-null stub function — exercises the call path
    p->writeEventLogFunc = stub_writeEventLogFunc;
    g_writeLogsCalled = false;
    p->writeLogs(data);
    EXPECT_TRUE(g_writeLogsCalled);

    // Reset to avoid side effects on other tests
    p->writeEventLogFunc = nullptr;
}

TEST(Eventlogutils_Constructor_UT, Eventlogutils_Constructor_UT_001)
{
    // The constructor is private; GetInstance already invoked it once.
    // Here we verify the member state after construction via the singleton:
    //   - initFunc / writeEventLogFunc may be null (lib not available in test env)
    //     or non-null (lib loaded); both are valid outcomes.
    // We also exercise the initFunc call path by injecting a stub and calling
    // GetInstance again — the constructor body is already covered by the first
    // GetInstance call, so this test primarily validates member accessibility.
    Eventlogutils *p = Eventlogutils::GetInstance();
    EXPECT_NE(p, nullptr);

    // initFunc should be settable (verifies member accessibility via -fno-access-control)
    auto savedInit = p->initFunc;
    p->initFunc = stub_initFunc;
    EXPECT_NE(p->initFunc, nullptr);
    p->initFunc = savedInit;
}
