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
#include "logfileparser.h"
#include "logiconbutton.h"
#include "logsearchnoresultwidget.h"
#include "logspinnerwidget.h"
#include "logtreeview.h"
#include "structdef.h"

class DisplayContent : public Dtk::Widget::DWidget
{
    Q_OBJECT
public:
    explicit DisplayContent(QWidget *parent = nullptr);
    ~DisplayContent();

private:
    void initUI();
    void initMap();
    void cleanText();
    void initTableView();
    void setTableViewData();
    void initConnections();

    void generateJournalFile(int id, int lId);
    void createJournalTable(QList<LOG_MSG_JOURNAL> &list);

    void generateDpkgFile(int id);
    void createDpkgTable(QList<LOG_MSG_DPKG> &list);

    void generateKernFile(int id);
    void createKernTable(QList<LOG_MSG_JOURNAL> &list);

    void generateAppFile(QString path, int id, int lId);
    void createAppTable(QList<LOG_MSG_APPLICATOIN> &list);

    void createBootTable(QList<LOG_MSG_BOOT> &list);

    void createXorgTable(QStringList &list);

    void insertJournalTable(QList<LOG_MSG_JOURNAL> logList, int start, int end);

    void fillDetailInfo(QString deamonName, QString usrName, QString pid, QString dateTime,
                        QModelIndex level, QString msg, QString status = "");

    QString getAppName(QString filePath);

    bool isAuthProcessAlive();

signals:
    void loadMoreInfo();

public slots:
    void slot_tableItemClicked(const QModelIndex &index);
    void slot_BtnSelected(int btnId, int lId, QModelIndex idx);
    void slot_appLogs(QString path);

    void slot_logCatelogueClicked(const QModelIndex &index);
    void slot_exportClicked();

    void slot_statusChagned(QString status);

    void slot_dpkgFinished();
    void slot_XorgFinished();
    void slot_bootFinished();
    void slot_kernFinished();
    void slot_journalFinished(QList<LOG_MSG_JOURNAL> logList);
    void slot_applicationFinished();

    void slot_vScrollValueChanged(int value);

    void slot_searchResult(QString str);

    void slot_themeChanged(Dtk::Widget::DApplicationHelper::ColorType colorType);

private:
    void paintEvent(QPaintEvent *event);

private:
    //    Dtk::Widget::DTableView *m_tableView;
    LogTreeView *m_treeView;
    QStandardItemModel *m_pModel;
    Dtk::Widget::DLabel *m_daemonName, *m_dateTime, *m_userName, *m_pid, *m_status;
    LogIconButton *m_level;
    Dtk::Widget::DLabel *m_userLabel, *m_pidLabel, *m_statusLabel;
    Dtk::Widget::DTextBrowser *m_textBrowser;
    Dtk::Widget::DLabel *noResultLabel {nullptr};
    QModelIndex m_curListIdx;
    QMap<QString, QString> m_transDict;
    int m_limitTag {0};

    LogSearchNoResultWidget *m_noResultWdg;

    LogSpinnerWidget *m_spinnerWgt;

    QString m_curAppLog;
    QString m_currentStatus;

    int m_curBtnId {ALL};
    int m_curLvId {INF};

    LOG_FLAG m_flag {NONE};

    LogFileParser m_logFileParse;
    QList<LOG_MSG_JOURNAL> jList;                // journalctl cmd.
    QList<LOG_MSG_DPKG> dList;                   // dpkg.log
    QStringList xList;                           // Xorg.0.log
    QList<LOG_MSG_BOOT> bList, currentBootList;  // boot.log
    QList<LOG_MSG_JOURNAL> kList;                // kern.log
    QList<LOG_MSG_APPLICATOIN> appList;          //~/.cache/deepin/xxx.log(.xxx)

    QString m_iconPrefix;
    QMap<QString, QString> m_icon_name_map;
    QString getIconByname(QString str);
};

#endif  // DISPLAYCONTENT_H
