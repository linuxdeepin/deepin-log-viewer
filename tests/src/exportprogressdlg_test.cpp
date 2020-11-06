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


#include <gtest/gtest.h>
#include <gtest/src/stub.h>
#include "exportprogressdlg.h"

#include <QDebug>
TEST(ExportProgressDlg_Constructor_UT, ExportProgressDlg_Constructor_UT)
{
    ExportProgressDlg *p = new ExportProgressDlg(nullptr);
    EXPECT_NE(p, nullptr);
    delete  p;
}


class ExportProgressDlg_setProgressBarRange_UT_Param
{
public:
    ExportProgressDlg_setProgressBarRange_UT_Param(bool iIsProgressBarExists, int iMinValue, int iMaxValue)
    {
        m_isProgressBarExists = iIsProgressBarExists;
        m_minValue = iMinValue;
        m_maxValue = iMaxValue;

    }
    bool m_isProgressBarExists;
    int m_minValue;
    int m_maxValue;
};

class ExportProgressDlg_setProgressBarRange_UT : public ::testing::TestWithParam<ExportProgressDlg_setProgressBarRange_UT_Param>
{
};


INSTANTIATE_TEST_SUITE_P(DisplayContent, ExportProgressDlg_setProgressBarRange_UT, ::testing::Values(ExportProgressDlg_setProgressBarRange_UT_Param(true, 0, 100),
                                                                                                     ExportProgressDlg_setProgressBarRange_UT_Param(false, 0, 100),
                                                                                                     ExportProgressDlg_setProgressBarRange_UT_Param(true, 100, 0)
                                                                                                    ));

TEST_P(ExportProgressDlg_setProgressBarRange_UT, ExportProgressDlg_setProgressBarRange_UT)
{
    ExportProgressDlg *p = new ExportProgressDlg(nullptr);
    EXPECT_NE(p, nullptr);
    ExportProgressDlg_setProgressBarRange_UT_Param param = GetParam();
    qDebug() << "ExportProgressDlg_setProgressBarRange_UT" << param.m_isProgressBarExists << param.m_minValue << param.m_maxValue;
    if (!param.m_isProgressBarExists) {
        delete p->m_pExportProgressBar;
        p->m_pExportProgressBar = nullptr;
        p->setProgressBarRange(param.m_minValue, param.m_maxValue);
    } else {
        p->setProgressBarRange(param.m_minValue, param.m_maxValue);
    }

    p->deleteLater();
}


class ExportProgressDlg_updateProgressBarValue_UT_Param
{
public:
    ExportProgressDlg_updateProgressBarValue_UT_Param(bool iIsProgressBarExists, int iValue)
    {
        m_isProgressBarExists = iIsProgressBarExists;
        m_Value = iValue;
    }
    bool m_isProgressBarExists;
    int m_Value;
};

class ExportProgressDlg_updateProgressBarValue_UT : public ::testing::TestWithParam<ExportProgressDlg_updateProgressBarValue_UT_Param>
{
};


INSTANTIATE_TEST_SUITE_P(ExportProgressDlg, ExportProgressDlg_updateProgressBarValue_UT, ::testing::Values(ExportProgressDlg_updateProgressBarValue_UT_Param(true, 50),
                                                                                                           ExportProgressDlg_updateProgressBarValue_UT_Param(false, 50)
                                                                                                          ));

TEST_P(ExportProgressDlg_updateProgressBarValue_UT, ExportProgressDlg_updateProgressBarValue_UT)
{
    ExportProgressDlg *p = new ExportProgressDlg(nullptr);
    EXPECT_NE(p, nullptr);
    ExportProgressDlg_updateProgressBarValue_UT_Param param = GetParam();
    if (!param.m_isProgressBarExists) {
        delete p->m_pExportProgressBar;
        p->m_pExportProgressBar = nullptr;
        p->updateProgressBarValue(param.m_Value);
    } else {
        p->updateProgressBarValue(param.m_Value);
        EXPECT_EQ(p->m_pExportProgressBar->value(), param.m_Value);
    }

    p->deleteLater();
}




TEST(ExportProgressDlg_hideEvent_UT, ExportProgressDlg_hideEvent_UT)
{
    ExportProgressDlg *p = new ExportProgressDlg(nullptr);
    EXPECT_NE(p, nullptr);

    p->hideEvent(nullptr);
    EXPECT_EQ(p->m_pExportProgressBar->value(), p->m_pExportProgressBar->minimum());
    p->deleteLater();
}









