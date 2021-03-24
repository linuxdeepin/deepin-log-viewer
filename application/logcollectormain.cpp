/*
* Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
*
* Author:    LZ <zhou.lu@archermind.com>
*
* Maintainer:  LZ <zhou.lu@archermind.com>
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
#include "logsettings.h"
#include "DebugTimeManager.h"

#include <DApplication>
#include <DTitlebar>
#include <DWindowOptionButton>
#include <DWindowCloseButton>

#include <QDateTime>
#include <QDebug>
#include <QHeaderView>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QSizePolicy>
#include <QList>
#include <QKeyEvent>
#include <DAboutDialog>
//958+53+50 976

//日志类型选择器宽度
#define LEFT_LIST_WIDTH 200
DWIDGET_USE_NAMESPACE

/**
 * @brief LogCollectorMain::LogCollectorMain 构造函数
 * @param parent 父对象
 */
LogCollectorMain::LogCollectorMain(QWidget *parent)
    : DMainWindow(parent)
{
    initUI();
    initConnection();

    initShortCut();
    //日志类型选择器选第一个
    m_logCatelogue->setDefaultSelect();
    //设置最小窗口尺寸
    setMinimumSize(MAINWINDOW_WIDTH, MAINWINDOW_HEIGHT);
    //恢复上次关闭时记录的窗口大小
    resize(LogSettings::instance()->getConfigWinSize());

}

/**
 * @brief LogCollectorMain::~LogCollectorMain 析构函数
 */
LogCollectorMain::~LogCollectorMain()
{
    /** delete when app quit */
    if (m_searchEdt) {
        delete m_searchEdt;
        m_searchEdt = nullptr;
    }
    //如果窗体状态不是最大最小状态，则记录此时窗口尺寸到配置文件里，方便下次打开时恢复大小
    if (windowState() == Qt::WindowNoState) {
        LogSettings::instance()->saveConfigWinSize(width(), height());
    }
}

/**
 * @brief LogCollectorMain::initUI 初始化界面
 */
void LogCollectorMain::initUI()
{
    /** add searchEdit */
    m_searchEdt = new DSearchEdit();

    // m_searchEdt->setFocusPolicy(Qt::TabFocus);
    m_searchEdt->setPlaceHolder(DApplication::translate("SearchBar", "Search"));
    m_searchEdt->setMaximumWidth(400);
    // m_searchEdt->setFocusPolicy(Qt::StrongFocus);
    titlebar()->setCustomWidget(m_searchEdt, true);
    /** add titleBar */
    titlebar()->setIcon(QIcon::fromTheme("deepin-log-viewer"));
    titlebar()->setTitle("");
    /** menu */
    //    titlebar()->menu()->addAction(new QAction(tr("help")));

#ifdef SPLITTER_TYPE

    //m_hLayout = new QHBoxLayout;
    m_hSplitter = new Dtk::Widget::DSplitter(this);
    m_hSplitter->setOrientation(Qt::Horizontal);
    /** left frame */
    m_logCatelogue = new LogListView(this);
    m_logCatelogue->setMaximumWidth(LEFT_LIST_WIDTH);
    m_logCatelogue->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

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
    rightWidget->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
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

#else

    this->setCentralWidget(new DWidget());

    m_hLayout = new QHBoxLayout;

    /** left frame */
    m_logCatelogue = new LogListView();
    m_logCatelogue->setObjectName("logTypeSelectList");
    m_logCatelogue->setAccessibleName("logTypeSelectList");
    m_hLayout->addWidget(m_logCatelogue, 1);
    m_logCatelogue->setFixedWidth(160);
    m_vLayout = new QVBoxLayout;
    /** topRight frame */
    m_topRightWgt = new FilterContent();
    m_vLayout->addWidget(m_topRightWgt);
    /** midRight frame */
    m_midRightWgt = new DisplayContent();

    m_vLayout->addWidget(m_midRightWgt, 1);
    m_vLayout->setContentsMargins(0, 10, 0, 10);
    m_vLayout->setSpacing(6);

    m_hLayout->addLayout(m_vLayout, 10);
    m_hLayout->setContentsMargins(0, 0, 10, 0);
    m_hLayout->setSpacing(10);

    this->centralWidget()->setLayout(m_hLayout);
    m_searchEdt->setObjectName("searchEdt");
    m_searchEdt->setAccessibleName("searchEdt");
    m_searchEdt->lineEdit()->setObjectName("searchChildEdt");
    m_searchEdt->lineEdit()->setAccessibleName("searchChildEdt");
    m_topRightWgt->setObjectName("FilterContent");
    m_midRightWgt->setObjectName("DisplayContent");
    titlebar()->setObjectName("titlebar");

#endif

    m_originFilterWidth = m_topRightWgt->geometry().width();
}

