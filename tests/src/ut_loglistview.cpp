// SPDX-FileCopyrightText: 2019 - 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "loglistview.h"
#include "dbusmanager.h"
#include "DebugTimeManager.h"
#include "dldbushandler.h"
#include "logapplicationhelper.h"
#include "ut_stubpublic.h"

#include <gtest/gtest.h>
#include <stub.h>
#include <QPainter>
#include <QDebug>
#include <QHelpEvent>
#include <QTreeView>
#include <QPaintEvent>
#include <QToolTip>
#include <QMenu>
#include <QAction>
#include <QStyle>
#include <DStyle>
#include <DSysInfo>
#include <dlfcn.h>
#include <sys/mman.h>
#include <unistd.h>

static bool stub_isSEOpen()
{
    return true;
}

QVariant stub_data(int arole)
{
    Q_UNUSED(arole);
    return QVariant("test");
}

bool stub_hasFocus()
{
    return true;
}

Dtk::Core::DSysInfo::UosEdition stub_uosProfessEditionType()
{
    return Dtk::Core::DSysInfo::UosProfessional;
}

Dtk::Core::DSysInfo::UosEdition stub_uosEulerEditionType()
{
    return Dtk::Core::DSysInfo::UosEuler;
}

QString stub_ListViewreadLog(const QString &filePath)
{
    Q_UNUSED(filePath);
    return "2021-04-06 13:29:32 install code:amd64 <none> 1.55.0-1617120720";
}

void stub_drawPrimitive(QStyle::PrimitiveElement pe, const QStyleOption *opt, QPainter *p, const QWidget *w = nullptr)
{
}

void stubbeginPointLinux(const QString &point, const QString &status)
{
}

TEST(LogListDelegate_Constructor_UT, LogListDelegate_Constructor_UT_001)
{
    Stub stub;
    stub.set(ADDR(DLDBusHandler, readLog), stub_ListViewreadLog);
    stub.set(ADDR(DebugTimeManager, beginPointLinux), stubbeginPointLinux);
    stub.set(ADDR(LogApplicationHelper, getCustomLogList), LogApplicationHelper_getCustomLogList);
    LogListView *v = new LogListView(nullptr);
    LogListDelegate *p = new LogListDelegate(v);
    EXPECT_NE(p, nullptr);
    p->deleteLater();
}

TEST(LogListDelegate_helpEvent_UT, LogListDelegate_helpEvent_UT_001)
{
    Stub stub;
    stub.set(ADDR(DLDBusHandler, readLog), stub_ListViewreadLog);
    stub.set(ADDR(DebugTimeManager, beginPointLinux), stubbeginPointLinux);
    stub.set(ADDR(LogApplicationHelper, getCustomLogList), LogApplicationHelper_getCustomLogList);
    LogListView *v = new LogListView(nullptr);
    LogListDelegate *p = new LogListDelegate(v);
    EXPECT_NE(p, nullptr);
    QStyleOptionViewItem op;
    QHelpEvent helpevent(QEvent::ToolTip, QPoint(0, 0), QPoint(0, 0));
    p->helpEvent(&helpevent, v, op, QModelIndex());
    p->deleteLater();
}

TEST(LogListDelegate_helpEvent_UT, LogListDelegate_helpEvent_UT_002)
{
    Stub stub;
    stub.set(ADDR(DLDBusHandler, readLog), stub_ListViewreadLog);
    stub.set(ADDR(DebugTimeManager, beginPointLinux), stubbeginPointLinux);
    stub.set(ADDR(LogApplicationHelper, getCustomLogList), LogApplicationHelper_getCustomLogList);
    LogListView *v = new LogListView(nullptr);
    LogListDelegate *p = new LogListDelegate(v);
    EXPECT_NE(p, nullptr);
    QStyleOptionViewItem op;
    stub.set(ADDR(QModelIndex, data), stub_data);
    QHelpEvent helpevent(QEvent::ToolTip, QPoint(0, 0), QPoint(0, 0));
    p->helpEvent(&helpevent, v, op, QModelIndex());
    p->deleteLater();
}

