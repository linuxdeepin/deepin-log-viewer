#include "logiconbutton.h"

LogIconButton::LogIconButton(QWidget *parent)
    : QPushButton(parent)
{
    this->setFlat(true);
    this->setFocusPolicy(Qt::NoFocus);
    this->setIconSize(QSize(32, 32));
}

LogIconButton::LogIconButton(QString text, QWidget *parent)
    : QPushButton(text, parent)
{
    this->setFlat(true);
    this->setFocusPolicy(Qt::NoFocus);
    this->setIconSize(QSize(32, 32));
}

void LogIconButton::mousePressEvent(QMouseEvent *e) {}