/**
 * @brief LogCollectorMain::initConnection 连接信号槽
 */
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
    connect(m_midRightWgt, &DisplayContent::setExportEnable, m_topRightWgt,
            &FilterContent::setExportButtonEnable, Qt::DirectConnection);
    //自适应宽度
//    connect(m_topRightWgt, &FilterContent::sigResizeWidth, this,
//            &LogCollectorMain::resizeWidthByFilterContentWidth);
    connect(m_logCatelogue, SIGNAL(sigRefresh(const QModelIndex &)), m_midRightWgt,
            SLOT(slot_refreshClicked(const QModelIndex &)));  // add by Airy for adding refresh
    connect(m_logCatelogue, SIGNAL(sigRefresh(const QModelIndex &)), m_topRightWgt,
            SLOT(slot_logCatelogueRefresh(const QModelIndex &)));
    connect(m_logCatelogue, &LogListView::sigRefresh, this, [=]() { m_searchEdt->clearEdit(); });
    //! treeView widget

    connect(m_logCatelogue, SIGNAL(itemChanged(const QModelIndex &)), m_midRightWgt,
            SLOT(slot_logCatelogueClicked(const QModelIndex &)));

    //! set tree <==> combobox visible
    connect(m_logCatelogue, SIGNAL(itemChanged(const QModelIndex &)), m_topRightWgt,
            SLOT(slot_logCatelogueClicked(const QModelIndex &)));

    // when item changed clear search text
    connect(m_logCatelogue, &LogListView::itemChanged, this, [=]() { m_searchEdt->clearEdit(); });
    connect(m_topRightWgt, &FilterContent::sigButtonClicked, this, [=]() { m_searchEdt->clearEdit(); });
    connect(m_topRightWgt, &FilterContent::sigCbxAppIdxChanged, this,
            [=]() { m_searchEdt->clearEdit(); });
}

/**
 * @brief LogCollectorMain::initShortCut 初始化快捷键
 */
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

/**
 * @brief LogCollectorMain::resizeWidthByFilterContentWidth
 * @param iWidth
 */
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

/**
 * @brief LogCollectorMain::handleApplicationTabEventNotify
 * 处理application中notify的tab keyevent ,直接在dapplication中调用
 * 只调整我们需要调整的顺序,其他的默认
 * @param obj 接收事件的对象
 * @param evt 对象接收的键盘事件
 * @return true处理并屏蔽事件 false交给application的notify处理
 */
bool LogCollectorMain::handleApplicationTabEventNotify(QObject *obj, QKeyEvent *evt)
{

//    qDebug() << "handleApplicationTabEventNotify" << obj->objectName() << obj->metaObject()->className() << obj << this->titlebar();

    if (evt->key() == Qt::Key_Tab) {
        DWindowCloseButton *closebtn = this->titlebar()->findChild<DWindowCloseButton *>("DTitlebarDWindowCloseButton");
        if (obj == this->titlebar()) {
            return false;
        } else if (obj->objectName() == "searchChildEdt") {
            return false;
        } else if (obj == closebtn) {
            m_logCatelogue->setFocus(Qt::TabFocusReason);
            return  true;
        } else if (obj->objectName() == "mainLogTable") {
            return false;
        }
    } else if (evt->key() == Qt::Key_Backtab) {
        DWindowOptionButton *optionbtn = this->titlebar()->findChild<DWindowOptionButton *>("DTitlebarDWindowOptionButton");
        if (obj->objectName() == "logTypeSelectList") {
            DWindowCloseButton   *closeButton = titlebar()->findChild<DWindowCloseButton *>("DTitlebarDWindowCloseButton");
            if (closeButton) {
                closeButton->setFocus();
            }
            return  true;
        } else if (obj == optionbtn) {
            return false;
        } else if (obj->objectName() == "searchChildEdt") {
            m_midRightWgt->mainLogTableView()->setFocus(Qt::BacktabFocusReason);
            return  true;
        }
    }
    return  false;
}

void LogCollectorMain::closeEvent(QCloseEvent *event)
{
    PERF_PRINT_BEGIN("POINT-02", "");
    DMainWindow::closeEvent(event);
}



