#include <DApplication>
#include <QHeaderView>
#include <QMargins>

#include <QPaintEvent>
#include <QPainter>

#include "loglistview.h"
#include "structdef.h"

#define ITEM_HEIGHT 36
#define ITEM_WIDTH 108

#define ICON_DATA (Qt::UserRole + 99)

DWIDGET_USE_NAMESPACE

LogListView::LogListView(QWidget *parent)
    : DListView(parent)
{
    initUI();

    connect(DApplicationHelper::instance(), &DApplicationHelper::themeTypeChanged, this,
            &LogListView::onChangedTheme);
    DGuiApplicationHelper::ColorType ct = DApplicationHelper::instance()->themeType();
    onChangedTheme(ct);
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
    item->setSizeHint(QSize(ITEM_WIDTH, ITEM_HEIGHT));
    m_pModel->appendRow(item);

    item = new QStandardItem(DApplication::translate("Tree", "Kernel Log"));
    item->setData(KERN_TREE_DATA);
    item->setSizeHint(QSize(ITEM_WIDTH, ITEM_HEIGHT));
    m_pModel->appendRow(item);

    item = new QStandardItem(DApplication::translate("Tree", "Boot Log"));
    item->setData(BOOT_TREE_DATA);
    item->setSizeHint(QSize(ITEM_WIDTH, ITEM_HEIGHT));
    m_pModel->appendRow(item);

    item = new QStandardItem(DApplication::translate("Tree", "dpkg Log"));
    item->setData(DPKG_TREE_DATA);
    item->setSizeHint(QSize(ITEM_WIDTH, ITEM_HEIGHT));
    m_pModel->appendRow(item);

    item = new QStandardItem(DApplication::translate("Tree", "Xorg Log"));
    item->setData(XORG_TREE_DATA);
    item->setSizeHint(QSize(ITEM_WIDTH, ITEM_HEIGHT));
    m_pModel->appendRow(item);

    item = new QStandardItem(DApplication::translate("Tree", "Application log"));
    item->setData(APP_TREE_DATA);
    item->setSizeHint(QSize(ITEM_WIDTH, ITEM_HEIGHT));
    m_pModel->appendRow(item);
    this->setModel(m_pModel);
}

void LogListView::onChangedTheme(DGuiApplicationHelper::ColorType themeType)
{
    icon = "://images/";

    icon += ((themeType == DGuiApplicationHelper::LightType) ? "light/" : "dark/");

    QString _itemIcon;

    for (auto i = 0; i < m_pModel->rowCount(); i++) {
        QStandardItem *item = m_pModel->item(i);
        if (item) {
            if (item->data().toString() == JOUR_TREE_DATA) {
                _itemIcon = icon + "system.svg";
            } else if (item->data().toString() == KERN_TREE_DATA) {
                _itemIcon = icon + "core.svg";

            } else if (item->data().toString() == BOOT_TREE_DATA) {
                _itemIcon = icon + "start.svg";

            } else if (item->data().toString() == DPKG_TREE_DATA) {
                _itemIcon = icon + "d.svg";

            } else if (item->data().toString() == XORG_TREE_DATA) {
                _itemIcon = icon + "x.svg";

            } else if (item->data().toString() == APP_TREE_DATA) {
                _itemIcon = icon + "application.svg";
            }
            item->setIcon(QIcon(_itemIcon));
            item->setData(_itemIcon, ICON_DATA);
        }
    }
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

void LogListView::currentChanged(const QModelIndex &current, const QModelIndex &previous)
{
    QStandardItem *currentItem = m_pModel->itemFromIndex(current);
    if (currentItem) {
        QString icon = currentItem->data(ICON_DATA).toString();
        if (icon.isEmpty() == false) {
            icon.replace(".svg", "_checked.svg");
            currentItem->setIcon(QIcon(icon));
            currentItem->setData(icon, ICON_DATA);
        }
    }

    QStandardItem *previousItem = m_pModel->itemFromIndex(previous);
    if (previousItem) {
        QString icon = previousItem->data(ICON_DATA).toString();
        if (icon.isEmpty() == false) {
            icon.replace("_checked", "");
            previousItem->setIcon(QIcon(icon));
            previousItem->setData(icon, ICON_DATA);
        }
    }

    DListView::currentChanged(current, previous);
}
