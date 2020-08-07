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

#include "logperiodbutton.h"
#include <DApplicationHelper>
#include <DPalette>
#include <DStyle>
#include <DApplication>
#include <QBrush>
#include <QPaintEvent>
#include <QPainter>
#include <QPen>
#include <QDebug>
#include <QStylePainter>


DWIDGET_USE_NAMESPACE

LogPeriodButton::LogPeriodButton(const QString text, QWidget *parent)
    : DPushButton(text, parent)
{
    this->setFocusPolicy(Qt::StrongFocus);
    QFont f = font();
    f.setPixelSize(16);
    setFlat(true);
    setCheckable(true);
    setContentsMargins(18, 18, contentsMargins().top(), contentsMargins().bottom());
}

void LogPeriodButton::setStandardSize(int iStahndardWidth)
{
    m_stahndardWidth = iStahndardWidth;
}

void LogPeriodButton::enterEvent(QEvent *e)
{
    isEnter = true;
    DPushButton::enterEvent(e);
}

void LogPeriodButton::leaveEvent(QEvent *e)
{
    isEnter = false;
    DPushButton::leaveEvent(e);
}

/**
 * @brief LogPeriodButton::paintEvent  通过鼠标是否在按钮上的状态绘hover效果
 * @param event
 */
void LogPeriodButton::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

    QRectF rect = this->rect();
    if (isEnter) {
        this->setAutoFillBackground(true);
        this->setBackgroundRole(DPalette::Base);

        painter.setRenderHint(QPainter::Antialiasing);
        DPalette pa = DApplicationHelper::instance()->palette(this);
        painter.setBrush(QBrush(pa.color(DPalette::Light)));
        QColor penColor = pa.color(DPalette::Base);
        painter.setPen(QPen(penColor));

        QPainterPath painterPath;
        painterPath.addRoundedRect(rect, 8, 8);
        painter.drawPath(painterPath);
    } else {
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
    if (hasFocus() && (m_reson == Qt::TabFocusReason)) {

        QStyleOptionFocusRect fropt;
        fropt.QStyleOption::operator=(btn);
        fropt.rect = style->proxy()->subElementRect(DStyle::SE_PushButtonFocusRect, & btn, this);
        style->proxy()->drawPrimitive(DStyle::PE_FrameFocusRect, &fropt, &painter2, this);
    }
}

void LogPeriodButton::focusInEvent(QFocusEvent *event)
{
    Q_UNUSED(event)
    m_reson = event->reason();
    if (event->reason() != Qt::MouseFocusReason && !this->isChecked()) {
        this->click();
    }

    DPushButton::focusInEvent(event);

}

