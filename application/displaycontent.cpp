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
#include <QDateTime>
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

#include <sys/utsname.h>

DWIDGET_USE_NAMESPACE

#define SINGLE_LOAD 200

#define LEVEL_WIDTH 80
#define STATUS_WIDTH 90
#define DATETIME_WIDTH 175
#define DEAMON_WIDTH 100

DisplayContent::DisplayContent(QWidget *parent)
    : DWidget(parent)

{
    initUI();
    initMap();
    initConnections();

}

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
}



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
    //    QFont labelFont = noResultLabel->font();
    //    labelFont.setPixelSize(20);
    //    noResultLabel->setFont(labelFont);
    noResultLabel->setAlignment(Qt::AlignCenter);
    //    vLayout->addWidget(noResultLabel, 5);
    m_spinnerWgt = new LogSpinnerWidget(this);
    m_spinnerWgt_K = new LogSpinnerWidget(this);

    vLayout->addWidget(m_spinnerWgt_K, 5);
    vLayout->addWidget(m_spinnerWgt, 5);

    //    m_noResultWdg = new LogSearchNoResultWidget(this);
    //    m_noResultWdg->setContent("");
    //    vLayout->addWidget(m_noResultWdg, 10);
    //    m_noResultWdg->hide();

    m_detailWgt = new logDetailInfoWidget(this);
    vLayout->addWidget(m_detailWgt, 3);

    vLayout->setContentsMargins(0, 0, 0, 0);
    vLayout->setSpacing(3);

    this->setLayout(vLayout);
    setLoadState(DATA_COMPLETE);
    //    DGuiApplicationHelper::ColorType ct = DApplicationHelper::instance()->themeType();
    //    slot_themeChanged(ct);
    m_exportDlg = new ExportProgressDlg(this);
    m_exportDlg->hide();
}

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

void DisplayContent::initTableView()
{
    m_treeView = new LogTreeView(this);
    m_pModel = new QStandardItemModel(this);
    m_treeView->setModel(m_pModel);
}

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
    connect(&m_logFileParse, &LogFileParser::applicationFinished, this,
            &DisplayContent::slot_applicationFinished);
    connect(&m_logFileParse, &LogFileParser::kwinFinished, this,
            &DisplayContent::slot_kwinFinished);
    connect(&m_logFileParse, SIGNAL(normalFinished()), this,
            SLOT(slot_NormalFinished()));  // add by Airy
    connect(m_treeView->verticalScrollBar(), &QScrollBar::valueChanged, this,
            &DisplayContent::slot_vScrollValueChanged);

    connect(DApplicationHelper::instance(), &DApplicationHelper::themeTypeChanged, this,
            &DisplayContent::slot_themeChanged);
}

void DisplayContent::generateJournalFile(int id, int lId, const QString &iSearchStr)
{
    Q_UNUSED(iSearchStr)
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
        m_logFileParse.parseByJournal(arg);
    } break;
    case ONE_DAY: {
        //        arg << "--since" << dt.toString("yyyy-MM-dd");
        arg << QString::number(dt.toMSecsSinceEpoch() * 1000);
        m_logFileParse.parseByJournal(arg);
    } break;
    case THREE_DAYS: {
        //        QString t = dt.addDays(-2).toString("yyyy-MM-dd");
        //        arg << "--since" << t;
        arg << QString::number(dt.addDays(-2).toMSecsSinceEpoch() * 1000);
        m_logFileParse.parseByJournal(arg);
    } break;
    case ONE_WEEK: {
        //        QString t = dt.addDays(-6).toString("yyyy-MM-dd");
        //        arg << "--since" << t;

        arg << QString::number(dt.addDays(-6).toMSecsSinceEpoch() * 1000);
        m_logFileParse.parseByJournal(arg);
    } break;
    case ONE_MONTH: {
        //        QString t = dt.addDays(-29).toString("yyyy-MM-dd");
        //        arg << "--since" << t;
        arg << QString::number(dt.addDays(-29).toMSecsSinceEpoch() * 1000);
        m_logFileParse.parseByJournal(arg);
    } break;
    case THREE_MONTHS: {
        //        QString t = dt.addDays(-89).toString("yyyy-MM-dd");
        //        arg << "--since" << t;
        arg << QString::number(dt.addDays(-89).toMSecsSinceEpoch() * 1000);
        m_logFileParse.parseByJournal(arg);
    } break;
    default:
        break;
    }
}

void DisplayContent::createJournalTableStart(QList<LOG_MSG_JOURNAL> &list)
{
    m_limitTag = 0;
    // m_pModel->clear();
    setLoadState(DATA_COMPLETE);
    int end = list.count() > SINGLE_LOAD ? SINGLE_LOAD : list.count();
    insertJournalTable(list, 0, end);
}

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

