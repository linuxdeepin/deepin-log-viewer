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

#include <QMap>
#include <QObject>
#include <QThread>
#include "journalwork.h"
#include "logapplicationparsethread.h"
#include "logauththread.h"
#include "structdef.h"

class LogFileParser : public QObject
{
    Q_OBJECT
public:
    explicit LogFileParser(QWidget *parent = nullptr);
    ~LogFileParser();

    void parseByJournal(QStringList arg = QStringList());
    void parseByDpkg(QList<LOG_MSG_DPKG> &dList, qint64 ms = 0);
#if 0
    void parseByXlog(QStringList &xList);
#endif
    void parseByXlog(QList<LOG_MSG_XORG> &xList, qint64 ms = 0);  // modifed by Airy for show period
    void parseByBoot();
    void parseByKern(qint64 ms = 0);
    void parseByApp(QString path, int lv = 6, qint64 ms = 0);

    void parseByNormal(QList<LOG_MSG_NORMAL> &nList, qint64 ms = 0);  // add by Airy
    void parseByKwin(KWIN_FILTERS iKwinfilter);
    void createFile(QString output, int count);
    void stopAllLoad();

signals:
    void dpkgFinished();
    void xlogFinished();
    void bootFinished(QList<LOG_MSG_BOOT>);
    void kernFinished(QList<LOG_MSG_JOURNAL>);
    void journalFinished();
    void applicationFinished(QList<LOG_MSG_APPLICATOIN>);
    void normalFinished();  // add by Airy
    void kwinFinished(QList<LOG_MSG_KWIN> iKwinList);


private:
    bool isErroCommand(QString str);
    qint64 formatDateTime(QString m, QString d, QString t);
signals:

public slots:
    void slot_journalFinished();
    void slot_applicationFinished(QList<LOG_MSG_APPLICATOIN> iAppList);
    void slot_threadFinished(LOG_FLAG flag, QString output);
private:
    QString m_rootPasswd;

    QMap<QString, QString> m_dateDict;
    QMap<QString, int> m_levelDict;  // example:warning=>4

    LogAuthThread *m_authThread {nullptr};
    LogApplicationParseThread *m_appThread {nullptr};
    journalWork *work {nullptr};
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
