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
//#include "WordProcessingMerger.h"
//#include "WordProcessingCompiler.h"
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

/**
 * @brief LogExportThread::LogExportThread 导出日志线程类构造函数
 * @param parent 父对象
 */
LogExportThread::LogExportThread(QObject *parent)
    :  QObject(parent),
       QRunnable()
{
    setAutoDelete(true);
    initMap();
}
/**
 * @brief LogExportThread::~LogExportThread 析构函数
 */
LogExportThread::~LogExportThread()
{
    //释放空闲内存
    malloc_trim(0);
}

/**
 * @brief LogExportThread::exportToTxtPublic 导出到日志txt格式配置函数对QStandardItemModel数据类型的重载
 * @param fileName 导出文件路径全称
 * @param pModel 要导出的数据源，为QStandardItemModel
 * @param flag 导出的日志类型
 */
void LogExportThread::exportToTxtPublic(QString fileName, QStandardItemModel *pModel, LOG_FLAG flag)
{
    m_fileName = fileName;
    m_pModel = pModel;
    m_flag = flag;
    m_runMode = TxtModel;
    m_canRunning = true;
}

/**
 * @brief LogExportThread::exportToTxtPublic 导出到日志txt格式配置函数，对LOG_MSG_JOURNAL数据类型的重载（指系统日志和内核日志）
 * @param fileName 导出文件路径全称
 * @param jList 要导出的数据源 数据类型为LOG_MSG_JOURNAL
 * @param labels 表头字符串
 * @param flag  导出的日志类型
 */
void LogExportThread::exportToTxtPublic(QString fileName, QList<LOG_MSG_JOURNAL> jList,  QStringList labels, LOG_FLAG flag)
{
    m_fileName = fileName;
    m_jList = jList;
    m_runMode = TxtJOURNAL;
    m_labels = labels;
    m_flag = flag;
    m_canRunning = true;

}

/**
 * @brief LogExportThread::exportToTxtPublic  导出到日志txt格式配置函数，对LOG_MSG_APPLICATOIN数据类型的重载（指应用日志）
 * @param fileName 导出文件路径全称
 * @param jList 要导出的数据源 数据类型为LOG_MSG_APPLICATOIN
 * @param labels 表头字符串
 * @param iAppName 导出的应用日志的应用名称
 */
void LogExportThread::exportToTxtPublic(QString fileName, QList<LOG_MSG_APPLICATOIN> jList, QStringList labels, QString &iAppName)
{
    m_fileName = fileName;
    m_appList = jList;
    m_labels = labels;
    m_runMode = TxtAPP;
    m_canRunning = true;
    m_appName = iAppName;
}

/**
 * @brief LogExportThread::exportToTxtPublic 导出到日志txt格式配置函数，对LOG_MSG_DPKG数据类型的重载（指dpkg日志）
 * @param fileName 导出文件路径全称
 * @param jList  要导出的数据源 LOG_MSG_DPKG
 * @param labels 表头字符串
 */
void LogExportThread::exportToTxtPublic(QString fileName, QList<LOG_MSG_DPKG> jList, QStringList labels)
{
    m_fileName = fileName;
    m_dpkgList = jList;
    m_labels = labels;
    m_runMode = TxtDPKG;
    m_canRunning = true;
}

/**
 * @brief LogExportThread::exportToTxtPublic 导出到日志txt格式配置函数，对LOG_MSG_BOOT数据类型的重载（指启动日志）
 * @param fileName 导出文件路径全称
 * @param jList  要导出的数据源 LOG_MSG_BOOT
 * @param labels 表头字符串
 */
void LogExportThread::exportToTxtPublic(QString fileName, QList<LOG_MSG_BOOT> jList, QStringList labels)
{
    m_fileName = fileName;
    m_bootList = jList;
    m_labels = labels;
    m_runMode = TxtBOOT;
    m_canRunning = true;
}

/**
 * @brief LogExportThread::exportToTxtPublic 导出到日志txt格式配置函数，对LOG_MSG_XORG数据类型的重载（指xorg日志）
 * @param fileName 导出文件路径全称
 * @param jList 要导出的数据源 LOG_MSG_XORG
 * @param labels 表头字符串
 */
void LogExportThread::exportToTxtPublic(QString fileName, QList<LOG_MSG_XORG> jList, QStringList labels)
{
    m_fileName = fileName;
    m_xorgList = jList;
    m_labels = labels;
    m_runMode = TxtXORG;
    m_canRunning = true;
}


/**
 * @brief LogExportThread::exportToTxtPublic 导出到日志txt格式配置函数，对LOG_MSG_NORMAL数据类型的重载（指开关机日志）
 * @param fileName 导出文件路径全称
 * @param jList 要导出的数据源 LOG_MSG_NORMAL
 * @param labels 表头字符串
 */
void LogExportThread::exportToTxtPublic(QString fileName, QList<LOG_MSG_NORMAL> jList, QStringList labels)
{
    m_fileName = fileName;
    m_normalList = jList;
    m_labels = labels;
    m_runMode = TxtNORMAL;
    m_canRunning = true;
}

/**
 * @brief LogExportThread::exportToTxtPublic导出到日志txt格式配置函数，对LOG_MSG_KWIN数据类型的重载（指klu上的kwin日志）
 * @param fileName 导出文件路径全称
 * @param jList 要导出的数据源 LOG_MSG_KWIN
 * @param labels 表头字符串
 */
void LogExportThread::exportToTxtPublic(QString fileName, QList<LOG_MSG_KWIN> jList, QStringList labels)
{
    m_fileName = fileName;
    m_kwinList = jList;
    m_labels = labels;
    m_runMode = TxtKWIN;
    m_canRunning = true;
}

/**
 * @brief LogExportThread::exportToHtmlPublic 导出到日志html格式配置函数对QStandardItemModel数据类型的重载
 * @param fileName 导出文件路径全称
 * @param pModel 要导出的数据源，为QStandardItemModel
 * @param flag 导出的日志类型
 */
void LogExportThread::exportToHtmlPublic(QString fileName, QStandardItemModel *pModel, LOG_FLAG flag)
{
    m_fileName = fileName;
    m_pModel = pModel;
    m_flag = flag;
    m_runMode = HtmlModel;
    m_canRunning = true;

}

/**
 * @brief LogExportThread::exportToHtmlPublic 导出到日志html格式配置函数，对LOG_MSG_JOURNAL数据类型的重载（指系统日志和内核日志）
 * @param fileName 导出文件路径全称
 * @param jList 要导出的数据源 数据类型为LOG_MSG_JOURNAL
 * @param labels 表头字符串
 * @param flag  导出的日志类型
 */
void LogExportThread::exportToHtmlPublic(QString fileName, QList<LOG_MSG_JOURNAL> jList, QStringList labels, LOG_FLAG flag)
{
    m_fileName = fileName;
    m_jList = jList;
    m_runMode = HtmlJOURNAL;
    m_labels = labels;
    m_flag = flag;
    m_canRunning = true;

}

/**
 * @brief LogExportThread::exportToHtmlPublic  导出到日志html格式配置函数，对LOG_MSG_APPLICATOIN数据类型的重载（指应用日志）
 * @param fileName 导出文件路径全称
 * @param jList 要导出的数据源 数据类型为LOG_MSG_APPLICATOIN
 * @param labels 表头字符串
 * @param iAppName 导出的应用日志的应用名称
 */
void LogExportThread::exportToHtmlPublic(QString fileName, QList<LOG_MSG_APPLICATOIN> jList, QStringList labels, QString &iAppName)
{
    m_fileName = fileName;
    m_appList = jList;
    m_labels = labels;
    m_runMode = HtmlAPP;
    m_canRunning = true;
    m_appName = iAppName;
}

/**
 * @brief LogExportThread::exportToHtmlPublic 导出到日志html格式配置函数，对LOG_MSG_DPKG数据类型的重载（指dpkg日志）
 * @param fileName 导出文件路径全称
 * @param jList  要导出的数据源 LOG_MSG_DPKG
 * @param labels 表头字符串
 */
void LogExportThread::exportToHtmlPublic(QString fileName, QList<LOG_MSG_DPKG> jList, QStringList labels)
{
    m_fileName = fileName;
    m_dpkgList = jList;
    m_labels = labels;
    m_runMode = HtmlDPKG;
    m_canRunning = true;
}

/**
 * @brief LogExportThread::exportToHtmlPublic导出到日志html格式配置函数，对LOG_MSG_BOOT数据类型的重载（指启动日志）
 * @param fileName 导出文件路径全称
 * @param jList  要导出的数据源 LOG_MSG_BOOT
 * @param labels 表头字符串
 */
void LogExportThread::exportToHtmlPublic(QString fileName, QList<LOG_MSG_BOOT> jList, QStringList labels)
{
    m_fileName = fileName;
    m_bootList = jList;
    m_labels = labels;
    m_runMode = HtmlBOOT;
    m_canRunning = true;
}

/**
 * @brief LogExportThread::exportToHtmlPublic导出到日志html格式配置函数，对LOG_MSG_XORG数据类型的重载（指xorg日志）
 * @param fileName 导出文件路径全称
 * @param jList 要导出的数据源 LOG_MSG_XORG
 * @param labels 表头字符串
 */
void LogExportThread::exportToHtmlPublic(QString fileName, QList<LOG_MSG_XORG> jList, QStringList labels)
{
    m_fileName = fileName;
    m_xorgList = jList;
    m_labels = labels;
    m_runMode = HtmlXORG;
    m_canRunning = true;
}

/**
 * @brief LogExportThread::exportToHtmlPublic导出到日志html格式配置函数，对LOG_MSG_NORMAL数据类型的重载（指开关机日志）
 * @param fileName 导出文件路径全称
 * @param jList 要导出的数据源 LOG_MSG_NORMAL
 * @param labels 表头字符串
 */
void LogExportThread::exportToHtmlPublic(QString fileName, QList<LOG_MSG_NORMAL> jList, QStringList labels)
{
    m_fileName = fileName;
    m_normalList = jList;
    m_labels = labels;
    m_runMode = HtmlNORMAL;
    m_canRunning = true;
}

/**
 * @brief LogExportThread::exportToHtmlPublic导出到日志html格式配置函数，对LOG_MSG_KWIN数据类型的重载（指klu上的kwin日志）
 * @param fileName 导出文件路径全称
 * @param jList 要导出的数据源 LOG_MSG_KWIN
 * @param labels 表头字符串
 */
void LogExportThread::exportToHtmlPublic(QString fileName, QList<LOG_MSG_KWIN> jList, QStringList labels)
{
    m_fileName = fileName;
    m_kwinList = jList;
    m_labels = labels;
    m_runMode = HtmlKWIN;
    m_canRunning = true;
}

/**
 * @brief LogExportThread::exportToDocPublic导出到日志doc格式配置函数对QStandardItemModel数据类型的重载
 * @param fileName 导出文件路径全称
 * @param pModel 要导出的数据源，为QStandardItemModel
 * @param flag 导出的日志类型
 */
void LogExportThread::exportToDocPublic(QString fileName, QStandardItemModel *pModel, LOG_FLAG flag)
{
    m_fileName = fileName;
    m_pModel = pModel;
    m_flag = flag;
    m_runMode = DocModel;
    m_canRunning = true;

}

/**
 * @brief LogExportThread::exportToDocPublic导出到日志doc格式配置函数，对LOG_MSG_JOURNAL数据类型的重载（指系统日志和内核日志）
 * @param fileName 导出文件路径全称
 * @param jList 要导出的数据源 数据类型为LOG_MSG_JOURNAL
 * @param labels 表头字符串
 * @param flag  导出的日志类型
 */
void LogExportThread::exportToDocPublic(QString fileName, QList<LOG_MSG_JOURNAL> jList, QStringList labels, LOG_FLAG iFlag)
{
    m_fileName = fileName;
    m_jList = jList;
    m_flag = iFlag;
    m_labels = labels;
    m_runMode = DocJOURNAL;
    m_canRunning = true;

}

/**
 * @brief LogExportThread::exportToDocPublic导出到日志doc格式配置函数，对LOG_MSG_APPLICATOIN数据类型的重载（指应用日志）
 * @param fileName 导出文件路径全称
 * @param jList 要导出的数据源 数据类型为LOG_MSG_APPLICATOIN
 * @param labels 表头字符串
 * @param iAppName 导出的应用日志的应用名称
 */
void LogExportThread::exportToDocPublic(QString fileName, QList<LOG_MSG_APPLICATOIN> jList, QStringList labels, QString &iAppName)
{
    m_fileName = fileName;
    m_appList = jList;
    m_labels = labels;
    m_runMode = DocAPP;
    m_canRunning = true;
    m_appName = iAppName;
}

/**
 * @brief LogExportThread::exportToDocPublic 导出到日志doc格式配置函数，对LOG_MSG_DPKG数据类型的重载（指dpkg日志）
 * @param fileName 导出文件路径全称
 * @param jList  要导出的数据源 LOG_MSG_DPKG
 * @param labels 表头字符串
 */
void LogExportThread::exportToDocPublic(QString fileName, QList<LOG_MSG_DPKG> jList, QStringList labels)
{
    m_fileName = fileName;
    m_dpkgList = jList;
    m_labels = labels;
    m_runMode = DocDPKG;
    m_canRunning = true;
}

/**
 * @brief LogExportThread::exportToDocPublic 导出到日志doc格式配置函数，对LOG_MSG_BOOT数据类型的重载（指启动日志）
 * @param fileName 导出文件路径全称
 * @param jList  要导出的数据源 LOG_MSG_BOOT
 * @param labels 表头字符串
 */
void LogExportThread::exportToDocPublic(QString fileName, QList<LOG_MSG_BOOT> jList, QStringList labels)
{
    m_fileName = fileName;
    m_bootList = jList;
    m_labels = labels;
    m_runMode = DocBOOT;
    m_canRunning = true;
}

