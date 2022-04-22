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

#include "dldbusinterface.h"

#include <stub.h>

#include <QDebug>

#include <gtest/gtest.h>

class DlDbusinterface_UT : public testing::Test
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
        m_interface = new DeepinLogviewerInterface("test", "test", QDBusConnection::systemBus());
        qDebug() << "SetUp" << endl;
    }
    void TearDown() //TEST跑完之后会执行TearDown
    {
        delete m_interface;
    }
    DeepinLogviewerInterface *m_interface;
};

TEST_F(DlDbusinterface_UT, Compare_UT)
{
    m_interface->exitCode();
    m_interface->exportLog("path", "path1", false);
    m_interface->getFileInfo("path", false);
    m_interface->quit();
    m_interface->readLog("Test");
}
