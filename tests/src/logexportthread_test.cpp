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
#include "logexportthread.h"
#include "structdef.h"
#include "stuballthread.h"
#include "../../3rdparty/DocxFactory/include/DocxFactory/WordProcessingMerger/WordProcessingMerger.h"
#include <stub.h>

#include <DApplication>
#include <DStandardItem>

#include <QDebug>
#include <QIcon>

#include <iostream>
#include <gtest/gtest.h>
void stub_load(const std::string &p_fileName)
{
    Q_UNUSED(p_fileName);
}

void stub_setClipboardValue(const std::string &p_itemName, const std::string &p_fieldName, const std::string &p_value)
{
    Q_UNUSED(p_itemName);
    Q_UNUSED(p_fieldName);
    Q_UNUSED(p_value);
}

void stub_paste(const std::string &p_itemName)
{
    Q_UNUSED(p_itemName);
}

void stub_save(const std::string &p_fileName)
{
    Q_UNUSED(p_fileName);
}

bool stub_dfwexists()
{
    return true;
}

bool stub_open(QFile::OpenMode flags)
{
    Q_UNUSED(flags);
    return true;
}

QString stub_text()
{
    return "test";
}

class LogExportthread_UT : public testing::Test
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
        bool iscomplete = true;
        exportThread = new LogExportThread(iscomplete);
        qDebug() << "SetUp" << endl;
    }
    void TearDown() //TEST跑完之后会执行TearDown
    {
        delete exportThread;
    }
    LogExportThread *exportThread;
};

TEST_F(LogExportthread_UT, ExportToText_UT)
{
    Stub stub;
    stub.set(ADDR(DocxFactory::WordProcessingMerger, load), stub_load);
    stub.set(ADDR(DocxFactory::WordProcessingMerger, paste), stub_paste);
    stub.set(ADDR(DocxFactory::WordProcessingMerger, save), stub_save);
    stub.set((void (DocxFactory::WordProcessingMerger::*)(const std::string &, const std::string &, const std::string &))ADDR(DocxFactory::WordProcessingMerger, setClipboardValue), stub_setClipboardValue);
    stub.set(ADDR(QStandardItem, text), stub_text);
    QStandardItemModel m_model;
    m_model.appendRow(new QStandardItem());
    LOG_FLAG m_flag = LOG_FLAG::JOURNAL;
    struct LOG_MSG_JOURNAL m_journal = {"20190503", "10:02", "uos", "d", "1", "2"};
    struct LOG_MSG_APPLICATOIN m_app = {"20190503", "10:02", "uos", "d"};
    struct LOG_MSG_DPKG m_dpkg = {"20190503", "10:02", "uos"};
    struct LOG_MSG_BOOT m_boot = {"20190503", "10:02"};
    struct LOG_MSG_XORG m_xorg = {"20190503", "10:02"};
    struct LOG_MSG_NORMAL m_normal = {"20190503", "10:02", "uos", "d"};
    struct LOG_MSG_KWIN m_kwin = {"test"};
    struct LOG_MSG_DNF m_dnf = {"20190503", "waring", "test"};
    struct LOG_MSG_DMESG m_dmesg = {"20190503", "waring", "test"};

    QList<LOG_MSG_JOURNAL> m_journalList {m_journal};
    QList<LOG_MSG_APPLICATOIN> m_appList {m_app};
    QList<LOG_MSG_DPKG> m_dpkgList {m_dpkg};
    QList<LOG_MSG_BOOT> m_bootList {m_boot};
    QList<LOG_MSG_XORG> m_xorgList {m_xorg};
    QList<LOG_MSG_NORMAL> m_normalList {m_normal};
    QList<LOG_MSG_KWIN> m_kwinList {m_kwin};
    QList<LOG_MSG_DNF> m_dnfList {m_dnf};
    QList<LOG_MSG_DMESG> m_dmesgList {m_dmesg};
    QString str("test");
    exportThread->m_canRunning = true;

    exportThread->exportToTxt("test", &m_model, m_flag);
    exportThread->exportToTxt("test", m_journalList, QStringList() << "test", m_flag);
    exportThread->exportToTxt("test", m_appList, QStringList() << "test", str);
    exportThread->exportToTxt("test", m_dpkgList, QStringList() << "test");
    exportThread->exportToTxt("test", m_bootList, QStringList() << "test");
    exportThread->exportToTxt("test", m_xorgList, QStringList() << "test");
    exportThread->exportToTxt("test", m_normalList, QStringList() << "test");
    exportThread->exportToTxt("test", m_kwinList, QStringList() << "test");
    exportThread->exportToTxt("test", m_dmesgList, QStringList() << "test");
    exportThread->exportToTxt("test", m_dnfList, QStringList() << "test");

    exportThread->exportToTxtPublic("test", &m_model, m_flag);
    exportThread->exportToTxtPublic("test", m_journalList, QStringList() << "test", m_flag);
    exportThread->exportToTxtPublic("test", m_appList, QStringList() << "test", str);
    exportThread->exportToTxtPublic("test", m_dpkgList, QStringList() << "test");
    exportThread->exportToTxtPublic("test", m_bootList, QStringList() << "test");
    exportThread->exportToTxtPublic("test", m_xorgList, QStringList() << "test");
    exportThread->exportToTxtPublic("test", m_normalList, QStringList() << "test");
    exportThread->exportToTxtPublic("test", m_kwinList, QStringList() << "test");
    exportThread->exportToTxtPublic("test", m_dmesgList, QStringList() << "test");
    exportThread->exportToTxtPublic("test", m_dnfList, QStringList() << "test");
}