void DisplayContent::generateDpkgFile(int id, const QString &iSearchStr)
{
    Q_UNUSED(iSearchStr)
    dList.clear();

    setLoadState(DATA_LOADING);

    QDateTime dt = QDateTime::currentDateTime();
    dt.setTime(QTime());  // get zero time
    switch (id) {
    case ALL:
        m_logFileParse.parseByDpkg();
        break;
    case ONE_DAY: {
        m_logFileParse.parseByDpkg(dt.toMSecsSinceEpoch());
    } break;
    case THREE_DAYS: {
        m_logFileParse.parseByDpkg(dt.addDays(-2).toMSecsSinceEpoch());
    } break;
    case ONE_WEEK: {
        m_logFileParse.parseByDpkg(dt.addDays(-6).toMSecsSinceEpoch());
    } break;
    case ONE_MONTH: {
        m_logFileParse.parseByDpkg(dt.addDays(-29).toMSecsSinceEpoch());
    } break;
    case THREE_MONTHS: {
        m_logFileParse.parseByDpkg(dt.addDays(-89).toMSecsSinceEpoch());
    } break;
    default:
        break;
    }
}

void DisplayContent::createDpkgTable(QList<LOG_MSG_DPKG> &list)
{
    //    m_treeView->show();
    m_limitTag = 0;
    setLoadState(DATA_COMPLETE);
    m_pModel->clear();
    parseListToModel(list, m_pModel);
    m_treeView->setColumnWidth(0, DATETIME_WIDTH);
    m_treeView->hideColumn(2);

    //    m_treeView->setModel(m_pModel);

    // default first row select
    //    m_treeView->selectRow(0);
    QItemSelectionModel *p = m_treeView->selectionModel();
    if (p)
        p->select(m_pModel->index(0, 0), QItemSelectionModel::Rows | QItemSelectionModel::Select);
    slot_tableItemClicked(m_pModel->index(0, 0));
}

void DisplayContent::generateKernFile(int id, const QString &iSearchStr)
{
    Q_UNUSED(iSearchStr)
    kList.clear();
    kListOrigin.clear();
    setLoadState(DATA_LOADING);
    //    m_spinnerWgt->hide();  // modified by Airy for bug 15520
    //    m_treeView->show();

    QDateTime dt = QDateTime::currentDateTime();
    dt.setTime(QTime());  // get zero time
    switch (id) {
    case ALL:
        m_logFileParse.parseByKern(0);
        break;
    case ONE_DAY: {
        m_logFileParse.parseByKern(dt.toMSecsSinceEpoch());
    } break;
    case THREE_DAYS: {
        m_logFileParse.parseByKern(dt.addDays(-2).toMSecsSinceEpoch());
    } break;
    case ONE_WEEK: {
        m_logFileParse.parseByKern(dt.addDays(-6).toMSecsSinceEpoch());
    } break;
    case ONE_MONTH: {
        m_logFileParse.parseByKern(dt.addDays(-29).toMSecsSinceEpoch());
    } break;
    case THREE_MONTHS: {
        m_logFileParse.parseByKern(dt.addDays(-89).toMSecsSinceEpoch());
    } break;
    default:
        break;
    }
}

// modified by Airy for bug  12263
void DisplayContent::createKernTable(QList<LOG_MSG_JOURNAL> &list)
{
    //    m_treeView->show();
    setLoadState(DATA_COMPLETE);
    m_pModel->clear();

    m_limitTag = 0;
    int end = list.count() > SINGLE_LOAD ? SINGLE_LOAD : list.count();
    insertKernTable(list, 0, end);

    //    DStandardItem *item = nullptr;
    //    for (int i = 0; i < list.size(); i++) {
    //        item = new DStandardItem(list[i].dateTime);
    //        item->setData(KERN_TABLE_DATA);
    //        m_pModel->setItem(i, 0, item);
    //        item = new DStandardItem(list[i].hostName);
    //        item->setData(KERN_TABLE_DATA);
    //        m_pModel->setItem(i, 1, item);
    //        item = new DStandardItem(list[i].daemonName);
    //        item->setData(KERN_TABLE_DATA);
    //        m_pModel->setItem(i, 2, item);
    //        item = new DStandardItem(list[i].msg);
    //        item->setData(KERN_TABLE_DATA);
    //        m_pModel->setItem(i, 3, item);
    //    }

    //    //    m_treeView->setModel(m_pModel);

    //    // default first row select
    //    //    m_treeView->selectRow(0);
    //    QItemSelectionModel *p = m_treeView->selectionModel();
    //    if (p)
    //        p->select(m_pModel->index(0, 0), QItemSelectionModel::Rows |
    //        QItemSelectionModel::Select);
    //    slot_tableItemClicked(m_pModel->index(0, 0));
}

