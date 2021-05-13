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
#include "logcombox.h"
#include "filtercontent.h"
#include "logperiodbutton.h"
#include "stuballthread.h"

#include <gtest/gtest.h>
#include <stub.h>
#include <QDebug>
#include <QKeyEvent>
#include <QThread>
#include <QThreadPool>

TEST(FilterContent_Constructor_UT, FilterContent_Constructor_UT)
{
    FilterContent *p = new FilterContent(nullptr);
    EXPECT_NE(p, nullptr);
    delete p;
}

TEST(FilterContent_initUI_UT, FilterContent_initUI_UT)
{
    FilterContent *p = new FilterContent(nullptr);
    EXPECT_NE(p, nullptr);
    p->initUI();
    delete p;
}

TEST(FilterContent_initUinitConnections_UT, FilterContent_initUinitConnections_UT)
{
    FilterContent *p = new FilterContent(nullptr);
    EXPECT_NE(p, nullptr);
    p->initConnections();
    delete p;
}

class FilterContent_shortCutExport_UT_Param
{
public:
    explicit FilterContent_shortCutExport_UT_Param(bool iIsExportBtn)
    {
        m_isExportBtn = iIsExportBtn;
    }
    bool m_isExportBtn;
};

class FilterContent_shortCutExport_UT : public ::testing::TestWithParam<FilterContent_shortCutExport_UT_Param>
{
};

INSTANTIATE_TEST_CASE_P(FilterContent, FilterContent_shortCutExport_UT, ::testing::Values(FilterContent_shortCutExport_UT_Param(true), FilterContent_shortCutExport_UT_Param(false)));

TEST_P(FilterContent_shortCutExport_UT, FilterContent_shortCutExport_UT)
{
    FilterContent *p = new FilterContent(nullptr);
    EXPECT_NE(p, nullptr);
    FilterContent_shortCutExport_UT_Param param = GetParam();
    if (param.m_isExportBtn) {
        p->exportBtn;
        p->exportBtn = nullptr;
    }
    p->shortCutExport();
    delete p;
}

TEST(FilterContent_setAppComboBoxItemUT, FilterContent_setAppComboBoxItem_UT)
{
    FilterContent *p = new FilterContent(nullptr);
    EXPECT_NE(p, nullptr);
    p->setAppComboBoxItem();
    delete p;
}

class FilterContent_setSelectorVisible_UT_Param
{
public:
    FilterContent_setSelectorVisible_UT_Param(bool ilvCbx, bool iappListCbx, bool istatusCbx, bool iperiod, bool ineedMove, bool itypecbx)
    {
        lvCbx = ilvCbx;
        appListCbx = iappListCbx;
        statusCbx = istatusCbx;
        period = iperiod;
        needMove = ineedMove;
        typecbx = itypecbx;
    }
    bool lvCbx;
    bool appListCbx;
    bool statusCbx;
    bool period;
    bool needMove;
    bool typecbx;
};

class FilterContent_setSelectorVisible_UT : public ::testing::TestWithParam<FilterContent_setSelectorVisible_UT_Param>
{
};

INSTANTIATE_TEST_CASE_P(FilterContent, FilterContent_setSelectorVisible_UT, ::testing::Values(FilterContent_setSelectorVisible_UT_Param(true, true, true, true, true, true), FilterContent_setSelectorVisible_UT_Param(false, false, false, false, false, false)));

TEST_P(FilterContent_setSelectorVisible_UT, FilterContent_setSelectorVisible_UT)
{
    FilterContent *p = new FilterContent(nullptr);
    EXPECT_NE(p, nullptr);
    FilterContent_setSelectorVisible_UT_Param param = GetParam();
    p->setSelectorVisible(param.lvCbx, param.appListCbx, param.statusCbx, param.period, param.needMove, param.typecbx);
    p->deleteLater();
}

class FilterContent_setSelection_UT_Param
{
public:
    FilterContent_setSelection_UT_Param(bool ilvCbx, bool iappListCbx, bool istatusCbx, bool itypecbx, bool ipath)
    {
        lvCbxVisible = ilvCbx;
        appListCbxVisible = iappListCbx;
        statusCbxVisible = istatusCbx;
        typecbxVisible = itypecbx;
        pathEmpty = ipath;
    }
    bool lvCbxVisible;
    bool appListCbxVisible;
    bool statusCbxVisible;
    bool typecbxVisible;
    bool pathEmpty;
};

