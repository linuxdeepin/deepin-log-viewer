// SPDX-FileCopyrightText: 2024-2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include <gtest/gtest.h>
#include <QThread>
#include <QCoreApplication>
#include <QVariant>
#include <QList>
#include <QMetaObject>
#include <functional>
#include <DConfig>

#include "configs/json/org_deepin_log_viewer.hpp"

using DTK_CORE_NAMESPACE::DConfig;
using DTK_CORE_NAMESPACE::DConfigBackend;

// ---------------------------------------------------------------------------
// Mock DConfigBackend — controllable backend for DConfig
// ---------------------------------------------------------------------------
class MockDConfigBackend : public DConfigBackend
{
public:
    bool isValid() const override { return true; }
    bool load(const QString &) override { return true; }
    QStringList keyList() const override
    {
        return {"coredumpReportMax", "coredumpReportTime",
                "customLogFiles", "log_rules", "specialComType"};
    }
    QVariant value(const QString &key, const QVariant &fallback) const override
    {
        return m_values.value(key, fallback);
    }
    void setValue(const QString &key, const QVariant &value) override
    {
        m_values[key] = value;
    }
    QString name() const override { return QStringLiteral("mock"); }
    bool isDefaultValue(const QString &) const override { return false; }
    bool isReadOnly(const QString &) const override { return false; }

    QMap<QString, QVariant> m_values;
};

// ---------------------------------------------------------------------------
// Helper: block until all pending events on *thread* have been processed.
// The sync object is moved to *thread* so the BlockingQueuedConnection
// executes there, after any previously-queued events (e.g. constructor lambda).
// ---------------------------------------------------------------------------
static void syncThread(QThread *thread)
{
    auto *syncObj = new QObject();
    syncObj->moveToThread(thread);
    QMetaObject::invokeMethod(syncObj, [syncObj]() {
        syncObj->deleteLater();
    }, Qt::BlockingQueuedConnection);
}

// ---------------------------------------------------------------------------
// Helper: run a lambda on *thread* (BlockingQueuedConnection).
// The config lives on the worker thread, so updateValue / valueChanged
// must be invoked there to satisfy the Q_ASSERT thread check.
// ---------------------------------------------------------------------------
static void runOnThread(QThread *thread, std::function<void()> fn)
{
    auto *runner = new QObject();
    runner->moveToThread(thread);
    QMetaObject::invokeMethod(runner, [runner, fn]() {
        fn();
        runner->deleteLater();
    }, Qt::BlockingQueuedConnection);
}

// ---------------------------------------------------------------------------
// Helper: create a MockDConfigBackend pre-populated with known values
// ---------------------------------------------------------------------------
static MockDConfigBackend *createMockBackend()
{
    auto *backend = new MockDConfigBackend();
    backend->m_values["coredumpReportMax"]   = QVariant(100.0);
    backend->m_values["coredumpReportTime"]  = QVariant(QString("2024-06-01"));
    backend->m_values["customLogFiles"]      = QVariant(QList<QVariant>{QVariant("a.log")});
    backend->m_values["log_rules"]           = QVariant(QString("*.debug=false"));
    backend->m_values["specialComType"]      = QVariant(7.0);
    return backend;
}

// ====================================================================
// Constructor tests — each constructor has its own lambda function
// that must be executed on the worker thread for coverage.
// ====================================================================

// Constructor 1: (QThread*, appId, name, subpath, parent)
TEST(OrgDeepinLogViewer_Constructor_UT, Constructor_001)
{
    QThread thread;
    thread.start();
    auto *obj = new org_deepin_log_viewer(&thread,
        QStringLiteral("org.deepin.log.viewer"),
        QStringLiteral("org.deepin.log.viewer"),
        QStringLiteral(""), nullptr);
    syncThread(&thread);
    QCoreApplication::processEvents(QEventLoop::AllEvents, 200);
    EXPECT_NE(obj, nullptr);
    thread.quit();
    thread.wait();
    delete obj;
    QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}

// Constructor 2: (QThread*, DConfigBackend*, appId, name, subpath, parent)
TEST(OrgDeepinLogViewer_Constructor_UT, Constructor_002)
{
    QThread thread;
    thread.start();
    auto *backend = createMockBackend();
    auto *obj = new org_deepin_log_viewer(&thread, backend,
        QStringLiteral("org.deepin.log.viewer"),
        QStringLiteral("org.deepin.log.viewer"),
        QStringLiteral(""), nullptr);
    syncThread(&thread);
    QCoreApplication::processEvents(QEventLoop::AllEvents, 200);
    EXPECT_NE(obj, nullptr);
    thread.quit();
    thread.wait();
    delete obj;
    QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}

