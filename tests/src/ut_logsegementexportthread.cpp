// SPDX-FileCopyrightText: 2019 - 2024 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "logsegementexportthread.h"
#include "structdef.h"
#include "utils.h"
#include "../../3rdparty/DocxFactory/include/DocxFactory/WordProcessingMerger/WordProcessingMerger.h"
#include <stub.h>

#include <QFile>
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>

#include <gtest/gtest.h>

// ---- DocxFactory stubs ----
static void seg_stub_load(const std::string &p_fileName)
{
    Q_UNUSED(p_fileName);
}

static void seg_stub_setClipboardValue(const std::string &p_itemName, const std::string &p_fieldName, const std::string &p_value)
{
    Q_UNUSED(p_itemName);
    Q_UNUSED(p_fieldName);
    Q_UNUSED(p_value);
}

static void seg_stub_paste(const std::string &p_itemName)
{
    Q_UNUSED(p_itemName);
}

static void seg_stub_save(const std::string &p_fileName)
{
    Q_UNUSED(p_fileName);
}

// ---- Utils stubs ----
static bool seg_stub_sleep(unsigned int msec)
{
    Q_UNUSED(msec);
    return true;
}

static bool seg_stub_checkAndDeleteDir(const QString &iFilePath)
{
    Q_UNUSED(iFilePath);
    return true;
}

// Build a compact JSON string that LOG_MSG_BASE::fromJson can parse.
static QString makeLogJson(const QString &msg, const QString &dateTime = "2026-10-27 10:00:00",
                           const QString &host = "uos", const QString &daemon = "daemon",
                           LOG_FLAG type = KERN)
{
    QJsonObject obj;
    obj.insert("type", static_cast<int>(type));
    obj.insert("msg", msg);
    obj.insert("dateTime", dateTime);
    obj.insert("hostName", host);
    obj.insert("daemonName", daemon);
    obj.insert("daemonId", "1");
    obj.insert("level", "info");
    return QString::fromUtf8(QJsonDocument(obj).toJson(QJsonDocument::Compact));
}

static void removeTempFile(const QString &name)
{
    QFile f(name);
    if (f.exists())
        f.remove();
}

class LogSegementExportThread_UT : public testing::Test
{
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    void SetUp() override
    {
        m_thread = new LogSegementExportThread();
    }
    void TearDown() override
    {
        delete m_thread;
        m_thread = nullptr;
    }
    LogSegementExportThread *m_thread = nullptr;
};

// ctor / dtor / inline setters / state queries
TEST_F(LogSegementExportThread_UT, ConstructorAndBasicAccessors)
{
    ASSERT_NE(m_thread, nullptr);
    m_thread->enableAppendWrite(true);
    m_thread->enableAppendWrite(false);
    m_thread->setTotalProcess(100);

    // isProcessing() returns !m_bForceStop; initially false -> true
    EXPECT_TRUE(m_thread->isProcessing());

    m_thread->stop();
    m_thread->stopImmediately();
    // After stopImmediately() m_bForceStop is true -> isProcessing() false
    EXPECT_FALSE(m_thread->isProcessing());
}

// setParameter: all four RUN_MODE branches (Txt/Html/Doc/Xls) + UnKnown
TEST_F(LogSegementExportThread_UT, SetParameter_AllModes)
{
    Stub stub;
    stub.set(ADDR(DocxFactory::WordProcessingMerger, load), seg_stub_load);
    stub.set(ADDR(DocxFactory::WordProcessingMerger, paste), seg_stub_paste);
    stub.set(ADDR(DocxFactory::WordProcessingMerger, save), seg_stub_save);
    stub.set((void (DocxFactory::WordProcessingMerger::*)(const std::string &, const std::string &, const std::string &))
                 ADDR(DocxFactory::WordProcessingMerger, setClipboardValue),
             seg_stub_setClipboardValue);

    QStringList labels = QStringList() << "time"
                                       << "host"
                                       << "daemon"
                                       << "msg";
    QList<QString> data;
    data << makeLogJson("hello <b>");

    // Txt
    m_thread->setParameter("/tmp/ut_seg_test.txt", data, labels, KERN);
    EXPECT_EQ(m_thread->m_runMode, LogSegementExportThread::Txt);

    // Html
    m_thread->setParameter("/tmp/ut_seg_test.html", data, labels, KERN);
    EXPECT_EQ(m_thread->m_runMode, LogSegementExportThread::Html);

    // Doc -> initDoc
    m_thread->setParameter("/tmp/ut_seg_test.doc", data, labels, KERN);
    EXPECT_EQ(m_thread->m_runMode, LogSegementExportThread::Doc);
    EXPECT_NE(m_thread->m_pDocMerger, nullptr);

    // Xls -> initXls (real workbook on temp file)
    m_thread->setParameter("/tmp/ut_seg_test.xls", data, labels, KERN);
    EXPECT_EQ(m_thread->m_runMode, LogSegementExportThread::Xls);
    EXPECT_NE(m_thread->m_pWorkbook, nullptr);
    EXPECT_NE(m_thread->m_pWorksheet, nullptr);

    // clean up xls workbook created by initXls
    m_thread->closeXls();
    removeTempFile("/tmp/ut_seg_test.xls");
}