class FilterContent_setSelection_UT : public ::testing::TestWithParam<FilterContent_setSelection_UT_Param>
{
};

INSTANTIATE_TEST_CASE_P(FilterContent, FilterContent_setSelection_UT, ::testing::Values(FilterContent_setSelection_UT_Param(true, true, true, true, true), FilterContent_setSelection_UT_Param(false, false, false, false, false), FilterContent_setSelection_UT_Param(true, true, true, true, false)));

TEST_P(FilterContent_setSelection_UT, FilterContent_setSelection_UT)
{
    FilterContent *p = new FilterContent(nullptr);
    EXPECT_NE(p, nullptr);
    FilterContent_setSelection_UT_Param param = GetParam();
    p->setSelectorVisible(param.lvCbxVisible, param.appListCbxVisible, param.statusCbxVisible, true, true, param.typecbxVisible);
    FILTER_CONFIG config;
    config.appListCbx = param.pathEmpty ? "" : "app";
    p->setSelection(config);
    p->deleteLater();
}

TEST(FilterContent_setUeButtonSytle_UT, FilterContent_setUeButtonSytle_UT)
{
    FilterContent *p = new FilterContent(nullptr);
    EXPECT_NE(p, nullptr);
    p->setUeButtonSytle();
    EXPECT_EQ(p->m_allBtn->isChecked(), true);
    delete p;
}

class FilterContent_eventFilter_UT_Param
{
public:
    FilterContent_eventFilter_UT_Param(bool iIsKeyPressed, bool iIsKeyRight, int iOriginWidgetName, int iResultWidgetName)
    {
        isKeyPressed = iIsKeyPressed;
        isKeyRight = iIsKeyRight;
        originWidgetName = iOriginWidgetName;
        resultWidgetName = iResultWidgetName;
    }
    bool isKeyPressed;
    bool isKeyRight;
    int originWidgetName;
    int resultWidgetName;
};

class FilterContent_eventFilter_UT : public ::testing::TestWithParam<FilterContent_eventFilter_UT_Param>
{
};

INSTANTIATE_TEST_CASE_P(FilterContent, FilterContent_eventFilter_UT, ::testing::Values(FilterContent_eventFilter_UT_Param(false, false, INVALID, INVALID), FilterContent_eventFilter_UT_Param(true, true, ALL, ONE_DAY), FilterContent_eventFilter_UT_Param(true, false, ALL, THREE_MONTHS), FilterContent_eventFilter_UT_Param(true, true, ONE_DAY, THREE_DAYS), FilterContent_eventFilter_UT_Param(true, false, ONE_DAY, ALL), FilterContent_eventFilter_UT_Param(true, true, THREE_DAYS, ONE_WEEK), FilterContent_eventFilter_UT_Param(true, false, THREE_DAYS, ONE_DAY), FilterContent_eventFilter_UT_Param(true, true, ONE_WEEK, ONE_MONTH), FilterContent_eventFilter_UT_Param(true, false, ONE_WEEK, THREE_DAYS), FilterContent_eventFilter_UT_Param(true, true, ONE_MONTH, THREE_MONTHS), FilterContent_eventFilter_UT_Param(true, false, ONE_MONTH, ONE_WEEK), FilterContent_eventFilter_UT_Param(true, true, THREE_MONTHS, ALL), FilterContent_eventFilter_UT_Param(true, false, THREE_MONTHS, ONE_MONTH)));
LogPeriodButton *getLogPeriodButtonByName(int iName, FilterContent *p)
{
    LogPeriodButton *w = nullptr;
    switch (iName) {
    case ALL:
        w = p->m_allBtn;
        break;
    case ONE_DAY:
        w = p->m_todayBtn;
        break;
    case THREE_DAYS:
        w = p->m_threeDayBtn;
        break;
    case ONE_WEEK:
        w = p->m_lastWeekBtn;
        break;
    case ONE_MONTH:
        w = p->m_lastMonthBtn;
        break;
    case THREE_MONTHS:
        w = p->m_threeMonthBtn;
        break;
    default:
        break;
    }
    return w;
}
TEST_P(FilterContent_eventFilter_UT, FilterContent_eventFilter_UT)
{
    FilterContent *p = new FilterContent(nullptr);
    EXPECT_NE(p, nullptr);
    FilterContent_eventFilter_UT_Param param = GetParam();
    QEvent *event = nullptr;
    if (param.isKeyPressed) {
        int key = param.isKeyRight ? Qt::Key_Right : Qt::Key_Left;
        QKeyEvent *keyEvent = new QKeyEvent(QEvent::KeyPress, key, Qt::NoModifier);
        event = keyEvent;
        LogPeriodButton *w = nullptr;
        w = getLogPeriodButtonByName(param.originWidgetName, p);
        p->eventFilter(w, event);
        LogPeriodButton *wResult = nullptr;
        wResult = getLogPeriodButtonByName(param.resultWidgetName, p);
        EXPECT_NE(wResult, nullptr);
        bool checkCorrect = false;
        if (wResult && wResult->isChecked()) {
            checkCorrect = true;
        }
        EXPECT_EQ(checkCorrect, true);
        delete keyEvent;
        wResult->deleteLater();
        w->deleteLater();
    } else {
        event = new QEvent(QEvent::None);
        p->eventFilter(nullptr, event);
        delete event;
    }
    p->deleteLater();
}

