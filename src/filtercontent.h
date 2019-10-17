#ifndef FILTERCONTENT_H
#define FILTERCONTENT_H

#include <DComboBox>
#include <DSuggestButton>
#include <DWidget>
#include <QButtonGroup>
#include <QWidget>

class FilterContent : public Dtk::Widget::DWidget
{
    Q_OBJECT
public:
    explicit FilterContent(QWidget *parent = nullptr);
    ~FilterContent();

    void initUI();
    void initConnections();

private:
    void setAppComboBoxItem();
    void setComboBoxVisible(bool first, bool second);

signals:
    void sigButtonClicked(int tId, int lId, QModelIndex idx);
    //    void sigCbxLvIdxChanged(int idx);
    void sigCbxAppIdxChanged(QString path);
    void sigExportInfo();

public slots:
    void slot_treeClicked(const QModelIndex &index);
    void slot_buttonClicked(int idx);
    void slot_cbxLvIdxChanged(int idx);
    void slot_cbxAppIdxChanged(int idx);

private:
    QButtonGroup *m_btnGroup;
    Dtk::Widget::DLabel *lvTxt;
    Dtk::Widget::DComboBox *cbx_lv;
    Dtk::Widget::DLabel *appTxt;
    Dtk::Widget::DComboBox *cbx_app;
    QModelIndex m_curTreeIndex;

    int m_curBtnId, m_curLvCbxId;
};

#endif  // FILTERCONTENT_H
