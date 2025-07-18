// SPDX-FileCopyrightText: 2019 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "logcombox.h"

#include <QStylePainter>
#include <QDebug>
#include <QKeyEvent>
#include <QLoggingCategory>

#include <DStyle>
#include <DApplication>

Q_DECLARE_LOGGING_CATEGORY(logApp)


LogCombox::LogCombox(QWidget *parent):
    DComboBox(parent)
{
    qCDebug(logApp) << "LogCombox constructor called";
    setFocusPolicy(Qt::TabFocus);
}

void LogCombox::setFocusReason(Qt::FocusReason iReson)
{
    qCDebug(logApp) << "LogCombox::setFocusReason called with reason:" << iReson;
    m_reson = iReson;
}

Qt::FocusReason LogCombox::getFocusReason()
{
    qCDebug(logApp) << "LogCombox::getFocusReason called, returning:" << m_reson;
    return m_reson;
}
/**
 * @brief LogCombox::paintEvent
 * 绘制焦点边框,屏蔽默认绘制事件,只在tabfoucus时绘制边框
 * @param e
 */
void LogCombox::paintEvent(QPaintEvent *e)
{
    qCDebug(logApp) << "LogCombox::paintEvent called";
    DComboBox::paintEvent(e);
//    if (hasFocus() && (m_reson == Qt::TabFocusReason || m_reson == Qt::BacktabFocusReason)) {
//        DStylePainter painter(this);
////    painter.setPen(palette().color(QPalette::Text));
//        DStyle *style = dynamic_cast<DStyle *>(DApplication::style());
////    style->drawControl()
////    // draw the combobox frame, focusrect and selected etc.
//        QStyleOptionComboBox opt;
//        initStyleOption(&opt);
//        const  QStyleOptionComboBox *opt1 = &opt;
////    painter.draw(QStyle::SE_ComboBoxFocusRect, opt);

////    // draw the icon and text
////    painter.drawControl(QStyle::CE_ComboBoxLabel, opt);
//        DStyleHelper dstyle(style);
//        int border_width = dstyle.pixelMetric(DStyle::PM_FocusBorderWidth, opt1, this);
////        int border_space = style->pixelMetric(DStyle::PM_FocusFrameVMargin, opt1, this);
////        int frame_radius = dstyle.pixelMetric(DStyle::PM_FrameRadius, opt1, this);
//        QColor color = dstyle.getColor(opt1, QPalette::Highlight);
//        // QRect borderRect = style->subElementRect(DStyle::SE_ComboBoxFocusRect, opt1, this);

//        painter.setPen(QPen(color, border_width, Qt::SolidLine));
//        painter.setBrush(Qt::NoBrush);
//        painter.setRenderHint(QPainter::Antialiasing);

//        //.adjusted(1, 1, -1, -1)

//        //painter.drawRoundedRect(borderRect, frame_radius, frame_radius);
//        qCDebug(logApp) << "tabcombox-------";
//        style->drawPrimitive(DStyle::PE_FrameFocusRect, opt1, & painter, this);
//    }
}
/**
 * @brief LogCombox::keyPressEvent
 * 增加回车触发按钮功能,捕获回车键盘事件发送空格键盘事件
 * @param event
 */
void LogCombox::keyPressEvent(QKeyEvent *event)
{
    // qCDebug(logApp) << "LogCombox::keyPressEvent called with key:" << event->key();
    if ((event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return)) {
        qCDebug(logApp) << "Enter/Return key pressed, showing popup";
        showPopup();
    }
    DComboBox::keyPressEvent(event);
}
/**
 * @brief LogCombox::focusInEvent
 * 捕获最近一次获得焦点的reason以区分是否为tabfoucs,以供绘制焦点效果时判断
 * @param event
 */
void LogCombox::focusInEvent(QFocusEvent *event)
{
    // qCDebug(logApp) << "LogCombox::focusInEvent called with reason:" << event->reason();
    if (event->reason() != Qt::PopupFocusReason) {
        m_reson = event->reason();
        qCDebug(logApp) << "Focus reason updated to:" << m_reson;
    }
    DComboBox::focusInEvent(event);
}

void LogCombox::focusOutEvent(QFocusEvent *event)
{
    // qCDebug(logApp) << "LogCombox::focusOutEvent called with reason:" << event->reason();
    DComboBox::focusOutEvent(event);
}
