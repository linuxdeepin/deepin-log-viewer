// SPDX-FileCopyrightText: 2019 - 2024 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "logsegementexportthread.h"
#include "utils.h"
#include "xlsxwriter.h"
#include "WordProcessingMerger.h"
#include "WordProcessingCompiler.h"
#include "dbusproxy/dldbushandler.h"

#include <DApplication>

#include <QDebug>
#include <QFile>
#include <QTextCodec>
#include <QTextStream>
#include <QTextDocument>
#include <QTextDocumentWriter>
#include <QElapsedTimer>
#include <QStandardPaths>
#include <QProcess>
#include <QLoggingCategory>

#include <malloc.h>
DWIDGET_USE_NAMESPACE

#ifdef QT_DEBUG
Q_LOGGING_CATEGORY(logSegementExport, "org.deepin.log.viewer.segement.export.work")
#else
Q_LOGGING_CATEGORY(logSegementExport, "org.deepin.log.viewer.segement.export.work", QtInfoMsg)
#endif

/**
 * @brief LogSegementExportThread::LogSegementExportThread 导出日志线程类构造函数
 * @param parent 父对象
 */
LogSegementExportThread::LogSegementExportThread(QObject *parent)
    :  QObject(parent),
       QRunnable()
{
    setAutoDelete(true);
}
/**
 * @brief LogSegementExportThread::~LogSegementExportThread 析构函数
 */
LogSegementExportThread::~LogSegementExportThread()
{
    qCDebug(logSegementExport) << "LogSegementExportThread destoryed.";
    //释放空闲内存
    malloc_trim(0);
}

void LogSegementExportThread::setParameter(const QString &fileName, const QList<QString> &jList, const QStringList &lables, LOG_FLAG flag)
{
    QMutexLocker locker(&mutex);
    m_fileName = fileName;
    m_logDataList = jList;
    m_flag = flag;
    m_labels = lables;
    m_bForceStop = false;
    if (fileName.endsWith(".doc")) {
        m_runMode = Doc;
        if (!m_pDocMerger)
            initDoc();
    } else if (fileName.endsWith(".xls")) {
        m_runMode = Xls;
        if (!m_pWorkbook)
            initXls();
    }
    condition.wakeOne();
}

void LogSegementExportThread::initDoc()
{
    QString tempdir = getDocTemplatePath();
    if (tempdir.isEmpty())
        return;

    m_pDocMerger = &DocxFactory:: WordProcessingMerger::getInstance();
    m_pDocMerger->load(tempdir.toStdString());
    //往表头中添加表头描述，表头为第一行，数据则在下面
    for (int col = 0; col < m_labels.count(); ++col) {
        m_pDocMerger->setClipboardValue("tableRow", QString("column%1").arg(col + 1).toStdString(), m_labels.at(col).toStdString());
    }
    m_pDocMerger->paste("tableRow");
}

void LogSegementExportThread::initXls()
{
    m_currentXlsRow = 0;
    m_pWorkbook = workbook_new(m_fileName.toStdString().c_str());
    m_pWorksheet = workbook_add_worksheet(m_pWorkbook, nullptr);
    lxw_format *format = workbook_add_format(m_pWorkbook);
    format_set_bold(format);
    for (int col = 0; col < m_labels.count(); ++col) {
        worksheet_write_string(m_pWorksheet, static_cast<lxw_row_t>(m_currentXlsRow), static_cast<lxw_col_t>(col), m_labels.at(col).toStdString().c_str(), format);
    }
    ++m_currentXlsRow;
}

QString LogSegementExportThread::getDocTemplatePath()
{
    QString tempdir("");
    if (m_flag == KERN) {
        tempdir = "/usr/share/deepin-log-viewer/DocxTemplate/4column.dfw";
    } else if (m_flag == Kwin) {
        tempdir = "/usr/share/deepin-log-viewer/DocxTemplate/1column.dfw";
    } else {
        qCWarning(logSegementExport) << "exportToDoc type is Wrong!";
        return "";
    }
    if (!QFile(tempdir).exists()) {
        qCWarning(logSegementExport) << "export docx template is not exisits";
        return "";
    }

    return tempdir;
}

/**
 * @brief LogSegementExportThread::isProcessing 返回当前线程获取数据逻辑启动停止控制的变量
 * @return 当前线程获取数据逻辑启动停止控制的变量
 */
bool LogSegementExportThread::isProcessing()
{
    return !m_bForceStop;
}

/**
 * @brief LogSegementExportThread::stopImmediately 强制停止前线程获取数据逻辑
 */
void LogSegementExportThread::stopImmediately()
{
    QMutexLocker locker(&mutex);
    m_bForceStop = true;
    condition.wakeOne();
}

/**
 * @brief LogSegementExportThread::stop 停止前线程获取数据逻辑
 */