TEST(LogListDelegate_hideTooltipImmediately_UT, LogListDelegate_hideTooltipImmediately_UT_001)
{
    Stub stub;
    stub.set(ADDR(DLDBusHandler, readLog), stub_ListViewreadLog);
    stub.set(ADDR(DebugTimeManager, beginPointLinux), stubbeginPointLinux);
    stub.set(ADDR(LogApplicationHelper, getCustomLogList), LogApplicationHelper_getCustomLogList);
    LogListView *v = new LogListView(nullptr);
    LogListDelegate *p = new LogListDelegate(v);
    EXPECT_NE(p, nullptr);
    p->hideTooltipImmediately();
    p->deleteLater();
}

TEST(LogListView_Constructor_UT, LogListView_Constructor_UT_001)
{
    Stub stub;
    stub.set(ADDR(DLDBusHandler, readLog), stub_ListViewreadLog);
    stub.set(ADDR(DebugTimeManager, beginPointLinux), stubbeginPointLinux);
    stub.set(ADDR(LogApplicationHelper, getCustomLogList), LogApplicationHelper_getCustomLogList);
    LogListView *p = new LogListView(nullptr);
    EXPECT_NE(p, nullptr);
    p->deleteLater();
}

TEST(LogListView_initUI_UT, LogListView_initUI_UT_001)
{
    Stub stub;
    stub.set(ADDR(DLDBusHandler, readLog), stub_ListViewreadLog);
    stub.set(ADDR(DebugTimeManager, beginPointLinux), stubbeginPointLinux);
    stub.set(ADDR(Dtk::Core::DSysInfo, uosEditionType), stub_uosProfessEditionType);
    stub.set(ADDR(LogApplicationHelper, getCustomLogList), LogApplicationHelper_getCustomLogList);
    LogListView *p = new LogListView(nullptr);
    EXPECT_NE(p, nullptr);
    p->initUI();
    p->deleteLater();
}

TEST(LogListView_initUI_UT, LogListView_initUI_UT_002)
{
    Stub stub;
    stub.set(ADDR(DLDBusHandler, readLog), stub_ListViewreadLog);
    stub.set(ADDR(DebugTimeManager, beginPointLinux), stubbeginPointLinux);
    stub.set(ADDR(DBusManager, isSEOpen), stub_isSEOpen);
    stub.set(ADDR(Dtk::Core::DSysInfo, uosEditionType), stub_uosEulerEditionType);
    stub.set(ADDR(LogApplicationHelper, getCustomLogList), LogApplicationHelper_getCustomLogList);
    LogListView *p = new LogListView(nullptr);
    EXPECT_NE(p, nullptr);
    p->initUI();
    p->deleteLater();
}

TEST(LogListView_setDefaultSelect_UT, LogListView_setDefaultSelectUT_001)
{
    Stub stub;
    stub.set(ADDR(DLDBusHandler, readLog), stub_ListViewreadLog);
    stub.set(ADDR(DebugTimeManager, beginPointLinux), stubbeginPointLinux);
    stub.set(ADDR(LogApplicationHelper, getCustomLogList), LogApplicationHelper_getCustomLogList);
    LogListView *p = new LogListView(nullptr);
    EXPECT_NE(p, nullptr);
    p->setDefaultSelect();
    p->deleteLater();
}

TEST(LogListView_isFileExist_UT, LogListView_isFileExist_001)
{
    Stub stub;
    stub.set(ADDR(DLDBusHandler, readLog), stub_ListViewreadLog);
    stub.set(ADDR(DebugTimeManager, beginPointLinux), stubbeginPointLinux);
    stub.set(ADDR(LogApplicationHelper, getCustomLogList), LogApplicationHelper_getCustomLogList);
    LogListView *p = new LogListView(nullptr);
    EXPECT_NE(p, nullptr);
    bool result = p->isFileExist("");
    EXPECT_EQ(result, false);
    p->deleteLater();
}

TEST(LogListView_initCustomLogItem_UT, LogListView_initCustomLogItem_001)
{
    Stub stub;
    stub.set(ADDR(LogApplicationHelper, getCustomLogList), LogApplicationHelper_getCustomLogList);
    LogListView *p = new LogListView(nullptr);
    EXPECT_NE(p, nullptr);
    p->initUI();
    p->initCustomLogItem();
    EXPECT_NE(p->m_customLogItem, nullptr);
    p->deleteLater();
}

