#ifndef LOGTREEVIEW_H
#define LOGTREEVIEW_H

#include <DTreeView>
#include <QStandardItem>
#include "logviewheaderview.h"
#include "logviewitemdelegate.h"

class LogTreeView : public Dtk::Widget::DTreeView
{
public:
    LogTreeView(QWidget *parent = nullptr);

private:
    void initUI();
    void paintEvent(QPaintEvent *event);

private:
    LogViewItemDelegate *m_itemDelegate;
    LogViewHeaderView *m_headerDelegate;
};

#endif  // LOGTREEVIEW_H
