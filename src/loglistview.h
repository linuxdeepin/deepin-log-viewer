#ifndef LOGLISTVIEW_H
#define LOGLISTVIEW_H

#include <DApplicationHelper>
#include <DListView>
#include <QStandardItemModel>

class LogListView : public Dtk::Widget::DListView
{
    Q_OBJECT
public:
    LogListView(QWidget *parent = nullptr);
    void initUI();

    void setDefaultSelect();

private:
    void setCustomFont(QStandardItem *item);

protected slots:
    void onChangedTheme(Dtk::Widget::DApplicationHelper::ColorType themeType);

protected:
    void paintEvent(QPaintEvent *event) override;
    void currentChanged(const QModelIndex &current, const QModelIndex &previous) override;

signals:
    void itemChanged();

private:
    QStandardItemModel *m_pModel;

    QString icon {"://images/"};
};

#endif  // LOGLISTVIEW_H