TEST(FilterContent_resizeWidth_UT, FilterContent_resizeWidth_UT)
{
    FilterContent *p = new FilterContent(nullptr);
    EXPECT_NE(p, nullptr);
    p->resizeWidth();
    p->deleteLater();
}

TEST(FilterContent_updateWordWrap_UT, FilterContent_updateWordWrap_UT)
{
    FilterContent *p = new FilterContent(nullptr);
    EXPECT_NE(p, nullptr);
    p->updateWordWrap();
    p->deleteLater();
}

class FilterContent_updateDataState_UT_Param
{
public:
    explicit FilterContent_updateDataState_UT_Param(bool iNew)
    {
        isNew = iNew;
    }
    bool isNew;
};

class FilterContent_updateDataState_UT : public ::testing::TestWithParam<FilterContent_updateDataState_UT_Param>
{
};

INSTANTIATE_TEST_CASE_P(FilterContent, FilterContent_updateDataState_UT, ::testing::Values(FilterContent_updateDataState_UT_Param(true), FilterContent_updateDataState_UT_Param(false)));

TEST_P(FilterContent_updateDataState_UT, FilterContent_updateDataState_UT)
{
    FilterContent *p = new FilterContent(nullptr);
    EXPECT_NE(p, nullptr);
    FilterContent_updateDataState_UT_Param param = GetParam();
    if (param.isNew) {
        p->m_config.clear();
    }
    p->updateDataState();
    p->deleteLater();
}

TEST(FilterContent_setCurrentConfig_UT, FilterContent_setCurrentConfig_UT)
{
    FilterContent *p = new FilterContent(nullptr);
    EXPECT_NE(p, nullptr);
    FILTER_CONFIG config;
    p->setCurrentConfig(config);
    EXPECT_EQ(config.levelCbx, p->m_config.value(p->m_currentType).levelCbx);
    EXPECT_EQ(config.appListCbx, p->m_config.value(p->m_currentType).appListCbx);
    EXPECT_EQ(config.statusCbx, p->m_config.value(p->m_currentType).statusCbx);
    EXPECT_EQ(config.dateBtn, p->m_config.value(p->m_currentType).dateBtn);
    EXPECT_EQ(config.typeCbx, p->m_config.value(p->m_currentType).typeCbx);
    p->deleteLater();
}

class FilterContent_slot_logCatelogueClicked_UT_Param
{
public:
    explicit FilterContent_slot_logCatelogueClicked_UT_Param(bool iIndex)
    {
        index = iIndex;
    }
    int index;
};

class FilterContent_slot_logCatelogueClicked_UT : public ::testing::TestWithParam<FilterContent_slot_logCatelogueClicked_UT_Param>
{
};