TEST(LogListView_currentChanged_UT, LogListView_currentChanged_UT_002)
{
    Stub stub;
    stub.set(ADDR(DLDBusHandler, readLog), stub_ListViewreadLog);
    stub.set(ADDR(DebugTimeManager, beginPointLinux), stubbeginPointLinux);
    stub.set(ADDR(LogApplicationHelper, getCustomLogList), LogApplicationHelper_getCustomLogList);
    LogListView *p = new LogListView(nullptr);
    EXPECT_NE(p, nullptr);
    p->currentChanged(QModelIndex(), QModelIndex());
    p->deleteLater();
}

TEST(LogListView_truncateFile_UT, LogListView_truncateFile_UT_002)
{
    Stub stub;
    stub.set(ADDR(DLDBusHandler, readLog), stub_ListViewreadLog);
    stub.set(ADDR(DebugTimeManager, beginPointLinux), stubbeginPointLinux);
    stub.set(ADDR(LogApplicationHelper, getCustomLogList), LogApplicationHelper_getCustomLogList);
    LogListView *p = new LogListView(nullptr);
    EXPECT_NE(p, nullptr);
    p->truncateFile("");
    p->deleteLater();
}

TEST(LogListView_slot_getAppPath_UT, LogListView_slot_getAppPath_UT_002)
{
    Stub stub;
    stub.set(ADDR(DLDBusHandler, readLog), stub_ListViewreadLog);
    stub.set(ADDR(DebugTimeManager, beginPointLinux), stubbeginPointLinux);
    stub.set(ADDR(LogApplicationHelper, getCustomLogList), LogApplicationHelper_getCustomLogList);
    LogListView *p = new LogListView(nullptr);
    EXPECT_NE(p, nullptr);
    QString path = "aa";
    p->slot_getAppPath(1, path);
    EXPECT_EQ(p->g_path, "");
    p->deleteLater();
}

TEST(LogListView_slot_valueChanged_dConfig_or_gSetting_UT, LogListView_slot_valueChanged_dConfig_or_gSetting_UT_001)
{
    Stub stub;
    stub.set(ADDR(LogApplicationHelper, getCustomLogList), LogApplicationHelper_getCustomLogList);
    LogListView *p = new LogListView(nullptr);
    EXPECT_NE(p, nullptr);
    p->slot_valueChanged_dConfig_or_gSetting("customLogFiles");
    p->deleteLater();
}

TEST(LogListView_slot_valueChanged_dConfig_or_gSetting_UT, LogListView_slot_valueChanged_dConfig_or_gSetting_UT_002)
{
    Stub stub;
    stub.set(ADDR(LogApplicationHelper, getCustomLogList), LogApplicationHelper_getCustomLogList_002);
    LogListView *p = new LogListView(nullptr);
    EXPECT_NE(p, nullptr);
    p->slot_valueChanged_dConfig_or_gSetting("customLogFiles");
    p->deleteLater();
}

TEST(LogListView_focusReson_UT, LogListView_focusReson_UT_002)
{
    Stub stub;
    stub.set(ADDR(DLDBusHandler, readLog), stub_ListViewreadLog);
    stub.set(ADDR(DebugTimeManager, beginPointLinux), stubbeginPointLinux);
    stub.set(ADDR(LogApplicationHelper, getCustomLogList), LogApplicationHelper_getCustomLogList);
    LogListView *p = new LogListView(nullptr);
    EXPECT_NE(p, nullptr);
    p->focusReson();
    p->deleteLater();
}

TEST(LogListView_rmouseMoveEvent_UT, LogListView_mouseMoveEvent_UT_001)
{
    Stub stub;
    stub.set(ADDR(DLDBusHandler, readLog), stub_ListViewreadLog);
    stub.set(ADDR(DebugTimeManager, beginPointLinux), stubbeginPointLinux);
    stub.set(ADDR(LogApplicationHelper, getCustomLogList), LogApplicationHelper_getCustomLogList);
    LogListView *p = new LogListView(nullptr);
    EXPECT_NE(p, nullptr);
    QToolTip::showText(QPoint(), "test", p);
    p->mouseMoveEvent(nullptr);
    p->deleteLater();
}

TEST(LogListView_rmouseMoveEvent_UT, LogListView_mouseMoveEvent_UT_002)
{
    Stub stub;
    stub.set(ADDR(DLDBusHandler, readLog), stub_ListViewreadLog);
    stub.set(ADDR(DebugTimeManager, beginPointLinux), stubbeginPointLinux);
    stub.set(ADDR(LogApplicationHelper, getCustomLogList), LogApplicationHelper_getCustomLogList);
    LogListView *p = new LogListView(nullptr);
    EXPECT_NE(p, nullptr);
    p->mouseMoveEvent(nullptr);
    p->deleteLater();
}