/**
 * @brief LogExportThread::exportToDocPublic 导出到日志doc格式配置函数，对LOG_MSG_XORG数据类型的重载（指xorg日志）
 * @param fileName 导出文件路径全称
 * @param jList 要导出的数据源 LOG_MSG_XORG
 * @param labels 表头字符串
 */
void LogExportThread::exportToDocPublic(QString fileName, QList<LOG_MSG_XORG> jList, QStringList labels)
{
    m_fileName = fileName;
    m_xorgList = jList;
    m_labels = labels;
    m_runMode = DocXORG;
    m_canRunning = true;
}

/**
 * @brief LogExportThread::exportToDocPublic 导出到日志doc格式配置函数，对LOG_MSG_NORMAL数据类型的重载（指开关机日志）
 * @param fileName 导出文件路径全称
 * @param jList 要导出的数据源 LOG_MSG_NORMAL
 * @param labels 表头字符串
 */
void LogExportThread::exportToDocPublic(QString fileName, QList<LOG_MSG_NORMAL> jList, QStringList labels)
{
    m_fileName = fileName;
    m_normalList = jList;
    m_labels = labels;
    m_runMode = DocNORMAL;
    m_canRunning = true;
}

/**
 * @brief LogExportThread::exportToDocPublic导出到日志doc格式配置函数，对LOG_MSG_KWIN数据类型的重载（指klu上的kwin日志）
 * @param fileName 导出文件路径全称
 * @param jList 要导出的数据源 LOG_MSG_KWIN
 * @param labels 表头字符串
 */
void LogExportThread::exportToDocPublic(QString fileName, QList<LOG_MSG_KWIN> jList, QStringList labels)
{
    m_fileName = fileName;
    m_kwinList = jList;
    m_labels = labels;
    m_runMode = DocKWIN;
    m_canRunning = true;
}

/**
 * @brief LogExportThread::exportToXlsPublic 导出到日志xlsx格式配置函数对QStandardItemModel数据类型的重载
 * @param fileName 导出文件路径全称
 * @param pModel 要导出的数据源，为QStandardItemModel
 * @param flag 导出的日志类型
 */
void LogExportThread::exportToXlsPublic(QString fileName, QStandardItemModel *pModel, LOG_FLAG flag)
{
    m_fileName = fileName;
    m_pModel = pModel;
    m_flag = flag;
    m_runMode = XlsModel;
    m_canRunning = true;
}

/**
 * @brief LogExportThread::exportToXlsPublic 导出到日志xlsx格式配置函数，对LOG_MSG_JOURNAL数据类型的重载（指系统日志和内核日志）
 * @param fileName 导出文件路径全称
 * @param jList 要导出的数据源 数据类型为LOG_MSG_JOURNAL
 * @param labels 表头字符串
 * @param flag  导出的日志类型
 */
void LogExportThread::exportToXlsPublic(QString fileName, QList<LOG_MSG_JOURNAL> jList, QStringList labels, LOG_FLAG iFlag)
{
    m_fileName = fileName;
    m_jList = jList;
    m_flag = iFlag;
    m_labels = labels;
    m_runMode = XlsJOURNAL;
    m_canRunning = true;
}

/**
 * @brief LogExportThread::exportToXlsPublic 导出到日志xlsx格式配置函数，对LOG_MSG_APPLICATOIN数据类型的重载（指应用日志）
 * @param fileName 导出文件路径全称
 * @param jList 要导出的数据源 数据类型为LOG_MSG_APPLICATOIN
 * @param labels 表头字符串
 * @param iAppName 导出的应用日志的应用名称
 */
void LogExportThread::exportToXlsPublic(QString fileName, QList<LOG_MSG_APPLICATOIN> jList, QStringList labels, QString &iAppName)
{
    m_fileName = fileName;
    m_appList = jList;
    m_labels = labels;
    m_runMode = XlsAPP;
    m_canRunning = true;
    m_appName = iAppName;
}

/**
 * @brief LogExportThread::exportToXlsPublic 导出到日志xlsx格式配置函数，对LOG_MSG_DPKG数据类型的重载（指dpkg日志）
 * @param fileName 导出文件路径全称
 * @param jList  要导出的数据源 LOG_MSG_DPKG
 * @param labels 表头字符串
 */
void LogExportThread::exportToXlsPublic(QString fileName, QList<LOG_MSG_DPKG> jList, QStringList labels)
{
    m_fileName = fileName;
    m_dpkgList = jList;
    m_labels = labels;
    m_runMode = XlsDPKG;
    m_canRunning = true;
}

/**
 * @brief LogExportThread::exportToXlsPublic导出到日志xlsx格式配置函数，对LOG_MSG_BOOT数据类型的重载（指启动日志）
 * @param fileName 导出文件路径全称
 * @param jList  要导出的数据源 LOG_MSG_DPKG
 * @param labels 表头字符串
 */
void LogExportThread::exportToXlsPublic(QString fileName, QList<LOG_MSG_BOOT> jList, QStringList labels)
{
    m_fileName = fileName;
    m_bootList = jList;
    m_labels = labels;
    m_runMode = XlsBOOT;
    m_canRunning = true;
}

/**
 * @brief LogExportThread::exportToXlsPublic导出到日志xlsx格式配置函数，对LOG_MSG_XORG数据类型的重载（指xorg日志）
 * @param fileName 导出文件路径全称
 * @param jList 要导出的数据源 LOG_MSG_XORG
 * @param labels 表头字符串
 */
void LogExportThread::exportToXlsPublic(QString fileName, QList<LOG_MSG_XORG> jList, QStringList labels)
{
    m_fileName = fileName;
    m_xorgList = jList;
    m_labels = labels;
    m_runMode = XlsXORG;
    m_canRunning = true;
}

/**
 * @brief LogExportThread::exportToXlsPublic 导出到日志xlsx格式配置函数，对LOG_MSG_NORMAL数据类型的重载（指开关机日志）
 * @param fileName 导出文件路径全称
 * @param jList 要导出的数据源 LOG_MSG_NORMAL
 * @param labels 表头字符串
 */
void LogExportThread::exportToXlsPublic(QString fileName, QList<LOG_MSG_NORMAL> jList, QStringList labels)
{
    m_fileName = fileName;
    m_normalList = jList;
    m_labels = labels;
    m_runMode = XlsNORMAL;
    m_canRunning = true;
}

/**
 * @brief LogExportThread::exportToXlsPublic 导出到日志xlsw格式配置函数，对LOG_MSG_KWIN数据类型的重载（指klu上的kwin日志）
 * @param fileName 导出文件路径全称
 * @param jList 要导出的数据源 LOG_MSG_KWIN
 * @param labels 表头字符串
 */
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

//    try {
//        DocxFactory::WordProcessingCompiler &l_compiler =
//            DocxFactory::WordProcessingCompiler::getInstance();
//        time_t l_start = clock();
//        l_compiler.compile(
//            "/home/zyc/Documents/work_space/works/oldlog/dde_log_viewer/3rdparty/DocxFactory/exercises/templates/test1.docx",
//            "/home/zyc/Documents/work_space/works/oldlog/dde_log_viewer/3rdparty/DocxFactory/exercises/templates/test1.dfw");



//        DocxFactory:: WordProcessingMerger &l_merger =
//            DocxFactory:: WordProcessingMerger::getInstance();
//        l_merger.load(
//            "/home/zyc/Documents/work_space/works/oldlog/dde_log_viewer/3rdparty/DocxFactory/exercises/templates/test1.dfw");


//        for (int i = 0; i < 1000; i++) {
//            for (int col = 1; col <= 8; ++col) {
//                l_merger.setClipboardValue("tableRow", QString("column%1").arg(col).toStdString(), "testDocxtestDocxtestDocxtestDocxtestDocxtestDocxtestDocxtestDocx");
//            }

//            l_merger.paste("tableRow");
//        }



//        l_merger.save("/home/zyc/Desktop/docfactory_benchmark.docx");


//        qDebug()  << "Completed (in "
//                  << (double)(clock() - l_start) / CLOCKS_PER_SEC
//                  << " seconds)."
//                  ;
//    } catch (const std::exception &p_exception) {
//        qDebug() << p_exception.what() ;
//    }
}


/**
 * @brief LogExportThread::isProcessing 返回当前线程获取数据逻辑启动停止控制的变量
 * @return 当前线程获取数据逻辑启动停止控制的变量
 */
bool LogExportThread::isProcessing()
{
    return m_canRunning;
}

/**
 * @brief LogExportThread::stopImmediately 停止前线程获取数据逻辑
 */
void LogExportThread::stopImmediately()
{
    m_canRunning = false;
}

/**
 * @brief LogExportThread::exportToTxt 导出数据到txt格式函数对QStandardItemModel数据类型的重载
 * @param fileName 导出文件路径全称
 * @param pModel 要导出的数据源，为QStandardItemModel
 * @param flag 导出的日志类型
 * @return 是否导出成功
 */
bool LogExportThread::exportToTxt(QString fileName, QStandardItemModel *pModel, LOG_FLAG flag)
{
    QFile fi(fileName);
    //判断文件路径是否存在，不存在就返回错误
    if (!fi.open(QIODevice::WriteOnly)) {
        emit sigResult(false);
        emit sigError(openErroStr);
        return false;
    }
    try {
        //判读啊model指针是为空，为空则不导出
        if (!pModel) {
            throw  QString("model is null");
        }
        QTextStream out(&fi);
        //日志类型为应用日志时
        if (flag == APP) {
            for (int row = 0; row < pModel->rowCount(); ++row) {
                //导出逻辑启动停止控制，外部把m_canRunning置false时停止运行，抛出异常处理
                if (!m_canRunning) {
                    throw  QString(stopStr);
                }
                //获取表头项目和对应内容，拼成目标字符串写入文件
                out << pModel->horizontalHeaderItem(0)->text() << ": "
                    << pModel->item(row, 0)->data(Qt::UserRole + 6).toString() << " ";
                for (int col = 1; col < pModel->columnCount(); ++col) {
                    out << pModel->horizontalHeaderItem(col)->text() << ": "
                        << pModel->item(row, col)->text() << " ";
                }
                out << "\n";
                //导出进度信号
                sigProgress(row + 1, pModel->rowCount());
            }
        } else {
            //日志类型为其他所有日志时
            for (int row = 0; row < pModel->rowCount(); row++) {
                //导出逻辑启动停止控制，外部把m_canRunning置false时停止运行，抛出异常处理
                if (!m_canRunning) {
                    throw  QString(stopStr);
                }
                //获取表头项目和对应内容，拼成目标字符串写入文件
                for (int col = 0; col < pModel->columnCount(); col++) {
                    out << pModel->horizontalHeaderItem(col)->text() << ": "
                        << pModel->item(row, col)->text() << " ";
                }
                out << "\n";
                //导出进度信号
                sigProgress(row + 1, pModel->rowCount());
            }
        }
    } catch (QString ErrorStr) {
        //捕获到异常，导出失败，发出失败信号
        qDebug() << "Export Stop" << ErrorStr;
        fi.close();
        emit sigResult(false);
        if (ErrorStr != stopStr) {
            emit sigError(QString("export error: %1").arg(ErrorStr));
        }
        return false;
    }
    fi.close();
    //导出成功，如果此时被停止，则发出导出失败信号
    emit sigResult(m_canRunning);
    return true && m_canRunning;
}

/**
 * @brief LogExportThread::exportToTxt导出到日志txt格式函数，对LOG_MSG_JOURNAL数据类型的重载（指系统日志和内核日志）
 * @param fileName 导出文件路径全称
 * @param jList 要导出的数据源 数据类型为LOG_MSG_JOURNAL
 * @param labels 表头字符串
 * @param flag  导出的日志类型
 * @return 是否导出成功
 */
bool LogExportThread::exportToTxt(QString fileName, QList<LOG_MSG_JOURNAL> jList,  QStringList labels, LOG_FLAG flag)
{
    //判断文件路径是否存在，不存在就返回错误
    QFile fi(fileName);
    if (!fi.open(QIODevice::WriteOnly)) {
        emit sigResult(false);
        emit sigError(openErroStr);
        return false;
    }
    try {

        QTextStream out(&fi);
        //导出日志为系统日志时
        if (flag == JOURNAL) {
            for (int i = 0; i < jList.count(); i++) {
                //导出逻辑启动停止控制，外部把m_canRunning置false时停止运行，抛出异常处理
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
                //导出各字段的描述和对应内容拼成目标字符串
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
                //导出进度信号
                sigProgress(i + 1, jList.count());
            }
        } else if (flag == KERN) {
            //导出日志为内核日志时
            for (int i = 0; i < jList.count(); i++) {
                //导出逻辑启动停止控制，外部把m_canRunning置false时停止运行，抛出异常处理
                if (!m_canRunning) {
                    throw  QString(stopStr);
                }
                int col = 0;
                LOG_MSG_JOURNAL jMsg = jList.at(i);
                //导出各字段的描述和对应内容拼成目标字符串
                out << labels.value(col++, "") << ":" << jMsg.dateTime << " ";
                out << labels.value(col++, "") << ":" << jMsg.hostName << " ";
                out << labels.value(col++, "") << ":" << jMsg.daemonName << " ";
                out << labels.value(col++, "") << ":" << jMsg.msg << " ";
                out << "\n";
                //导出进度信号
                sigProgress(i + 1, jList.count());
            }
        }
        //设置文件编码为utf8
        out.setCodec(QTextCodec::codecForName("utf-8"));

    } catch (QString ErrorStr) {
        //捕获到异常，导出失败，发出失败信号
        qDebug() << "Export Stop" << ErrorStr;
        fi.close();
        emit sigResult(false);
        if (ErrorStr != stopStr) {
            emit sigError(QString("export error: %1").arg(ErrorStr));
        }
        return false;
    }
    fi.close();
    //导出成功，如果此时被停止，则发出导出失败信号
    emit sigResult(m_canRunning);
    return true && m_canRunning;
}

