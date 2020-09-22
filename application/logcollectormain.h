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

#ifndef LOGCOLLECTORMAIN_H
#define LOGCOLLECTORMAIN_H
#include "displaycontent.h"
#include "filtercontent.h"
#include "loglistview.h"
#include "logtreeview.h"

#include <DMainWindow>
#include <DSearchEdit>
#include <DSplitter>
#include <DTreeView>


#include <QHBoxLayout>
#include <QShortcut>
#include <QSplitter>
#include <QVBoxLayout>

class DSplitter;
class LogCollectorMain : public Dtk::Widget::DMainWindow
{
public:
    LogCollectorMain(QWidget *parent = nullptr);
    ~LogCollectorMain();

    void initUI();
    void initConnection();
    void initSettings();
    void initShortCut();

public slots:
    void resizeWidthByFilterContentWidth(int iWidth);
    bool handleApplicationTabEventNotify(QObject *obj, QKeyEvent *evt);
private:
    Dtk::Widget::DSearchEdit *m_searchEdt;
    FilterContent *m_topRightWgt;
    //    LogTreeView *m_treeView;
    LogListView *m_logCatelogue;
    DisplayContent *m_midRightWgt;
    QHBoxLayout *m_hLayout;
    QVBoxLayout *m_vLayout;

    Dtk::Widget::DSplitter *m_hSplitter;

    QShortcut *m_scWndReize {nullptr};  // Resize Window      --> Ctrl+Alt+F
    QShortcut *m_scFindFont {nullptr};  // Find font          --> Ctrl+F
    QShortcut *m_scExport {nullptr};    // export file          --> Ctrl+E
    int m_originFilterWidth = 0;
    QList<QWidget * > m_focusWidgetOrder;


};

#endif  // LOGCOLLECTORMAIN_H
