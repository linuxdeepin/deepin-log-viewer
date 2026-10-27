// SPDX-FileCopyrightText: 2019 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "logdetailinfowidget.h"
#include "structdef.h"

#include <gtest/gtest.h>
#include <stub.h>

#include <DLabel>
#include <DHorizontalLine>
#include <DFontSizeManager>

#include <QDebug>
#include <QPaintEvent>
#include <QTest>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QTextBrowser>
#include <QIcon>

class LogDetailInfoWidget_UT : public testing::Test
{
public:
    static void SetUpTestCase()
    {
        qDebug() << "LogDetailInfoWidget SetUpTestCase";
    }
    static void TearDownTestCase()
    {
        qDebug() << "LogDetailInfoWidget TearDownTestCase";
    }
    void SetUp() override
    {
        m_widget = new logDetailInfoWidget(nullptr);
    }
    void TearDown() override
    {
        delete m_widget;
        m_widget = nullptr;
    }
    logDetailInfoWidget *m_widget = nullptr;
};

TEST_F(LogDetailInfoWidget_UT, logDetailInfoWidget_Constructor)
{
    EXPECT_NE(m_widget, nullptr);
    EXPECT_NE(m_widget->m_daemonName, nullptr);
    EXPECT_NE(m_widget->m_dateTime, nullptr);
    EXPECT_NE(m_widget->m_userName, nullptr);
    EXPECT_NE(m_widget->m_pid, nullptr);
    EXPECT_NE(m_widget->m_action, nullptr);
    EXPECT_NE(m_widget->m_status, nullptr);
    EXPECT_NE(m_widget->m_name, nullptr);
    EXPECT_NE(m_widget->m_event, nullptr);
    EXPECT_NE(m_widget->m_level, nullptr);
    EXPECT_NE(m_widget->m_userLabel, nullptr);
    EXPECT_NE(m_widget->m_pidLabel, nullptr);
    EXPECT_NE(m_widget->m_statusLabel, nullptr);
    EXPECT_NE(m_widget->m_actionLabel, nullptr);
    EXPECT_NE(m_widget->m_nameLabel, nullptr);
    EXPECT_NE(m_widget->m_eventLabel, nullptr);
    EXPECT_NE(m_widget->m_errorLabel, nullptr);
    EXPECT_NE(m_widget->m_textBrowser, nullptr);
    EXPECT_NE(m_widget->m_hline, nullptr);
    EXPECT_NE(m_widget->m_bottomLayer, nullptr);
}

TEST_F(LogDetailInfoWidget_UT, initUI)
{
    m_widget->initUI();
    EXPECT_NE(m_widget->m_daemonName, nullptr);
    EXPECT_NE(m_widget->m_dateTime, nullptr);
    EXPECT_NE(m_widget->m_level, nullptr);
    EXPECT_NE(m_widget->m_textBrowser, nullptr);
    EXPECT_NE(m_widget->m_hline, nullptr);
    EXPECT_NE(m_widget->m_bottomLayer, nullptr);
}

TEST_F(LogDetailInfoWidget_UT, cleanText)
{
    m_widget->cleanText();
    EXPECT_EQ(m_widget->m_dateTime->isHidden(), true);
    EXPECT_EQ(m_widget->m_userName->isHidden(), true);
    EXPECT_EQ(m_widget->m_userLabel->isHidden(), true);
    EXPECT_EQ(m_widget->m_pid->isHidden(), true);
    EXPECT_EQ(m_widget->m_pidLabel->isHidden(), true);
    EXPECT_EQ(m_widget->m_action->isHidden(), true);
    EXPECT_EQ(m_widget->m_actionLabel->isHidden(), true);
    EXPECT_EQ(m_widget->m_status->isHidden(), true);
    EXPECT_EQ(m_widget->m_statusLabel->isHidden(), true);
    EXPECT_EQ(m_widget->m_level->isHidden(), true);
    EXPECT_EQ(m_widget->m_daemonName->isHidden(), true);
    EXPECT_EQ(m_widget->m_textBrowser->toPlainText().isEmpty(), true);
    EXPECT_EQ(m_widget->m_name->isHidden(), true);
    EXPECT_EQ(m_widget->m_nameLabel->isHidden(), true);
    EXPECT_EQ(m_widget->m_event->isHidden(), true);
    EXPECT_EQ(m_widget->m_eventLabel->isHidden(), true);
    EXPECT_EQ(m_widget->m_errorLabel->isHidden(), true);
}

