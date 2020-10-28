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
#include <QScroller>
#include <private/qflickgesture_p.h>
#include <QEasingCurve>
#include <private/qguiapplication_p.h>
#include <qpa/qplatformtheme.h>
#include <QTouchEvent>
#include <QPainterPath>
DWIDGET_USE_NAMESPACE

LogTreeView::LogTreeView(QWidget *parent)
    : DTreeView(parent)
{
    initUI();
//    QScroller *sc = QScroller::scroller(this);
//    QScroller::ScrollerGestureType gesture = QScroller::TouchGesture;
//    QScrollerProperties porpy = sc->scrollerProperties();
//    porpy.setScrollMetric(QScrollerProperties::ScrollingCurve, QEasingCurve::OutQuart);
//    porpy.setScrollMetric(QScrollerProperties::VerticalOvershootPolicy, QScrollerProperties::OvershootAlwaysOff);
//    porpy.setScrollMetric(QScrollerProperties::HorizontalOvershootPolicy, QScrollerProperties::OvershootAlwaysOff);
//    porpy.setScrollMetric(QScrollerProperties::DecelerationFactor, 1);
//    sc->setScrollerProperties(porpy);
//    sc->grabGesture(this->viewport(), gesture);
    connect(this->verticalScrollBar(), &QScrollBar::sliderPressed, this, [ = ] {
        if (QScroller::hasScroller(this))
        {
            // 不可使用 ungrab，会导致应用崩溃，或许是Qt的bug
            QScroller::scroller(this)->stop();
        }
    });
    this->setAttribute(Qt::WA_AcceptTouchEvents);
    //QScroller::grabGesture(this->viewport(), QScroller::TouchGesture);
    touchTapDistance = QGuiApplicationPrivate::platformTheme()->themeHint(QPlatformTheme::TouchDoubleTapDistance).toInt();
}

