#ifndef LOGDETAILINFOWIDGET_H
#define LOGDETAILINFOWIDGET_H

#include <DWidget>
#include <QModelIndex>
#include <QStandardItemModel>
#include "logiconbutton.h"

class logDetailInfoWidget : public Dtk::Widget::DWidget
{
    Q_OBJECT
public:
    logDetailInfoWidget(QWidget *parent = nullptr);

    void cleanText();

private:
    void initUI();

    void fillDetailInfo(QString deamonName, QString usrName, QString pid, QString dateTime,
                        QModelIndex level, QString msg, QString status = "");

protected:
    void paintEvent(QPaintEvent *event) override;

public slots:
    void slot_DetailInfo(const QModelIndex &index, QStandardItemModel *pModel, QString name);

private:
    Dtk::Widget::DLabel *m_daemonName, *m_dateTime, *m_userName, *m_pid, *m_status;
    LogIconButton *m_level;
    Dtk::Widget::DLabel *m_userLabel, *m_pidLabel, *m_statusLabel;
    Dtk::Widget::DTextBrowser *m_textBrowser;

    QStandardItemModel *m_pModel;
};

#endif  // LOGDETAILINFOWIDGET_H
