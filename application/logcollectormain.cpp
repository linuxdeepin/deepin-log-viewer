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

#include "logcollectormain.h"

#include <DApplication>
#include <DTitlebar>
#include <QDateTime>
#include <QDebug>
#include <QHeaderView>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QSizePolicy>
#include <QList>
//958+53+50
#define MAINWINDOW_WIDTH 1011
#define MAINWINDOW_HEIGHT 736
#define LEFT_LIST_WIDTH 200
DWIDGET_USE_NAMESPACE

LogCollectorMain::LogCollectorMain(QWidget *parent)
    : DMainWindow(parent)
{
    initUI();
    initConnection();

    initShortCut();

    m_logCatelogue->setDefaultSelect();
    setMinimumSize(MAINWINDOW_WIDTH, MAINWINDOW_HEIGHT);
}

LogCollectorMain::~LogCollectorMain()
{
    /** delete when app quit */
    if (m_searchEdt) {
        delete m_searchEdt;
        m_searchEdt = nullptr;
    }
}

void LogCollectorMain::initUI()
{
    /** add searchEdit */
    m_searchEdt = new DSearchEdit;
    m_searchEdt->setPlaceHolder(DApplication::translate("SearchBar", "Search"));
    m_searchEdt->setMaximumWidth(400);
    titlebar()->setCustomWidget(m_searchEdt, true);

    /** add titleBar */
    titlebar()->setIcon(QIcon::fromTheme("deepin-log-viewer"));
    titlebar()->setTitle("");

    /** menu */
    //    titlebar()->menu()->addAction(new QAction(tr("help")));

    /** inherit QMainWidow, why need new centralWidget?? */
    this->setCentralWidget(new DWidget());

    //m_hLayout = new QHBoxLayout;
    m_hSplitter = new Dtk::Widget::DSplitter(this);
    m_hSplitter->setOrientation(Qt::Horizontal);
    /** left frame */
    m_logCatelogue = new LogListView(this);
    m_logCatelogue->setMaximumWidth(LEFT_LIST_WIDTH);
    m_logCatelogue->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);

    m_hSplitter->addWidget(m_logCatelogue);
    m_hSplitter->setStretchFactor(0, 1);

    m_vLayout = new QVBoxLayout;
    /** topRight frame */
    m_topRightWgt = new FilterContent();
    // m_topRightWgt->setMinimumWidth(8000);
    // m_topRightWgt->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    m_vLayout->addWidget(m_topRightWgt);
    /** midRight frame */
    m_midRightWgt = new DisplayContent(this);
    m_vLayout->addWidget(m_midRightWgt, 1);
    m_vLayout->setContentsMargins(0, 10, 0, 10);
    m_vLayout->setSpacing(6);
    DWidget *rightWidget = new DWidget(this);
    //撑开右侧控件
    rightWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    rightWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    rightWidget->setContentsMargins(10, 0, 0, 0);
    rightWidget->setLayout(m_vLayout);
    // rightWidget->setMinimumWidth(693);
    m_hSplitter->addWidget(rightWidget);
    m_hSplitter->setStretchFactor(1, 10);
    m_hSplitter->setOpaqueResize(true);
    m_hSplitter->setContentsMargins(0, 0, 10, 0);
    m_hSplitter->setChildrenCollapsible(false);
    m_hSplitter->setSizes(QList<int>() << 200);
    //m_hSplitter->setSpacing(10);
    this->setCentralWidget(m_hSplitter);
    // this->centralWidget()->setLayout(m_hSplitter);
    m_originFilterWidth = m_topRightWgt->geometry().width();
}

