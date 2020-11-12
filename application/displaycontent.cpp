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

#include "displaycontent.h"
#include "logapplicationhelper.h"
#include "logexportthread.h"
#include "logfileparser.h"
#include "exportprogressdlg.h"
#include "utils.h"
#include "DebugTimeManager.h"

#include <DApplication>
#include <DApplicationHelper>
#include <DFileDialog>
#include <DFontSizeManager>
#include <DHorizontalLine>
#include <DSplitter>
#include <DScrollBar>
#include <DStandardItem>
#include <DStandardPaths>
#include <DMessageManager>

#include <QAbstractItemView>
#include <QDebug>
#include <QHeaderView>
#include <QtConcurrent>
#include <QIcon>
#include <QPaintEvent>
#include <QPainter>
#include <QProcess>
#include <QProgressDialog>
#include <QThread>
#include <QVBoxLayout>
#include <QElapsedTimer>
#include <QDateTime>

#include <sys/utsname.h>
#include "malloc.h"
DWIDGET_USE_NAMESPACE

#define SINGLE_LOAD 200

#define LEVEL_WIDTH 80
#define STATUS_WIDTH 90
#define DATETIME_WIDTH 175
#define DEAMON_WIDTH 100
/**
 * @brief DisplayContent::DisplayContent 初始化界面\等级数据和实际显示文字转换的数据结构\信号槽连接
 * @param parent
 */
DisplayContent::DisplayContent(QWidget *parent)
    : DWidget(parent)

{
    initUI();
    initMap();
    initConnections();


}

/**
 * @brief DisplayContent::~DisplayContent 析构成员指针,释放空间
 */
DisplayContent::~DisplayContent()
{
    if (m_treeView) {
        delete m_treeView;
        m_treeView = nullptr;
    }
    if (m_pModel) {
        delete m_pModel;
        m_pModel = nullptr;
    }
    malloc_trim(0);
}
/**
 * @brief DisplayContent::mainLogTableView  返回主表控件指针给外部调用
 * @return 主表控件成员指针
 */
LogTreeView *DisplayContent::mainLogTableView()
{
    return  m_treeView;
}


/**
 * @brief DisplayContent::initUI 初始化布局及界面
 */
void DisplayContent::initUI()
{
    QVBoxLayout *vLayout = new QVBoxLayout(this);
    // set table for display log data
    initTableView();

    // layout for widgets
    vLayout->addWidget(m_treeView, 5);

    noResultLabel = new DLabel(this);
    DPalette pa = DApplicationHelper::instance()->palette(noResultLabel);
    pa.setBrush(DPalette::WindowText, pa.color(DPalette::TextTips));
    DApplicationHelper::instance()->setPalette(noResultLabel, pa);

    noResultLabel->setText(DApplication::translate("SearchBar", "No search results"));

    DFontSizeManager::instance()->bind(noResultLabel, DFontSizeManager::T4);
    noResultLabel->setAlignment(Qt::AlignCenter);
    m_spinnerWgt = new LogSpinnerWidget(this);
    m_spinnerWgt_K = new LogSpinnerWidget(this);

    vLayout->addWidget(m_spinnerWgt_K, 5);
    vLayout->addWidget(m_spinnerWgt, 5);
    m_detailWgt = new logDetailInfoWidget(this);
    vLayout->addWidget(m_detailWgt, 3);

    vLayout->setContentsMargins(0, 0, 0, 0);
    vLayout->setSpacing(3);

    this->setLayout(vLayout);
    setLoadState(DATA_COMPLETE);
    //    DGuiApplicationHelper::ColorType ct = DApplicationHelper::instance()->themeType();
    //    slot_themeChanged(ct);
    m_exportDlg = new ExportProgressDlg(this);
    m_exportDlg->setAccessibleName("export_dialog");
    m_exportDlg->hide();
}

/**
 * @brief DisplayContent::initMap 初始化等级数据和实际显示文字转换的数据结构
 */
void DisplayContent::initMap()
{
    m_transDict.clear();
//    m_transDict.insert("Warning", "warning");
//    m_transDict.insert("Debug", "debug");
    m_transDict.insert("Warning", DApplication::translate("Level", "Warning"));  //add by Airy for bug 19167 and 19161
    m_transDict.insert("Debug", DApplication::translate("Level", "Debug")); //add by Airy for bug 19167 and 19161
    m_transDict.insert("Info", DApplication::translate("Level", "Info"));
    m_transDict.insert("Error", DApplication::translate("Level", "Error"));

    // icon <==> level
    m_icon_name_map.clear();
    m_icon_name_map.insert(DApplication::translate("Level", "Emergency"), "warning2.svg");
    m_icon_name_map.insert(DApplication::translate("Level", "Alert"), "warning3.svg");
    m_icon_name_map.insert(DApplication::translate("Level", "Critical"), "warning2.svg");
    m_icon_name_map.insert(DApplication::translate("Level", "Error"), "wrong.svg");
    m_icon_name_map.insert(DApplication::translate("Level", "Warning"), "warning.svg");
    m_icon_name_map.insert(DApplication::translate("Level", "Notice"), "warning.svg");
    m_icon_name_map.insert(DApplication::translate("Level", "Info"), "");
    m_icon_name_map.insert(DApplication::translate("Level", "Debug"), "");
    m_icon_name_map.insert("Warning", "warning.svg");
    m_icon_name_map.insert("Debug", "");
    m_icon_name_map.insert("Error", "wrong.svg");
}

/**
 * @brief DisplayContent::initTableView 初始化主表控件,设置tablemodel
 */
void DisplayContent::initTableView()
{
    m_treeView = new LogTreeView(this);
    m_treeView->setObjectName("mainLogTable");
    m_treeView->setAccessibleName("main_log_table");
    m_pModel = new QStandardItemModel(this);
    m_treeView->setModel(m_pModel);
}

/**
 * @brief DisplayContent::initConnections 初始化槽函数信号连接
 */
void DisplayContent::initConnections()
{
    connect(m_treeView, SIGNAL(clicked(const QModelIndex &)), this,
            SLOT(slot_tableItemClicked(const QModelIndex &)));

    connect(this, SIGNAL(sigDetailInfo(const QModelIndex &, QStandardItemModel *, QString)),
            m_detailWgt, SLOT(slot_DetailInfo(const QModelIndex &, QStandardItemModel *, QString)));
    connect(&m_logFileParse, SIGNAL(dpkgFinished(QList<LOG_MSG_DPKG>)), this, SLOT(slot_dpkgFinished(QList<LOG_MSG_DPKG>)));
    connect(&m_logFileParse, SIGNAL(xlogFinished(QList<LOG_MSG_XORG>)), this, SLOT(slot_XorgFinished(QList<LOG_MSG_XORG>)));
    connect(&m_logFileParse, SIGNAL(bootFinished(QList<LOG_MSG_BOOT>)), this,
            SLOT(slot_bootFinished(QList<LOG_MSG_BOOT>)));
    connect(&m_logFileParse, SIGNAL(kernFinished(QList<LOG_MSG_JOURNAL>)), this,
            SLOT(slot_kernFinished(QList<LOG_MSG_JOURNAL>)));
    connect(&m_logFileParse, SIGNAL(journalFinished()), this, SLOT(slot_journalFinished()),
            Qt::QueuedConnection);
    connect(&m_logFileParse, &LogFileParser::journalData, this, &DisplayContent::slot_journalData,
            Qt::QueuedConnection);
    connect(&m_logFileParse, &LogFileParser::journaBootlData, this, &DisplayContent::slot_journalBootData,
            Qt::QueuedConnection);
    connect(&m_logFileParse, &LogFileParser::applicationFinished, this,
            &DisplayContent::slot_applicationFinished);
    connect(&m_logFileParse, &LogFileParser::kwinFinished, this,
            &DisplayContent::slot_kwinFinished);
    connect(&m_logFileParse, SIGNAL(normalFinished()), this,
            SLOT(slot_NormalFinished()));  // add by Airy
    connect(&m_logFileParse, SIGNAL(journalBootFinished()), this, SLOT(slot_journalBootFinished()));

    connect(m_treeView->verticalScrollBar(), &QScrollBar::valueChanged, this,
            &DisplayContent::slot_vScrollValueChanged);
    connect(&m_logFileParse, &LogFileParser::proccessError, this, &DisplayContent::slot_logLoadFailed,
            Qt::QueuedConnection);
    connect(DApplicationHelper::instance(), &DApplicationHelper::themeTypeChanged, this,
            &DisplayContent::slot_themeChanged);
}

/**
 * @brief DisplayContent::generateJournalFile 获取系统日志
 * @param id 时间筛选id 对应BUTTONID枚举,0表示全部,1是今天,2是3天内,3是筛选1周内数据,4是筛选一个月内的,5是三个月
 * @param lId 等级筛选id,对应PRIORITY枚举,直接传入获取系统接口,-1表示全部等级不筛选,
 * 0表示紧急
 * 1  ALERT（警告）：必须马上采取措施解决的问题
 * 2  CRIT（严重）：比较严重的情况 *
 * 3  ERR（错误）：运行出现错误
 * 4  WARNING（提醒）：可能会影响系统功能的事件
 * 5  NOTICE（注意）：不会影响系统但值得注意
 * 6  INFO（信息）：一般信息
 * 7  DEBUG（调试）：程序或系统调试信息等
 * @param iSearchStr 搜索关键字,现阶段不用,保留参数
 */
void DisplayContent::generateJournalFile(int id, int lId, const QString &iSearchStr)
{
    Q_UNUSED(iSearchStr)
    //系统日志上次获取的时间,和筛选条件一起判断,防止获取过于频繁
    if (m_lastJournalGetTime.msecsTo(QDateTime::currentDateTime()) < 500 && m_journalFilter.timeFilter == id && m_journalFilter.eventTypeFilter == lId) {
        qDebug() << "repeat refrsh journal too fast!";
        QItemSelectionModel *p = m_treeView->selectionModel();
        if (p)
            p->select(m_pModel->index(0, 0), QItemSelectionModel::Rows | QItemSelectionModel::Select);
        slot_tableItemClicked(m_pModel->index(0, 0));
        return;
    }
    m_lastJournalGetTime = QDateTime::currentDateTime();
    m_journalFilter.timeFilter = id;
    m_journalFilter.eventTypeFilter = lId;
    m_firstLoadPageData = true;
    clearAllFilter();
    clearAllDatalist();
    m_firstLoadPageData = true;
    jList.clear();
    jListOrigin.clear();
    createJournalTableForm();
    setLoadState(DATA_LOADING);
    QDateTime dt = QDateTime::currentDateTime();
    dt.setTime(QTime());
    QStringList arg;
    if (lId != LVALL) {
        QString prio = QString("PRIORITY=%1").arg(lId);
        arg.append(prio);
    } else {
        arg.append("all");
    }
    switch (id) {
    case ALL: {
        m_journalCurrentIndex =  m_logFileParse.parseByJournal(arg);
    } break;
    case ONE_DAY: {
        QDateTime dtStart = dt;
        QDateTime dtEnd = dt;
        dtEnd.setTime(QTime(23, 59, 59, 999));
        arg << QString::number(dtStart.toMSecsSinceEpoch() * 1000) << QString::number(dtEnd.toMSecsSinceEpoch() * 1000);
        m_journalCurrentIndex =  m_logFileParse.parseByJournal(arg);
    } break;
    case THREE_DAYS: {
        QDateTime dtStart = dt;
        QDateTime dtEnd = dt;
        dtEnd.setTime(QTime(23, 59, 59, 999));
        arg << QString::number(dtStart.addDays(-2).toMSecsSinceEpoch() * 1000) << QString::number(dtEnd.toMSecsSinceEpoch() * 1000);
        m_journalCurrentIndex =  m_logFileParse.parseByJournal(arg);
    } break;
    case ONE_WEEK: {
        QDateTime dtStart = dt;
        QDateTime dtEnd = dt;
        dtEnd.setTime(QTime(23, 59, 59, 999));
        arg << QString::number(dtStart.addDays(-6).toMSecsSinceEpoch() * 1000) << QString::number(dtEnd.toMSecsSinceEpoch() * 1000);
        m_journalCurrentIndex =   m_logFileParse.parseByJournal(arg);
    } break;
    case ONE_MONTH: {
        QDateTime dtStart = dt;
        QDateTime dtEnd = dt;
        dtEnd.setTime(QTime(23, 59, 59, 999));
        arg << QString::number(dtStart.addMonths(-1).toMSecsSinceEpoch() * 1000) << QString::number(dtEnd.toMSecsSinceEpoch() * 1000);
        m_journalCurrentIndex =    m_logFileParse.parseByJournal(arg);
    } break;
    case THREE_MONTHS: {
        QDateTime dtStart = dt;
        QDateTime dtEnd = dt;
        dtEnd.setTime(QTime(23, 59, 59, 999));
        arg << QString::number(dtStart.addMonths(-3).toMSecsSinceEpoch() * 1000) << QString::number(dtEnd.toMSecsSinceEpoch() * 1000);
        m_journalCurrentIndex =  m_logFileParse.parseByJournal(arg);
    } break;
    default:
        break;
    }
    m_treeView->setColumnWidth(JOURNAL_SPACE::journalLevelColumn, LEVEL_WIDTH);
    m_treeView->setColumnWidth(JOURNAL_SPACE::journalDaemonNameColumn, DEAMON_WIDTH);
    m_treeView->setColumnWidth(JOURNAL_SPACE::journalDateTimeColumn, DATETIME_WIDTH);

}
/**
 * @brief DisplayContent::createJournalTableStart 获取系统日志完成时第一次加载数据的第一页到treeview中
 * @param list 获得的系统日志数据list
 */
void DisplayContent::createJournalTableStart(QList<LOG_MSG_JOURNAL> &list)
{
    m_limitTag = 0;
    // m_pModel->clear();
    setLoadState(DATA_COMPLETE);
    int end = list.count() > SINGLE_LOAD ? SINGLE_LOAD : list.count();
    insertJournalTable(list, 0, end);
}

/**
 * @brief DisplayContent::createJournalTableForm 系统日志表头项目创建和重置
 */
