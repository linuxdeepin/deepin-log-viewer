/*
* Copyright (C) 2019 ~ 2020 UnionTech Software Technology Co.,Ltd
*
* Author:     zyc <zyc@uniontech.com>
* Maintainer:  zyc <zyc@uniontech.com>
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* any later version.
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef LOGDETAILINFOWIDGET_H
#define LOGDETAILINFOWIDGET_H
#include "logiconbutton.h"
#include "logdetailedit.h"

#include <DHorizontalLine>
#include <DLabel>
#include <DWidget>
#include <DTextBrowser>

#include <QModelIndex>
#include <QStandardItemModel>

DWIDGET_USE_NAMESPACE
class QStandardItemModel;
/**
 * @brief The logDetailInfoWidget class 详情页控件
 */
class logDetailInfoWidget : public Dtk::Widget::DWidget
{
    Q_OBJECT
public:
    explicit logDetailInfoWidget(QWidget *parent = nullptr);

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
    //m_daemonName:进程名显示控件 m_dateTime:时间显示控件 m_userName：用户名显示控件  m_pid：进程号显示控件 m_action：动作显示控件  m_status：状态显示控件 m_name:开关机日志用户名显示控件 m_event: 开关机日志时间类型显示
    Dtk::Widget::DLabel *m_daemonName, *m_dateTime, *m_userName, *m_pid, *m_action, *m_status,
        *m_name, *m_event;  // modified by Airy
    /**
     * @brief m_level 日志等级显示控件
     */
    LogIconButton *m_level;
    Dtk::Widget::DLabel *m_userLabel, *m_pidLabel, *m_statusLabel, *m_actionLabel, *m_nameLabel,
        *m_eventLabel;  // modified by Airy
    /**
     * @brief m_textBrowser 日志信息显示控件
     */
    logDetailEdit *m_textBrowser;
    /**
     * @brief m_hline 中间的分割线
     */
    Dtk::Widget::DHorizontalLine *m_hline;
    /**
     * @brief m_pModel 当前主表的model
     */
    QStandardItemModel *m_pModel;
};

#endif  // LOGDETAILINFOWIDGET_H
