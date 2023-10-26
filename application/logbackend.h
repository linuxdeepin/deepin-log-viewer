// SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef LOGBACKEND_H
#define LOGBACKEND_H

#include "structdef.h"

#include <QObject>

class LogFileParser;
class LogBackend : public QObject
{
    Q_OBJECT

public:
    static LogBackend *instance(QObject *parent = nullptr);
    ~LogBackend();
    explicit LogBackend(QObject *parent = nullptr);

    // 设置命令行当前工作目录
    void setCmdWorkDir(const QString &dirPath);

    // 导出全部日志到指定目录
    void exportAllLogs(const QString &outDir = "");

    // 按类型导出日志
    int exportTypeLogs(const QString &outDir, const QString &type = "");

    // 按条件导出日志
    // condition 可能为级别、事件类型、状态、审计类型等条件
    bool exportTypeLogsByCondition(const QString &outDir, const QString &type, const QString &period, const QString &condition = "", const QString &keyword = "");

    // 按应用导出日志
    int exportAppLogs(const QString &outDir, const QString &appName = "");

    // 按条件导出应用日志
    bool exportAppLogsByCondition(const QString &outDir, const QString& appName, const QString &period, const QString &level = "", const QString &keyword = "");

    // 获取当前系统支持的日志种类
    QStringList getLogTypes();

    QString getOutDirPath() const;

    static LOG_FLAG type2Flag(const QString &type, QString &error);

private:
    static LogBackend *m_staticbackend;

private slots:
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
    void slot_dnfFinished(const QList<LOG_MSG_DNF> &list);
    void slot_dmesgFinished(const QList<LOG_MSG_DMESG> &list);
    void slot_journalFinished(int index);
    void slot_journalBootFinished(int index);
    void slot_journalBootData(int index, QList<LOG_MSG_JOURNAL> list);
    void slot_journalData(int index, QList<LOG_MSG_JOURNAL> list);
    void slot_applicationFinished(int index);
    void slot_applicationData(int index, QList<LOG_MSG_APPLICATOIN> list);
    void slot_normalFinished(int index);
    void slot_normalData(int index, QList<LOG_MSG_NORMAL> list);
    void slot_auditFinished(int index, bool bShowTip = false);
    void slot_auditData(int index, QList<LOG_MSG_AUDIT> list);
    void slot_coredumpFinished(int index);
    void slot_coredumpData(int index, QList<LOG_MSG_COREDUMP> list);

    void slot_logLoadFailed(const QString &iError);

    void onExportProgress(int nCur, int nTotal);
    void onExportResult(bool isSuccess);

    QList<LOG_MSG_BOOT> filterBoot(BOOT_FILTERS ibootFilter, const QList<LOG_MSG_BOOT> &iList);
    QList<LOG_MSG_NORMAL> filterNomal(NORMAL_FILTERS inormalFilter, QList<LOG_MSG_NORMAL> &iList);
    QList<LOG_MSG_DPKG> filterDpkg(const QString &iSearchStr, const QList<LOG_MSG_DPKG> &iList);
    QList<LOG_MSG_JOURNAL> filterKern(const QString &iSearchStr, const QList<LOG_MSG_JOURNAL> &iList);
    QList<LOG_MSG_XORG> filterXorg(const QString &iSearchStr, const QList<LOG_MSG_XORG> &iList);
    QList<LOG_MSG_KWIN> filterKwin(const QString &iSearchStr, const QList<LOG_MSG_KWIN> &iList);
    QList<LOG_MSG_APPLICATOIN> filterApp(const QString &iSearchStr, const QList<LOG_MSG_APPLICATOIN> &iList);
    QList<LOG_MSG_DNF> filterDnf(const QString &iSearchStr, const QList<LOG_MSG_DNF> &iList);
    QList<LOG_MSG_DMESG> filterDmesg(const QString &iSearchStr, const QList<LOG_MSG_DMESG> &iList);
    QList<LOG_MSG_JOURNAL> filterJournal(const QString &iSearchStr, const QList<LOG_MSG_JOURNAL> &iList);
    QList<LOG_MSG_JOURNAL> filterJournalBoot(const QString &iSearchStr, const QList<LOG_MSG_JOURNAL> &iList);
    QList<LOG_MSG_AUDIT> filterAudit(AUDIT_FILTERS auditFilter, const QList<LOG_MSG_AUDIT> &iList);
    QList<LOG_MSG_COREDUMP> filterCoredump(const QString &iSearchStr, const QList<LOG_MSG_COREDUMP> &iList);

private:
    void initParser();