void DisplayContent::createJournalTableForm()
{
    m_pModel->clear();
    m_pModel->setHorizontalHeaderLabels(
        QStringList() << DApplication::translate("Table", "Level")
        << DApplication::translate("Table", "Process")  // modified by Airy
        << DApplication::translate("Table", "Date and Time")
        << DApplication::translate("Table", "Info")
        << DApplication::translate("Table", "User")
        << DApplication::translate("Table", "PID"));
}

/**
 * @brief DisplayContent::generateDpkgFile 触发获取dpkg日志数据线程
 * @param id 时间筛选id 对应BUTTONID枚举,0表示全部,1是今天,2是3天内,3是筛选1周内数据,4是筛选一个月内的,5是三个月
 * @param iSearchStr 搜索关键字,保留字段,暂时不用
 */
void DisplayContent::generateDpkgFile(int id, const QString &iSearchStr)
{
    Q_UNUSED(iSearchStr)
    dList.clear();
    dListOrigin.clear();
    clearAllFilter();
    clearAllDatalist();
    setLoadState(DATA_LOADING);

    QDateTime dt = QDateTime::currentDateTime();
    dt.setTime(QTime());  // get zero time
    DKPG_FILTERS  dpkgFilter;

    switch (id) {
    case ALL:
        m_logFileParse.parseByDpkg(dpkgFilter);
        break;
    case ONE_DAY: {
        QDateTime dtStart = dt;
        QDateTime dtEnd = dt;
        dtEnd.setTime(QTime(23, 59, 59, 999));
        dpkgFilter.timeFilterBegin = dtStart.toMSecsSinceEpoch();
        dpkgFilter.timeFilterEnd = dtEnd.toMSecsSinceEpoch();
        m_logFileParse.parseByDpkg(dpkgFilter);
    } break;
    case THREE_DAYS: {
        QDateTime dtStart = dt;
        QDateTime dtEnd = dt;
        dtEnd.setTime(QTime(23, 59, 59, 999));
        dpkgFilter.timeFilterBegin = dtStart.addDays(-2).toMSecsSinceEpoch();
        dpkgFilter.timeFilterEnd = dtEnd.toMSecsSinceEpoch();
        m_logFileParse.parseByDpkg(dpkgFilter);
    } break;
    case ONE_WEEK: {
        QDateTime dtStart = dt;
        QDateTime dtEnd = dt;
        dtEnd.setTime(QTime(23, 59, 59, 999));
        dpkgFilter.timeFilterBegin = dtStart.addDays(-6).toMSecsSinceEpoch();
        dpkgFilter.timeFilterEnd = dtEnd.toMSecsSinceEpoch();
        m_logFileParse.parseByDpkg(dpkgFilter);
    } break;
    case ONE_MONTH: {
        QDateTime dtStart = dt;
        QDateTime dtEnd = dt;
        dtEnd.setTime(QTime(23, 59, 59, 999));
        dpkgFilter.timeFilterBegin = dtStart.addMonths(-1).toMSecsSinceEpoch();
        dpkgFilter.timeFilterEnd = dtEnd.toMSecsSinceEpoch();
        m_logFileParse.parseByDpkg(dpkgFilter);
    } break;
    case THREE_MONTHS: {
        QDateTime dtStart = dt;
        QDateTime dtEnd = dt;
        dtEnd.setTime(QTime(23, 59, 59, 999));
        dpkgFilter.timeFilterBegin = dtStart.addMonths(-3).toMSecsSinceEpoch();
        dpkgFilter.timeFilterEnd = dtEnd.toMSecsSinceEpoch();
        m_logFileParse.parseByDpkg(dpkgFilter);
    } break;
    default:
        break;
    }
}

/**
 * @brief DisplayContent::createDpkgTable 获取系统日志完成时第一次加载数据的第一页到treeview中
 * @param list 获得的DPKG日志数据list
 */
void DisplayContent::createDpkgTable(QList<LOG_MSG_DPKG> &list)
{
    //    m_treeView->show();
    m_limitTag = 0;
    setLoadState(DATA_COMPLETE);
    m_pModel->clear();
    parseListToModel(list, m_pModel);
    m_treeView->setColumnWidth(0, DATETIME_WIDTH);
    m_treeView->hideColumn(2);
    QItemSelectionModel *p = m_treeView->selectionModel();
    if (p)
        p->select(m_pModel->index(0, 0), QItemSelectionModel::Rows | QItemSelectionModel::Select);
    slot_tableItemClicked(m_pModel->index(0, 0));
}

/**
 * @brief DisplayContent::generateKernFile 触发获取内核日志数据线程
 * @param id 时间筛选id 对应BUTTONID枚举,0表示全部,1是今天,2是3天内,3是筛选1周内数据,4是筛选一个月内的,5是三个月
 * @param iSearchStr 搜索关键字,保留字段,暂时不用
 */
void DisplayContent::generateKernFile(int id, const QString &iSearchStr)
{
    Q_UNUSED(iSearchStr)
    kList.clear();
    kListOrigin.clear();
    clearAllFilter();
    clearAllDatalist();
    setLoadState(DATA_LOADING);
    createKernTableForm();
    QDateTime dt = QDateTime::currentDateTime();
    dt.setTime(QTime());  // get zero time
    KERN_FILTERS kernFilter;
    switch (id) {
    case ALL:
        m_logFileParse.parseByKern(kernFilter);
        break;
    case ONE_DAY: {
        QDateTime dtStart = dt;
        QDateTime dtEnd = dt;
        dtEnd.setTime(QTime(23, 59, 59, 999));
        kernFilter.timeFilterBegin = dtStart.toMSecsSinceEpoch();
        kernFilter.timeFilterEnd = dtEnd.toMSecsSinceEpoch();
        m_logFileParse.parseByKern(kernFilter);
    } break;
    case THREE_DAYS: {
        QDateTime dtStart = dt;
        QDateTime dtEnd = dt;
        dtEnd.setTime(QTime(23, 59, 59, 999));
        kernFilter.timeFilterBegin = dtStart.addDays(-2).toMSecsSinceEpoch();
        kernFilter.timeFilterEnd = dtEnd.toMSecsSinceEpoch();
        m_logFileParse.parseByKern(kernFilter);
    } break;
    case ONE_WEEK: {
        QDateTime dtStart = dt;
        QDateTime dtEnd = dt;
        dtEnd.setTime(QTime(23, 59, 59, 999));
        kernFilter.timeFilterBegin = dtStart.addDays(-6).toMSecsSinceEpoch();
        kernFilter.timeFilterEnd = dtEnd.toMSecsSinceEpoch();
        m_logFileParse.parseByKern(kernFilter);
    } break;
    case ONE_MONTH: {
        QDateTime dtStart = dt;
        QDateTime dtEnd = dt;
        dtEnd.setTime(QTime(23, 59, 59, 999));
        kernFilter.timeFilterBegin = dtStart.addMonths(-1).toMSecsSinceEpoch();
        kernFilter.timeFilterEnd = dtEnd.toMSecsSinceEpoch();
        m_logFileParse.parseByKern(kernFilter);
    } break;
    case THREE_MONTHS: {
        QDateTime dtStart = dt;
        QDateTime dtEnd = dt;
        dtEnd.setTime(QTime(23, 59, 59, 999));
        kernFilter.timeFilterBegin = dtStart.addMonths(-3).toMSecsSinceEpoch();
        kernFilter.timeFilterEnd = dtEnd.toMSecsSinceEpoch();
        m_logFileParse.parseByKern(kernFilter);
    } break;
    default:
        break;
    }

}
/**
 * @brief DisplayContent::createJournalTableForm 内核日志表头项目创建和重置
 */
void DisplayContent::createKernTableForm()
{
    m_pModel->clear();
    m_pModel->setHorizontalHeaderLabels(QStringList()
                                        << DApplication::translate("Table", "Date and Time")
                                        << DApplication::translate("Table", "User")
                                        << DApplication::translate("Table", "Process")
                                        << DApplication::translate("Table", "Info"));
    m_treeView->setColumnWidth(0, DATETIME_WIDTH - 30);
    m_treeView->setColumnWidth(1, DEAMON_WIDTH);
    m_treeView->setColumnWidth(2, DEAMON_WIDTH);
}

/**
 * @brief DisplayContent::createJournalTableStart 获取内核日志完成时第一次加载数据的第一页到treeview中
 * @param list 获得的内核日志数据list
 */
// modified by Airy for bug  12263
void DisplayContent::createKernTable(QList<LOG_MSG_JOURNAL> &list)
{
    //    m_treeView->show();
    setLoadState(DATA_COMPLETE);
    //  m_pModel->clear();

    m_limitTag = 0;
    int end = list.count() > SINGLE_LOAD ? SINGLE_LOAD : list.count();
    insertKernTable(list, 0, end);
}



/**
 * @brief DisplayContent::insertKernTable 按分页转换插入内核日志到treeview的model中
 * @param list 当前筛选状态下所有符合条件的内核日志数据结构
 * @param start 分页开始的数组下标
 * @param end 分页结束的数组下标
 */
// add by Airy for bug
void DisplayContent::insertKernTable(QList<LOG_MSG_JOURNAL> list, int start, int end)
{
    QList<LOG_MSG_JOURNAL> midList = list;
    if (end >= start) {
        midList = midList.mid(start, end - start);
    }
    parseListToModel(midList, m_pModel);
    QItemSelectionModel *p = m_treeView->selectionModel();
    if (p)
        p->select(m_pModel->index(0, 0), QItemSelectionModel::Rows | QItemSelectionModel::Select);
    slot_tableItemClicked(m_pModel->index(0, 0));

}

/**
 * @brief DisplayContent::generateAppFile 触发获取应用日志数据线程
 * @param path 要获取的某一个应用的日志的日志文件路径
 * @param id 时间筛选id 对应BUTTONID枚举,0表示全部,1是今天,2是3天内,3是筛选1周内数据,4是筛选一个月内的,5是三个月
 * @param lId 等级筛选id,对应PRIORITY枚举,直接传入获取系统接口,-1表示全部等级不筛选,
 * @param iSearchStr 搜索关键字,现阶段不用,保留参数
 */
void DisplayContent::generateAppFile(QString path, int id, int lId, const QString &iSearchStr)
{
    Q_UNUSED(iSearchStr)
    appList.clear();
    appListOrigin.clear();
    clearAllFilter();
    clearAllDatalist();
    setLoadState(DATA_LOADING);
    QDateTime dt = QDateTime::currentDateTime();
    dt.setTime(QTime());  // get zero time
    createAppTableForm();
    APP_FILTERS  appFilter;
    appFilter.path = path;
    appFilter.lvlFilter = lId;
    switch (id) {
    case ALL:
        m_logFileParse.parseByApp(appFilter);
        break;
    case ONE_DAY: {
        QDateTime dtStart = dt;
        QDateTime dtEnd = dt;
        dtEnd.setTime(QTime(23, 59, 59, 999));
        appFilter.timeFilterBegin = dtStart.toMSecsSinceEpoch();
        appFilter.timeFilterEnd = dtEnd.toMSecsSinceEpoch();
        m_logFileParse.parseByApp(appFilter);
    } break;
    case THREE_DAYS: {
        QDateTime dtStart = dt;
        QDateTime dtEnd = dt;
        dtEnd.setTime(QTime(23, 59, 59, 999));
        appFilter.timeFilterBegin = dtStart.addDays(-2).toMSecsSinceEpoch();
        appFilter.timeFilterEnd = dtEnd.toMSecsSinceEpoch();
        m_logFileParse.parseByApp(appFilter);

    } break;
    case ONE_WEEK: {
        QDateTime dtStart = dt;
        QDateTime dtEnd = dt;
        dtEnd.setTime(QTime(23, 59, 59, 999));
        appFilter.timeFilterBegin = dtStart.addDays(-6).toMSecsSinceEpoch();
        appFilter.timeFilterEnd = dtEnd.toMSecsSinceEpoch();
        m_logFileParse.parseByApp(appFilter);
    } break;
    case ONE_MONTH: {
        QDateTime dtStart = dt;
        QDateTime dtEnd = dt;
        dtEnd.setTime(QTime(23, 59, 59, 999));
        appFilter.timeFilterBegin = dtStart.addMonths(-1).toMSecsSinceEpoch();
        appFilter.timeFilterEnd = dtEnd.toMSecsSinceEpoch();
        m_logFileParse.parseByApp(appFilter);
    } break;
    case THREE_MONTHS: {
        QDateTime dtStart = dt;
        QDateTime dtEnd = dt;
        dtEnd.setTime(QTime(23, 59, 59, 999));
        appFilter.timeFilterBegin = dtStart.addMonths(-3).toMSecsSinceEpoch();
        appFilter.timeFilterEnd = dtEnd.toMSecsSinceEpoch();
        m_logFileParse.parseByApp(appFilter);
    } break;
    default:
        break;
    }

}

/**
 * @brief DisplayContent::createAppTableForm 系统日志表头项目创建和重置 设置列宽
 */
void DisplayContent::createAppTableForm()
{
    m_pModel->clear();
    m_pModel->setHorizontalHeaderLabels(QStringList()
                                        << DApplication::translate("Table", "Level")
                                        << DApplication::translate("Table", "Date and Time")
                                        << DApplication::translate("Table", "Source")
                                        << DApplication::translate("Table", "Info"));
    m_treeView->setColumnWidth(0, LEVEL_WIDTH);
    m_treeView->setColumnWidth(1, DATETIME_WIDTH + 20);
    m_treeView->setColumnWidth(2, DEAMON_WIDTH);
}

/**
 * @brief DisplayContent::createAppTable 获取应用日志完成时第一次加载数据的第一页到treeview中
 * @param list 获得的应用日志数据list
 */
void DisplayContent::createAppTable(QList<LOG_MSG_APPLICATOIN> &list)
{
    m_limitTag = 0;
    setLoadState(DATA_COMPLETE);
    int end = list.count() > SINGLE_LOAD ? SINGLE_LOAD : list.count();
    insertApplicationTable(list, 0, end);
}

/**
 * @brief DisplayContent::createBootTable 获取启动日志完成时加载所有数据到treeview中
 * @param list 获得的启动日志数据list
 */
void DisplayContent::createBootTable(QList<LOG_MSG_BOOT> &list)
{
    m_limitTag = 0;
    setLoadState(DATA_COMPLETE);
    m_pModel->clear();
    m_treeView->setColumnWidth(0, STATUS_WIDTH);
    parseListToModel(list, m_pModel);
    QItemSelectionModel *p = m_treeView->selectionModel();
    if (p)
        p->select(m_pModel->index(0, 0), QItemSelectionModel::Rows | QItemSelectionModel::Select);
    slot_tableItemClicked(m_pModel->index(0, 0));
}

