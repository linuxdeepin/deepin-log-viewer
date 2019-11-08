/*
 * Copyright (C) 2019 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     LZ <zhou.lu@archermind.com>
 *
 * Maintainer: LZ <zhou.lu@archermind.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef LOGPASSWORDAUTH_H
#define LOGPASSWORDAUTH_H

#include <dabstractdialog.h>
#include <DDialog>
#include <DLabel>
#include <DPasswordEdit>
#include <DPushButton>
#include <DWidget>
#include <QTimer>
class LogPasswordAuth : public Dtk::Widget::DDialog
{
public:
    LogPasswordAuth(Dtk::Widget::DWidget *parent = nullptr);
    void initUI();
    void initConnections();

    QString getPasswd();

public slots:
    void slotOkBtnClicked();
    void slotButtonClicked(int index, const QString &text);

private:
    void setPasswd(QString str);
    QString m_passwd;
    Dtk::Widget::DPasswordEdit *m_edt;
    Dtk::Widget::DPushButton *m_okBtn, *m_cancleBtn;
    QTimer *m_timer;
};

#endif  // LOGPASSWORDAUTH_H