class LogListView_keyPressEvent_UT_Param
{
public:
    explicit LogListView_keyPressEvent_UT_Param(int iKey)
    {
        key = iKey;
    }
    int key;
};

class LogListView_keyPressEvent_UT : public ::testing::TestWithParam<LogListView_keyPressEvent_UT_Param>
{
};

INSTANTIATE_TEST_CASE_P(LogListView, LogListView_keyPressEvent_UT, ::testing::Values(LogListView_keyPressEvent_UT_Param(Qt::Key_Up), LogListView_keyPressEvent_UT_Param(Qt::Key_Down), LogListView_keyPressEvent_UT_Param(Qt::Key_0)));

TEST_P(LogListView_keyPressEvent_UT, LogListView_keyPressEvent_UT_001)
{
    Stub stub;
    stub.set(ADDR(DLDBusHandler, readLog), stub_ListViewreadLog);
    stub.set(ADDR(DebugTimeManager, beginPointLinux), stubbeginPointLinux);
    stub.set(ADDR(LogApplicationHelper, getCustomLogList), LogApplicationHelper_getCustomLogList);
    LogListView_keyPressEvent_UT_Param param = GetParam();
    LogListView *p = new LogListView(nullptr);
    EXPECT_NE(p, nullptr);

    QKeyEvent keyEvent(QEvent::KeyPress, param.key, Qt::NoModifier);
    p->keyPressEvent(&keyEvent);
    p->deleteLater();
}

class LogListView_mousePressEvent_UT_Param
{
public:
    explicit LogListView_mousePressEvent_UT_Param(Qt::MouseButton iKey)
        : key(iKey)
    {
    }
    Qt::MouseButton key;
};

class LogListView_mousePressEvent_UT : public ::testing::TestWithParam<LogListView_mousePressEvent_UT_Param>
{
};

INSTANTIATE_TEST_CASE_P(LogListView, LogListView_mousePressEvent_UT, ::testing::Values(LogListView_mousePressEvent_UT_Param(Qt::RightButton), LogListView_mousePressEvent_UT_Param(Qt::NoButton)));

TEST_P(LogListView_mousePressEvent_UT, LogListView_mousePressEvent_UT_001)
{
    Stub stub;
    stub.set(ADDR(DLDBusHandler, readLog), stub_ListViewreadLog);
    stub.set(ADDR(DebugTimeManager, beginPointLinux), stubbeginPointLinux);
    stub.set(ADDR(LogApplicationHelper, getCustomLogList), LogApplicationHelper_getCustomLogList);
    LogListView_mousePressEvent_UT_Param param = GetParam();
    LogListView *p = new LogListView(nullptr);
    EXPECT_NE(p, nullptr);
    QMouseEvent keyEvent(QEvent::MouseButtonPress, QPoint(1, 1), param.key, param.key, Qt::NoModifier);
    p->mousePressEvent(&keyEvent);
    p->deleteLater();
}

class LogListView_focusInEvent_UT_Param
{
public:
    explicit LogListView_focusInEvent_UT_Param(Qt::FocusReason iReason)
        : reason(iReason)
    {
    }
    Qt::FocusReason reason;
};

class LogListView_focusInEvent_UT : public ::testing::TestWithParam<LogListView_focusInEvent_UT_Param>
{
};

INSTANTIATE_TEST_CASE_P(LogListView, LogListView_focusInEvent_UT, ::testing::Values(LogListView_focusInEvent_UT_Param(Qt::PopupFocusReason), LogListView_focusInEvent_UT_Param(Qt::ActiveWindowFocusReason), LogListView_focusInEvent_UT_Param(Qt::NoFocusReason)));

TEST_P(LogListView_focusInEvent_UT, LogListView_focusInEvent_UT_001)
{
    Stub stub;
    stub.set(ADDR(DebugTimeManager, beginPointLinux), stubbeginPointLinux);
    stub.set(ADDR(DLDBusHandler, readLog), stub_ListViewreadLog);
    stub.set(ADDR(LogApplicationHelper, getCustomLogList), LogApplicationHelper_getCustomLogList);
    LogListView_focusInEvent_UT_Param param = GetParam();
    LogListView *p = new LogListView(nullptr);
    EXPECT_NE(p, nullptr);

    QFocusEvent *focusEvent = new QFocusEvent(QEvent::FocusIn, param.reason);
    p->focusInEvent(focusEvent);
    delete focusEvent;
    p->deleteLater();
}