void LogSegementExportThread::stop()
{
    QMutexLocker locker(&mutex);
    m_bStop = true;
    condition.wakeOne();
}

/**
 * @brief LogSegementExportThread::run 线程的run函数，通过配置类型执行相应的导出逻辑
 */
void LogSegementExportThread::run()
{
    qCDebug(logSegementExport) << "threadrun";
    sigProgress(0, 100);

    QMutexLocker locker(&mutex);
    while(!m_bForceStop && !m_bStop) {
        if (m_logDataList.isEmpty()) {
            condition.wait(&mutex);
        } else {
            try {
                switch (m_runMode) {
                case Doc: {
                    exportToDoc();
                    break;
                }

                case Xls: {
                    exportToXls();
                    break;
                }
                default:
                    break;
                }
                m_logDataList.clear();
            } catch (const QString &ErrorStr) {
                qCWarning(logSegementExport) << "Export Stop" << ErrorStr;
                emit sigResult(false);
                if (ErrorStr != m_forceStopStr) {
                    emit sigError(QString("export error: %1").arg(ErrorStr));
                }
            }
        }
    }

    if (!m_bForceStop) {
        // 保存数据
        switch (m_runMode) {
        case Doc:
            saveDoc();
            break;
        case Xls:
            closeXls();
            break;
        default:
            break;
        }

        sigProgress(100, 100);
        //延时200ms再发送导出成功信号，关闭导出进度框，让100%的进度有时间显示
        Utils::sleep(200);
    }

    emit sigResult(!m_bForceStop);

    if (m_bForceStop) {
        Utils::checkAndDeleteDir(m_fileName);
    }

    m_bForceStop = false;
}

bool LogSegementExportThread::exportToDoc()
{
    if (!m_pDocMerger)
        return false;

    for (int row = 0; row < m_logDataList.count(); ++row) {
        //导出逻辑启动停止控制，外部把m_canRunning置false时停止运行，抛出异常处理
        if (m_bForceStop) {
            throw  QString(m_forceStopStr);
        }
        LOG_MSG_BASE message;
        message.fromJson(m_logDataList.at(row));
        //把数据填入表格单元格中
        if (m_flag == KERN) {
            m_pDocMerger->setClipboardValue("tableRow", QString("column1").toStdString(), message.dateTime.toStdString());
            m_pDocMerger->setClipboardValue("tableRow", QString("column2").toStdString(), message.hostName.toStdString());
            m_pDocMerger->setClipboardValue("tableRow", QString("column3").toStdString(), message.daemonName.toStdString());
            m_pDocMerger->setClipboardValue("tableRow", QString("column4").toStdString(), message.msg.toStdString());
        } else if (m_flag == Kwin) {
            m_pDocMerger->setClipboardValue("tableRow", QString("column1").toStdString(), message.msg.toStdString());
        }
        m_pDocMerger->paste("tableRow");
    }

    return true;
}

bool LogSegementExportThread::exportToXls()
{
    if (!m_pWorksheet)
        return false;

    for (int row = 0; row < m_logDataList.count() ; ++row) {
        if (m_bForceStop) {
            throw  QString(m_forceStopStr);
        }
        LOG_MSG_BASE message;
        message.fromJson(m_logDataList.at(row));
        int col = 0;

        if (m_flag == KERN) {
            worksheet_write_string(m_pWorksheet, static_cast<lxw_row_t>(m_currentXlsRow), static_cast<lxw_col_t>(col++), message.dateTime.toStdString().c_str(), nullptr);
            worksheet_write_string(m_pWorksheet, static_cast<lxw_row_t>(m_currentXlsRow), static_cast<lxw_col_t>(col++), message.hostName.toStdString().c_str(), nullptr);
            worksheet_write_string(m_pWorksheet, static_cast<lxw_row_t>(m_currentXlsRow), static_cast<lxw_col_t>(col++), message.daemonName.toStdString().c_str(), nullptr);
            worksheet_write_string(m_pWorksheet, static_cast<lxw_row_t>(m_currentXlsRow), static_cast<lxw_col_t>(col++), message.msg.toStdString().c_str(), nullptr);
        } else if (m_flag == Kwin) {
            worksheet_write_string(m_pWorksheet, static_cast<lxw_row_t>(m_currentXlsRow), static_cast<lxw_col_t>(col++), message.msg.toStdString().c_str(), nullptr);
        }

        ++m_currentXlsRow;
    }

    return true;
}

void LogSegementExportThread::saveDoc()
{
    //保存，把拼好的xml写入文件中
    QString fileNamex = m_fileName + "x";

    QFile rsNameFile(m_fileName) ;
    if (rsNameFile.exists()) {
        rsNameFile.remove();
    }
    m_pDocMerger->save(fileNamex.toStdString());
    QFile(fileNamex).rename(m_fileName);
}

void LogSegementExportThread::closeXls()
{
    workbook_close(m_pWorkbook);
    malloc_trim(0);
}
