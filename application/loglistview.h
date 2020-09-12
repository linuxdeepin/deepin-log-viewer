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

#include "structdef.h"

#include <DApplicationHelper>
#include <DStyledItemDelegate>
#include <DListView>
class QShortcut;
class QStandardItemModel;
class LogListView ;
class LogListDelegate : public Dtk::Widget::DStyledItemDelegate
{
public:
    LogListDelegate(LogListView *parent = nullptr);
protected:

    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const override;

    bool helpEvent(QHelpEvent *event, QAbstractItemView *view, const QStyleOptionViewItem &option, const QModelIndex &index)override;

private:
    void hideTooltipImmediately();

};
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
    Qt::FocusReason focusReson();
    void showRightMenu(const QPoint &pos, bool isUsePoint);
    void requestshowRightMenu(const QPoint &pos);
protected:
    void paintEvent(QPaintEvent *event) override;
    void currentChanged(const QModelIndex &current, const QModelIndex &previous) override;
    // void contextMenuEvent(QContextMenuEvent *event) override;   //add by Airy
    void mouseMoveEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void focusInEvent(QFocusEvent *event)override;
    void focusOutEvent(QFocusEvent *event)override;
signals:
    void itemChanged(const QModelIndex &index);
    void sigRefresh(const QModelIndex &index);  // add refresh

private:
    QStandardItemModel *m_pModel{nullptr};
    QString icon = ICONLIGHTPREFIX;
    // add
    QMenu *g_context{nullptr};
    QAction *g_openForder{nullptr};
    QAction *g_clear{nullptr};
    QAction *g_refresh{nullptr};  // add
    QString g_path{""};                  // add by Airy
    QShortcut *m_rightClickTriggerShortCut {nullptr};
    //判断是否通过tab获取的焦点
    Qt::FocusReason m_reson;
};

#endif  // LOGLISTVIEW_H
