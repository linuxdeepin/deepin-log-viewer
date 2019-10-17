#ifndef LOGTREEVIEW_H
#define LOGTREEVIEW_H

#include <DTreeView>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QTreeView>

class LogTreeView : public Dtk::Widget::DTreeView
{
public:
    LogTreeView(QWidget *parent = nullptr);

    void slot_addAppLogs(QString path);

private:
    void initUI();
    void clearChildren(QStandardItem *pItem);

private:
    QStandardItemModel *m_pModel;
};

#endif  // LOGTREEVIEW_H
