// SPDX-FileCopyrightText: 2019 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "logviewitemdelegate.h"
#include "logtreeview.h"

#include <gtest/gtest.h>
#include <stub.h>

#include <DStyle>
#include <DApplication>
#include <DGuiApplicationHelper>

#include <QDebug>
#include <QPainter>
#include <QStandardItemModel>
#include <QImage>

DWIDGET_USE_NAMESPACE

int stub_pixelMetric001(DStyle::PixelMetric m, const QStyleOption *opt = nullptr, const QWidget *widget = nullptr)
{
    return 1;
}

void stub_drawPrimitive001(DStyle::PrimitiveElement pe, const QStyleOption *opt, QPainter *p, const QWidget *w = nullptr)
{
}

class LogViewItemDelegate_UT : public testing::Test
{
public:
    static void SetUpTestCase()
    {
        qDebug() << "SetUpTestCase";
    }
    static void TearDownTestCase()
    {
        qDebug() << "TearDownTestCase";
    }
    void SetUp()
    {
        QApplication::setStyle(new DStyle);
        m_treeview = new LogTreeView();
        m_delegate = new LogViewItemDelegate(m_treeview);
    }
    void TearDown()
    {
        delete m_delegate;
        delete m_treeview;
    }
    LogTreeView *m_treeview;
    LogViewItemDelegate *m_delegate;
};

TEST_F(LogViewItemDelegate_UT, Constructor)
{
    EXPECT_NE(m_delegate, nullptr);
    LogViewItemDelegate *delegate2 = new LogViewItemDelegate(nullptr);
    EXPECT_NE(delegate2, nullptr);
    delete delegate2;
}

TEST_F(LogViewItemDelegate_UT, InitStyleOption_WithTextAlignmentRole)
{
    QStandardItemModel model;
    QStandardItem *item = new QStandardItem("test data");
    model.appendRow(item);
    QModelIndex index = model.index(0, 0);

    QStyleOptionViewItem opt;
    m_delegate->initStyleOption(&opt, index);
    EXPECT_TRUE(opt.showDecorationSelected);
    EXPECT_EQ(opt.textElideMode, Qt::ElideRight);
    EXPECT_TRUE(opt.features & QStyleOptionViewItem::HasDisplay);
    EXPECT_EQ(opt.text, QString("test data"));
}

TEST_F(LogViewItemDelegate_UT, InitStyleOption_WithoutTextAlignmentRole)
{
    QStandardItemModel model;
    QStandardItem *item = new QStandardItem("test");
    item->setData(QVariant(), Qt::TextAlignmentRole);
    model.appendRow(item);
    QModelIndex index = model.index(0, 0);

    QStyleOptionViewItem opt;
    m_delegate->initStyleOption(&opt, index);
    EXPECT_EQ(opt.displayAlignment, Qt::AlignLeft | Qt::AlignVCenter);
}

TEST_F(LogViewItemDelegate_UT, InitStyleOption_EvenRow_Alternate)
{
    QStandardItemModel model;
    QStandardItem *item = new QStandardItem("row0");
    model.appendRow(item);
    QModelIndex index = model.index(0, 0);

    QStyleOptionViewItem opt;
    m_delegate->initStyleOption(&opt, index);
    EXPECT_TRUE(opt.features & QStyleOptionViewItem::Alternate);
}

TEST_F(LogViewItemDelegate_UT, InitStyleOption_OddRow_NoAlternate)
{
    QStandardItemModel model;
    model.appendRow(new QStandardItem("row0"));
    model.appendRow(new QStandardItem("row1"));
    QModelIndex index = model.index(1, 0);

    QStyleOptionViewItem opt;
    m_delegate->initStyleOption(&opt, index);
    EXPECT_FALSE(opt.features & QStyleOptionViewItem::Alternate);
}

TEST_F(LogViewItemDelegate_UT, InitStyleOption_WithoutDisplayRole)
{
    QStandardItemModel model;
    QStandardItem *item = new QStandardItem();
    item->setData(QVariant(), Qt::DisplayRole);
    model.appendRow(item);
    QModelIndex index = model.index(0, 0);

    QStyleOptionViewItem opt;
    m_delegate->initStyleOption(&opt, index);
    EXPECT_TRUE(opt.text.isEmpty());
}