/**
 * @brief LogExportThread::exportToTxt导出到日志txt格式函数，对LOG_MSG_APPLICATOIN数据类型的重载（指应用日志）
 * @param fileName 导出文件路径全称
 * @param jList 要导出的数据源 数据类型为LOG_MSG_APPLICATOIN
 * @param labels 表头字符串
 * @param iAppName 导出的应用日志的应用名称
 * @return 是否导出成功
 */
bool LogExportThread::exportToTxt(QString fileName, QList<LOG_MSG_APPLICATOIN> jList, QStringList labels, QString &iAppName)
{
    //判断文件路径是否存在，不存在就返回错误
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
            //导出逻辑启动停止控制，外部把m_canRunning置false时停止运行，抛出异常处理
            if (!m_canRunning) {
                throw  QString(stopStr);
            }
            int col = 0;
            //导出各字段的描述和对应内容拼成目标字符串
            LOG_MSG_APPLICATOIN jMsg = jList.at(i);
            out << labels.value(col++, "") << ":" << strTranslate(jMsg.level)  << " ";
            out << labels.value(col++, "") << ":" << jMsg.dateTime << " ";
            out << labels.value(col++, "") << ":" << iAppName << " ";
            out << labels.value(col++, "") << ":" << jMsg.msg << " ";
            out << "\n";
            //导出进度信号
            sigProgress(i + 1, jList.count());
        }
    } catch (QString ErrorStr) {
        //捕获到异常，导出失败，发出失败信号
        qDebug() << "Export Stop" << ErrorStr;
        fi.close();
        emit sigResult(false);
        if (ErrorStr != stopStr) {
            emit sigError(QString("export error: %1").arg(ErrorStr));
        }
        return false;
    }
    fi.close();
    //导出成功，如果此时被停止，则发出导出失败信号
    emit sigResult(m_canRunning);
    return true && m_canRunning;
}

/**
 * @brief LogExportThread::exportToTxt 导出到日志txt格式配置函数，对LOG_MSG_DPKG数据类型的重载（指dpkg日志）
 * @param fileName 导出文件路径全称
 * @param jList  要导出的数据源 LOG_MSG_DPKG
 * @param labels 表头字符串
 * @return 是否导出成功
 */
bool LogExportThread::exportToTxt(QString fileName, QList<LOG_MSG_DPKG> jList, QStringList labels)
{
    //判断文件路径是否存在，不存在就返回错误
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
            //导出逻辑启动停止控制，外部把m_canRunning置false时停止运行，抛出异常处理
            if (!m_canRunning) {
                throw  QString(stopStr);
            }
            int col = 0;
            //导出各字段的描述和对应内容拼成目标字符串
            LOG_MSG_DPKG jMsg = jList.at(i);
            out << labels.value(col++, "") << ":" << jMsg.dateTime << " ";
            out << labels.value(col++, "") << ":" << jMsg.msg << " ";
            out << labels.value(col++, "") << ":" << jMsg.action << " ";
            out << "\n";
            //导出进度信号
            sigProgress(i + 1, jList.count());
        }
    } catch (QString ErrorStr) {
        //捕获到异常，导出失败，发出失败信号
        qDebug() << "Export Stop" << ErrorStr;
        fi.close();
        emit sigResult(false);
        if (ErrorStr != stopStr) {
            emit sigError(QString("export error: %1").arg(ErrorStr));
        }
        return false;
    }
    fi.close();
    //导出成功，如果此时被停止，则发出导出失败信号
    emit sigResult(m_canRunning);
    return true && m_canRunning;
}

/**
 * @brief LogExportThread::exportToTxt 导出到日志txt格式函数，对LOG_MSG_BOOT数据类型的重载（指启动日志）
 * @param fileName 导出文件路径全称
 * @param jList  要导出的数据源 LOG_MSG_BOOT
 * @param labels 表头字符串
 * @return 是否导出成功
 */
bool LogExportThread::exportToTxt(QString fileName, QList<LOG_MSG_BOOT> jList, QStringList labels)
{
    //判断文件路径是否存在，不存在就返回错误
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
            //导出逻辑启动停止控制，外部把m_canRunning置false时停止运行，抛出异常处理
            if (!m_canRunning) {
                throw  QString(stopStr);
            }
            //导出各字段的描述和对应内容拼成目标字符串
            LOG_MSG_BOOT jMsg = jList.at(i);
            int col = 0;
            out << labels.value(col++, "") << ":" << jMsg.status << " ";
            out << labels.value(col++, "") << ":" << jMsg.msg << " ";
            out << "\n";
            //导出进度信号
            sigProgress(i + 1, jList.count());
        }
    } catch (QString ErrorStr) {
        //捕获到异常，导出失败，发出失败信号
        qDebug() << "Export Stop" << ErrorStr;
        fi.close();
        emit sigResult(false);
        if (ErrorStr != stopStr) {
            emit sigError(QString("export error: %1").arg(ErrorStr));
        }
        return false;
    }
    fi.close();
    //导出成功，如果此时被停止，则发出导出失败信号
    emit sigResult(m_canRunning);
    return true && m_canRunning;
}

/**
 * @brief LogExportThread::exportToTxt 导出到日志txt格式函数，对LOG_MSG_XORG数据类型的重载（指xorg日志）
 * @param fileName 导出文件路径全称
 * @param jList 要导出的数据源 LOG_MSG_XORG
 * @param labels 表头字符串
 * @return 是否导出成功
 */
bool LogExportThread::exportToTxt(QString fileName, QList<LOG_MSG_XORG> jList, QStringList labels)
{
    //判断文件路径是否存在，不存在就返回错误
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
            //导出逻辑启动停止控制，外部把m_canRunning置false时停止运行，抛出异常处理
            if (!m_canRunning) {
                throw  QString(stopStr);
            }
            //导出各字段的描述和对应内容拼成目标字符串
            LOG_MSG_XORG jMsg = jList.at(i);
            int col = 0;
            out << labels.value(col++, "") << ":" << jMsg.dateTime << " ";
            out << labels.value(col++, "") << ":" << jMsg.msg << " ";
            out << "\n";
            //导出进度信号
            sigProgress(i + 1, jList.count());
        }
    } catch (QString ErrorStr) {
        //捕获到异常，导出失败，发出失败信号
        qDebug() << "Export Stop" << ErrorStr;
        fi.close();
        emit sigResult(false);
        if (ErrorStr != stopStr) {
            emit sigError(QString("export error: %1").arg(ErrorStr));
        }
        return false;
    }
    fi.close();
    //导出成功，如果此时被停止，则发出导出失败信号
    emit sigResult(m_canRunning);
    return true && m_canRunning;
}

/**
 * @brief LogExportThread::exportToTxt 导出到日志txt格式函数，对LOG_MSG_NORMAL数据类型的重载（指开关机日志）
 * @param fileName 导出文件路径全称
 * @param jList 要导出的数据源 LOG_MSG_NORMAL
 * @param labels 表头字符串
 * @return 是否导出成功
 */
bool LogExportThread::exportToTxt(QString fileName, QList<LOG_MSG_NORMAL> jList, QStringList labels)
{
    //判断文件路径是否存在，不存在就返回错误
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
            //导出逻辑启动停止控制，外部把m_canRunning置false时停止运行，抛出异常处理
            if (!m_canRunning) {
                throw  QString(stopStr);
            }
            //导出各字段的描述和对应内容拼成目标字符串
            LOG_MSG_NORMAL jMsg = jList.at(i);
            int col = 0;
            out << labels.value(col++, "") << ":" << jMsg.eventType << " ";
            out << labels.value(col++, "") << ":" << jMsg.userName << " ";
            out << labels.value(col++, "") << ":" << jMsg.dateTime << " ";
            out << labels.value(col++, "") << ":" << jMsg.msg << " ";
            out << "\n";
            //导出进度信号
            sigProgress(i + 1, jList.count());
        }
    } catch (QString ErrorStr) {
        //捕获到异常，导出失败，发出失败信号
        qDebug() << "Export Stop" << ErrorStr;
        fi.close();
        emit sigResult(false);
        if (ErrorStr != stopStr) {
            emit sigError(QString("export error: %1").arg(ErrorStr));
        }
        return false;
    }
    fi.close();
    //导出成功，如果此时被停止，则发出导出失败信号
    emit sigResult(m_canRunning);
    return true && m_canRunning;
}
/**
 * @brief LogExportThread::exportToTxt导出到日志txt格式函数，对LOG_MSG_KWIN数据类型的重载（指klu上的kwin日志）
 * @param fileName 导出文件路径全称
 * @param jList 要导出的数据源 LOG_MSG_KWIN
 * @param labels 表头字符串
 * @return 是否导出成功
 */
bool LogExportThread::exportToTxt(QString fileName, QList<LOG_MSG_KWIN> jList, QStringList labels)
{
    //判断文件路径是否存在，不存在就返回错误
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
            //导出逻辑启动停止控制，外部把m_canRunning置false时停止运行，抛出异常处理
            if (!m_canRunning) {
                throw  QString(stopStr);
            }
            //导出各字段的描述和对应内容拼成目标字符串
            LOG_MSG_KWIN jMsg = jList.at(i);
            int col = 0;
            out << labels.value(col++, "") << ":" << jMsg.msg << " ";
            out << "\n";
            //导出进度信号
            sigProgress(i + 1, jList.count());
        }
    } catch (QString ErrorStr) {
        //捕获到异常，导出失败，发出失败信号
        qDebug() << "Export Stop" << ErrorStr;
        fi.close();
        emit sigResult(false);
        if (ErrorStr != stopStr) {
            emit sigError(QString("export error: %1").arg(ErrorStr));
        }
        return false;
    }
    fi.close();
    //导出成功，如果此时被停止，则发出导出失败信号
    emit sigResult(m_canRunning);
    return true && m_canRunning;
}

/**
 * @brief LogExportThread::exportToDoc导出到日志doc格式函数对QStandardItemModel数据类型的重载
 * @param fileName 导出文件路径全称
 * @param pModel 要导出的数据源，为QStandardItemModel
 * @param flag 导出的日志类型
 * @return 是否导出成功
 */