TEST(LogListView_focusOutEventcontextMenuEvent_UT, LogListView_focusOutEvent_UT_001)
{
    Stub stub;
    stub.set(ADDR(DLDBusHandler, readLog), stub_ListViewreadLog);
    stub.set(ADDR(DebugTimeManager, beginPointLinux), stubbeginPointLinux);
    stub.set(ADDR(LogApplicationHelper, getCustomLogList), LogApplicationHelper_getCustomLogList);
    LogListView *p = new LogListView(nullptr);
    EXPECT_NE(p, nullptr);

    QFocusEvent *focusEvent = new QFocusEvent(QEvent::FocusOut, Qt::OtherFocusReason);
    p->focusOutEvent(focusEvent);
    delete focusEvent;
    p->deleteLater();
}

TEST(LogListView_focusOutEventcontextMenuEvent_UT, LogListView_showRightMenu)
{
    Stub stub;
    stub.set(ADDR(DLDBusHandler, readLog), stub_ListViewreadLog);
    stub.set(ADDR(DebugTimeManager, beginPointLinux), stubbeginPointLinux);
    stub.set((QAction * (QMenu::*)(const QPoint &, QAction *)) ADDR(QMenu, exec), stub_menu_exec);
    stub.set(ADDR(LogApplicationHelper, getCustomLogList), LogApplicationHelper_getCustomLogList);
    LogListView *p = new LogListView(nullptr);
    EXPECT_NE(p, nullptr);
    QPoint point(0, 0);
    p->showRightMenu(point, false);
    p->deleteLater();
}

TEST(LogListView_focusOutEventcontextMenuEvent_UT, LogListView_paintEvent)
{
    Stub stub;
    stub.set(ADDR(DLDBusHandler, readLog), stub_ListViewreadLog);
    stub.set(ADDR(DebugTimeManager, beginPointLinux), stubbeginPointLinux);
    stub.set(ADDR(LogApplicationHelper, getCustomLogList), LogApplicationHelper_getCustomLogList);
    LogListView *p = new LogListView(nullptr);
    EXPECT_NE(p, nullptr);

    QPaintEvent repaint(p->rect());
    p->paintEvent(&repaint);
}

// ---- new includes for additional coverage ----
#include <DDesktopServices>
#include <DDialog>
#include <QShortcut>
#include <QStandardItemModel>
#include <QImage>
#include <QTimer>
#include <QMetaObject>
#include <QAbstractItemView>
#include <QItemSelectionModel>

// ---- stubs for additional coverage ----

AppLogConfig stub_appLogConfig_valid(void * /*self*/, const QString &app)
{
    AppLogConfig cfg;
    cfg.name = app;
    cfg.visible = true;
    SubModuleConfig sm;
    sm.name = app;
    sm.logType = "file";
    sm.logPath = "/tmp/dlv_test_app.log";
    cfg.subModules.append(sm);
    return cfg;
}

AppLogConfig stub_appLogConfig_invalid(void * /*self*/, const QString &app)
{
    Q_UNUSED(app);
    AppLogConfig cfg;
    return cfg;
}

int stub_ddialog_exec_rejected()
{
    return 0;
}

static bool stub_showFileItem(const QString &localFilePath, const QString &startupId)
{
    Q_UNUSED(localFilePath);
    Q_UNUSED(startupId);
    return true;
}

bool stub_showFolder_file(const QString &localFilePath, const QString &startupId)
{
    Q_UNUSED(localFilePath);
    Q_UNUSED(startupId);
    return true;
}

// 1. getLogTypes
TEST(LogListView_getLogTypes_UT, LogListView_getLogTypes_UT_001)
{
    Stub stub;
    stub.set(ADDR(DLDBusHandler, readLog), stub_ListViewreadLog);
    stub.set(ADDR(DebugTimeManager, beginPointLinux), stubbeginPointLinux);
    stub.set(ADDR(LogApplicationHelper, getCustomLogList), LogApplicationHelper_getCustomLogList);
    LogListView *p = new LogListView(nullptr);
    EXPECT_NE(p, nullptr);
    QStringList types = p->getLogTypes();
    EXPECT_FALSE(types.isEmpty());
    p->deleteLater();
}