// getDocTemplatePath: KERN / Kwin / other branches
TEST_F(LogSegementExportThread_UT, GetDocTemplatePath_Branches)
{
    m_thread->m_flag = KERN;
    QString kernPath = m_thread->getDocTemplatePath();
    EXPECT_FALSE(kernPath.isEmpty());

    m_thread->m_flag = Kwin;
    QString kwinPath = m_thread->getDocTemplatePath();
    EXPECT_FALSE(kwinPath.isEmpty());

    m_thread->m_flag = NONE;
    QString nonePath = m_thread->getDocTemplatePath();
    EXPECT_TRUE(nonePath.isEmpty());
}

// initDoc / initXls direct invocation (full load/paste/setClipboard coverage)
TEST_F(LogSegementExportThread_UT, InitDoc_InitXls)
{
    Stub stub;
    stub.set(ADDR(DocxFactory::WordProcessingMerger, load), seg_stub_load);
    stub.set(ADDR(DocxFactory::WordProcessingMerger, paste), seg_stub_paste);
    stub.set((void (DocxFactory::WordProcessingMerger::*)(const std::string &, const std::string &, const std::string &))
                 ADDR(DocxFactory::WordProcessingMerger, setClipboardValue),
             seg_stub_setClipboardValue);

    m_thread->m_flag = KERN;
    m_thread->m_labels = QStringList() << "time"
                                       << "host"
                                       << "daemon"
                                       << "msg";
    m_thread->initDoc();
    EXPECT_NE(m_thread->m_pDocMerger, nullptr);

    m_thread->m_fileName = "/tmp/ut_seg_initxls.xls";
    m_thread->m_labels = QStringList() << "time"
                                       << "host"
                                       << "daemon"
                                       << "msg";
    m_thread->initXls();
    EXPECT_NE(m_thread->m_pWorkbook, nullptr);
    EXPECT_EQ(m_thread->m_currentXlsRow, 1);
    m_thread->closeXls();
    removeTempFile("/tmp/ut_seg_initxls.xls");
}

// exportTxt: KERN + Kwin branches
TEST_F(LogSegementExportThread_UT, ExportTxt_KernKwin)
{
    QString fname = "/tmp/ut_seg_export_txt.txt";
    removeTempFile(fname);

    QList<QString> data;
    data << makeLogJson("line1 <tag>") << makeLogJson("line2 ? | ￥", "2026-10-27 11:00:00", "h", "d");

    QStringList labels = QStringList() << "time"
                                       << "host"
                                       << "daemon"
                                       << "msg";
    m_thread->setParameter(fname, data, labels, KERN);
    EXPECT_TRUE(m_thread->exportTxt());

    // Kwin branch
    m_thread->m_flag = Kwin;
    m_thread->m_labels = QStringList() << "msg";
    EXPECT_TRUE(m_thread->exportTxt());

    // append-write branch
    m_thread->enableAppendWrite(true);
    EXPECT_TRUE(m_thread->exportTxt());

    removeTempFile(fname);
}

// exportTxt: file open failure path
TEST_F(LogSegementExportThread_UT, ExportTxt_OpenFail)
{
    // An unwritable directory path makes QFile::open fail.
    m_thread->m_fileName = "/nonexistent_dir/ut_seg_fail.txt";
    m_thread->m_flag = KERN;
    m_thread->m_labels = QStringList() << "a";
    QList<QString> data;
    data << makeLogJson("x");
    m_thread->m_logDataList = data;
    EXPECT_FALSE(m_thread->exportTxt());
}

