#ifndef LOGTABLEVIEW_H
#define LOGTABLEVIEW_H

#include <DTableView>

class LogTableView : public Dtk::Widget::DTableView
{
public:
    LogTableView(QWidget *parent = nullptr);

    void focusInEvent(QFocusEvent *event);

    void focusOutEvent(QFocusEvent *event);
};

#endif  // LOGTABLEVIEW_H
