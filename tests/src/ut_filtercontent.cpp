// SPDX-FileCopyrightText: 2019 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "filtercontent.h"
#include "logperiodbutton.h"
#include "lognormalbutton.h"
#include "structdef.h"
#include "logcombox.h"
#include "qtcompat.h"

#include <QPaintEvent>
#include <QEvent>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QDebug>

#include <stub.h>
#include <gtest/gtest.h>

bool setSelection_IsVisible()
{
    return true;
}

class FilterContent_UT : public testing::Test
{
public:
    //添加日志
    static void SetUpTestCase()
    {
        qDebug() << "SetUpTestCase" << ENDL;
    }
    static void TearDownTestCase()
    {
        qDebug() << "TearDownTestCase" << ENDL;
    }
    void SetUp() //TEST跑之前会执行SetUp
    {
        m_filter = new FilterContent;
        qDebug() << "SetUp" << ENDL;
    }
    void TearDown() //TEST跑完之后会执行TearDown
    {
        delete m_filter;
    }
    FilterContent *m_filter;
};

TEST_F(FilterContent_UT, shortCutExport_UT)
{
    m_filter->shortCutExport();
    EXPECT_NE(m_filter, nullptr)<<"check the status after shortCutExport()";
    EXPECT_NE(m_filter->exportBtn, nullptr)<<"check the status after shortCutExport()";
}

TEST_F(FilterContent_UT, setAppComboBoxItem_UT)
{
    m_filter->setAppComboBoxItem();
    EXPECT_NE(m_filter->cbx_app,nullptr)<<"check the status after setAppComboBoxItem()";
}

TEST_F(FilterContent_UT, setSelection_UT)
{
    Stub stub;
    stub.set(ADDR(QWidget, isVisible), setSelection_IsVisible);
    FILTER_CONFIG fitler;
    m_filter->setSelection(fitler);
    EXPECT_NE(m_filter->cbx_app,nullptr)<<"check the status after setSelection()";
    EXPECT_NE(m_filter->cbx_lv,nullptr)<<"check the status after setSelection()";
}

TEST_F(FilterContent_UT, paintEvent_UT)
{
    QPaintEvent repaint(m_filter->rect());
    m_filter->paintEvent(&repaint);
    EXPECT_NE(m_filter, nullptr)<<"check the status after paintEvent()";
}

TEST_F(FilterContent_UT, eventFilter_UT)
{
    QKeyEvent keyEvent(QEvent::KeyPress, Qt::Key_Right, Qt::NoModifier);
    m_filter->eventFilter(m_filter->m_allBtn, &keyEvent);
    m_filter->eventFilter(m_filter->m_todayBtn, &keyEvent);
    m_filter->eventFilter(m_filter->m_threeDayBtn, &keyEvent);
    m_filter->eventFilter(m_filter->m_lastWeekBtn, &keyEvent);
    m_filter->eventFilter(m_filter->m_lastMonthBtn, &keyEvent);
    m_filter->eventFilter(m_filter->m_threeMonthBtn, &keyEvent);

    QKeyEvent keyLeftEvent(QEvent::KeyPress, Qt::Key_Left, Qt::NoModifier);
    m_filter->eventFilter(m_filter->m_allBtn, &keyLeftEvent);
    m_filter->eventFilter(m_filter->m_todayBtn, &keyLeftEvent);
    m_filter->eventFilter(m_filter->m_threeDayBtn, &keyLeftEvent);
    m_filter->eventFilter(m_filter->m_lastWeekBtn, &keyLeftEvent);
    m_filter->eventFilter(m_filter->m_lastMonthBtn, &keyLeftEvent);
    m_filter->eventFilter(m_filter->m_threeMonthBtn, &keyLeftEvent);
    EXPECT_NE(m_filter, nullptr)<<"check the status after paintEvent()";
}

TEST_F(FilterContent_UT, updateWordWrap_UT)
{
    m_filter->updateWordWrap();
    m_filter->rect().setWidth(500);
    LogPeriodButton btn("test", m_filter);
    m_filter->hLayout_period->addWidget(&btn);
    m_filter->updateWordWrap();
    EXPECT_NE(m_filter, nullptr)<<"check the status after updateWordWrap()";
}