// exportHtml: KERN + Kwin branches
TEST_F(LogSegementExportThread_UT, ExportHtml_KernKwin)
{
    QString fname = "/tmp/ut_seg_export_html.html";
    removeTempFile(fname);

    QList<QString> data;
    data << makeLogJson("<html>line1</html>") << makeLogJson("line2 ? | ￥");

    QStringList labels = QStringList() << "time"
                                       << "host"
                                       << "daemon"
                                       << "msg";
    m_thread->setParameter(fname, data, labels, KERN);
    EXPECT_TRUE(m_thread->exportHtml());

    // Kwin branch
    m_thread->m_flag = Kwin;
    m_thread->m_labels = QStringList() << "msg";
    EXPECT_TRUE(m_thread->exportHtml());

    removeTempFile(fname);
}

// exportHtml: open failure path
TEST_F(LogSegementExportThread_UT, ExportHtml_OpenFail)
{
    m_thread->m_fileName = "/nonexistent_dir/ut_seg_fail.html";
    m_thread->m_flag = KERN;
    m_thread->m_labels = QStringList() << "a";
    QList<QString> data;
    data << makeLogJson("x");
    m_thread->m_logDataList = data;
    EXPECT_FALSE(m_thread->exportHtml());
}

// exportToDoc: KERN + Kwin branches
TEST_F(LogSegementExportThread_UT, ExportToDoc_KernKwin)
{
    Stub stub;
    stub.set(ADDR(DocxFactory::WordProcessingMerger, load), seg_stub_load);
    stub.set(ADDR(DocxFactory::WordProcessingMerger, paste), seg_stub_paste);
    stub.set((void (DocxFactory::WordProcessingMerger::*)(const std::string &, const std::string &, const std::string &))
                 ADDR(DocxFactory::WordProcessingMerger, setClipboardValue),
             seg_stub_setClipboardValue);

    QStringList labels = QStringList() << "time"
                                       << "host"
                                       << "daemon"
                                       << "msg";
    QList<QString> data;
    data << makeLogJson("doc1") << makeLogJson("doc2");

    m_thread->setParameter("/tmp/ut_seg_export_doc.doc", data, labels, KERN);
    EXPECT_TRUE(m_thread->exportToDoc());

    // Kwin branch
    m_thread->m_flag = Kwin;
    m_thread->m_labels = QStringList() << "msg";
    EXPECT_TRUE(m_thread->exportToDoc());

    // null merger guard
    m_thread->m_pDocMerger = nullptr;
    EXPECT_FALSE(m_thread->exportToDoc());
}

// exportToXls: KERN + Kwin branches + null guard
TEST_F(LogSegementExportThread_UT, ExportToXls_KernKwin)
{
    QStringList labels = QStringList() << "time"
                                       << "host"
                                       << "daemon"
                                       << "msg";
    QList<QString> data;
    data << makeLogJson("xls1") << makeLogJson("xls2");

    m_thread->setParameter("/tmp/ut_seg_export_xls.xls", data, labels, KERN);
    EXPECT_TRUE(m_thread->exportToXls());

    // Kwin branch
    m_thread->m_flag = Kwin;
    EXPECT_TRUE(m_thread->exportToXls());

    m_thread->closeXls();
    removeTempFile("/tmp/ut_seg_export_xls.xls");

    // null worksheet guard
    m_thread->m_pWorksheet = nullptr;
    EXPECT_FALSE(m_thread->exportToXls());
}

// htmlEscapeCovert
TEST_F(LogSegementExportThread_UT, HtmlEscapeCovert)
{
    QString msg = "a<b>c?d|e￥f>g";
    m_thread->htmlEscapeCovert(msg);
    EXPECT_FALSE(msg.contains("<"));
    EXPECT_FALSE(msg.contains(">"));
    EXPECT_TRUE(msg.contains("&lt"));
    EXPECT_TRUE(msg.contains("&gt"));
    EXPECT_TRUE(msg.contains("&iexcl"));
    EXPECT_TRUE(msg.contains("&yen"));
    EXPECT_TRUE(msg.contains("&brvbar"));
}

