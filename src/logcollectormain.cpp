#include "logcollectormain.h"

#include <DApplication>
#include <DTitlebar>
#include <QDebug>
#include <QHeaderView>
#include <QStandardItem>
#include <QStandardItemModel>

DWIDGET_USE_NAMESPACE

LogCollectorMain::LogCollectorMain(QWidget *parent)
    : DMainWindow(parent)
{
    initUI();
    initConnection();

    m_logCatelogue->setDefaultSelect();
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
    titlebar()->setIcon(QIcon::fromTheme("dde-log-viewer"));
    titlebar()->setTitle("");

    /** menu */
    //    titlebar()->menu()->addAction(new QAction(tr("help")));

    /** inherit QMainWidow, why need new centralWidget?? */
    this->setCentralWidget(new DWidget());

    m_hLayout = new QHBoxLayout(this->centralWidget());
    m_hSplitter = new QSplitter(this->centralWidget());
    m_hSplitter->setOrientation(Qt::Horizontal);

    /** left frame */
    m_logCatelogue = new LogListView(m_hSplitter);
    m_hSplitter->addWidget(m_logCatelogue);

    QWidget *layoutWidget = new QWidget(m_hSplitter);

    m_vLayout = new QVBoxLayout(layoutWidget);

    /** topRight frame */
    m_topRightWgt = new FilterContent(layoutWidget);
    m_vLayout->addWidget(m_topRightWgt);

    /** midRight frame */
    m_midRightWgt = new DisplayContent(layoutWidget);
    m_vLayout->addWidget(m_midRightWgt, 1);
    m_vLayout->setContentsMargins(6, 0, 0, 0);
    m_vLayout->setSpacing(6);

    m_hSplitter->addWidget(layoutWidget);

    m_hLayout->addWidget(m_hSplitter);
    m_hLayout->setSizeConstraint(QLayout::SetMinAndMaxSize);

    m_hSplitter->setStretchFactor(0, 5);
    m_hSplitter->setStretchFactor(1, 15);

#if 0
    // filter button
    DPushButton *filterBtn = new DPushButton("filter");
    filterBtn->setCheckable(true);
    filterBtn->setChecked(true);
    titlebar()->addWidget(filterBtn);
    connect(filterBtn, &DPushButton::clicked, this, [=](bool checked) {
        if (checked) {
            m_topRightWgt->show();
        } else {
            m_topRightWgt->hide();
        }
    });
#endif
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

    //! treeView widget
    connect(m_logCatelogue, SIGNAL(clicked(const QModelIndex &)), m_midRightWgt,
            SLOT(slot_logCatelogueClicked(const QModelIndex &)));
    //! set tree <==> combobox visible
    connect(m_logCatelogue, SIGNAL(clicked(const QModelIndex &)), m_topRightWgt,
            SLOT(slot_logCatelogueClicked(const QModelIndex &)));

    // when item changed clear search text
    connect(m_logCatelogue, &LogListView::itemChanged, this, [=]() { m_searchEdt->clear(); });
    connect(m_topRightWgt, &FilterContent::sigButtonClicked, this, [=]() { m_searchEdt->clear(); });
    connect(m_topRightWgt, &FilterContent::sigCbxAppIdxChanged, this,
            [=]() { m_searchEdt->clear(); });
}
