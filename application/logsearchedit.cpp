#include "logsearchedit.h"
#include <QDebug>

LogSearchEdit::LogSearchEdit(QWidget *parent):
    DSearchEdit(parent)
{

}

void LogSearchEdit::focusInEvent(QFocusEvent *event)
{
    qDebug() << "LogSearchEdit::focusInEvent";
    this->setFocus();
}
