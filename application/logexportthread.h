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

#ifndef LOGEXPORTTHREAD_H
#define LOGEXPORTTHREAD_H
#include "structdef.h"

#include <QRunnable>
#include <QObject>
#include <QStandardItemModel>

class LogExportThread : public QObject, public QRunnable
{
    Q_OBJECT
public:
    LogExportThread(QObject *parent = nullptr);
    enum RUN_MODE {
        HtmlModel,
        HtmlJOURNAL,
        HtmlAPP,
        HtmlDPKG,
        HtmlBOOT,
        HtmlXORG,
        HtmlNORMAL,
        HtmlKWIN,
        TxtModel,
        TxtJOURNAL,
        TxtAPP,
        TxtDPKG,
        TxtBOOT,
        TxtXORG,
        TxtNORMAL,
        TxtKWIN,
        DocModel,
        DocJOURNAL,
        DocAPP,
        DocDPKG,
        DocBOOT,
        DocXORG,
        DocNORMAL,
        DocKWIN,
        XlsModel,
        XlsJOURNAL,
        XlsAPP,
        XlsDPKG,
        XlsBOOT,
        XlsXORG,
        XlsNORMAL,
        XlsKWIN,
        NoneExportType = 9999
    };

    void exportToTxtPublic(QString fileName, QStandardItemModel *pModel, LOG_FLAG flag);
    void exportToTxtPublic(QString fileName, QList<LOG_MSG_JOURNAL> jList,  QStringList labels, LOG_FLAG flag);
    void exportToTxtPublic(QString fileName, QList<LOG_MSG_APPLICATOIN> jList, QStringList labels, QString &iAppName);
    void exportToTxtPublic(QString fileName, QList<LOG_MSG_DPKG> jList, QStringList labels);
    void exportToTxtPublic(QString fileName, QList<LOG_MSG_BOOT> jList, QStringList labels);
    void exportToTxtPublic(QString fileName, QList<LOG_MSG_XORG> jList, QStringList labels);
    void exportToTxtPublic(QString fileName, QList<LOG_MSG_NORMAL> jList, QStringList labels);
    void exportToTxtPublic(QString fileName, QList<LOG_MSG_KWIN> jList, QStringList labels);

    void exportToHtmlPublic(QString fileName, QStandardItemModel *pModel, LOG_FLAG flag);
    void exportToHtmlPublic(QString fileName, QList<LOG_MSG_JOURNAL> jList,  QStringList labels, LOG_FLAG flag);
    void exportToHtmlPublic(QString fileName, QList<LOG_MSG_APPLICATOIN> jList, QStringList labels, QString &iAppName);
    void exportToHtmlPublic(QString fileName, QList<LOG_MSG_DPKG> jList, QStringList labels);
    void exportToHtmlPublic(QString fileName, QList<LOG_MSG_BOOT> jList, QStringList labels);
    void exportToHtmlPublic(QString fileName, QList<LOG_MSG_XORG> jList, QStringList labels);
    void exportToHtmlPublic(QString fileName, QList<LOG_MSG_NORMAL> jList, QStringList labels);
    void exportToHtmlPublic(QString fileName, QList<LOG_MSG_KWIN> jList, QStringList labels);


    void exportToDocPublic(QString fileName, QStandardItemModel *pModel, LOG_FLAG flag);
    void exportToDocPublic(QString fileName, QList<LOG_MSG_JOURNAL> jList, QStringList labels, LOG_FLAG iFlag);
    void exportToDocPublic(QString fileName, QList<LOG_MSG_APPLICATOIN> jList, QStringList labels, QString &iAppName);
    void exportToDocPublic(QString fileName, QList<LOG_MSG_DPKG> jList, QStringList labels);
    void exportToDocPublic(QString fileName, QList<LOG_MSG_BOOT> jList, QStringList labels);
    void exportToDocPublic(QString fileName, QList<LOG_MSG_XORG> jList, QStringList labels);
    void exportToDocPublic(QString fileName, QList<LOG_MSG_NORMAL> jList, QStringList labels);
    void exportToDocPublic(QString fileName, QList<LOG_MSG_KWIN> jList, QStringList labels);

    void exportToXlsPublic(QString fileName, QStandardItemModel *pModel, LOG_FLAG flag);
    void exportToXlsPublic(QString fileName, QList<LOG_MSG_JOURNAL> jList, QStringList labels, LOG_FLAG iFlag);
    void exportToXlsPublic(QString fileName, QList<LOG_MSG_APPLICATOIN> jList, QStringList labels, QString &iAppName);
    void exportToXlsPublic(QString fileName, QList<LOG_MSG_DPKG> jList, QStringList labels);
    void exportToXlsPublic(QString fileName, QList<LOG_MSG_BOOT> jList, QStringList labels);
    void exportToXlsPublic(QString fileName, QList<LOG_MSG_XORG> jList, QStringList labels);
    void exportToXlsPublic(QString fileName, QList<LOG_MSG_NORMAL> jList, QStringList labels);
    void exportToXlsPublic(QString fileName, QList<LOG_MSG_KWIN> jList, QStringList labels);

