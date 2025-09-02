// SPDX-FileCopyrightText: 2019 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "logperiodbutton.h"

#include <DGuiApplicationHelper>
#include <DPaletteHelper>
#include <DPalette>
#include <DStyle>
#include <DApplication>

#include <QBrush>
#include <QPaintEvent>
#include <QPainter>
#include <QPen>
#include <QDebug>
#include <QStylePainter>
#include <QPainterPath>
#include <QLoggingCategory>

DWIDGET_USE_NAMESPACE

Q_DECLARE_LOGGING_CATEGORY(logApp)

LogPeriodButton::LogPeriodButton(const QString &text, QWidget *parent)
    : DPushButton(text, parent)
{
    qCDebug(logApp) << "LogPeriodButton constructor called with text:" << text;
    this->setFocusPolicy(Qt::StrongFocus);
    QFont f = font();
    f.setPixelSize(16);
    setFlat(true);
    setCheckable(true);
    setContentsMargins(18, 18, contentsMargins().top(), contentsMargins().bottom());
}

void LogPeriodButton::setStandardSize(int iStahndardWidth)
{
    // qCDebug(logApp) << "Setting standard size to:" << iStahndardWidth;
    m_stahndardWidth = iStahndardWidth;
}

Qt::FocusReason LogPeriodButton::getFocusReason()
{
    // qCDebug(logApp) << "Getting focus reason:" << m_reson;
    return m_reson;
}

void LogPeriodButton::enterEvent(EnterEvent *e)
{
    // qCDebug(logApp) << "Mouse entered LogPeriodButton";
    isEnter = true;
    DPushButton::enterEvent(e);
}

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
void LogPeriodButton::leaveEvent(EnterEvent *e)
#else
void LogPeriodButton::leaveEvent(QEvent *e)
#endif
{
    // qCDebug(logApp) << "Mouse left LogPeriodButton";
    isEnter = false;
    DPushButton::leaveEvent(e);
}

/**
 * @brief LogPeriodButton::paintEvent  通过鼠标是否在按钮上的状态绘hover效果 绘制焦点边框,屏蔽默认绘制事件,只在tabfoucus时绘制边框
 * @param event
 */
void LogPeriodButton::paintEvent(QPaintEvent *event)
{
    // qCDebug(logApp) << "LogPeriodButton paintEvent called, isEnter:" << isEnter;
    QPainter painter(this);

    QRectF rect = this->rect();
    if (isEnter) {
        // qCDebug(logApp) << "Drawing hover effect for LogPeriodButton";
        this->setAutoFillBackground(true);
        this->setBackgroundRole(DPalette::Base);

        painter.setRenderHint(QPainter::Antialiasing);
        DPalette pa = DPaletteHelper::instance()->palette(this);
        painter.setBrush(QBrush(pa.color(DPalette::Light)));
        QColor penColor = pa.color(DPalette::Base);
        painter.setPen(QPen(penColor));

        QPainterPath painterPath;
        painterPath.addRoundedRect(rect, 8, 8);
        painter.drawPath(painterPath);
    } else {
        // qCDebug(logApp) << "No hover effect for LogPeriodButton";
        this->setAutoFillBackground(false);
    }
    DStyle *style = dynamic_cast<DStyle *>(DApplication::style());
    QStyleOptionButton btn;
    initStyleOption(&btn);
    QStylePainter painter2(this);
    style->proxy()->drawControl(DStyle::CE_PushButtonBevel, &btn, &painter2, this);
    QStyleOptionButton subopt = btn;
    subopt.rect = style->proxy()->subElementRect(DStyle::SE_PushButtonContents, &btn, this);
    style->proxy()->drawControl(DStyle::CE_PushButtonLabel, &subopt,  &painter2, this);
    if (hasFocus() && (m_reson == Qt::TabFocusReason || m_reson == Qt::BacktabFocusReason)) {
        // qCDebug(logApp) << "Drawing focus rect for LogPeriodButton with reason:" << m_reson;
        QStyleOptionFocusRect fropt;
        fropt.QStyleOption::operator=(btn);
        fropt.rect = style->proxy()->subElementRect(DStyle::SE_PushButtonFocusRect, & btn, this);
        style->proxy()->drawPrimitive(DStyle::PE_FrameFocusRect, &fropt, &painter2, this);
    }
}
/**
 * @brief LogPeriodButton::focusInEvent
 * 捕获最近一次获得焦点的reason以区分是否为tabfoucs
 * @param event
 */
void LogPeriodButton::focusInEvent(QFocusEvent *event)
{
    Q_UNUSED(event)
    // qCDebug(logApp) << "LogPeriodButton focus in event, reason:" << event->reason();
    m_reson = event->reason();
    if (event->reason() != Qt::MouseFocusReason && !this->isChecked()) {
        // qCDebug(logApp) << "Auto-clicking LogPeriodButton due to non-mouse focus";
        this->click();
    }

    DPushButton::focusInEvent(event);

}
