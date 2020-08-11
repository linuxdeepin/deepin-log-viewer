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

#ifndef DISPLAYCONTENT_H
#define DISPLAYCONTENT_H

#include <DApplicationHelper>
#include <DIconButton>
#include <DLabel>
#include <DSpinner>
#include <DTableView>
#include <DTextBrowser>
#include <QStandardItemModel>
#include <QWidget>
#include "filtercontent.h"  //add by Airy
#include "logdetailinfowidget.h"
#include "logfileparser.h"
#include "logiconbutton.h"
#include "logspinnerwidget.h"
#include "logtreeview.h"
#include "structdef.h"



class DisplayContent : public Dtk::Widget::DWidget
{
    Q_OBJECT
    enum LOAD_STATE {
        DATA_LOADING = 0,
        DATA_COMPLETE,
        DATA_LOADING_K,
        DATA_NO_SEARCH_RESULT,
    };
public:
    explicit DisplayContent(QWidget *parent = nullptr);
    ~DisplayContent();


private:
    void initUI();
    void initMap();
    void initTableView();
    void setTableViewData();
    void initConnections();

    void generateJournalFile(int id, int lId, const QString &iSearchStr = "");
    void createJournalTableStart(QList<LOG_MSG_JOURNAL> &list);
    void createJournalTableForm();
    void generateDpkgFile(int id, const QString &iSearchStr = "");
    void createDpkgTable(QList<LOG_MSG_DPKG> &list);

    void generateKernFile(int id, const QString &iSearchStr = "");
    void createKernTable(QList<LOG_MSG_JOURNAL> &list);

    void generateAppFile(QString path, int id, int lId, const QString &iSearchStr = "");
    void createAppTable(QList<LOG_MSG_APPLICATOIN> &list);
    void createApplicationTable(QList<LOG_MSG_APPLICATOIN> &list);

    void createBootTable(QList<LOG_MSG_BOOT> &list);

    void createXorgTable(QList<LOG_MSG_XORG> &list);
    void generateXorgFile(int id);  // add by Airy for peroid

    void creatKwinTable(QList<LOG_MSG_KWIN> &list);
    void generateKwinFile(KWIN_FILTERS iFilters);

    void createNormalTable(QList<LOG_MSG_NORMAL> &list);  // add by Airy
    void generateNormalFile(int id);                      // add by Airy for peroid

    void insertJournalTable(QList<LOG_MSG_JOURNAL> logList, int start, int end);
    void insertApplicationTable(QList<LOG_MSG_APPLICATOIN> list, int start, int end);
    void insertKernTable(QList<LOG_MSG_JOURNAL> list, int start,
                         int end);  // add by Airy for bug 12263
    void createKernTableForm();
    void createAppTableForm();
    QString getAppName(QString filePath);

    bool isAuthProcessAlive();



    //

signals:
    void loadMoreInfo();
    void sigDetailInfo(QModelIndex index, QStandardItemModel *pModel, QString name);
    void setExportEnable(bool iEnable);
public slots:
    void slot_tableItemClicked(const QModelIndex &index);
    void slot_BtnSelected(int btnId, int lId, QModelIndex idx);
    void slot_appLogs(QString path);

    void slot_logCatelogueClicked(const QModelIndex &index);
    void slot_exportClicked();

    void slot_statusChagned(QString status);

    void slot_dpkgFinished(QList<LOG_MSG_DPKG> list);
    void slot_XorgFinished(QList<LOG_MSG_XORG> list);
    void slot_bootFinished(QList<LOG_MSG_BOOT> list);
    void slot_kernFinished(QList<LOG_MSG_JOURNAL> list);
    void slot_kwinFinished(QList<LOG_MSG_KWIN> list);
    void slot_journalFinished();
    void slot_journalData(QList<LOG_MSG_JOURNAL> list);
    void slot_applicationFinished(QList<LOG_MSG_APPLICATOIN> list);
    void slot_NormalFinished();  // add by Airy

