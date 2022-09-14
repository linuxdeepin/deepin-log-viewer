// SPDX-FileCopyrightText: 2019 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef LOGLISTVIEW_H
#define LOGLISTVIEW_H

#include "structdef.h"

#include <DApplicationHelper>
#include <DStyledItemDelegate>
#include <DListView>
class QShortcut;
class QStandardItemModel;
class LogListView;
class LogListDelegate : public Dtk::Widget::DStyledItemDelegate
{
public:
    explicit LogListDelegate(LogListView *parent = nullptr);

protected:
    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const override;

    bool helpEvent(QHelpEvent *event, QAbstractItemView *view, const QStyleOptionViewItem &option, const QModelIndex &index) override;

private:
    void hideTooltipImmediately();
};
class LogListView : public Dtk::Widget::DListView
{
    Q_OBJECT
public:
    explicit LogListView(QWidget *parent = nullptr);
    void initUI();
    void setDefaultSelect();
    void truncateFile(QString path_); //add by Airy for truncate file
    QStringList getLogTypes() { return m_logTypes; }

private:
    bool isFileExist(const QString &iFile);
public slots:
    void slot_getAppPath(int id, QString path); // add by Airy
    Qt::FocusReason focusReson();
    void showRightMenu(const QPoint &pos, bool isUsePoint);
    void requestshowRightMenu(const QPoint &pos);

protected:
    void paintEvent(QPaintEvent *event) override;
    void currentChanged(const QModelIndex &current, const QModelIndex &previous) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void focusInEvent(QFocusEvent *event) override;
    void focusOutEvent(QFocusEvent *event) override;
signals:
    void itemChanged(const QModelIndex &index);
    void sigRefresh(const QModelIndex &index); // add refresh

private:
    QStandardItemModel *m_pModel {nullptr};
    QString icon = ICONLIGHTPREFIX;
    // add
    QMenu *g_context {nullptr};
    QAction *g_openForder {nullptr};
    QAction *g_clear {nullptr};
    QAction *g_refresh {nullptr}; // add
    QString g_path {""}; // add by Airy
    QShortcut *m_rightClickTriggerShortCut {nullptr};
    //判断是否通过tab获取的焦点
    Qt::FocusReason m_reson = Qt::MouseFocusReason;
    QStringList m_logTypes;
};

#endif // LOGLISTVIEW_H
