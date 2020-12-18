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
#include "filtercontent.h"  //add by Airy
#include "logdetailinfowidget.h"
#include "logfileparser.h"
#include "logiconbutton.h"
#include "logspinnerwidget.h"
#include "logtreeview.h"
#include "structdef.h"

#include <DApplicationHelper>
#include <DIconButton>
#include <DLabel>
#include <DSpinner>
#include <DTableView>
#include <DTextBrowser>

#include <QStandardItemModel>
#include <QWidget>
#include <QDateTime>


class ExportProgressDlg;
/**
 * @brief The DisplayContent class 主显示数据区域控件,包括数据表格和详情页
 */
class DisplayContent : public Dtk::Widget::DWidget
{
    Q_OBJECT
    /**
     * @brief The LOAD_STATE enum 主表部分的显示状态
     */
    enum LOAD_STATE {
        DATA_LOADING = 0, //正在加载
        DATA_COMPLETE, //加载完成
        DATA_LOADING_K, //内核日志正在加载
        DATA_NO_SEARCH_RESULT, //搜索无记录
    };
public:
    explicit DisplayContent(QWidget *parent = nullptr);
    ~DisplayContent();
    LogTreeView *mainLogTableView();

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
    void createDpkgTableStart(QList<LOG_MSG_DPKG> &list);
    void createDpkgTableForm();

    void generateKernFile(int id, const QString &iSearchStr = "");
    void createKernTableForm();
    void createKernTable(QList<LOG_MSG_JOURNAL> &list);

    void generateAppFile(QString path, int id, int lId, const QString &iSearchStr = "");
    void createAppTableForm();
    void createAppTable(QList<LOG_MSG_APPLICATOIN> &list);

    void createBootTableForm();
    void createBootTable(QList<LOG_MSG_BOOT> &list);
    void generateBootFile();

    void createXorgTableForm();
    void createXorgTable(QList<LOG_MSG_XORG> &list);
    void generateXorgFile(int id);  // add by Airy for peroid

    void createKwinTableForm();
    void creatKwinTable(QList<LOG_MSG_KWIN> &list);
    void generateKwinFile(KWIN_FILTERS iFilters);

    void createNormalTableForm();
    void createNormalTable(QList<LOG_MSG_NORMAL> &list);  // add by Airy
    void generateNormalFile(int id);                      // add by Airy for peroid

    void insertJournalTable(QList<LOG_MSG_JOURNAL> logList, int start, int end);
    void insertApplicationTable(QList<LOG_MSG_APPLICATOIN> list, int start, int end);
    void insertKernTable(QList<LOG_MSG_JOURNAL> list, int start,
                         int end);  // add by Airy for bug 12263
    void insertDpkgTable(QList<LOG_MSG_DPKG> list, int start, int end);
    void insertXorgTable(QList<LOG_MSG_XORG> list, int start, int end);
    void insertBootTable(QList<LOG_MSG_BOOT> list, int start, int end);
    void insertKwinTable(QList<LOG_MSG_KWIN> list, int start, int end);
    void insertNormalTable(QList<LOG_MSG_NORMAL> list, int start, int end);
    QString getAppName(QString filePath);

    bool isAuthProcessAlive();

    void generateJournalBootFile(int lId, const QString &iSearchStr = "");
    void createJournalBootTableStart(QList<LOG_MSG_JOURNAL> &list);
    void createJournalBootTableForm();
    void insertJournalBootTable(QList<LOG_MSG_JOURNAL> logList, int start, int end);

    void generateDnfFile(BUTTONID iDate, DNFPRIORITY iLevel);
    void createDnfTable(QList<LOG_MSG_DNF> &list);