void LogCollectorMain::initConnection()
{
    //! search
    connect(m_searchEdt, &DSearchEdit::textChanged, m_midRightWgt,
            &DisplayContent::slot_searchResult);

    //! filter widget
    connect(m_topRightWgt, SIGNAL(sigButtonClicked(int, int, QModelIndex)), m_midRightWgt,
            SLOT(slot_BtnSelected(int, int, QModelIndex)));

    connect(m_topRightWgt, &FilterContent::sigCbxAppIdxChanged, m_midRightWgt,
            &DisplayContent::slot_appLogs);

    connect(m_topRightWgt, &FilterContent::sigExportInfo, m_midRightWgt,
            &DisplayContent::slot_exportClicked);

    connect(m_topRightWgt, &FilterContent::sigStatusChanged, m_midRightWgt,
            &DisplayContent::slot_statusChagned);

    connect(m_topRightWgt, &FilterContent::sigLogtypeChanged, m_midRightWgt,
            &DisplayContent::slot_getLogtype);  // add by Airy

    connect(m_topRightWgt, &FilterContent::sigCbxAppIdxChanged, m_logCatelogue,
            &LogListView::slot_getAppPath);  // add by Airy for getting app path
    //自适应宽度
//    connect(m_topRightWgt, &FilterContent::sigResizeWidth, this,
//            &LogCollectorMain::resizeWidthByFilterContentWidth);
    connect(m_logCatelogue, SIGNAL(sigRefresh(const QModelIndex &)), m_midRightWgt,
            SLOT(slot_refreshClicked(const QModelIndex &)));  // add by Airy for adding refresh
    connect(m_logCatelogue, &LogListView::sigRefresh, this, [ = ]() { m_searchEdt->clear(); });
    //! treeView widget
    connect(m_logCatelogue, SIGNAL(clicked(const QModelIndex &)), m_midRightWgt,
            SLOT(slot_logCatelogueClicked(const QModelIndex &)));
    //! set tree <==> combobox visible
    connect(m_logCatelogue, SIGNAL(clicked(const QModelIndex &)), m_topRightWgt,
            SLOT(slot_logCatelogueClicked(const QModelIndex &)));

    // when item changed clear search text
    connect(m_logCatelogue, &LogListView::itemChanged, this, [ = ]() { m_searchEdt->clear(); });
    connect(m_topRightWgt, &FilterContent::sigButtonClicked, this, [ = ]() { m_searchEdt->clear(); });
    connect(m_topRightWgt, &FilterContent::sigCbxAppIdxChanged, this,
    [ = ]() { m_searchEdt->clear(); });

    connect(DGuiApplicationHelper::instance(), &DGuiApplicationHelper::newProcessInstance, this,
    [ = ] {
        this->setWindowState(Qt::WindowActive);
        this->activateWindow();
    });
}

void LogCollectorMain::initShortCut()
{
    // Resize Window --> Ctrl+Alt+F
    if (nullptr == m_scWndReize) {
        m_scWndReize = new QShortcut(this);
        //      m_scWndReize->setKey(tr("Ctrl+Alt+F"));
        m_scWndReize->setKey(Qt::CTRL + Qt::ALT + Qt::Key_F);
        m_scWndReize->setContext(Qt::ApplicationShortcut);
        m_scWndReize->setAutoRepeat(false);

        connect(m_scWndReize, &QShortcut::activated, this, [this] {
            if (this->windowState() & Qt::WindowMaximized)
            {
                this->showNormal();
            } else if (this->windowState() == Qt::WindowNoState)
            {
                this->showMaximized();
            }
        });
    }

    // Find font --> Ctrl+F
    if (nullptr == m_scFindFont) {
        m_scFindFont = new QShortcut(this);
        //        m_scFindFont->setKey(tr("Ctrl+F"));
        m_scFindFont->setKey(Qt::CTRL + Qt::Key_F);
        m_scFindFont->setContext(Qt::ApplicationShortcut);
        m_scFindFont->setAutoRepeat(false);

        connect(m_scFindFont, &QShortcut::activated, this,
                [this] { this->m_searchEdt->lineEdit()->setFocus(Qt::MouseFocusReason); });
    }

    // export file --> Ctrl+E
    if (nullptr == m_scExport) {
        m_scExport = new QShortcut(this);
        //        m_scExport->setKey(tr("Ctrl+E"));
        m_scExport->setKey(Qt::CTRL + Qt::Key_E);
        m_scExport->setContext(Qt::ApplicationShortcut);
        m_scExport->setAutoRepeat(false);

        connect(m_scExport, &QShortcut::activated, this,
                [this] { this->m_topRightWgt->shortCutExport(); });
    }
}

void LogCollectorMain::resizeWidthByFilterContentWidth(int iWidth)
{
    int otherWidth = MAINWINDOW_WIDTH - m_originFilterWidth;
    qDebug() << "otherWidth" << otherWidth << iWidth;
    int currentWidth = otherWidth + iWidth;
    int  setedWidth = currentWidth > MAINWINDOW_WIDTH ? currentWidth : MAINWINDOW_WIDTH;
    //  setMinimumWidth(setedWidth);
    if (setedWidth <= MAINWINDOW_WIDTH) {
        // setM
        //setWidth(setWidth);
    }
}



