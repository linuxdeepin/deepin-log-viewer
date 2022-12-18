// SPDX-FileCopyrightText: 2019 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "ut_stuballthread.h"
#include "../liblogviewerplugin/src/private/utils.h"
#include "../liblogviewerplugin/src/plugins/logviewerplugin.h"
#include "../liblogviewerplugin/src/plugininterfaces/logviewerplugininterface.h"


#include <gtest/gtest.h>
#include <stub.h>

#include <DApplication>

#include <QDebug>
#include <QFileDialog>
#include <QPaintEvent>
#include <QDBusAbstractInterfaceBase>
#include <QStandardPaths>
#include <QMessageBox>

class LogViewerPlugin_UT : public testing::Test
{
public:
    void SetUp() //TEST跑之前会执行SetUp
    {
        m_instance = new LogViewerPlugin;
        qDebug() << "SetUp" << endl;
    }
    void TearDown() //TEST跑完之后会执行TearDown
    {
        delete m_instance;
    }
    LogViewerPlugin *m_instance;
};

class LogViewerPlugin_generateAppFile_UT_Param
{
public:
    explicit LogViewerPlugin_generateAppFile_UT_Param(int id)
    {
        time = id;
    }
    int time;
};

class LogViewerPlugin_generateAppFile_UT : public ::testing::TestWithParam<LogViewerPlugin_generateAppFile_UT_Param>
{
    virtual void nothing();//消除编译警告
};

void LogViewerPlugin_generateAppFile_UT::nothing(){}

INSTANTIATE_TEST_CASE_P(LogViewerPlugin, LogViewerPlugin_generateAppFile_UT, ::testing::Values(LogViewerPlugin_generateAppFile_UT_Param(0), LogViewerPlugin_generateAppFile_UT_Param(1), LogViewerPlugin_generateAppFile_UT_Param(2), LogViewerPlugin_generateAppFile_UT_Param(3), LogViewerPlugin_generateAppFile_UT_Param(4), LogViewerPlugin_generateAppFile_UT_Param(5)));

TEST_P(LogViewerPlugin_generateAppFile_UT, LogViewerPlugin_generateAppFile_UT_001)
{
//    Stub stub;
//    stub.set(ADDR(JournalBootWork, getReplaceColorStr), stub_getReplaceColorStr_bootwork);
//    stub.set(ADDR(journalWork, getReplaceColorStr), stub_getReplaceColorStr_bootwork);
//    stub.set(ADDR(LogFileParser, parseByApp), LogFileParser_parseByApp);
    LogViewerPlugin_generateAppFile_UT_Param param = GetParam();
    LogViewerPlugin *p = new LogViewerPlugin();
    EXPECT_NE(p, nullptr);
    Utils::sleep(200);
    p->generateAppFile("", param.time, 7, "");
    p->deleteLater();

//    LogViewerPlugin_generateAppFile_UT_Param param = GetParam();
//    QPluginLoader loader("ProcessMultimeterD.dll");
//    if(loader.load())
//    {
//        if(QObject * plugin = loader.instance())
//        {
//            LogViewerPluginInterface *p = dynamic_cast<LogViewerPluginInterface *>(plugin);
//            qDebug()<<p;
//            p->generateAppFile("", param.time, 7, "");
//        }
//    }
//    else
//    {
//        qDebug()<<loader.errorString();
//    }
}