// saveDoc
TEST_F(LogSegementExportThread_UT, SaveDoc)
{
    Stub stub;
    stub.set(ADDR(DocxFactory::WordProcessingMerger, load), seg_stub_load);
    stub.set(ADDR(DocxFactory::WordProcessingMerger, paste), seg_stub_paste);
    stub.set(ADDR(DocxFactory::WordProcessingMerger, save), seg_stub_save);
    stub.set((void (DocxFactory::WordProcessingMerger::*)(const std::string &, const std::string &, const std::string &))
                 ADDR(DocxFactory::WordProcessingMerger, setClipboardValue),
             seg_stub_setClipboardValue);

    m_thread->m_flag = KERN;
    m_thread->m_labels = QStringList() << "time"
                                       << "host"
                                       << "daemon"
                                       << "msg";
    m_thread->initDoc();
    ASSERT_NE(m_thread->m_pDocMerger, nullptr);

    m_thread->m_fileName = "/tmp/ut_seg_savedoc.doc";
    removeTempFile(m_thread->m_fileName);
    EXPECT_NO_FATAL_FAILURE(m_thread->saveDoc());
    removeTempFile(m_thread->m_fileName);
    removeTempFile("/tmp/ut_seg_savedoc.docx");
}

// run(): stop path (m_bStop) covering post-loop save-switch default + sigProgress + sigResult(true)
TEST_F(LogSegementExportThread_UT, Run_StopPath)
{
    Stub stub;
    stub.set(ADDR(Utils, sleep), seg_stub_sleep);

    QList<QString> data;
    data << makeLogJson("run1");
    m_thread->setParameter("/tmp/ut_seg_run.txt", data, QStringList() << "msg", KERN);
    m_thread->m_bStop = true;
    EXPECT_NO_FATAL_FAILURE(m_thread->run());
    removeTempFile("/tmp/ut_seg_run.txt");
}

// run(): force-stop path covering sigResult(false) + checkAndDeleteDir
TEST_F(LogSegementExportThread_UT, Run_ForceStopPath)
{
    Stub stub;
    stub.set(ADDR(Utils, checkAndDeleteDir), seg_stub_checkAndDeleteDir);

    m_thread->m_fileName = "/tmp/ut_seg_forcestop.txt";
    m_thread->m_flag = KERN;
    m_thread->m_runMode = LogSegementExportThread::Txt;
    m_thread->m_bForceStop = true;
    EXPECT_NO_FATAL_FAILURE(m_thread->run());
}

// run(): Doc stop path -> saveDoc inside run
TEST_F(LogSegementExportThread_UT, Run_DocStopPath)
{
    Stub stub;
    stub.set(ADDR(DocxFactory::WordProcessingMerger, load), seg_stub_load);
    stub.set(ADDR(DocxFactory::WordProcessingMerger, paste), seg_stub_paste);
    stub.set(ADDR(DocxFactory::WordProcessingMerger, save), seg_stub_save);
    stub.set((void (DocxFactory::WordProcessingMerger::*)(const std::string &, const std::string &, const std::string &))
                 ADDR(DocxFactory::WordProcessingMerger, setClipboardValue),
             seg_stub_setClipboardValue);
    stub.set(ADDR(Utils, sleep), seg_stub_sleep);

    QList<QString> data;
    data << makeLogJson("rundoc");
    m_thread->setParameter("/tmp/ut_seg_rundoc.doc", data, QStringList() << "msg", KERN);
    m_thread->m_bStop = true;
    EXPECT_NO_FATAL_FAILURE(m_thread->run());
    removeTempFile("/tmp/ut_seg_rundoc.doc");
    removeTempFile("/tmp/ut_seg_rundoc.docx");
}

// run(): Xls stop path -> closeXls inside run
TEST_F(LogSegementExportThread_UT, Run_XlsStopPath)
{
    Stub stub;
    stub.set(ADDR(Utils, sleep), seg_stub_sleep);

    QList<QString> data;
    data << makeLogJson("runxls");
    m_thread->setParameter("/tmp/ut_seg_runxls.xls", data, QStringList() << "msg", KERN);
    m_thread->m_bStop = true;
    EXPECT_NO_FATAL_FAILURE(m_thread->run());
    removeTempFile("/tmp/ut_seg_runxls.xls");
}