// add by Airy for bug
void DisplayContent::insertKernTable(QList<LOG_MSG_JOURNAL> list, int start, int end)
{
    QList<LOG_MSG_JOURNAL> midList = list;
    if (end >= start) {
        midList = midList.mid(start, end - start);
    }
    parseListToModel(midList, m_pModel);
    //    m_treeView->setModel(m_pModel);

    // default first row select
    //    m_treeView->selectRow(0);
    //排序插入后可能列宽会改变，故把调整列宽放到insert中 by zyc
    m_treeView->setColumnWidth(0, DATETIME_WIDTH - 30);
    m_treeView->setColumnWidth(1, DEAMON_WIDTH);
    m_treeView->setColumnWidth(2, DEAMON_WIDTH);
    QItemSelectionModel *p = m_treeView->selectionModel();
    if (p)
        p->select(m_pModel->index(0, 0), QItemSelectionModel::Rows | QItemSelectionModel::Select);
    slot_tableItemClicked(m_pModel->index(0, 0));
}

void DisplayContent::generateAppFile(QString path, int id, int lId, const QString &iSearchStr)
{
    Q_UNUSED(iSearchStr)
    appList.clear();
    setLoadState(DATA_LOADING);
    QDateTime dt = QDateTime::currentDateTime();
    dt.setTime(QTime());  // get zero time
    switch (id) {
    case ALL:
        m_logFileParse.parseByApp(path, lId);
        break;
    case ONE_DAY: {
        m_logFileParse.parseByApp(path, lId, dt.toMSecsSinceEpoch());
    } break;
    case THREE_DAYS: {
        m_logFileParse.parseByApp(path, lId, dt.addDays(-2).toMSecsSinceEpoch());
    } break;
    case ONE_WEEK: {
        m_logFileParse.parseByApp(path, lId, dt.addDays(-6).toMSecsSinceEpoch());
    } break;
    case ONE_MONTH: {
        m_logFileParse.parseByApp(path, lId, dt.addDays(-29).toMSecsSinceEpoch());
    } break;
    case THREE_MONTHS: {
        m_logFileParse.parseByApp(path, lId, dt.addDays(-89).toMSecsSinceEpoch());
    } break;
    default:
        break;
    }
}

void DisplayContent::createAppTable(QList<LOG_MSG_APPLICATOIN> &list)
{
    //    m_treeView->show();
    m_limitTag = 0;
    setLoadState(DATA_COMPLETE);
    m_pModel->clear();
    int end = list.count() > SINGLE_LOAD ? SINGLE_LOAD : list.count();
    insertApplicationTable(list, 0, end);
}

void DisplayContent::createBootTable(QList<LOG_MSG_BOOT> &list)
{
    //    m_treeView->show();
    m_limitTag = 0;
    setLoadState(DATA_COMPLETE);
    m_pModel->clear();
    m_treeView->setColumnWidth(0, STATUS_WIDTH);
    parseListToModel(list, m_pModel);
    //    m_treeView->setModel(m_pModel);
    // default first row select
    //    m_treeView->selectRow(0);
    QItemSelectionModel *p = m_treeView->selectionModel();
    if (p)
        p->select(m_pModel->index(0, 0), QItemSelectionModel::Rows | QItemSelectionModel::Select);
    slot_tableItemClicked(m_pModel->index(0, 0));
}

void DisplayContent::createXorgTable(QList<LOG_MSG_XORG> &list)
{
    //    m_treeView->show();
    m_limitTag = 0;
    setLoadState(DATA_COMPLETE);
    m_pModel->clear();
    parseListToModel(list, m_pModel);
    m_treeView->setColumnWidth(0, DATETIME_WIDTH + 20);


    //    m_treeView->setModel(m_pModel);

    // default first row select
    //    m_treeView->selectRow(0);
    QItemSelectionModel *p = m_treeView->selectionModel();
    if (p)
        p->select(m_pModel->index(0, 0), QItemSelectionModel::Rows | QItemSelectionModel::Select);
    slot_tableItemClicked(m_pModel->index(0, 0));
}

void DisplayContent::generateXorgFile(int id)
{
    xList.clear();
    setLoadState(DATA_LOADING);
    QDateTime dt = QDateTime::currentDateTime();
    dt.setTime(QTime());  // get zero time
    switch (id) {
    case ALL:
        m_logFileParse.parseByXlog();
        break;
    case ONE_DAY: {
        m_logFileParse.parseByXlog(dt.toMSecsSinceEpoch());
    } break;
    case THREE_DAYS: {
        m_logFileParse.parseByXlog(dt.addDays(-2).toMSecsSinceEpoch());
    } break;
    case ONE_WEEK: {
        m_logFileParse.parseByXlog(dt.addDays(-6).toMSecsSinceEpoch());
    } break;
    case ONE_MONTH: {
        m_logFileParse.parseByXlog(dt.addDays(-29).toMSecsSinceEpoch());
    } break;
    case THREE_MONTHS: {
        m_logFileParse.parseByXlog(dt.addDays(-89).toMSecsSinceEpoch());
    } break;
    default:
        break;
    }
}

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

void DisplayContent::generateKwinFile(KWIN_FILTERS iFilters)
{
    m_kwinList.clear();
    m_currentKwinList.clear();
    setLoadState(DATA_LOADING);
    m_logFileParse.parseByKwin(iFilters);
}

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
    //    m_treeView->setModel(m_pModel);

    // default first row select
    //    m_treeView->selectRow(0);
    QItemSelectionModel *p = m_treeView->selectionModel();
    if (p)
        p->select(m_pModel->index(0, 0), QItemSelectionModel::Rows | QItemSelectionModel::Select);
    slot_tableItemClicked(m_pModel->index(0, 0));
}

