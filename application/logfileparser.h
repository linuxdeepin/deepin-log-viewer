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

#ifndef LOGFILEPARSER_H
#define LOGFILEPARSER_H

#include "journalwork.h"
#include "journalbootwork.h"
#include "logapplicationparsethread.h"
#include "logauththread.h"
#include "dbusproxy/dldbushandler.h"
#include "dbusproxy/dldbusinterface.h"

#include <QMap>
#include <QThread>

class LogFileParser : public QObject
{
    Q_OBJECT
public:
    explicit LogFileParser(QWidget *parent = nullptr);
    ~LogFileParser();


    int parseByJournal(QStringList arg = QStringList());
    int parseByJournalBoot(QStringList arg = QStringList());

    int parseByDpkg(DKPG_FILTERS &iDpkgFilter);
#if 0
    void parseByXlog(QStringList &xList);
    void parseByXlog(QList<LOG_MSG_XORG> &xList, qint64 ms = 0);  // modifed by Airy for show period
#endif
    int parseByXlog(XORG_FILTERS &iXorgFilter);
    int parseByBoot();
    int parseByKern(KERN_FILTERS &iKernFilter);
    int parseByApp(APP_FILTERS &iAPPFilter);

    void parseByNormal1(QList<LOG_MSG_NORMAL> &nList, NORMAL_FILTERS &iNormalFiler);  // add by Airy
    int parseByNormal(NORMAL_FILTERS &iNormalFiler);   // add by Airy

    int parseByKwin(KWIN_FILTERS iKwinfilter);
    void createFile(QString output, int count);
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
    void stopKern();
    void stopBoot();
    void stopDpkg();
    void stopXlog();
    void stopNormal();
    void stopKwin();
    void stopApp();
    void stopJournal();
    void stopJournalBoot();
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
};

#endif  // LOGFILEPARSER_H
