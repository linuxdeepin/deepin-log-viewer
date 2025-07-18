// SPDX-FileCopyrightText: 2019 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "logtreeview.h"
#include "structdef.h"
#include "logviewheaderview.h"
#include "logviewitemdelegate.h"

#include <DApplication>
#include <DGuiApplicationHelper>
#include <DPaletteHelper>
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
//#include <private/qflickgesture_p.h>
#include <QEasingCurve>
//#include <private/qguiapplication_p.h>
//#include <qpa/qplatformtheme.h>
#include <QTouchEvent>
#include <QPainterPath>
#include <QLoggingCategory>

DWIDGET_USE_NAMESPACE

Q_DECLARE_LOGGING_CATEGORY(logApp)

LogTreeView::LogTreeView(QWidget *parent)
    : DTreeView(parent)
{
    qInfo() << "LogTreeView constructor initialized";
    initUI();
    connect(this->verticalScrollBar(), &QScrollBar::sliderPressed, this, [ = ] {
        if (QScroller::hasScroller(this))
        {
            // 不可使用 ungrab，会导致应用崩溃，或许是Qt的bug
            QScroller::scroller(this)->stop();
        }
    });
    this->setAttribute(Qt::WA_AcceptTouchEvents);
//    touchTapDistance = QGuiApplicationPrivate::platformTheme()->themeHint(QPlatformTheme::TouchDoubleTapDistance).toInt();
}

int LogTreeView::singleRowHeight()
{
    qCDebug(logApp) << "Getting single row height";
    if (this->model() == nullptr) {
        qCWarning(logApp) << "Model is null, returning -1";
        return -1;
    }
    QModelIndex firstIndex = this->model()->index(0, 0);
    if (firstIndex.isValid()) {
        int height = this->rowHeight(firstIndex);
        qCDebug(logApp) << "Single row height:" << height;
        return height;
    } else {
        qCWarning(logApp) << "First index is invalid, returning -1";
        return -1;
    }

}

/**
 * @brief LogTreeView::initU  进行一些属性的定制，初始化treeview为需要的样式
 */
void LogTreeView::initUI()
{
    qInfo() << "Initializing LogTreeView UI components";
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
    // qCDebug(logApp) << "LogTreeView paintEvent";
    QPainter painter(viewport());
    painter.save();
    painter.setRenderHints(QPainter::Antialiasing);
    painter.setOpacity(1);
    painter.setClipping(true);
    //根据窗口激活状态设置颜色
    QWidget *wnd = DApplication::activeWindow();
    DPalette::ColorGroup cg;
    if (!wnd) {
        // qCDebug(logApp) << "Window is not active";
        cg = DPalette::Inactive;
    } else {
        // qCDebug(logApp) << "Window is active";
        cg = DPalette::Active;
    }
    auto *dAppHelper = DGuiApplicationHelper::instance();
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
    // qCDebug(logApp) << "LogTreeView drawRow";
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing);

#ifdef ENABLE_INACTIVE_DISPLAY
    QWidget *wnd = DApplication::activeWindow();