// 2. requestshowRightMenu with a valid index position
TEST(LogListView_requestshowRightMenu_UT, LogListView_requestshowRightMenu_UT_001)
{
    Stub stub;
    stub.set(ADDR(DLDBusHandler, readLog), stub_ListViewreadLog);
    stub.set(ADDR(DebugTimeManager, beginPointLinux), stubbeginPointLinux);
    stub.set(ADDR(LogApplicationHelper, getCustomLogList), LogApplicationHelper_getCustomLogList);
    stub.set((QAction * (QMenu::*)(const QPoint &, QAction *)) ADDR(QMenu, exec), stub_menu_exec);
    LogListView *p = new LogListView(nullptr);
    EXPECT_NE(p, nullptr);
    p->resize(QSize(200, 400));
    p->show();
    qApp->processEvents();
    QModelIndex idx = p->model()->index(0, 0);
    if (idx.isValid()) {
        p->selectionModel()->select(idx, QItemSelectionModel::ClearAndSelect);
        p->setCurrentIndex(idx);
        QPoint pos = p->visualRect(idx).center();
        p->requestshowRightMenu(pos);
    }
    p->deleteLater();
}

// 3. LogListDelegate::paint with focus branch
TEST(LogListDelegate_paint_UT, LogListDelegate_paint_UT_001)
{
    Stub stub;
    stub.set(ADDR(DLDBusHandler, readLog), stub_ListViewreadLog);
    stub.set(ADDR(DebugTimeManager, beginPointLinux), stubbeginPointLinux);
    stub.set(ADDR(LogApplicationHelper, getCustomLogList), LogApplicationHelper_getCustomLogList);
    stub.set(ADDR(QWidget, hasFocus), stub_hasFocus);
    qApp->setStyle(new Dtk::Widget::DStyle);
    LogListView *v = new LogListView(nullptr);
    LogListDelegate *p = new LogListDelegate(v);
    EXPECT_NE(p, nullptr);
    v->m_reson = Qt::TabFocusReason;
    QImage img(200, 200, QImage::Format_ARGB32);
    QPainter painter(&img);
    QStyleOptionViewItem option;
    option.widget = v;
    option.state |= QStyle::State_HasFocus;
    option.rect = QRect(0, 0, 100, 40);
    QModelIndex idx = v->model()->index(0, 0);
    p->paint(&painter, option, idx);
    painter.end();
    p->deleteLater();
    v->deleteLater();
}

// 4. constructor lambda: trigger m_rightClickTriggerShortCut activated
TEST(LogListView_rightClickShortCut_UT, LogListView_rightClickShortCut_UT_001)
{
    Stub stub;
    stub.set(ADDR(DLDBusHandler, readLog), stub_ListViewreadLog);
    stub.set(ADDR(DebugTimeManager, beginPointLinux), stubbeginPointLinux);
    stub.set(ADDR(LogApplicationHelper, getCustomLogList), LogApplicationHelper_getCustomLogList);
    stub.set((QAction * (QMenu::*)(const QPoint &, QAction *)) ADDR(QMenu, exec), stub_menu_exec);
    LogListView *p = new LogListView(nullptr);
    EXPECT_NE(p, nullptr);
    p->resize(QSize(200, 400));
    p->show();
    qApp->processEvents();
    QModelIndex idx = p->model()->index(0, 0);
    if (idx.isValid()) {
        p->selectionModel()->select(idx, QItemSelectionModel::ClearAndSelect);
        p->setCurrentIndex(idx);
    }
    QShortcut *sc = p->findChild<QShortcut *>("RightClickTriggerShortCut");
    EXPECT_NE(sc, nullptr);
    if (sc) {
        QMetaObject::invokeMethod(sc, "activated", Qt::DirectConnection);
    }
    p->deleteLater();
}

// 5. slot_getAppPath lambda: stub appLogConfig to return valid config with matching submodule
TEST(LogListView_slot_getAppPath_UT, LogListView_slot_getAppPath_UT_001)
{
    Stub stub;
    stub.set(ADDR(DLDBusHandler, readLog), stub_ListViewreadLog);
    stub.set(ADDR(DebugTimeManager, beginPointLinux), stubbeginPointLinux);
    stub.set(ADDR(LogApplicationHelper, getCustomLogList), LogApplicationHelper_getCustomLogList);
    stub.set(ADDR(LogApplicationHelper, appLogConfig), stub_appLogConfig_valid);
    LogListView *p = new LogListView(nullptr);
    EXPECT_NE(p, nullptr);
    QString app = "dlv_test_app";
    p->slot_getAppPath(0, app);
    EXPECT_EQ(p->g_path, QString("/tmp/dlv_test_app.log"));
    p->deleteLater();
}

