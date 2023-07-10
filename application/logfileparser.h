// SPDX-FileCopyrightText: 2019 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef LOGFILEPARSER_H
#define LOGFILEPARSER_H

#include "journalwork.h"
#include "journalbootwork.h"
#include "logapplicationparsethread.h"
#include "logauththread.h"
#include "dbusproxy/dldbushandler.h"
#include "dbusproxy/dldbusinterface.h"
#include "logoocfileparsethread.h"

#include <QMap>
#include <QThread>

class LogFileParser : public QObject
{
    Q_OBJECT
public:
    explicit LogFileParser(QWidget *parent = nullptr);
    ~LogFileParser();


    int parseByJournal(const QStringList &arg = QStringList());
    int parseByJournalBoot(const QStringList &arg = QStringList());

    int parseByDpkg(const DKPG_FILTERS &iDpkgFilter);
#if 0
    void parseByXlog(QStringList &xList);
    void parseByXlog(QList<LOG_MSG_XORG> &xList, qint64 ms = 0);  // modifed by Airy for show period
#endif
    int parseByXlog(const XORG_FILTERS &iXorgFilter);
    int parseByBoot();
    int parseByKern(const KERN_FILTERS &iKernFilter);
    int parseByApp(const APP_FILTERS &iAPPFilter);
    void parseByDnf(DNF_FILTERS iDnfFilter);
    void parseByDmesg(DMESG_FILTERS iDmesgFilter);
    int parseByNormal(const NORMAL_FILTERS &iNormalFiler);   // add by Airy

    int parseByKwin(const KWIN_FILTERS &iKwinfilter);
    int parseByOOC(const QString &path);

    int parseByAudit(const AUDIT_FILTERS &iAuditFilter);

    void createFile(const QString &output, int count);
    void stopAllLoad();

signals:
    void dpkgFinished(int index);
    void dpkgData(int index, QList<LOG_MSG_DPKG>);
    void xlogFinished(int index);
    void xlogData(int index, QList<LOG_MSG_XORG>);
    void bootFinished(int index);
    void bootData(int index, QList<LOG_MSG_BOOT>);
    void kernFinished(int index);
    void kernData(int index, QList<LOG_MSG_JOURNAL>);
    void dnfFinished(QList<LOG_MSG_DNF>);
    void dmesgFinished(QList<LOG_MSG_DMESG>);
    void journalFinished(int index);
    void journalBootFinished(int index);
    void journalData(int index, QList<LOG_MSG_JOURNAL>);
    void journaBootlData(int index, QList<LOG_MSG_JOURNAL>);

    //void normalFinished();  // add by Airy

    void normalFinished(int index);
    void normalData(int index, QList<LOG_MSG_NORMAL>);
    void kwinFinished(int index);
    void kwinData(int index, QList<LOG_MSG_KWIN> iKwinList);
    /**
     * @brief appFinished 获取数据结束信号
     */
    void appFinished(int index);
    void appData(int index, QList<LOG_MSG_APPLICATOIN> iDataList);
    void OOCFinished(int index, int error = 0);
    void OOCData(int index, const QString &data);

    void auditFinished(int index, bool bShowTip = false);
    void auditData(int index, QList<LOG_MSG_AUDIT>);

    void stopKern();
    void stopBoot();
    void stopDpkg();
    void stopXlog();
    void stopNormal();
    void stopKwin();
    void stopApp();
    void stopJournal();
    void stopJournalBoot();
    void stopDnf();
    void stopDmesg();
    void stopOOC();
    /**
     * @brief proccessError 获取日志文件失败错误信息传递信号，传递到主界面显示 DMessage tooltip
     * @param iError 错误字符
     */
    void proccessError(const QString &iError);

private:
    void quitLogAuththread(QThread *iThread);
signals:

public slots:
    void slog_proccessError(const QString &iError);
private:
    QString m_rootPasswd;

    QMap<QString, QString> m_dateDict;
    QMap<QString, int> m_levelDict;  // example:warning=>4

    LogOOCFileParseThread *m_OOCThread {nullptr};
    LogApplicationParseThread *m_appThread {nullptr};
    journalWork *work {nullptr};
    JournalBootWork *m_bootJournalWork{nullptr};
    QProcess *m_pDkpgDataLoader{nullptr};
    QProcess *m_pXlogDataLoader{nullptr};
    QProcess *m_KwinDataLoader{nullptr};
    bool m_isProcess = false;
    qint64 m_selectTime {0};
    bool m_isJournalLoading = false;
    bool m_isDpkgLoading = false;
    bool m_isXlogLoading = false;
    bool m_isKwinLoading = false;
    bool m_isBootLoading = false;
    bool m_isKernLoading = false;
    bool m_isAppLoading = false;
    bool m_isNormalLoading = false;
    bool m_isOOCLoading = false;
    bool m_isAuditLoading = false;
};

#endif  // LOGFILEPARSER_H
