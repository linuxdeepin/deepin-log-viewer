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