int LogTreeView::singleRowHeight()
{
    if (this->model() == nullptr) {
        return -1;
    }
    QModelIndex firstIndex = this->model()->index(0, 0);
    if (firstIndex.isValid()) {
        return  this->rowHeight(firstIndex);
    } else {
        return -1;
    }

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
    this->setVerticalScrollMode(QAbstractItemView::ScrollMode::ScrollPerPixel);

    setSelectionMode(QAbstractItemView::SingleSelection);
    setSelectionBehavior(QAbstractItemView::SelectRows);

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
    // draw focus
    if (hasFocus() && currentIndex().row() == index.row() && (m_reson == Qt::TabFocusReason || m_reson == Qt::BacktabFocusReason)) {
        QStyleOptionFocusRect o;
        o.QStyleOption::operator=(options);
        o.state |= QStyle::State_KeyboardFocusChange | QStyle::State_HasFocus;
        o.rect = style->visualRect(layoutDirection(), viewport()->rect(), rowRect);
        style->drawPrimitive(DStyle::PE_FrameFocusRect, &o, painter);
    }

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

bool LogTreeView::event(QEvent *e)
{
    //qDebug() << "11111" << e->type();
    switch (e->type()) {

#if 1
    case QEvent::TouchBegin: {

        QTouchEvent *touchEvent = static_cast<QTouchEvent *>(e);

        if (!m_isPressed && touchEvent && touchEvent->device() && touchEvent->device()->type() == QTouchDevice::TouchScreen && touchEvent->touchPointStates() == Qt::TouchPointPressed) {

            QList<QTouchEvent::TouchPoint> points = touchEvent->touchPoints();
            //dell触摸屏幕只有一个touchpoint 但却能捕获到pinchevent缩放手势?
            //  qDebug() << "11111" << points.count();
            if (points.count() == 1) {
                QTouchEvent::TouchPoint p = points.at(0);
                m_lastTouchBeginPos =  p.pos();
                m_lastTouchTime = QTime::currentTime();
                m_isPressed = true;

            }
        }
        break;

    }
#endif
//    case QEvent::TouchUpdate: {
//        qDebug() << "22222";
//        break;
//    }
//    case QEvent::TouchEnd: {
//        qDebug() << "33333";
//        m_isPressed = false;
//        break;
//    }
//    case QEvent::TouchCancel: {
//        qDebug() << "44444444";
//        m_isPressed = false;
//        break;
//    }
//    case QEvent::ScrollPrepare: {
//        QScrollPrepareEvent *touchEvent = static_cast<QScrollPrepareEvent *>(e);
//        qDebug() << "QEvent::ScrollPrepare" << touchEvent->startPos() << touchEvent->contentPos();
//        break;
//    }
//    case QEvent::Scroll: {
//        QScrollEvent *touchEvent = static_cast<QScrollEvent *>(e);
//        qDebug() << "QEvent::Scroll" << touchEvent->contentPos() << touchEvent->scrollState();
//        break;
//    }

    }
    return  DTreeView::event(e);
}

void LogTreeView::mousePressEvent(QMouseEvent *event)
{
#if 0
    qDebug() << "mousePressEvent";
    return DTreeView::mousePressEvent(event);
    QScrollBar *bar = this->verticalScrollBar();
    switch (event->button()) {
    case Qt::LeftButton: {
        // 当事件source为MouseEventSynthesizedByQt，认为此事件为TouchBegin转换而来
        if (event->source() == Qt::MouseEventSynthesizedByQt) {

            lastTouchBeginPos = event->pos();

            // 清空触屏滚动操作，因为在鼠标按下时还不知道即将进行的是触屏滚动还是文件框选
            if (QScroller::hasScroller(this)) {
                // 不可使用 ungrab，会导致应用崩溃，或许是Qt的bug
                QScroller::scroller(this)->deleteLater();
            }

        }
        if (dragDropMode() != NoDragDrop) {
            setDragDropMode(DragDrop);
        }
        const QModelIndex &index = indexAt(event->pos());
        // 避免通过触屏拖动视图时当前选中被清除
        if (event->source() != Qt::MouseEventSynthesizedByQt) {
            clearSelection();
            update();
        }
        mouseLastPressedIndex = QModelIndex();
        DTreeView::mousePressEvent(event);
        break;
    }
    default:
        break;
    }
#endif

    DTreeView::mousePressEvent(event);
}

void LogTreeView::mouseMoveEvent(QMouseEvent *event)
{




#if 0
    qDebug() << "mouseMoveEvent";
    return DTreeView::mouseMoveEvent(event);
    // source为此类型时认为是触屏事件
    if (event->source() == Qt::MouseEventSynthesizedByQt) {
        if (QScroller::hasScroller(this))
            return;


        const QPoint difference_pos = event->pos() - lastTouchBeginPos;
        QScrollBar *bar = this->verticalScrollBar();
////如果当前触摸点在滚动条上则不处理，因为要用滚动条
//        if (bar->rect().contains(event->pos())) {
//            qDebug() << "bar";

//            return DTreeView::mouseMoveEvent(event);
//        }
        if (qAbs(difference_pos.x()) > touchTapDistance
                || qAbs(difference_pos.y()) > touchTapDistance) {
            QScroller::grabGesture(this->viewport(), QScroller::TouchGesture);
            QScroller *scroller = QScroller::scroller(this);
            QScrollerProperties  properties = scroller->scrollerProperties();
            QVariant overshootPolicy = QVariant::fromValue<QScrollerProperties::OvershootPolicy>(QScrollerProperties::OvershootAlwaysOff);
            properties.setScrollMetric(QScrollerProperties::VerticalOvershootPolicy, overshootPolicy);
            scroller->setScrollerProperties(properties);
            scroller->handleInput(QScroller::InputPress, event->localPos(), static_cast<qint64>(event->timestamp()));
            scroller->handleInput(QScroller::InputMove, event->localPos(), static_cast<qint64>(event->timestamp()));
        }

        return;

    }
#endif

    if (m_isPressed) {


//      /  qDebug() <<  m_lastTouchTime.msecsTo(QTime::currentTime());
//        if (m_lastTouchTime.msecsTo(QTime::currentTime()) < 100) {
//            return ;
//        }
        //最小距离为防误触和双向滑动时,只触发横向或者纵向的
        int touchmindistance = 2;
        //最大步进距离是因为原地点按马上放开,则会出现-35~-38的不合理位移,加上每次步进距离没有那么大,所以设置为30
        int touchMaxDistance = 30;
        event->accept();
        double horiDelta = event->pos().x() - m_lastTouchBeginPos.x();
        double vertDelta = event->pos().y() - m_lastTouchBeginPos.y();
        //  qDebug()  << "horiDelta" << horiDelta << "vertDelta" << vertDelta << "event->pos()" << event->pos() << "m_lastTouchBeginPos" << m_lastTouchBeginPos;
        if (qAbs(horiDelta) > touchmindistance && qAbs(horiDelta) < touchMaxDistance) {
            //    qDebug()  << "horizontalScrollBar()->value()" << horizontalScrollBar()->value();
            horizontalScrollBar()->setValue(static_cast<int>(horizontalScrollBar()->value() - horiDelta)) ;
        }

        if (qAbs(vertDelta) > touchmindistance && !(qAbs(vertDelta) < header()->height() + 2 && qAbs(vertDelta) > header()->height() - 2 && m_lastTouchTime.msecsTo(QTime::currentTime()) < 100)) {
            //       qDebug()  << "verticalScrollBar()->value()" << verticalScrollBar()->value() << "vertDelta" << vertDelta;
            double svalue = 1;
            if (vertDelta > 0) {
                //svalue = svalue;
            } else if (vertDelta < 0) {
                svalue = -svalue;
            } else {
                svalue = 0;
            }
            verticalScrollBar()->setValue(static_cast<int>(verticalScrollBar()->value() - vertDelta));
        }
        m_lastTouchBeginPos = event->pos();
        return;
    }
    return DTreeView::mouseMoveEvent(event);
}

void LogTreeView::mouseReleaseEvent(QMouseEvent *event)
{
#if 0
    qDebug() << "mouseReleaseEvent";
    return DTreeView::mouseReleaseEvent(event);
//    if (mouseLastPressedIndex.isValid()) {
//        if (d->mouseLastPressedIndex == indexAt(event->pos()))
//            selectionModel()->select(d->mouseLastPressedIndex, QItemSelectionModel::Deselect);
//    }

    // 避免滚动视图导致文件选中状态被取消
    if (!QScroller::hasScroller(this))
        return DTreeView::mouseReleaseEvent(event);
#endif
    if (m_isPressed) {
        //  qDebug() << "mouseReleaseEvent";
        m_isPressed = false;
        return;
    }
    return DTreeView::mouseReleaseEvent(event);
}

void LogTreeView::focusInEvent(QFocusEvent *event)
{
    m_reson = event->reason();
    DTreeView::focusInEvent(event);
}



