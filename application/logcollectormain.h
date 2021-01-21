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
/**
 * @brief The LogCollectorMain class 主窗口类
 */
class LogCollectorMain : public Dtk::Widget::DMainWindow
{
    Q_OBJECT
public:
    explicit LogCollectorMain(QWidget *parent = nullptr);
    ~LogCollectorMain();

    void initUI();
    void initConnection();
    void initSettings();
    void initShortCut();

public slots:
    void resizeWidthByFilterContentWidth(int iWidth);
    bool handleApplicationTabEventNotify(QObject *obj, QKeyEvent *evt);
    void slotHandleTablet(bool isActive);
    // QWidget interface
protected:
    void closeEvent(QCloseEvent *event);
private:
    /**
     * @brief m_searchEdt titlebar上的搜索框
     */
    Dtk::Widget::DSearchEdit *m_searchEdt;
    /**
     * @brief m_topRightWgt 筛选控件
     */
    FilterContent *m_topRightWgt;
    //    LogTreeView *m_treeView;
    /**
     * @brief m_logCatelogue  日志类型选择器
     */
    LogListView *m_logCatelogue;
    /**
     * @brief m_midRightWgt 数据展示控件，包括表格和详情页
     */
    DisplayContent *m_midRightWgt;
    /**
     * @brief m_hLayout 主layout,用来放m_logCatelogue和m_vLayout
     */
    QHBoxLayout *m_hLayout;
    /**
     * @brief m_vLayout 用来放置m_midRightWgt和m_topRightWgt
     */
    QVBoxLayout *m_vLayout;

    Dtk::Widget::DSplitter *m_hSplitter;
    // Resize Window      --> Ctrl+Alt+F
    QShortcut *m_scWndReize {nullptr};
    // Find font          --> Ctrl+F
    QShortcut *m_scFindFont {nullptr};
    // export file          --> Ctrl+E
    QShortcut *m_scExport {nullptr};
    int m_originFilterWidth = 0;
};

#endif  // LOGCOLLECTORMAIN_H