TEST_F(LogDetailInfoWidget_UT, hideLine_true)
{
    m_widget->hideLine(true);
    EXPECT_EQ(m_widget->m_hline->isHidden(), true);
}

TEST_F(LogDetailInfoWidget_UT, hideLine_false)
{
    m_widget->hideLine(false);
    EXPECT_EQ(m_widget->m_hline->isHidden(), false);
}

TEST_F(LogDetailInfoWidget_UT, setTextCustomSize)
{
    QWidget *w = new QWidget;
    m_widget->setTextCustomSize(w);
    EXPECT_NE(w, nullptr);
    delete w;
}

TEST_F(LogDetailInfoWidget_UT, fillDetailInfo_allEmpty)
{
    m_widget->fillDetailInfo("", "", "", "", QModelIndex(), "");
    EXPECT_EQ(m_widget->m_daemonName->isHidden(), true);
    EXPECT_EQ(m_widget->m_dateTime->isHidden(), true);
    EXPECT_EQ(m_widget->m_userName->isHidden(), true);
    EXPECT_EQ(m_widget->m_userLabel->isHidden(), true);
    EXPECT_EQ(m_widget->m_pid->isHidden(), true);
    EXPECT_EQ(m_widget->m_pidLabel->isHidden(), true);
    EXPECT_EQ(m_widget->m_status->isHidden(), true);
    EXPECT_EQ(m_widget->m_statusLabel->isHidden(), true);
    EXPECT_EQ(m_widget->m_action->isHidden(), true);
    EXPECT_EQ(m_widget->m_actionLabel->isHidden(), true);
    EXPECT_EQ(m_widget->m_name->isHidden(), true);
    EXPECT_EQ(m_widget->m_nameLabel->isHidden(), true);
    EXPECT_EQ(m_widget->m_event->isHidden(), true);
    EXPECT_EQ(m_widget->m_eventLabel->isHidden(), true);
    EXPECT_EQ(m_widget->m_level->isHidden(), true);
    EXPECT_EQ(m_widget->m_hline->isHidden(), false);
    EXPECT_EQ(m_widget->m_errorLabel->isHidden(), true);
    EXPECT_EQ(m_widget->m_textBrowser->toPlainText(), "");
}

TEST_F(LogDetailInfoWidget_UT, fillDetailInfo_allNonEmpty)
{
    m_widget->fillDetailInfo("daemon", "user", "pid1", "2024-01-01", QModelIndex(),
                             "msg", "status1", "action1", "uname1", "event1");
    EXPECT_EQ(m_widget->m_daemonName->isHidden(), false);
    EXPECT_EQ(m_widget->m_daemonName->text(), "daemon");
    EXPECT_EQ(m_widget->m_dateTime->isHidden(), false);
    EXPECT_EQ(m_widget->m_dateTime->text(), "2024-01-01");
    EXPECT_EQ(m_widget->m_userName->isHidden(), false);
    EXPECT_EQ(m_widget->m_userLabel->isHidden(), false);
    EXPECT_EQ(m_widget->m_userName->text(), "user");
    EXPECT_EQ(m_widget->m_pid->isHidden(), false);
    EXPECT_EQ(m_widget->m_pidLabel->isHidden(), false);
    EXPECT_EQ(m_widget->m_pid->text(), "pid1");
    EXPECT_EQ(m_widget->m_status->isHidden(), false);
    EXPECT_EQ(m_widget->m_statusLabel->isHidden(), false);
    EXPECT_EQ(m_widget->m_status->text(), "status1");
    EXPECT_EQ(m_widget->m_action->isHidden(), false);
    EXPECT_EQ(m_widget->m_actionLabel->isHidden(), false);
    EXPECT_EQ(m_widget->m_action->text(), "action1");
    EXPECT_EQ(m_widget->m_name->isHidden(), false);
    EXPECT_EQ(m_widget->m_nameLabel->isHidden(), false);
    EXPECT_EQ(m_widget->m_name->text(), "uname1");
    EXPECT_EQ(m_widget->m_event->isHidden(), false);
    EXPECT_EQ(m_widget->m_eventLabel->isHidden(), false);
    EXPECT_EQ(m_widget->m_event->text(), "event1");
    EXPECT_EQ(m_widget->m_textBrowser->toPlainText(), "msg");
}

