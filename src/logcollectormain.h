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

#include <DMainWindow>
#include <DSearchEdit>
#include <DSplitter>
#include <DTreeView>
#include <QHBoxLayout>
#include <QSplitter>
#include <QVBoxLayout>
#include "displaycontent.h"
#include "filtercontent.h"
#include "loglistview.h"
#include "logtreeview.h"

class LogCollectorMain : public Dtk::Widget::DMainWindow
{
public:
    LogCollectorMain(QWidget *parent = nullptr);
    ~LogCollectorMain();

    void initUI();
    void initConnection();

private:
    Dtk::Widget::DSearchEdit *m_searchEdt;
    FilterContent *m_topRightWgt;
    //    LogTreeView *m_treeView;
    LogListView *m_logCatelogue;
    DisplayContent *m_midRightWgt;
    QHBoxLayout *m_hLayout;
    QVBoxLayout *m_vLayout;

    Dtk::Widget::DSplitter *m_hSplitter;
};

#endif  // LOGCOLLECTORMAIN_H