// Constructor 3: (QThread*, name, subpath, parent)
TEST(OrgDeepinLogViewer_Constructor_UT, Constructor_003)
{
    QThread thread;
    thread.start();
    auto *obj = new org_deepin_log_viewer(&thread,
        QStringLiteral("org.deepin.log.viewer"),
        QStringLiteral(""), nullptr);
    syncThread(&thread);
    QCoreApplication::processEvents(QEventLoop::AllEvents, 200);
    EXPECT_NE(obj, nullptr);
    thread.quit();
    thread.wait();
    delete obj;
    QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}

// Constructor 4: (QThread*, DConfigBackend*, name, subpath, parent)
TEST(OrgDeepinLogViewer_Constructor_UT, Constructor_004)
{
    QThread thread;
    thread.start();
    auto *backend = createMockBackend();
    auto *obj = new org_deepin_log_viewer(&thread, backend,
        QStringLiteral("org.deepin.log.viewer"),
        QStringLiteral(""), nullptr);
    syncThread(&thread);
    QCoreApplication::processEvents(QEventLoop::AllEvents, 200);
    EXPECT_NE(obj, nullptr);
    thread.quit();
    thread.wait();
    delete obj;
    QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}

// ====================================================================
// Getter/Setter tests — setters set member variables directly (no
// thread assertion), so these can be called from the main thread.
// ====================================================================
TEST(OrgDeepinLogViewer_GetterSetter_UT, GetterSetter_001)
{
    QThread thread;
    thread.start();
    auto *backend = createMockBackend();
    auto *obj = new org_deepin_log_viewer(&thread, backend,
        QStringLiteral("org.deepin.log.viewer"),
        QStringLiteral("org.deepin.log.viewer"),
        QStringLiteral(""), nullptr);
    syncThread(&thread);
    QCoreApplication::processEvents(QEventLoop::AllEvents, 200);

    // Test setters + getters (keep worker thread running so setter
    // lambdas posted to config can execute)
    obj->setCoredumpReportMax(200.0);
    EXPECT_EQ(obj->coredumpReportMax(), 200.0);

    obj->setCoredumpReportTime(QStringLiteral("2025-01-01"));
    EXPECT_EQ(obj->coredumpReportTime(), QStringLiteral("2025-01-01"));

    QList<QVariant> files{QVariant("b.log"), QVariant("c.log")};
    obj->setCustomLogFiles(files);
    EXPECT_EQ(obj->customLogFiles(), files);

    obj->setLog_rules(QStringLiteral("*.info=true"));
    EXPECT_EQ(obj->log_rules(), QStringLiteral("*.info=true"));

    obj->setSpecialComType(42.0);
    EXPECT_EQ(obj->specialComType(), 42.0);

    // Flush setter lambdas posted to the worker thread
    syncThread(&thread);
    QCoreApplication::processEvents(QEventLoop::AllEvents, 100);

    thread.quit();
    thread.wait();
    delete obj;
    QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}

// ====================================================================
// updateValue tests — updateValue asserts it runs on the config's
// thread (the worker thread), so we invoke it there via runOnThread.
// ====================================================================
TEST(OrgDeepinLogViewer_UpdateValue_UT, UpdateValue_001)
{
    QThread thread;
    thread.start();
    auto *backend = createMockBackend();
    backend->m_values["coredumpReportMax"]  = QVariant(999.0);
    backend->m_values["coredumpReportTime"] = QVariant(QString("2099-12-31"));
    backend->m_values["customLogFiles"]     = QVariant(QList<QVariant>{QVariant("z.log")});
    backend->m_values["log_rules"]          = QVariant(QString("*.warning=false"));
    backend->m_values["specialComType"]     = QVariant(88.0);

    auto *obj = new org_deepin_log_viewer(&thread, backend,
        QStringLiteral("org.deepin.log.viewer"),
        QStringLiteral("org.deepin.log.viewer"),
        QStringLiteral(""), nullptr);
    syncThread(&thread);
    QCoreApplication::processEvents(QEventLoop::AllEvents, 200);

    // Call updateValue from the worker thread (config lives there)
    runOnThread(&thread, [obj]() {
        obj->updateValue(QStringLiteral("coredumpReportMax"));
        obj->updateValue(QStringLiteral("coredumpReportTime"));
        obj->updateValue(QStringLiteral("customLogFiles"));
        obj->updateValue(QStringLiteral("log_rules"));
        obj->updateValue(QStringLiteral("specialComType"));
        // No-match key — exercises the fall-through path
        obj->updateValue(QStringLiteral("nonexistent_key"));
        // updateValue with explicit fallback
        obj->updateValue(QStringLiteral("coredumpReportMax"), QVariant(55.0));
    });
    // Process the QMetaObject::invokeMethod lambdas posted to main thread
    QCoreApplication::processEvents(QEventLoop::AllEvents, 200);

    EXPECT_EQ(obj->coredumpReportMax(), 999.0);
    EXPECT_EQ(obj->coredumpReportTime(), QStringLiteral("2099-12-31"));
    EXPECT_EQ(obj->customLogFiles().size(), 1);
    EXPECT_EQ(obj->log_rules(), QStringLiteral("*.warning=false"));
    EXPECT_EQ(obj->specialComType(), 88.0);

    thread.quit();
    thread.wait();
    delete obj;
    QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}

