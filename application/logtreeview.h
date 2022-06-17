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

#ifndef LOGTREEVIEW_H
#define LOGTREEVIEW_H

#include <DTreeView>
#include <QTime>

class QKeyEvent;
class LogViewHeaderView;
class LogViewItemDelegate;
class QTime;
class LogTreeView : public Dtk::Widget::DTreeView
{
public:
    explicit LogTreeView(QWidget *parent = nullptr);
    int singleRowHeight();
protected:
    void initUI();
    void paintEvent(QPaintEvent *event) override;
    void drawRow(QPainter *painter, const QStyleOptionViewItem &options,
                 const QModelIndex &index) const override;
    void keyPressEvent(QKeyEvent *event) override;
    bool event(QEvent *e) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void focusInEvent(QFocusEvent *event)override;

private:
    LogViewItemDelegate *m_itemDelegate;
    LogViewHeaderView *m_headerDelegate;
    // 记录触摸按下事件，在mouse move事件中使用，用于判断手指移动的距离，当大于
    // QPlatformTheme::TouchDoubleTapDistance 的值时认为触发触屏滚动
    QPoint lastTouchBeginPos;
    QModelIndex mouseLastPressedIndex;
//    int touchTapDistance = -1;
    bool m_isPressed = false;
    QPointF m_lastTouchBeginPos;
    QTime m_lastTouchTime;
    Qt::FocusReason m_reson = Qt::MouseFocusReason;
};

#endif  // LOGTREEVIEW_H
