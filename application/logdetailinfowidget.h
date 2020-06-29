#ifndef LOGDETAILINFOWIDGET_H
#define LOGDETAILINFOWIDGET_H

#include <DHorizontalLine>
#include <DLabel>
#include <DWidget>
#include <QModelIndex>
#include <QStandardItemModel>
#include "logiconbutton.h"
class QStandardItemModel;
class logDetailInfoWidget : public Dtk::Widget::DWidget
{
    Q_OBJECT
public:
    logDetailInfoWidget(QWidget *parent = nullptr);

    void cleanText();

    void hideLine(bool isHidden);

private:
    void initUI();
    void setTextCustomSize(QWidget *w);

    void fillDetailInfo(QString deamonName, QString usrName, QString pid, QString dateTime,
                        QModelIndex level, QString msg, QString status = "", QString action = "",
                        QString uname = "", QString event = "");  // modified by Airy

protected:
    void paintEvent(QPaintEvent *event) override;
    // bool event(QEvent *ev) override;
public slots:
    void slot_DetailInfo(const QModelIndex &index, QStandardItemModel *pModel, QString name);

private:
    Dtk::Widget::DLabel *m_daemonName, *m_dateTime, *m_userName, *m_pid, *m_action, *m_status,
        *m_name, *m_event;  // modified by Airy
    LogIconButton *m_level;
    Dtk::Widget::DLabel *m_userLabel, *m_pidLabel, *m_statusLabel, *m_actionLabel, *m_nameLabel,
        *m_eventLabel;  // modified by Airy
    Dtk::Widget::DTextBrowser *m_textBrowser;
    Dtk::Widget::DHorizontalLine *m_hline;

    QStandardItemModel *m_pModel;
};

#endif  // LOGDETAILINFOWIDGET_H