TEST_F(LogViewItemDelegate_UT, Paint_InvalidIndex)
{
    QImage image(100, 50, QImage::Format_ARGB32);
    QPainter painter(&image);
    QStyleOptionViewItem opt;
    QModelIndex invalidIndex;
    m_delegate->paint(&painter, opt, invalidIndex);
    SUCCEED();
}

TEST_F(LogViewItemDelegate_UT, Paint_Beginning)
{
    Stub stub;
    stub.set((int (DStyle::*)(DStyle::PixelMetric, const QStyleOption *, const QWidget *) const)ADDR(DStyle, pixelMetric), stub_pixelMetric001);

    QStandardItemModel model;
    QStandardItem *item = new QStandardItem("test");
    model.appendRow(item);
    QModelIndex index = model.index(0, 0);

    QImage image(200, 50, QImage::Format_ARGB32);
    QPainter painter(&image);
    QStyleOptionViewItem opt;
    opt.rect = QRect(0, 0, 200, 50);
    opt.viewItemPosition = QStyleOptionViewItem::Beginning;
    m_delegate->paint(&painter, opt, index);
    SUCCEED();
}

TEST_F(LogViewItemDelegate_UT, Paint_Beginning_WithIcon)
{
    Stub stub;
    stub.set((int (DStyle::*)(DStyle::PixelMetric, const QStyleOption *, const QWidget *) const)ADDR(DStyle, pixelMetric), stub_pixelMetric001);

    QStandardItemModel model;
    QStandardItem *item = new QStandardItem("test");
    item->setIcon(QIcon::fromTheme("document"));
    model.appendRow(item);
    QModelIndex index = model.index(0, 0);

    QImage image(200, 50, QImage::Format_ARGB32);
    QPainter painter(&image);
    QStyleOptionViewItem opt;
    opt.rect = QRect(0, 0, 200, 50);
    opt.viewItemPosition = QStyleOptionViewItem::Beginning;
    m_delegate->paint(&painter, opt, index);
    SUCCEED();
}

TEST_F(LogViewItemDelegate_UT, Paint_Middle)
{
    Stub stub;
    stub.set((int (DStyle::*)(DStyle::PixelMetric, const QStyleOption *, const QWidget *) const)ADDR(DStyle, pixelMetric), stub_pixelMetric001);

    QStandardItemModel model;
    QStandardItem *item = new QStandardItem("test");
    model.appendRow(item);
    QModelIndex index = model.index(0, 0);

    QImage image(200, 50, QImage::Format_ARGB32);
    QPainter painter(&image);
    QStyleOptionViewItem opt;
    opt.rect = QRect(0, 0, 200, 50);
    opt.viewItemPosition = QStyleOptionViewItem::Middle;
    m_delegate->paint(&painter, opt, index);
    SUCCEED();
}

TEST_F(LogViewItemDelegate_UT, Paint_End)
{
    Stub stub;
    stub.set((int (DStyle::*)(DStyle::PixelMetric, const QStyleOption *, const QWidget *) const)ADDR(DStyle, pixelMetric), stub_pixelMetric001);

    QStandardItemModel model;
    QStandardItem *item = new QStandardItem("test");
    model.appendRow(item);
    QModelIndex index = model.index(0, 0);

    QImage image(200, 50, QImage::Format_ARGB32);
    QPainter painter(&image);
    QStyleOptionViewItem opt;
    opt.rect = QRect(0, 0, 200, 50);
    opt.viewItemPosition = QStyleOptionViewItem::End;
    m_delegate->paint(&painter, opt, index);
    SUCCEED();
}