// add by Airy
void DisplayContent::generateNormalFile(int id)
{
    norList.clear();
    nortempList.clear();
    setLoadState(DATA_LOADING);

    QDateTime dt = QDateTime::currentDateTime();
    dt.setTime(QTime());  // get zero time
    switch (id) {
    case ALL:
        m_logFileParse.parseByNormal(norList);
        break;
    case ONE_DAY: {
        m_logFileParse.parseByNormal(norList, dt.toMSecsSinceEpoch());
    } break;
    case THREE_DAYS: {
        m_logFileParse.parseByNormal(norList, dt.addDays(-2).toMSecsSinceEpoch());
    } break;
    case ONE_WEEK: {
        m_logFileParse.parseByNormal(norList, dt.addDays(-6).toMSecsSinceEpoch());
    } break;
    case ONE_MONTH: {
        m_logFileParse.parseByNormal(norList, dt.addDays(-29).toMSecsSinceEpoch());
    } break;
    case THREE_MONTHS: {
        m_logFileParse.parseByNormal(norList, dt.addDays(-89).toMSecsSinceEpoch());
    } break;
    default:
        break;
    }
    nortempList = norList;
}

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
        items << item;
        item = new DStandardItem(logList[i].daemonName);
        item->setData(JOUR_TABLE_DATA);
        // m_pModel->setItem(i, JOURNAL_SPACE::journalDaemonNameColumn, item);
        items << item;
        item = new DStandardItem(logList[i].dateTime);
        item->setData(JOUR_TABLE_DATA);
        //m_pModel->setItem(i, JOURNAL_SPACE::journalDateTimeColumn, item);
        items << item;
        item = new DStandardItem(logList[i].msg);
        item->setData(JOUR_TABLE_DATA);
        //m_pModel->setItem(i, JOURNAL_SPACE::journalMsgColumn, item);
        items << item;
        item = new DStandardItem(logList[i].hostName);
        item->setData(JOUR_TABLE_DATA);
        // m_pModel->setItem(i, JOURNAL_SPACE::journalHostNameColumn, item);
        items << item;
        item = new DStandardItem(logList[i].daemonId);
        item->setData(JOUR_TABLE_DATA);
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

    // default first row select
    m_treeView->setColumnWidth(JOURNAL_SPACE::journalLevelColumn, LEVEL_WIDTH);
    m_treeView->setColumnWidth(JOURNAL_SPACE::journalDaemonNameColumn, DEAMON_WIDTH);
    m_treeView->setColumnWidth(JOURNAL_SPACE::journalDateTimeColumn, DATETIME_WIDTH);
    QItemSelectionModel *p = m_treeView->selectionModel();
    if (p)
        p->select(m_pModel->index(0, 0), QItemSelectionModel::Rows | QItemSelectionModel::Select);
    slot_tableItemClicked(m_pModel->index(0, 0));
}

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

bool DisplayContent::isAuthProcessAlive()
{
    bool ret = false;
    QProcess proc;
    int rslt = proc.execute("ps -aux | grep 'logViewerAuth'");
    qDebug() << rslt << "*************";
    return !(ret = (rslt == 0));
}

void DisplayContent::slot_tableItemClicked(const QModelIndex &index)
{
    emit sigDetailInfo(index, m_pModel, getAppName(m_curAppLog));
}

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
    } else if (treeData.contains(DPKG_TREE_DATA, Qt::CaseInsensitive)) {
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

void DisplayContent::slot_appLogs(QString path)
{
//    if (path.isEmpty())
//        return;        //delete by Airy for bug 20457 :if path is empty,item is not empty
    appList.clear();
    m_curAppLog = path;
    //    m_logFileParse.parseByApp(path, appList);
    generateAppFile(path, m_curBtnId, m_curLevel);
}

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

    void clearAllFilter();
    void clearAllDatalist();

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
    }

//    if (!itemData.contains(JOUR_TREE_DATA, Qt::CaseInsensitive) ||   //modified by Airy for bug 19660:spinner always running
//            !itemData.contains(KERN_TREE_DATA, Qt::CaseInsensitive)) {  // modified by Airy
//        setLoadState(DATA_COMPLETE);
//    }
}

