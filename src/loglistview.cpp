#include "loglistview.h"

#include <DApplication>
#include <QHeaderView>
#include "structdef.h"

DWIDGET_USE_NAMESPACE

LogListView::LogListView(QWidget *parent)
    : DListView(parent)
{
    initUI();
}

void LogListView::initUI()
{
    this->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_pModel = new QStandardItemModel;
    QStandardItem *item = nullptr;

    item = new QStandardItem(DApplication::translate("Tree", "System Log"));
    item->setData(JOUR_TREE_DATA);
    item->setIcon(QIcon("://images/logo.svg"));
    m_pModel->appendRow(item);

    item = new QStandardItem(DApplication::translate("Tree", "Kernel Log"));
    item->setData(KERN_TREE_DATA);
    item->setIcon(QIcon("://images/logo.svg"));
    m_pModel->appendRow(item);

    item = new QStandardItem(DApplication::translate("Tree", "Boot Log"));
    item->setData(BOOT_TREE_DATA);
    item->setIcon(QIcon("://images/logo.svg"));
    m_pModel->appendRow(item);

    item = new QStandardItem(DApplication::translate("Tree", "dpkg Log"));
    item->setData(DPKG_TREE_DATA);
    item->setIcon(QIcon("://images/logo.svg"));
    m_pModel->appendRow(item);

    item = new QStandardItem(DApplication::translate("Tree", "Xorg Log"));
    item->setData(XORG_TREE_DATA);
    item->setIcon(QIcon("://images/logo.svg"));
    m_pModel->appendRow(item);

    item = new QStandardItem(DApplication::translate("Tree", "Application log"));
    item->setData(APP_TREE_DATA);
    item->setIcon(QIcon("://images/logo.svg"));
    m_pModel->appendRow(item);

    this->setModel(m_pModel);
}
