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
#include "filtercontent.h" //add by Airy for new menu

class LogListView : public Dtk::Widget::DListView
{
    Q_OBJECT
public:
    LogListView(QWidget *parent = nullptr);
    void initUI();

    void setDefaultSelect();
    void truncateFile(QString path_); //add by Airy for truncate file

private:
    void setCustomFont(QStandardItem *item);
    bool isFileExist(const QString &iFile);

protected slots:
    void onChangedTheme(Dtk::Widget::DApplicationHelper::ColorType themeType);

public slots:
    void slot_getAppPath(QString path);  // add by Airy

protected:
    void paintEvent(QPaintEvent *event) override;
    void currentChanged(const QModelIndex &current, const QModelIndex &previous) override;
    void contextMenuEvent(QContextMenuEvent *event) override;   //add by Airy

signals:
    void itemChanged();
    void sigRefresh(const QModelIndex &index);  // add refresh

private:
    QStandardItemModel *m_pModel;

    QString icon {"://images/"};

    // add
    QMenu *g_context;
    QAction *g_openForder;
    QAction *g_clear;
    QAction *g_refresh;  // add

    QString g_path;                  // add by Airy
    FilterContent *g_filtercontent;  // add
};

#endif  // LOGLISTVIEW_H
