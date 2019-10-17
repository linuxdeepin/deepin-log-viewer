#include "logtreeview.h"
#include <DApplication>
#include <DStyledItemDelegate>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QHeaderView>
#include "structdef.h"

DWIDGET_USE_NAMESPACE

LogTreeView::LogTreeView(QWidget *parent)
    : DTreeView(parent)
{
    initUI();
}

void LogTreeView::initUI()
{
    this->setAnimated(true);
    this->header()->setHidden(true);
    this->setEditTriggers(QAbstractItemView::NoEditTriggers);
    this->setRootIsDecorated(false);

    m_pModel = new QStandardItemModel;
    QStandardItem *item = nullptr;

    item = new QStandardItem(DApplication::translate("Tree", "System Log"));
    item->setData(JOUR_TREE_DATA);
    item->setIcon(QIcon("://images/logo.svg"));
    m_pModel->appendRow(item);

#if 0
    item = new QStandardItem("系统日志");
    QStandardItem *cItem = nullptr;
    cItem = new QStandardItem(DApplication::translate("Tree", "Kernel log"));
    cItem->setData(KERN_TREE_DATA);
    item->appendRow(cItem);
    cItem = new QStandardItem(DApplication::translate("Tree", "Boot log"));
    cItem->setData(BOOT_TREE_DATA);
    item->appendRow(cItem);
    cItem = new QStandardItem(DApplication::translate("Tree", "dpkg log"));
    cItem->setData(DPKG_TREE_DATA);
    item->appendRow(cItem);
    cItem = new QStandardItem(DApplication::translate("Tree", "Xorg log"));
    cItem->setData(XORG_TREE_DATA);
    item->appendRow(cItem);
    m_pModel->appendRow(item);
#endif

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

void LogTreeView::clearChildren(QStandardItem *pItem)
{
    int childCnt = pItem->rowCount();
    for (int i = 0; i < childCnt; i++) {
        pItem->removeRow(0);
    }
}

void LogTreeView::slot_addAppLogs(QString path)
{
    /*** 20191010,discard this func.
     **  clicked application node, display all logs in application-list combobox
     **  select log then show in the table direct.
     ***/
    Q_UNUSED(path)
#if 0
    QStandardItem *lastItem = m_pModel->item(2);
    if (nullptr == lastItem)
        return;

    clearChildren(lastItem);

    QDir dir(path);
    if (dir.exists()) {
        QStringList logFiles = dir.entryList(QDir::NoDotAndDotDot | QDir::Files);
        QStandardItem *cItem = nullptr;
        for (int i = 0; i < logFiles.count(); i++) {
            QString fileName = logFiles.at(i);
            if (!fileName.contains(".log"))
                continue;
            cItem = new QStandardItem(logFiles.at(i));
            cItem->setData(path + "/" + logFiles.at(i), Qt::UserRole + 1);
            lastItem->appendRow(cItem);
        }
    }
    this->expandAll();
#endif
}
