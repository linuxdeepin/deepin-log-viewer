#ifndef LOGLISTVIEW_H
#define LOGLISTVIEW_H

#include <DApplicationHelper>
#include <DListView>
#include <QStandardItemModel>

class LogListView : public Dtk::Widget::DListView
{
public:
    LogListView(QWidget *parent = nullptr);
    void initUI();

protected slots:
    void onChangedTheme(Dtk::Widget::DApplicationHelper::ColorType themeType);

protected:
    void paintEvent(QPaintEvent *event);
    void currentChanged(const QModelIndex &current, const QModelIndex &previous) override;

private:
    QStandardItemModel *m_pModel;

    QString icon {"://images/"};
};

#endif  // LOGLISTVIEW_H