static QString FilterContent_slot_logCatelogueClicked_ModelIndex_data = "";
INSTANTIATE_TEST_CASE_P(FilterContent, FilterContent_slot_logCatelogueClicked_UT, ::testing::Values(FilterContent_slot_logCatelogueClicked_UT_Param(0), FilterContent_slot_logCatelogueClicked_UT_Param(1), FilterContent_slot_logCatelogueClicked_UT_Param(2), FilterContent_slot_logCatelogueClicked_UT_Param(3), FilterContent_slot_logCatelogueClicked_UT_Param(4), FilterContent_slot_logCatelogueClicked_UT_Param(5), FilterContent_slot_logCatelogueClicked_UT_Param(6), FilterContent_slot_logCatelogueClicked_UT_Param(7), FilterContent_slot_logCatelogueClicked_UT_Param(8), FilterContent_slot_logCatelogueClicked_UT_Param(9)));
QVariant FilterContent_slot_logCatelogueClicked_ModelIndex_data_Func(void *obj, int arole)
{
    return QString(FilterContent_slot_logCatelogueClicked_ModelIndex_data);
}
TEST_P(FilterContent_slot_logCatelogueClicked_UT, FilterContent_slot_logCatelogueClicked_UT)
{
    Stub stub;

    FilterContent *p = new FilterContent(nullptr);
    EXPECT_NE(p, nullptr);
    FilterContent_slot_logCatelogueClicked_UT_Param param = GetParam();

    switch (param.index) {
    case 0:
        FilterContent_slot_logCatelogueClicked_ModelIndex_data = JOUR_TREE_DATA;
        break;
    case 1:
        FilterContent_slot_logCatelogueClicked_ModelIndex_data = DPKG_TREE_DATA;
        break;
    case 2:
        FilterContent_slot_logCatelogueClicked_ModelIndex_data = XORG_TREE_DATA;
        break;
    case 3:
        FilterContent_slot_logCatelogueClicked_ModelIndex_data = BOOT_TREE_DATA;
        break;
    case 4:
        FilterContent_slot_logCatelogueClicked_ModelIndex_data = KERN_TREE_DATA;
        break;
    case 5:
        FilterContent_slot_logCatelogueClicked_ModelIndex_data = APP_TREE_DATA;
        break;
    case 6:
        FilterContent_slot_logCatelogueClicked_ModelIndex_data = LAST_TREE_DATA;
        break;
    case 7:
        FilterContent_slot_logCatelogueClicked_ModelIndex_data = KWIN_TREE_DATA;
        break;
    case 8:
        FilterContent_slot_logCatelogueClicked_ModelIndex_data = BOOT_KLU_TREE_DATA;
        break;
    case 9:
        FilterContent_slot_logCatelogueClicked_ModelIndex_data = "";
        break;
    default:
        break;
    }

    stub.set(ADDR(QModelIndex, data), FilterContent_slot_logCatelogueClicked_ModelIndex_data_Func);
    p->slot_logCatelogueClicked(QModelIndex());
    p->deleteLater();
}

class FilterContent_slot_logCatelogueRefresh_UT_Param
{
public:
    explicit FilterContent_slot_logCatelogueRefresh_UT_Param(bool iIndex)
    {
        index = iIndex;
    }
    int index;
};

class FilterContent_slot_logCatelogueRefresh_UT : public ::testing::TestWithParam<FilterContent_slot_logCatelogueRefresh_UT_Param>
{
};

static QString FilterContent_slot_logCatelogueRefresh_ModelIndex_data = "";
INSTANTIATE_TEST_CASE_P(FilterContent, FilterContent_slot_logCatelogueRefresh_UT, ::testing::Values(FilterContent_slot_logCatelogueRefresh_UT_Param(0), FilterContent_slot_logCatelogueRefresh_UT_Param(1), FilterContent_slot_logCatelogueRefresh_UT_Param(2), FilterContent_slot_logCatelogueRefresh_UT_Param(3), FilterContent_slot_logCatelogueRefresh_UT_Param(4), FilterContent_slot_logCatelogueRefresh_UT_Param(5), FilterContent_slot_logCatelogueRefresh_UT_Param(6), FilterContent_slot_logCatelogueRefresh_UT_Param(7), FilterContent_slot_logCatelogueRefresh_UT_Param(8), FilterContent_slot_logCatelogueRefresh_UT_Param(9)));
QVariant FilterContent_slot_logCatelogueRefresh_ModelIndex_data_Func(void *obj, int arole)
{
    return QString(FilterContent_slot_logCatelogueRefresh_ModelIndex_data);
}
TEST_P(FilterContent_slot_logCatelogueRefresh_UT, FilterContent_slot_logCatelogueRefresh_UT)
{
    FilterContent *p = new FilterContent(nullptr);
    EXPECT_NE(p, nullptr);
    FilterContent_slot_logCatelogueRefresh_UT_Param param = GetParam();
    Stub stub;
    switch (param.index) {
    case 0:
        FilterContent_slot_logCatelogueRefresh_ModelIndex_data = JOUR_TREE_DATA;
        break;
    case 1:
        FilterContent_slot_logCatelogueRefresh_ModelIndex_data = DPKG_TREE_DATA;
        break;
    case 2:
        FilterContent_slot_logCatelogueRefresh_ModelIndex_data = XORG_TREE_DATA;
        break;
    case 3:
        FilterContent_slot_logCatelogueRefresh_ModelIndex_data = BOOT_TREE_DATA;
        break;
    case 4:
        FilterContent_slot_logCatelogueRefresh_ModelIndex_data = KERN_TREE_DATA;
        break;
    case 5:
        FilterContent_slot_logCatelogueRefresh_ModelIndex_data = APP_TREE_DATA;
        break;
    case 6:
        FilterContent_slot_logCatelogueRefresh_ModelIndex_data = LAST_TREE_DATA;
        break;
    case 7:
        FilterContent_slot_logCatelogueRefresh_ModelIndex_data = KWIN_TREE_DATA;
        break;
    case 8:
        FilterContent_slot_logCatelogueRefresh_ModelIndex_data = BOOT_KLU_TREE_DATA;
        break;
    case 9:
        FilterContent_slot_logCatelogueRefresh_ModelIndex_data = "";
        break;
    default:
        break;
    }

    stub.set(ADDR(QModelIndex, data), FilterContent_slot_logCatelogueRefresh_ModelIndex_data_Func);
    p->slot_logCatelogueRefresh(QModelIndex());
    p->deleteLater();
}