TEST_F(LogExportthread_UT, ExportToDoc_UT)
{
    Stub stub;
    stub.set(ADDR(DocxFactory::WordProcessingMerger, load), stub_load);
    stub.set(ADDR(DocxFactory::WordProcessingMerger, paste), stub_paste);
    stub.set(ADDR(DocxFactory::WordProcessingMerger, save), stub_save);
    stub.set((void (DocxFactory::WordProcessingMerger::*)(const std::string &, const std::string &, const std::string &))ADDR(DocxFactory::WordProcessingMerger, setClipboardValue), stub_setClipboardValue);
    stub.set((bool (QFile::*)() const)ADDR(QFile, exists), stub_dfwexists);
    QStandardItemModel m_model;
    m_model.appendRow(new QStandardItem());
    LOG_FLAG m_flag = LOG_FLAG::JOURNAL;
    struct LOG_MSG_JOURNAL m_journal = {"20190503", "10:02", "uos", "d", "1", "2"};
    struct LOG_MSG_APPLICATOIN m_app = {"20190503", "10:02", "uos", "d"};
    struct LOG_MSG_DPKG m_dpkg = {"20190503", "10:02", "uos"};
    struct LOG_MSG_BOOT m_boot = {"20190503", "10:02"};
    struct LOG_MSG_XORG m_xorg = {"20190503", "10:02"};
    struct LOG_MSG_NORMAL m_normal = {"20190503", "10:02", "uos", "d"};
    struct LOG_MSG_KWIN m_kwin = {"test"};
    struct LOG_MSG_DNF m_dnf = {"20190503", "waring", "test"};
    struct LOG_MSG_DMESG m_dmesg = {"20190503", "waring", "test"};

    QList<LOG_MSG_JOURNAL> m_journalList {m_journal};
    QList<LOG_MSG_APPLICATOIN> m_appList {m_app};
    QList<LOG_MSG_DPKG> m_dpkgList {m_dpkg};
    QList<LOG_MSG_BOOT> m_bootList {m_boot};
    QList<LOG_MSG_XORG> m_xorgList {m_xorg};
    QList<LOG_MSG_NORMAL> m_normalList {m_normal};
    QList<LOG_MSG_KWIN> m_kwinList {m_kwin};
    QList<LOG_MSG_DNF> m_dnfList {m_dnf};
    QList<LOG_MSG_DMESG> m_dmesgList {m_dmesg};
    QString test = "test";
    exportThread->m_canRunning = true;

    exportThread->exportToDocPublic("test", &m_model, m_flag);
    exportThread->exportToDocPublic("test", m_journalList, QStringList() << "test", m_flag);
    exportThread->exportToDocPublic("test", m_appList, QStringList() << "test", test);
    exportThread->exportToDocPublic("test", m_dpkgList, QStringList() << "test");
    exportThread->exportToDocPublic("test", m_bootList, QStringList() << "test");
    exportThread->exportToDocPublic("test", m_xorgList, QStringList() << "test");
    exportThread->exportToDocPublic("test", m_normalList, QStringList() << "test");
    exportThread->exportToDocPublic("test", m_kwinList, QStringList() << "test");
    exportThread->exportToDocPublic("test", m_dnfList, QStringList() << "test");
    exportThread->exportToDocPublic("test", m_dmesgList, QStringList() << "test");

    exportThread->exportToDoc("test", m_journalList, QStringList() << "test", m_flag);
    exportThread->exportToDoc("test", m_appList, QStringList() << "test", test);
    exportThread->exportToDoc("test", m_dpkgList, QStringList() << "test");
    exportThread->exportToDoc("test", m_bootList, QStringList() << "test");
    exportThread->exportToDoc("test", m_xorgList, QStringList() << "test");
    exportThread->exportToDoc("test", m_normalList, QStringList() << "test");
    exportThread->exportToDoc("test", m_kwinList, QStringList() << "test");
    exportThread->exportToDoc("test", m_dmesgList, QStringList() << "test");
    exportThread->exportToDoc("test", m_dnfList, QStringList() << "test");
}