    void generateDmesgFile(BUTTONID iDate, PRIORITY iLevel);
    void createDmesgTable(QList<LOG_MSG_DMESG> &list);
    void createDnfForm();
    void createDmesgForm();
    void insertDmesgTable(QList<LOG_MSG_DMESG> list, int start, int end);
    void insertDnfTable(QList<LOG_MSG_DNF> list, int start, int end);
    //

signals:
    void loadMoreInfo();
    /**
     * @brief sigDetailInfo 选中treeview显示详情页信号
     * @param index 当前选中的index
     * @param pModel 当前的model指针
     * @param name 当前应用日志选择的日志名称
     */
    void sigDetailInfo(QModelIndex index, QStandardItemModel *pModel, QString name);
    /**
     * @brief setExportEnable 是否允许导出信号
     * @param iEnable 是否允许导出
     */
    void setExportEnable(bool iEnable);

public slots:
    void slot_tableItemClicked(const QModelIndex &index);
    void slot_BtnSelected(int btnId, int lId, QModelIndex idx);
    void slot_appLogs(QString path);

    void slot_logCatelogueClicked(const QModelIndex &index);
    void slot_exportClicked();

    void slot_statusChagned(QString status);

    void slot_dpkgFinished(int index);
    void slot_dpkgData(int index, QList<LOG_MSG_DPKG> list);
    void slot_XorgFinished(int index);
    void slot_xorgData(int index, QList<LOG_MSG_XORG> list);
    void slot_bootFinished(int index);
    void slot_bootData(int index, QList<LOG_MSG_BOOT> list);
    void slot_kernFinished(int index);
    void slot_kernData(int index, QList<LOG_MSG_JOURNAL> list);
    void slot_kwinFinished(int index);
    void slot_kwinData(int index, QList<LOG_MSG_KWIN> list);
    void slot_dnfFinished(QList<LOG_MSG_DNF> list);
    void slot_dmesgFinished(QList<LOG_MSG_DMESG> list);
    void slot_journalFinished(int index);
    void slot_journalBootFinished(int index);
    void slot_journalBootData(int index, QList<LOG_MSG_JOURNAL> list);
    void slot_journalData(int index, QList<LOG_MSG_JOURNAL> list);
    void slot_applicationFinished(int index);
    void slot_applicationData(int index, QList<LOG_MSG_APPLICATOIN> list);
    void slot_normalFinished(int index);
    void slot_normalData(int index, QList<LOG_MSG_NORMAL> list);

    void slot_logLoadFailed(const QString &iError);
    void slot_vScrollValueChanged(int valuePixel);

    void slot_searchResult(QString str);

    void slot_themeChanged(Dtk::Widget::DApplicationHelper::ColorType colorType);

