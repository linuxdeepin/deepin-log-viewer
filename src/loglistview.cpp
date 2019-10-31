#include "loglistview.h"

#include <DApplication>
#include <DApplicationHelper>
#include <QHeaderView>
#include <QMargins>
#include <QPaintEvent>
#include <QPainter>
#include "structdef.h"

#define ITEM_HEIGHT 40
#define ITEM_WIDTH 100

DWIDGET_USE_NAMESPACE

LogListView::LogListView(QWidget *parent)
    : DListView(parent)
{
    initUI();
}

void LogListView::initUI()
{
    this->setAutoFillBackground(true);
    this->setBackgroundRole(DPalette::Base);

    this->setEditTriggers(QAbstractItemView::NoEditTriggers);

    this->setItemSpacing(2);
    m_pModel = new QStandardItemModel;

    QStandardItem *item = nullptr;

    item = new QStandardItem(DApplication::translate("Tree", "System Log"));
    item->setData(JOUR_TREE_DATA);
    item->setIcon(QIcon("://images/logo.svg"));
    item->setSizeHint(QSize(ITEM_WIDTH, ITEM_HEIGHT));
    m_pModel->appendRow(item);

    item = new QStandardItem(DApplication::translate("Tree", "Kernel Log"));
    item->setData(KERN_TREE_DATA);
    item->setIcon(QIcon("://images/logo.svg"));
    item->setSizeHint(QSize(ITEM_WIDTH, ITEM_HEIGHT));
    m_pModel->appendRow(item);

    item = new QStandardItem(DApplication::translate("Tree", "Boot Log"));
    item->setData(BOOT_TREE_DATA);
    item->setIcon(QIcon("://images/logo.svg"));
    item->setSizeHint(QSize(ITEM_WIDTH, ITEM_HEIGHT));
    m_pModel->appendRow(item);

    item = new QStandardItem(DApplication::translate("Tree", "dpkg Log"));
    item->setData(DPKG_TREE_DATA);
    item->setIcon(QIcon("://images/logo.svg"));
    item->setSizeHint(QSize(ITEM_WIDTH, ITEM_HEIGHT));
    m_pModel->appendRow(item);

    item = new QStandardItem(DApplication::translate("Tree", "Xorg Log"));
    item->setData(XORG_TREE_DATA);
    item->setIcon(QIcon("://images/logo.svg"));
    item->setSizeHint(QSize(ITEM_WIDTH, ITEM_HEIGHT));
    m_pModel->appendRow(item);

    item = new QStandardItem(DApplication::translate("Tree", "Application log"));
    item->setData(APP_TREE_DATA);
    item->setIcon(QIcon("://images/logo.svg"));
    item->setSizeHint(QSize(ITEM_WIDTH, ITEM_HEIGHT));
    m_pModel->appendRow(item);

    this->setModel(m_pModel);
}

void LogListView::paintEvent(QPaintEvent *event)
{
    //    QPainter painter(this->viewport());
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

    DListView::paintEvent(event);
}