TEST_F(LogExportthread_UT, ExportToHtml_UT)
{
    Stub stub;
    stub.set(ADDR(DocxFactory::WordProcessingMerger, load), stub_load);
    stub.set(ADDR(DocxFactory::WordProcessingMerger, paste), stub_paste);
    stub.set(ADDR(DocxFactory::WordProcessingMerger, save), stub_save);
    stub.set((void (DocxFactory::WordProcessingMerger::*)(const std::string &, const std::string &, const std::string &))ADDR(DocxFactory::WordProcessingMerger, setClipboardValue), stub_setClipboardValue);
    stub.set((bool (QFile::*)() const)ADDR(QFile, exists), stub_dfwexists);

    stub.set((bool (QFile::*)() const)ADDR(QFile, exists), stub_dfwexists);
    QStandardItemModel m_model;
    m_model.appendRow(new QStandardItem());
    LOG_FLAG m_flag = LOG_FLAG::JOURNAL;
    struct LOG_MSG_JOURNAL m_journal = {"20190503", "10:02", "uos", "d", "1", "2"};
    struct LOG_MSG_APPLICATOIN m_app = {"20190503", "10:02", "uos", "d"};
    struct LOG_MSG_DPKG m_dpkg = {"20190503", "10:02", "uos"};
    struct LOG_MSG_BOOT m_boot = {"20190503", "10:02"};
    struct LOG_MSG_XORG m_xorg = {"20190503", "10:02"};
    struct LOG_MSG_NORMAL m_normal = {"20190503", "10:02", "uos", "d"};
    struct LOG_MSG_KWIN m_kwin = {"test"};
    struct LOG_MSG_DNF m_dnf = {"20190503", "waring", "test"};
    struct LOG_MSG_DMESG m_dmesg = {"20190503", "waring", "test"};

    QList<LOG_MSG_JOURNAL> m_journalList {m_journal};
    QList<LOG_MSG_APPLICATOIN> m_appList {m_app};
    QList<LOG_MSG_DPKG> m_dpkgList {m_dpkg};
    QList<LOG_MSG_BOOT> m_bootList {m_boot};
    QList<LOG_MSG_XORG> m_xorgList {m_xorg};
    QList<LOG_MSG_NORMAL> m_normalList {m_normal};
    QList<LOG_MSG_KWIN> m_kwinList {m_kwin};
    QList<LOG_MSG_DNF> m_dnfList {m_dnf};
    QList<LOG_MSG_DMESG> m_dmesgList {m_dmesg};
    QString test = "test";
    exportThread->m_canRunning = true;
    exportThread->exportToHtmlPublic("test", &m_model, m_flag);
    exportThread->exportToHtmlPublic("test", m_journalList, QStringList() << "test", m_flag);
    exportThread->exportToHtmlPublic("test", m_appList, QStringList() << "test", test);
    exportThread->exportToHtmlPublic("test", m_dpkgList, QStringList() << "test");
    exportThread->exportToHtmlPublic("test", m_bootList, QStringList() << "test");
    exportThread->exportToHtmlPublic("test", m_xorgList, QStringList() << "test");
    exportThread->exportToHtmlPublic("test", m_normalList, QStringList() << "test");
    exportThread->exportToHtmlPublic("test", m_kwinList, QStringList() << "test");
    exportThread->exportToHtmlPublic("test", m_dnfList, QStringList() << "test");
    exportThread->exportToHtmlPublic("test", m_dmesgList, QStringList() << "test");

    exportThread->exportToHtml("test", new QStandardItemModel(), m_flag);
    exportThread->exportToHtml("test", m_journalList, QStringList() << "test", m_flag);
    exportThread->exportToHtml("test", m_appList, QStringList() << "test", test);
    exportThread->exportToHtml("test", m_dpkgList, QStringList() << "test");
    exportThread->exportToHtml("test", m_bootList, QStringList() << "test");
    exportThread->exportToHtml("test", m_xorgList, QStringList() << "test");
    exportThread->exportToHtml("test", m_normalList, QStringList() << "test");
    exportThread->exportToHtml("test", m_kwinList, QStringList() << "test");
    exportThread->exportToHtml("test", m_dnfList, QStringList() << "test");
    exportThread->exportToHtml("test", m_dmesgList, QStringList() << "test");
}

