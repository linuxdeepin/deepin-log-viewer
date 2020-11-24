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
#include <QBrush>
#include <QPaintEvent>
#include <QPainterPath>
#include <QPainter>
#include <QPen>

DWIDGET_USE_NAMESPACE

LogPeriodButton::LogPeriodButton(const QString text, QWidget *parent)
    : DPushButton(text, parent)
{
    this->setFocusPolicy(Qt::NoFocus);
    QFont f = font();
    f.setPixelSize(16);
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

void LogPeriodButton::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

    QRectF rect = this->rect();
//    if (m_stahndardWidth > 0) {
//        this->setText(QFontMetrics(this->font()).elidedText(this->text(), Qt::ElideRight, m_stahndardWidth));
//    }
    // this->setText(QFontMetrics(this->font()).elidedText(this->text(), Qt::ElideRight, 30));
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
    DPushButton::paintEvent(event);
}
