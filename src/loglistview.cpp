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

#include <DApplication>
#include <QDebug>
#include <QHeaderView>
#include <QItemSelectionModel>
#include <QMargins>
#include <QPaintEvent>
#include <QPainter>

#include "loglistview.h"
#include "structdef.h"

#define ITEM_HEIGHT 40
#define ITEM_WIDTH 108

#define ICON_DATA (Qt::UserRole + 99)

Q_DECLARE_METATYPE(QMargins)

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
    this->setMinimumWidth(128);
    this->setEditTriggers(QAbstractItemView::NoEditTriggers);

    this->setItemSpacing(0);
    this->setViewportMargins(10, 10, 10, 0);

    const QMargins ListViweItemMargin(5, 0, 0, 0);
    const QVariant VListViewItemMargin = QVariant::fromValue(ListViweItemMargin);

    m_pModel = new QStandardItemModel;
    QStandardItem *item = nullptr;
    item = new QStandardItem(DApplication::translate("Tree", "System Log"));
    item->setData(JOUR_TREE_DATA, ITEM_DATE_ROLE);
    item->setSizeHint(QSize(ITEM_WIDTH, ITEM_HEIGHT));
    item->setData(VListViewItemMargin, Dtk::MarginsRole);
    m_pModel->appendRow(item);

    item = new QStandardItem(DApplication::translate("Tree", "Kernel Log"));
    item->setData(KERN_TREE_DATA, ITEM_DATE_ROLE);
    item->setSizeHint(QSize(ITEM_WIDTH, ITEM_HEIGHT));
    item->setData(VListViewItemMargin, Dtk::MarginsRole);
    m_pModel->appendRow(item);

    item = new QStandardItem(DApplication::translate("Tree", "Boot Log"));
    item->setData(BOOT_TREE_DATA, ITEM_DATE_ROLE);
    item->setSizeHint(QSize(ITEM_WIDTH, ITEM_HEIGHT));
    item->setData(VListViewItemMargin, Dtk::MarginsRole);
    m_pModel->appendRow(item);

    item = new QStandardItem(DApplication::translate("Tree", "dpkg Log"));
    item->setData(DPKG_TREE_DATA, ITEM_DATE_ROLE);
    item->setSizeHint(QSize(ITEM_WIDTH, ITEM_HEIGHT));
    item->setData(VListViewItemMargin, Dtk::MarginsRole);
    m_pModel->appendRow(item);

    item = new QStandardItem(DApplication::translate("Tree", "Xorg Log"));
    item->setData(XORG_TREE_DATA, ITEM_DATE_ROLE);
    item->setSizeHint(QSize(ITEM_WIDTH, ITEM_HEIGHT));
    item->setData(VListViewItemMargin, Dtk::MarginsRole);
    m_pModel->appendRow(item);

    item = new QStandardItem(DApplication::translate("Tree", "Application log"));
    item->setData(APP_TREE_DATA, ITEM_DATE_ROLE);
    item->setSizeHint(QSize(ITEM_WIDTH, ITEM_HEIGHT));
    item->setData(VListViewItemMargin, Dtk::MarginsRole);
    m_pModel->appendRow(item);
    this->setModel(m_pModel);

    // set first item is select when app start.
    this->setCurrentIndex(m_pModel->index(0, 0));
}

void LogListView::setDefaultSelect()
{
    emit clicked(currentIndex());
}

void LogListView::setCustomFont(QStandardItem *item)
{
    QFont font = item->font();
    font.setPointSize(11);
    item->setFont(font);
    this->setIconSize(QSize(20, 20));

    //    item->setTextAlignment(Qt::AlignCenter);
}

void LogListView::onChangedTheme(DGuiApplicationHelper::ColorType themeType)
{
    icon = "://images/";

    icon += ((themeType == DGuiApplicationHelper::LightType) ? "light/" : "dark/");

    QString _itemIcon;

    QModelIndex idx = this->currentIndex();

    QStandardItem *currentItem = m_pModel->itemFromIndex(idx);

    for (auto i = 0; i < m_pModel->rowCount(); i++) {
        QStandardItem *item = m_pModel->item(i);

        if (item) {
            setCustomFont(item);
            if (item->data(ITEM_DATE_ROLE).toString() == JOUR_TREE_DATA) {
                _itemIcon = icon + "system.svg";
            } else if (item->data(ITEM_DATE_ROLE).toString() == KERN_TREE_DATA) {
                _itemIcon = icon + "core.svg";

            } else if (item->data(ITEM_DATE_ROLE).toString() == BOOT_TREE_DATA) {
                _itemIcon = icon + "start.svg";

            } else if (item->data(ITEM_DATE_ROLE).toString() == DPKG_TREE_DATA) {
                _itemIcon = icon + "d.svg";

            } else if (item->data(ITEM_DATE_ROLE).toString() == XORG_TREE_DATA) {
                _itemIcon = icon + "x.svg";

            } else if (item->data(ITEM_DATE_ROLE).toString() == APP_TREE_DATA) {
                _itemIcon = icon + "application.svg";
            }
            if (currentItem != nullptr && item == currentItem) {
                _itemIcon.replace(".svg", "_checked.svg");
            }
            item->setIcon(QIcon(_itemIcon));
            item->setData(_itemIcon, ICON_DATA);
        }
    }

    // set itembackground color
    DPalette pa = DApplicationHelper::instance()->palette(this);
    pa.setBrush(DPalette::ItemBackground, pa.color(DPalette::Base));
    pa.setBrush(DPalette::Background, pa.color(DPalette::Base));
    this->setPalette(pa);
    DApplicationHelper::instance()->setPalette(this, pa);

    this->setAutoFillBackground(true);
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

    emit itemChanged();
    DListView::currentChanged(current, previous);
}