TEST_F(LogExportthread_UT, ExportToXls_UT)
{
    Stub stub;
    stub.set(ADDR(DocxFactory::WordProcessingMerger, load), stub_load);
    stub.set(ADDR(DocxFactory::WordProcessingMerger, paste), stub_paste);
    stub.set(ADDR(DocxFactory::WordProcessingMerger, save), stub_save);
    stub.set((void (DocxFactory::WordProcessingMerger::*)(const std::string &, const std::string &, const std::string &))ADDR(DocxFactory::WordProcessingMerger, setClipboardValue), stub_setClipboardValue);
    stub.set((bool (QFile::*)() const)ADDR(QFile, exists), stub_dfwexists);
    QStandardItemModel m_model;
    m_model.appendRow(new QStandardItem());
    LOG_FLAG m_flag = LOG_FLAG::JOURNAL;
    struct LOG_MSG_JOURNAL m_journal = {"20190503", "10:02", "uos", "d", "1", "2"};
    struct LOG_MSG_APPLICATOIN m_app = {"20190503", "10:02", "uos", "d"};
    struct LOG_MSG_DPKG m_dpkg = {"20190503", "10:02", "uos"};
    struct LOG_MSG_BOOT m_boot = {"20190503", "10:02"};
    struct LOG_MSG_XORG m_xorg = {"20190503", "10:02"};
    struct LOG_MSG_NORMAL m_normal = {"20190503", "10:02", "uos", "d"};
    struct LOG_MSG_KWIN m_kwin = {"test"};
    struct LOG_MSG_DNF m_dnf = {"20190503", "waring", "test"};
    struct LOG_MSG_DMESG m_dmesg = {"20190503", "waring", "test"};

    QList<LOG_MSG_JOURNAL> m_journalList {m_journal};
    QList<LOG_MSG_APPLICATOIN> m_appList {m_app};
    QList<LOG_MSG_DPKG> m_dpkgList {m_dpkg};
    QList<LOG_MSG_BOOT> m_bootList {m_boot};
    QList<LOG_MSG_XORG> m_xorgList {m_xorg};
    QList<LOG_MSG_NORMAL> m_normalList {m_normal};
    QList<LOG_MSG_KWIN> m_kwinList {m_kwin};
    QList<LOG_MSG_DNF> m_dnfList {m_dnf};
    QList<LOG_MSG_DMESG> m_dmesgList {m_dmesg};
    QString test = "test";
    exportThread->m_canRunning = true;

    exportThread->exportToXlsPublic("test", &m_model, m_flag);
    exportThread->exportToXlsPublic("test", m_journalList, QStringList() << "test", m_flag);
    exportThread->exportToXlsPublic("test", m_appList, QStringList() << "test", test);
    exportThread->exportToXlsPublic("test", m_dpkgList, QStringList() << "test");
    exportThread->exportToXlsPublic("test", m_bootList, QStringList() << "test");
    exportThread->exportToXlsPublic("test", m_xorgList, QStringList() << "test");
    exportThread->exportToXlsPublic("test", m_normalList, QStringList() << "test");
    exportThread->exportToXlsPublic("test", m_kwinList, QStringList() << "test");
    exportThread->exportToXlsPublic("test", m_dnfList, QStringList() << "test");
    exportThread->exportToXlsPublic("test", m_dmesgList, QStringList() << "test");

    exportThread->exportToXls("test", m_journalList, QStringList() << "test", m_flag);
    exportThread->exportToXls("test", m_appList, QStringList() << "test", test);
    exportThread->exportToXls("test", m_dpkgList, QStringList() << "test");
    exportThread->exportToXls("test", m_bootList, QStringList() << "test");
    exportThread->exportToXls("test", m_xorgList, QStringList() << "test");
    exportThread->exportToXls("test", m_normalList, QStringList() << "test");
    exportThread->exportToXls("test", m_kwinList, QStringList() << "test");
    exportThread->exportToXls("test", m_dnfList, QStringList() << "test");
    exportThread->exportToXls("test", m_dmesgList, QStringList() << "test");
}