    void slot_vScrollValueChanged(int value);

    void slot_searchResult(QString str);

    void slot_themeChanged(Dtk::Widget::DApplicationHelper::ColorType colorType);

    void slot_getLogtype(int tcbx);  // add by Airy
    void slot_refreshClicked(const QModelIndex &index); //add by Airy for adding refresh
//导出前把当前要导出的当前信息的Qlist转换成QStandardItemModel便于导出
    void parseListToModel(QList<LOG_MSG_DPKG> iList, QStandardItemModel *oPModel);
    void parseListToModel(QList<LOG_MSG_BOOT> iList, QStandardItemModel *oPModel);
    void parseListToModel(QList<LOG_MSG_APPLICATOIN> iList, QStandardItemModel *oPModel);
    void parseListToModel(QList<LOG_MSG_XORG> iList, QStandardItemModel *oPModel);
    void parseListToModel(QList<LOG_MSG_JOURNAL> iList, QStandardItemModel *oPModel);
    void parseListToModel(QList<LOG_MSG_NORMAL> iList, QStandardItemModel *oPModel);
    void parseListToModel(QList<LOG_MSG_KWIN> iList, QStandardItemModel *oPModel);
    QString getIconByname(QString str);
    void setLoadState(LOAD_STATE iState);
    void onExportResult(bool isSuccess);
    void clearAllFilter();
    void clearAllDatalist();
    void filterBoot(BOOT_FILTERS ibootFilter);
    void filterNomal(NORMAL_FILTERS inormalFilter);
private:
    void paintEvent(QPaintEvent *event);
    void resizeEvent(QResizeEvent *event);

private:
    //    Dtk::Widget::DTableView *m_tableView;
    LogTreeView *m_treeView;
    QStandardItemModel *m_pModel;


    logDetailInfoWidget *m_detailWgt {nullptr};
    Dtk::Widget::DLabel *noResultLabel {nullptr};
    QModelIndex m_curListIdx;
    QMap<QString, QString> m_transDict;
    int m_limitTag {0};

    LogSpinnerWidget *m_spinnerWgt;
    LogSpinnerWidget *m_spinnerWgt_K;  // add by Airy

    QString m_curAppLog;
    QString m_currentStatus;

    int m_curBtnId {ALL};
    int m_curLevel {INF};

    LOG_FLAG m_flag {NONE};

    LogFileParser m_logFileParse;
    QList<LOG_MSG_JOURNAL> jList, jListOrigin; // journalctl cmd.
    QList<LOG_MSG_DPKG> dList, dListOrigin;    // dpkg.log
    //    QStringList xList;                           // Xorg.0.log
    QList<LOG_MSG_XORG> xList, xListOrigin;                  // Xorg.0.log
    QList<LOG_MSG_BOOT> bList, currentBootList;  // boot.log
    QList<LOG_MSG_JOURNAL> kList, kListOrigin;                // kern.log
    QList<LOG_MSG_APPLICATOIN> appList, appListOrigin;         //~/.cache/deepin/xxx.log(.xxx)
    QList<LOG_MSG_NORMAL> norList;               // add by Airy
    QList<LOG_MSG_NORMAL> nortempList;           // add by Airy
    QList<LOG_MSG_KWIN> m_currentKwinList;
    QList<LOG_MSG_KWIN> m_kwinList;                   //$HOME/.kwin.log
    QString m_iconPrefix = ICONPREFIX;
    QMap<QString, QString> m_icon_name_map;
    QString m_currentSearchStr{""};
    KWIN_FILTERS m_currentKwinFilter;
    bool m_firstLoadPageData = false;
    BOOT_FILTERS m_bootFilter = {"", ""};
    NORMAL_FILTERS m_normalFilter = {"", -1};
    DisplayContent::LOAD_STATE m_state;
};

#endif  // DISPLAYCONTENT_H
