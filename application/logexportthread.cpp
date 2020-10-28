/*
 * Copyright (C) 2019 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     LZ <zhou.lu@archermind.com>
 *
 * Maintainer: LZ <zhou.lu@archermind.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "logexportthread.h"
#include "document.h"
#include "table.h"
#include "xlsxdocument.h"
#include "utils.h"
//#include <Xlsx/Workbook.h>
#include "xlsxwriter.h"
#include "WordProcessingMerger.h"
#include "WordProcessingCompiler.h"
#include <exception>

#include <DApplication>

#include <QDebug>
#include <QFile>
#include <QTextCodec>
#include <QTextStream>
#include <QTextDocument>
#include <QTextDocumentWriter>
#include <QElapsedTimer>

#include <malloc.h>
DWIDGET_USE_NAMESPACE

LogExportThread::LogExportThread(QObject *parent)
    :  QObject(parent),
       QRunnable()
{
    setAutoDelete(true);
    initMap();
}

LogExportThread::~LogExportThread()
{
    malloc_trim(0);
}

void LogExportThread::exportToTxtPublic(QString fileName, QStandardItemModel *pModel, LOG_FLAG flag)
{
    m_fileName = fileName;
    m_pModel = pModel;
    m_flag = flag;
    m_runMode = TxtModel;
    m_canRunning = true;
}

void LogExportThread::exportToTxtPublic(QString fileName, QList<LOG_MSG_JOURNAL> jList,  QStringList labels, LOG_FLAG flag)
{
    m_fileName = fileName;
    m_jList = jList;
    m_runMode = TxtJOURNAL;
    m_labels = labels;
    m_flag = flag;
    m_canRunning = true;

}

void LogExportThread::exportToTxtPublic(QString fileName, QList<LOG_MSG_APPLICATOIN> jList, QStringList labels, QString &iAppName)
{
    m_fileName = fileName;
    m_appList = jList;
    m_labels = labels;
    m_runMode = TxtAPP;
    m_canRunning = true;
    m_appName = iAppName;
}

void LogExportThread::exportToTxtPublic(QString fileName, QList<LOG_MSG_DPKG> jList, QStringList labels)
{
    m_fileName = fileName;
    m_dpkgList = jList;
    m_labels = labels;
    m_runMode = TxtDPKG;
    m_canRunning = true;
}

void LogExportThread::exportToTxtPublic(QString fileName, QList<LOG_MSG_BOOT> jList, QStringList labels)
{
    m_fileName = fileName;
    m_bootList = jList;
    m_labels = labels;
    m_runMode = TxtBOOT;
    m_canRunning = true;
}

void LogExportThread::exportToTxtPublic(QString fileName, QList<LOG_MSG_XORG> jList, QStringList labels)
{
    m_fileName = fileName;
    m_xorgList = jList;
    m_labels = labels;
    m_runMode = TxtXORG;
    m_canRunning = true;
}

void LogExportThread::exportToTxtPublic(QString fileName, QList<LOG_MSG_NORMAL> jList, QStringList labels)
{
    m_fileName = fileName;
    m_normalList = jList;
    m_labels = labels;
    m_runMode = TxtNORMAL;
    m_canRunning = true;
}

void LogExportThread::exportToTxtPublic(QString fileName, QList<LOG_MSG_KWIN> jList, QStringList labels)
{
    m_fileName = fileName;
    m_kwinList = jList;
    m_labels = labels;
    m_runMode = TxtKWIN;
    m_canRunning = true;
}

void LogExportThread::exportToHtmlPublic(QString fileName, QStandardItemModel *pModel, LOG_FLAG flag)
{
    m_fileName = fileName;
    m_pModel = pModel;
    m_flag = flag;
    m_runMode = HtmlModel;
    m_canRunning = true;

}

void LogExportThread::exportToHtmlPublic(QString fileName, QList<LOG_MSG_JOURNAL> jList, QStringList labels, LOG_FLAG flag)
{
    m_fileName = fileName;
    m_jList = jList;
    m_runMode = HtmlJOURNAL;
    m_labels = labels;
    m_flag = flag;
    m_canRunning = true;

}

void LogExportThread::exportToHtmlPublic(QString fileName, QList<LOG_MSG_APPLICATOIN> jList, QStringList labels, QString &iAppName)
{
    m_fileName = fileName;
    m_appList = jList;
    m_labels = labels;
    m_runMode = HtmlAPP;
    m_canRunning = true;
    m_appName = iAppName;
}

void LogExportThread::exportToHtmlPublic(QString fileName, QList<LOG_MSG_DPKG> jList, QStringList labels)
{
    m_fileName = fileName;
    m_dpkgList = jList;
    m_labels = labels;
    m_runMode = HtmlDPKG;
    m_canRunning = true;
}

void LogExportThread::exportToHtmlPublic(QString fileName, QList<LOG_MSG_BOOT> jList, QStringList labels)
{
    m_fileName = fileName;
    m_bootList = jList;
    m_labels = labels;
    m_runMode = HtmlBOOT;
    m_canRunning = true;
}

void LogExportThread::exportToHtmlPublic(QString fileName, QList<LOG_MSG_XORG> jList, QStringList labels)
{
    m_fileName = fileName;
    m_xorgList = jList;
    m_labels = labels;
    m_runMode = HtmlXORG;
    m_canRunning = true;
}

void LogExportThread::exportToHtmlPublic(QString fileName, QList<LOG_MSG_NORMAL> jList, QStringList labels)
{
    m_fileName = fileName;
    m_normalList = jList;
    m_labels = labels;
    m_runMode = HtmlNORMAL;
    m_canRunning = true;
}

void LogExportThread::exportToHtmlPublic(QString fileName, QList<LOG_MSG_KWIN> jList, QStringList labels)
{
    m_fileName = fileName;
    m_kwinList = jList;
    m_labels = labels;
    m_runMode = HtmlKWIN;
    m_canRunning = true;
}

void LogExportThread::exportToDocPublic(QString fileName, QStandardItemModel *pModel, LOG_FLAG flag)
{
    m_fileName = fileName;
    m_pModel = pModel;
    m_flag = flag;
    m_runMode = DocModel;
    m_canRunning = true;

}

void LogExportThread::exportToDocPublic(QString fileName, QList<LOG_MSG_JOURNAL> jList, QStringList labels, LOG_FLAG iFlag)
{
    m_fileName = fileName;
    m_jList = jList;
    m_flag = iFlag;
    m_labels = labels;
    m_runMode = DocJOURNAL;
    m_canRunning = true;

}

void LogExportThread::exportToDocPublic(QString fileName, QList<LOG_MSG_APPLICATOIN> jList, QStringList labels, QString &iAppName)
{
    m_fileName = fileName;
    m_appList = jList;
    m_labels = labels;
    m_runMode = DocAPP;
    m_canRunning = true;
    m_appName = iAppName;
}

void LogExportThread::exportToDocPublic(QString fileName, QList<LOG_MSG_DPKG> jList, QStringList labels)
{
    m_fileName = fileName;
    m_dpkgList = jList;
    m_labels = labels;
    m_runMode = DocDPKG;
    m_canRunning = true;
}

void LogExportThread::exportToDocPublic(QString fileName, QList<LOG_MSG_BOOT> jList, QStringList labels)
{
    m_fileName = fileName;
    m_bootList = jList;
    m_labels = labels;
    m_runMode = DocBOOT;
    m_canRunning = true;
}

void LogExportThread::exportToDocPublic(QString fileName, QList<LOG_MSG_XORG> jList, QStringList labels)
{
    m_fileName = fileName;
    m_xorgList = jList;
    m_labels = labels;
    m_runMode = DocXORG;
    m_canRunning = true;
}

void LogExportThread::exportToDocPublic(QString fileName, QList<LOG_MSG_NORMAL> jList, QStringList labels)
{
    m_fileName = fileName;
    m_normalList = jList;
    m_labels = labels;
    m_runMode = DocNORMAL;
    m_canRunning = true;
}

void LogExportThread::exportToDocPublic(QString fileName, QList<LOG_MSG_KWIN> jList, QStringList labels)
{
    m_fileName = fileName;
    m_kwinList = jList;
    m_labels = labels;
    m_runMode = DocKWIN;
    m_canRunning = true;
}

void LogExportThread::exportToXlsPublic(QString fileName, QStandardItemModel *pModel, LOG_FLAG flag)
{
    m_fileName = fileName;
    m_pModel = pModel;
    m_flag = flag;
    m_runMode = XlsModel;
    m_canRunning = true;
}

void LogExportThread::exportToXlsPublic(QString fileName, QList<LOG_MSG_JOURNAL> jList, QStringList labels, LOG_FLAG iFlag)
{
    m_fileName = fileName;
    m_jList = jList;
    m_flag = iFlag;
    m_labels = labels;
    m_runMode = XlsJOURNAL;
    m_canRunning = true;
}

void LogExportThread::exportToXlsPublic(QString fileName, QList<LOG_MSG_APPLICATOIN> jList, QStringList labels, QString &iAppName)
{
    m_fileName = fileName;
    m_appList = jList;
    m_labels = labels;
    m_runMode = XlsAPP;
    m_canRunning = true;
    m_appName = iAppName;
}

void LogExportThread::exportToXlsPublic(QString fileName, QList<LOG_MSG_DPKG> jList, QStringList labels)
{
    m_fileName = fileName;
    m_dpkgList = jList;
    m_labels = labels;
    m_runMode = XlsDPKG;
    m_canRunning = true;
}

void LogExportThread::exportToXlsPublic(QString fileName, QList<LOG_MSG_BOOT> jList, QStringList labels)
{
    m_fileName = fileName;
    m_bootList = jList;
    m_labels = labels;
    m_runMode = XlsBOOT;
    m_canRunning = true;
}

void LogExportThread::exportToXlsPublic(QString fileName, QList<LOG_MSG_XORG> jList, QStringList labels)
{
    m_fileName = fileName;
    m_xorgList = jList;
    m_labels = labels;
    m_runMode = XlsXORG;
    m_canRunning = true;
}

void LogExportThread::exportToXlsPublic(QString fileName, QList<LOG_MSG_NORMAL> jList, QStringList labels)
{
    m_fileName = fileName;
    m_normalList = jList;
    m_labels = labels;
    m_runMode = XlsNORMAL;
    m_canRunning = true;
}

void LogExportThread::exportToXlsPublic(QString fileName, QList<LOG_MSG_KWIN> jList, QStringList labels)
{
    m_fileName = fileName;
    m_kwinList = jList;
    m_labels = labels;
    m_runMode = XlsKWIN;
    m_canRunning = true;
}

void LogExportThread::exportTest()
{
    try {
        QElapsedTimer timer;
        timer.start();
//        connect(&xlsx, &QXlsx::Document::sigProcessAbstractSheet, this, [ = ](int iCurrent, int iTotal) {
//            int end = static_cast<int>(iTotal * 0.1 > 5 ? iTotal * 0.1 : 5);
//            sigProgress(iCurrent + iTotal, iTotal * 3 + end);
//        });
//        connect(&xlsx, &QXlsx::Document::sigProcessharedStrings, this, [ = ](int iCurrent, int iTotal) {
//            int end = static_cast<int>(iTotal * 0.1 > 5 ? iTotal * 0.1 : 5);
//            sigProgress(iCurrent + iTotal * 2, iTotal * 3 + end);
//        });
//        for (int col = 0; col < labels.count(); ++col) {
//            QXlsx::Format boldFont;
//            boldFont.setFontBold(true);
//            xlsx.write(currentXlsRow, col + 1, labels.at(col), boldFont);
//        }
        // ++currentXlsRow;

//        SimpleXlsx::CWorkbook book("Incognito");
//        std::vector<SimpleXlsx::ColumnWidth> ColWidth;
//        SimpleXlsx::CWorksheet &Sheet = book.AddSheet("Unicode", ColWidth);
//        for (int row = 0; row < 1048576; ++row) {
//            Sheet.BeginRow();
//            for (int col = 0; col < 8; ++col) {
//                Sheet.AddCell("OpenXLSX");
//            }
//            Sheet.EndRow();

//        }
//        if (book.Save("/home/zyc/Desktop/Simple.xlsx"))  qDebug() << "The book has been saved successfully" ;
//        else qDebug() << "The book saving has been failed" ;


        lxw_workbook  *workbook  = workbook_new("/home/zyc/Desktop/Simple.xlsx");
        lxw_worksheet *worksheet = workbook_add_worksheet(workbook, NULL);
        for (int i = 0; i < 1048576; ++i) {
            for (int j = 0; j < 8; ++j) {
                worksheet_write_string(worksheet, i, j, "OpenXLSX", NULL);
            }
        }

        workbook_close(workbook);
        malloc_trim(0);
        qDebug() << "exportTest" << workbook << worksheet << timer.elapsed();


    } catch (QString ErrorStr) {
        qDebug() << "Export Stop" << ErrorStr;
        emit sigResult(false);
        if (ErrorStr != stopStr) {
            emit sigError(QString("export error: %1").arg(ErrorStr));
        }
        return ;
    }
    // emit sigResult(m_canRunning);
    return  ;
}

void LogExportThread::exportDocTest()
{

    try {
        DocxFactory::WordProcessingCompiler &l_compiler =
            DocxFactory::WordProcessingCompiler::getInstance();
        time_t l_start = clock();
        l_compiler.compile(
            "/home/zyc/Documents/work_space/works/oldlog/dde_log_viewer/3rdparty/DocxFactory/exercises/templates/test1.docx",
            "/home/zyc/Documents/work_space/works/oldlog/dde_log_viewer/3rdparty/DocxFactory/exercises/templates/test1.dfw");



        DocxFactory:: WordProcessingMerger &l_merger =
            DocxFactory:: WordProcessingMerger::getInstance();
        l_merger.load(
            "/home/zyc/Documents/work_space/works/oldlog/dde_log_viewer/3rdparty/DocxFactory/exercises/templates/test1.dfw");


        for (int i = 0; i < 1000; i++) {
            for (int col = 1; col <= 8; ++col) {
                l_merger.setClipboardValue("tableRow", QString("column%1").arg(col).toStdString(), "testDocxtestDocxtestDocxtestDocxtestDocxtestDocxtestDocxtestDocx");
            }

            l_merger.paste("tableRow");
        }



        l_merger.save("/home/zyc/Desktop/docfactory_benchmark.docx");


        qDebug()  << "Completed (in "
                  << (double)(clock() - l_start) / CLOCKS_PER_SEC
                  << " seconds)."
                  ;
    } catch (const std::exception &p_exception) {
        qDebug() << p_exception.what() ;
    }
}

bool LogExportThread::isProcessing()
{
    return m_canRunning;
}

void LogExportThread::stopImmediately()
{
    m_canRunning = false;
}

bool LogExportThread::exportToTxt(QString fileName, QStandardItemModel *pModel, LOG_FLAG flag)
{
    QFile fi(fileName);
    if (!fi.open(QIODevice::WriteOnly)) {
        emit sigResult(false);
        emit sigError(openErroStr);
        return false;
    }
    try {
        if (!pModel) {
            throw  QString("model is null");
        }
        QTextStream out(&fi);
        if (flag == APP) {
            for (int row = 0; row < pModel->rowCount(); ++row) {
                if (!m_canRunning) {
                    throw  QString(stopStr);
                }
                out << pModel->horizontalHeaderItem(0)->text() << ": "
                    << pModel->item(row, 0)->data(Qt::UserRole + 6).toString() << " ";
                for (int col = 1; col < pModel->columnCount(); ++col) {
                    out << pModel->horizontalHeaderItem(col)->text() << ": "
                        << pModel->item(row, col)->text() << " ";
                }
                out << "\n";
                sigProgress(row + 1, pModel->rowCount());
            }
        } else {
            for (int row = 0; row < pModel->rowCount(); row++) {
                if (!m_canRunning) {
                    throw  QString(stopStr);
                }
                for (int col = 0; col < pModel->columnCount(); col++) {
                    out << pModel->horizontalHeaderItem(col)->text() << ": "
                        << pModel->item(row, col)->text() << " ";
                }
                out << "\n";
                sigProgress(row + 1, pModel->rowCount());
            }
        }
    } catch (QString ErrorStr) {
        qDebug() << "Export Stop" << ErrorStr;
        fi.close();
        emit sigResult(false);
        if (ErrorStr != stopStr) {
            emit sigError(QString("export error: %1").arg(ErrorStr));
        }
        return false;
    }
    fi.close();
    emit sigResult(m_canRunning);
    return true && m_canRunning;
}

bool LogExportThread::exportToTxt(QString fileName, QList<LOG_MSG_JOURNAL> jList,  QStringList labels, LOG_FLAG flag)
{
    QFile fi(fileName);
    if (!fi.open(QIODevice::WriteOnly)) {
        emit sigResult(false);
        emit sigError(openErroStr);
        return false;
    }
    try {

        QTextStream out(&fi);
        if (flag == JOURNAL) {
            for (int i = 0; i < jList.count(); i++) {
                if (!m_canRunning) {
                    throw  QString(stopStr);
                }
                LOG_MSG_JOURNAL jMsg = jList.at(i);
                //        out << QString(DApplication::translate("Table", "Date and Time:")) <<
                //        jMsg.dateTime << " "; out << QString(DApplication::translate("Table", "User:")) <<
                //        jMsg.hostName << " "; out << QString(DApplication::translate("Table", "Daemon:"))
                //        << jMsg.daemonName << " "; out << QString(DApplication::translate("Table",
                //        "PID:")) << jMsg.daemonId << " "; out << QString(DApplication::translate("Table",
                //        "Level:")) << jMsg.level << " "; out << QString(DApplication::translate("Table",
                //        "Info:")) << jMsg.msg << " "; //delete bug
                out << QString(DApplication::translate("Table", "Level:")) << jMsg.level << " ";
                out << QString(DApplication::translate("Table", "Process:")) << jMsg.daemonName << " ";
                out << QString(DApplication::translate("Table", "Date and Time:")) << jMsg.dateTime << " ";
                if (jMsg.msg.isEmpty()) {
                    out << QString(DApplication::translate("Table", "Info:"))
                        << QString(DApplication::translate("Table", "Null")) << " ";  // modify for bug
                } else {
                    out << QString(DApplication::translate("Table", "Info:")) << jMsg.msg << " ";
                }
                out << QString(DApplication::translate("Table", "User:")) << jMsg.hostName << " ";
                out << QString(DApplication::translate("Table", "PID:")) << jMsg.daemonId << " ";
                out << "\n";
                sigProgress(i + 1, jList.count());
            }
        } else if (flag == KERN) {
            for (int i = 0; i < jList.count(); i++) {
                if (!m_canRunning) {
                    throw  QString(stopStr);
                }
                int col = 0;
                LOG_MSG_JOURNAL jMsg = jList.at(i);
                out << labels.value(col++, "") << ":" << jMsg.dateTime << " ";
                out << labels.value(col++, "") << ":" << jMsg.hostName << " ";
                out << labels.value(col++, "") << ":" << jMsg.daemonName << " ";
                out << labels.value(col++, "") << ":" << jMsg.msg << " ";
                out << "\n";
                sigProgress(i + 1, jList.count());
            }
        }
        out.setCodec(QTextCodec::codecForName("utf-8"));

    } catch (QString ErrorStr) {
        qDebug() << "Export Stop" << ErrorStr;
        fi.close();
        emit sigResult(false);
        if (ErrorStr != stopStr) {
            emit sigError(QString("export error: %1").arg(ErrorStr));
        }
        return false;
    }
    fi.close();
    emit sigResult(m_canRunning);
    return true && m_canRunning;
}

bool LogExportThread::exportToTxt(QString fileName, QList<LOG_MSG_APPLICATOIN> jList, QStringList labels, QString &iAppName)
{
    QFile fi(fileName);
    if (!fi.open(QIODevice::WriteOnly)) {
        emit sigResult(false);
        emit sigError(openErroStr);
        return false;
    }
    try {
        QTextStream out(&fi);
        out.setCodec(QTextCodec::codecForName("utf-8"));
        for (int i = 0; i < jList.count(); i++) {
            if (!m_canRunning) {
                throw  QString(stopStr);
            }
            int col = 0;
            LOG_MSG_APPLICATOIN jMsg = jList.at(i);
            out << labels.value(col++, "") << ":" << strTranslate(jMsg.level)  << " ";
            out << labels.value(col++, "") << ":" << jMsg.dateTime << " ";
            out << labels.value(col++, "") << ":" << iAppName << " ";
            out << labels.value(col++, "") << ":" << jMsg.msg << " ";
            out << "\n";
            sigProgress(i + 1, jList.count());
        }
    } catch (QString ErrorStr) {
        qDebug() << "Export Stop" << ErrorStr;
        fi.close();
        emit sigResult(false);
        if (ErrorStr != stopStr) {
            emit sigError(QString("export error: %1").arg(ErrorStr));
        }
        return false;
    }
    fi.close();
    emit sigResult(m_canRunning);
    return true && m_canRunning;
}

bool LogExportThread::exportToTxt(QString fileName, QList<LOG_MSG_DPKG> jList, QStringList labels)
{
    QFile fi(fileName);
    if (!fi.open(QIODevice::WriteOnly)) {
        emit sigResult(false);
        emit sigError(openErroStr);
        return false;
    }
    try {
        QTextStream out(&fi);
        out.setCodec(QTextCodec::codecForName("utf-8"));
        for (int i = 0; i < jList.count(); i++) {
            if (!m_canRunning) {
                throw  QString(stopStr);
            }
            int col = 0;
            LOG_MSG_DPKG jMsg = jList.at(i);
            out << labels.value(col++, "") << ":" << jMsg.dateTime << " ";
            out << labels.value(col++, "") << ":" << jMsg.msg << " ";
            out << labels.value(col++, "") << ":" << jMsg.action << " ";
            out << "\n";
            sigProgress(i + 1, jList.count());
        }
    } catch (QString ErrorStr) {
        qDebug() << "Export Stop" << ErrorStr;
        fi.close();
        emit sigResult(false);
        if (ErrorStr != stopStr) {
            emit sigError(QString("export error: %1").arg(ErrorStr));
        }
        return false;
    }
    fi.close();
    emit sigResult(m_canRunning);
    return true && m_canRunning;
}

bool LogExportThread::exportToTxt(QString fileName, QList<LOG_MSG_BOOT> jList, QStringList labels)
{
    QFile fi(fileName);
    if (!fi.open(QIODevice::WriteOnly)) {
        emit sigResult(false);
        emit sigError(openErroStr);
        return false;
    }
    try {
        QTextStream out(&fi);
        out.setCodec(QTextCodec::codecForName("utf-8"));
        for (int i = 0; i < jList.count(); i++) {
            if (!m_canRunning) {
                throw  QString(stopStr);
            }
            LOG_MSG_BOOT jMsg = jList.at(i);
            int col = 0;
            out << labels.value(col++, "") << ":" << jMsg.status << " ";
            out << labels.value(col++, "") << ":" << jMsg.msg << " ";
            out << "\n";
            sigProgress(i + 1, jList.count());
        }
    } catch (QString ErrorStr) {
        qDebug() << "Export Stop" << ErrorStr;
        fi.close();
        emit sigResult(false);
        if (ErrorStr != stopStr) {
            emit sigError(QString("export error: %1").arg(ErrorStr));
        }
        return false;
    }
    fi.close();
    emit sigResult(m_canRunning);
    return true && m_canRunning;
}

bool LogExportThread::exportToTxt(QString fileName, QList<LOG_MSG_XORG> jList, QStringList labels)
{
    QFile fi(fileName);
    if (!fi.open(QIODevice::WriteOnly)) {
        emit sigResult(false);
        emit sigError(openErroStr);
        return false;
    }
    try {
        QTextStream out(&fi);
        out.setCodec(QTextCodec::codecForName("utf-8"));
        for (int i = 0; i < jList.count(); i++) {
            if (!m_canRunning) {
                throw  QString(stopStr);
            }
            LOG_MSG_XORG jMsg = jList.at(i);
            int col = 0;
            out << labels.value(col++, "") << ":" << jMsg.dateTime << " ";
            out << labels.value(col++, "") << ":" << jMsg.msg << " ";
            out << "\n";
            sigProgress(i + 1, jList.count());
        }
    } catch (QString ErrorStr) {
        qDebug() << "Export Stop" << ErrorStr;
        fi.close();
        emit sigResult(false);
        if (ErrorStr != stopStr) {
            emit sigError(QString("export error: %1").arg(ErrorStr));
        }
        return false;
    }
    fi.close();
    emit sigResult(m_canRunning);
    return true && m_canRunning;
}

bool LogExportThread::exportToTxt(QString fileName, QList<LOG_MSG_NORMAL> jList, QStringList labels)
{
    QFile fi(fileName);
    if (!fi.open(QIODevice::WriteOnly)) {
        emit sigResult(false);
        emit sigError(openErroStr);
        return false;
    }
    try {
        QTextStream out(&fi);
        out.setCodec(QTextCodec::codecForName("utf-8"));
        for (int i = 0; i < jList.count(); i++) {
            if (!m_canRunning) {
                throw  QString(stopStr);
            }
            LOG_MSG_NORMAL jMsg = jList.at(i);
            int col = 0;
            out << labels.value(col++, "") << ":" << jMsg.eventType << " ";
            out << labels.value(col++, "") << ":" << jMsg.userName << " ";
            out << labels.value(col++, "") << ":" << jMsg.dateTime << " ";
            out << labels.value(col++, "") << ":" << jMsg.msg << " ";
            out << "\n";
            sigProgress(i + 1, jList.count());
        }
    } catch (QString ErrorStr) {
        qDebug() << "Export Stop" << ErrorStr;
        fi.close();
        emit sigResult(false);
        if (ErrorStr != stopStr) {
            emit sigError(QString("export error: %1").arg(ErrorStr));
        }
        return false;
    }
    fi.close();
    emit sigResult(m_canRunning);
    return true && m_canRunning;
}

bool LogExportThread::exportToTxt(QString fileName, QList<LOG_MSG_KWIN> jList, QStringList labels)
{
    QFile fi(fileName);
    if (!fi.open(QIODevice::WriteOnly)) {
        emit sigResult(false);
        emit sigError(openErroStr);
        return false;
    }
    try {
        QTextStream out(&fi);
        out.setCodec(QTextCodec::codecForName("utf-8"));
        for (int i = 0; i < jList.count(); i++) {
            if (!m_canRunning) {
                throw  QString(stopStr);
            }
            LOG_MSG_KWIN jMsg = jList.at(i);
            int col = 0;
            out << labels.value(col++, "") << ":" << jMsg.msg << " ";
            out << "\n";
            sigProgress(i + 1, jList.count());
        }
    } catch (QString ErrorStr) {
        qDebug() << "Export Stop" << ErrorStr;
        fi.close();
        emit sigResult(false);
        if (ErrorStr != stopStr) {
            emit sigError(QString("export error: %1").arg(ErrorStr));
        }
        return false;
    }
    fi.close();
    emit sigResult(m_canRunning);
    return true && m_canRunning;
}


bool LogExportThread::exportToDoc(QString fileName, QStandardItemModel *pModel, LOG_FLAG flag)
{
#if 1
    try {
        if (!pModel) {
            throw  QString("model is null");
        }
        Docx::Document doc(DOCTEMPLATE);
        Docx::Table *tab = doc.addTable(pModel->rowCount() + 1, pModel->columnCount());
        tab->setAlignment(Docx::WD_TABLE_ALIGNMENT::LEFT);

        for (int col = 0; col < pModel->columnCount(); ++col) {
            auto item = pModel->horizontalHeaderItem(col);
            auto cel = tab->cell(0, col);
            if (item) {
                cel->addText(pModel->horizontalHeaderItem(col)->text());
            }
        }
        int end = static_cast<int>(pModel->rowCount() * 0.1 > 5 ? pModel->rowCount() * 0.1 : 5);

        if (flag == APP) {
            for (int row = 0; row < pModel->rowCount(); ++row) {
                if (!m_canRunning) {
                    throw  QString(stopStr);
                }
                auto cel = tab->cell(row + 1, 0);
                cel->addText(pModel->item(row, 0)->data(Qt::UserRole + 6).toString());
                for (int col = 1; col < pModel->columnCount(); ++col) {
                    if (!m_canRunning) {
                        throw  QString(stopStr);
                    }
                    auto cel = tab->cell(row + 1, col);
                    cel->addText(pModel->item(row, col)->text());
                }
                sigProgress(row + 1, pModel->rowCount() + end);
            }
        } else {
            for (int row = 0; row < pModel->rowCount(); ++row) {
                if (!m_canRunning) {
                    throw  QString(stopStr);
                }
                for (int col = 0; col < pModel->columnCount(); ++col) {
                    if (!m_canRunning) {
                        throw  QString(stopStr);
                    }
                    auto cel = tab->cell(row + 1, col);
                    cel->addText(pModel->item(row, col)->text());
                }
                sigProgress(row + 1, pModel->rowCount() + end);
            }
        }

        doc.save(fileName);
    } catch (QString ErrorStr) {
        qDebug() << "Export Stop" << ErrorStr;
        if (!m_canRunning) {
            Utils::checkAndDeleteDir(m_fileName);
        }
        emit sigResult(false);
        if (ErrorStr != stopStr) {
            emit sigError(QString("export error: %1").arg(ErrorStr));
        }
        return false;
    }
#else
    QTextDocumentWriter writer(fileName);
    writer.setFormat("odf");
    QTextDocument *doc = new QTextDocument;
    QString html;
    html.append("<!DOCTYPE html>\n");
    html.append("<html>\n");
    html.append("<body>\n");
    html.append("<table border=\"1\">\n");
    // write title
    html.append("<tr>");
    for (int i = 0; i < pModel->columnCount(); ++i) {
        QString labelInfo = QString("<td>%1</td>").arg(pModel->horizontalHeaderItem(i)->text());
        html.append(labelInfo.toUtf8().data());
    }
    html.append("</tr>");
    // write contents
    for (int row = 0; row < pModel->rowCount(); ++row) {
        html.append("<tr>");
        for (int col = 0; col < pModel->columnCount(); ++col) {
            QString info = QString("<td>%1</td>").arg(pModel->item(row, col)->text());
            html.append(info.toUtf8().data());
        }
        html.append("</tr>");
    }
    html.append("</table>\n");
    html.append("</body>\n");
    html.append("</html>\n");
    doc->setHtml(html);
    writer.write(doc);
#endif
    if (!m_canRunning) {
        Utils::checkAndDeleteDir(m_fileName);
    }
    sigProgress(100, 100);
    Utils::sleep(200);
    emit sigResult(m_canRunning);
    return true && m_canRunning;
}

bool LogExportThread::exportToDoc(QString fileName, QList<LOG_MSG_JOURNAL> jList,
                                  QStringList labels, LOG_FLAG iFlag)
{
#if 1
    try {
        Docx::Document doc(DOCTEMPLATE);
        Docx::Table *tab;
        if (iFlag == JOURNAL) {
            tab = doc.addTable(jList.count() + 1, 6);
        } else if (iFlag == KERN) {
            tab = doc.addTable(jList.count() + 1, 4);
        }

        tab->setAlignment(Docx::WD_TABLE_ALIGNMENT::LEFT);

        for (int col = 0; col < labels.count(); ++col) {
            auto cel = tab->cell(0, col);
            cel->addText(labels.at(col));
        }
        int end = static_cast<int>(jList.count() * 0.1 > 5 ? jList.count() * 0.1 : 5);

        for (int row = 0; row < jList.count(); ++row) {
            if (!m_canRunning) {
                throw  QString(stopStr);
            }
            LOG_MSG_JOURNAL message = jList.at(row);
            int col = 0;
            if (iFlag == JOURNAL) {
                tab->cell(row + 1, col++)->addText(message.level);
                tab->cell(row + 1, col++)->addText(message.daemonName);
                tab->cell(row + 1, col++)->addText(message.dateTime);
                tab->cell(row + 1, col++)->addText(message.msg);
                tab->cell(row + 1, col++)->addText(message.hostName);
                tab->cell(row + 1, col++)->addText(message.daemonId);
            } else if (iFlag == KERN) {
                tab->cell(row + 1, col++)->addText(message.dateTime);
                tab->cell(row + 1, col++)->addText(message.hostName);
                tab->cell(row + 1, col++)->addText(message.daemonName);
                tab->cell(row + 1, col++)->addText(message.msg);
            }
            sigProgress(row + 1, jList.count() + end);
        }

        doc.save(fileName);
    } catch (QString ErrorStr) {
        qDebug() << "Export Stop" << ErrorStr;
        if (!m_canRunning) {
            Utils::checkAndDeleteDir(m_fileName);
        }

        emit sigResult(false);
        if (ErrorStr != stopStr) {
            emit sigError(QString("export error: %1").arg(ErrorStr));
        }
        return false;
    }
#else
    QTextDocumentWriter writer(fileName);
    writer.setFormat("odf");
    QTextDocument *doc = new QTextDocument;
    QString html;
    html.append("<!DOCTYPE html>\n");
    html.append("<html>\n");
    html.append("<body>\n");
    html.append("<table border=\"1\">\n");
    QString title = QString(
                        "<tr><td>时间</td><td>主机名</td><td>守护进程</td><td>进程ID</td><td>级别</td><td>消息</"
                        "td></tr>");
    html.append(title.toUtf8().data());
    for (int i = 0; i < jList.count(); i++) {
        LOG_MSG_JOURNAL jMsg = jList.at(i);
        QString info =
            QString("<tr><td>%1</td><td>%2</td><td>%3</td><td>%4</td><td>%5</td><td>%6</td></tr>")
            .arg(jMsg.dateTime)
            .arg(jMsg.hostName)
            .arg(jMsg.daemonName)
            .arg(jMsg.daemonId)
            .arg(jMsg.level)
            .arg(jMsg.msg);
        html.append(info.toUtf8().data());
    }

    html.append("</table>\n");
    html.append("</body>\n");
    html.append("</html>\n");
    doc->setHtml(html);
    writer.write(doc);
#endif
    if (!m_canRunning) {
        Utils::checkAndDeleteDir(m_fileName);
    }
    sigProgress(100, 100);
    Utils::sleep(200);
    emit sigResult(m_canRunning);
    return true && m_canRunning;
}

bool LogExportThread::exportToDoc(QString fileName, QList<LOG_MSG_APPLICATOIN> jList, QStringList labels, QString &iAppName)
{
    QElapsedTimer timer;
    timer.start();
    try {
        Docx::Document doc(DOCTEMPLATE);
        Docx::Table *tab = doc.addTable(jList.count() + 1, 4);
        tab->setAlignment(Docx::WD_TABLE_ALIGNMENT::LEFT);

        for (int col = 0; col < labels.count(); ++col) {
            auto cel = tab->cell(0, col);
            cel->addText(labels.at(col));
        }
        int end = static_cast<int>(jList.count() * 0.1 > 5 ? jList.count() * 0.1 : 5);
        for (int row = 0; row < jList.count(); ++row) {
            if (!m_canRunning) {
                throw  QString(stopStr);
            }
            LOG_MSG_APPLICATOIN message = jList.at(row);
            int col = 0;
            tab->cell(row + 1, col++)->addText(strTranslate(message.level));
            tab->cell(row + 1, col++)->addText(message.dateTime);
            tab->cell(row + 1, col++)->addText(iAppName);
            tab->cell(row + 1, col++)->addText(message.msg);
            sigProgress(row + 1, jList.count() + end);
        }


        doc.save(fileName);

    } catch (QString ErrorStr) {
        if (!m_canRunning) {
            Utils::checkAndDeleteDir(m_fileName);
        }

        qDebug() << "Export Stop" << ErrorStr;
        emit sigResult(false);
        if (ErrorStr != stopStr) {
            emit sigError(QString("export error: %1").arg(ErrorStr));
        }

        return false;
    }

    if (!m_canRunning) {
        Utils::checkAndDeleteDir(m_fileName);
    }

    sigProgress(100, 100);
    Utils::sleep(200);
    emit sigResult(m_canRunning);

    qDebug() << "exportdoc benchmark rows:" << jList.count() << "time:" << timer.elapsed();
    return true && m_canRunning;
}

bool LogExportThread::exportToDocNew(QString fileName, QList<LOG_MSG_APPLICATOIN> jList, QStringList labels, QString &iAppName)
{
    QElapsedTimer timer;
    timer.start();
    try {
        DocxFactory::WordProcessingCompiler &l_compiler =
            DocxFactory::WordProcessingCompiler::getInstance();
        l_compiler.compile(
            "/home/zyc/Documents/work_space/works/oldlog/dde_log_viewer/3rdparty/DocxFactory/exercises/templates/test_app.docx",
            "/home/zyc/Documents/work_space/works/oldlog/dde_log_viewer/3rdparty/DocxFactory/exercises/templates/test_app.dfw");



        DocxFactory:: WordProcessingMerger &l_merger =
            DocxFactory:: WordProcessingMerger::getInstance();
        l_merger.load(
            "/home/zyc/Documents/work_space/works/oldlog/dde_log_viewer/3rdparty/DocxFactory/exercises/templates/test_app.dfw");
        for (int col = 0; col < labels.count(); ++col) {
            l_merger.setClipboardValue("tableRow", QString("column%1").arg(col + 1).toStdString(), labels.at(col).toStdString());
        }

        l_merger.paste("tableRow");
        for (int row = 0; row < 1000000; ++row)  {
            if (!m_canRunning) {
                throw  QString(stopStr);
            }
            LOG_MSG_APPLICATOIN message = jList.at(100);
            l_merger.setClipboardValue("tableRow", QString("column1").toStdString(), strTranslate(message.level).toStdString());
            l_merger.setClipboardValue("tableRow", QString("column2").toStdString(), message.dateTime.toStdString());
            l_merger.setClipboardValue("tableRow", QString("column3").toStdString(), iAppName.toStdString());
            l_merger.setClipboardValue("tableRow", QString("column4").toStdString(), message.msg.toStdString());
            l_merger.paste("tableRow");
            sigProgress(row + 1, jList.count());
        }


        fileName = fileName + "x";
        l_merger.save(fileName.toStdString());



        sigProgress(100, 100);
        emit sigResult(m_canRunning);
        qDebug() << "exportdoc New benchmark rows:" << jList.count() << "time:" << timer.elapsed();
        return  true;
    } catch (const std::exception &p_exception) {
        qDebug() << p_exception.what() ;
        emit sigResult(false);
        return  false;
    }


}

bool LogExportThread::exportToDoc(QString fileName, QList<LOG_MSG_DPKG> jList, QStringList labels)
{
    try {
        Docx::Document doc(DOCTEMPLATE);
        Docx::Table *tab = doc.addTable(jList.count() + 1, 3);
        tab->setAlignment(Docx::WD_TABLE_ALIGNMENT::LEFT);

        for (int col = 0; col < labels.count(); ++col) {
            auto cel = tab->cell(0, col);
            cel->addText(labels.at(col));
        }
        int end = static_cast<int>(jList.count() * 0.1 > 5 ? jList.count() * 0.1 : 5);

        for (int row = 0; row < jList.count(); ++row) {
            if (!m_canRunning) {
                throw  QString(stopStr);
            }
            LOG_MSG_DPKG message = jList.at(row);
            int col = 0;
            tab->cell(row + 1, col++)->addText(message.dateTime);
            tab->cell(row + 1, col++)->addText(message.msg);
            tab->cell(row + 1, col++)->addText(message.action);
            sigProgress(row + 1, jList.count() + end);
        }


        doc.save(fileName);

    } catch (QString ErrorStr) {
        qDebug() << "Export Stop" << ErrorStr;
        if (!m_canRunning) {
            Utils::checkAndDeleteDir(m_fileName);
        }

        emit sigResult(false);
        if (ErrorStr != stopStr) {
            emit sigError(QString("export error: %1").arg(ErrorStr));
        }
        return false;
    }
    if (!m_canRunning) {
        Utils::checkAndDeleteDir(m_fileName);
    }
    sigProgress(100, 100);
    Utils::sleep(200);
    emit sigResult(m_canRunning);
    return true && m_canRunning;
}

bool LogExportThread::exportToDoc(QString fileName, QList<LOG_MSG_BOOT> jList, QStringList labels)
{
    try {
        Docx::Document doc(DOCTEMPLATE);
        Docx::Table *tab = doc.addTable(jList.count() + 1, 2);
        tab->setAlignment(Docx::WD_TABLE_ALIGNMENT::LEFT);
        int end = static_cast<int>(jList.count() * 0.1 > 5 ? jList.count() * 0.1 : 5);

        for (int col = 0; col < labels.count(); ++col) {
            auto cel = tab->cell(0, col);
            cel->addText(labels.at(col));
        }

        for (int row = 0; row < jList.count(); ++row) {
            if (!m_canRunning) {
                throw  QString(stopStr);
            }
            LOG_MSG_BOOT message = jList.at(row);
            int col = 0;
            tab->cell(row + 1, col++)->addText(message.status);
            tab->cell(row + 1, col++)->addText(message.msg);
            sigProgress(row + 1, jList.count() + end);
        }
        doc.save(fileName);

    } catch (QString ErrorStr) {
        qDebug() << "Export Stop" << ErrorStr;
        if (!m_canRunning) {
            Utils::checkAndDeleteDir(m_fileName);
        }

        emit sigResult(false);
        if (ErrorStr != stopStr) {
            emit sigError(QString("export error: %1").arg(ErrorStr));
        }
        return false;
    }
    if (!m_canRunning) {
        Utils::checkAndDeleteDir(m_fileName);
    }
    sigProgress(100, 100);
    Utils::sleep(200);
    emit sigResult(m_canRunning);
    return true && m_canRunning;
}

bool LogExportThread::exportToDoc(QString fileName, QList<LOG_MSG_XORG> jList, QStringList labels)
{
    try {
        Docx::Document doc(DOCTEMPLATE);
        Docx::Table *tab = doc.addTable(jList.count() + 1, 2);
        tab->setAlignment(Docx::WD_TABLE_ALIGNMENT::LEFT);

        for (int col = 0; col < labels.count(); ++col) {
            auto cel = tab->cell(0, col);
            cel->addText(labels.at(col));
        }
        int end = static_cast<int>(jList.count() * 0.1 > 5 ? jList.count() * 0.1 : 5);
        for (int row = 0; row < jList.count(); ++row) {
            if (!m_canRunning) {
                throw  QString(stopStr);
            }
            LOG_MSG_XORG message = jList.at(row);
            int col = 0;
            tab->cell(row + 1, col++)->addText(message.dateTime);
            tab->cell(row + 1, col++)->addText(message.msg);
            sigProgress(row + 1, jList.count() + end);
        }


        doc.save(fileName);

    } catch (QString ErrorStr) {
        qDebug() << "Export Stop" << ErrorStr;
        if (!m_canRunning) {
            Utils::checkAndDeleteDir(m_fileName);
        }

        emit sigResult(false);
        if (ErrorStr != stopStr) {
            emit sigError(QString("export error: %1").arg(ErrorStr));
        }
        return false;
    }
    if (!m_canRunning) {
        Utils::checkAndDeleteDir(m_fileName);
    }
    sigProgress(100, 100);
    Utils::sleep(200);
    emit sigResult(m_canRunning);

    return true && m_canRunning;
}

bool LogExportThread::exportToDoc(QString fileName, QList<LOG_MSG_NORMAL> jList, QStringList labels)
{
    try {
        Docx::Document doc(DOCTEMPLATE);
        Docx::Table *tab = doc.addTable(jList.count() + 1, 4);
        tab->setAlignment(Docx::WD_TABLE_ALIGNMENT::LEFT);

        for (int col = 0; col < labels.count(); ++col) {
            auto cel = tab->cell(0, col);
            cel->addText(labels.at(col));
        }
        int end = static_cast<int>(jList.count() * 0.1 > 5 ? jList.count() * 0.1 : 5);
        for (int row = 0; row < jList.count(); ++row) {
            if (!m_canRunning) {
                throw  QString(stopStr);
            }
            LOG_MSG_NORMAL message = jList.at(row);
            int col = 0;
            tab->cell(row + 1, col++)->addText(message.eventType);
            tab->cell(row + 1, col++)->addText(message.userName);
            tab->cell(row + 1, col++)->addText(message.dateTime);
            tab->cell(row + 1, col++)->addText(message.msg);
            sigProgress(row + 1, jList.count() + end);
        }


        doc.save(fileName);

    } catch (QString ErrorStr) {
        qDebug() << "Export Stop" << ErrorStr;
        if (!m_canRunning) {
            Utils::checkAndDeleteDir(m_fileName);
        }

        emit sigResult(false);
        if (ErrorStr != stopStr) {
            emit sigError(QString("export error: %1").arg(ErrorStr));
        }
        return false;
    }
    if (!m_canRunning) {
        Utils::checkAndDeleteDir(m_fileName);
    }
    sigProgress(100, 100);
    Utils::sleep(200);
    emit sigResult(m_canRunning);
    return true && m_canRunning;
}

bool LogExportThread::exportToDoc(QString fileName, QList<LOG_MSG_KWIN> jList, QStringList labels)
{
    try {
        Docx::Document doc(DOCTEMPLATE);
        Docx::Table *tab = doc.addTable(jList.count() + 1, 1);
        tab->setAlignment(Docx::WD_TABLE_ALIGNMENT::LEFT);

        for (int col = 0; col < labels.count(); ++col) {
            auto cel = tab->cell(0, col);
            cel->addText(labels.at(col));
        }
        int end = static_cast<int>(jList.count() * 0.1 > 5 ? jList.count() * 0.1 : 5);
        for (int row = 0; row < jList.count(); ++row) {
            if (!m_canRunning) {
                throw  QString(stopStr);
            }
            LOG_MSG_KWIN message = jList.at(row);
            int col = 0;
            tab->cell(row + 1, col++)->addText(message.msg);
            sigProgress(row + 1, jList.count() + end);
        }


        doc.save(fileName);

    } catch (QString ErrorStr) {
        qDebug() << "Export Stop" << ErrorStr;
        if (!m_canRunning) {
            Utils::checkAndDeleteDir(m_fileName);
        }

        emit sigResult(false);
        if (ErrorStr != stopStr) {
            emit sigError(QString("export error: %1").arg(ErrorStr));
        }
        return false;
    }
    if (!m_canRunning) {
        Utils::checkAndDeleteDir(m_fileName);
    }
    sigProgress(100, 100);
    Utils::sleep(200);
    emit sigResult(m_canRunning);
    return true && m_canRunning;
}





bool LogExportThread::exportToHtml(QString fileName, QStandardItemModel *pModel, LOG_FLAG flag)
{
    QFile html(fileName);
    if (!html.open(QIODevice::WriteOnly)) {
        emit sigResult(false);
        emit sigError(openErroStr);
        return false;
    }
    try {
        if (!pModel) {
            throw  QString("model is null");
        }
        html.write("<!DOCTYPE html>\n");
        html.write("<html>\n");
        html.write("<body>\n");
        html.write("<table border=\"1\">\n");
        // write title
        html.write("<tr>");
        for (int i = 0; i < pModel->columnCount(); ++i) {
            QString labelInfo = QString("<td>%1</td>").arg(pModel->horizontalHeaderItem(i)->text());
            html.write(labelInfo.toUtf8().data());
        }
        html.write("</tr>");
        // write contents
        if (flag == APP) {
            for (int row = 0; row < pModel->rowCount(); ++row) {
                if (!m_canRunning) {
                    throw  QString(stopStr);
                }
                html.write("<tr>");

                QString info =
                    QString("<td>%1</td>").arg(pModel->item(row, 0)->data(Qt::UserRole + 6).toString());
                html.write(info.toUtf8().data());

                for (int col = 1; col < pModel->columnCount(); ++col) {
                    QString info = QString("<td>%1</td>").arg(pModel->item(row, col)->text());
                    html.write(info.toUtf8().data());
                }
                html.write("</tr>");
                sigProgress(row + 1, pModel->rowCount());
            }
        } else {
            for (int row = 0; row < pModel->rowCount(); ++row) {
                if (!m_canRunning) {
                    throw  QString(stopStr);
                }
                html.write("<tr>");
                for (int col = 0; col < pModel->columnCount(); ++col) {
                    QString info = QString("<td>%1</td>").arg(pModel->item(row, col)->text());
                    html.write(info.toUtf8().data());
                }
                html.write("</tr>");
                sigProgress(row + 1, pModel->rowCount());
            }
        }
        html.write("</table>\n");
        html.write("</body>\n");
        html.write("</html>\n");
    } catch (QString ErrorStr) {
        qDebug() << "Export Stop" << ErrorStr;
        html.close();
        emit sigResult(false);
        if (ErrorStr != stopStr) {
            emit sigError(QString("export error: %1").arg(ErrorStr));
        }
        return false;
    }
    html.close();
    emit sigResult(m_canRunning);
    return true && m_canRunning;
}

bool LogExportThread::exportToHtml(QString fileName, QList<LOG_MSG_JOURNAL> jList,  QStringList labels, LOG_FLAG flag)
{
    QFile html(fileName);
    if (!html.open(QIODevice::WriteOnly)) {
        emit sigResult(false);
        emit sigError(openErroStr);
        return false;
    }
    try {

        html.write("<!DOCTYPE html>\n");
        html.write("<html>\n");
        html.write("<body>\n");
        html.write("<table border=\"1\">\n");
        if (flag == JOURNAL) {
            //    QString title = QString(
            //        "<tr><td>时间</td><td>主机名</td><td>守护进程</td><td>进程ID</td><td>级别</td><td>消息</"
            //        "td></tr>");
            //    QString title = QString("<tr><td>") +
            //                    QString(DApplication::translate("Table", "Date and Time")) +
            //                    QString("</td><td>") + QString(DApplication::translate("Table", "User")) +
            //                    QString("</td><td>") + QString(DApplication::translate("Table", "Daemon"))
            //                    + QString("</td><td>") + QString(DApplication::translate("Table", "PID"))
            //                    + QString("</td><td>") + QString(DApplication::translate("Table",
            //                    "Level")) + QString("</td><td>") +
            //                    QString(DApplication::translate("Table", "Info")) + QString("</td></tr>");
            //                    //delete  for bug
            QString title = QString("<tr><td>") + QString(DApplication::translate("Table", "Level")) +
                            QString("</td><td>") + QString(DApplication::translate("Table", "Process")) +
                            QString("</td><td>") +
                            QString(DApplication::translate("Table", "Date and Time")) +
                            QString("</td><td>") + QString(DApplication::translate("Table", "Info")) +
                            QString("</td><td>") + QString(DApplication::translate("Table", "User")) +
                            QString("</td><td>") + QString(DApplication::translate("Table", "PID")) +
                            QString("</td></tr>");
            html.write(title.toUtf8().data());
            for (int i = 0; i < jList.count(); i++) {
                if (!m_canRunning) {
                    throw  QString(stopStr);
                }
                LOG_MSG_JOURNAL jMsg = jList.at(i);
                //        QString info =
                //            QString("<tr><td>%1</td><td>%2</td><td>%3</td><td>%4</td><td>%5</td><td>%6</td></tr>")
                //                .arg(jMsg.dateTime)
                //                .arg(jMsg.hostName)
                //                .arg(jMsg.daemonName)
                //                .arg(jMsg.daemonId)
                //                .arg(jMsg.level)
                //                .arg(jMsg.msg); //delete for bug
                QString info =
                    QString("<tr><td>%1</td><td>%2</td><td>%3</td><td>%4</td><td>%5</td><td>%6</td></tr>")
                    .arg(jMsg.level)
                    .arg(jMsg.daemonName)
                    .arg(jMsg.dateTime)
                    .arg(jMsg.msg)
                    .arg(jMsg.hostName)
                    .arg(jMsg.daemonId);
                html.write(info.toUtf8().data());
                sigProgress(i + 1, jList.count());

            }
        } else if (flag == KERN) {
            html.write("<tr>");
            for (int i = 0; i < labels.count(); ++i) {
                QString labelInfo = QString("<td>%1</td>").arg(labels.value(i));
                html.write(labelInfo.toUtf8().data());
            }
            html.write("</tr>");
            for (int row = 0; row < jList.count(); ++row) {
                if (!m_canRunning) {
                    throw  QString(stopStr);
                }
                LOG_MSG_JOURNAL jMsg = jList.at(row);
                html.write("<tr>");
                QString info = QString("<td>%1</td>").arg(jMsg.dateTime);
                html.write(info.toUtf8().data());
                info = QString("<td>%1</td>").arg(jMsg.hostName);
                html.write(info.toUtf8().data());
                info = QString("<td>%1</td>").arg(jMsg.daemonName);
                html.write(info.toUtf8().data());
                info = QString("<td>%1</td>").arg(jMsg.msg);
                html.write(info.toUtf8().data());
                html.write("</tr>");
                sigProgress(row + 1, jList.count());
            }

        }

        html.write("</table>\n");
        html.write("</body>\n");
        html.write("</html>\n");
    } catch (QString ErrorStr) {
        qDebug() << "Export Stop" << ErrorStr;
        html.close();
        emit sigResult(false);
        if (ErrorStr != stopStr) {
            emit sigError(QString("export error: %1").arg(ErrorStr));
        }
        return false;
    }
    html.close();
    emit sigResult(m_canRunning);
    return true && m_canRunning;
}

bool LogExportThread::exportToHtml(QString fileName, QList<LOG_MSG_APPLICATOIN> jList, QStringList labels, QString &iAppName)
{
    QFile html(fileName);
    if (!html.open(QIODevice::WriteOnly)) {
        emit sigResult(false);
        emit sigError(openErroStr);
        return false;
    }
    try {
        html.write("<!DOCTYPE html>\n");
        html.write("<html>\n");
        html.write("<body>\n");
        html.write("<table border=\"1\">\n");
        html.write("<tr>");
        for (int i = 0; i < labels.count(); ++i) {
            QString labelInfo = QString("<td>%1</td>").arg(labels.value(i));
            html.write(labelInfo.toUtf8().data());
        }
        html.write("</tr>");
        for (int row = 0; row < jList.count(); ++row) {
            if (!m_canRunning) {
                throw  QString(stopStr);
            }
            LOG_MSG_APPLICATOIN jMsg = jList.at(row);
            html.write("<tr>");
            QString info = QString("<td>%1</td>").arg(strTranslate(jMsg.level));
            html.write(info.toUtf8().data());
            info = QString("<td>%1</td>").arg(jMsg.dateTime);
            html.write(info.toUtf8().data());
            info = QString("<td>%1</td>").arg(iAppName);
            html.write(info.toUtf8().data());
            info = QString("<td>%1</td>").arg(jMsg.msg);
            html.write(info.toUtf8().data());
            html.write("</tr>");
            sigProgress(row + 1, jList.count());
        }

        html.write("</table>\n");
        html.write("</body>\n");
        html.write("</html>\n");
    } catch (QString ErrorStr) {
        qDebug() << "Export Stop" << ErrorStr;
        html.close();
        emit sigResult(false);
        if (ErrorStr != stopStr) {
            emit sigError(QString("export error: %1").arg(ErrorStr));
        }
        return false;
    }
    html.close();
    emit sigResult(m_canRunning);
    return true && m_canRunning;
}

bool LogExportThread::exportToHtml(QString fileName, QList<LOG_MSG_DPKG> jList, QStringList labels)
{
    QFile html(fileName);
    if (!html.open(QIODevice::WriteOnly)) {
        emit sigResult(false);
        emit sigError(openErroStr);
        return false;
    }
    try {
        html.write("<!DOCTYPE html>\n");
        html.write("<html>\n");
        html.write("<body>\n");
        html.write("<table border=\"1\">\n");
        html.write("<tr>");
        for (int i = 0; i < labels.count(); ++i) {
            QString labelInfo = QString("<td>%1</td>").arg(labels.value(i));
            html.write(labelInfo.toUtf8().data());
        }
        html.write("</tr>");
        for (int row = 0; row < jList.count(); ++row) {
            if (!m_canRunning) {
                throw  QString(stopStr);
            }
            LOG_MSG_DPKG jMsg = jList.at(row);
            html.write("<tr>");
            QString info = QString("<td>%1</td>").arg(jMsg.dateTime);
            html.write(info.toUtf8().data());
            info = QString("<td>%1</td>").arg(jMsg.msg);
            html.write(info.toUtf8().data());
            info = QString("<td>%1</td>").arg(jMsg.action);
            html.write(info.toUtf8().data());
            html.write("</tr>");
            sigProgress(row + 1, jList.count());
        }

        html.write("</table>\n");
        html.write("</body>\n");
        html.write("</html>\n");
    } catch (QString ErrorStr) {
        qDebug() << "Export Stop" << ErrorStr;
        html.close();
        emit sigResult(false);
        if (ErrorStr != stopStr) {
            emit sigError(QString("export error: %1").arg(ErrorStr));
        }
        return false;
    }
    html.close();
    emit sigResult(m_canRunning);
    return true && m_canRunning;
}

bool LogExportThread::exportToHtml(QString fileName, QList<LOG_MSG_BOOT> jList, QStringList labels)
{
    QFile html(fileName);
    if (!html.open(QIODevice::WriteOnly)) {
        emit sigResult(false);
        emit sigError(openErroStr);
        return false;
    }
    try {
        html.write("<!DOCTYPE html>\n");
        html.write("<html>\n");
        html.write("<body>\n");
        html.write("<table border=\"1\">\n");
        html.write("<tr>");
        for (int i = 0; i < labels.count(); ++i) {
            QString labelInfo = QString("<td>%1</td>").arg(labels.value(i));
            html.write(labelInfo.toUtf8().data());
        }
        html.write("</tr>");
        for (int row = 0; row < jList.count(); ++row) {
            if (!m_canRunning) {
                throw  QString(stopStr);
            }
            LOG_MSG_BOOT jMsg = jList.at(row);
            html.write("<tr>");
            QString info = QString("<td>%1</td>").arg(jMsg.status);
            html.write(info.toUtf8().data());
            info = QString("<td>%1</td>").arg(jMsg.msg);
            html.write(info.toUtf8().data());
            html.write("</tr>");
            sigProgress(row + 1, jList.count());
        }

        html.write("</table>\n");
        html.write("</body>\n");
        html.write("</html>\n");
    } catch (QString ErrorStr) {
        qDebug() << "Export Stop" << ErrorStr;
        html.close();
        emit sigResult(false);
        if (ErrorStr != stopStr) {
            emit sigError(QString("export error: %1").arg(ErrorStr));
        }
        return false;
    }
    html.close();
    emit sigResult(m_canRunning);
    return true && m_canRunning;
}

bool LogExportThread::exportToHtml(QString fileName, QList<LOG_MSG_XORG> jList, QStringList labels)
{
    QFile html(fileName);
    if (!html.open(QIODevice::WriteOnly)) {
        emit sigResult(false);
        emit sigError(openErroStr);
        return false;
    }
    try {
        html.write("<!DOCTYPE html>\n");
        html.write("<html>\n");
        html.write("<body>\n");
        html.write("<table border=\"1\">\n");
        html.write("<tr>");
        for (int i = 0; i < labels.count(); ++i) {
            QString labelInfo = QString("<td>%1</td>").arg(labels.value(i));
            html.write(labelInfo.toUtf8().data());
        }
        html.write("</tr>");
        for (int row = 0; row < jList.count(); ++row) {
            if (!m_canRunning) {
                throw  QString(stopStr);
            }
            LOG_MSG_XORG jMsg = jList.at(row);
            html.write("<tr>");
            QString info = QString("<td>%1</td>").arg(jMsg.dateTime);
            html.write(info.toUtf8().data());
            info = QString("<td>%1</td>").arg(jMsg.msg);
            html.write(info.toUtf8().data());
            html.write("</tr>");
            sigProgress(row + 1, jList.count());
        }

        html.write("</table>\n");
        html.write("</body>\n");
        html.write("</html>\n");
    } catch (QString ErrorStr) {
        qDebug() << "Export Stop" << ErrorStr;
        html.close();
        emit sigResult(false);
        if (ErrorStr != stopStr) {
            emit sigError(QString("export error: %1").arg(ErrorStr));
        }
        return false;
    }
    html.close();
    emit sigResult(m_canRunning);
    return true && m_canRunning;
}

bool LogExportThread::exportToHtml(QString fileName, QList<LOG_MSG_NORMAL> jList, QStringList labels)
{
    QFile html(fileName);
    if (!html.open(QIODevice::WriteOnly)) {
        emit sigResult(false);
        emit sigError(openErroStr);
        return false;
    }
    try {
        html.write("<!DOCTYPE html>\n");
        html.write("<html>\n");
        html.write("<body>\n");
        html.write("<table border=\"1\">\n");
        html.write("<tr>");
        for (int i = 0; i < labels.count(); ++i) {
            QString labelInfo = QString("<td>%1</td>").arg(labels.value(i));
            html.write(labelInfo.toUtf8().data());
        }
        html.write("</tr>");
        for (int row = 0; row < jList.count(); ++row) {
            if (!m_canRunning) {
                throw  QString(stopStr);
            }
            LOG_MSG_NORMAL jMsg = jList.at(row);
            html.write("<tr>");
            QString info = QString("<td>%1</td>").arg(jMsg.eventType);
            html.write(info.toUtf8().data());
            info = QString("<td>%1</td>").arg(jMsg.userName);
            html.write(info.toUtf8().data());
            info = QString("<td>%1</td>").arg(jMsg.dateTime);
            html.write(info.toUtf8().data());
            info = QString("<td>%1</td>").arg(jMsg.msg);
            html.write(info.toUtf8().data());
            html.write("</tr>");
            sigProgress(row + 1, jList.count());
        }

        html.write("</table>\n");
        html.write("</body>\n");
        html.write("</html>\n");
    } catch (QString ErrorStr) {
        qDebug() << "Export Stop" << ErrorStr;
        html.close();
        emit sigResult(false);
        if (ErrorStr != stopStr) {
            emit sigError(QString("export error: %1").arg(ErrorStr));
        }
        return false;
    }
    html.close();
    emit sigResult(m_canRunning);
    return true && m_canRunning;
}

bool LogExportThread::exportToHtml(QString fileName, QList<LOG_MSG_KWIN> jList, QStringList labels)
{
    QFile html(fileName);
    if (!html.open(QIODevice::WriteOnly)) {
        emit sigResult(false);
        emit sigError(openErroStr);
        return false;
    }
    try {
        html.write("<!DOCTYPE html>\n");
        html.write("<html>\n");
        html.write("<body>\n");
        html.write("<table border=\"1\">\n");
        html.write("<tr>");
        for (int i = 0; i < labels.count(); ++i) {
            QString labelInfo = QString("<td>%1</td>").arg(labels.value(i));
            html.write(labelInfo.toUtf8().data());
        }
        html.write("</tr>");
        for (int row = 0; row < jList.count(); ++row) {
            if (!m_canRunning) {
                throw  QString(stopStr);
            }
            LOG_MSG_KWIN jMsg = jList.at(row);
            html.write("<tr>");
            QString info = QString("<td>%1</td>").arg(jMsg.msg);
            html.write(info.toUtf8().data());
            html.write("</tr>");
            sigProgress(row + 1, jList.count());
        }

        html.write("</table>\n");
        html.write("</body>\n");
        html.write("</html>\n");
    } catch (QString ErrorStr) {
        qDebug() << "Export Stop" << ErrorStr;
        html.close();
        emit sigResult(false);
        if (ErrorStr != stopStr) {
            emit sigError(QString("export error: %1").arg(ErrorStr));
        }
        return false;
    }
    html.close();
    emit sigResult(m_canRunning);
    return true && m_canRunning;
}



bool LogExportThread::exportToXls(QString fileName, QStandardItemModel *pModel, LOG_FLAG flag)
{
    try {
        if (!pModel) {
            throw  QString("model is null");
        }
        auto currentXlsRow = 1;
        QXlsx::Document xlsx(&m_canRunning);
        connect(&xlsx, &QXlsx::Document::sigProcessAbstractSheet, this, [ = ](int iCurrent, int iTotal) {
            int end = static_cast<int>(iTotal * 0.1 > 5 ? iTotal * 0.1 : 5);
            sigProgress(iCurrent + iTotal, iTotal * 3 + end);
        });
        connect(&xlsx, &QXlsx::Document::sigProcessharedStrings, this, [ = ](int iCurrent, int iTotal) {
            int end = static_cast<int>(iTotal * 0.1 > 5 ? iTotal * 0.1 : 5);
            sigProgress(iCurrent + iTotal * 2, iTotal * 3 + end);
        });
        for (int col = 0; col < pModel->columnCount(); ++col) {
            auto item = pModel->horizontalHeaderItem(col);
            if (item) {
                QXlsx::Format boldFont;
                boldFont.setFontBold(true);
                xlsx.write(currentXlsRow, col + 1, item->text(), boldFont);
            }
        }
        ++currentXlsRow;
        int end = static_cast<int>(pModel->rowCount() * 0.1 > 5 ? pModel->rowCount() * 0.1 : 5);

        if (flag == APP) {
            for (int row = 0; row < pModel->rowCount(); ++row) {
                if (!m_canRunning) {
                    throw  QString(stopStr);
                }
                xlsx.write(currentXlsRow, 0 + 1,
                           pModel->item(row, 0)->data(Qt::UserRole + 6).toString());
                for (int col = 1; col < pModel->columnCount(); ++col) {
                    if (!m_canRunning) {
                        throw  QString(stopStr);
                    }
                    xlsx.write(currentXlsRow, col + 1, pModel->item(row, col)->text());
                }
                ++currentXlsRow;

                sigProgress(row + 1, pModel->rowCount() * 3 + end);
            }
        } else {
            for (int row = 0; row < pModel->rowCount(); ++row) {
                if (!m_canRunning) {
                    throw  QString(stopStr);
                }
                for (int col = 0; col < pModel->columnCount(); ++col) {
                    if (!m_canRunning) {
                        throw  QString(stopStr);
                    }
                    xlsx.write(currentXlsRow, col + 1, pModel->item(row, col)->text());
                }
                ++currentXlsRow;

                sigProgress(row + 1, pModel->rowCount() * 3 + end);

            }
        }

        ++currentXlsRow;
        if (!xlsx.saveAs(fileName)) {
            throw  QString(stopStr);
        }
        sigProgress(100, 100);
    } catch (QString ErrorStr) {
        qDebug() << "Export Stop" << ErrorStr;
        emit sigResult(false);
        if (ErrorStr != stopStr) {
            emit sigError(QString("export error: %1").arg(ErrorStr));
        }
        return false;
    }
    emit sigResult(m_canRunning);
    return true && m_canRunning;
}

bool LogExportThread::exportToXls(QString fileName, QList<LOG_MSG_JOURNAL> jList,
                                  QStringList labels, LOG_FLAG iFlag)
{


    try {
        auto currentXlsRow = 1;
        QXlsx::Document xlsx(&m_canRunning);
        connect(&xlsx, &QXlsx::Document::sigProcessAbstractSheet, this, [ = ](int iCurrent, int iTotal) {
            int end = static_cast<int>(iTotal * 0.1 > 5 ? iTotal * 0.1 : 5);
            sigProgress(iCurrent + iTotal, iTotal * 3 + end);
        });
        connect(&xlsx, &QXlsx::Document::sigProcessharedStrings, this, [ = ](int iCurrent, int iTotal) {
            int end = static_cast<int>(iTotal * 0.1 > 5 ? iTotal * 0.1 : 5);
            sigProgress(iCurrent + iTotal * 2, iTotal * 3 + end);
        });
        for (int col = 0; col < labels.count(); ++col) {
            QXlsx::Format boldFont;
            boldFont.setFontBold(true);
            xlsx.write(currentXlsRow, col + 1, labels.at(col), boldFont);
        }
        ++currentXlsRow;
        int end = static_cast<int>(jList.count() * 0.1 > 5 ? jList.count() * 0.1 : 5);

        for (int row = 0; row < jList.count(); ++row) {
            if (!m_canRunning) {
                throw  QString(stopStr);
            }
            LOG_MSG_JOURNAL message = jList.at(row);
            int col = 1;

            if (iFlag == JOURNAL) {
                xlsx.write(currentXlsRow, col++, message.level);
                xlsx.write(currentXlsRow, col++, message.daemonName);
                xlsx.write(currentXlsRow, col++, message.dateTime);
                xlsx.write(currentXlsRow, col++, message.msg);
                xlsx.write(currentXlsRow, col++, message.hostName);
                xlsx.write(currentXlsRow, col++, message.daemonId);
            } else if (iFlag == KERN) {
                xlsx.write(currentXlsRow, col++, message.dateTime);
                xlsx.write(currentXlsRow, col++, message.hostName);
                xlsx.write(currentXlsRow, col++, message.daemonName);
                xlsx.write(currentXlsRow, col++, message.msg);
            }

            ++currentXlsRow;
            sigProgress(row + 1, jList.count() * 3 + end);
        }
        ++currentXlsRow;
        if (!xlsx.saveAs(fileName)) {
            throw  QString(stopStr);
        }
        sigProgress(100, 100);
    } catch (QString ErrorStr) {
        qDebug() << "Export Stop" << ErrorStr;
        emit sigResult(false);
        if (ErrorStr != stopStr) {
            emit sigError(QString("export error: %1").arg(ErrorStr));
        }
        return false;
    }
    emit sigResult(m_canRunning);
    return true && m_canRunning;
}

bool LogExportThread::exportToXlsNew(QString fileName, QList<LOG_MSG_JOURNAL> jList, QStringList labels, LOG_FLAG iFlag)
{
    try {
        auto currentXlsRow = 0;
        lxw_workbook  *workbook  = workbook_new(fileName.toStdString().c_str());
        lxw_worksheet *worksheet = workbook_add_worksheet(workbook, NULL);
        lxw_format *format = workbook_add_format(workbook);
        format_set_bold(format);
        for (int col = 0; col < labels.count(); ++col) {
            worksheet_write_string(worksheet, currentXlsRow, col, labels.at(col).toStdString().c_str(), format);
        }
        ++currentXlsRow;
        int end = static_cast<int>(jList.count() * 0.1 > 5 ? jList.count() * 0.1 : 5);

        for (int row = 0; row < jList.count(); ++row) {
            if (!m_canRunning) {
                throw  QString(stopStr);
            }
            LOG_MSG_JOURNAL message = jList.at(row);
            int col = 0;

            if (iFlag == JOURNAL) {
                worksheet_write_string(worksheet, currentXlsRow, col++, message.level.toStdString().c_str(), NULL);
                worksheet_write_string(worksheet, currentXlsRow, col++, message.daemonName.toStdString().c_str(), NULL);
                worksheet_write_string(worksheet, currentXlsRow, col++, message.dateTime.toStdString().c_str(), NULL);
                worksheet_write_string(worksheet, currentXlsRow, col++, message.msg.toStdString().c_str(), NULL);
                worksheet_write_string(worksheet, currentXlsRow, col++, message.hostName.toStdString().c_str(), NULL);
                worksheet_write_string(worksheet, currentXlsRow, col++, message.daemonId.toStdString().c_str(), NULL);
            } else if (iFlag == KERN) {
                worksheet_write_string(worksheet, currentXlsRow, col++, message.dateTime.toStdString().c_str(), NULL);
                worksheet_write_string(worksheet, currentXlsRow, col++, message.hostName.toStdString().c_str(), NULL);
                worksheet_write_string(worksheet, currentXlsRow, col++, message.daemonName.toStdString().c_str(), NULL);
                worksheet_write_string(worksheet, currentXlsRow, col++, message.msg.toStdString().c_str(), NULL);
            }

            ++currentXlsRow;
            sigProgress(row + 1, jList.count() * 3 + end);
        }


        workbook_close(workbook);
        malloc_trim(0);
        sigProgress(100, 100);
    } catch (QString ErrorStr) {
        qDebug() << "Export Stop" << ErrorStr;
        emit sigResult(false);
        if (ErrorStr != stopStr) {
            emit sigError(QString("export error: %1").arg(ErrorStr));
        }
        return false;
    }
    emit sigResult(m_canRunning);
    return true && m_canRunning;
}

bool LogExportThread::exportToXls(QString fileName, QList<LOG_MSG_APPLICATOIN> jList, QStringList labels, QString &iAppName)
{
    QElapsedTimer timer;
    timer.start();
    try {
        auto currentXlsRow = 1;

        QXlsx::Document xlsx(&m_canRunning);
        connect(& xlsx, &QXlsx::Document::sigProcessAbstractSheet, this, [ = ](int iCurrent, int iTotal) {
            int end = static_cast<int>(iTotal * 0.1 > 5 ? iTotal * 0.1 : 5);
            sigProgress(iCurrent + iTotal, iTotal * 3 + end);
        });
        connect(&xlsx, &QXlsx::Document::sigProcessharedStrings, this, [ = ](int iCurrent, int iTotal) {
            int end = static_cast<int>(iTotal * 0.1 > 5 ? iTotal * 0.1 : 5);
            sigProgress(iCurrent + iTotal * 2, iTotal * 3 + end);
        });
        for (int col = 0; col < labels.count(); ++col) {
            QXlsx::Format boldFont;
            boldFont.setFontBold(true);
            xlsx.write(currentXlsRow, col + 1, labels.at(col), boldFont);
        }
        ++currentXlsRow;
        int end = static_cast<int>(jList.count() * 0.1 > 5 ? jList.count() * 0.1 : 5);
        for (int row = 0; row < jList.count(); ++row) {
            if (!m_canRunning) {
                throw  QString(stopStr);
            }
            LOG_MSG_APPLICATOIN message = jList.at(row);
            int col = 1;
            xlsx.write(currentXlsRow, col++, strTranslate(message.level));
            xlsx.write(currentXlsRow, col++, message.dateTime);
            xlsx.write(currentXlsRow, col++, iAppName);
            xlsx.write(currentXlsRow, col++, message.msg);
            ++currentXlsRow;
            sigProgress(row + 1, jList.count() * 3 + end);
        }
        ++currentXlsRow;
        if (!xlsx.saveAs(fileName)) {
            throw  QString(stopStr);
        }
        sigProgress(100, 100);
    } catch (QString ErrorStr) {
        qDebug() << "Export Stop" << ErrorStr;
        emit sigResult(false);
        if (ErrorStr != stopStr) {
            emit sigError(QString("export error: %1").arg(ErrorStr));
        }
        return false;
    }
    emit sigResult(m_canRunning);
    return true && m_canRunning;
}

bool LogExportThread::exportToXls(QString fileName, QList<LOG_MSG_DPKG> jList, QStringList labels)
{
    try {
        auto currentXlsRow = 1;
        QXlsx::Document xlsx(&m_canRunning);
        connect(&xlsx, &QXlsx::Document::sigProcessAbstractSheet, this, [ = ](int iCurrent, int iTotal) {
            int end = static_cast<int>(iTotal * 0.1 > 5 ? iTotal * 0.1 : 5);
            sigProgress(iCurrent + iTotal, iTotal * 3 + end);
        });
        connect(&xlsx, &QXlsx::Document::sigProcessharedStrings, this, [ = ](int iCurrent, int iTotal) {
            int end = static_cast<int>(iTotal * 0.1 > 5 ? iTotal * 0.1 : 5);
            sigProgress(iCurrent + iTotal * 2, iTotal * 3 + end);
        });
        for (int col = 0; col < labels.count(); ++col) {
            QXlsx::Format boldFont;
            boldFont.setFontBold(true);
            xlsx.write(currentXlsRow, col + 1, labels.at(col), boldFont);
        }
        ++currentXlsRow;
        int end = static_cast<int>(jList.count() * 0.1 > 5 ? jList.count() * 0.1 : 5);
        for (int row = 0; row < jList.count(); ++row) {
            if (!m_canRunning) {
                throw  QString(stopStr);
            }
            LOG_MSG_DPKG message = jList.at(row);
            int col = 1;
            xlsx.write(currentXlsRow, col++, message.dateTime);
            xlsx.write(currentXlsRow, col++, message.msg);
            xlsx.write(currentXlsRow, col++, message.action);
            ++currentXlsRow;
            sigProgress(row + 1, jList.count() * 3 + end);
        }
        ++currentXlsRow;
        if (!xlsx.saveAs(fileName)) {
            throw  QString(stopStr);
        }
        sigProgress(100, 100);
    } catch (QString ErrorStr) {
        qDebug() << "Export Stop" << ErrorStr;
        emit sigResult(false);
        if (ErrorStr != stopStr) {
            emit sigError(QString("export error: %1").arg(ErrorStr));
        }
        return false;
    }
    emit sigResult(m_canRunning);
    return true && m_canRunning;
}

bool LogExportThread::exportToXls(QString fileName, QList<LOG_MSG_BOOT> jList, QStringList labels)
{
    try {
        auto currentXlsRow = 1;
        QXlsx::Document xlsx(&m_canRunning);
//        connect(&xlsx, &QXlsx::Document::saveProcess, this, [ = ](int iCurrent, int iTotal) {
//            sigProgress(iCurrent + iTotal, iTotal * 2);
//        });
        connect(&xlsx, &QXlsx::Document::sigProcessAbstractSheet, this, [ = ](int iCurrent, int iTotal) {
            int end = static_cast<int>(iTotal * 0.1 > 5 ? iTotal * 0.1 : 5);
            sigProgress(iCurrent + iTotal, iTotal * 3 + end);
        });
        connect(&xlsx, &QXlsx::Document::sigProcessharedStrings, this, [ = ](int iCurrent, int iTotal) {
            int end = static_cast<int>(iTotal * 0.1 > 5 ? iTotal * 0.1 : 5);
            sigProgress(iCurrent + iTotal * 2, iTotal * 3 + end);
        });
        for (int col = 0; col < labels.count(); ++col) {
            QXlsx::Format boldFont;
            boldFont.setFontBold(true);
            xlsx.write(currentXlsRow, col + 1, labels.at(col), boldFont);
        }
        ++currentXlsRow;
        int end = static_cast<int>(jList.count() * 0.1 > 5 ? jList.count() * 0.1 : 5);
        for (int row = 0; row < jList.count(); ++row) {
            if (!m_canRunning) {
                throw  QString(stopStr);
            }
            LOG_MSG_BOOT message = jList.at(row);
            int col = 1;
            xlsx.write(currentXlsRow, col++, message.status);
            xlsx.write(currentXlsRow, col++, message.msg);
            ++currentXlsRow;
            sigProgress(row + 1, jList.count() * 3 + end);
        }
        ++currentXlsRow;
        if (!xlsx.saveAs(fileName)) {
            throw  QString(stopStr);
        }
        sigProgress(100, 100);
    } catch (QString ErrorStr) {
        qDebug() << "Export Stop" << ErrorStr;
        emit sigResult(false);
        if (ErrorStr != stopStr) {
            emit sigError(QString("export error: %1").arg(ErrorStr));
        }
        return false;
    }
    emit sigResult(m_canRunning);
    return true && m_canRunning;
}

bool LogExportThread::exportToXls(QString fileName, QList<LOG_MSG_XORG> jList, QStringList labels)
{
    try {
        auto currentXlsRow = 1;
        QXlsx::Document xlsx(&m_canRunning);
        connect(&xlsx, &QXlsx::Document::sigProcessAbstractSheet, this, [ = ](int iCurrent, int iTotal) {
            int end = static_cast<int>(iTotal * 0.1 > 5 ? iTotal * 0.1 : 5);
            sigProgress(iCurrent + iTotal, iTotal * 3 + end);
        });
        connect(&xlsx, &QXlsx::Document::sigProcessharedStrings, this, [ = ](int iCurrent, int iTotal) {
            int end = static_cast<int>(iTotal * 0.1 > 5 ? iTotal * 0.1 : 5);
            sigProgress(iCurrent + iTotal * 2, iTotal * 3 + end);
        });
        for (int col = 0; col < labels.count(); ++col) {
            QXlsx::Format boldFont;
            boldFont.setFontBold(true);
            xlsx.write(currentXlsRow, col + 1, labels.at(col), boldFont);
        }
        ++currentXlsRow;
        int end = static_cast<int>(jList.count() * 0.1 > 5 ? jList.count() * 0.1 : 5);
        for (int row = 0; row < jList.count(); ++row) {
            if (!m_canRunning) {
                throw  QString(stopStr);
            }
            LOG_MSG_XORG message = jList.at(row);
            int col = 1;
            xlsx.write(currentXlsRow, col++, message.dateTime);
            xlsx.write(currentXlsRow, col++, message.msg);
            ++currentXlsRow;
            sigProgress(row + 1, jList.count() * 3 + end);
        }
        ++currentXlsRow;
        if (!xlsx.saveAs(fileName)) {
            throw  QString(stopStr);
        }
        sigProgress(100, 100);
    } catch (QString ErrorStr) {
        qDebug() << "Export Stop" << ErrorStr;
        emit sigResult(false);
        if (ErrorStr != stopStr) {
            emit sigError(QString("export error: %1").arg(ErrorStr));
        }
        return false;
    }
    emit sigResult(m_canRunning);
    return true && m_canRunning;
}

bool LogExportThread::exportToXls(QString fileName, QList<LOG_MSG_NORMAL> jList, QStringList labels)
{
    try {
        auto currentXlsRow = 1;
        QXlsx::Document xlsx(&m_canRunning);
        connect(&xlsx, &QXlsx::Document::sigProcessAbstractSheet, this, [ = ](int iCurrent, int iTotal) {
            int end = static_cast<int>(iTotal * 0.1 > 5 ? iTotal * 0.1 : 5);
            sigProgress(iCurrent + iTotal, iTotal * 3 + end);
        });
        connect(&xlsx, &QXlsx::Document::sigProcessharedStrings, this, [ = ](int iCurrent, int iTotal) {
            int end = static_cast<int>(iTotal * 0.1 > 5 ? iTotal * 0.1 : 5);
            sigProgress(iCurrent + iTotal * 2, iTotal * 3 + end);
        });
        for (int col = 0; col < labels.count(); ++col) {
            QXlsx::Format boldFont;
            boldFont.setFontBold(true);
            xlsx.write(currentXlsRow, col + 1, labels.at(col), boldFont);
        }
        ++currentXlsRow;
        int end = static_cast<int>(jList.count() * 0.1 > 5 ? jList.count() * 0.1 : 5);
        for (int row = 0; row < jList.count(); ++row) {
            if (!m_canRunning) {
                throw  QString(stopStr);
            }
            LOG_MSG_NORMAL message = jList.at(row);
            int col = 1;
            xlsx.write(currentXlsRow, col++, message.eventType);
            xlsx.write(currentXlsRow, col++, message.userName);
            xlsx.write(currentXlsRow, col++, message.dateTime);
            xlsx.write(currentXlsRow, col++, message.msg);
            ++currentXlsRow;
            sigProgress(row + 1, jList.count() * 3 + end);
        }
        ++currentXlsRow;
        if (!xlsx.saveAs(fileName)) {
            throw  QString(stopStr);
        }
        sigProgress(100, 100);
    } catch (QString ErrorStr) {
        qDebug() << "Export Stop" << ErrorStr;
        emit sigResult(false);
        if (ErrorStr != stopStr) {
            emit sigError(QString("export error: %1").arg(ErrorStr));
        }
        return false;
    }

    emit sigResult(m_canRunning);
    return true && m_canRunning;
}

bool LogExportThread::exportToXls(QString fileName, QList<LOG_MSG_KWIN> jList, QStringList labels)
{
    try {
        auto currentXlsRow = 1;
        QXlsx::Document xlsx(&m_canRunning);
        connect(&xlsx, &QXlsx::Document::sigProcessAbstractSheet, this, [ = ](int iCurrent, int iTotal) {
            int end = static_cast<int>(iTotal * 0.1 > 5 ? iTotal * 0.1 : 5);
            sigProgress(iCurrent + iTotal, iTotal * 3 + end);
        });
        connect(&xlsx, &QXlsx::Document::sigProcessharedStrings, this, [ = ](int iCurrent, int iTotal) {
            int end = static_cast<int>(iTotal * 0.1 > 5 ? iTotal * 0.1 : 5);
            sigProgress(iCurrent + iTotal * 2, iTotal * 3 + end);
        });
        for (int col = 0; col < labels.count(); ++col) {
            QXlsx::Format boldFont;
            boldFont.setFontBold(true);
            xlsx.write(currentXlsRow, col + 1, labels.at(col), boldFont);
        }
        ++currentXlsRow;
        int end = static_cast<int>(jList.count() * 0.1 > 5 ? jList.count() * 0.1 : 5);
        for (int row = 0; row < jList.count(); ++row) {
            if (!m_canRunning) {
                throw  QString(stopStr);
            }
            LOG_MSG_KWIN message = jList.at(row);
            int col = 1;
            xlsx.write(currentXlsRow, col++, message.msg);
            ++currentXlsRow;
            sigProgress(row + 1, jList.count() * 3 + end);
        }
        ++currentXlsRow;

        if (!xlsx.saveAs(fileName)) {
            throw  QString(stopStr);
        }
        sigProgress(100, 100);
    } catch (QString ErrorStr) {
        qDebug() << "Export Stop" << ErrorStr;
        emit sigResult(false);
        if (ErrorStr != stopStr) {
            emit sigError(QString("export error: %1").arg(ErrorStr));
        }
        return false;
    }
    emit sigResult(m_canRunning);
    return true && m_canRunning;
}

void LogExportThread::initMap()
{
    m_levelStrMap.clear();
    m_levelStrMap.insert("Emergency", DApplication::translate("Level", "Emergency"));
    m_levelStrMap.insert("Alert", DApplication::translate("Level", "Alert"));
    m_levelStrMap.insert("Critical", DApplication::translate("Level", "Critical"));
    m_levelStrMap.insert("Error", DApplication::translate("Level", "Error"));
    m_levelStrMap.insert("Warning", DApplication::translate("Level", "Warning"));
    m_levelStrMap.insert("Notice", DApplication::translate("Level", "Notice"));
    m_levelStrMap.insert("Info", DApplication::translate("Level", "Info"));
    m_levelStrMap.insert("Debug", DApplication::translate("Level", "Debug"));
}

QString LogExportThread::strTranslate(QString &iLevelStr)
{
    return m_levelStrMap.value(iLevelStr, iLevelStr);
}





void LogExportThread::run()
{
    switch (m_runMode) {
    case TxtModel: {
        exportToTxt(m_fileName, m_pModel, m_flag);
        break;
    }
    case TxtJOURNAL: {
        exportToTxt(m_fileName, m_jList, m_labels, m_flag);
        break;
    }
    case TxtAPP: {
        exportToTxt(m_fileName, m_appList, m_labels, m_appName);
        break;
    }
    case TxtDPKG: {
        exportToTxt(m_fileName, m_dpkgList, m_labels);
        break;
    }
    case TxtBOOT: {
        exportToTxt(m_fileName, m_bootList, m_labels);
        break;
    }
    case TxtXORG: {
        exportToTxt(m_fileName, m_xorgList, m_labels);
        break;
    }
    case TxtNORMAL: {
        exportToTxt(m_fileName, m_normalList, m_labels);
        break;
    }
    case TxtKWIN: {
        exportToTxt(m_fileName, m_kwinList, m_labels);
        break;
    }
    case HtmlModel: {
        exportToHtml(m_fileName, m_pModel, m_flag);
        break;
    }
    case HtmlJOURNAL: {
        exportToHtml(m_fileName, m_jList, m_labels, m_flag);
        break;
    }
    case HtmlAPP: {
        exportToHtml(m_fileName, m_appList, m_labels, m_appName);
        break;
    }
    case HtmlDPKG: {
        exportToHtml(m_fileName, m_dpkgList, m_labels);
        break;
    }
    case HtmlBOOT: {
        exportToHtml(m_fileName, m_bootList, m_labels);
        break;
    }
    case HtmlXORG: {
        exportToHtml(m_fileName, m_xorgList, m_labels);
        break;
    }
    case HtmlNORMAL: {
        exportToHtml(m_fileName, m_normalList, m_labels);
        break;
    }
    case HtmlKWIN: {
        exportToHtml(m_fileName, m_kwinList, m_labels);
        break;
    }
    case DocModel: {
        exportToDoc(m_fileName, m_pModel, m_flag);
        break;
    }
    case DocJOURNAL: {
        exportToDoc(m_fileName, m_jList, m_labels, m_flag);
        break;
    }
    case DocAPP: {
        exportToDocNew(m_fileName, m_appList, m_labels, m_appName);
        break;
    }
    case DocDPKG: {
        exportToDoc(m_fileName, m_dpkgList, m_labels);
        break;
    }
    case DocBOOT: {
        exportToDoc(m_fileName, m_bootList, m_labels);
        break;
    }
    case DocXORG: {
        exportToDoc(m_fileName, m_xorgList, m_labels);
        break;
    }
    case DocNORMAL: {
        exportToDoc(m_fileName, m_normalList, m_labels);
        break;
    }
    case DocKWIN: {
        exportToDoc(m_fileName, m_kwinList, m_labels);
        break;
    }
    case XlsModel: {
        exportToXls(m_fileName, m_pModel, m_flag);
        break;
    }
    case XlsJOURNAL: {
        QElapsedTimer timer;
        timer.start();
        //  exportToXls(m_fileName, m_jList, m_labels, m_flag);
        exportToXlsNew(m_fileName, m_jList, m_labels, m_flag);
        qDebug() << "export test" << timer.elapsed();
        break;
    }
    case XlsAPP: {
        exportToXls(m_fileName, m_appList, m_labels, m_appName);
        break;
    }
    case XlsDPKG: {
        exportToXls(m_fileName, m_dpkgList, m_labels);
        break;
    }
    case XlsBOOT: {
        exportToXls(m_fileName, m_bootList, m_labels);
        break;
    }
    case XlsXORG: {
        exportToXls(m_fileName, m_xorgList, m_labels);
        break;
    }
    case XlsNORMAL: {
        exportToXls(m_fileName, m_normalList, m_labels);
        break;
    }
    case XlsKWIN: {
        exportToXls(m_fileName, m_kwinList, m_labels);
        break;
    }
    default:
        break;
    }
    if (!m_canRunning) {
        Utils::checkAndDeleteDir(m_fileName);
    }

    m_canRunning = false;
}
