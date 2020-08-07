#include "logcombox.h"
#include <QStylePainter>
#include <QDebug>
#include <QKeyEvent>

#include <DStyle>
#include <DApplication>
LogCombox::LogCombox(QWidget *parent):
    DComboBox(parent)
{

}

void LogCombox::paintEvent(QPaintEvent *e)
{
    DComboBox::paintEvent(e);
    if (hasFocus() && (m_reson & Qt::TabFocusReason)) {
        DStylePainter painter(this);
//    painter.setPen(palette().color(QPalette::Text));
        DStyle *style = dynamic_cast<DStyle *>(DApplication::style());
//    style->drawControl()
//    // draw the combobox frame, focusrect and selected etc.
        QStyleOptionComboBox opt;
        initStyleOption(&opt);
        const  QStyleOptionComboBox *opt1 = &opt;
//    painter.draw(QStyle::SE_ComboBoxFocusRect, opt);

//    // draw the icon and text
//    painter.drawControl(QStyle::CE_ComboBoxLabel, opt);
        DStyleHelper dstyle(style);
        int border_width = dstyle.pixelMetric(DStyle::PM_FocusBorderWidth, opt1, this);
//        int border_space = style->pixelMetric(DStyle::PM_FocusFrameVMargin, opt1, this);
//        int frame_radius = dstyle.pixelMetric(DStyle::PM_FrameRadius, opt1, this);
        QColor color = dstyle.getColor(opt1, QPalette::Highlight);
        // QRect borderRect = style->subElementRect(DStyle::SE_ComboBoxFocusRect, opt1, this);

        painter.setPen(QPen(color, border_width, Qt::SolidLine));
        painter.setBrush(Qt::NoBrush);
        painter.setRenderHint(QPainter::Antialiasing);

        //.adjusted(1, 1, -1, -1)

        //painter.drawRoundedRect(borderRect, frame_radius, frame_radius);
        style->drawPrimitive(DStyle::PE_FrameFocusRect, opt1, & painter, this);
    }
}

void LogCombox::keyPressEvent(QKeyEvent *event)
{
    if ((event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return)) {
        showPopup();
    }
    DComboBox::keyPressEvent(event);
}

void LogCombox::focusInEvent(QFocusEvent *event)
{
    m_reson = event->reason();
    DComboBox::focusInEvent(event);
}

void LogCombox::focusOutEvent(QFocusEvent *event)
{
    DComboBox::focusOutEvent(event);
}
