#include "lognormalbutton.h"
#include <QKeyEvent>
#include <QDebug>
#include <QStylePainter>
#include <QEvent>

#include <DStyle>
#include <DApplication>

LogNormalButton::LogNormalButton(QWidget *parent):
    DPushButton(parent)
{
    setFocusPolicy(Qt::StrongFocus);
}

LogNormalButton::LogNormalButton(const QString &text, QWidget *parent):
    DPushButton(text, parent)
{
    setFocusPolicy(Qt::StrongFocus);
}

LogNormalButton::LogNormalButton(const QIcon &icon, const QString &text, QWidget *parent):
    DPushButton(icon, text, parent)
{
    setFocusPolicy(Qt::StrongFocus);
}

void LogNormalButton::keyPressEvent(QKeyEvent *event)
{
    if ((event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return)) {
        QKeyEvent spaceEvent(QEvent::KeyPress, Qt::Key_Space, Qt::NoModifier, 65, 32, 16, " ", event->isAutoRepeat(), static_cast<ushort>(event->count()));
        DApplication::sendEvent(this, &spaceEvent);
        return;
    }
    DPushButton::keyPressEvent(event);
}

void LogNormalButton::keyReleaseEvent(QKeyEvent *event)
{
    if ((event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return)) {
        QKeyEvent spaceEvent(QEvent::KeyRelease, Qt::Key_Space, Qt::NoModifier, 65, 32, 16, " ", event->isAutoRepeat(), static_cast<ushort>(event->count()));
        DApplication::sendEvent(this, &spaceEvent);
        return;

    }
    DPushButton::keyReleaseEvent(event);
}

void LogNormalButton::paintEvent(QPaintEvent *e)
{
    Q_UNUSED(e)
    DStyle *style = dynamic_cast<DStyle *>(DApplication::style());
    QStyleOptionButton btn;
    initStyleOption(&btn);
    QStylePainter painter(this);
    style->proxy()->drawControl(DStyle::CE_PushButtonBevel, &btn, &painter, this);
    QStyleOptionButton subopt = btn;
    subopt.rect = style->proxy()->subElementRect(DStyle::SE_PushButtonContents, &btn, this);
    style->proxy()->drawControl(DStyle::CE_PushButtonLabel, &subopt,  &painter, this);
    if (hasFocus() && (m_reson == Qt::TabFocusReason || m_reson == Qt::BacktabFocusReason)) {

        QStyleOptionFocusRect fropt;
        fropt.QStyleOption::operator=(btn);
        fropt.rect = style->proxy()->subElementRect(DStyle::SE_PushButtonFocusRect, & btn, this);
        style->proxy()->drawPrimitive(DStyle::PE_FrameFocusRect, &fropt, &painter, this);
    }

}

void LogNormalButton::focusInEvent(QFocusEvent *event)
{
    if (event->reason() != Qt::ActiveWindowFocusReason) {
        m_reson = event->reason();
    }
    DPushButton::focusInEvent(event);
}