// ====================================================================
// markPropertySet / testPropertySet tests
// ====================================================================
TEST(OrgDeepinLogViewer_PropertySet_UT, PropertySet_001)
{
    QThread thread;
    thread.start();
    auto *backend = createMockBackend();
    auto *obj = new org_deepin_log_viewer(&thread, backend,
        QStringLiteral("org.deepin.log.viewer"),
        QStringLiteral("org.deepin.log.viewer"),
        QStringLiteral(""), nullptr);
    syncThread(&thread);
    QCoreApplication::processEvents(QEventLoop::AllEvents, 200);

    // After initialize, testPropertySet should return false for all (status was 0)
    EXPECT_FALSE(obj->testPropertySet(0));
    EXPECT_FALSE(obj->testPropertySet(1));
    EXPECT_FALSE(obj->testPropertySet(2));
    EXPECT_FALSE(obj->testPropertySet(3));
    EXPECT_FALSE(obj->testPropertySet(4));

    // Call setter which calls markPropertySet (keep worker thread running)
    obj->setCoredumpReportMax(200.0);
    EXPECT_TRUE(obj->testPropertySet(0));

    obj->setCoredumpReportTime(QStringLiteral("x"));
    EXPECT_TRUE(obj->testPropertySet(1));

    obj->setCustomLogFiles(QList<QVariant>{});
    EXPECT_TRUE(obj->testPropertySet(2));

    obj->setLog_rules(QStringLiteral("y"));
    EXPECT_TRUE(obj->testPropertySet(3));

    obj->setSpecialComType(1.0);
    EXPECT_TRUE(obj->testPropertySet(4));

    // Flush setter lambdas posted to the worker thread
    syncThread(&thread);
    QCoreApplication::processEvents(QEventLoop::AllEvents, 100);

    thread.quit();
    thread.wait();
    delete obj;
    QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}

// ====================================================================
// Connect lambda test — emit valueChanged from the worker thread
// (DirectConnection runs on emitter's thread) to trigger the connect
// lambda registered in initialize(), which calls updateValue.
// ====================================================================
TEST(OrgDeepinLogViewer_ConnectLambda_UT, ConnectLambda_001)
{
    QThread thread;
    thread.start();
    auto *backend = createMockBackend();
    auto *obj = new org_deepin_log_viewer(&thread, backend,
        QStringLiteral("org.deepin.log.viewer"),
        QStringLiteral("org.deepin.log.viewer"),
        QStringLiteral(""), nullptr);
    syncThread(&thread);
    QCoreApplication::processEvents(QEventLoop::AllEvents, 200);

    auto *config = obj->m_config.loadRelaxed();
    ASSERT_NE(config, nullptr);

    // Emit valueChanged from the worker thread (config's thread)
    runOnThread(&thread, [config]() {
        config->valueChanged(QStringLiteral("coredumpReportMax"));
        config->valueChanged(QStringLiteral("coredumpReportTime"));
        config->valueChanged(QStringLiteral("customLogFiles"));
        config->valueChanged(QStringLiteral("log_rules"));
        config->valueChanged(QStringLiteral("specialComType"));
        // Non-matching key — connect lambda calls updateValue which falls through
        config->valueChanged(QStringLiteral("unknown_key"));
    });
    // Process the QMetaObject::invokeMethod lambdas posted to main thread
    QCoreApplication::processEvents(QEventLoop::AllEvents, 200);

    thread.quit();
    thread.wait();
    delete obj;
    QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}
