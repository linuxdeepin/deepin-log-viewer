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

#include "logpasswordauth.h"
#include <DApplication>
#include <QDebug>
#include <QHBoxLayout>
#include <QVBoxLayout>

DWIDGET_USE_NAMESPACE

LogPasswordAuth::LogPasswordAuth(DWidget *parent)
    : DDialog(parent)
{
    //    this->setFixedSize(350, 120);

    initUI();
    initConnections();
}

void LogPasswordAuth::initUI()
{
    this->setTitle(
        DApplication::translate("password", "please input sudo password to execute operate"));
    m_edt = new DPasswordEdit(this);
    m_edt->setEchoMode(QLineEdit::Password);
    //    m_edt->setMaximumHeight(30);
    this->addSpacing(10);
    this->addContent(m_edt);
    this->setIcon(QIcon("://images/logo.svg"), QSize(50, 50));
    this->addButton(DApplication::translate("Button", "Cancel"));
    this->addButton(DApplication::translate("Button", "Comfirm"), true);
}

void LogPasswordAuth::initConnections()
{
    connect(this, &DDialog::buttonClicked, this, &LogPasswordAuth::slotButtonClicked);
}

QString LogPasswordAuth::getPasswd()
{
    return m_passwd;
}

void LogPasswordAuth::slotButtonClicked(int index, const QString &text)
{
    Q_UNUSED(text)

    switch (index) {
        case 0:
            this->setPasswd("");
            this->setOnButtonClickedClose(true);
            break;
        case 1:
            if (m_edt->text().isEmpty())
                this->setOnButtonClickedClose(false);
            else {
                this->setOnButtonClickedClose(true);
                this->setPasswd(m_edt->text());
            }
            break;
        default:
            break;
    }
    m_edt->clear();
}

void LogPasswordAuth::setPasswd(QString str)
{
    m_passwd = str;
}