#endif
    DPalette::ColorGroup cg;
    if (!(options.state & DStyle::State_Enabled)) {
        // qCDebug(logApp) << "State is disabled";
        cg = DPalette::Disabled;
    } else {
#ifdef ENABLE_INACTIVE_DISPLAY
        if (!wnd) {
            // qCDebug(logApp) << "Window is not active";
            cg = DPalette::Inactive;
        } else {
            if (wnd->isModal()) {
                // qCDebug(logApp) << "Window is modal";
                cg = DPalette::Inactive;
            } else {
                // qCDebug(logApp) << "Window is active";
                cg = DPalette::Active;
            }
        }
#else
        // qCDebug(logApp) << "Window is active";
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
        // qCDebug(logApp) << "Row is even";
        auto dpa = DPaletteHelper::instance()->palette(this);
        background = dpa.color(DPalette::ItemBackground);
    } else {
        // qCDebug(logApp) << "Row is odd";
        background = palette.color(cg, DPalette::Base);
    }
    if (options.state & DStyle::State_Enabled) {
        if (selectionModel()->isSelected(index)) {
            // qCDebug(logApp) << "Row is selected";
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
        // qCDebug(logApp) << "Drawing focus rect";
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
    // qCDebug(logApp) << "LogTreeView key press event, key:" << event->key();
    DTreeView::keyPressEvent(event);
    if (event->key() == Qt::Key_Up || event->key() == Qt::Key_Down) {
        // qCDebug(logApp) << "Arrow key pressed, emitting pressed signal";
        emit pressed(this->currentIndex());
    }
}


bool LogTreeView::event(QEvent *e)
{
    // qCDebug(logApp) << "LogTreeView event";
    if (e->type() == QEvent::TouchBegin) {
        // qCDebug(logApp) << "Touch begin event detected";
        QTouchEvent *touchEvent = static_cast<QTouchEvent *>(e);
        if (!m_isPressed && touchEvent) {
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
            if (touchEvent->device() && touchEvent->device()->type() == QTouchDevice::TouchScreen) {
                QList<QTouchEvent::TouchPoint> points = touchEvent->touchPoints();
            
#else
            if (touchEvent->deviceType() == QInputDevice::DeviceType::TouchScreen) {
                QList<QTouchEvent::TouchPoint> points = touchEvent->points();
#endif
                // qCDebug(logApp) << "Touch screen detected with" << points.count() << "touch points";
                //dell触摸屏幕只有一个touchpoint 但却能捕获到pinchevent缩放手势?
                if (points.count() == 1) {
                    QTouchEvent::TouchPoint p = points.at(0);
                    #if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
                    m_lastTouchBeginPos = p.pos();
                    #else
                    m_lastTouchBeginPos = p.position();
                    #endif
                    m_lastTouchTime = QTime::currentTime();
                    m_isPressed = true;
                    // qCDebug(logApp) << "Single touch point registered, position:" << m_lastTouchBeginPos;
                }
            }
        }
    }
    return DTreeView::event(e);
}

void LogTreeView::mouseMoveEvent(QMouseEvent *event)
{
    // qCDebug(logApp) << "LogTreeView mouse move event";
    if (m_isPressed) {
        // qCDebug(logApp) << "Touch move event while pressed";
        //最小距离为防误触和双向滑动时,只触发横向或者纵向的
        int touchmindistance = 2;
        //最大步进距离是因为原地点按马上放开,则会出现-35~-38的不合理位移,加上每次步进距离没有那么大,所以设置为30
        int touchMaxDistance = 30;
        event->accept();
        double horiDelta = event->pos().x() - m_lastTouchBeginPos.x();
        double vertDelta = event->pos().y() - m_lastTouchBeginPos.y();
        if (qAbs(horiDelta) > touchmindistance && qAbs(horiDelta) < touchMaxDistance) {
            // qCDebug(logApp) << "Horizontal scroll delta:" << horiDelta;
            horizontalScrollBar()->setValue(static_cast<int>(horizontalScrollBar()->value() - horiDelta)) ;
        }

        if (qAbs(vertDelta) > touchmindistance && !(qAbs(vertDelta) < header()->height() + 2 && qAbs(vertDelta) > header()->height() - 2 && m_lastTouchTime.msecsTo(QTime::currentTime()) < 100)) {
            // qCDebug(logApp) << "Vertical scroll delta:" << vertDelta;
            verticalScrollBar()->setValue(static_cast<int>(verticalScrollBar()->value() - vertDelta));
        }
        m_lastTouchBeginPos = event->pos();
        return;
    }
    return DTreeView::mouseMoveEvent(event);
}

void LogTreeView::mouseReleaseEvent(QMouseEvent *event)
{
    // qCDebug(logApp) << "LogTreeView mouse release event";
    if (m_isPressed) {
        // qCDebug(logApp) << "Touch released";
        m_isPressed = false;
        return;
    }
    return DTreeView::mouseReleaseEvent(event);
}

void LogTreeView::focusInEvent(QFocusEvent *event)
{
    // qCDebug(logApp) << "LogTreeView focus in event, reason:" << event->reason();
    m_reson = event->reason();
    DTreeView::focusInEvent(event);
}
