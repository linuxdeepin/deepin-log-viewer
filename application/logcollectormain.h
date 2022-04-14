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
#include <DSettings>

#include <QHBoxLayout>
#include <QShortcut>
#include <QSplitter>
#include <QVBoxLayout>
#include <QTimer>
DCORE_USE_NAMESPACE

class DSplitter;
class ExportProgressDlg;
/**
 * @brief The LogCollectorMain class 主窗口类
 */
class LogCollectorMain : public Dtk::Widget::DMainWindow
{
public:
    explicit LogCollectorMain(QWidget *parent = nullptr);
    ~LogCollectorMain();

    void initUI();
    void initConnection();
    void initSettings();
    void initShortCut();
    void initTitlebarExtensions();
    void exportAllLogs();
public slots:
    bool handleApplicationTabEventNotify(QObject *obj, QKeyEvent *evt);
    void switchRefreshActionTriggered(QAction *action);

protected:
    void closeEvent(QCloseEvent *event);

private:
    /**
     * @brief m_searchEdt titlebar上的搜索框
     */
    Dtk::Widget::DSearchEdit *m_searchEdt {nullptr};
    /**
     * @brief m_topRightWgt 筛选控件
     */
    FilterContent *m_topRightWgt {nullptr};
    /**
     * @brief m_logCatelogue  日志类型选择器
     */
    LogListView *m_logCatelogue {nullptr};
    /**
     * @brief m_midRightWgt 数据展示控件，包括表格和详情页
     */
    DisplayContent *m_midRightWgt {nullptr};
    /**
     * @brief m_hLayout 主layout,用来放m_logCatelogue和m_vLayout
     */
    QHBoxLayout *m_hLayout {nullptr};
    /**
     * @brief m_vLayout 用来放置m_midRightWgt和m_topRightWgt
     */
    QVBoxLayout *m_vLayout {nullptr};

    Dtk::Widget::DSplitter *m_hSplitter;
    // Resize Window      --> Ctrl+Alt+F
    QShortcut *m_scWndReize {nullptr};
    // Find font          --> Ctrl+F
    QShortcut *m_scFindFont {nullptr};
    // export file          --> Ctrl+E
    QShortcut *m_scExport {nullptr};
    int m_originFilterWidth = 0;

    QList<QAction *> m_refreshActions;
    QTimer *m_refreshTimer {nullptr};
    DSettings *m_settings {nullptr};

    DIconButton *m_refreshBtn {nullptr};
    DIconButton *m_exportAllBtn {nullptr};
    ExportProgressDlg *m_exportDlg {nullptr};
};

#endif // LOGCOLLECTORMAIN_H