void DisplayContent::slot_exportClicked()
{
    QString logName;
    if (m_curListIdx.isValid())
        logName = QString("/%1").arg(m_curListIdx.data().toString());
    else {
        logName = QString("/%1").arg(("New File"));
    }
    QString selectFilter;
    QString path = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation) + "/logtemp/" + logName + ".txt";
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
    if (selectFilter == "TEXT (*.txt)") {
        LogExportThread *exportThread = new LogExportThread(this);
        connect(m_exportDlg, &ExportProgressDlg::sigCloseBtnClicked, exportThread, &LogExportThread::stopImmediately);
        connect(exportThread, &LogExportThread::sigResult, this, &DisplayContent::onExportResult);
        connect(exportThread, &LogExportThread::sigProgress, this, &DisplayContent::onExportProgress);
        switch (m_flag) {
        case JOURNAL:
            exportThread->exportToTxtPublic(fileName, jList, labels, m_flag);
            break;
        case APP: {
            QString appName = getAppName(m_curAppLog);
            exportThread->exportToTxtPublic(fileName, appList, labels, appName);
            break;
        }
        case DPKG:
            exportThread->exportToTxtPublic(fileName, dList, labels);
            break;
        case BOOT:
            exportThread->exportToTxtPublic(fileName, currentBootList, labels);
            break;
        case XORG:
            exportThread->exportToTxtPublic(fileName, xList, labels);
            break;
        case Normal:
            exportThread->exportToTxtPublic(fileName, nortempList, labels);
            break;
        case KERN:
            exportThread->exportToTxtPublic(fileName, jList, labels, m_flag);
            break;
        case Kwin:
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
            exportThread->exportToHtmlPublic(fileName, jList, labels, m_flag);
            break;
        case APP: {
            QString appName = getAppName(m_curAppLog);
            exportThread->exportToHtmlPublic(fileName, appList, labels, appName);
            break;
        }
        case DPKG:
            exportThread->exportToHtmlPublic(fileName, dList, labels);
            break;
        case BOOT:
            exportThread->exportToHtmlPublic(fileName, currentBootList, labels);
            break;
        case XORG:
            exportThread->exportToHtmlPublic(fileName, xList, labels);
            break;
        case Normal:
            exportThread->exportToHtmlPublic(fileName, nortempList, labels);
            break;
        case KERN:
            exportThread->exportToHtmlPublic(fileName, jList, labels, m_flag);
            break;
        case Kwin:
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
        switch (m_flag) {
        case JOURNAL:
            exportThread->exportToDocPublic(fileName, jList, labels, m_flag);
            break;
        case APP: {
            QString appName = getAppName(m_curAppLog);
            exportThread->exportToDocPublic(fileName, appList, labels, appName);
            break;
        }
        case DPKG:
            exportThread->exportToDocPublic(fileName, dList, labels);
            break;
        case BOOT:
            exportThread->exportToDocPublic(fileName, currentBootList, labels);
            break;
        case XORG:
            exportThread->exportToDocPublic(fileName, xList, labels);
            break;
        case Normal:
            exportThread->exportToDocPublic(fileName, nortempList, labels);
            break;
        case KERN:
            exportThread->exportToDocPublic(fileName, jList, labels, m_flag);
            break;
        case Kwin:
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
        switch (m_flag) {
        case JOURNAL:
            exportThread->exportToXlsPublic(fileName, jList, labels, m_flag);
            break;
        case APP: {
            QString appName = getAppName(m_curAppLog);
            exportThread->exportToXlsPublic(fileName, appList, labels, appName);
            break;
        }
        case DPKG:
            exportThread->exportToXlsPublic(fileName, dList, labels);
            break;
        case BOOT:
            exportThread->exportToXlsPublic(fileName, currentBootList, labels);
            break;
        case XORG:
            exportThread->exportToXlsPublic(fileName, xList, labels);
            break;
        case Normal:
            exportThread->exportToXlsPublic(fileName, nortempList, labels);
            break;
        case KERN:
            exportThread->exportToXlsPublic(fileName, jList, labels, m_flag);
            break;
        case Kwin:
            exportThread->exportToXlsPublic(fileName, m_currentKwinList, labels);
            break;
        default:
            break;
        }
        QThreadPool::globalInstance()->start(exportThread);
    }
//    if (exportTempModel) {
//        exportTempModel->deleteLater();
//        exportTempModel = nullptr;
//    }
}

void DisplayContent::slot_statusChagned(QString status)
{
    m_bootFilter.statusFilter = status;
    currentBootList.clear();

    if (status.isEmpty()) {
        currentBootList = bList;
    } else {
        for (int i = 0; i < bList.size(); i++) {
            QString _statusStr = bList[i].status;
            if (_statusStr.compare(status, Qt::CaseInsensitive) != 0)
                continue;
            currentBootList.append(bList[i]);
        }
    }

    createBootTable(currentBootList);
}

void DisplayContent::slot_dpkgFinished(QList<LOG_MSG_DPKG> list)
{
    if (m_flag != DPKG)
        return;
    dList = list;
    createDpkgTable(dList);
}

void DisplayContent::slot_XorgFinished(QList<LOG_MSG_XORG> list)
{
    if (m_flag != XORG)
        return;
    xList = list;
    createXorgTable(xList);
}

void DisplayContent::slot_bootFinished(QList<LOG_MSG_BOOT> list)
{
    if (m_flag != BOOT)
        return;

    bList.clear();
    bList = list;
    currentBootList.clear();
    currentBootList = bList;
    createBootTable(currentBootList);
}

void DisplayContent::slot_kernFinished(QList<LOG_MSG_JOURNAL> list)
{
    if (m_flag != KERN)
        return;
    kListOrigin = list;
    kList = list;
    setLoadState(DATA_COMPLETE);
    createKernTable(kList);
}

void DisplayContent::slot_kwinFinished(QList<LOG_MSG_KWIN> list)
{
    if (m_flag != Kwin)
        return;

    m_kwinList = list;
    m_currentKwinList = m_kwinList;
    setLoadState(DATA_COMPLETE);
    creatKwinTable(m_currentKwinList);
}

void DisplayContent::slot_journalFinished()
{

}

void DisplayContent::slot_journalData(QList<LOG_MSG_JOURNAL> list)
{
    if (m_flag != JOURNAL)
        return;

    if (list.isEmpty()) {
        setLoadState(DATA_COMPLETE);
        createJournalTableStart(jList);
        return;
    }
    jListOrigin.append(list);
    jList.append(list);
    if (m_firstLoadPageData) {
        createJournalTableStart(jList);
        m_firstLoadPageData = false;
    }
    // qDebug() << "jList" << jList.count();
}

void DisplayContent::slot_applicationFinished(QList<LOG_MSG_APPLICATOIN> list)
{
    if (m_flag != APP)
        return;

    appList.clear();
    setLoadState(DATA_COMPLETE);
    appList = list;
    createApplicationTable(appList);
}

void DisplayContent::slot_NormalFinished()
{
    if (m_flag != Normal)
        return;
    setLoadState(DATA_COMPLETE);
    nortempList = norList;
    //    createXorgTable(xList);
    createNormalTable(nortempList);
}

void DisplayContent::slot_vScrollValueChanged(int value)
{
    if (m_flag == JOURNAL) {
        int rate = (value + 25) / SINGLE_LOAD;
        //    qDebug() << "value: " << value << "rate: " << rate << "single: " << SINGLE_LOAD;

        if (value < SINGLE_LOAD * rate - 20 || value < SINGLE_LOAD * rate) {
            if (m_limitTag >= rate)
                return;

            int leftCnt = jList.count() - SINGLE_LOAD * rate;
            int end = leftCnt > SINGLE_LOAD ? SINGLE_LOAD : leftCnt;
            //        qDebug() << "total count: " << jList.count() << "left count : " << leftCnt
            //                 << " start : " << SINGLE_LOAD * rate << "end: " << end + SINGLE_LOAD
            //                 * rate;
            qDebug() << "rate" << rate;
            insertJournalTable(jList, SINGLE_LOAD * rate, SINGLE_LOAD * rate + end);

            m_limitTag = rate;
        }
        m_treeView->verticalScrollBar()->setValue(value);
    } else if (m_flag == APP) {
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
        }
        m_treeView->verticalScrollBar()->setValue(value);
    } else if (m_flag == KERN) {  // modified by Airy for bug 12263
        int rate = (value + 25) / SINGLE_LOAD;

        if (value < SINGLE_LOAD * rate - 20 || value < SINGLE_LOAD * rate) {
            if (m_limitTag >= rate)
                return;

            int leftCnt = kList.count() - SINGLE_LOAD * rate;
            int end = leftCnt > SINGLE_LOAD ? SINGLE_LOAD : leftCnt;

            insertKernTable(kList, SINGLE_LOAD * rate, SINGLE_LOAD * rate + end);

            m_limitTag = rate;
        }
        m_treeView->verticalScrollBar()->setValue(value);
    }
}

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
        createJournalTableForm();
        createJournalTableStart(jList);
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
        createKernTable(kList);
    } break;
    case BOOT: {
        QList<LOG_MSG_BOOT> tmp = currentBootList;
        int cnt = tmp.count();
        for (int i = cnt - 1; i >= 0; --i) {
            LOG_MSG_BOOT msg = tmp.at(i);
            if (msg.status.contains(m_currentSearchStr, Qt::CaseInsensitive) ||
                    msg.msg.contains(m_currentSearchStr, Qt::CaseInsensitive))
                continue;
            tmp.removeAt(i);
        }
        createBootTable(tmp);
    } break;
    case XORG: {
        QList<LOG_MSG_XORG> tmp = xList;
        int cnt = tmp.count();
        for (int i = cnt - 1; i >= 0; --i) {
            LOG_MSG_XORG msg = tmp.at(i);
            if (msg.dateTime.contains(m_currentSearchStr, Qt::CaseInsensitive) ||
                    msg.msg.contains(m_currentSearchStr, Qt::CaseInsensitive))
                continue;
            tmp.removeAt(i);
        }
        createXorgTable(tmp);
    } break;
    case DPKG: {
        QList<LOG_MSG_DPKG> tmp = dList;
        int cnt = tmp.count();
        for (int i = cnt - 1; i >= 0; --i) {
            LOG_MSG_DPKG msg = tmp.at(i);
            if (msg.dateTime.contains(m_currentSearchStr, Qt::CaseInsensitive) ||
                    msg.msg.contains(m_currentSearchStr, Qt::CaseInsensitive))
                continue;
            tmp.removeAt(i);
        }
        createDpkgTable(tmp);
    } break;
    case APP: {
        QList<LOG_MSG_APPLICATOIN> tmp = appList;
        int cnt = tmp.count();
        for (int i = cnt - 1; i >= 0; --i) {
            LOG_MSG_APPLICATOIN msg = tmp.at(i);
            if (msg.dateTime.contains(m_currentSearchStr, Qt::CaseInsensitive) ||
                    msg.level.contains(m_currentSearchStr, Qt::CaseInsensitive) ||
                    msg.src.contains(m_currentSearchStr, Qt::CaseInsensitive) ||
                    msg.msg.contains(m_currentSearchStr, Qt::CaseInsensitive))
                continue;
            tmp.removeAt(i);
        }
        createAppTable(tmp);
    } break;
    case Normal: {
        QList<LOG_MSG_NORMAL> tmp = norList;
        int cnt = tmp.count();
        for (int i = cnt - 1; i >= 0; --i) {
            LOG_MSG_NORMAL msg = tmp.at(i);
            if (msg.eventType.contains(m_currentSearchStr, Qt::CaseInsensitive) ||
                    msg.userName.contains(m_currentSearchStr, Qt::CaseInsensitive) ||
                    msg.dateTime.contains(m_currentSearchStr, Qt::CaseInsensitive) ||
                    msg.msg.contains(m_currentSearchStr, Qt::CaseInsensitive))
                continue;
            tmp.removeAt(i);
        }
        createNormalTable(tmp);
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
void DisplayContent::slot_getLogtype(int tcbx)
{
    if (0 == tcbx) {
        nortempList.clear();
        for (auto i = 0; i < norList.size(); i++) {
            nortempList.append(norList[i]);
        }
    } else if (1 == tcbx) {
        nortempList.clear();
        for (auto i = 0; i < norList.size(); i++) {
            if (norList[i].eventType.compare("Boot", Qt::CaseInsensitive) != 0 &&
                    norList[i].eventType.compare("shutdown", Qt::CaseInsensitive) != 0 &&
                    norList[i].eventType.compare("runlevel", Qt::CaseInsensitive) != 0)
                nortempList.append(norList[i]);
        }
    } else if (2 == tcbx) {
        nortempList.clear();
        for (auto i = 0; i < norList.size(); i++) {
            if (norList[i].eventType.compare("Boot", Qt::CaseInsensitive) == 0)
                nortempList.append(norList[i]);
        }
    } else if (3 == tcbx) {
        nortempList.clear();
        for (auto i = 0; i < norList.size(); i++) {
            if (norList[i].eventType.compare("shutdown", Qt::CaseInsensitive) == 0)
                nortempList.append(norList[i]);
        }
    }
    createNormalTable(nortempList);
}

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
        items << item;
        item = new DStandardItem(list[i].msg);
        item->setData(DPKG_TABLE_DATA);
        items << item;
        item = new DStandardItem(list[i].action);
        item->setData(DPKG_TABLE_DATA);
        items << item;
        oPModel->insertRow(oPModel->rowCount(), items);
    }
}

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
        item->setData(BOOT_TABLE_DATA);
        items << item;
        item = new DStandardItem(list[i].msg);
        item->setData(BOOT_TABLE_DATA);
        items << item;
        oPModel->insertRow(oPModel->rowCount(), items);
    }
}