/**
 * @brief DisplayContent::createXorgTable 获取Xorg日志完成时加载所有数据到treeview中
 * @param list 获得的Xorg日志数据list
 */
void DisplayContent::createXorgTable(QList<LOG_MSG_XORG> &list)
{
    m_limitTag = 0;
    setLoadState(DATA_COMPLETE);
    m_pModel->clear();
    parseListToModel(list, m_pModel);
    m_treeView->setColumnWidth(0, DATETIME_WIDTH + 20);
    QItemSelectionModel *p = m_treeView->selectionModel();
    if (p)
        p->select(m_pModel->index(0, 0), QItemSelectionModel::Rows | QItemSelectionModel::Select);
    slot_tableItemClicked(m_pModel->index(0, 0));
}

/**
 * @brief DisplayContent::generateXorgFile 触发获取Xorg日志数据线程
 * @param id 时间筛选id 对应BUTTONID枚举,0表示全部,1是今天,2是3天内,3是筛选1周内数据,4是筛选一个月内的,5是三个月
 */
void DisplayContent::generateXorgFile(int id)
{
    clearAllFilter();
    xList.clear();
    clearAllDatalist();
    xListOrigin.clear();
    setLoadState(DATA_LOADING);
    QDateTime dt = QDateTime::currentDateTime();
    dt.setTime(QTime());  // get zero time
    XORG_FILTERS xorgFilter;
    switch (id) {
    case ALL:
        m_logFileParse.parseByXlog(xorgFilter);
        break;
    case ONE_DAY: {
        QDateTime dtStart = dt;
        QDateTime dtEnd = dt;
        dtEnd.setTime(QTime(23, 59, 59, 999));
        xorgFilter.timeFilterBegin = dtStart.toMSecsSinceEpoch();
        xorgFilter.timeFilterEnd = dtEnd.toMSecsSinceEpoch();
        m_logFileParse.parseByXlog(xorgFilter);
    } break;
    case THREE_DAYS: {
        QDateTime dtStart = dt;
        QDateTime dtEnd = dt;
        dtEnd.setTime(QTime(23, 59, 59, 999));
        xorgFilter.timeFilterBegin = dtStart.addDays(-2).toMSecsSinceEpoch();
        xorgFilter.timeFilterEnd = dtEnd.toMSecsSinceEpoch();
        m_logFileParse.parseByXlog(xorgFilter);
    } break;
    case ONE_WEEK: {
        QDateTime dtStart = dt;
        QDateTime dtEnd = dt;
        dtEnd.setTime(QTime(23, 59, 59, 999));
        xorgFilter.timeFilterBegin = dtStart.addDays(-6).toMSecsSinceEpoch();
        xorgFilter.timeFilterEnd = dtEnd.toMSecsSinceEpoch();
        m_logFileParse.parseByXlog(xorgFilter);
    } break;
    case ONE_MONTH: {
        QDateTime dtStart = dt;
        QDateTime dtEnd = dt;
        dtEnd.setTime(QTime(23, 59, 59, 999));
        xorgFilter.timeFilterBegin = dtStart.addMonths(-1).toMSecsSinceEpoch();
        xorgFilter.timeFilterEnd = dtEnd.toMSecsSinceEpoch();
        m_logFileParse.parseByXlog(xorgFilter);
    } break;
    case THREE_MONTHS: {
        QDateTime dtStart = dt;
        QDateTime dtEnd = dt;
        dtEnd.setTime(QTime(23, 59, 59, 999));
        xorgFilter.timeFilterBegin = dtStart.addMonths(-3).toMSecsSinceEpoch();
        xorgFilter.timeFilterEnd = dtEnd.toMSecsSinceEpoch();
        m_logFileParse.parseByXlog(xorgFilter);
    } break;
    default:
        break;
    }
}

/**
 * @brief DisplayContent::creatKwinTable 获取kwin日志完成时加载所有数据到treeview中
 * @param list 获得的kwin日志数据list
 */
void DisplayContent::creatKwinTable(QList<LOG_MSG_KWIN> &list)
{
    m_limitTag = 0;
    setLoadState(DATA_COMPLETE);
    m_pModel->clear();
    parseListToModel(list, m_pModel);
    QItemSelectionModel *p = m_treeView->selectionModel();
    if (p)
        p->select(m_pModel->index(0, 0), QItemSelectionModel::Rows | QItemSelectionModel::Select);
    slot_tableItemClicked(m_pModel->index(0, 0));
}

/**
 * @brief DisplayContent::generateKwinFile 触发获取Kwin日志数据线程
 * @param iFilters 获取线程筛选条件结构体
 */
void DisplayContent::generateKwinFile(KWIN_FILTERS iFilters)
{
    clearAllFilter();
    clearAllDatalist();
    m_kwinList.clear();
    m_currentKwinList.clear();
    setLoadState(DATA_LOADING);
    m_logFileParse.parseByKwin(iFilters);
}

/**
 * @brief DisplayContent::createNormalTable 开关机日志表头项目创建和重置
 * @param list
 */
void DisplayContent::createNormalTable(QList<LOG_MSG_NORMAL> &list)
{
    setLoadState(DATA_COMPLETE);
    m_pModel->clear();
    m_limitTag = 0;
    parseListToModel(list, m_pModel);
    m_treeView->setColumnWidth(0, DATETIME_WIDTH - 20);
    m_treeView->setColumnWidth(1, DATETIME_WIDTH);
    m_treeView->setColumnWidth(2, DATETIME_WIDTH);
    m_treeView->setColumnWidth(3, DATETIME_WIDTH);
    QItemSelectionModel *p = m_treeView->selectionModel();
    if (p)
        p->select(m_pModel->index(0, 0), QItemSelectionModel::Rows | QItemSelectionModel::Select);
    slot_tableItemClicked(m_pModel->index(0, 0));
}


/**
 * @brief DisplayContent::generateNormalFile 触发获取开关机日志数据线程
 * @param id 时间筛选id 对应BUTTONID枚举,0表示全部,1是今天,2是3天内,3是筛选1周内数据,4是筛选一个月内的,5是三个月
 */
// add by Airy
void DisplayContent::generateNormalFile(int id)
{
    clearAllFilter();
    clearAllDatalist();
    norList.clear();
    nortempList.clear();
    setLoadState(DATA_LOADING);

    QDateTime dt = QDateTime::currentDateTime();
    dt.setTime(QTime());  // get zero time

    switch (id) {
    case ALL:
        m_normalFilter.timeFilterEnd = -1;
        m_normalFilter.timeFilterBegin = -1;
        m_logFileParse.parseByNormal(norList, m_normalFilter);
        break;
    case ONE_DAY: {
        QDateTime dtStart = dt;
        QDateTime dtEnd = dt;
        dtEnd.setTime(QTime(23, 59, 59, 999));
        m_normalFilter.timeFilterBegin = dtStart.toMSecsSinceEpoch();
        m_normalFilter.timeFilterEnd = dtEnd.toMSecsSinceEpoch();
        m_logFileParse.parseByNormal(norList, m_normalFilter);
    } break;
    case THREE_DAYS: {
        QDateTime dtStart = dt;
        QDateTime dtEnd = dt;
        dtEnd.setTime(QTime(23, 59, 59, 999));
        m_normalFilter.timeFilterBegin = dtStart.addDays(-2).toMSecsSinceEpoch();
        m_normalFilter.timeFilterEnd = dtEnd.toMSecsSinceEpoch();
        m_logFileParse.parseByNormal(norList, m_normalFilter);
    } break;
    case ONE_WEEK: {
        QDateTime dtStart = dt;
        QDateTime dtEnd = dt;
        dtEnd.setTime(QTime(23, 59, 59, 999));
        m_normalFilter.timeFilterBegin = dtStart.addDays(-6).toMSecsSinceEpoch();
        m_normalFilter.timeFilterEnd = dtEnd.toMSecsSinceEpoch();
        m_logFileParse.parseByNormal(norList, m_normalFilter);
    } break;
    case ONE_MONTH: {
        QDateTime dtStart = dt;
        QDateTime dtEnd = dt;
        dtEnd.setTime(QTime(23, 59, 59, 999));
        m_normalFilter.timeFilterBegin = dtStart.addMonths(-1).toMSecsSinceEpoch();
        m_normalFilter.timeFilterEnd = dtEnd.toMSecsSinceEpoch();
        m_logFileParse.parseByNormal(norList, m_normalFilter);
    } break;
    case THREE_MONTHS: {
        QDateTime dtStart = dt;
        QDateTime dtEnd = dt;
        dtEnd.setTime(QTime(23, 59, 59, 999));
        m_normalFilter.timeFilterBegin = dtStart.addMonths(-3).toMSecsSinceEpoch();
        m_normalFilter.timeFilterEnd = dtEnd.toMSecsSinceEpoch();
        m_logFileParse.parseByNormal(norList, m_normalFilter);
    } break;
    default:
        break;
    }
    nortempList = norList;
}

/**
 * @brief DisplayContent::insertJournalTable 按分页转换插入系统日志到treeview的model中
 * @param logList 当前筛选状态下所有符合条件的系统日志数据结构
 * @param start 分页开始的数组下标
 * @param end 分页结束的数组下标
 */
void DisplayContent::insertJournalTable(QList<LOG_MSG_JOURNAL> logList, int start, int end)
{
    DStandardItem *item = nullptr;
    //  m_treeView->setUpdatesEnabled(false);
    // m_pModel->beginInsertRows(logList.size());
    QList<QStandardItem *> items;
    for (int i = start; i < end; i++) {
        // int col = 0;
        items.clear();
        item = new DStandardItem();
        //        qDebug() << "journal level" << logList[i].level;
        QString iconPath = m_iconPrefix + getIconByname(logList[i].level);

        if (getIconByname(logList[i].level).isEmpty())
            item->setText(logList[i].level);
        item->setIcon(QIcon(iconPath));
        item->setData(JOUR_TABLE_DATA);
        item->setData(logList[i].level, Log_Item_SPACE::levelRole);
        //m_pModel->setItem(i, JOURNAL_SPACE::journalLevelColumn, item);
        item->setAccessibleText(QString("treeview_context_%1_%2").arg(i).arg(0));
        items << item;
        item = new DStandardItem(logList[i].daemonName);
        item->setData(JOUR_TABLE_DATA);
        // m_pModel->setItem(i, JOURNAL_SPACE::journalDaemonNameColumn, item);
        item->setAccessibleText(QString("treeview_context_%1_%2").arg(i).arg(1));
        items << item;
        item = new DStandardItem(logList[i].dateTime);
        item->setData(JOUR_TABLE_DATA);
        //m_pModel->setItem(i, JOURNAL_SPACE::journalDateTimeColumn, item);
        item->setAccessibleText(QString("treeview_context_%1_%2").arg(i).arg(2));
        items << item;
        item = new DStandardItem(logList[i].msg);
        item->setData(JOUR_TABLE_DATA);
        //m_pModel->setItem(i, JOURNAL_SPACE::journalMsgColumn, item);
        item->setAccessibleText(QString("treeview_context_%1_%2").arg(i).arg(3));
        items << item;
        item = new DStandardItem(logList[i].hostName);
        item->setData(JOUR_TABLE_DATA);
        // m_pModel->setItem(i, JOURNAL_SPACE::journalHostNameColumn, item);
        item->setAccessibleText(QString("treeview_context_%1_%2").arg(i).arg(4));
        items << item;
        item = new DStandardItem(logList[i].daemonId);
        item->setData(JOUR_TABLE_DATA);
        //  m_pModel->setItem(i, JOURNAL_SPACE::journalDaemonIdColumn, item);
        item->setAccessibleText(QString("treeview_context_%1_%2").arg(i).arg(5));
        items << item;
        m_pModel->insertRow(m_pModel->rowCount(), items);
    }
    //  m_pModel->endInsertRows();
    m_treeView->hideColumn(JOURNAL_SPACE::journalHostNameColumn);
    m_treeView->hideColumn(JOURNAL_SPACE::journalDaemonIdColumn);
    //m_treeView->setUpdatesEnabled(true);
    //    qDebug() << m_pModel->index(0, 0).data(Qt::DecorationRole);

    //    m_treeView->setModel(m_pModel);


    QItemSelectionModel *p = m_treeView->selectionModel();
    if (p)
        p->select(m_pModel->index(0, 0), QItemSelectionModel::Rows | QItemSelectionModel::Select);
    slot_tableItemClicked(m_pModel->index(0, 0));
    // default first row select
//    m_treeView->setColumnWidth(JOURNAL_SPACE::journalLevelColumn, LEVEL_WIDTH);
//    m_treeView->setColumnWidth(JOURNAL_SPACE::journalDaemonNameColumn, DEAMON_WIDTH);
//    m_treeView->setColumnWidth(JOURNAL_SPACE::journalDateTimeColumn, DATETIME_WIDTH);
}

/**
 * @brief DisplayContent::getAppName 获取当前选择的应用的日志路径对应的日志名称
 * @param filePath  当前选择的应用的日志路径
 * @return 对应的日志名称
 */
QString DisplayContent::getAppName(QString filePath)
{
    QString ret;
    if (filePath.isEmpty())
        return ret;

    QStringList strList = filePath.split("/");
    if (strList.count() < 2) {
        if (filePath.contains("."))
            ret = filePath.section(".", 0, 0);
        else {
            ret = filePath;
        }
        return LogApplicationHelper::instance()->transName(ret);
    }

    QString desStr = filePath.section("/", -1);
    if (desStr.contains(".")) {
        ret = desStr.section(".", 0, 0);
    }
    return LogApplicationHelper::instance()->transName(ret);
}

/**
 * @brief DisplayContent::isAuthProcessAlive 日志获取进程是否存在
 * @return true:存在 false: 不存在
 */
bool DisplayContent::isAuthProcessAlive()
{
    bool ret = false;
    QProcess proc;
    int rslt = proc.execute("ps -aux | grep 'logViewerAuth'");
    qDebug() << rslt << "*************";
    return !(ret = (rslt == 0));
}