    bool isProcessing();
public slots:
    // 停止线程
    void stopImmediately();
protected:
    void run() override;
signals:
    void sigProgress(int nCur, int nTotal);
    void sigResult(bool isSuccess);
    void sigError(QString iError);
private:
    bool exportToTxt(QString fileName, QStandardItemModel *pModel, LOG_FLAG flag);
    bool exportToTxt(QString fileName, QList<LOG_MSG_JOURNAL> jList,  QStringList labels, LOG_FLAG flag);
    bool exportToTxt(QString fileName, QList<LOG_MSG_APPLICATOIN> jList, QStringList labels, QString &iAppName);
    bool exportToTxt(QString fileName, QList<LOG_MSG_DPKG> jList, QStringList labels);
    bool exportToTxt(QString fileName, QList<LOG_MSG_BOOT> jList, QStringList labels);
    bool exportToTxt(QString fileName, QList<LOG_MSG_XORG> jList, QStringList labels);
    bool exportToTxt(QString fileName, QList<LOG_MSG_NORMAL> jList, QStringList labels);
    bool exportToTxt(QString fileName, QList<LOG_MSG_KWIN> jList, QStringList labels);

    bool exportToDoc(QString fileName, QStandardItemModel *pModel, LOG_FLAG flag);
    bool exportToDoc(QString fileName, QList<LOG_MSG_JOURNAL> jList, QStringList labels, LOG_FLAG iFlag);
    bool exportToDoc(QString fileName, QList<LOG_MSG_APPLICATOIN> jList, QStringList labels, QString &iAppName);
    bool exportToDoc(QString fileName, QList<LOG_MSG_DPKG> jList, QStringList labels);
    bool exportToDoc(QString fileName, QList<LOG_MSG_BOOT> jList, QStringList labels);
    bool exportToDoc(QString fileName, QList<LOG_MSG_XORG> jList, QStringList labels);
    bool exportToDoc(QString fileName, QList<LOG_MSG_NORMAL> jList, QStringList labels);
    bool exportToDoc(QString fileName, QList<LOG_MSG_KWIN> jList, QStringList labels);

    bool exportToHtml(QString fileName, QStandardItemModel *pModel, LOG_FLAG flag);
    bool exportToHtml(QString fileName, QList<LOG_MSG_JOURNAL> jList,  QStringList labels, LOG_FLAG flag);
    bool exportToHtml(QString fileName, QList<LOG_MSG_APPLICATOIN> jList, QStringList labels, QString &iAppName);
    bool exportToHtml(QString fileName, QList<LOG_MSG_DPKG> jList, QStringList labels);
    bool exportToHtml(QString fileName, QList<LOG_MSG_BOOT> jList, QStringList labels);
    bool exportToHtml(QString fileName, QList<LOG_MSG_XORG> jList, QStringList labels);
    bool exportToHtml(QString fileName, QList<LOG_MSG_NORMAL> jList, QStringList labels);
    bool exportToHtml(QString fileName, QList<LOG_MSG_KWIN> jList, QStringList labels);

    bool exportToXls(QString fileName, QStandardItemModel *pModel, LOG_FLAG flag);
    bool exportToXls(QString fileName, QList<LOG_MSG_JOURNAL> jList, QStringList labels, LOG_FLAG iFlag);
    bool exportToXls(QString fileName, QList<LOG_MSG_APPLICATOIN> jList, QStringList labels, QString &iAppName);
    bool exportToXls(QString fileName, QList<LOG_MSG_DPKG> jList, QStringList labels);
    bool exportToXls(QString fileName, QList<LOG_MSG_BOOT> jList, QStringList labels);
    bool exportToXls(QString fileName, QList<LOG_MSG_XORG> jList, QStringList labels);
    bool exportToXls(QString fileName, QList<LOG_MSG_NORMAL> jList, QStringList labels);
    bool exportToXls(QString fileName, QList<LOG_MSG_KWIN> jList, QStringList labels);
    void initMap();
    QString strTranslate(QString &iLevelStr);
private:
    QString m_fileName = "";
    QStandardItemModel *m_pModel = nullptr;
    LOG_FLAG m_flag = NONE;
    QStringList m_labels;
    QList<LOG_MSG_JOURNAL> m_jList;
    QList<LOG_MSG_APPLICATOIN> m_appList;
    QList<LOG_MSG_DPKG> m_dpkgList;
    QList<LOG_MSG_BOOT> m_bootList;
    QList<LOG_MSG_XORG> m_xorgList;
    QList<LOG_MSG_NORMAL> m_normalList;
    QList<LOG_MSG_KWIN> m_kwinList;
    RUN_MODE m_runMode = NoneExportType;
    bool m_canRunning = false;
    QString openErroStr = "export open file error";
    QString stopStr = "stop export";
    QString m_appName = "";
    QMap<QString, QString> m_levelStrMap;


};

#endif  // LOGEXPORTTHREAD_H