void DisplayContent::parseListToModel(QList<LOG_MSG_APPLICATOIN> iList, QStandardItemModel *oPModel)
{
    if (!oPModel) {
        qWarning() << "parse model is  Empty" << __LINE__;
        return;
    }
    oPModel->setColumnCount(4);
    oPModel->setHorizontalHeaderLabels(QStringList()
                                       << DApplication::translate("Table", "Level")
                                       << DApplication::translate("Table", "Date and Time")
                                       << DApplication::translate("Table", "Source")
                                       << DApplication::translate("Table", "Info"));
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
        items << item;
        item = new DStandardItem(list[i].dateTime);
        item->setData(APP_TABLE_DATA);
        items << item;
        //        item = new DStandardItem(list[i].src);
        item = new DStandardItem(getAppName(m_curAppLog));
        item->setData(APP_TABLE_DATA);
        items << item;
        item = new DStandardItem(list[i].msg);
        item->setData(APP_TABLE_DATA);
        items << item;
        oPModel->insertRow(oPModel->rowCount(), items);
    }
}

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
        items << item;
        item = new DStandardItem(list[i].msg);
        item->setData(XORG_TABLE_DATA);
        items << item;
        oPModel->insertRow(oPModel->rowCount(), items);
    }
}

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
        items << item;
        item = new DStandardItem(list[i].userName);
        item->setData(LAST_TABLE_DATA);
        items << item;
        item = new DStandardItem(list[i].dateTime);
        item->setData(LAST_TABLE_DATA);
        items << item;
        item = new DStandardItem(list[i].msg);
        item->setData(LAST_TABLE_DATA);
        items << item;
        oPModel->insertRow(oPModel->rowCount(), items);
    }
}

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
        items << item;
        oPModel->insertRow(oPModel->rowCount(), items);
    }
}

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
        m_spinnerWgt->show();
        m_spinnerWgt->spinnerStart();
        break;
    }
    case DATA_COMPLETE: {
        m_treeView->show();
        break;
    }
    case DATA_LOADING_K: {
        m_spinnerWgt_K->show();
        m_spinnerWgt_K->spinnerStart();
        break;
    }
    case DATA_NO_SEARCH_RESULT: {
        m_treeView->show();
        noResultLabel->resize(m_treeView->viewport()->width(), m_treeView->viewport()->height());
        noResultLabel->show();
        break;
    }
    }
    this->update();

}