/**
 * @brief DisplayContent::generateJournalBootFile 触发获取klu下启动日志的线程
 * @param lId 等级筛选id,对应PRIORITY枚举,直接传入获取系统接口,-1表示全部等级不筛选,
 * 0表示紧急
 * 1  ALERT（警告）：必须马上采取措施解决的问题
 * 2  CRIT（严重）：比较严重的情况 *
 * 3  ERR（错误）：运行出现错误
 * 4  WARNING（提醒）：可能会影响系统功能的事件
 * 5  NOTICE（注意）：不会影响系统但值得注意
 * 6  INFO（信息）：一般信息
 * 7  DEBUG（调试）：程序或系统调试信息等
 * @param iSearchStr iSearchStr 搜索关键字,现阶段不用,保留参数
 */
void DisplayContent::generateJournalBootFile(int lId, const QString &iSearchStr)
{
    Q_UNUSED(iSearchStr)
    m_firstLoadPageData = true;
    clearAllFilter();
    clearAllDatalist();
    m_firstLoadPageData = true;
    createJournalBootTableForm();
    setLoadState(DATA_LOADING);
    QDateTime dt = QDateTime::currentDateTime();
    dt.setTime(QTime());
    QStringList arg;
    if (lId != LVALL) {
        QString prio = QString("PRIORITY=%1").arg(lId);
        arg.append(prio);
    } else {
        arg.append("all");
    }
    m_journalBootCurrentIndex = m_logFileParse.parseByJournalBoot(arg);
    // default first row select
    m_treeView->setColumnWidth(JOURNAL_SPACE::journalLevelColumn, LEVEL_WIDTH);
    m_treeView->setColumnWidth(JOURNAL_SPACE::journalDaemonNameColumn, DEAMON_WIDTH);
    m_treeView->setColumnWidth(JOURNAL_SPACE::journalDateTimeColumn, DATETIME_WIDTH);

}

/**
 * @brief DisplayContent::createJournalBootTableStart 获取klu下启动日志完成时第一次加载数据的第一页到treeview中
 * @param list klu下启动日志数据list
 */
void DisplayContent::createJournalBootTableStart(QList<LOG_MSG_JOURNAL> &list)
{
    m_limitTag = 0;
    setLoadState(DATA_COMPLETE);
    int end = list.count() > SINGLE_LOAD ? SINGLE_LOAD : list.count();
    insertJournalBootTable(list, 0, end);
}

/**
 * @brief DisplayContent::createJournalBootTableForm klu下启动日志表头项目创建和重置
 */
void DisplayContent::createJournalBootTableForm()
{
    m_pModel->clear();
    //m_pModel->setColumnCount(6);
    m_pModel->setHorizontalHeaderLabels(
        QStringList() << DApplication::translate("Table", "Level")
        << DApplication::translate("Table", "Process")  // modified by Airy
        << DApplication::translate("Table", "Date and Time")
        << DApplication::translate("Table", "Info")
        << DApplication::translate("Table", "User")
        << DApplication::translate("Table", "PID"));
}

/**
 * @brief DisplayContent::insertJournalBootTable 按分页转换插入klu下启动日志到treeview的model中
 * @param logList 当前筛选状态下所有符合条件的klu下启动日志数据结构
 * @param start 分页开始的数组下标
 * @param end 分页结束的数组下标
 */
void DisplayContent::insertJournalBootTable(QList<LOG_MSG_JOURNAL> logList, int start, int end)
{
    DStandardItem *item = nullptr;
    //  m_treeView->setUpdatesEnabled(false);
    // m_pModel->beginInsertRows(logList.size());
    QList<QStandardItem *> items;
    for (int i = start; i < end; i++) {
        // int col = 0;
        items.clear();
        item = new DStandardItem();
        //        qDebug() << "journal level" << logList[i].level;
        QString iconPath = m_iconPrefix + getIconByname(logList[i].level);

        if (getIconByname(logList[i].level).isEmpty())
            item->setText(logList[i].level);
        item->setIcon(QIcon(iconPath));
        item->setData(BOOT_KLU_TABLE_DATA);
        item->setData(logList[i].level, Log_Item_SPACE::levelRole);
        //m_pModel->setItem(i, JOURNAL_SPACE::journalLevelColumn, item);
        item->setAccessibleText(QString("treeview_context_%1_%2").arg(i).arg(0));
        items << item;
        item = new DStandardItem(logList[i].daemonName);
        item->setData(BOOT_KLU_TABLE_DATA);
        // m_pModel->setItem(i, JOURNAL_SPACE::journalDaemonNameColumn, item);
        item->setAccessibleText(QString("treeview_context_%1_%2").arg(i).arg(1));
        items << item;
        item = new DStandardItem(logList[i].dateTime);
        item->setData(BOOT_KLU_TABLE_DATA);
        //m_pModel->setItem(i, JOURNAL_SPACE::journalDateTimeColumn, item);
        item->setAccessibleText(QString("treeview_context_%1_%2").arg(i).arg(2));
        items << item;
        item = new DStandardItem(logList[i].msg);
        item->setData(BOOT_KLU_TABLE_DATA);
        //m_pModel->setItem(i, JOURNAL_SPACE::journalMsgColumn, item);
        item->setAccessibleText(QString("treeview_context_%1_%2").arg(i).arg(3));
        items << item;
        item = new DStandardItem(logList[i].hostName);
        item->setData(BOOT_KLU_TABLE_DATA);
        // m_pModel->setItem(i, JOURNAL_SPACE::journalHostNameColumn, item);
        item->setAccessibleText(QString("treeview_context_%1_%2").arg(i).arg(4));
        items << item;
        item = new DStandardItem(logList[i].daemonId);
        item->setData(BOOT_KLU_TABLE_DATA);
        item->setAccessibleText(QString("treeview_context_%1_%2").arg(i).arg(5));
        //  m_pModel->setItem(i, JOURNAL_SPACE::journalDaemonIdColumn, item);
        items << item;
        m_pModel->insertRow(m_pModel->rowCount(), items);
    }
    //  m_pModel->endInsertRows();
    m_treeView->hideColumn(JOURNAL_SPACE::journalHostNameColumn);
    m_treeView->hideColumn(JOURNAL_SPACE::journalDaemonIdColumn);
    //m_treeView->setUpdatesEnabled(true);
    //    qDebug() << m_pModel->index(0, 0).data(Qt::DecorationRole);

    //    m_treeView->setModel(m_pModel);


    QItemSelectionModel *p = m_treeView->selectionModel();
    if (p)
        p->select(m_pModel->index(0, 0), QItemSelectionModel::Rows | QItemSelectionModel::Select);
    slot_tableItemClicked(m_pModel->index(0, 0));
}

/**
 * @brief DisplayContent::slot_tableItemClicked treeview主表点击槽函数,用来发出信号在详情页显示当前选中项日志详细信息
 * @param index 选中的modelindex
 */
void DisplayContent::slot_tableItemClicked(const QModelIndex &index)
{
    emit sigDetailInfo(index, m_pModel, getAppName(m_curAppLog));
}

/**
 * @brief DisplayContent::slot_BtnSelected 连接外部筛选控件筛选条件处理触发获取对应数据的槽函数
 * @param btnId 时间筛选id 对应BUTTONID枚举,0表示全部,1是今天,2是3天内,3是筛选1周内数据,4是筛选一个月内的,5是三个月
 * @param lId 等级筛选id,对应PRIORITY枚举,直接传入获取系统接口,-1表示全部等级不筛选,
 * @param idx 日志类型选择的listview的当前选中的QModelIndex
 */
void DisplayContent::slot_BtnSelected(int btnId, int lId, QModelIndex idx)
{
    qDebug() << QString("Button %1 clicked\n combobox: level is %2, cbxIdx is %3 tree %4 node!!")
             .arg(btnId)
             .arg(lId)
             .arg(lId + 1)
             .arg(idx.data(ITEM_DATE_ROLE).toString());

    m_detailWgt->cleanText();

    m_curLevel = lId;  // m_curLevel equal combobox index-1;
    m_curBtnId = btnId;

    QString treeData = idx.data(ITEM_DATE_ROLE).toString();
    if (treeData.isEmpty())
        return;

    if (treeData.contains(JOUR_TREE_DATA, Qt::CaseInsensitive)) {
        generateJournalFile(btnId, m_curLevel);
    } else if (treeData.contains(BOOT_KLU_TREE_DATA, Qt::CaseInsensitive)) {
        generateJournalBootFile(m_curLevel);
    }  else if (treeData.contains(DPKG_TREE_DATA, Qt::CaseInsensitive)) {
        generateDpkgFile(btnId);
    } else if (treeData.contains(KERN_TREE_DATA, Qt::CaseInsensitive)) {
        generateKernFile(btnId);
    } else if (treeData.contains(".cache")) {
        //        generateAppFile(treeData, btnId, lId);
    } else if (treeData.contains(APP_TREE_DATA, Qt::CaseInsensitive)) {
        if (!m_curAppLog.isEmpty()) {
            generateAppFile(m_curAppLog, btnId, m_curLevel);
        }
    } else if (treeData.contains(XORG_TREE_DATA, Qt::CaseInsensitive)) {  // add by Airy
        generateXorgFile(btnId);
    } else if (treeData.contains(LAST_TREE_DATA, Qt::CaseInsensitive)) {  // add by Airy
        generateNormalFile(btnId);
    }
}

/**
 * @brief DisplayContent::slot_appLogs 根据应用日志应用类型变化触发应用日志获取线程
 * @param path 应用日志的路径
 */
void DisplayContent::slot_appLogs(QString path)
{
    appList.clear();
    m_curAppLog = path;
    generateAppFile(path, m_curBtnId, m_curLevel);
}

/**
 * @brief DisplayContent::slot_logCatelogueClicked 日志类型选择列表选项目变化根据日志类型获取数据
 * @param index loglistView当前选择的项目
 */
void DisplayContent::slot_logCatelogueClicked(const QModelIndex &index)
{
    if (!index.isValid()) {
        return;
    }

    if (m_curListIdx == index && (m_flag != KERN && m_flag != BOOT)) {
        qDebug() << "repeat click" << m_flag;
        return;
    }
    m_currentKwinFilter = {""};
    m_curListIdx = index;


    clearAllDatalist();

    QString itemData = index.data(ITEM_DATE_ROLE).toString();
    if (itemData.isEmpty())
        return;

    if (itemData.contains(JOUR_TREE_DATA, Qt::CaseInsensitive)) {
        // default level is info so PRIORITY=6
        m_flag = JOURNAL;
        generateJournalFile(m_curBtnId, m_curLevel);
    } else if (itemData.contains(DPKG_TREE_DATA, Qt::CaseInsensitive)) {
        m_flag = DPKG;
        generateDpkgFile(m_curBtnId);
    } else if (itemData.contains(XORG_TREE_DATA, Qt::CaseInsensitive)) {
        xList.clear();
        m_flag = XORG;
        //        m_logFileParse.parseByXlog(xList);
        generateXorgFile(m_curBtnId);
    } else if (itemData.contains(BOOT_TREE_DATA, Qt::CaseInsensitive)) {
        bList.clear();
        setLoadState(DATA_LOADING);
        m_flag = BOOT;
        m_logFileParse.parseByBoot();
    } else if (itemData.contains(KERN_TREE_DATA, Qt::CaseInsensitive)) {
        m_flag = KERN;
        generateKernFile(m_curBtnId);
    } else if (itemData.contains(".cache")) {
    } else if (itemData.contains(APP_TREE_DATA, Qt::CaseInsensitive)) {
        m_pModel->clear();  // clicked parent node application, clear table contents
        m_flag = APP;
    } else if (itemData.contains(LAST_TREE_DATA, Qt::CaseInsensitive)) {
        norList.clear();
        m_flag = Normal;
        //        m_logFileParse.parseByNormal(norList);
        generateNormalFile(m_curBtnId);
    } else if (itemData.contains(KWIN_TREE_DATA, Qt::CaseInsensitive)) {
        setLoadState(DATA_LOADING);
        m_kwinList.clear();
        m_currentKwinList.clear();
        m_flag = Kwin;
        m_logFileParse.parseByKwin(m_currentKwinFilter);
    } else if (itemData.contains(BOOT_KLU_TREE_DATA, Qt::CaseInsensitive)) {
        m_flag = BOOT_KLU;
        generateJournalBootFile(m_curLevel);
    }

}

/**
 * @brief DisplayContent::slot_exportClicked 导出按钮触发槽函数,根据当前筛选出来的数据和数据类型,弹出文件目录选择框选择导出目录,执行导出逻辑
 */
