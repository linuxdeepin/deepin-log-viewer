#include "logperiodbutton.h"
#include <DApplicationHelper>
#include <DPalette>
#include <QBrush>
#include <QPaintEvent>
#include <QPainter>
#include <QPen>

DWIDGET_USE_NAMESPACE

LogPeriodButton::LogPeriodButton(const QString text, QWidget *parent)
    : DPushButton(text, parent)
{
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