TEST_F(FilterContent_UT, slot_logCatelogueClicked_UT)
{
    QStandardItemModel m_model;
    QStandardItem *item;

    for (int i = 0; i < 10; i++) {
        item = new QStandardItem;
        m_model.appendRow(item);
    }
    m_model.setData(m_model.index(0, 0), QString("application"), Qt::UserRole + 66);
    m_model.setData(m_model.index(1, 0), QString("journalctl"), Qt::UserRole + 66);
    m_model.setData(m_model.index(2, 0), QString("/var/log/boot.log"), Qt::UserRole + 66);
    m_model.setData(m_model.index(3, 0), QString("/var/log/kern.log"), Qt::UserRole + 66);
    m_model.setData(m_model.index(4, 0), QString("/var/log/dpkg.log"), Qt::UserRole + 66);
    m_model.setData(m_model.index(5, 0), QString("/var/log/Xorg.0.log"), Qt::UserRole + 66);
    m_model.setData(m_model.index(6, 0), QString("last"), Qt::UserRole + 66);
    m_model.setData(m_model.index(7, 0), QString("bootklu"), Qt::UserRole + 66);
    m_model.setData(m_model.index(8, 0), QString("/var/log/dnf.log"), Qt::UserRole + 66);
    m_model.setData(m_model.index(9, 0), QString("dmesg"), Qt::UserRole + 66);
    m_model.setData(m_model.index(10, 0), QString(Utils::homePath + "/.kwin.log"), Qt::UserRole + 66);
    for (int i = 0; i < m_model.rowCount(); i++) {
        QModelIndex modelindex = m_model.index(i, 0, QModelIndex());
        m_filter->slot_logCatelogueClicked(modelindex);
        EXPECT_EQ(m_filter->m_curTreeIndex, modelindex)<<"check the status after slot_logCatelogueClicked()";
    }
    delete item;
}

TEST_F(FilterContent_UT, slot_exportButtonClicked_UT)
{
    m_filter->slot_exportButtonClicked();
    EXPECT_NE(m_filter, nullptr)<<"check the status after slot_exportButtonClicked()";
}

TEST_F(FilterContent_UT, slot_cbxLvIdxChanged_UT)
{
    m_filter->slot_cbxLvIdxChanged(2);
    EXPECT_EQ(m_filter->m_curLvCbxId, 1)<<"check the status after slot_cbxLvIdxChanged()";
}

TEST_F(FilterContent_UT, slot_cbxLogTypeChanged_UT)
{
    m_filter->slot_cbxLogTypeChanged(2);
    EXPECT_NE(m_filter, nullptr)<<"check the status after slot_cbxLogTypeChanged()";
}

TEST_F(FilterContent_UT, slot_logCatelogueRefresh_UT)
{
    QStandardItemModel m_model;
    QStandardItem item;
    m_model.appendRow(&item);
    m_model.setData(m_model.index(0, 0), QString("application"), Qt::UserRole + 66);
    m_filter->slot_logCatelogueRefresh(m_model.index(0, 0));
    EXPECT_NE(m_filter->cbx_app,nullptr)<<"check the status after slot_logCatelogueRefresh()";
}

TEST_F(FilterContent_UT, slot_buttonClicked_UT)
{
    m_filter->slot_buttonClicked(BUTTONID::RESET);
    EXPECT_EQ(m_filter->m_curBtnId,ALL)<<"check the status after slot_buttonClicked()";
}

// ======================== Additional tests for 100% function coverage ========================

TEST_F(FilterContent_UT, getSetLeftButtonState_UT)
{
    m_filter->setLeftButtonState(false);
    EXPECT_FALSE(m_filter->getLeftButtonState());
    m_filter->setLeftButtonState(true);
    EXPECT_TRUE(m_filter->getLeftButtonState());
}

TEST_F(FilterContent_UT, getSetChangedcomboxstate_UT)
{
    m_filter->setChangedcomboxstate(false);
    EXPECT_FALSE(m_filter->getChangedcomboxstate());
    m_filter->setChangedcomboxstate(true);
    EXPECT_TRUE(m_filter->getChangedcomboxstate());
}

TEST_F(FilterContent_UT, setExportButtonEnable_UT)
{
    ASSERT_NE(m_filter->exportBtn, nullptr);
    m_filter->setExportButtonEnable(true);
    EXPECT_TRUE(m_filter->exportBtn->isEnabled());
    m_filter->setExportButtonEnable(false);
    EXPECT_FALSE(m_filter->exportBtn->isEnabled());
}

TEST_F(FilterContent_UT, slot_cbxStatusChanged_UT)
{
    // idx 0 -> str empty ("All")
    m_filter->slot_cbxStatusChanged(0);
    // idx 1 -> str "OK"
    m_filter->slot_cbxStatusChanged(1);
    // idx 2 -> str "Failed"
    m_filter->slot_cbxStatusChanged(2);
    SUCCEED();
}

TEST_F(FilterContent_UT, slot_cbxAuditTypeChanged_UT)
{
    m_filter->slot_cbxAuditTypeChanged(1);
    SUCCEED();
}

TEST_F(FilterContent_UT, slot_cbxDnfLvIdxChanged_UT)
{
    ASSERT_GT(m_filter->cbx_dnf_lv->count(), 0);
    m_filter->slot_cbxDnfLvIdxChanged(0);
    SUCCEED();
}