void DisplayContent::slot_exportClicked()
{
    QString logName;
    if (m_curListIdx.isValid())
        logName = QString("/%1").arg(m_curListIdx.data().toString());
    else {
        logName = QString("/%1").arg(("New File"));
    }
    QString selectFilter;
    QString path = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation) +  logName + ".txt";
    QString fileName = DFileDialog::getSaveFileName(
                           this, DApplication::translate("File", "Export File"),
                           path,
                           tr("TEXT (*.txt);; Doc (*.doc);; Xls (*.xls);; Html (*.html)"), &selectFilter);

    //    QString fileName = "";
    //    DFileDialog dialog(this);
    //    dialog.setWindowTitle(DApplication::translate("File", "Export File"));

    //    dialog.setAcceptMode(QFileDialog::AcceptSave);//设置文件对话框为保存模式
    //    dialog.setViewMode(DFileDialog::List);
    //    dialog.setDirectory(QDir::homePath() + "/Documents");
    //    //dialog.selectFile(tr("Unnamed.ddf"));//设置默认的文件名
    //    dialog.selectFile(logName + ".doc"); //设置默认的文件名
    //    QStringList nameFilters;
    //    nameFilters << "*.doc" << "*.txt"  << "*.xls" << "*.html"; //<< "*.doc" << "*.xls" << "*.html"
    //    dialog.setNameFilters(nameFilters); //设置文件类型过滤器
    //    if (dialog.exec()) {
    //        fileName = dialog.selectedFiles().first();
    //    }
    if (fileName.isEmpty())
        return;

    m_exportDlg->show();
    QStringList labels;
    for (int col = 0; col < m_pModel->columnCount(); ++col) {
        labels.append(m_pModel->horizontalHeaderItem(col)->text());
    }
    //根据导出格式判断执行逻辑
    if (selectFilter == "TEXT (*.txt)") {
        LogExportThread *exportThread = new LogExportThread(this);
        connect(m_exportDlg, &ExportProgressDlg::sigCloseBtnClicked, exportThread, &LogExportThread::stopImmediately);
        connect(exportThread, &LogExportThread::sigResult, this, &DisplayContent::onExportResult);
        connect(exportThread, &LogExportThread::sigProgress, this, &DisplayContent::onExportProgress);
        switch (m_flag) {
        //根据导出日志类型执行正确的导出逻辑
        case JOURNAL:
            PERF_PRINT_BEGIN("POINT-04", QString("format=txt count=%1").arg(jList.count()));
            exportThread->exportToTxtPublic(fileName, jList, labels, m_flag);
            break;
        case BOOT_KLU:
            PERF_PRINT_BEGIN("POINT-04", QString("format=txt count=%1").arg(jBootList.count()));
            exportThread->exportToTxtPublic(fileName, jBootList, labels, JOURNAL);
            break;
        case APP: {
            PERF_PRINT_BEGIN("POINT-04", QString("format=txt count=%1").arg(appList.count()));
            QString appName = getAppName(m_curAppLog);
            exportThread->exportToTxtPublic(fileName, appList, labels, appName);
            break;
        }
        case DPKG:
            PERF_PRINT_BEGIN("POINT-04", QString("format=txt count=%1").arg(dList.count()));
            exportThread->exportToTxtPublic(fileName, dList, labels);
            break;
        case BOOT:
            PERF_PRINT_BEGIN("POINT-04", QString("format=txt count=%1").arg(currentBootList.count()));
            exportThread->exportToTxtPublic(fileName, currentBootList, labels);
            break;
        case XORG:
            PERF_PRINT_BEGIN("POINT-04", QString("format=txt count=%1").arg(xList.count()));
            exportThread->exportToTxtPublic(fileName, xList, labels);
            break;
        case Normal:
            PERF_PRINT_BEGIN("POINT-04", QString("format=txt count=%1").arg(nortempList.count()));
            exportThread->exportToTxtPublic(fileName, nortempList, labels);
            break;
        case KERN:
            PERF_PRINT_BEGIN("POINT-04", QString("format=txt count=%1").arg(kList.count()));
            exportThread->exportToTxtPublic(fileName, kList, labels, m_flag);
            break;
        case Kwin:
            PERF_PRINT_BEGIN("POINT-04", QString("format=txt count=%1").arg(m_currentKwinList.count()));
            exportThread->exportToTxtPublic(fileName, m_currentKwinList, labels);
            break;
        default:
            break;
        }
        QThreadPool::globalInstance()->start(exportThread);
    } else if (selectFilter == "Html (*.html)") {
        LogExportThread *exportThread = new LogExportThread(this);
        connect(m_exportDlg, &ExportProgressDlg::sigCloseBtnClicked, exportThread, &LogExportThread::stopImmediately);
        connect(exportThread, &LogExportThread::sigResult, this, &DisplayContent::onExportResult);
        connect(exportThread, &LogExportThread::sigProgress, this, &DisplayContent::onExportProgress);
        switch (m_flag) {
        case JOURNAL:
            PERF_PRINT_BEGIN("POINT-04", QString("format=html count=%1").arg(jList.count()));
            exportThread->exportToHtmlPublic(fileName, jList, labels, m_flag);
            break;
        case BOOT_KLU:
            PERF_PRINT_BEGIN("POINT-04", QString("format=html count=%1").arg(jBootList.count()));
            exportThread->exportToHtmlPublic(fileName, jBootList, labels, JOURNAL);
            break;
        case APP: {
            PERF_PRINT_BEGIN("POINT-04", QString("format=html count=%1").arg(appList.count()));
            QString appName = getAppName(m_curAppLog);
            exportThread->exportToHtmlPublic(fileName, appList, labels, appName);
            break;
        }
        case DPKG:
            PERF_PRINT_BEGIN("POINT-04", QString("format=html count=%1").arg(dList.count()));
            exportThread->exportToHtmlPublic(fileName, dList, labels);
            break;
        case BOOT:
            PERF_PRINT_BEGIN("POINT-04", QString("format=html count=%1").arg(currentBootList.count()));
            exportThread->exportToHtmlPublic(fileName, currentBootList, labels);
            break;
        case XORG:
            PERF_PRINT_BEGIN("POINT-04", QString("format=html count=%1").arg(xList.count()));
            exportThread->exportToHtmlPublic(fileName, xList, labels);
            break;
        case Normal:
            PERF_PRINT_BEGIN("POINT-04", QString("format=html count=%1").arg(nortempList.count()));
            exportThread->exportToHtmlPublic(fileName, nortempList, labels);
            break;
        case KERN:
            PERF_PRINT_BEGIN("POINT-04", QString("format=html count=%1").arg(kList.count()));
            exportThread->exportToHtmlPublic(fileName, kList, labels, m_flag);
            break;
        case Kwin:
            PERF_PRINT_BEGIN("POINT-04", QString("format=html count=%1").arg(m_currentKwinList.count()));
            exportThread->exportToHtmlPublic(fileName, m_currentKwinList, labels);
            break;
        default:
            break;
        }
        QThreadPool::globalInstance()->start(exportThread);
    } else if (selectFilter == "Doc (*.doc)") {
        LogExportThread *exportThread = new LogExportThread(this);
        connect(m_exportDlg, &ExportProgressDlg::sigCloseBtnClicked, exportThread, &LogExportThread::stopImmediately);
        connect(exportThread, &LogExportThread::sigResult, this, &DisplayContent::onExportResult);
        connect(exportThread, &LogExportThread::sigProgress, this, &DisplayContent::onExportProgress);
        connect(exportThread, &LogExportThread::sigProcessFull, this, &DisplayContent::onExportFakeCloseDlg);

        switch (m_flag) {
        case JOURNAL:
            PERF_PRINT_BEGIN("POINT-04", QString("format=doc count=%1").arg(jList.count()));
            exportThread->exportToDocPublic(fileName, jList, labels, m_flag);
            break;
        case BOOT_KLU:
            PERF_PRINT_BEGIN("POINT-04", QString("format=doc count=%1").arg(jBootList.count()));
            exportThread->exportToDocPublic(fileName, jBootList, labels, JOURNAL);
            break;
        case APP: {
            PERF_PRINT_BEGIN("POINT-04", QString("format=doc count=%1").arg(appList.count()));
            QString appName = getAppName(m_curAppLog);
            exportThread->exportToDocPublic(fileName, appList, labels, appName);
            break;
        }
        case DPKG:
            PERF_PRINT_BEGIN("POINT-04", QString("format=doc count=%1").arg(dList.count()));
            exportThread->exportToDocPublic(fileName, dList, labels);
            break;
        case BOOT:
            PERF_PRINT_BEGIN("POINT-04", QString("format=doc count=%1").arg(currentBootList.count()));
            exportThread->exportToDocPublic(fileName, currentBootList, labels);
            break;
        case XORG:
            PERF_PRINT_BEGIN("POINT-04", QString("format=doc count=%1").arg(xList.count()));
            exportThread->exportToDocPublic(fileName, xList, labels);
            break;
        case Normal:
            PERF_PRINT_BEGIN("POINT-04", QString("format=doc count=%1").arg(nortempList.count()));
            exportThread->exportToDocPublic(fileName, nortempList, labels);
            break;
        case KERN:
            PERF_PRINT_BEGIN("POINT-04", QString("format=doc count=%1").arg(kList.count()));
            exportThread->exportToDocPublic(fileName, kList, labels, m_flag);
            break;
        case Kwin:
            PERF_PRINT_BEGIN("POINT-04", QString("format=doc count=%1").arg(m_currentKwinList.count()));
            exportThread->exportToDocPublic(fileName, m_currentKwinList, labels);
            break;
        default:
            break;
        }
        QThreadPool::globalInstance()->start(exportThread);
    } else if (selectFilter == "Xls (*.xls)") {
        LogExportThread *exportThread = new LogExportThread(this);
        connect(m_exportDlg, &ExportProgressDlg::sigCloseBtnClicked, exportThread, &LogExportThread::stopImmediately);
        connect(exportThread, &LogExportThread::sigResult, this, &DisplayContent::onExportResult);
        connect(exportThread, &LogExportThread::sigProgress, this, &DisplayContent::onExportProgress);
        connect(exportThread, &LogExportThread::sigProcessFull, this, &DisplayContent::onExportFakeCloseDlg);
        switch (m_flag) {
        case JOURNAL:
            PERF_PRINT_BEGIN("POINT-04", QString("format=xls count=%1").arg(jList.count()));
            exportThread->exportToXlsPublic(fileName, jList, labels, m_flag);
            break;
        case BOOT_KLU:
            PERF_PRINT_BEGIN("POINT-04", QString("format=xls count=%1").arg(jBootList.count()));
            exportThread->exportToXlsPublic(fileName, jBootList, labels, JOURNAL);
            break;
        case APP: {
            PERF_PRINT_BEGIN("POINT-04", QString("format=xls count=%1").arg(appList.count()));
            QString appName = getAppName(m_curAppLog);
            exportThread->exportToXlsPublic(fileName, appList, labels, appName);
            break;
        }
        case DPKG:
            PERF_PRINT_BEGIN("POINT-04", QString("format=xls count=%1").arg(dList.count()));
            exportThread->exportToXlsPublic(fileName, dList, labels);
            break;
        case BOOT:
            PERF_PRINT_BEGIN("POINT-04", QString("format=xls count=%1").arg(currentBootList.count()));
            exportThread->exportToXlsPublic(fileName, currentBootList, labels);
            break;
        case XORG:
            PERF_PRINT_BEGIN("POINT-04", QString("format=xls count=%1").arg(xList.count()));
            exportThread->exportToXlsPublic(fileName, xList, labels);
            break;
        case Normal:
            PERF_PRINT_BEGIN("POINT-04", QString("format=xls count=%1").arg(nortempList.count()));
            exportThread->exportToXlsPublic(fileName, nortempList, labels);
            break;
        case KERN:
            PERF_PRINT_BEGIN("POINT-04", QString("format=xls count=%1").arg(kList.count()));
            exportThread->exportToXlsPublic(fileName, kList, labels, m_flag);
            break;
        case Kwin:
            PERF_PRINT_BEGIN("POINT-04", QString("format=xls count=%1").arg(m_currentKwinList.count()));
            exportThread->exportToXlsPublic(fileName, m_currentKwinList, labels);
            break;
        default:
            break;
        }
        QThreadPool::globalInstance()->start(exportThread);
    }
}

/**
 * @brief DisplayContent::slot_statusChagned 启动日志的状态combox选项改变槽函数,筛选当前启动日志内容
 * @param status 筛选的状态,有all ok failed
 */
void DisplayContent::slot_statusChagned(QString status)
{
    m_bootFilter.statusFilter = status;
    filterBoot(m_bootFilter);

}

/**
 * @brief DisplayContent::slot_dpkgFinished 获取dpkg日志数据线程结果的槽函数,把获取到的数据加入treeview的model中以显示
 * @param list dpkg日志数据线程list
 */
void DisplayContent::slot_dpkgFinished(QList<LOG_MSG_DPKG> list)
{
    if (m_flag != DPKG)
        return;
    dList = list;
    dListOrigin = list;
    createDpkgTable(dList);
    PERF_PRINT_END("POINT-03", "type=dpkg");
}

/**
 * @brief DisplayContent::slot_XorgFinished 获取xorg日志数据线程结果的槽函数,把获取到的数据加入treeview的model中以显示
 * @param list xorg日志数据线程list
 */
void DisplayContent::slot_XorgFinished(QList<LOG_MSG_XORG> list)
{
    if (m_flag != XORG)
        return;
    xListOrigin = list;
    xList = list;
    createXorgTable(xList);
    PERF_PRINT_END("POINT-03", "type=xorg");
}

/**
 * @brief DisplayContent::slot_bootFinished 获取启动日志数据线程结果的槽函数,把获取到的数据加入treeview的model中以显示
 * @param list 启动日志数据线程list
 */
void DisplayContent::slot_bootFinished(QList<LOG_MSG_BOOT> list)
{
    if (m_flag != BOOT)
        return;
    bList.clear();
    bList = list;
    currentBootList.clear();
    currentBootList = bList;
    createBootTable(currentBootList);
    PERF_PRINT_END("POINT-03", "type=boot");
}

/**
 * @brief DisplayContent::slot_kernFinished 获取内核日志数据线程结果的槽函数,把获取到的数据加入treeview的model中以显示
 * @param list 启动日志数据线程list
 */
void DisplayContent::slot_kernFinished(QList<LOG_MSG_JOURNAL> list)
{
    if (m_flag != KERN)
        return;
    kListOrigin = list;
    kList = list;
    setLoadState(DATA_COMPLETE);
    createKernTable(kList);
    PERF_PRINT_END("POINT-03", "type=kern");
}

/**
 * @brief DisplayContent::slot_kwinFinished 获取kwin日志数据线程结果的槽函数,把获取到的数据加入treeview的model中以显示
 * @param list kwin日志数据线程list
 */
void DisplayContent::slot_kwinFinished(QList<LOG_MSG_KWIN> list)
{
    if (m_flag != Kwin)
        return;

    m_kwinList = list;
    m_currentKwinList = m_kwinList;
    setLoadState(DATA_COMPLETE);
    creatKwinTable(m_currentKwinList);
    PERF_PRINT_END("POINT-03", "type=kwin");
}

void DisplayContent::slot_journalFinished()
{
//    if (m_flag != JOURNAL) {
//        journalWork::instance()->mutex.unlock();
//        return;
//    }
//    if (journalWork::instance()->logList.isEmpty()) {
//        setLoadState(DATA_COMPLETE);
//        createJournalTableStart(jList);
//        journalWork::instance()->mutex.unlock();
//        return;
//    }
//    jList.append(journalWork::instance()->logList);
//    jListOrigin.append(journalWork::instance()->logList);
//    //    qDebug() << "&&&&&&&&&&&&&&&" << journalWork::instance()->logList.count();
//    journalWork::instance()->logList.clear();
//    journalWork::instance()->mutex.unlock();
//    if (m_firstLoadPageData) {
//        createJournalTableStart(jList);
//        m_firstLoadPageData = false;
//    }
//    // qDebug() << "jList" << jList.count();
}