class FilterContent_slot_buttonClicked_UT : public ::testing::TestWithParam<int>
{
};

INSTANTIATE_TEST_CASE_P(FilterContent, FilterContent_slot_buttonClicked_UT, ::testing::Values(ALL, ONE_DAY, THREE_DAYS, ONE_WEEK, ONE_MONTH, THREE_MONTHS, RESET));

TEST_P(FilterContent_slot_buttonClicked_UT, FilterContent_slot_buttonClicked_UT)
{
    FilterContent *p = new FilterContent(nullptr);
    EXPECT_NE(p, nullptr);
    int param = GetParam();
    p->slot_buttonClicked(param);
    p->deleteLater();
}

TEST(FilterContent_slot_exportButtonClicked_UT, FilterContent_slot_exportButtonClicked_UT)
{
    FilterContent *p = new FilterContent(nullptr);
    EXPECT_NE(p, nullptr);
    p->slot_exportButtonClicked();
    p->deleteLater();
}

TEST(FilterContent_slot_cbxLvIdxChanged_UT, FilterContent_slot_cbxLvIdxChanged_UT)
{
    //    Stub *stub = new Stub;
    FilterContent *p = new FilterContent(nullptr);
    EXPECT_NE(p, nullptr);
    p->slot_cbxLvIdxChanged(1);
    p->deleteLater();
}

TEST(FilterContent_slot_cbxAppIdxChanged_UT, FilterContent_slot_cbxAppIdxChanged_UT)
{
    FilterContent *p = new FilterContent(nullptr);
    EXPECT_NE(p, nullptr);
    p->slot_cbxAppIdxChanged(1);
    p->deleteLater();
}

TEST(FilterContent_slot_cbxStatusChanged_UT, FilterContent_slot_cbxStatusChanged_UT)
{
    FilterContent *p = new FilterContent(nullptr);
    EXPECT_NE(p, nullptr);
    p->slot_cbxStatusChanged(1);
    p->deleteLater();
}

TEST(FilterContent_slot_cbxLogTypeChanged_UT, FilterContent_slot_cbxLogTypeChanged_UT)
{
    FilterContent *p = new FilterContent(nullptr);
    EXPECT_NE(p, nullptr);
    p->slot_cbxLogTypeChanged(1);
    p->deleteLater();
}

TEST(FilterContent_setExportButtonEnable_UT, FilterContent_setExportButtonEnable_UT)
{
    FilterContent *p = new FilterContent(nullptr);
    EXPECT_NE(p, nullptr);
    p->setExportButtonEnable(true);
    p->deleteLater();
}

TEST(FilterContent_setExportButtonEnable_UT, FilterContent_paintEvent_UT)
{
    FilterContent *p = new FilterContent(nullptr);
    EXPECT_NE(p, nullptr);
    QPaintEvent repaint(p->rect());
    p->paintEvent(&repaint);
    p->deleteLater();
}