    bool parseData(const LOG_FLAG &flag, const QString &period, const QString &condition);

    void exportData();

    void resetCategoryOutputPath(const QString & path);
    bool getOutDirPath(const QString &path);
    BUTTONID period2Enum(const QString &period);
    int level2Id(const QString &level);
    DNFPRIORITY dnfLevel2Id(const QString &level);
    int normal2eventType(const QString &eventType);
    int audit2eventType(const QString &eventType);
    TIME_RANGE getTimeRange(const BUTTONID& periodId);
    QString getApplogPath(const QString &appName);

private:
    QStringList m_logTypes;
    bool m_newDir {false};
    QString m_outPath {""};
    QString m_cmdWorkDir {""};

private:

    /**
     * @brief m_curAppLog 当前选中的应用的日志文件路径
     */
    QString m_curAppLog {""};

    //当前解析的日志类型
    LOG_FLAG m_flag {NONE};

    /**
     * @brief jBootList 经过筛选完成的启动日志列表
     */
    /**
     * @brief jBootListOrigin 未经过筛选的启动日志数据 journalctl --boot cmd.
     */
    QList<LOG_MSG_JOURNAL> jBootList, jBootListOrigin;

    // System log data
    QList<LOG_MSG_JOURNAL> jList, jListOrigin;
    // Dmesg log data
    QList<LOG_MSG_DMESG> dmesgList, dmesgListOrigin;

    QList<LOG_MSG_DNF> dnfList, dnfListOrigin; //dnf.log
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
     * @brief aListOrigin 未经过筛选的审计日志数据   audit/audit.log
     */
    QList<LOG_MSG_AUDIT> aList, aListOrigin;

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


    QList<LOG_MSG_COREDUMP> m_coredumpList;
    QList<LOG_MSG_COREDUMP> m_currentCoredumpList;

    //当前搜索关键字
    QString m_currentSearchStr {""};

    /**
     * @brief m_journalFilter 当前系统日志筛选条件
     */
    JOURNAL_FILTERS m_journalFilter;
    /**
     * @brief m_currentKwinFilter kwin日志当前筛选条件
     */
    KWIN_FILTERS m_currentKwinFilter;
    /**
     * @brief m_normalFilter 开关机日志当前筛选条件
     */
    NORMAL_FILTERS m_normalFilter;
    //启动日志当前筛选条件
    BOOT_FILTERS m_bootFilter = {"", ""};
    /**
     * @brief m_auditFilter 当前审计日志筛选条件
     */
    AUDIT_FILTERS m_auditFilter;

    //当前系统日志获取进程标记量
    int m_journalCurrentIndex {-1};
    //当前klu启动日志获取进程标记量
    int m_journalBootCurrentIndex {-1};
    //当前启动日志获取进程标记量
    int m_bootCurrentIndex {-1};
    int m_dpkgCurrentIndex {-1};
    int m_kernCurrentIndex {-1};
    int m_normalCurrentIndex {-1};
    int m_xorgCurrentIndex {-1};
    int m_kwinCurrentIndex {-1};
    int m_appCurrentIndex {-1};
    int m_OOCCurrentIndex {-1};
    int m_auditCurrentIndex {-1};
    int m_coredumpCurrentIndex {-1};

    bool m_isDataLoadComplete {false};
    bool m_bNeedExport {false};

private:
    LogFileParser *m_pParser {nullptr};
};

#endif // LOGBACKEND_H