/**
 * @brief DisplayContent::slot_journalData 系统日志数据获取线程槽函数,系统日志为获取500条就会执行此信号,不是一次把所有数据传进来,所以执行槽函数应该为每次获取向现在的model中添加而不是重置
 * @param index 槽函数发出线程的标记量序号
 * @param list 本次获取的500个或以下的数据
 */
void DisplayContent::slot_journalData(int index, QList<LOG_MSG_JOURNAL> list)
{
    //判断最近一次获取数据线程的标记量,和信号曹发来的sender的标记量作对比,如果相同才可以刷新,因为会出现上次的获取线程就算停下信号也发出来了
    if (m_flag != JOURNAL || index != m_journalCurrentIndex)
        return;
    if (list.isEmpty()) {
        setLoadState(DATA_COMPLETE);
        createJournalTableStart(jList);
        return;
    }
    jListOrigin.append(list);
    jList.append(list);
    //因为此槽会在同一次加载数据完成前触发数次,所以第一次收到数据需要更新界面状态,后面的话往model里塞数据就行
    if (m_firstLoadPageData) {
        createJournalTableStart(jList);
        m_firstLoadPageData = false;
        PERF_PRINT_END("POINT-01", "");
        PERF_PRINT_END("POINT-03", "type=system");
    }
    // qDebug() << "jList" << jList.count();
}

void DisplayContent::slot_journalBootFinished()
{
//    if (m_flag != BOOT_KLU)
//        return;

//    //    jList = logList;
//    //    journalWork::instance()->mutex.lock();
//    if (JournalBootWork::instance()->logList.isEmpty()) {
//        setLoadState(DATA_COMPLETE);
//        createJournalBootTable(jBootList);
//        return;
//    }

//    jBootList.append(JournalBootWork::instance()->logList);
//    //    qDebug() << "&&&&&&&&&&&&&&&" << journalWork::instance()->logList.count();
//    JournalBootWork::instance()->logList.clear();
//    JournalBootWork::instance()->mutex.unlock();
//    setLoadState(DATA_COMPLETE);
    //    createJournalBootTable(jBootList);
}

/**
 * @brief DisplayContent::slot_journalBootData klu下启动日志日志数据获取线程槽函数,系统日志为获取500条就会执行此信号,不是一次把所有数据传进来,所以执行槽函数应该为每次获取向现在的model中添加而不是重置
 * @param index 槽函数发出线程的标记量序号
 * @param list 本次获取的500个或以下的数据
 */
void DisplayContent::slot_journalBootData(int index, QList<LOG_MSG_JOURNAL> list)
{
    if (m_flag != BOOT_KLU || index != m_journalBootCurrentIndex)
        return;
    if (list.isEmpty()) {
        setLoadState(DATA_COMPLETE);
        createJournalBootTableStart(jBootList);
        return;
    }
    jBootListOrigin.append(list);
    jBootList.append(list);
    //因为此槽会在同一次加载数据完成前触发数次,所以第一次收到数据需要更新界面状态,后面的话往model里塞数据就行
    if (m_firstLoadPageData) {
        createJournalBootTableStart(jBootList);
        m_firstLoadPageData = false;
        PERF_PRINT_END("POINT-03", "type=boot_klu");
    }
}

/**
 * @brief DisplayContent::slot_applicationFinished 获取应用日志数据线程结果的槽函数,把获取到的数据加入treeview的model中以显示
 * @param list 应用日志数据线程list
 */
void DisplayContent::slot_applicationFinished(QList<LOG_MSG_APPLICATOIN> list)
{
    if (m_flag != APP)
        return;

    appList.clear();
    setLoadState(DATA_COMPLETE);
    appList = list;
    appListOrigin = list;

    createApplicationTable(appList);
    PERF_PRINT_END("POINT-03", "type=application");
}

/**
 * @brief DisplayContent::slot_NormalFinished 获取开关机日志数据线程结果的槽函数,把获取到的数据加入treeview的model中以显示
 */
void DisplayContent::slot_NormalFinished()
{
    if (m_flag != Normal)
        return;
    setLoadState(DATA_COMPLETE);
    nortempList = norList;
    //    createXorgTable(xList);
    // createNormalTable(nortempList);
    filterNomal(m_normalFilter);
    PERF_PRINT_END("POINT-03", "type=on_off");
}
/**
 * @brief DisplayContent::slot_logLoadFailed 数据获取失败槽函数，显示错误提示框
 * @param iError 错误信息
 */
void DisplayContent::slot_logLoadFailed(const QString &iError)
{
    QString titleIcon = ICONPREFIX ;
    DMessageManager::instance()->sendMessage(this->window(), QIcon(titleIcon + "warning_info.svg"), iError);
}

/**
 * @brief DisplayContent::slot_vScrollValueChanged 滚动条滚动实现分页加载数据槽函数
 * @param valuePixel 当前滑动的像素
 */
void DisplayContent::slot_vScrollValueChanged(int valuePixel)
{
    if (!m_treeView) {
        return;
    }
    if (m_treeView->singleRowHeight() < 0) {
        return;
    }
    //根据当前表格单行行高计算现在滑动了多少项
    int value = valuePixel / m_treeView->singleRowHeight(); // m_treeView->singleRowHeight();
    if (m_treeViewLastScrollValue == value) {
        return;
    }
    m_treeViewLastScrollValue = value;
    if (m_flag == JOURNAL) {
        //算出现在滚动了多少页
        int rate = (value + 25) / SINGLE_LOAD;
        //  qDebug() << "valuePixel:" << valuePixel << "value: " << value << "rate: " << rate << "single: " << SINGLE_LOAD;
        //    qDebug() << m_treeView->verticalScrollBar()->height();
        //如果快滚到页底了就加载下一页数据到表格中
        if (value < SINGLE_LOAD * rate - 20 || value < SINGLE_LOAD * rate) {
            if (m_limitTag >= rate)
                return;

            int leftCnt = jList.count() - SINGLE_LOAD * rate;
            //如果在页尾部则只加载最后一页的数量,否则加载单页全部数量
            int end = leftCnt > SINGLE_LOAD ? SINGLE_LOAD : leftCnt;
            qDebug() << "rate" << rate;
            //把数据加入model中
            insertJournalTable(jList, SINGLE_LOAD * rate, SINGLE_LOAD * rate + end);
            m_limitTag = rate;
            m_treeView->verticalScrollBar()->setValue(valuePixel);
        }

        update();
    } else if (m_flag == BOOT_KLU) {

        int rate = (value + 25) / SINGLE_LOAD;
        //  qDebug() << "valuePixel:" << valuePixel << "value: " << value << "rate: " << rate << "single: " << SINGLE_LOAD;
        //    qDebug() << m_treeView->verticalScrollBar()->height();
        if (value < SINGLE_LOAD * rate - 20 || value < SINGLE_LOAD * rate) {
            if (m_limitTag >= rate)
                return;

            int leftCnt = jBootList.count() - SINGLE_LOAD * rate;
            int end = leftCnt > SINGLE_LOAD ? SINGLE_LOAD : leftCnt;
            //        qDebug() << "total count: " << jList.count() << "left count : " << leftCnt
            //                 << " start : " << SINGLE_LOAD * rate << "end: " << end + SINGLE_LOAD
            //                 * rate;
            qDebug() << "rate" << rate;
            insertJournalBootTable(jBootList, SINGLE_LOAD * rate, SINGLE_LOAD * rate + end);
            m_limitTag = rate;
            m_treeView->verticalScrollBar()->setValue(valuePixel);
        }

        update();
    }  else if (m_flag == APP) {
        int rate = (value + 25) / SINGLE_LOAD;
        //        qDebug() << "value: " << value << "rate: " << rate << "single: " << SINGLE_LOAD;
        qDebug() << "m_limitTag" << m_limitTag << "rate" << rate;
        if (value < SINGLE_LOAD * rate - 20 || value < SINGLE_LOAD * rate) {
            if (m_limitTag >= rate)
                return;

            int leftCnt = appList.count() - SINGLE_LOAD * rate;
            int end = leftCnt > SINGLE_LOAD ? SINGLE_LOAD : leftCnt;
            //            qDebug() << "total count: " << appList.count() << "left count : " <<
            //            leftCnt
            //                     << " start : " << SINGLE_LOAD * rate << "end: " << end +
            //                     SINGLE_LOAD * rate;

            insertApplicationTable(appList, SINGLE_LOAD * rate, SINGLE_LOAD * rate + end);

            m_limitTag = rate;
            m_treeView->verticalScrollBar()->setValue(valuePixel);
        }

    } else if (m_flag == KERN) {  // modified by Airy for bug 12263
        int rate = (value + 25) / SINGLE_LOAD;

        if (value < SINGLE_LOAD * rate - 20 || value < SINGLE_LOAD * rate) {
            if (m_limitTag >= rate)
                return;

            int leftCnt = kList.count() - SINGLE_LOAD * rate;
            int end = leftCnt > SINGLE_LOAD ? SINGLE_LOAD : leftCnt;

            insertKernTable(kList, SINGLE_LOAD * rate, SINGLE_LOAD * rate + end);

            m_limitTag = rate;
            m_treeView->verticalScrollBar()->setValue(valuePixel);
        }

    }

}

/**
 * @brief DisplayContent::slot_searchResult 搜索框执行搜索槽函数
 * @param str 要搜索的关键字
 */
void DisplayContent::slot_searchResult(QString str)
{
    qDebug() << QString("search: %1  treeIndex: %2")
             .arg(str)
             .arg(m_curListIdx.data(ITEM_DATE_ROLE).toString());
    m_currentSearchStr = str;
    if (m_flag == NONE)
        return;

    switch (m_flag) {
    case JOURNAL: {
        jList = jListOrigin;
        int cnt = jList.count();
        for (int i = cnt - 1; i >= 0; --i) {
            //按关键字筛选
            LOG_MSG_JOURNAL msg = jList.at(i);
            if (msg.dateTime.contains(m_currentSearchStr, Qt::CaseInsensitive) ||
                    msg.hostName.contains(m_currentSearchStr, Qt::CaseInsensitive) ||
                    msg.daemonName.contains(m_currentSearchStr, Qt::CaseInsensitive) ||
                    msg.daemonId.contains(m_currentSearchStr, Qt::CaseInsensitive) ||
                    msg.level.contains(m_currentSearchStr, Qt::CaseInsensitive) ||
                    msg.msg.contains(m_currentSearchStr, Qt::CaseInsensitive))
                continue;
            jList.removeAt(i);
        }
        qDebug() << "tmp" << jList.length();
        //清空model和分页重新加载
        createJournalTableForm();
        createJournalTableStart(jList);
    } break;
    case BOOT_KLU: {
        jBootList = jBootListOrigin;
        int cnt = jBootList.count();
        for (int i = cnt - 1; i >= 0; --i) {
            LOG_MSG_JOURNAL msg = jBootList.at(i);
            if (msg.dateTime.contains(m_currentSearchStr, Qt::CaseInsensitive) ||
                    msg.hostName.contains(m_currentSearchStr, Qt::CaseInsensitive) ||
                    msg.daemonName.contains(m_currentSearchStr, Qt::CaseInsensitive) ||
                    msg.daemonId.contains(m_currentSearchStr, Qt::CaseInsensitive) ||
                    msg.level.contains(m_currentSearchStr, Qt::CaseInsensitive) ||
                    msg.msg.contains(m_currentSearchStr, Qt::CaseInsensitive))
                continue;
            jBootList.removeAt(i);
        }
        qDebug() << "tmp" << jBootList.length();
        createJournalBootTableForm();
        createJournalBootTableStart(jBootList);
    } break;
    case KERN: {
        kList = kListOrigin;
        int cnt = kList.count();
        for (int i = cnt - 1; i >= 0; --i) {
            LOG_MSG_JOURNAL msg = kList.at(i);
            if (msg.dateTime.contains(m_currentSearchStr, Qt::CaseInsensitive) ||
                    msg.hostName.contains(m_currentSearchStr, Qt::CaseInsensitive) ||
                    msg.daemonName.contains(m_currentSearchStr, Qt::CaseInsensitive) ||
                    msg.msg.contains(m_currentSearchStr, Qt::CaseInsensitive))
                continue;
            kList.removeAt(i);
        }
        createKernTableForm();
        createKernTable(kList);
    } break;
    case BOOT: {
        m_bootFilter.searchstr = m_currentSearchStr;
        filterBoot(m_bootFilter);

    } break;
    case XORG: {
        xList = xListOrigin ;
        int cnt = xList.count();
        for (int i = cnt - 1; i >= 0; --i) {
            LOG_MSG_XORG msg = xList.at(i);
            if (msg.dateTime.contains(m_currentSearchStr, Qt::CaseInsensitive) ||
                    msg.msg.contains(m_currentSearchStr, Qt::CaseInsensitive))
                continue;
            xList.removeAt(i);
        }
        createXorgTable(xList);
    } break;
    case DPKG: {
        dList = dListOrigin;
        int cnt = dList.count();
        for (int i = cnt - 1; i >= 0; --i) {
            LOG_MSG_DPKG msg = dList.at(i);
            if (msg.dateTime.contains(m_currentSearchStr, Qt::CaseInsensitive) ||
                    msg.msg.contains(m_currentSearchStr, Qt::CaseInsensitive))
                continue;
            dList.removeAt(i);
        }
        createDpkgTable(dList);
    } break;
    case APP: {
        appList = appListOrigin;
        int cnt = appList.count();
        for (int i = cnt - 1; i >= 0; --i) {
            LOG_MSG_APPLICATOIN msg = appList.at(i);
            if (msg.dateTime.contains(m_currentSearchStr, Qt::CaseInsensitive) ||
                    msg.level.contains(m_currentSearchStr, Qt::CaseInsensitive) ||
                    msg.src.contains(m_currentSearchStr, Qt::CaseInsensitive) ||
                    msg.msg.contains(m_currentSearchStr, Qt::CaseInsensitive))
                continue;
            appList.removeAt(i);
        }
        createAppTableForm();
        createAppTable(appList);
    } break;
    case Normal: {
        m_normalFilter.searchstr = m_currentSearchStr;
        filterNomal(m_normalFilter);
    } break;  // add by Airy
    case Kwin: {
        m_currentKwinList = m_kwinList;

        int cnt = m_currentKwinList.count();
        for (int i = cnt - 1; i >= 0; --i) {
            LOG_MSG_KWIN msg = m_currentKwinList.at(i);
            if (msg.msg.contains(m_currentSearchStr, Qt::CaseInsensitive))
                continue;
            m_currentKwinList.removeAt(i);
        }
        creatKwinTable(m_currentKwinList);
    } break;
    default:
        break;
    }
    //如果搜索结果为空要显示无搜索结果提示
    if (0 == m_pModel->rowCount()) {
        if (m_currentSearchStr.isEmpty()) {
            setLoadState(DATA_COMPLETE);
        } else {
            setLoadState(DATA_NO_SEARCH_RESULT);
        }
        m_detailWgt->cleanText();
        m_detailWgt->hideLine(true);
    } else {
        setLoadState(DATA_COMPLETE);
        m_detailWgt->hideLine(false);
    }
}