// 6. showRightMenu lambdas: g_openForder / g_refresh / g_clear
TEST(LogListView_showRightMenuLambdas_UT, LogListView_showRightMenuLambdas_UT_001)
{
    Stub stub;
    stub.set(ADDR(DLDBusHandler, readLog), stub_ListViewreadLog);
    stub.set(ADDR(DebugTimeManager, beginPointLinux), stubbeginPointLinux);
    stub.set(ADDR(LogApplicationHelper, getCustomLogList), LogApplicationHelper_getCustomLogList);
    stub.set((QAction * (QMenu::*)(const QPoint &, QAction *)) ADDR(QMenu, exec), stub_menu_exec);
    stub.set(static_cast<bool (*)(const QString &, const QString &)>(&Dtk::Gui::DDesktopServices::showFileItem), stub_showFileItem);
    stub.set(static_cast<bool (*)(const QString &, const QString &)>(&Dtk::Gui::DDesktopServices::showFolder), stub_showFolder_file);

    LogListView *p = new LogListView(nullptr);
    EXPECT_NE(p, nullptr);
    p->resize(QSize(200, 400));
    p->show();
    qApp->processEvents();
    QModelIndex idx = p->model()->index(0, 0);
    ASSERT_TRUE(idx.isValid());
    p->selectionModel()->select(idx, QItemSelectionModel::ClearAndSelect);
    p->setCurrentIndex(idx);
    QPoint point(0, 0);
    p->showRightMenu(point, false);

    // trigger g_openForder lambda
    if (p->g_openForder) {
        p->g_openForder->setEnabled(true);
        p->g_openForder->trigger();
    }
    // trigger g_refresh lambda
    if (p->g_refresh) {
        p->g_refresh->setEnabled(true);
        p->g_refresh->trigger();
    }
    // trigger g_clear lambda
    // DDialog::exec() is virtual, so stub.h (which patches function entry
    // addresses) cannot intercept it. Instead, directly patch DDialog's
    // vtable to replace the exec() slot with a stub returning Rejected (0).
    // This avoids the nested event loop that crashes in the offscreen Qt
    // platform after state pollution from earlier tests in the full suite.
    if (p->g_clear) {
        p->g_clear->setEnabled(true);

        // Locate DDialog::exec() address from the loaded shared library
        void *dtk_handle = dlopen("libdtk6widget.so.6", RTLD_NOLOAD | RTLD_LAZY);
        ASSERT_NE(dtk_handle, nullptr) << "Failed to open libdtk6widget.so.6";
        void *real_exec = dlsym(dtk_handle, "_ZN3Dtk6Widget7DDialog4execEv");
        ASSERT_NE(real_exec, nullptr) << "Failed to find DDialog::exec symbol";

        // Create a temporary DDialog to access the class vtable
        Dtk::Widget::DDialog *tmpDlg = new Dtk::Widget::DDialog();
        void **vtable = *(void ***)tmpDlg;

        // Search the vtable for the exec() entry
        int exec_idx = -1;
        for (int i = 0; i < 256; i++) {
            if (vtable[i] == real_exec) {
                exec_idx = i;
                break;
            }
        }
        ASSERT_NE(exec_idx, -1) << "Could not find exec() in DDialog vtable";

        // Make the vtable page writable and patch the entry
        uintptr_t page_start = (uintptr_t)&vtable[exec_idx] & ~((uintptr_t)getpagesize() - 1);
        mprotect((void *)page_start, getpagesize() * 2, PROT_READ | PROT_WRITE);
        vtable[exec_idx] = reinterpret_cast<void *>(&stub_ddialog_exec_rejected);

        // Trigger g_clear — DDialog::exec() now calls our stub, returns Rejected
        p->g_clear->trigger();
        qApp->processEvents();

        // Restore the original vtable entry
        vtable[exec_idx] = real_exec;

        delete tmpDlg;
    }
    p->deleteLater();
}
