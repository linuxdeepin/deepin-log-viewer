/*
* Copyright (C) 2019 ~ 2020 UnionTech Software Technology Co.,Ltd
*
* Author:     linxun <linxun@uniontech.com>
* Maintainer:  linxun <linxun@uniontech.com>
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

#include "filtercontent.h"
#include "logperiodbutton.h"
#include "structdef.h"

#include <QPaintEvent>
#include <QEvent>
#include <QStandardItemModel>
#include <QStandardItem>

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
        qDebug() << "SetUpTestCase" << endl;
    }
    static void TearDownTestCase()
    {
        qDebug() << "TearDownTestCase" << endl;
    }
    void SetUp() //TEST跑之前会执行SetUp
    {
        m_filter = new FilterContent;
        qDebug() << "SetUp" << endl;
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
}

TEST_F(FilterContent_UT, setAppComboBoxItem_UT)
{
    m_filter->setAppComboBoxItem();
}

TEST_F(FilterContent_UT, setSelection_UT)
{
    Stub stub;
    stub.set(ADDR(QWidget, isVisible), setSelection_IsVisible);
    FILTER_CONFIG fitler;
    m_filter->setSelection(fitler);
}

TEST_F(FilterContent_UT, paintEvent_UT)
{
    QPaintEvent repaint(m_filter->rect());
    m_filter->paintEvent(&repaint);
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
}

TEST_F(FilterContent_UT, updateWordWrap_UT)
{
    m_filter->updateWordWrap();
    m_filter->rect().setWidth(500);
    LogPeriodButton btn("test", m_filter);
    m_filter->hLayout_period->addWidget(&btn);
    m_filter->updateWordWrap();
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
    m_model.setData(m_model.index(10, 0), QString(QDir::homePath() + "/.kwin.log"), Qt::UserRole + 66);
    for (int i = 0; i < m_model.rowCount(); i++) {
        QModelIndex modelindex = m_model.index(i, 0, QModelIndex());
        m_filter->slot_logCatelogueClicked(modelindex);
    }
    delete item;
}

TEST_F(FilterContent_UT, slot_exportButtonClicked_UT)
{
    m_filter->slot_exportButtonClicked();
}

TEST_F(FilterContent_UT, slot_cbxLvIdxChanged_UT)
{
    m_filter->slot_cbxLvIdxChanged(2);
}

TEST_F(FilterContent_UT, slot_cbxLogTypeChanged_UT)
{
    m_filter->slot_cbxLogTypeChanged(2);
}

TEST_F(FilterContent_UT, slot_logCatelogueRefresh_UT)
{
    QStandardItemModel m_model;
    QStandardItem item;
    m_model.appendRow(&item);
    m_model.setData(m_model.index(0, 0), QString("application"), Qt::UserRole + 66);
    m_filter->slot_logCatelogueRefresh(m_model.index(0, 0));
}

TEST_F(FilterContent_UT, slot_buttonClicked_UT)
{
    m_filter->slot_buttonClicked(BUTTONID::RESET);
}