    void slot_getLogtype(int tcbx);  // add by Airy
    void slot_refreshClicked(const QModelIndex &index); //add by Airy for adding refresh
    void slot_dnfLevel(DNFPRIORITY iLevel);
    //导出前把当前要导出的当前信息的Qlist转换成QStandardItemModel便于导出
    void parseListToModel(QList<LOG_MSG_DPKG> iList, QStandardItemModel *oPModel);
    void parseListToModel(QList<LOG_MSG_BOOT> iList, QStandardItemModel *oPModel);
    void parseListToModel(QList<LOG_MSG_APPLICATOIN> iList, QStandardItemModel *oPModel);
    void parseListToModel(QList<LOG_MSG_XORG> iList, QStandardItemModel *oPModel);
    void parseListToModel(QList<LOG_MSG_JOURNAL> iList, QStandardItemModel *oPModel);
    void parseListToModel(QList<LOG_MSG_NORMAL> iList, QStandardItemModel *oPModel);
    void parseListToModel(QList<LOG_MSG_KWIN> iList, QStandardItemModel *oPModel);
    void parseListToModel(QList<LOG_MSG_DNF> iList, QStandardItemModel *oPModel);
    void parseListToModel(QList<LOG_MSG_DMESG> iList, QStandardItemModel *oPModel);
    QString getIconByname(QString str);
    void setLoadState(LOAD_STATE iState);
    void onExportProgress(int nCur, int nTotal);
    void onExportResult(bool isSuccess);
    void onExportFakeCloseDlg();
    void clearAllFilter();
    void clearAllDatalist();
    QList<LOG_MSG_BOOT> filterBoot(BOOT_FILTERS ibootFilter, QList<LOG_MSG_BOOT> &iList);
    QList<LOG_MSG_NORMAL> filterNomal(NORMAL_FILTERS inormalFilter, QList<LOG_MSG_NORMAL> &iList);
    QList<LOG_MSG_DPKG> filterDpkg(const QString &iSearchStr, QList<LOG_MSG_DPKG> &iList);
    QList<LOG_MSG_JOURNAL> filterKern(const QString &iSearchStr, QList<LOG_MSG_JOURNAL> &iList);
    QList<LOG_MSG_XORG> filterXorg(const QString &iSearchStr, QList<LOG_MSG_XORG> &iList);
    QList<LOG_MSG_KWIN> filterKwin(const QString &iSearchStr, QList<LOG_MSG_KWIN> &iList);
    QList<LOG_MSG_APPLICATOIN> filterApp(const QString &iSearchStr, QList<LOG_MSG_APPLICATOIN> &iList);
    QList<LOG_MSG_JOURNAL> filterJournal(const QString &iSearchStr, QList<LOG_MSG_JOURNAL> &iList);
    QList<LOG_MSG_JOURNAL> filterJournalBoot(const QString &iSearchStr, QList<LOG_MSG_JOURNAL> &iList);

private:
    void paintEvent(QPaintEvent *event);
    void resizeEvent(QResizeEvent *event);

private:
    /**
     * @brief m_treeView 主数据表控件件
     */
    LogTreeView *m_treeView;
    /**
     * @brief m_pModel 主数据表的model
     */
    QStandardItemModel *m_pModel;

    //详情页控件
    logDetailInfoWidget *m_detailWgt {nullptr};
    //搜索无结果时显示无搜索结果提示的label
    Dtk::Widget::DLabel *noResultLabel {nullptr};
    //当前选中的日志类型的index
    QModelIndex m_curListIdx;
    //日志等级的显示文本和代码内文本的转换map
    QMap<QString, QString> m_transDict;
    //分页加载时,当前加载到的页数
    int m_limitTag {0};
    /**
     * @brief m_spinnerWgt 加载数据时转轮控件
     */
    LogSpinnerWidget *m_spinnerWgt;
    /**
     * @brief m_spinnerWgt_K 加载内核日志数据时转轮控件
     */
    LogSpinnerWidget *m_spinnerWgt_K;  // add by Airy
    /**
     * @brief m_curAppLog 当前选中的应用的日志文件路径
     */
    QString m_curAppLog;

    QString m_currentStatus;

    //当前选中的时间筛选选项
    int m_curBtnId {ALL};
    //当前选中的等级筛选选项
    int m_curLevel {INF};

    //当前加载的日志类型
    LOG_FLAG m_flag {NONE};
    /**
     * @brief m_logFileParse 获取日志工具类对象
     */
    LogFileParser m_logFileParse;

    /**
     * @brief jBootList 经过筛选完成的启动日志列表
     */
    /**
     * @brief jBootListOrigin 未经过筛选的启动日志数据 journalctl --boot cmd.
     */
    QList<LOG_MSG_JOURNAL> jBootList, jBootListOrigin;