void DisplayContent::onExportResult(bool isSuccess)
{
    QString titleIcon = ICONPREFIX ;
    if (isSuccess) {
        if (m_exportDlg) {
            m_exportDlg->hide();
        }
        DMessageManager::instance()->sendMessage(this->window(), QIcon(titleIcon + "ok.svg"), DApplication::translate("ExportMessage", "Export Success"));
    }
}

void DisplayContent::clearAllFilter()
{
    m_bootFilter = {"", ""};

    m_currentSearchStr.clear();
    m_currentKwinFilter = {""};
}

void DisplayContent::clearAllDatalist()
{
    m_detailWgt->cleanText();
    m_pModel->clear();
    jList.clear();
    jListOrigin.clear();
    dList.clear();
    xList.clear();
    bList.clear();
    currentBootList.clear();
    kList.clear();
    kListOrigin.clear();
    appList.clear();
    norList.clear();
    nortempList.clear();
    m_currentKwinList.clear();
    m_kwinList.clear();
}

void DisplayContent::onExportProgress(int nCur, int nTotal)
{
    if (!m_exportDlg) {
        return;
    }
    //弹窗
    if (m_exportDlg->isHidden()) {
        m_exportDlg->show();
    }
    m_exportDlg->setProgressBarRange(0, nTotal);
    m_exportDlg->updateProgressBarValue(nCur);
}