bool LogExportThread::exportToDoc(QString fileName, QStandardItemModel *pModel, LOG_FLAG flag)
{
#if 1
    try {
        //判读啊model指针是为空，为空则不导出
        if (!pModel) {
            throw  QString("model is null");
        }
        //声明Doc对象,参数为模板文件路径
        Docx::Document doc(DOCTEMPLATE);
        //添加一个行数为数据的条数+1,列数等于数据字段数的表格
        Docx::Table *tab = doc.addTable(pModel->rowCount() + 1, pModel->columnCount());
        //设置表格内文字内容向左对齐
        tab->setAlignment(Docx::WD_TABLE_ALIGNMENT::LEFT);
        //往表头中添加表头描述，表头为第一行，数据则在下面
        for (int col = 0; col < pModel->columnCount(); ++col) {
            auto item = pModel->horizontalHeaderItem(col);
            auto cel = tab->cell(0, col);
            if (item) {
                cel->addText(pModel->horizontalHeaderItem(col)->text());
            }
        }
        //计算导出进度条最后一段的长度，因为最后写入文件那一段没有进度，所以预先留出一段进度
        int end = static_cast<int>(pModel->rowCount() * 0.1 > 5 ? pModel->rowCount() * 0.1 : 5);
        //往表格的单元格中添加数据内容
        //日志类型为应用日志时
        if (flag == APP) {
            for (int row = 0; row < pModel->rowCount(); ++row) {
                //导出逻辑启动停止控制，外部把m_canRunning置false时停止运行，抛出异常处理
                if (!m_canRunning) {
                    throw  QString(stopStr);
                }
                //把数据填入表格单元格中
                auto cel = tab->cell(row + 1, 0);
                cel->addText(pModel->item(row, 0)->data(Qt::UserRole + 6).toString());
                for (int col = 1; col < pModel->columnCount(); ++col) {
                    if (!m_canRunning) {
                        throw  QString(stopStr);
                    }
                    auto cel = tab->cell(row + 1, col);
                    cel->addText(pModel->item(row, col)->text());
                }
                //导出进度信号
                sigProgress(row + 1, pModel->rowCount() + end);
            }
        } else {
            for (int row = 0; row < pModel->rowCount(); ++row) {
                //导出逻辑启动停止控制，外部把m_canRunning置false时停止运行，抛出异常处理
                if (!m_canRunning) {
                    throw  QString(stopStr);
                }
                //把数据填入表格单元格中
                for (int col = 0; col < pModel->columnCount(); ++col) {
                    if (!m_canRunning) {
                        throw  QString(stopStr);
                    }
                    auto cel = tab->cell(row + 1, col);
                    cel->addText(pModel->item(row, col)->text());
                }
                //导出进度信号
                sigProgress(row + 1, pModel->rowCount() + end);
            }
        }
        //保存，把拼好的xml写入文件中
        doc.save(fileName);
    } catch (QString ErrorStr) {
        //捕获到异常，导出失败，发出失败信号
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
    //如果取消导出，则删除文件
    if (!m_canRunning) {
        Utils::checkAndDeleteDir(m_fileName);
    }
    //100%进度
    sigProgress(100, 100);
    //延时200ms再发送导出成功信号，关闭导出进度框，让100%的进度有时间显示
    Utils::sleep(200);
    //导出成功，如果此时被停止，则发出导出失败信号
    emit sigResult(m_canRunning);
    return true && m_canRunning;
}

/**
 * @brief LogExportThread::exportToDoc导出到日志doc格式函数，对LOG_MSG_JOURNAL数据类型的重载（指系统日志和内核日志）
 * @param fileName 导出文件路径全称
 * @param jList 要导出的数据源 数据类型为LOG_MSG_JOURNAL
 * @param labels 表头字符串
 * @param flag  导出的日志类型
 * @return 是否导出成功
 */
bool LogExportThread::exportToDoc(QString fileName, QList<LOG_MSG_JOURNAL> jList,
                                  QStringList labels, LOG_FLAG iFlag)
{
#if 1
    try {
        //声明Doc对象,参数为模板文件路径
        Docx::Document doc(DOCTEMPLATE);
        Docx::Table *tab;
        //添加一个行数为数据的条数+1,列数等于数据字段数的表格
        if (iFlag == JOURNAL) {
            tab = doc.addTable(jList.count() + 1, 6);
        } else if (iFlag == KERN) {
            tab = doc.addTable(jList.count() + 1, 4);
        }

        //设置表格内文字内容向左对齐
        tab->setAlignment(Docx::WD_TABLE_ALIGNMENT::LEFT);

        //往表头中添加表头描述，表头为第一行，数据则在下面
        for (int col = 0; col < labels.count(); ++col) {
            auto cel = tab->cell(0, col);
            cel->addText(labels.at(col));
        }
        //计算导出进度条最后一段的长度，因为最后写入文件那一段没有进度，所以预先留出一段进度
        int end = static_cast<int>(jList.count() * 0.1 > 5 ? jList.count() * 0.1 : 5);
        //往表格的单元格中添加数据内容
        for (int row = 0; row < jList.count(); ++row) {
            //导出逻辑启动停止控制，外部把m_canRunning置false时停止运行，抛出异常处理
            if (!m_canRunning) {
                throw  QString(stopStr);
            }
            LOG_MSG_JOURNAL message = jList.at(row);
            //把数据填入表格单元格中
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
            //导出进度信号
            sigProgress(row + 1, jList.count() + end);
        }

        //保存，把拼好的xml写入文件中
        doc.save(fileName);
    } catch (QString ErrorStr) {
        //捕获到异常，导出失败，发出失败信号
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
    //如果取消导出，则删除文件
    if (!m_canRunning) {
        Utils::checkAndDeleteDir(m_fileName);
    }
    //100%进度
    sigProgress(100, 100);
    //延时200ms再发送导出成功信号，关闭导出进度框，让100%的进度有时间显示
    Utils::sleep(200);
    //导出成功，如果此时被停止，则发出导出失败信号
    emit sigResult(m_canRunning);
    return true && m_canRunning;
}

/**
 * @brief LogExportThread::exportToDoc导出到日志doc格式配置函数，对LOG_MSG_APPLICATOIN数据类型的重载（指应用日志）
 * @param fileName 导出文件路径全称
 * @param jList 要导出的数据源 数据类型为LOG_MSG_APPLICATOIN
 * @param labels 表头字符串
 * @param iAppName 导出的应用日志的应用名称
 * @return 是否导出成功
 */
bool LogExportThread::exportToDoc(QString fileName, QList<LOG_MSG_APPLICATOIN> jList, QStringList labels, QString &iAppName)
{
    QElapsedTimer timer;
    timer.start();
    try {
        //声明Doc对象,参数为模板文件路径
        Docx::Document doc(DOCTEMPLATE);
        //添加一个行数为数据的条数+1,列数等于数据字段数的表格
        Docx::Table *tab = doc.addTable(jList.count() + 1, 4);
        //设置表格内文字内容向左对齐
        tab->setAlignment(Docx::WD_TABLE_ALIGNMENT::LEFT);

        //往表头中添加表头描述，表头为第一行，数据则在下面
        for (int col = 0; col < labels.count(); ++col) {
            auto cel = tab->cell(0, col);
            cel->addText(labels.at(col));
        }
        //计算导出进度条最后一段的长度，因为最后写入文件那一段没有进度，所以预先留出一段进度
        int end = static_cast<int>(jList.count() * 0.1 > 5 ? jList.count() * 0.1 : 5);
        for (int row = 0; row < jList.count(); ++row) {
            //导出逻辑启动停止控制，外部把m_canRunning置false时停止运行，抛出异常处理
            if (!m_canRunning) {
                throw  QString(stopStr);
            }
            LOG_MSG_APPLICATOIN message = jList.at(row);
            int col = 0;
            //把数据填入表格单元格中
            tab->cell(row + 1, col++)->addText(strTranslate(message.level));
            tab->cell(row + 1, col++)->addText(message.dateTime);
            tab->cell(row + 1, col++)->addText(iAppName);
            tab->cell(row + 1, col++)->addText(message.msg);
            //导出进度信号
            sigProgress(row + 1, jList.count() + end);
        }


        //保存，把拼好的xml写入文件中
        doc.save(fileName);

    } catch (QString ErrorStr) {
        //捕获到异常，导出失败，发出失败信号
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

    //如果取消导出，则删除文件
    if (!m_canRunning) {
        Utils::checkAndDeleteDir(m_fileName);
    }

    //100%进度
    sigProgress(100, 100);
    //延时200ms再发送导出成功信号，关闭导出进度框，让100%的进度有时间显示
    Utils::sleep(200);
    //导出成功，如果此时被停止，则发出导出失败信号
    emit sigResult(m_canRunning);

    qDebug() << "exportdoc benchmark rows:" << jList.count() << "time:" << timer.elapsed();
    return true && m_canRunning;
}


bool LogExportThread::exportToDocNew(QString fileName, QList<LOG_MSG_APPLICATOIN> jList, QStringList labels, QString &iAppName)
{
//    QElapsedTimer timer;
//    timer.start();
//    try {
//        DocxFactory::WordProcessingCompiler &l_compiler =
//            DocxFactory::WordProcessingCompiler::getInstance();
//        l_compiler.compile(
//            "~/test_app.docx",
//            "~/test_app.dfw");



//        DocxFactory:: WordProcessingMerger &l_merger =
//            DocxFactory:: WordProcessingMerger::getInstance();
//        l_merger.load(
//            "~/test_app.dfw");
//        for (int col = 0; col < labels.count(); ++col) {
//            l_merger.setClipboardValue("tableRow", QString("column%1").arg(col + 1).toStdString(), labels.at(col).toStdString());
//        }

//        l_merger.paste("tableRow");
//        for (int row = 0; row < 100000; ++row)  {
//            if (!m_canRunning) {
//                throw  QString(stopStr);
//            }
//            LOG_MSG_APPLICATOIN message = jList.at(100);
//            l_merger.setClipboardValue("tableRow", QString("column1").toStdString(), strTranslate(message.level).toStdString());
//            l_merger.setClipboardValue("tableRow", QString("column2").toStdString(), message.dateTime.toStdString());
//            l_merger.setClipboardValue("tableRow", QString("column3").toStdString(), iAppName.toStdString());
//            l_merger.setClipboardValue("tableRow", QString("column4").toStdString(), message.msg.toStdString());
////            l_merger.setClipboardValue("tableRow", QString("column5").toStdString(), strTranslate(message.level).toStdString());
////            l_merger.setClipboardValue("tableRow", QString("column6").toStdString(), message.dateTime.toStdString());
////            l_merger.setClipboardValue("tableRow", QString("column7").toStdString(), iAppName.toStdString());
////            l_merger.setClipboardValue("tableRow", QString("column8").toStdString(), message.msg.toStdString());
//            l_merger.paste("tableRow");
//            sigProgress(row + 1, jList.count());
//        }


//        fileName = fileName + "x";
//        l_merger.save(fileName.toStdString());



//        sigProgress(100, 100);
//        emit sigResult(m_canRunning);
//        qDebug() << "exportdoc New benchmark rows:" << jList.count() << "time:" << timer.elapsed();
//        return  true;
//    } catch (const std::exception &p_exception) {
//        qDebug() << p_exception.what() ;
//        emit sigResult(false);
//        return  false;
//    }
    return true;

}


/**
 * @brief LogExportThread::exportToDoc导出到日志doc格式函数，对LOG_MSG_DPKG数据类型的重载（指dpkg日志）
 * @param fileName 导出文件路径全称
 * @param jList  要导出的数据源 LOG_MSG_DPKG
 * @param labels 表头字符串
 * @return 是否导出成功
 */
bool LogExportThread::exportToDoc(QString fileName, QList<LOG_MSG_DPKG> jList, QStringList labels)
{
    try {
        //声明Doc对象,参数为模板文件路径
        Docx::Document doc(DOCTEMPLATE);
        //添加一个行数为数据的条数+1,列数等于数据字段数的表格
        Docx::Table *tab = doc.addTable(jList.count() + 1, 3);
        //设置表格内文字内容向左对齐
        tab->setAlignment(Docx::WD_TABLE_ALIGNMENT::LEFT);

        //往表头中添加表头描述，表头为第一行，数据则在下面
        for (int col = 0; col < labels.count(); ++col) {
            auto cel = tab->cell(0, col);
            cel->addText(labels.at(col));
        }
        //计算导出进度条最后一段的长度，因为最后写入文件那一段没有进度，所以预先留出一段进度
        int end = static_cast<int>(jList.count() * 0.1 > 5 ? jList.count() * 0.1 : 5);

        for (int row = 0; row < jList.count(); ++row) {
            //导出逻辑启动停止控制，外部把m_canRunning置false时停止运行，抛出异常处理
            if (!m_canRunning) {
                throw  QString(stopStr);
            }
            LOG_MSG_DPKG message = jList.at(row);
            int col = 0;
            //把数据填入表格单元格中
            tab->cell(row + 1, col++)->addText(message.dateTime);
            tab->cell(row + 1, col++)->addText(message.msg);
            tab->cell(row + 1, col++)->addText(message.action);
            //导出进度信号
            sigProgress(row + 1, jList.count() + end);
        }


        //保存，把拼好的xml写入文件中
        doc.save(fileName);

    } catch (QString ErrorStr) {
        //捕获到异常，导出失败，发出失败信号
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
    //如果取消导出，则删除文件
    if (!m_canRunning) {
        Utils::checkAndDeleteDir(m_fileName);
    }
    //100%进度
    sigProgress(100, 100);
    //延时200ms再发送导出成功信号，关闭导出进度框，让100%的进度有时间显示
    Utils::sleep(200);
    //导出成功，如果此时被停止，则发出导出失败信号
    emit sigResult(m_canRunning);
    return true && m_canRunning;
}

/**
 * @brief LogExportThread::exportToDoc导出到日志doc格式配置函数，对LOG_MSG_BOOT数据类型的重载（指启动日志）
 * @param fileName 导出文件路径全称
 * @param jList  要导出的数据源 LOG_MSG_BOOT
 * @param labels 表头字符串
 * @return 是否导出成功
 */
bool LogExportThread::exportToDoc(QString fileName, QList<LOG_MSG_BOOT> jList, QStringList labels)
{
    try {
        //声明Doc对象,参数为模板文件路径
        Docx::Document doc(DOCTEMPLATE);
        //添加一个行数为数据的条数+1,列数等于数据字段数的表格
        Docx::Table *tab = doc.addTable(jList.count() + 1, 2);
        //设置表格内文字内容向左对齐
        tab->setAlignment(Docx::WD_TABLE_ALIGNMENT::LEFT);
        //计算导出进度条最后一段的长度，因为最后写入文件那一段没有进度，所以预先留出一段进度
        int end = static_cast<int>(jList.count() * 0.1 > 5 ? jList.count() * 0.1 : 5);

        //往表头中添加表头描述，表头为第一行，数据则在下面
        for (int col = 0; col < labels.count(); ++col) {
            auto cel = tab->cell(0, col);
            cel->addText(labels.at(col));
        }

        for (int row = 0; row < jList.count(); ++row) {
            //导出逻辑启动停止控制，外部把m_canRunning置false时停止运行，抛出异常处理
            if (!m_canRunning) {
                throw  QString(stopStr);
            }
            LOG_MSG_BOOT message = jList.at(row);
            int col = 0;
            //把数据填入表格单元格中
            tab->cell(row + 1, col++)->addText(message.status);
            tab->cell(row + 1, col++)->addText(message.msg);
            //导出进度信号
            sigProgress(row + 1, jList.count() + end);
        }
        //保存，把拼好的xml写入文件中
        doc.save(fileName);

    } catch (QString ErrorStr) {
        //捕获到异常，导出失败，发出失败信号
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
    //如果取消导出，则删除文件
    if (!m_canRunning) {
        Utils::checkAndDeleteDir(m_fileName);
    }
    //100%进度
    sigProgress(100, 100);
    //延时200ms再发送导出成功信号，关闭导出进度框，让100%的进度有时间显示
    Utils::sleep(200);
    //导出成功，如果此时被停止，则发出导出失败信号
    emit sigResult(m_canRunning);
    return true && m_canRunning;
}

/**
 * @brief LogExportThread::exportToDoc导出到日志doc格式函数，对LOG_MSG_XORG数据类型的重载（指xorg日志）
 * @param fileName 导出文件路径全称
 * @param jList 要导出的数据源 LOG_MSG_XORG
 * @param labels 表头字符串
 * @return 是否导出成功
 */
bool LogExportThread::exportToDoc(QString fileName, QList<LOG_MSG_XORG> jList, QStringList labels)
{
    try {
        //声明Doc对象,参数为模板文件路径
        Docx::Document doc(DOCTEMPLATE);
        //添加一个行数为数据的条数+1,列数等于数据字段数的表格
        Docx::Table *tab = doc.addTable(jList.count() + 1, 2);
        //设置表格内文字内容向左对齐
        tab->setAlignment(Docx::WD_TABLE_ALIGNMENT::LEFT);

        //往表头中添加表头描述，表头为第一行，数据则在下面
        for (int col = 0; col < labels.count(); ++col) {
            auto cel = tab->cell(0, col);
            cel->addText(labels.at(col));
        }
        //计算导出进度条最后一段的长度，因为最后写入文件那一段没有进度，所以预先留出一段进度
        int end = static_cast<int>(jList.count() * 0.1 > 5 ? jList.count() * 0.1 : 5);
        for (int row = 0; row < jList.count(); ++row) {
            //导出逻辑启动停止控制，外部把m_canRunning置false时停止运行，抛出异常处理
            if (!m_canRunning) {
                throw  QString(stopStr);
            }
            LOG_MSG_XORG message = jList.at(row);
            int col = 0;
            //把数据填入表格单元格中
            tab->cell(row + 1, col++)->addText(message.dateTime);
            tab->cell(row + 1, col++)->addText(message.msg);
            //导出进度信号
            sigProgress(row + 1, jList.count() + end);
        }


        //保存，把拼好的xml写入文件中
        doc.save(fileName);

    } catch (QString ErrorStr) {
        //捕获到异常，导出失败，发出失败信号
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
    //如果取消导出，则删除文件
    if (!m_canRunning) {
        Utils::checkAndDeleteDir(m_fileName);
    }
    //100%进度
    sigProgress(100, 100);
    //延时200ms再发送导出成功信号，关闭导出进度框，让100%的进度有时间显示
    Utils::sleep(200);
    //导出成功，如果此时被停止，则发出导出失败信号
    emit sigResult(m_canRunning);

    return true && m_canRunning;
}

/**
 * @brief LogExportThread::exportToDoc 导出到日志doc格式函数，对LOG_MSG_NORMAL数据类型的重载（指开关机日志）
 * @param fileName 导出文件路径全称
 * @param jList 要导出的数据源 LOG_MSG_NORMAL
 * @param labels 表头字符串
 * @return 是否导出成功
 */
bool LogExportThread::exportToDoc(QString fileName, QList<LOG_MSG_NORMAL> jList, QStringList labels)
{
    try {
        //声明Doc对象,参数为模板文件路径
        Docx::Document doc(DOCTEMPLATE);
        //添加一个行数为数据的条数+1,列数等于数据字段数的表格
        Docx::Table *tab = doc.addTable(jList.count() + 1, 4);
        //设置表格内文字内容向左对齐
        tab->setAlignment(Docx::WD_TABLE_ALIGNMENT::LEFT);

        //往表头中添加表头描述，表头为第一行，数据则在下面
        for (int col = 0; col < labels.count(); ++col) {
            auto cel = tab->cell(0, col);
            cel->addText(labels.at(col));
        }
        //计算导出进度条最后一段的长度，因为最后写入文件那一段没有进度，所以预先留出一段进度
        int end = static_cast<int>(jList.count() * 0.1 > 5 ? jList.count() * 0.1 : 5);
        for (int row = 0; row < jList.count(); ++row) {
            //导出逻辑启动停止控制，外部把m_canRunning置false时停止运行，抛出异常处理
            if (!m_canRunning) {
                throw  QString(stopStr);
            }
            LOG_MSG_NORMAL message = jList.at(row);
            int col = 0;
            //把数据填入表格单元格中
            tab->cell(row + 1, col++)->addText(message.eventType);
            tab->cell(row + 1, col++)->addText(message.userName);
            tab->cell(row + 1, col++)->addText(message.dateTime);
            tab->cell(row + 1, col++)->addText(message.msg);
            //导出进度信号
            sigProgress(row + 1, jList.count() + end);
        }


        //保存，把拼好的xml写入文件中
        doc.save(fileName);

    } catch (QString ErrorStr) {
        //捕获到异常，导出失败，发出失败信号
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
    //如果取消导出，则删除文件
    if (!m_canRunning) {
        Utils::checkAndDeleteDir(m_fileName);
    }
    //100%进度
    sigProgress(100, 100);
    //延时200ms再发送导出成功信号，关闭导出进度框，让100%的进度有时间显示
    Utils::sleep(200);
    //导出成功，如果此时被停止，则发出导出失败信号
    emit sigResult(m_canRunning);
    return true && m_canRunning;
}

/**
 * @brief LogExportThread::exportToDoc导出到日志doc格式函数，对LOG_MSG_KWIN数据类型的重载（指klu上的kwin日志）
 * @param fileName 导出文件路径全称
 * @param jList 要导出的数据源 LOG_MSG_KWIN
 * @param labels 表头字符串
 * @return 是否导出成功
 */
bool LogExportThread::exportToDoc(QString fileName, QList<LOG_MSG_KWIN> jList, QStringList labels)
{
    try {
        //声明Doc对象,参数为模板文件路径
        Docx::Document doc(DOCTEMPLATE);
        //添加一个行数为数据的条数+1,列数等于数据字段数的表格
        Docx::Table *tab = doc.addTable(jList.count() + 1, 1);
        //设置表格内文字内容向左对齐
        tab->setAlignment(Docx::WD_TABLE_ALIGNMENT::LEFT);

        //往表头中添加表头描述，表头为第一行，数据则在下面
        for (int col = 0; col < labels.count(); ++col) {
            auto cel = tab->cell(0, col);
            cel->addText(labels.at(col));
        }
        //计算导出进度条最后一段的长度，因为最后写入文件那一段没有进度，所以预先留出一段进度
        int end = static_cast<int>(jList.count() * 0.1 > 5 ? jList.count() * 0.1 : 5);
        for (int row = 0; row < jList.count(); ++row) {
            //导出逻辑启动停止控制，外部把m_canRunning置false时停止运行，抛出异常处理
            if (!m_canRunning) {
                throw  QString(stopStr);
            }
            //把数据填入表格单元格中
            LOG_MSG_KWIN message = jList.at(row);
            int col = 0;
            tab->cell(row + 1, col++)->addText(message.msg);
            sigProgress(row + 1, jList.count() + end);
        }


        //保存，把拼好的xml写入文件中
        doc.save(fileName);

    } catch (QString ErrorStr) {
        //捕获到异常，导出失败，发出失败信号
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
    //如果取消导出，则删除文件
    if (!m_canRunning) {
        Utils::checkAndDeleteDir(m_fileName);
    }
    //100%进度
    sigProgress(100, 100);
    //延时200ms再发送导出成功信号，关闭导出进度框，让100%的进度有时间显示
    Utils::sleep(200);
    //导出成功，如果此时被停止，则发出导出失败信号
    emit sigResult(m_canRunning);
    return true && m_canRunning;
}




/**
 * @brief LogExportThread::exportToHtml导出到日志html格式函数对QStandardItemModel数据类型的重载
 * @param fileName 导出文件路径全称
 * @param pModel 要导出的数据源，为QStandardItemModel
 * @param flag 导出的日志类型
 * @return 是否导出成功
 */
bool LogExportThread::exportToHtml(QString fileName, QStandardItemModel *pModel, LOG_FLAG flag)
{
    QFile html(fileName);
    //判断文件路径是否存在，不存在就返回错误
    if (!html.open(QIODevice::WriteOnly)) {
        emit sigResult(false);
        emit sigError(openErroStr);
        return false;
    }
    try {
        //判读啊model指针是为空，为空则不导出
        if (!pModel) {
            throw  QString("model is null");
        }
        //写网页头
        html.write("<!DOCTYPE html>\n");
        html.write("<html>\n");
        html.write("<body>\n");
        //写入表格标签
        html.write("<table border=\"1\">\n");
        // 写入表头
        html.write("<tr>");
        for (int i = 0; i < pModel->columnCount(); ++i) {
            QString labelInfo = QString("<td>%1</td>").arg(pModel->horizontalHeaderItem(i)->text());
            html.write(labelInfo.toUtf8().data());
        }
        html.write("</tr>");
        // 写入内容
        //日志类型为应用日志时
        if (flag == APP) {
            for (int row = 0; row < pModel->rowCount(); ++row) {
                //导出逻辑启动停止控制，外部把m_canRunning置false时停止运行，抛出异常处理
                if (!m_canRunning) {
                    throw  QString(stopStr);
                }
                //根据字段拼出每行的网页内容
                html.write("<tr>");

                QString info =
                    QString("<td>%1</td>").arg(pModel->item(row, 0)->data(Qt::UserRole + 6).toString());
                html.write(info.toUtf8().data());

                for (int col = 1; col < pModel->columnCount(); ++col) {
                    QString info = QString("<td>%1</td>").arg(pModel->item(row, col)->text());
                    html.write(info.toUtf8().data());
                }
                html.write("</tr>");
                //导出进度信号
                sigProgress(row + 1, pModel->rowCount());
            }
        } else {
            //日志类型为其他所有日志时
            for (int row = 0; row < pModel->rowCount(); ++row) {
                //导出逻辑启动停止控制，外部把m_canRunning置false时停止运行，抛出异常处理
                if (!m_canRunning) {
                    throw  QString(stopStr);
                }
                //根据字段拼出每行的网页内容
                html.write("<tr>");
                for (int col = 0; col < pModel->columnCount(); ++col) {
                    QString info = QString("<td>%1</td>").arg(pModel->item(row, col)->text());
                    html.write(info.toUtf8().data());
                }
                html.write("</tr>");
                //导出进度信号
                sigProgress(row + 1, pModel->rowCount());
            }
        }
        html.write("</table>\n");
        html.write("</body>\n");
        html.write("</html>\n");
    } catch (QString ErrorStr) {
        //捕获到异常，导出失败，发出失败信号
        qDebug() << "Export Stop" << ErrorStr;
        html.close();
        emit sigResult(false);
        if (ErrorStr != stopStr) {
            emit sigError(QString("export error: %1").arg(ErrorStr));
        }
        return false;
    }
    html.close();
    //导出成功，如果此时被停止，则发出导出失败信号
    emit sigResult(m_canRunning);
    return true && m_canRunning;
}

/**
 * @brief LogExportThread::exportToHtml 导出到日志html格式函数，对LOG_MSG_JOURNAL数据类型的重载（指系统日志和内核日志）
 * @param fileName 导出文件路径全称
 * @param jList 要导出的数据源 数据类型为LOG_MSG_JOURNAL
 * @param labels 表头字符串
 * @param flag  导出的日志类型
 * @return 是否导出成功
 */
bool LogExportThread::exportToHtml(QString fileName, QList<LOG_MSG_JOURNAL> jList,  QStringList labels, LOG_FLAG flag)
{
    QFile html(fileName);
    //判断文件路径是否存在，不存在就返回错误
    if (!html.open(QIODevice::WriteOnly)) {
        emit sigResult(false);
        emit sigError(openErroStr);
        return false;
    }
    try {

        //写网页头
        html.write("<!DOCTYPE html>\n");
        html.write("<html>\n");
        html.write("<body>\n");
        //写入表格标签
        html.write("<table border=\"1\">\n");
        // 写入内容
        //日志类型为系统日志时
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
            // 写入表头
            QString title = QString("<tr><td>") + QString(DApplication::translate("Table", "Level")) +
                            QString("</td><td>") + QString(DApplication::translate("Table", "Process")) +
                            QString("</td><td>") +
                            QString(DApplication::translate("Table", "Date and Time")) +
                            QString("</td><td>") + QString(DApplication::translate("Table", "Info")) +
                            QString("</td><td>") + QString(DApplication::translate("Table", "User")) +
                            QString("</td><td>") + QString(DApplication::translate("Table", "PID")) +
                            QString("</td></tr>");
            html.write(title.toUtf8().data());
            // 写入内容
            for (int i = 0; i < jList.count(); i++) {
                //导出逻辑启动停止控制，外部把m_canRunning置false时停止运行，抛出异常处理
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
                //根据字段拼出每行的网页内容
                QString info =
                    QString("<tr><td>%1</td><td>%2</td><td>%3</td><td>%4</td><td>%5</td><td>%6</td></tr>")
                    .arg(jMsg.level)
                    .arg(jMsg.daemonName)
                    .arg(jMsg.dateTime)
                    .arg(jMsg.msg)
                    .arg(jMsg.hostName)
                    .arg(jMsg.daemonId);
                html.write(info.toUtf8().data());
                //导出进度信号
                sigProgress(i + 1, jList.count());

            }
        } else if (flag == KERN) {
            //日志类型为内核日志时
            // 写入表头
            html.write("<tr>");
            for (int i = 0; i < labels.count(); ++i) {
                QString labelInfo = QString("<td>%1</td>").arg(labels.value(i));
                html.write(labelInfo.toUtf8().data());
            }
            //根据字段拼出每行的网页内容
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
        //捕获到异常，导出失败，发出失败信号
        qDebug() << "Export Stop" << ErrorStr;
        html.close();
        emit sigResult(false);
        if (ErrorStr != stopStr) {
            emit sigError(QString("export error: %1").arg(ErrorStr));
        }
        return false;
    }
    html.close();
    //导出成功，如果此时被停止，则发出导出失败信号
    emit sigResult(m_canRunning);
    return true && m_canRunning;
}

/**
 * @brief LogExportThread::exportToHtml导出到日志html格式函数，对LOG_MSG_APPLICATOIN数据类型的重载（指应用日志）
 * @param fileName 导出文件路径全称
 * @param jList 要导出的数据源 数据类型为LOG_MSG_APPLICATOIN
 * @param labels 表头字符串
 * @param iAppName 导出的应用日志的应用名称
 * @return 是否导出成功
 */
bool LogExportThread::exportToHtml(QString fileName, QList<LOG_MSG_APPLICATOIN> jList, QStringList labels, QString &iAppName)
{
    //判断文件路径是否存在，不存在就返回错误
    QFile html(fileName);
    if (!html.open(QIODevice::WriteOnly)) {
        emit sigResult(false);
        emit sigError(openErroStr);
        return false;
    }
    try {
        //写网页头
        html.write("<!DOCTYPE html>\n");
        html.write("<html>\n");
        html.write("<body>\n");
        //写入表格标签
        html.write("<table border=\"1\">\n");
        // 写入表头
        html.write("<tr>");
        for (int i = 0; i < labels.count(); ++i) {
            QString labelInfo = QString("<td>%1</td>").arg(labels.value(i));
            html.write(labelInfo.toUtf8().data());
        }
        html.write("</tr>");
        // 写入内容
        for (int row = 0; row < jList.count(); ++row) {
            //导出逻辑启动停止控制，外部把m_canRunning置false时停止运行，抛出异常处理
            if (!m_canRunning) {
                throw  QString(stopStr);
            }
            //根据字段拼出每行的网页内容
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
            //导出进度信号
            sigProgress(row + 1, jList.count());
        }

        html.write("</table>\n");
        html.write("</body>\n");
        html.write("</html>\n");
    } catch (QString ErrorStr) {
        //捕获到异常，导出失败，发出失败信号
        qDebug() << "Export Stop" << ErrorStr;
        html.close();
        emit sigResult(false);
        if (ErrorStr != stopStr) {
            emit sigError(QString("export error: %1").arg(ErrorStr));
        }
        return false;
    }
    html.close();
    //导出成功，如果此时被停止，则发出导出失败信号
    emit sigResult(m_canRunning);
    return true && m_canRunning;
}

/**
 * @brief LogExportThread::exportToHtml 导出到日志html格式配置函数，对LOG_MSG_DPKG数据类型的重载（指dpkg日志）
 * @param fileName 导出文件路径全称
 * @param jList  要导出的数据源 LOG_MSG_DPKG
 * @param labels 表头字符串
 * @return 是否导出成功
 */
bool LogExportThread::exportToHtml(QString fileName, QList<LOG_MSG_DPKG> jList, QStringList labels)
{
    //判断文件路径是否存在，不存在就返回错误
    QFile html(fileName);
    if (!html.open(QIODevice::WriteOnly)) {
        emit sigResult(false);
        emit sigError(openErroStr);
        return false;
    }
    try {
        //写网页头
        html.write("<!DOCTYPE html>\n");
        html.write("<html>\n");
        html.write("<body>\n");
        //写入表格标签
        html.write("<table border=\"1\">\n");
        // 写入表头
        html.write("<tr>");
        for (int i = 0; i < labels.count(); ++i) {
            QString labelInfo = QString("<td>%1</td>").arg(labels.value(i));
            html.write(labelInfo.toUtf8().data());
        }
        html.write("</tr>");
        // 写入内容
        for (int row = 0; row < jList.count(); ++row) {
            //导出逻辑启动停止控制，外部把m_canRunning置false时停止运行，抛出异常处理
            if (!m_canRunning) {
                throw  QString(stopStr);
            }
            //根据字段拼出每行的网页内容
            LOG_MSG_DPKG jMsg = jList.at(row);
            html.write("<tr>");
            QString info = QString("<td>%1</td>").arg(jMsg.dateTime);
            html.write(info.toUtf8().data());
            info = QString("<td>%1</td>").arg(jMsg.msg);
            html.write(info.toUtf8().data());
            info = QString("<td>%1</td>").arg(jMsg.action);
            html.write(info.toUtf8().data());
            html.write("</tr>");
            //导出进度信号
            sigProgress(row + 1, jList.count());
        }

        html.write("</table>\n");
        html.write("</body>\n");
        html.write("</html>\n");
    } catch (QString ErrorStr) {
        //捕获到异常，导出失败，发出失败信号
        qDebug() << "Export Stop" << ErrorStr;
        html.close();
        emit sigResult(false);
        if (ErrorStr != stopStr) {
            emit sigError(QString("export error: %1").arg(ErrorStr));
        }
        return false;
    }
    html.close();
    //导出成功，如果此时被停止，则发出导出失败信号
    emit sigResult(m_canRunning);
    return true && m_canRunning;
}

/**
 * @brief LogExportThread::exportToHtml导出到日志html格式函数，对LOG_MSG_BOOT数据类型的重载（指启动日志）
 * @param fileName 导出文件路径全称
 * @param jList  要导出的数据源 LOG_MSG_BOOT
 * @param labels 表头字符串
 * @return 是否导出成功
 */
bool LogExportThread::exportToHtml(QString fileName, QList<LOG_MSG_BOOT> jList, QStringList labels)
{
    //判断文件路径是否存在，不存在就返回错误
    QFile html(fileName);
    if (!html.open(QIODevice::WriteOnly)) {
        emit sigResult(false);
        emit sigError(openErroStr);
        return false;
    }
    try {
        //写网页头
        html.write("<!DOCTYPE html>\n");
        html.write("<html>\n");
        html.write("<body>\n");
        //写入表格标签
        html.write("<table border=\"1\">\n");
        // 写入表头
        html.write("<tr>");
        for (int i = 0; i < labels.count(); ++i) {
            QString labelInfo = QString("<td>%1</td>").arg(labels.value(i));
            html.write(labelInfo.toUtf8().data());
        }
        html.write("</tr>");
        // 写入内容
        for (int row = 0; row < jList.count(); ++row) {
            //导出逻辑启动停止控制，外部把m_canRunning置false时停止运行，抛出异常处理
            if (!m_canRunning) {
                throw  QString(stopStr);
            }
            //根据字段拼出每行的网页内容
            LOG_MSG_BOOT jMsg = jList.at(row);
            html.write("<tr>");
            QString info = QString("<td>%1</td>").arg(jMsg.status);
            html.write(info.toUtf8().data());
            info = QString("<td>%1</td>").arg(jMsg.msg);
            html.write(info.toUtf8().data());
            html.write("</tr>");
            //导出进度信号
            sigProgress(row + 1, jList.count());
        }

        html.write("</table>\n");
        html.write("</body>\n");
        html.write("</html>\n");
    } catch (QString ErrorStr) {
        //捕获到异常，导出失败，发出失败信号
        qDebug() << "Export Stop" << ErrorStr;
        html.close();
        emit sigResult(false);
        if (ErrorStr != stopStr) {
            emit sigError(QString("export error: %1").arg(ErrorStr));
        }
        return false;
    }
    html.close();
    //导出成功，如果此时被停止，则发出导出失败信号
    emit sigResult(m_canRunning);
    return true && m_canRunning;
}

/**
 * @brief LogExportThread::exportToHtml 导出到日志html格式函数，对LOG_MSG_XORG数据类型的重载（指xorg日志）
 * @param fileName 导出文件路径全称
 * @param jList 要导出的数据源 LOG_MSG_XORG
 * @param labels 表头字符串
 * @return 是否导出成功
 */
bool LogExportThread::exportToHtml(QString fileName, QList<LOG_MSG_XORG> jList, QStringList labels)
{
    //判断文件路径是否存在，不存在就返回错误
    QFile html(fileName);
    if (!html.open(QIODevice::WriteOnly)) {
        emit sigResult(false);
        emit sigError(openErroStr);
        return false;
    }
    try {
        //写网页头
        html.write("<!DOCTYPE html>\n");
        html.write("<html>\n");
        html.write("<body>\n");
        //写入表格标签
        html.write("<table border=\"1\">\n");
        // 写入表头
        html.write("<tr>");
        for (int i = 0; i < labels.count(); ++i) {
            QString labelInfo = QString("<td>%1</td>").arg(labels.value(i));
            html.write(labelInfo.toUtf8().data());
        }
        html.write("</tr>");
        // 写入内容
        for (int row = 0; row < jList.count(); ++row) {
            //导出逻辑启动停止控制，外部把m_canRunning置false时停止运行，抛出异常处理
            if (!m_canRunning) {
                throw  QString(stopStr);
            }
            //根据字段拼出每行的网页内容
            LOG_MSG_XORG jMsg = jList.at(row);
            html.write("<tr>");
            QString info = QString("<td>%1</td>").arg(jMsg.dateTime);
            html.write(info.toUtf8().data());
            info = QString("<td>%1</td>").arg(jMsg.msg);
            html.write(info.toUtf8().data());
            html.write("</tr>");
            //导出进度信号
            sigProgress(row + 1, jList.count());
        }

        html.write("</table>\n");
        html.write("</body>\n");
        html.write("</html>\n");
    } catch (QString ErrorStr) {
        //捕获到异常，导出失败，发出失败信号
        qDebug() << "Export Stop" << ErrorStr;
        html.close();
        emit sigResult(false);
        if (ErrorStr != stopStr) {
            emit sigError(QString("export error: %1").arg(ErrorStr));
        }
        return false;
    }
    html.close();
    //导出成功，如果此时被停止，则发出导出失败信号
    emit sigResult(m_canRunning);
    return true && m_canRunning;
}

/**
 * @brief LogExportThread::exportToHtml 导出到日志html格式配置函数，对LOG_MSG_NORMAL数据类型的重载（指开关机日志）
 * @param fileName 导出文件路径全称
 * @param jList 要导出的数据源 LOG_MSG_NORMAL
 * @param labels 表头字符串
 * @return 是否导出成功
 */
bool LogExportThread::exportToHtml(QString fileName, QList<LOG_MSG_NORMAL> jList, QStringList labels)
{
    //判断文件路径是否存在，不存在就返回错误
    QFile html(fileName);
    if (!html.open(QIODevice::WriteOnly)) {
        emit sigResult(false);
        emit sigError(openErroStr);
        return false;
    }
    try {
        //写网页头
        html.write("<!DOCTYPE html>\n");
        html.write("<html>\n");
        html.write("<body>\n");
        //写入表格标签
        html.write("<table border=\"1\">\n");
        // 写入表头
        html.write("<tr>");
        for (int i = 0; i < labels.count(); ++i) {
            QString labelInfo = QString("<td>%1</td>").arg(labels.value(i));
            html.write(labelInfo.toUtf8().data());
        }
        html.write("</tr>");
        // 写入内容
        for (int row = 0; row < jList.count(); ++row) {
            //导出逻辑启动停止控制，外部把m_canRunning置false时停止运行，抛出异常处理
            if (!m_canRunning) {
                throw  QString(stopStr);
            }
            //根据字段拼出每行的网页内容
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
            //导出进度信号
            sigProgress(row + 1, jList.count());
        }

        html.write("</table>\n");
        html.write("</body>\n");
        html.write("</html>\n");
    } catch (QString ErrorStr) {
        //捕获到异常，导出失败，发出失败信号
        qDebug() << "Export Stop" << ErrorStr;
        html.close();
        emit sigResult(false);
        if (ErrorStr != stopStr) {
            emit sigError(QString("export error: %1").arg(ErrorStr));
        }
        return false;
    }
    html.close();
    //导出成功，如果此时被停止，则发出导出失败信号
    emit sigResult(m_canRunning);
    return true && m_canRunning;
}

/**
 * @brief LogExportThread::exportToHtml导出到日志html格式函数，对LOG_MSG_KWIN数据类型的重载（指klu上的kwin日志）
 * @param fileName 导出文件路径全称
 * @param jList 要导出的数据源 LOG_MSG_KWIN
 * @param labels 表头字符串
 * @return 是否导出成功
 */
bool LogExportThread::exportToHtml(QString fileName, QList<LOG_MSG_KWIN> jList, QStringList labels)
{
    //判断文件路径是否存在，不存在就返回错误
    QFile html(fileName);
    if (!html.open(QIODevice::WriteOnly)) {
        emit sigResult(false);
        emit sigError(openErroStr);
        return false;
    }
    try {
        //写网页头
        html.write("<!DOCTYPE html>\n");
        html.write("<html>\n");
        html.write("<body>\n");
        //写入表格标签
        html.write("<table border=\"1\">\n");
        // 写入表头
        html.write("<tr>");
        for (int i = 0; i < labels.count(); ++i) {
            QString labelInfo = QString("<td>%1</td>").arg(labels.value(i));
            html.write(labelInfo.toUtf8().data());
        }
        html.write("</tr>");
        // 写入内容
        for (int row = 0; row < jList.count(); ++row) {
            //导出逻辑启动停止控制，外部把m_canRunning置false时停止运行，抛出异常处理
            if (!m_canRunning) {
                throw  QString(stopStr);
            }
            //根据字段拼出每行的网页内容
            LOG_MSG_KWIN jMsg = jList.at(row);
            html.write("<tr>");
            QString info = QString("<td>%1</td>").arg(jMsg.msg);
            html.write(info.toUtf8().data());
            html.write("</tr>");
            //导出进度信号
            sigProgress(row + 1, jList.count());
        }

        html.write("</table>\n");
        html.write("</body>\n");
        html.write("</html>\n");
    } catch (QString ErrorStr) {
        //捕获到异常，导出失败，发出失败信号
        qDebug() << "Export Stop" << ErrorStr;
        html.close();
        emit sigResult(false);
        if (ErrorStr != stopStr) {
            emit sigError(QString("export error: %1").arg(ErrorStr));
        }
        return false;
    }
    html.close();
    //导出成功，如果此时被停止，则发出导出失败信号
    emit sigResult(m_canRunning);
    return true && m_canRunning;
}


/**
 * @brief LogExportThread::exportToXls 导出到日志xlsx格式函数对QStandardItemModel数据类型的重载
 * @param fileName 导出文件路径全称
 * @param pModel 要导出的数据源，为QStandardItemModel
 * @param flag 导出的日志类型
 * @return 是否导出成功
 */
bool LogExportThread::exportToXls(QString fileName, QStandardItemModel *pModel, LOG_FLAG flag)
{
    try {
        //判断model指针是否为空，为空则抛出错误，返回不执行
        if (!pModel) {
            throw  QString("model is null");
        }
        auto currentXlsRow = 1;
        //传入控制停止导出逻辑的bool变量
        QXlsx::Document xlsx(&m_canRunning);
        //连接对象执行进度信号，计算总进度并发出，把进度分成三段
        connect(&xlsx, &QXlsx::Document::sigProcessAbstractSheet, this, [ = ](int iCurrent, int iTotal) {
            int end = static_cast<int>(iTotal * 0.1 > 5 ? iTotal * 0.1 : 5);
            sigProgress(iCurrent + iTotal, iTotal * 3 + end);
        });
        connect(&xlsx, &QXlsx::Document::sigProcessharedStrings, this, [ = ](int iCurrent, int iTotal) {
            int end = static_cast<int>(iTotal * 0.1 > 5 ? iTotal * 0.1 : 5);
            sigProgress(iCurrent + iTotal * 2, iTotal * 3 + end);
        });
        // 写入表头
        for (int col = 0; col < pModel->columnCount(); ++col) {
            auto item = pModel->horizontalHeaderItem(col);
            if (item) {
                //设置表头字体加粗
                QXlsx::Format boldFont;
                boldFont.setFontBold(true);
                xlsx.write(currentXlsRow, col + 1, item->text(), boldFont);
            }
        }
        ++currentXlsRow;
        int end = static_cast<int>(pModel->rowCount() * 0.1 > 5 ? pModel->rowCount() * 0.1 : 5);
        //应用日志导出内容
        if (flag == APP) {
            for (int row = 0; row < pModel->rowCount(); ++row) {
                //导出逻辑启动停止控制，外部把m_canRunning置false时停止运行，抛出异常处理
                if (!m_canRunning) {
                    throw  QString(stopStr);
                }
                xlsx.write(currentXlsRow, 0 + 1,
                           pModel->item(row, 0)->data(Qt::UserRole + 6).toString());
                //根据字段拼出每行的网页内容
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
                //导出逻辑启动停止控制，外部把m_canRunning置false时停止运行，抛出异常处理
                if (!m_canRunning) {
                    throw  QString(stopStr);
                }
                //根据字段拼出每行的网页内容
                for (int col = 0; col < pModel->columnCount(); ++col) {
                    if (!m_canRunning) {
                        throw  QString(stopStr);
                    }
                    xlsx.write(currentXlsRow, col + 1, pModel->item(row, col)->text());
                }
                ++currentXlsRow;

                //导出进度信号
                sigProgress(row + 1, pModel->rowCount() * 3 + end);

            }
        }

        ++currentXlsRow;
        if (!xlsx.saveAs(fileName)) {
            throw  QString(stopStr);
        }
        sigProgress(100, 100);
    } catch (QString ErrorStr) {
        //捕获到异常，导出失败，发出失败信号
        qDebug() << "Export Stop" << ErrorStr;
        emit sigResult(false);
        if (ErrorStr != stopStr) {
            emit sigError(QString("export error: %1").arg(ErrorStr));
        }
        return false;
    }
    //导出成功，如果此时被停止，则发出导出失败信号
    emit sigResult(m_canRunning);
    return true && m_canRunning;
}
/**
 * @brief LogExportThread::exportToXls导出到日志xlsx格式函数，对LOG_MSG_JOURNAL数据类型的重载（指系统日志和内核日志）
 * @param fileName 导出文件路径全称
 * @param jList 要导出的数据源 数据类型为LOG_MSG_JOURNAL
 * @param labels 表头字符串
 * @param flag  导出的日志类型
 * @return 是否导出成功
 */
bool LogExportThread::exportToXls(QString fileName, QList<LOG_MSG_JOURNAL> jList,
                                  QStringList labels, LOG_FLAG iFlag)
{


    try {
        auto currentXlsRow = 1;
        //传入控制停止导出逻辑的bool变量
        QXlsx::Document xlsx(&m_canRunning);
        //连接对象执行进度信号，计算总进度并发出，把进度分成三段
        connect(&xlsx, &QXlsx::Document::sigProcessAbstractSheet, this, [ = ](int iCurrent, int iTotal) {
            int end = static_cast<int>(iTotal * 0.1 > 5 ? iTotal * 0.1 : 5);
            sigProgress(iCurrent + iTotal, iTotal * 3 + end);
        });
        connect(&xlsx, &QXlsx::Document::sigProcessharedStrings, this, [ = ](int iCurrent, int iTotal) {
            int end = static_cast<int>(iTotal * 0.1 > 5 ? iTotal * 0.1 : 5);
            sigProgress(iCurrent + iTotal * 2, iTotal * 3 + end);
        });
        // 写入表头
        for (int col = 0; col < labels.count(); ++col) {
            //设置表头字体加粗
            QXlsx::Format boldFont;
            boldFont.setFontBold(true);
            xlsx.write(currentXlsRow, col + 1, labels.at(col), boldFont);
        }
        ++currentXlsRow;
        int end = static_cast<int>(jList.count() * 0.1 > 5 ? jList.count() * 0.1 : 5);

        for (int row = 0; row < jList.count(); ++row) {
            //导出逻辑启动停止控制，外部把m_canRunning置false时停止运行，抛出异常处理
            if (!m_canRunning) {
                throw  QString(stopStr);
            }
            LOG_MSG_JOURNAL message = jList.at(row);
            int col = 1;

            //根据字段拼出每行的网页内容,根据情况区分系统日志和内核日志
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
            //导出进度信号

            sigProgress(row + 1, jList.count() * 3 + end);
        }
        ++currentXlsRow;
        if (!xlsx.saveAs(fileName)) {
            throw  QString(stopStr);
        }
        sigProgress(100, 100);
    } catch (QString ErrorStr) {
        //捕获到异常，导出失败，发出失败信号
        qDebug() << "Export Stop" << ErrorStr;
        emit sigResult(false);
        if (ErrorStr != stopStr) {
            emit sigError(QString("export error: %1").arg(ErrorStr));
        }
        return false;
    }
    //导出成功，如果此时被停止，则发出导出失败信号
    emit sigResult(m_canRunning);
    return true && m_canRunning;
}


bool LogExportThread::exportToXlsNew(QString fileName, QList<LOG_MSG_JOURNAL> jList, QStringList labels, LOG_FLAG iFlag)
{
//    try {
//        auto currentXlsRow = 0;
//        lxw_workbook  *workbook  = workbook_new(fileName.toStdString().c_str());
//        lxw_worksheet *worksheet = workbook_add_worksheet(workbook, NULL);
//        lxw_format *format = workbook_add_format(workbook);
//        format_set_bold(format);
//        for (int col = 0; col < labels.count(); ++col) {
//            worksheet_write_string(worksheet, currentXlsRow, col, labels.at(col).toStdString().c_str(), format);
//        }
//        ++currentXlsRow;
//        int end = static_cast<int>(jList.count() * 0.1 > 5 ? jList.count() * 0.1 : 5);

//        for (int row = 0; row < 100000 ; ++row) {
//            if (!m_canRunning) {
//                throw  QString(stopStr);
//            }
//            LOG_MSG_JOURNAL message = jList.at(1);
//            int col = 0;

//            if (iFlag == JOURNAL) {
//                worksheet_write_string(worksheet, currentXlsRow, col++, message.level.toStdString().c_str(), NULL);
//                worksheet_write_string(worksheet, currentXlsRow, col++, message.daemonName.toStdString().c_str(), NULL);
//                worksheet_write_string(worksheet, currentXlsRow, col++, message.dateTime.toStdString().c_str(), NULL);
//                worksheet_write_string(worksheet, currentXlsRow, col++, message.msg.toStdString().c_str(), NULL);
//                worksheet_write_string(worksheet, currentXlsRow, col++, message.hostName.toStdString().c_str(), NULL);
//                worksheet_write_string(worksheet, currentXlsRow, col++, message.daemonId.toStdString().c_str(), NULL);
//            } else if (iFlag == KERN) {
//                worksheet_write_string(worksheet, currentXlsRow, col++, message.dateTime.toStdString().c_str(), NULL);
//                worksheet_write_string(worksheet, currentXlsRow, col++, message.hostName.toStdString().c_str(), NULL);
//                worksheet_write_string(worksheet, currentXlsRow, col++, message.daemonName.toStdString().c_str(), NULL);
//                worksheet_write_string(worksheet, currentXlsRow, col++, message.msg.toStdString().c_str(), NULL);
//            }

//            ++currentXlsRow;
//            sigProgress(row + 1, jList.count() * 3 + end);
//        }


//        workbook_close(workbook);
//        malloc_trim(0);
//        sigProgress(100, 100);
//        qDebug() << "export xlsx new";
//    } catch (QString ErrorStr) {
//        qDebug() << "Export Stop" << ErrorStr;
//        emit sigResult(false);
//        if (ErrorStr != stopStr) {
//            emit sigError(QString("export error: %1").arg(ErrorStr));
//        }
//        return false;
//    }
//    emit sigResult(m_canRunning);
//    return true && m_canRunning;
    return  true;
}


/**
 * @brief LogExportThread::exportToXls导出到日志xlsx格式函数，对LOG_MSG_APPLICATOIN数据类型的重载（指应用日志）
 * @param fileName 导出文件路径全称
 * @param jList 要导出的数据源 数据类型为LOG_MSG_APPLICATOIN
 * @param labels 表头字符串
 * @param iAppName 导出的应用日志的应用名称
 * @return 是否导出成功
 */

bool LogExportThread::exportToXls(QString fileName, QList<LOG_MSG_APPLICATOIN> jList, QStringList labels, QString &iAppName)
{
    QElapsedTimer timer;
    timer.start();
    try {
        auto currentXlsRow = 1;

        //传入控制停止导出逻辑的bool变量
        QXlsx::Document xlsx(&m_canRunning);

        //连接对象执行进度信号，计算总进度并发出，把进度分成三段
        connect(&xlsx, &QXlsx::Document::sigProcessAbstractSheet, this, [ = ](int iCurrent, int iTotal) {
            int end = static_cast<int>(iTotal * 0.1 > 5 ? iTotal * 0.1 : 5);
            sigProgress(iCurrent + iTotal, iTotal * 3 + end);
        });
        connect(&xlsx, &QXlsx::Document::sigProcessharedStrings, this, [ = ](int iCurrent, int iTotal) {
            int end = static_cast<int>(iTotal * 0.1 > 5 ? iTotal * 0.1 : 5);
            sigProgress(iCurrent + iTotal * 2, iTotal * 3 + end);
        });
        // 写入表头
        for (int col = 0; col < labels.count(); ++col) {
            //设置表头字体加粗
            QXlsx::Format boldFont;
            boldFont.setFontBold(true);
            xlsx.write(currentXlsRow, col + 1, labels.at(col), boldFont);
        }
        ++currentXlsRow;
        int end = static_cast<int>(jList.count() * 0.1 > 5 ? jList.count() * 0.1 : 5);
        for (int row = 0; row < jList.count(); ++row) {
            //导出逻辑启动停止控制，外部把m_canRunning置false时停止运行，抛出异常处理
            if (!m_canRunning) {
                throw  QString(stopStr);
            }
            //根据字段拼出每行的网页内容
            LOG_MSG_APPLICATOIN message = jList.at(row);
            int col = 1;
            xlsx.write(currentXlsRow, col++, strTranslate(message.level));
            xlsx.write(currentXlsRow, col++, message.dateTime);
            xlsx.write(currentXlsRow, col++, iAppName);
            xlsx.write(currentXlsRow, col++, message.msg);
            ++currentXlsRow;
            //导出进度信号
            sigProgress(row + 1, jList.count() * 3 + end);
        }
        ++currentXlsRow;
        if (!xlsx.saveAs(fileName)) {
            throw  QString(stopStr);
        }
        sigProgress(100, 100);
    } catch (QString ErrorStr) {
        //捕获到异常，导出失败，发出失败信号
        qDebug() << "Export Stop" << ErrorStr;
        emit sigResult(false);
        if (ErrorStr != stopStr) {
            emit sigError(QString("export error: %1").arg(ErrorStr));
        }
        return false;
    }
    //导出成功，如果此时被停止，则发出导出失败信号
    emit sigResult(m_canRunning);
    return true && m_canRunning;
}

/**
 * @brief LogExportThread::exportToXls 导出到日志xlsx格式函数，对LOG_MSG_DPKG数据类型的重载（指dpkg日志）
 * @param fileName 导出文件路径全称
 * @param jList  要导出的数据源 LOG_MSG_DPKG
 * @param labels 表头字符串
 * @return 是否导出成功
 */
bool LogExportThread::exportToXls(QString fileName, QList<LOG_MSG_DPKG> jList, QStringList labels)
{
    try {
        auto currentXlsRow = 1;
        //传入控制停止导出逻辑的bool变量
        QXlsx::Document xlsx(&m_canRunning);
        //连接对象执行进度信号，计算总进度并发出，把进度分成三段
        connect(&xlsx, &QXlsx::Document::sigProcessAbstractSheet, this, [ = ](int iCurrent, int iTotal) {
            int end = static_cast<int>(iTotal * 0.1 > 5 ? iTotal * 0.1 : 5);
            sigProgress(iCurrent + iTotal, iTotal * 3 + end);
        });
        connect(&xlsx, &QXlsx::Document::sigProcessharedStrings, this, [ = ](int iCurrent, int iTotal) {
            int end = static_cast<int>(iTotal * 0.1 > 5 ? iTotal * 0.1 : 5);
            sigProgress(iCurrent + iTotal * 2, iTotal * 3 + end);
        });
        // 写入表头
        for (int col = 0; col < labels.count(); ++col) {
            //设置表头字体加粗
            QXlsx::Format boldFont;
            boldFont.setFontBold(true);
            xlsx.write(currentXlsRow, col + 1, labels.at(col), boldFont);
        }
        ++currentXlsRow;
        int end = static_cast<int>(jList.count() * 0.1 > 5 ? jList.count() * 0.1 : 5);
        for (int row = 0; row < jList.count(); ++row) {
            //导出逻辑启动停止控制，外部把m_canRunning置false时停止运行，抛出异常处理
            if (!m_canRunning) {
                throw  QString(stopStr);
            }
            //根据字段拼出每行的网页内容
            LOG_MSG_DPKG message = jList.at(row);
            int col = 1;
            xlsx.write(currentXlsRow, col++, message.dateTime);
            xlsx.write(currentXlsRow, col++, message.msg);
            xlsx.write(currentXlsRow, col++, message.action);
            ++currentXlsRow;
            //导出进度信号
            sigProgress(row + 1, jList.count() * 3 + end);
        }
        ++currentXlsRow;
        if (!xlsx.saveAs(fileName)) {
            throw  QString(stopStr);
        }
        sigProgress(100, 100);
    } catch (QString ErrorStr) {
        //捕获到异常，导出失败，发出失败信号
        qDebug() << "Export Stop" << ErrorStr;
        emit sigResult(false);
        if (ErrorStr != stopStr) {
            emit sigError(QString("export error: %1").arg(ErrorStr));
        }
        return false;
    }
    //导出成功，如果此时被停止，则发出导出失败信号
    emit sigResult(m_canRunning);
    return true && m_canRunning;
}

/**
 * @brief LogExportThread::exportToXls导出到日志xlsx格式函数，对LOG_MSG_BOOT数据类型的重载（指启动日志）
 * @param fileName 导出文件路径全称
 * @param jList  要导出的数据源 LOG_MSG_DPKG
 * @param labels 表头字符串
 * @return 是否导出成功
 */
bool LogExportThread::exportToXls(QString fileName, QList<LOG_MSG_BOOT> jList, QStringList labels)
{
    try {
        auto currentXlsRow = 1;
        //传入控制停止导出逻辑的bool变量
        QXlsx::Document xlsx(&m_canRunning);
//        connect(&xlsx, &QXlsx::Document::saveProcess, this, [ = ](int iCurrent, int iTotal) {
//            sigProgress(iCurrent + iTotal, iTotal * 2);
//        });
        //连接对象执行进度信号，计算总进度并发出，把进度分成三段
        connect(&xlsx, &QXlsx::Document::sigProcessAbstractSheet, this, [ = ](int iCurrent, int iTotal) {
            int end = static_cast<int>(iTotal * 0.1 > 5 ? iTotal * 0.1 : 5);
            sigProgress(iCurrent + iTotal, iTotal * 3 + end);
        });
        connect(&xlsx, &QXlsx::Document::sigProcessharedStrings, this, [ = ](int iCurrent, int iTotal) {
            int end = static_cast<int>(iTotal * 0.1 > 5 ? iTotal * 0.1 : 5);
            sigProgress(iCurrent + iTotal * 2, iTotal * 3 + end);
        });
        // 写入表头
        for (int col = 0; col < labels.count(); ++col) {
            //设置表头字体加粗
            QXlsx::Format boldFont;
            boldFont.setFontBold(true);
            xlsx.write(currentXlsRow, col + 1, labels.at(col), boldFont);
        }
        ++currentXlsRow;
        int end = static_cast<int>(jList.count() * 0.1 > 5 ? jList.count() * 0.1 : 5);
        for (int row = 0; row < jList.count(); ++row) {
            //导出逻辑启动停止控制，外部把m_canRunning置false时停止运行，抛出异常处理
            if (!m_canRunning) {
                throw  QString(stopStr);
            }
            //根据字段拼出每行的网页内容
            LOG_MSG_BOOT message = jList.at(row);
            int col = 1;
            xlsx.write(currentXlsRow, col++, message.status);
            xlsx.write(currentXlsRow, col++, message.msg);
            ++currentXlsRow;
            //导出进度信号
            sigProgress(row + 1, jList.count() * 3 + end);
        }
        ++currentXlsRow;
        if (!xlsx.saveAs(fileName)) {
            throw  QString(stopStr);
        }
        sigProgress(100, 100);
    } catch (QString ErrorStr) {
        //捕获到异常，导出失败，发出失败信号
        qDebug() << "Export Stop" << ErrorStr;
        emit sigResult(false);
        if (ErrorStr != stopStr) {
            emit sigError(QString("export error: %1").arg(ErrorStr));
        }
        return false;
    }
    //导出成功，如果此时被停止，则发出导出失败信号
    emit sigResult(m_canRunning);
    return true && m_canRunning;
}

/**
 * @brief LogExportThread::exportToXls导出到日志xlsx格式函数，对LOG_MSG_XORG数据类型的重载（指xorg日志）
 * @param fileName 导出文件路径全称
 * @param jList 要导出的数据源 LOG_MSG_XORG
 * @param labels 表头字符串
 * @return 是否导出成功
 */
bool LogExportThread::exportToXls(QString fileName, QList<LOG_MSG_XORG> jList, QStringList labels)
{
    try {
        auto currentXlsRow = 1;
        //传入控制停止导出逻辑的bool变量
        QXlsx::Document xlsx(&m_canRunning);
        //连接对象执行进度信号，计算总进度并发出，把进度分成三段
        connect(&xlsx, &QXlsx::Document::sigProcessAbstractSheet, this, [ = ](int iCurrent, int iTotal) {
            int end = static_cast<int>(iTotal * 0.1 > 5 ? iTotal * 0.1 : 5);
            sigProgress(iCurrent + iTotal, iTotal * 3 + end);
        });
        connect(&xlsx, &QXlsx::Document::sigProcessharedStrings, this, [ = ](int iCurrent, int iTotal) {
            int end = static_cast<int>(iTotal * 0.1 > 5 ? iTotal * 0.1 : 5);
            sigProgress(iCurrent + iTotal * 2, iTotal * 3 + end);
        });
        // 写入表头
        for (int col = 0; col < labels.count(); ++col) {
            //设置表头字体加粗
            QXlsx::Format boldFont;
            boldFont.setFontBold(true);
            xlsx.write(currentXlsRow, col + 1, labels.at(col), boldFont);
        }
        ++currentXlsRow;
        int end = static_cast<int>(jList.count() * 0.1 > 5 ? jList.count() * 0.1 : 5);
        for (int row = 0; row < jList.count(); ++row) {
            //导出逻辑启动停止控制，外部把m_canRunning置false时停止运行，抛出异常处理
            if (!m_canRunning) {
                throw  QString(stopStr);
            }
            //根据字段拼出每行的网页内容
            LOG_MSG_XORG message = jList.at(row);
            int col = 1;
            xlsx.write(currentXlsRow, col++, message.dateTime);
            xlsx.write(currentXlsRow, col++, message.msg);
            ++currentXlsRow;
            //导出进度信号
            sigProgress(row + 1, jList.count() * 3 + end);
        }
        ++currentXlsRow;
        if (!xlsx.saveAs(fileName)) {
            throw  QString(stopStr);
        }
        sigProgress(100, 100);
    } catch (QString ErrorStr) {
        //捕获到异常，导出失败，发出失败信号
        qDebug() << "Export Stop" << ErrorStr;
        emit sigResult(false);
        if (ErrorStr != stopStr) {
            emit sigError(QString("export error: %1").arg(ErrorStr));
        }
        return false;
    }
    //导出成功，如果此时被停止，则发出导出失败信号
    emit sigResult(m_canRunning);
    return true && m_canRunning;
}

/**
 * @brief LogExportThread::exportToXls导出到日志xlsx格式函数，对LOG_MSG_NORMAL数据类型的重载（指开关机日志）
 * @param fileName 导出文件路径全称
 * @param jList 要导出的数据源 LOG_MSG_NORMAL
 * @param labels 表头字符串
 * @return 是否导出成功
 */
bool LogExportThread::exportToXls(QString fileName, QList<LOG_MSG_NORMAL> jList, QStringList labels)
{
    try {
        auto currentXlsRow = 1;
        //传入控制停止导出逻辑的bool变量
        QXlsx::Document xlsx(&m_canRunning);
        //连接对象执行进度信号，计算总进度并发出，把进度分成三段
        connect(&xlsx, &QXlsx::Document::sigProcessAbstractSheet, this, [ = ](int iCurrent, int iTotal) {
            int end = static_cast<int>(iTotal * 0.1 > 5 ? iTotal * 0.1 : 5);
            sigProgress(iCurrent + iTotal, iTotal * 3 + end);
        });
        connect(&xlsx, &QXlsx::Document::sigProcessharedStrings, this, [ = ](int iCurrent, int iTotal) {
            int end = static_cast<int>(iTotal * 0.1 > 5 ? iTotal * 0.1 : 5);
            sigProgress(iCurrent + iTotal * 2, iTotal * 3 + end);
        });
        // 写入表头
        for (int col = 0; col < labels.count(); ++col) {
            //设置表头字体加粗
            QXlsx::Format boldFont;
            boldFont.setFontBold(true);
            xlsx.write(currentXlsRow, col + 1, labels.at(col), boldFont);
        }
        ++currentXlsRow;
        int end = static_cast<int>(jList.count() * 0.1 > 5 ? jList.count() * 0.1 : 5);
        for (int row = 0; row < jList.count(); ++row) {
            //导出逻辑启动停止控制，外部把m_canRunning置false时停止运行，抛出异常处理
            if (!m_canRunning) {
                throw  QString(stopStr);
            }
            //根据字段拼出每行的网页内容
            LOG_MSG_NORMAL message = jList.at(row);
            int col = 1;
            xlsx.write(currentXlsRow, col++, message.eventType);
            xlsx.write(currentXlsRow, col++, message.userName);
            xlsx.write(currentXlsRow, col++, message.dateTime);
            xlsx.write(currentXlsRow, col++, message.msg);
            ++currentXlsRow;
            //导出进度信号
            sigProgress(row + 1, jList.count() * 3 + end);
        }
        ++currentXlsRow;
        if (!xlsx.saveAs(fileName)) {
            throw  QString(stopStr);
        }
        sigProgress(100, 100);
    } catch (QString ErrorStr) {
        //捕获到异常，导出失败，发出失败信号
        qDebug() << "Export Stop" << ErrorStr;
        emit sigResult(false);
        if (ErrorStr != stopStr) {
            emit sigError(QString("export error: %1").arg(ErrorStr));
        }
        return false;
    }

    //导出成功，如果此时被停止，则发出导出失败信号
    emit sigResult(m_canRunning);
    return true && m_canRunning;
}

/**
 * @brief LogExportThread::exportToXls导出到日志xlsw格式函数，对LOG_MSG_KWIN数据类型的重载（指klu上的kwin日志）
 * @param fileName 导出文件路径全称
 * @param jList 要导出的数据源 LOG_MSG_KWIN
 * @param labels 表头字符串
 * @return 是否导出成功
 */
bool LogExportThread::exportToXls(QString fileName, QList<LOG_MSG_KWIN> jList, QStringList labels)
{
    try {
        auto currentXlsRow = 1;
        //传入控制停止导出逻辑的bool变量
        QXlsx::Document xlsx(&m_canRunning);
        //连接对象执行进度信号，计算总进度并发出，把进度分成三段
        connect(&xlsx, &QXlsx::Document::sigProcessAbstractSheet, this, [ = ](int iCurrent, int iTotal) {
            int end = static_cast<int>(iTotal * 0.1 > 5 ? iTotal * 0.1 : 5);
            sigProgress(iCurrent + iTotal, iTotal * 3 + end);
        });
        connect(&xlsx, &QXlsx::Document::sigProcessharedStrings, this, [ = ](int iCurrent, int iTotal) {
            int end = static_cast<int>(iTotal * 0.1 > 5 ? iTotal * 0.1 : 5);
            sigProgress(iCurrent + iTotal * 2, iTotal * 3 + end);
        });
        // 写入表头
        for (int col = 0; col < labels.count(); ++col) {
            QXlsx::Format boldFont;
            //设置表头字体加粗
            boldFont.setFontBold(true);
            xlsx.write(currentXlsRow, col + 1, labels.at(col), boldFont);
        }
        ++currentXlsRow;
        int end = static_cast<int>(jList.count() * 0.1 > 5 ? jList.count() * 0.1 : 5);
        for (int row = 0; row < jList.count(); ++row) {
            //导出逻辑启动停止控制，外部把m_canRunning置false时停止运行，抛出异常处理
            if (!m_canRunning) {
                throw  QString(stopStr);
            }
            //根据字段拼出每行的网页内容
            LOG_MSG_KWIN message = jList.at(row);
            int col = 1;
            xlsx.write(currentXlsRow, col++, message.msg);
            ++currentXlsRow;
            //导出进度信号
            sigProgress(row + 1, jList.count() * 3 + end);
        }
        ++currentXlsRow;

        if (!xlsx.saveAs(fileName)) {
            throw  QString(stopStr);
        }
        sigProgress(100, 100);
    } catch (QString ErrorStr) {
        //捕获到异常，导出失败，发出失败信号
        qDebug() << "Export Stop" << ErrorStr;
        emit sigResult(false);
        if (ErrorStr != stopStr) {
            emit sigError(QString("export error: %1").arg(ErrorStr));
        }
        return false;
    }
    //导出成功，如果此时被停止，则发出导出失败信号
    emit sigResult(m_canRunning);
    return true && m_canRunning;
}

/**
 * @brief LogExportThread::initMap 初始化等级和对应显示字符的map
 */
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

/**
 * @brief LogExportThread::strTranslate 通过等级字符获取显示字符
 * @param iLevelStr 等级字符
 * @return 显示字符
 */
QString LogExportThread::strTranslate(QString &iLevelStr)
{
    return m_levelStrMap.value(iLevelStr, iLevelStr);
}




/**
 * @brief LogExportThread::run 线程的run函数，通过配置类型执行相应的导出逻辑
 */
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