TEST_F(LogDetailInfoWidget_UT, fillDetailInfo_dateTimeSplit_notXorg)
{
    m_widget->fillDetailInfo("mydaemon", "user", "", "2024-01-01.123", QModelIndex(), "msg");
    EXPECT_EQ(m_widget->m_dateTime->text(), "2024-01-01");
}

TEST_F(LogDetailInfoWidget_UT, fillDetailInfo_dateTimeSplit_Xorg)
{
    m_widget->fillDetailInfo("Xorg", "user", "", "2024-01-01.123", QModelIndex(), "msg");
    EXPECT_EQ(m_widget->m_dateTime->text(), "2024-01-01.123");
}

TEST_F(LogDetailInfoWidget_UT, fillDetailInfo_dateTimeNoSplit)
{
    m_widget->fillDetailInfo("daemon", "user", "", "2024-01-01", QModelIndex(), "msg");
    EXPECT_EQ(m_widget->m_dateTime->text(), "2024-01-01");
}

TEST_F(LogDetailInfoWidget_UT, fillDetailInfo_validLevel)
{
    QStandardItemModel model;
    model.setColumnCount(2);
    model.setRowCount(1);
    QStandardItem *item = new QStandardItem("levelItem");
    model.setItem(0, 0, item);
    model.setData(model.index(0, 0), "levelText", Qt::UserRole + 6);
    model.setData(model.index(0, 0), QIcon(), Qt::DecorationRole);

    QModelIndex levelIdx = model.index(0, 0);
    m_widget->fillDetailInfo("daemon", "", "", "", levelIdx, "msg");
    EXPECT_EQ(m_widget->m_level->isHidden(), false);
}

TEST_F(LogDetailInfoWidget_UT, fillDetailInfo_invalidLevel)
{
    m_widget->fillDetailInfo("daemon", "", "", "", QModelIndex(), "msg");
    EXPECT_EQ(m_widget->m_level->isHidden(), true);
}

TEST_F(LogDetailInfoWidget_UT, fillOOCDetailInfo_errorZero)
{
    m_widget->fillOOCDetailInfo("oocdata", 0);
    EXPECT_EQ(m_widget->m_textBrowser->isHidden(), false);
    EXPECT_EQ(m_widget->m_textBrowser->toPlainText(), "oocdata");
    EXPECT_EQ(m_widget->m_errorLabel->isHidden(), true);
    EXPECT_EQ(m_widget->m_daemonName->isHidden(), true);
    EXPECT_EQ(m_widget->m_dateTime->isHidden(), true);
    EXPECT_EQ(m_widget->m_hline->isHidden(), true);
}

TEST_F(LogDetailInfoWidget_UT, fillOOCDetailInfo_errorNonZero)
{
    m_widget->fillOOCDetailInfo("errormsg", 1);
    EXPECT_EQ(m_widget->m_textBrowser->isHidden(), true);
    EXPECT_EQ(m_widget->m_errorLabel->isHidden(), false);
    EXPECT_EQ(m_widget->m_errorLabel->text(), "errormsg");
}

TEST_F(LogDetailInfoWidget_UT, paintEvent)
{
    m_widget->resize(200, 100);
    m_widget->show();
    QTest::qWaitForWindowExposed(m_widget);
    m_widget->repaint();
    QTest::qWait(50);
    m_widget->hide();
    SUCCEED();
}