void DisplayContent::parseListToModel(QList<LOG_MSG_JOURNAL> iList, QStandardItemModel *oPModel)
{
    if (!oPModel) {
        qWarning() << "parse model is  Empty" << __LINE__;
        return;
    }
    //oPModel->setColumnCount(4);
    oPModel->setHorizontalHeaderLabels(QStringList()
                                       << DApplication::translate("Table", "Date and Time")
                                       << DApplication::translate("Table", "User")
                                       << DApplication::translate("Table", "Process")
                                       << DApplication::translate("Table", "Info"));
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

void DisplayContent::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event)
    noResultLabel->resize(m_treeView->viewport()->width(), m_treeView->viewport()->height());
}

QString DisplayContent::getIconByname(QString str)
{
    //    qDebug() << str << m_icon_name_map.value(str);
    return m_icon_name_map.value(str);
}

void DisplayContent::createApplicationTable(QList<LOG_MSG_APPLICATOIN> &list)
{
    //    m_treeView->show();
    m_limitTag = 0;
    setLoadState(DATA_COMPLETE);
    m_pModel->clear();


    int end = list.count() > SINGLE_LOAD ? SINGLE_LOAD : list.count();
    insertApplicationTable(list, 0, end);
}

void DisplayContent::insertApplicationTable(QList<LOG_MSG_APPLICATOIN> list, int start, int end)
{
    QList<LOG_MSG_APPLICATOIN> midList = list;
    if (end >= start) {
        midList = midList.mid(start, end - start);
    }
    parseListToModel(midList, m_pModel);
    QItemSelectionModel *p = m_treeView->selectionModel();
    m_treeView->setColumnWidth(0, LEVEL_WIDTH);
    m_treeView->setColumnWidth(1, DATETIME_WIDTH + 20);
    m_treeView->setColumnWidth(2, DEAMON_WIDTH);
    if (p)
        p->select(m_pModel->index(0, 0), QItemSelectionModel::Rows | QItemSelectionModel::Select);
    slot_tableItemClicked(m_pModel->index(0, 0));
}

/**
 * @author Airy
 * @brief DisplayContent::slot_refreshClicked for refresh
 * @param index
 */
void DisplayContent::slot_refreshClicked(const QModelIndex &index)
{
    if (!index.isValid()) {
        return;
    }

    m_curListIdx = index;
    void clearAllFilter();
    void clearAllDatalist();
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
        m_flag = APP;
        generateAppFile(m_curAppLog, m_curBtnId, m_curLevel);
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
    }


    if (!itemData.contains(JOUR_TREE_DATA, Qt::CaseInsensitive) ||
            !itemData.contains(KERN_TREE_DATA, Qt::CaseInsensitive)) {  // modified by Airy
//        m_spinnerWgt_K->spinnerStop();
//        m_treeView->show();
//        m_spinnerWgt_K->hide();
    }
}