void DisplayContent::slot_themeChanged(DGuiApplicationHelper::ColorType colorType)
{
    Q_UNUSED(colorType)
    //    if (colorType == DGuiApplicationHelper::DarkType) {
    //        m_iconPrefix = "://images/dark/";
    //    } else if (colorType == DGuiApplicationHelper::LightType) {
    //        m_iconPrefix = "://images/light/";
    //    }
    //    slot_BtnSelected(m_curBtnId, m_curLvId, m_curListIdx);
}

// add by Airy
/**
 * @brief DisplayContent::slot_getLogtype 开关机日志筛选开关机日志类型的选择槽函数,根据选择类型筛选当前获取到的所有开关机日志以显示
 * @param tcbx 开关机日志的类型 0全部, 1登陆 2开机 3关机
 */
void DisplayContent::slot_getLogtype(int tcbx)
{
    m_normalFilter.eventTypeFilter = tcbx;
    filterNomal(m_normalFilter);

}

/**
 * @brief DisplayContent::parseListToModel 把dpkglist加入model中以供treeview显示
 * @param iList 要加入model中的原始数据
 * @param oPModel 要增加数据的model指针
 */
void DisplayContent::parseListToModel(QList<LOG_MSG_DPKG> iList, QStandardItemModel *oPModel)
{
    if (!oPModel) {
        qWarning() << "parse model is  Empty" << __LINE__;
        return;
    }

    oPModel->setColumnCount(3);
    oPModel->setHorizontalHeaderLabels(QStringList()
                                       << DApplication::translate("Table", "Date and Time")
                                       << DApplication::translate("Table", "Info")
                                       << DApplication::translate("Table", "Action"));
    if (iList.isEmpty()) {
        qWarning() << "parse model is  Empty" << __LINE__;
        return;
    }
    QList<LOG_MSG_DPKG> list = iList;
    DStandardItem *item = nullptr;
    QList<QStandardItem *> items;
    //每次插一行，减少刷新次数，重写QStandardItemModel有问题
    for (int i = 0; i < list.size(); i++) {
        items.clear();
        item = new DStandardItem(list[i].dateTime);
        item->setData(DPKG_TABLE_DATA);
        item->setAccessibleText(QString("treeview_context_%1_%2").arg(i).arg(0));
        items << item;
        item = new DStandardItem(list[i].msg);
        item->setAccessibleText(QString("treeview_context_%1_%2").arg(i).arg(1));
        item->setData(DPKG_TABLE_DATA);
        items << item;
        item = new DStandardItem(list[i].action);
        item->setAccessibleText(QString("treeview_context_%1_%2").arg(i).arg(2));
        item->setData(DPKG_TABLE_DATA);
        items << item;
        oPModel->insertRow(oPModel->rowCount(), items);
    }
}

/**
 * @brief DisplayContent::parseListToModel 把启动日志数据list加入model中以供treeview显示
 * @param iList 要加入model中的原始数据
 * @param oPModel 要增加数据的model指针
 */
void DisplayContent::parseListToModel(QList<LOG_MSG_BOOT> iList, QStandardItemModel *oPModel)
{
    if (!oPModel) {
        qWarning() << "parse model is  Empty" << __LINE__;
        return;
    }
    oPModel->setColumnCount(2);
    oPModel->setHorizontalHeaderLabels(QStringList() << DApplication::translate("Table", "Status")
                                       << DApplication::translate("Table", "Info"));
    if (iList.isEmpty()) {
        qWarning() << "parse model is  Empty" << __LINE__;
        return;
    }
    QList<LOG_MSG_BOOT> list = iList;
    DStandardItem *item = nullptr;
    QList<QStandardItem *> items;
    for (int i = 0; i < list.size(); i++) {
        items.clear();
        item = new DStandardItem(list[i].status);
        item->setAccessibleText(QString("treeview_context_%1_%2").arg(i).arg(0));
        item->setData(BOOT_TABLE_DATA);
        items << item;
        item = new DStandardItem(list[i].msg);
        item->setAccessibleText(QString("treeview_context_%1_%2").arg(i).arg(1));
        item->setData(BOOT_TABLE_DATA);
        items << item;
        oPModel->insertRow(oPModel->rowCount(), items);
    }
}

/**
 * @brief DisplayContent::parseListToModel 把应用日志数据list加入model中以供treeview显示
 * @param iList 要加入model中的原始数据
 * @param oPModel 要增加数据的model指针
 */
void DisplayContent::parseListToModel(QList<LOG_MSG_APPLICATOIN> iList, QStandardItemModel *oPModel)
{
    if (!oPModel) {
        qWarning() << "parse model is  Empty" << __LINE__;
        return;
    }

    if (iList.isEmpty()) {
        qWarning() << "parse model is  Empty" << __LINE__;
        return;
    }
    QList<LOG_MSG_APPLICATOIN> list = iList;
    QList<QStandardItem *> items;
    DStandardItem *item = nullptr;
    for (int i = 0; i < list.size(); i++) {
        items.clear();
        //int col = 0;
        QString CH_str = m_transDict.value(list[i].level);
        QString lvStr = CH_str.isEmpty() ? list[i].level : CH_str;
        //        item = new DStandardItem(lvStr);
        item = new DStandardItem();
        QString iconPath = m_iconPrefix + getIconByname(list[i].level);
        if (getIconByname(list[i].level).isEmpty())
            item->setText(lvStr);
        item->setIcon(QIcon(iconPath));
        item->setData(APP_TABLE_DATA);
        item->setData(lvStr, Log_Item_SPACE::levelRole);
        item->setAccessibleText(QString("treeview_context_%1_%2").arg(i).arg(0));
        items << item;
        item = new DStandardItem(list[i].dateTime);
        item->setData(APP_TABLE_DATA);
        item->setAccessibleText(QString("treeview_context_%1_%2").arg(i).arg(1));
        items << item;
        //        item = new DStandardItem(list[i].src);
        item = new DStandardItem(getAppName(m_curAppLog));
        item->setData(APP_TABLE_DATA);
        item->setAccessibleText(QString("treeview_context_%1_%2").arg(i).arg(2));
        items << item;
        item = new DStandardItem(list[i].msg);
        item->setData(APP_TABLE_DATA);
        item->setAccessibleText(QString("treeview_context_%1_%2").arg(i).arg(3));
        items << item;
        oPModel->insertRow(oPModel->rowCount(), items);
    }
}

/**
 * @brief DisplayContent::parseListToModel 把xorg日志数据list加入model中以供treeview显示
 * @param iList 要加入model中的原始数据
 * @param oPModel 要增加数据的model指针
 */
void DisplayContent::parseListToModel(QList<LOG_MSG_XORG> iList, QStandardItemModel *oPModel)
{
    if (!oPModel) {
        qWarning() << "parse model is  Empty" << __LINE__;
        return;
    }
    oPModel->setColumnCount(2);
    oPModel->setHorizontalHeaderLabels(QStringList()
                                       << DApplication::translate("Table", "Date and Time")
                                       << DApplication::translate("Table", "Info"));
    if (iList.isEmpty()) {
        qWarning() << "parse model is  Empty" << __LINE__;
        return;
    }
    QList<LOG_MSG_XORG> list = iList;
    DStandardItem *item = nullptr;
    QList<QStandardItem *> items;
    for (int i = 0; i < list.size(); i++) {
        items.clear();
        item = new DStandardItem(list[i].dateTime);
        item->setData(XORG_TABLE_DATA);
        item->setAccessibleText(QString("treeview_context_%1_%2").arg(i).arg(0));
        items << item;
        item = new DStandardItem(list[i].msg);
        item->setData(XORG_TABLE_DATA);
        item->setAccessibleText(QString("treeview_context_%1_%2").arg(i).arg(1));
        items << item;
        oPModel->insertRow(oPModel->rowCount(), items);
    }
}

/**
 * @brief DisplayContent::parseListToModel 把开关机日志数据list加入model中以供treeview显示
 * @param iList 要加入model中的原始数据
 * @param oPModel 要增加数据的model指针
 */
void DisplayContent::parseListToModel(QList<LOG_MSG_NORMAL> iList, QStandardItemModel *oPModel)
{
    if (!oPModel) {
        qWarning() << "parse model is  Empty" << __LINE__;
        return;
    }
    oPModel->setColumnCount(4);
    oPModel->setHorizontalHeaderLabels(QStringList()
                                       << DApplication::translate("Table", "Event Type")
                                       << DApplication::translate("Table", "Username")
                                       << DApplication::translate("Tbble", "Date and Time")
                                       << DApplication::translate("Table", "Info"));
    if (iList.isEmpty()) {
        qWarning() << "parse model is  Empty" << __LINE__;
        return;
    }
    QList<LOG_MSG_NORMAL> list = iList;
    DStandardItem *item = nullptr;
    QList<QStandardItem *> items;
    for (int i = 0; i < list.size(); i++) {
        items.clear();
        item = new DStandardItem(list[i].eventType);
        item->setData(LAST_TABLE_DATA);
        item->setAccessibleText(QString("treeview_context_%1_%2").arg(i).arg(0));
        items << item;
        item = new DStandardItem(list[i].userName);
        item->setData(LAST_TABLE_DATA);
        item->setAccessibleText(QString("treeview_context_%1_%2").arg(i).arg(1));
        items << item;
        item = new DStandardItem(list[i].dateTime);
        item->setData(LAST_TABLE_DATA);
        item->setAccessibleText(QString("treeview_context_%1_%2").arg(i).arg(2));
        items << item;
        item = new DStandardItem(list[i].msg);
        item->setData(LAST_TABLE_DATA);
        item->setAccessibleText(QString("treeview_context_%1_%2").arg(i).arg(3));
        items << item;
        oPModel->insertRow(oPModel->rowCount(), items);
    }
}

/**
 * @brief DisplayContent::parseListToModel 把kwin日志数据list加入model中以供treeview显示
 * @param iList 要加入model中的原始数据
 * @param oPModel 要增加数据的model指针
 */
void DisplayContent::parseListToModel(QList<LOG_MSG_KWIN> iList, QStandardItemModel *oPModel)
{
    if (!oPModel) {
        qWarning() << "parse model is  Empty" << __LINE__;
        return;
    }
    oPModel->setColumnCount(1);
    oPModel->setHorizontalHeaderLabels(QStringList()
                                       << DApplication::translate("Table", "Info"));
    if (iList.isEmpty()) {
        qWarning() << "parse model is  Empty" << __LINE__;
        return;
    }
    QList<LOG_MSG_KWIN> list = iList;
    DStandardItem *item = nullptr;
    QList<QStandardItem *> items;
    for (int i = 0; i < list.size(); i++) {
        items.clear();
        item = new DStandardItem(list[i].msg);
        item->setData(KWIN_TABLE_DATA);
        item->setAccessibleText(QString("treeview_context_%1_%2").arg(i).arg(0));
        items << item;
        oPModel->insertRow(oPModel->rowCount(), items);
    }
}

/**
 * @brief DisplayContent::setLoadState 设置当前的显示状态
 * @param iState 显示状态
 */
void DisplayContent::setLoadState(DisplayContent::LOAD_STATE iState)
{
    if (!m_spinnerWgt->isHidden()) {
        m_spinnerWgt->spinnerStop();
        m_spinnerWgt->hide();
    }
    if (!m_spinnerWgt_K->isHidden()) {
        m_spinnerWgt_K->spinnerStop();
        m_spinnerWgt_K->hide();
    }
    if (!noResultLabel->isHidden()) {
        noResultLabel->hide();
    }
    if (!m_treeView->isHidden()) {
        m_treeView->hide();
    }
    switch (iState) {
    case DATA_LOADING: {
        //如果为正在加载,则不显示主表\搜索为空的提示lable,只显示加载的转圈动画控件,并且禁止导出,导出按钮置灰
        emit  setExportEnable(false);
        m_spinnerWgt->show();
        m_spinnerWgt->spinnerStart();
        break;
    }
    case DATA_COMPLETE: {
        //如果为加载完成,则只显示主表,导出按钮置可用
        m_treeView->show();
        emit  setExportEnable(true);
        break;
    }
    case DATA_LOADING_K: {
        //如果为内核正在加载,则不显示主表\搜索为空的提示lable,只显示加载的转圈动画控件,并且禁止导出,导出按钮置灰
        emit  setExportEnable(false);
        m_spinnerWgt_K->show();
        m_spinnerWgt_K->spinnerStart();
        break;
    }
    case DATA_NO_SEARCH_RESULT: {
        //如果为无搜索结果状态,则只显示无搜索结果的提示label
        m_treeView->show();
        noResultLabel->resize(m_treeView->viewport()->width(), m_treeView->viewport()->height());
        noResultLabel->show();
        emit  setExportEnable(true);
        break;
    }
    }
    this->update();

}

/**
 * @brief DisplayContent::onExportResult 导出数据结束槽函数,成功则显示提示toast
 * @param isSuccess 导出是否成功
 */
void DisplayContent::onExportResult(bool isSuccess)
{
    QString titleIcon = ICONPREFIX ;
    if (isSuccess) {

        if (m_exportDlg && !m_exportDlg->isHidden()) {
            m_exportDlg->hide();
        }
        DMessageManager::instance()->sendMessage(this->window(), QIcon(titleIcon + "ok.svg"), DApplication::translate("ExportMessage", "Export successful"));
        qDebug() << "sendMessage"  ;
        PERF_PRINT_END("POINT-04", "");
    }
    //  this->setFocus();
    DApplication::setActiveWindow(this);
}
/**
 * @brief DisplayContent::onExportFakeCloseDlg
 * doc和xls格式导出最后save之前无进度变化先关闭窗口,后续再在导出逻辑里加进度信号
 */