static QModelIndex makeIndexWithUserRole1(QStandardItemModel &model, const QString &dataStr, int cols = 6)
{
    model.setColumnCount(cols);
    model.setRowCount(1);
    for (int c = 0; c < cols; ++c) {
        if (!model.item(0, c)) {
            model.setItem(0, c, new QStandardItem(QString("col%1").arg(c)));
        }
    }
    QModelIndex idx = model.index(0, 0);
    model.setData(idx, dataStr, Qt::UserRole + 1);
    return idx;
}

TEST_F(LogDetailInfoWidget_UT, slot_DetailInfo_invalidIndex)
{
    QStandardItemModel model;
    m_widget->slot_DetailInfo(QModelIndex(), &model, "data", 0);
    SUCCEED();
}

TEST_F(LogDetailInfoWidget_UT, slot_DetailInfo_nullModel)
{
    QStandardItemModel model;
    QModelIndex idx = makeIndexWithUserRole1(model, DPKG_TABLE_DATA);
    m_widget->slot_DetailInfo(idx, nullptr, "data", 0);
    SUCCEED();
}

TEST_F(LogDetailInfoWidget_UT, slot_DetailInfo_DPKG)
{
    QStandardItemModel model;
    QModelIndex idx = makeIndexWithUserRole1(model, DPKG_TABLE_DATA);
    m_widget->slot_DetailInfo(idx, &model, "data", 0);
    EXPECT_EQ(m_widget->m_daemonName->text(), "dpkg");
}

TEST_F(LogDetailInfoWidget_UT, slot_DetailInfo_XORG)
{
    QStandardItemModel model;
    QModelIndex idx = makeIndexWithUserRole1(model, XORG_TABLE_DATA);
    m_widget->slot_DetailInfo(idx, &model, "data", 0);
    EXPECT_EQ(m_widget->m_daemonName->text(), "Xorg");
}

TEST_F(LogDetailInfoWidget_UT, slot_DetailInfo_BOOT)
{
    QStandardItemModel model;
    QModelIndex idx = makeIndexWithUserRole1(model, BOOT_TABLE_DATA);
    m_widget->slot_DetailInfo(idx, &model, "data", 0);
    EXPECT_EQ(m_widget->m_daemonName->text(), "Boot");
}

TEST_F(LogDetailInfoWidget_UT, slot_DetailInfo_KERN)
{
    QStandardItemModel model;
    QModelIndex idx = makeIndexWithUserRole1(model, KERN_TABLE_DATA);
    m_widget->slot_DetailInfo(idx, &model, "data", 0);
    EXPECT_FALSE(m_widget->m_textBrowser->toPlainText().isEmpty());
}

TEST_F(LogDetailInfoWidget_UT, slot_DetailInfo_JOUR)
{
    QStandardItemModel model;
    QModelIndex idx = makeIndexWithUserRole1(model, JOUR_TABLE_DATA);
    m_widget->slot_DetailInfo(idx, &model, "data", 0);
    SUCCEED();
}

TEST_F(LogDetailInfoWidget_UT, slot_DetailInfo_APP)
{
    QStandardItemModel model;
    QModelIndex idx = makeIndexWithUserRole1(model, APP_TABLE_DATA, 4);
    model.setData(model.index(0, 3), "appmsg", Qt::UserRole + 99);
    m_widget->slot_DetailInfo(idx, &model, "appdata", 0);
    EXPECT_EQ(m_widget->m_daemonName->text(), "appdata");
}

TEST_F(LogDetailInfoWidget_UT, slot_DetailInfo_LAST_Boot)
{
    QStandardItemModel model;
    QModelIndex idx = makeIndexWithUserRole1(model, LAST_TABLE_DATA);
    model.setData(model.index(0, 0), "Boot");
    m_widget->slot_DetailInfo(idx, &model, "data", 0);
    SUCCEED();
}

TEST_F(LogDetailInfoWidget_UT, slot_DetailInfo_LAST_shutdown)
{
    QStandardItemModel model;
    QModelIndex idx = makeIndexWithUserRole1(model, LAST_TABLE_DATA);
    model.setData(model.index(0, 0), "shutdown");
    m_widget->slot_DetailInfo(idx, &model, "data", 0);
    SUCCEED();
}

