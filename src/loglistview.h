#ifndef LOGLISTVIEW_H
#define LOGLISTVIEW_H

#include <DListView>
#include <QStandardItemModel>

class LogListView : public Dtk::Widget::DListView
{
public:
    LogListView(QWidget *parent = nullptr);
    void initUI();

private:
    QStandardItemModel *m_pModel;
};

#endif  // LOGLISTVIEW_H
