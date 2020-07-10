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

#include "logtreeview.h"
#include "structdef.h"
#include "logviewheaderview.h"
#include "logviewitemdelegate.h"

#include <DApplication>
#include <DApplicationHelper>
#include <DStyledItemDelegate>
#include <DStyle>

#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QHeaderView>
#include <QScrollBar>
#include <QKeyEvent>

DWIDGET_USE_NAMESPACE

LogTreeView::LogTreeView(QWidget *parent)
    : DTreeView(parent)
{
    initUI();
}

/**
 * @brief LogTreeView::initU  进行一些属性的定制，初始化treeview为需要的样式
 */
void LogTreeView::initUI()
{
    m_itemDelegate = new LogViewItemDelegate(this);
    setItemDelegate(m_itemDelegate);

    m_headerDelegate = new LogViewHeaderView(Qt::Horizontal, this);
    setHeader(m_headerDelegate);

    this->setEditTriggers(QAbstractItemView::NoEditTriggers);
    this->setRootIsDecorated(false);

    //这个设置默认是值是ScrollPerPixel，和我们想要的Mode不一样，我们要通过纵向滚动实现分页的
    this->setVerticalScrollMode(QAbstractItemView::ScrollMode::ScrollPerItem);

    setSelectionMode(QAbstractItemView::SingleSelection);
    setSelectionBehavior(QAbstractItemView::SelectRows);
    this->header()->setStretchLastSection(true);

    this->header()->setDefaultAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    this->header()->setFixedHeight(37);
    setRootIsDecorated(false);
    setItemsExpandable(false);
    setFrameStyle(QFrame::NoFrame);
    this->viewport()->setAutoFillBackground(false);
    //不需要间隔颜色的样式，因为自绘了，它默认的效果和我们想要的不一样
    setAlternatingRowColors(false);
    setAllColumnsShowFocus(false);
}

/**
 * @brief LogTreeView::paintEvent  绘制背景
 * @param event
 */
void LogTreeView::paintEvent(QPaintEvent *event)
{
    QPainter painter(viewport());
    painter.save();
    painter.setRenderHints(QPainter::Antialiasing);
    painter.setOpacity(1);
    painter.setClipping(true);
    //根据窗口激活状态设置颜色
    QWidget *wnd = DApplication::activeWindow();
    DPalette::ColorGroup cg;
    if (!wnd) {
        cg = DPalette::Inactive;
    } else {
        cg = DPalette::Active;
    }
    auto *dAppHelper = DApplicationHelper::instance();
    auto palette = dAppHelper->applicationPalette();
    QBrush bgBrush(palette.color(cg, DPalette::Base));
    //绘制背景
    QStyleOptionFrame option;
    initStyleOption(&option);
    QRect rect = viewport()->rect();
    QRectF clipRect(rect.x(), rect.y() - rect.height(), rect.width(), rect.height() * 2);
    QRectF subRect(rect.x(), rect.y() - rect.height(), rect.width(), rect.height());
    QPainterPath clipPath, subPath;
    clipPath.addRect(rect);
    painter.fillPath(clipPath, bgBrush);
    painter.restore();
    DTreeView::paintEvent(event);
}

/**
 * @brief LogTreeView::drawRow 使用drawrow虚函数绘制灰白交替的行背景
 * @param painter
 * @param options
 * @param index
 */
void LogTreeView::drawRow(QPainter *painter, const QStyleOptionViewItem &options, const QModelIndex &index) const
{
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing);

#ifdef ENABLE_INACTIVE_DISPLAY
    QWidget *wnd = DApplication::activeWindow();
#endif
    DPalette::ColorGroup cg;
    if (!(options.state & DStyle::State_Enabled)) {
        cg = DPalette::Disabled;
    } else {
#ifdef ENABLE_INACTIVE_DISPLAY
        if (!wnd) {
            cg = DPalette::Inactive;
        } else {
            if (wnd->isModal()) {
                cg = DPalette::Inactive;
            } else {
                cg = DPalette::Active;
            }
        }
#else
        cg = DPalette::Active;
#endif
    }

    auto *style = dynamic_cast<DStyle *>(DApplication::style());

    auto radius = style->pixelMetric(DStyle::PM_FrameRadius, &options);
    auto margin = style->pixelMetric(DStyle::PM_ContentsMargins, &options);
    //根据实际情况设置颜色，奇数行为灰色
    auto palette = options.palette;
    QBrush background;
    if (!(index.row() & 1)) {
        background = palette.color(cg, DPalette::AlternateBase);
    } else {
        background = palette.color(cg, DPalette::Base);
    }
    if (options.state & DStyle::State_Enabled) {
        if (selectionModel()->isSelected(index)) {
            background = palette.color(cg, DPalette::Highlight);
        }
    }

    // 绘制整行背景，高度-2以让高分屏非整数缩放比例下无被选中的蓝色细线，防止原来通过delegate绘制单元格交替颜色背景出现的高分屏非整数缩放比例下qrect精度问题导致的横向单元格间出现白色边框
    QPainterPath path;
    QRect rowRect { options.rect.x() - header()->offset(),
                    options.rect.y() + 1,
                    header()->length() - header()->sectionPosition(0),
                    options.rect.height() - 2 };
    rowRect.setX(rowRect.x() + margin);
    rowRect.setWidth(rowRect.width() - margin);

    path.addRoundedRect(rowRect, radius, radius);
    painter->fillPath(path, background);

    QTreeView::drawRow(painter, options, index);

    painter->restore();
}

/**
 * @brief LogTreeView::keyPressEvent 快捷键上下移动切换表格选中行
 * @param event
 */
void LogTreeView::keyPressEvent(QKeyEvent *event)
{
    DTreeView::keyPressEvent(event);
    if (event->key() == Qt::Key_Up || event->key() == Qt::Key_Down) {
        emit clicked(this->currentIndex());
    }
}