TEST_F(LogExportthread_UT, ExportRun_UT)
{
    Stub stub;
    stub.set(ADDR(DocxFactory::WordProcessingMerger, load), stub_load);
    stub.set(ADDR(DocxFactory::WordProcessingMerger, paste), stub_paste);
    stub.set(ADDR(DocxFactory::WordProcessingMerger, save), stub_save);
    stub.set((void (DocxFactory::WordProcessingMerger::*)(const std::string &, const std::string &, const std::string &))ADDR(DocxFactory::WordProcessingMerger, setClipboardValue), stub_setClipboardValue);
    stub.set((bool (QFile::*)() const)ADDR(QFile, exists), stub_dfwexists);

    QStandardItemModel m_model;
    m_model.appendRow(new QStandardItem());
    LOG_FLAG m_flag = LOG_FLAG::JOURNAL;
    struct LOG_MSG_JOURNAL m_journal = {"20190503", "10:02", "uos", "d", "1", "2"};
    struct LOG_MSG_APPLICATOIN m_app = {"20190503", "10:02", "uos", "d"};
    struct LOG_MSG_DPKG m_dpkg = {"20190503", "10:02", "uos"};
    struct LOG_MSG_BOOT m_boot = {"20190503", "10:02"};
    struct LOG_MSG_XORG m_xorg = {"20190503", "10:02"};
    struct LOG_MSG_NORMAL m_normal = {"20190503", "10:02", "uos", "d"};
    struct LOG_MSG_KWIN m_kwin = {"test"};
    struct LOG_MSG_DNF m_dnf = {"20190503", "waring", "test"};
    struct LOG_MSG_DMESG m_dmesg = {"20190503", "waring", "test"};

    QList<LOG_MSG_JOURNAL> m_journalList {m_journal};
    QList<LOG_MSG_APPLICATOIN> m_appList {m_app};
    QList<LOG_MSG_DPKG> m_dpkgList {m_dpkg};
    QList<LOG_MSG_BOOT> m_bootList {m_boot};
    QList<LOG_MSG_XORG> m_xorgList {m_xorg};
    QList<LOG_MSG_NORMAL> m_normalList {m_normal};
    QList<LOG_MSG_KWIN> m_kwinList {m_kwin};
    QList<LOG_MSG_DNF> m_dnfList {m_dnf};
    QList<LOG_MSG_DMESG> m_dmesgList {m_dmesg};
    QString test = "test";
    exportThread->m_canRunning = false;
    exportThread->m_allLoadComplete = true;

    exportThread->m_runMode = LogExportThread::HtmlModel;
    exportThread->run();
    exportThread->m_runMode = LogExportThread::HtmlAPP;
    exportThread->run();
    exportThread->m_runMode = LogExportThread::HtmlDNF;
    exportThread->run();
    exportThread->m_runMode = LogExportThread::HtmlBOOT;
    exportThread->run();
    exportThread->m_runMode = LogExportThread::HtmlDPKG;
    exportThread->run();
    exportThread->m_runMode = LogExportThread::HtmlKWIN;
    exportThread->run();
    exportThread->m_runMode = LogExportThread::HtmlDMESG;
    exportThread->run();
    exportThread->m_runMode = LogExportThread::HtmlXORG;
    exportThread->run();
    exportThread->m_runMode = LogExportThread::HtmlNORMAL;
    exportThread->run();
    exportThread->m_runMode = LogExportThread::HtmlJOURNAL;
    exportThread->run();

    exportThread->m_runMode = LogExportThread::TxtModel;
    exportThread->run();
    exportThread->m_runMode = LogExportThread::TxtAPP;
    exportThread->run();
    exportThread->m_runMode = LogExportThread::TxtDNF;
    exportThread->run();
    exportThread->m_runMode = LogExportThread::TxtBOOT;
    exportThread->run();
    exportThread->m_runMode = LogExportThread::TxtDPKG;
    exportThread->run();
    exportThread->m_runMode = LogExportThread::TxtKWIN;
    exportThread->run();
    exportThread->m_runMode = LogExportThread::TxtDMESG;
    exportThread->run();
    exportThread->m_runMode = LogExportThread::TxtXORG;
    exportThread->run();
    exportThread->m_runMode = LogExportThread::TxtNORMAL;
    exportThread->run();
    exportThread->m_runMode = LogExportThread::TxtJOURNAL;
    exportThread->run();

    exportThread->m_runMode = LogExportThread::DocAPP;
    exportThread->run();
    exportThread->m_runMode = LogExportThread::DocDNF;
    exportThread->run();
    exportThread->m_runMode = LogExportThread::DocBOOT;
    exportThread->run();
    exportThread->m_runMode = LogExportThread::DocDPKG;
    exportThread->run();
    exportThread->m_runMode = LogExportThread::DocKWIN;
    exportThread->run();
    exportThread->m_runMode = LogExportThread::DocDMESG;
    exportThread->run();
    exportThread->m_runMode = LogExportThread::DocXORG;
    exportThread->run();
    exportThread->m_runMode = LogExportThread::DocNORMAL;
    exportThread->run();
    exportThread->m_runMode = LogExportThread::DocJOURNAL;
    exportThread->run();

    exportThread->m_runMode = LogExportThread::XlsAPP;
    exportThread->run();
    exportThread->m_runMode = LogExportThread::XlsDNF;
    exportThread->run();
    exportThread->m_runMode = LogExportThread::XlsBOOT;
    exportThread->run();
    exportThread->m_runMode = LogExportThread::XlsDPKG;
    exportThread->run();
    exportThread->m_runMode = LogExportThread::XlsKWIN;
    exportThread->run();
    exportThread->m_runMode = LogExportThread::XlsDMESG;
    exportThread->run();
    exportThread->m_runMode = LogExportThread::XlsXORG;
    exportThread->run();
    exportThread->m_runMode = LogExportThread::XlsNORMAL;
    exportThread->run();
    exportThread->m_runMode = LogExportThread::XlsJOURNAL;
    exportThread->run();
}
