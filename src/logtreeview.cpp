#include "logtreeview.h"
#include <DApplication>
#include <DApplicationHelper>
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
    m_itemDelegate = new LogViewItemDelegate(this);
    setItemDelegate(m_itemDelegate);

    m_headerDelegate = new LogViewHeaderView(Qt::Horizontal, this);
    //    this->setHorizontalHeader(m_headerDelegate);
    setHeader(m_headerDelegate);
    //    this->verticalHeader()->hide();

    this->setEditTriggers(QAbstractItemView::NoEditTriggers);
    this->setRootIsDecorated(false);

    // this is setting is necessary,because scrollperpixel is default in dtk!!
    this->setVerticalScrollMode(QAbstractItemView::ScrollMode::ScrollPerItem);

    setSelectionMode(QAbstractItemView::SingleSelection);
    setSelectionBehavior(QAbstractItemView::SelectRows);
    this->header()->setStretchLastSection(true);
    setRootIsDecorated(false);
    setItemsExpandable(false);
    setFrameStyle(QFrame::NoFrame);
    this->viewport()->setAutoFillBackground(false);
}

void LogTreeView::paintEvent(QPaintEvent *event)
{
    QPainter painter(viewport());
    painter.save();
    painter.setRenderHints(QPainter::Antialiasing);
    painter.setOpacity(1);
    painter.setClipping(true);

    QWidget *wnd = DApplication::activeWindow();
    DPalette::ColorGroup cg;
    if (!wnd) {
        cg = DPalette::Inactive;
    } else {
        cg = DPalette::Active;
    }

    auto style = dynamic_cast<DStyle *>(DApplication::style());
    auto *dAppHelper = DApplicationHelper::instance();
    auto palette = dAppHelper->applicationPalette();

    QBrush bgBrush(palette.color(cg, DPalette::Base));

    QStyleOptionFrame option;
    initStyleOption(&option);
    int radius = style->pixelMetric(DStyle::PM_FrameRadius, &option);

    QRect rect = viewport()->rect();
    QRectF clipRect(rect.x(), rect.y() - rect.height(), rect.width(), rect.height() * 2);
    QRectF subRect(rect.x(), rect.y() - rect.height(), rect.width(), rect.height());
    QPainterPath clipPath, subPath;
    clipPath.addRoundedRect(clipRect, radius, radius);
    subPath.addRect(subRect);
    clipPath = clipPath.subtracted(subPath);

    painter.fillPath(clipPath, bgBrush);

    painter.restore();
    DTreeView::paintEvent(event);
}