    /**
     * @brief jList 经过筛选完成的系统日志数据
     */
    /**
     * @brief jListOrigin 未经过筛选的系统日志数据 journalctl cmd.
     */
    QList<LOG_MSG_JOURNAL> jList, jListOrigin;
    /**
     * @brief dList 经过筛选完成的dpkg日志数据
     */
    /**
     * @brief dListOrigin 未经过筛选的dpkg日志数据  dpkg.log
     */
    QList<LOG_MSG_DPKG> dList, dListOrigin;
    /**
     * @brief xList 经过筛选完成的xorg日志数据
     */
    /**
     * @brief xListOrigin 未经过筛选的xorg日志数据   Xorg.0.log
     */
    QList<LOG_MSG_XORG> xList, xListOrigin;
    /**
     * @brief currentBootList 经过筛选完成的启动日志数据
     */
    /**
     * @brief bList 未经过筛选的启动日志数据   boot.log
     */
    QList<LOG_MSG_BOOT> bList, currentBootList;
    /**
     * @brief kList 经过筛选完成的内核日志数据
     */
    /**
     * @brief kListOrigin 未经过筛选的内核日志数据   kern.log
     */
    QList<LOG_MSG_JOURNAL> kList, kListOrigin;
    /**
     * @brief appList 经过筛选完成的内核日志数据
     */
    /**
     * @brief appListOrigin 未经过筛选的内核日志数据   ~/.cache/deepin/xxx.log(.xxx)
     */
    QList<LOG_MSG_APPLICATOIN> appList, appListOrigin;
    /**
     * @brief norList add 未经过筛选完成的开关机日志数据 by Airy
     */
    QList<LOG_MSG_NORMAL> norList;
    /**
     * @brief nortempList 经过筛选的开关机日志数据 add by Airy
     */
    QList<LOG_MSG_NORMAL> nortempList;
    /**
     * @brief m_currentKwinList add 经过筛选完成的kwin日志数据 by Airy /$HOME/.kwin.log
     */
    QList<LOG_MSG_KWIN> m_currentKwinList;
    /**
     * @brief m_kwinList 未经过筛选的开关机日志数据
     */
    QList<LOG_MSG_KWIN> m_kwinList;
    /**
     * @brief m_iconPrefix 图标资源文件路径前缀
     */
    QString m_iconPrefix = ICONPREFIX;
    /**
     * @brief m_icon_name_map 日志等级对应图标资源文件名的map
     */
    QMap<QString, QString> m_icon_name_map;
    //当前搜索关键字
    QString m_currentSearchStr{""};
    /**
     * @brief m_currentKwinFilter kwin日志当前筛选条件
     */
    KWIN_FILTERS m_currentKwinFilter;
    //导出进度条弹框
    ExportProgressDlg *m_exportDlg{nullptr};
    //是否为第一次加载完成后收到数据,journalctl获取方式专用
    bool m_firstLoadPageData = false;
    //启动日志当前筛选条件
    BOOT_FILTERS m_bootFilter = {"", ""};
    /**
     * @brief m_normalFilter 开关机日志当前筛选条件
     */
    NORMAL_FILTERS m_normalFilter ;
    //上次treeview滚筒条的值
    int m_treeViewLastScrollValue = -1;
    //当前的显示加载状态
    DisplayContent::LOAD_STATE m_state;
    //系统日志上次获取的时间
    QDateTime m_lastJournalGetTime{QDateTime::fromTime_t(0)};
    /**
     * @brief m_journalFilter 当前系统日志筛选条件
     */
    JOURNAL_FILTERS m_journalFilter;
    QList<LOG_MSG_DNF> dnfList, dnfListOrigin; //dnf.log
    QList<LOG_MSG_DMESG> dmesgList, dmesgListOrigin;//dmesg cmd
    QMap<QString, QString> m_dnfIconNameMap;
    DNFPRIORITY m_curDnfLevel {INFO};
    //当前系统日志获取进程标记量
    int m_journalCurrentIndex{-1};
    //当前klu启动日志获取进程标记量
    int m_journalBootCurrentIndex{-1};
    //当前启动日志获取进程标记量
    int m_bootCurrentIndex {-1};
    int m_dpkgCurrentIndex {-1};
    int m_kernCurrentIndex {-1};
    int m_normalCurrentIndex {-1};
    int m_xorgCurrentIndex {-1};
    int m_kwinCurrentIndex {-1};
    int m_appCurrentIndex {-1};
    bool m_isDataLoadComplete {false};
};

#endif  // DISPLAYCONTENT_H
