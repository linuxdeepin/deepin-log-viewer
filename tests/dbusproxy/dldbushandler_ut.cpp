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

#include "dldbushandler.h"
#include "logfileparser.h"

#include <stub.h>

#include <QDebug>

#include <gtest/gtest.h>

QDBusPendingCall stub_asyncCallWithArgumentList(const QString &method,
                                                const QList<QVariant> &args)
{
    Q_UNUSED(method);
    Q_UNUSED(args);
    //    message.createReply(QStringList()<<"test");
    QDBusPendingReply<QStringList> reply;
    return reply;
}

class DlDbushandler_UT : public testing::Test
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
        m_handler = DLDBusHandler::instance(&m_fileparser);
        qDebug() << "SetUp" << endl;
    }
    void TearDown() //TEST跑完之后会执行TearDown
    {
        delete m_handler;
    }
    DLDBusHandler *m_handler;
    LogFileParser m_fileparser;
};

TEST_F(DlDbushandler_UT, Compare_UT)
{
    Stub stub;
    stub.set(ADDR(QDBusAbstractInterface, asyncCallWithArgumentList), stub_asyncCallWithArgumentList);
    m_handler->quit();
    m_handler->readLog("Test");
}