void DisplayContent::onExportFakeCloseDlg()
{
    if (m_exportDlg && !m_exportDlg->isHidden()) {
        m_exportDlg->hide();
    }
}

/**
 * @brief DisplayContent::clearAllFilter 清空当前所有的筛选条件成员变量(只限在本类中筛选的条件)
 */
void DisplayContent::clearAllFilter()
{
    m_bootFilter = {"", ""};

    m_currentSearchStr.clear();
    m_currentKwinFilter = {""};
    m_normalFilter.searchstr = "";
//    m_normalFilter.timeFilterEnd = -1;
//    m_normalFilter.timeFilterBegin = -1;
//    m_normalFilter.eventTypeFilter = 0;
}

/**
 * @brief DisplayContent::clearAllDatalist 清空所有获取的数据list
 */
void DisplayContent::clearAllDatalist()
{
    m_detailWgt->cleanText();
    m_pModel->clear();
    jList.clear();
    jListOrigin.clear();
    dList.clear();
    dListOrigin.clear();
    xList.clear();
    xListOrigin.clear();
    bList.clear();
    currentBootList.clear();
    kList.clear();
    kListOrigin.clear();
    appList.clear();
    appListOrigin.clear();
    norList.clear();
    nortempList.clear();
    m_currentKwinList.clear();
    m_kwinList.clear();
    jBootList.clear();
    jBootListOrigin.clear();
    malloc_trim(0);

}

/**
 * @brief DisplayContent::filterBoot 对启动日志进行筛选显示
 * @param ibootFilter  启动日志筛选条件
 */
void DisplayContent::filterBoot(BOOT_FILTERS ibootFilter)
{
    currentBootList.clear();
    bool isStatusFilterEmpty = ibootFilter.statusFilter.isEmpty();
    if (ibootFilter.statusFilter.isEmpty() && ibootFilter.searchstr.isEmpty()) {
        currentBootList = bList;
    } else {
        currentBootList.clear();
        for (int i = 0; i < bList.size(); i++) {
            LOG_MSG_BOOT msg = bList.at(i);
            QString _statusStr = msg.status;
            qDebug() << "xxx" << msg.msg.contains(ibootFilter.searchstr, Qt::CaseInsensitive) << "--" << msg.msg;
            if ((_statusStr.compare(ibootFilter.statusFilter, Qt::CaseInsensitive) != 0) && !isStatusFilterEmpty)
                continue;
            if ((msg.status.contains(ibootFilter.searchstr, Qt::CaseInsensitive)) ||
                    (msg.msg.contains(ibootFilter.searchstr, Qt::CaseInsensitive))) {
                currentBootList.append(bList[i]);
            }
        }
    }
    qDebug() << "bList.count filter" << bList.count();
    createBootTable(currentBootList);
}

/**
 * @brief DisplayContent::filterNomal 对开关机日志进行筛选显示
 * @param inormalFilter 对开关机日志筛选条件
 */
void DisplayContent::filterNomal(NORMAL_FILTERS inormalFilter)
{

    nortempList.clear();
    if (inormalFilter.searchstr.isEmpty() && inormalFilter.eventTypeFilter < 0) {
        nortempList = norList ;
    }
    int tcbx = inormalFilter.eventTypeFilter;
    if (0 == tcbx) {
        for (auto i = 0; i < norList.size(); i++) {
            LOG_MSG_NORMAL msg = norList.at(i);
            if (msg.eventType.contains(inormalFilter.searchstr, Qt::CaseInsensitive) ||
                    msg.userName.contains(inormalFilter.searchstr, Qt::CaseInsensitive) ||
                    msg.dateTime.contains(inormalFilter.searchstr, Qt::CaseInsensitive) ||
                    msg.msg.contains(inormalFilter.searchstr, Qt::CaseInsensitive)) {
                nortempList.append(msg);
            }
        }
    } else if (1 == tcbx) {
        for (auto i = 0; i < norList.size(); i++) {
            LOG_MSG_NORMAL msg = norList.at(i);
            if (msg.eventType.contains(inormalFilter.searchstr, Qt::CaseInsensitive) ||
                    msg.userName.contains(inormalFilter.searchstr, Qt::CaseInsensitive) ||
                    msg.dateTime.contains(inormalFilter.searchstr, Qt::CaseInsensitive) ||
                    msg.msg.contains(inormalFilter.searchstr, Qt::CaseInsensitive)) {
                if (msg.eventType.compare("Boot", Qt::CaseInsensitive) != 0 &&
                        msg.eventType.compare("shutdown", Qt::CaseInsensitive) != 0 &&
                        msg.eventType.compare("runlevel", Qt::CaseInsensitive) != 0)
                    nortempList.append(msg);
            }
        }
    } else if (2 == tcbx) {
        for (auto i = 0; i < norList.size(); i++) {
            LOG_MSG_NORMAL msg = norList.at(i);
            if (msg.eventType.contains(inormalFilter.searchstr, Qt::CaseInsensitive) ||
                    msg.userName.contains(inormalFilter.searchstr, Qt::CaseInsensitive) ||
                    msg.dateTime.contains(inormalFilter.searchstr, Qt::CaseInsensitive) ||
                    msg.msg.contains(inormalFilter.searchstr, Qt::CaseInsensitive)) {
                if (norList[i].eventType.compare("Boot", Qt::CaseInsensitive) == 0)
                    nortempList.append(norList[i]);
            }
        }
    } else if (3 == tcbx) {
        for (auto i = 0; i < norList.size(); i++) {
            LOG_MSG_NORMAL msg = norList.at(i);
            if (msg.eventType.contains(inormalFilter.searchstr, Qt::CaseInsensitive) ||
                    msg.userName.contains(inormalFilter.searchstr, Qt::CaseInsensitive) ||
                    msg.dateTime.contains(inormalFilter.searchstr, Qt::CaseInsensitive) ||
                    msg.msg.contains(inormalFilter.searchstr, Qt::CaseInsensitive)) {
                if (norList[i].eventType.compare("shutdown", Qt::CaseInsensitive) == 0)
                    nortempList.append(norList[i]);
            }
        }
    }
    createNormalTable(nortempList);
}

/**
 * @brief DisplayContent::onExportProgress 导出时进度显示槽函数,连接导出数据进程
 * @param nCur 当前进行到的count
 * @param nTotal 总数量
 */
void DisplayContent::onExportProgress(int nCur, int nTotal)
{
    LogExportThread *exportThread = nullptr;
    if (sender()) {
        exportThread = qobject_cast<LogExportThread *>(sender());
    }
    //如果导出线程不再运行则不处理此信号
    if (!m_exportDlg || !exportThread || !exportThread->isProcessing()) {
        return;
    }
    //弹窗
    if (m_exportDlg->isHidden()) {
        m_exportDlg->show();
    }
    m_exportDlg->setProgressBarRange(0, nTotal);
    m_exportDlg->updateProgressBarValue(nCur);
}

/**
 * @brief DisplayContent::parseListToModel 把系统日志数据list加入model中以供treeview显示
 * @param iList 要加入model中的原始数据
 * @param oPModel 要增加数据的model指针
 */
void DisplayContent::parseListToModel(QList<LOG_MSG_JOURNAL> iList, QStandardItemModel *oPModel)
{
    if (!oPModel) {
        qWarning() << "parse model is  Empty" << __LINE__;
        return;
    }

    if (iList.isEmpty()) {
        qWarning() << "parse model is  Empty" << __LINE__;
        return;
    }
    QList<LOG_MSG_JOURNAL> list = iList;
    DStandardItem *item = nullptr;
    QList<QStandardItem *> items;
    for (int i = 0; i < list.size(); i++) {
        items.clear();
        item = new DStandardItem(list[i].dateTime);
        item->setData(KERN_TABLE_DATA);
        items << item;
        item = new DStandardItem(list[i].hostName);
        item->setData(KERN_TABLE_DATA);
        items << item;
        item = new DStandardItem(list[i].daemonName);
        item->setData(KERN_TABLE_DATA);
        items << item;
        item = new DStandardItem(list[i].msg);
        item->setData(KERN_TABLE_DATA);
        items << item;
        oPModel->insertRow(oPModel->rowCount(), items);
    }
}

void DisplayContent::paintEvent(QPaintEvent *event)
{
    DWidget::paintEvent(event);
    return;

//    QPainter painter(this);
//    painter.setRenderHint(QPainter::Antialiasing, true);

//    // Save pen
//    QPen oldPen = painter.pen();

//    painter.setRenderHint(QPainter::Antialiasing);
//    DPalette pa = DApplicationHelper::instance()->palette(this);
//    painter.setBrush(QBrush(pa.color(DPalette::Base)));
//    QColor penColor = pa.color(DPalette::FrameBorder);
//    penColor.setAlphaF(0.05);
//    painter.setPen(QPen(penColor));

//    QRectF rect = this->rect();
//    rect.setX(0.5);
//    rect.setY(0.5);
//    rect.setWidth(rect.width() - 0.5);
//    rect.setHeight(rect.height() - 0.5);

//    QPainterPath painterPath;
//    painterPath.addRoundedRect(rect, 8, 8);
//    painter.drawPath(painterPath);

//    // Restore the pen
//    painter.setPen(oldPen);

}

/**
 * @brief DisplayContent::resizeEvent 重载resizeEvent以让无搜索结果提示的lable大小和treeview使文字居中
 * @param event
 */
void DisplayContent::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event)
    noResultLabel->resize(m_treeView->viewport()->width(), m_treeView->viewport()->height());
}

/**
 * @brief DisplayContent::getIconByname 获取日志等级信息对应的图标文件名
 * @param str 日志等级字符串
 * @return 日志等级图标文件名
 */
QString DisplayContent::getIconByname(QString str)
{
    //    qDebug() << str << m_icon_name_map.value(str);
    return m_icon_name_map.value(str);
}

/**
 * @brief DisplayContent::createApplicationTable 获取应用日志完成时加载所有数据到treeview中
 * @param list 获得的应用日志数据list
 */
void DisplayContent::createApplicationTable(QList<LOG_MSG_APPLICATOIN> &list)
{
    //    m_treeView->show();
    m_limitTag = 0;
    setLoadState(DATA_COMPLETE);
    //m_pModel->clear();


    int end = list.count() > SINGLE_LOAD ? SINGLE_LOAD : list.count();
    insertApplicationTable(list, 0, end);
}

/**
 * @brief DisplayContent::insertApplicationTable 按分页转换插入应用日志到treeview的model中
 * @param logList 当前筛选状态下所有符合条件的应用日志数据结构
 * @param start 分页开始的数组下标
 * @param end 分页结束的数组下标
 */
void DisplayContent::insertApplicationTable(QList<LOG_MSG_APPLICATOIN> list, int start, int end)
{
    QList<LOG_MSG_APPLICATOIN> midList = list;
    if (end >= start) {
        midList = midList.mid(start, end - start);
    }
    parseListToModel(midList, m_pModel);
    QItemSelectionModel *p = m_treeView->selectionModel();

    if (p)
        p->select(m_pModel->index(0, 0), QItemSelectionModel::Rows | QItemSelectionModel::Select);
    slot_tableItemClicked(m_pModel->index(0, 0));
}

/**
 * @author Airy
 * @brief DisplayContent::slot_refreshClicked for refresh日志类型listview右键刷新数据槽函数
 * @param index 当前选中的日志类型的index
 */
void DisplayContent::slot_refreshClicked(const QModelIndex &index)
{
    if (!index.isValid()) {
        return;
    }

    m_curListIdx = index;

    clearAllDatalist();
    QString itemData = index.data(ITEM_DATE_ROLE).toString();
    if (itemData.isEmpty())
        return;

    if (itemData.contains(JOUR_TREE_DATA, Qt::CaseInsensitive)) {
        // default level is info so PRIORITY=6
        m_flag = JOURNAL;
        generateJournalFile(m_curBtnId, m_curLevel);
    } else if (itemData.contains(DPKG_TREE_DATA, Qt::CaseInsensitive)) {
        m_flag = DPKG;
        generateDpkgFile(m_curBtnId);
    } else if (itemData.contains(XORG_TREE_DATA, Qt::CaseInsensitive)) {
        xList.clear();
        m_flag = XORG;
        //        m_logFileParse.parseByXlog(xList);
        generateXorgFile(m_curBtnId);
    } else if (itemData.contains(BOOT_TREE_DATA, Qt::CaseInsensitive)) {
        bList.clear();
        m_flag = BOOT;
        m_logFileParse.parseByBoot();
    } else if (itemData.contains(KERN_TREE_DATA, Qt::CaseInsensitive)) {
        m_flag = KERN;
        generateKernFile(m_curBtnId);
    } else if (itemData.contains(".cache")) {
    } else if (itemData.contains(APP_TREE_DATA, Qt::CaseInsensitive)) {
//        m_pModel->clear();  // clicked parent node application, clear table contents
        //应用日志不使用直接刷新,而是刷新筛选器中的选项造成刷新
//        m_flag = APP;
//        generateAppFile(m_curAppLog, m_curBtnId, m_curLevel);
    } else if (itemData.contains(LAST_TREE_DATA, Qt::CaseInsensitive)) {
        norList.clear();
        m_flag = Normal;
        //        m_logFileParse.parseByNormal(norList);
        generateNormalFile(m_curBtnId);
    } else if (itemData.contains(KWIN_TREE_DATA, Qt::CaseInsensitive)) {
        m_kwinList.clear();
        m_currentKwinList.clear();
        m_flag = Kwin;
        m_logFileParse.parseByKwin(m_currentKwinFilter);
    } else if (itemData.contains(BOOT_KLU_TREE_DATA, Qt::CaseInsensitive)) {
        m_flag = BOOT_KLU;
        generateJournalBootFile(m_curLevel);
    }


    if (!itemData.contains(JOUR_TREE_DATA, Qt::CaseInsensitive) ||
            !itemData.contains(KERN_TREE_DATA, Qt::CaseInsensitive)) {  // modified by Airy
//        m_spinnerWgt_K->spinnerStop();
//        m_treeView->show();
//        m_spinnerWgt_K->hide();
    }
}