TEST_F(LogDetailInfoWidget_UT, slot_DetailInfo_LAST_other)
{
    QStandardItemModel model;
    QModelIndex idx = makeIndexWithUserRole1(model, LAST_TABLE_DATA);
    model.setData(model.index(0, 0), "login");
    m_widget->slot_DetailInfo(idx, &model, "data", 0);
    SUCCEED();
}

TEST_F(LogDetailInfoWidget_UT, slot_DetailInfo_KWIN)
{
    QStandardItemModel model;
    QModelIndex idx = makeIndexWithUserRole1(model, KWIN_TABLE_DATA, 1);
    m_widget->slot_DetailInfo(idx, &model, "data", 0);
    EXPECT_EQ(m_widget->m_daemonName->text(), "Kwin");
}

TEST_F(LogDetailInfoWidget_UT, slot_DetailInfo_BOOT_KLU)
{
    QStandardItemModel model;
    QModelIndex idx = makeIndexWithUserRole1(model, BOOT_KLU_TABLE_DATA);
    m_widget->slot_DetailInfo(idx, &model, "data", 0);
    SUCCEED();
}

TEST_F(LogDetailInfoWidget_UT, slot_DetailInfo_DNF)
{
    QStandardItemModel model;
    QModelIndex idx = makeIndexWithUserRole1(model, DNF_TABLE_DATA, 3);
    m_widget->slot_DetailInfo(idx, &model, "data", 0);
    EXPECT_EQ(m_widget->m_daemonName->text(), "dnf");
}

TEST_F(LogDetailInfoWidget_UT, slot_DetailInfo_DMESG)
{
    QStandardItemModel model;
    QModelIndex idx = makeIndexWithUserRole1(model, DMESG_TABLE_DATA, 3);
    m_widget->slot_DetailInfo(idx, &model, "data", 0);
    EXPECT_EQ(m_widget->m_daemonName->text(), "kernel");
}

TEST_F(LogDetailInfoWidget_UT, slot_DetailInfo_OOC)
{
    QStandardItemModel model;
    QModelIndex idx = makeIndexWithUserRole1(model, OOC_TABLE_DATA);
    m_widget->slot_DetailInfo(idx, &model, "oocdata", 0);
    EXPECT_EQ(m_widget->m_textBrowser->toPlainText(), "oocdata");
}

TEST_F(LogDetailInfoWidget_UT, slot_DetailInfo_OOC_error)
{
    QStandardItemModel model;
    QModelIndex idx = makeIndexWithUserRole1(model, OOC_TABLE_DATA);
    m_widget->slot_DetailInfo(idx, &model, "oocerr", 1);
    EXPECT_EQ(m_widget->m_errorLabel->text(), "oocerr");
}

TEST_F(LogDetailInfoWidget_UT, slot_DetailInfo_AUDIT)
{
    QStandardItemModel model;
    QModelIndex idx = makeIndexWithUserRole1(model, AUDIT_TABLE_DATA, 5);
    m_widget->slot_DetailInfo(idx, &model, "data", 0);
    EXPECT_EQ(m_widget->m_daemonName->text(), "audit");
}

TEST_F(LogDetailInfoWidget_UT, slot_DetailInfo_COREDUMP)
{
    QStandardItemModel model;
    QModelIndex idx = makeIndexWithUserRole1(model, COREDUMP_TABLE_DATA, 5);
    model.setData(model.index(0, 4), "coremsg", Qt::UserRole + 2);
    m_widget->slot_DetailInfo(idx, &model, "data", 0);
    SUCCEED();
}

TEST_F(LogDetailInfoWidget_UT, slot_DetailInfo_AUTH)
{
    QStandardItemModel model;
    QModelIndex idx = makeIndexWithUserRole1(model, AUTH_TABLE_DATA, 4);
    m_widget->slot_DetailInfo(idx, &model, "data", 0);
    SUCCEED();
}

TEST_F(LogDetailInfoWidget_UT, slot_DetailInfo_noMatch)
{
    QStandardItemModel model;
    QModelIndex idx = makeIndexWithUserRole1(model, "unknownData");
    m_widget->slot_DetailInfo(idx, &model, "data", 0);
    SUCCEED();
}
