#ifndef DISPLAYCONTENT_H
#define DISPLAYCONTENT_H

#include <DLabel>
#include <DTableView>
#include <DTextBrowser>
#include <QStandardItemModel>
#include <QWidget>
#include "logfileparser.h"
#include "logsearchnoresultwidget.h"
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
                        QString level, QString msg, QString status = "");
signals:
    void loadMoreInfo();

public slots:
    void slot_tableItemClicked(const QModelIndex &index);
    void slot_BtnSelected(int btnId, int lId, QModelIndex idx);
    void slot_appLogs(QString path);

    void slot_treeClicked(const QModelIndex &index);
    void slot_exportClicked();

    void slot_dpkgFinished();
    void slot_XorgFinished();
    void slot_bootFinished();
    void slot_kernFinished();
    void slot_journalFinished(QList<LOG_MSG_JOURNAL> logList);
    void slot_applicationFinished();

    void slot_vScrollValueChanged(int value);

    void slot_searchResult(QString str);

private:
    void paintEvent(QPaintEvent *event);

private:
    Dtk::Widget::DTableView *m_tableView;
    //    LogTableView *m_tableView;
    QStandardItemModel *m_pModel;
    Dtk::Widget::DLabel *m_daemonName, *m_dateTime, *m_userName, *m_pid, *m_level, *m_status;
    Dtk::Widget::DLabel *m_userLabel, *m_pidLabel, *m_statusLabel;
    Dtk::Widget::DTextBrowser *m_textBrowser;
    Dtk::Widget::DLabel *noResultLabel {nullptr};
    QModelIndex curIdx;
    QMap<QString, QString> m_transDict;
    int m_limitTag {0};

    LogSearchNoResultWidget *m_noResultWdg;

    QString m_curAppLog;

    int m_curBtnId {ALL};
    int m_curLvId {INF};

    LOG_FLAG m_flag {NONE};

    LogFileParser m_logFileParse;
    QList<LOG_MSG_JOURNAL> jList;        // journalctl cmd.
    QList<LOG_MSG_DPKG> dList;           // dpkg.log
    QStringList xList;                   // Xorg.0.log
    QList<LOG_MSG_BOOT> bList;           // boot.log
    QList<LOG_MSG_JOURNAL> kList;        // kern.log
    QList<LOG_MSG_APPLICATOIN> appList;  //~/.cache/deepin/xxx.log(.xxx)
};

#endif  // DISPLAYCONTENT_H