TEST_F(FilterContent_UT, slot_cbxAppIdxChanged_UT)
{
    // Add a dummy item so itemData returns a value
    m_filter->cbx_app->addItem("testApp");
    m_filter->cbx_app->setItemData(0, "testApp", Qt::UserRole + 1);
    m_filter->slot_cbxAppIdxChanged(0);
    SUCCEED();
}

TEST_F(FilterContent_UT, setSubmodulesComboBoxItem_UT)
{
    // Call with empty app name; LogApplicationHelper returns default config
    m_filter->setSubmodulesComboBoxItem("");
    EXPECT_NE(m_filter->cbx_submodule, nullptr);
}

TEST_F(FilterContent_UT, slot_cbxSubmoduleChanged_UT)
{
    m_filter->cbx_submodule->addItem("All");
    m_filter->cbx_submodule->addItem("mod1");
    m_filter->cbx_submodule->setCurrentIndex(1);
    m_filter->slot_cbxSubmoduleChanged(1);
    SUCCEED();
}

TEST_F(FilterContent_UT, slot_buttonClicked_All_UT)
{
    m_filter->slot_buttonClicked(BUTTONID::ALL);
    EXPECT_EQ(m_filter->m_curBtnId, BUTTONID::ALL);
    m_filter->slot_buttonClicked(BUTTONID::ONE_DAY);
    EXPECT_EQ(m_filter->m_curBtnId, BUTTONID::ONE_DAY);
    m_filter->slot_buttonClicked(BUTTONID::THREE_DAYS);
    EXPECT_EQ(m_filter->m_curBtnId, BUTTONID::THREE_DAYS);
    m_filter->slot_buttonClicked(BUTTONID::ONE_WEEK);
    EXPECT_EQ(m_filter->m_curBtnId, BUTTONID::ONE_WEEK);
    m_filter->slot_buttonClicked(BUTTONID::ONE_MONTH);
    EXPECT_EQ(m_filter->m_curBtnId, BUTTONID::ONE_MONTH);
    m_filter->slot_buttonClicked(BUTTONID::THREE_MONTHS);
    EXPECT_EQ(m_filter->m_curBtnId, BUTTONID::THREE_MONTHS);
}

TEST_F(FilterContent_UT, shortCutExport_Disabled_UT)
{
    m_filter->setExportButtonEnable(false);
    m_filter->shortCutExport();
    EXPECT_FALSE(m_filter->exportBtn->isEnabled());
    m_filter->setExportButtonEnable(true);
}

TEST_F(FilterContent_UT, slot_logCatelogueRefresh_Boot_UT)
{
    QStandardItemModel m_model;
    QStandardItem item;
    m_model.appendRow(&item);
    m_model.setData(m_model.index(0, 0), QString(BOOT_TREE_DATA), Qt::UserRole + 66);
    m_filter->slot_logCatelogueRefresh(m_model.index(0, 0));
    SUCCEED();
}

TEST_F(FilterContent_UT, eventFilter_Tab_UT)
{
    QKeyEvent tabEvent(QEvent::KeyPress, Qt::Key_Tab, Qt::NoModifier);
    EXPECT_FALSE(m_filter->eventFilter(m_filter->m_allBtn, &tabEvent));
    EXPECT_FALSE(m_filter->eventFilter(m_filter->m_todayBtn, &tabEvent));
    EXPECT_FALSE(m_filter->eventFilter(m_filter->m_threeDayBtn, &tabEvent));
    EXPECT_FALSE(m_filter->eventFilter(m_filter->m_lastWeekBtn, &tabEvent));
    EXPECT_FALSE(m_filter->eventFilter(m_filter->m_lastMonthBtn, &tabEvent));
    EXPECT_FALSE(m_filter->eventFilter(m_filter->m_threeMonthBtn, &tabEvent));

    QKeyEvent backtabEvent(QEvent::KeyPress, Qt::Key_Backtab, Qt::NoModifier);
    EXPECT_FALSE(m_filter->eventFilter(m_filter->m_allBtn, &backtabEvent));
}

TEST_F(FilterContent_UT, eventFilter_NonKeyPress_UT)
{
    QEvent nonKey(QEvent::HoverEnter);
    EXPECT_FALSE(m_filter->eventFilter(m_filter->m_allBtn, &nonKey));
}

TEST_F(FilterContent_UT, updateDataState_UT)
{
    m_filter->updateDataState();
    SUCCEED();
}

TEST_F(FilterContent_UT, exportBtnClicked_Signal_UT)
{
    // Trigger the clicked signal to exercise the QtPrivate FunctorCall lambda
    m_filter->exportBtn->click();
    SUCCEED();
}
