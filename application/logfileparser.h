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
#include "structdef.h"

#include <QMap>
#include <QObject>
#include <QThread>

class LogFileParser : public QObject
{
    Q_OBJECT
public:
    explicit LogFileParser(QWidget *parent = nullptr);
    ~LogFileParser();


    int parseByJournal(QStringList arg = QStringList());
    int parseByJournalBoot(QStringList arg = QStringList());

    void parseByDpkg(DKPG_FILTERS &iDpkgFilter);
#if 0
    void parseByXlog(QStringList &xList);
    void parseByXlog(QList<LOG_MSG_XORG> &xList, qint64 ms = 0);  // modifed by Airy for show period
#endif
    void parseByXlog(XORG_FILTERS &iXorgFilter);
    void parseByBoot();
    void parseByKern(KERN_FILTERS &iKernFilter);
    void parseByApp(APP_FILTERS &iAPPFilter);
    void parseByDnf(DNF_FILTERS iDnfFilter);
    void parseByDmesg(DMESG_FILTERS iDmesgFilter);

    void parseByNormal(QList<LOG_MSG_NORMAL> &nList, NORMAL_FILTERS &iNormalFiler);  // add by Airy
    void parseByKwin(KWIN_FILTERS iKwinfilter);
    void createFile(QString output, int count);
    void stopAllLoad();

signals:
    void dpkgFinished(QList<LOG_MSG_DPKG>);
    void xlogFinished(QList<LOG_MSG_XORG>);
    void bootFinished(QList<LOG_MSG_BOOT>);
    void kernFinished(QList<LOG_MSG_JOURNAL>);
    void dnfFinished(QList<LOG_MSG_DNF>);
    void dmesgFinished(QList<LOG_MSG_DMESG>);

    void journalFinished();

    void journalBootFinished();

    void journalData(int index, QList<LOG_MSG_JOURNAL>);
    void journaBootlData(int index, QList<LOG_MSG_JOURNAL>);

    void applicationFinished(QList<LOG_MSG_APPLICATOIN>);
    void normalFinished();  // add by Airy
    void kwinFinished(QList<LOG_MSG_KWIN> iKwinList);
    void stopKern();
    void stopBoot();
    void stopDpkg();
    void stopXlog();
    void stopKwin();
    void stopApp();
    void stopJournal();
    void stopJournalBoot();
    void stopDnf();
    void stopDmesg();
    /**
     * @brief proccessError 获取日志文件失败错误信息传递信号，传递到主界面显示 DMessage tooltip
     * @param iError 错误字符
     */
    void proccessError(const QString &iError);

private:
    void quitLogAuththread(QThread *iThread);
signals:

public slots:
    void slot_journalFinished();
    void slot_journalBootFinished();
    void slot_applicationFinished(QList<LOG_MSG_APPLICATOIN> iAppList);
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