TEST_F(LogViewItemDelegate_UT, Paint_OnlyOne)
{
    Stub stub;
    stub.set((int (DStyle::*)(DStyle::PixelMetric, const QStyleOption *, const QWidget *) const)ADDR(DStyle, pixelMetric), stub_pixelMetric001);

    QStandardItemModel model;
    QStandardItem *item = new QStandardItem("test");
    model.appendRow(item);
    QModelIndex index = model.index(0, 0);

    QImage image(200, 50, QImage::Format_ARGB32);
    QPainter painter(&image);
    QStyleOptionViewItem opt;
    opt.rect = QRect(0, 0, 200, 50);
    opt.viewItemPosition = QStyleOptionViewItem::OnlyOne;
    m_delegate->paint(&painter, opt, index);
    SUCCEED();
}

TEST_F(LogViewItemDelegate_UT, Paint_DefaultPosition)
{
    Stub stub;
    stub.set((int (DStyle::*)(DStyle::PixelMetric, const QStyleOption *, const QWidget *) const)ADDR(DStyle, pixelMetric), stub_pixelMetric001);
    stub.set((void (DStyle::*)(DStyle::PrimitiveElement, const QStyleOption *, QPainter *, const QWidget *) const)ADDR(DStyle, drawPrimitive), stub_drawPrimitive001);

    QStandardItemModel model;
    QStandardItem *item = new QStandardItem("test");
    model.appendRow(item);
    QModelIndex index = model.index(0, 0);

    QImage image(200, 50, QImage::Format_ARGB32);
    QPainter painter(&image);
    QStyleOptionViewItem opt;
    opt.rect = QRect(0, 0, 200, 50);
    opt.viewItemPosition = QStyleOptionViewItem::Invalid;
    m_delegate->paint(&painter, opt, index);
    SUCCEED();
}

TEST_F(LogViewItemDelegate_UT, Paint_SelectedState)
{
    Stub stub;
    stub.set((int (DStyle::*)(DStyle::PixelMetric, const QStyleOption *, const QWidget *) const)ADDR(DStyle, pixelMetric), stub_pixelMetric001);

    QStandardItemModel model;
    QStandardItem *item = new QStandardItem("test");
    model.appendRow(item);
    QModelIndex index = model.index(0, 0);

    QImage image(200, 50, QImage::Format_ARGB32);
    QPainter painter(&image);
    QStyleOptionViewItem opt;
    opt.rect = QRect(0, 0, 200, 50);
    opt.viewItemPosition = QStyleOptionViewItem::OnlyOne;
    opt.state |= QStyle::State_Enabled;
    opt.state |= QStyle::State_Selected;
    m_delegate->paint(&painter, opt, index);
    SUCCEED();
}

TEST_F(LogViewItemDelegate_UT, Paint_DisabledState)
{
    Stub stub;
    stub.set((int (DStyle::*)(DStyle::PixelMetric, const QStyleOption *, const QWidget *) const)ADDR(DStyle, pixelMetric), stub_pixelMetric001);

    QStandardItemModel model;
    QStandardItem *item = new QStandardItem("test");
    model.appendRow(item);
    QModelIndex index = model.index(0, 0);

    QImage image(200, 50, QImage::Format_ARGB32);
    QPainter painter(&image);
    QStyleOptionViewItem opt;
    opt.rect = QRect(0, 0, 200, 50);
    opt.viewItemPosition = QStyleOptionViewItem::OnlyOne;
    opt.state = QStyle::State_None;
    m_delegate->paint(&painter, opt, index);
    SUCCEED();
}

TEST_F(LogViewItemDelegate_UT, SizeHint)
{
    QStandardItemModel model;
    QStandardItem *item = new QStandardItem("test");
    model.appendRow(item);
    QModelIndex index = model.index(0, 0);

    QStyleOptionViewItem opt;
    QSize size = m_delegate->sizeHint(opt, index);
    EXPECT_TRUE(size.height() > 0);
}

TEST_F(LogViewItemDelegate_UT, SizeHint_InvalidIndex)
{
    QStyleOptionViewItem opt;
    QModelIndex invalidIndex;
    QSize size = m_delegate->sizeHint(opt, invalidIndex);
    EXPECT_TRUE(size.height() > 0);
}
