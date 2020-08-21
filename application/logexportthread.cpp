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

#include "logexportthread.h"
#include "document.h"
#include "table.h"
#include "xlsxdocument.h"

#include <DApplication>

#include <QDebug>
#include <QFile>
#include <QTextCodec>
#include <QTextStream>
#include <QTextDocument>
#include <QTextDocumentWriter>

DWIDGET_USE_NAMESPACE

LogExportThread::LogExportThread(QObject *parent)
    :  QObject(parent),
       QRunnable()
{
    setAutoDelete(true);
    initMap();
}

void LogExportThread::exportToTxtPublic(QString fileName, QStandardItemModel *pModel, LOG_FLAG flag)
{
    m_fileName = fileName;
    m_pModel = pModel;
    m_flag = flag;
    m_runMode = TxtModel;
    m_canRunning = true;
}

void LogExportThread::exportToTxtPublic(QString fileName, QList<LOG_MSG_JOURNAL> jList,  QStringList labels, LOG_FLAG flag)
{
    m_fileName = fileName;
    m_jList = jList;
    m_runMode = TxtJOURNAL;
    m_labels = labels;
    m_flag = flag;
    m_canRunning = true;

}

void LogExportThread::exportToTxtPublic(QString fileName, QList<LOG_MSG_APPLICATOIN> jList, QStringList labels, QString &iAppName)
{
    m_fileName = fileName;
    m_appList = jList;
    m_labels = labels;
    m_runMode = TxtAPP;
    m_canRunning = true;
    m_appName = iAppName;
}

void LogExportThread::exportToTxtPublic(QString fileName, QList<LOG_MSG_DPKG> jList, QStringList labels)
{
    m_fileName = fileName;
    m_dpkgList = jList;
    m_labels = labels;
    m_runMode = TxtDPKG;
    m_canRunning = true;
}

void LogExportThread::exportToTxtPublic(QString fileName, QList<LOG_MSG_BOOT> jList, QStringList labels)
{
    m_fileName = fileName;
    m_bootList = jList;
    m_labels = labels;
    m_runMode = TxtBOOT;
    m_canRunning = true;
}

void LogExportThread::exportToTxtPublic(QString fileName, QList<LOG_MSG_XORG> jList, QStringList labels)
{
    m_fileName = fileName;
    m_xorgList = jList;
    m_labels = labels;
    m_runMode = TxtXORG;
    m_canRunning = true;
}

void LogExportThread::exportToTxtPublic(QString fileName, QList<LOG_MSG_NORMAL> jList, QStringList labels)
{
    m_fileName = fileName;
    m_normalList = jList;
    m_labels = labels;
    m_runMode = TxtNORMAL;
    m_canRunning = true;
}

void LogExportThread::exportToTxtPublic(QString fileName, QList<LOG_MSG_KWIN> jList, QStringList labels)
{
    m_fileName = fileName;
    m_kwinList = jList;
    m_labels = labels;
    m_runMode = TxtKWIN;
    m_canRunning = true;
}

void LogExportThread::exportToTxtPublic(QString fileName, QList<LOG_MSG_DNF> jList, QStringList labels)
{
    m_fileName = fileName;
    m_dnfList = jList;
    m_labels = labels;
    m_runMode = TxtDNF;
    m_canRunning = true;

}

void LogExportThread::exportToTxtPublic(QString fileName, QList<LOG_MSG_DMESG> jList, QStringList labels)
{
    m_fileName = fileName;
    m_dmesgList = jList;
    m_labels = labels;
    m_runMode = TxtDMESG;
    m_canRunning = true;
}

void LogExportThread::exportToHtmlPublic(QString fileName, QStandardItemModel *pModel, LOG_FLAG flag)
{
    m_fileName = fileName;
    m_pModel = pModel;
    m_flag = flag;
    m_runMode = HtmlModel;
    m_canRunning = true;

}

void LogExportThread::exportToHtmlPublic(QString fileName, QList<LOG_MSG_JOURNAL> jList, QStringList labels, LOG_FLAG flag)
{
    m_fileName = fileName;
    m_jList = jList;
    m_runMode = HtmlJOURNAL;
    m_labels = labels;
    m_flag = flag;
    m_canRunning = true;

}

void LogExportThread::exportToHtmlPublic(QString fileName, QList<LOG_MSG_APPLICATOIN> jList, QStringList labels, QString &iAppName)
{
    m_fileName = fileName;
    m_appList = jList;
    m_labels = labels;
    m_runMode = HtmlAPP;
    m_canRunning = true;
    m_appName = iAppName;
}

void LogExportThread::exportToHtmlPublic(QString fileName, QList<LOG_MSG_DPKG> jList, QStringList labels)
{
    m_fileName = fileName;
    m_dpkgList = jList;
    m_labels = labels;
    m_runMode = HtmlDPKG;
    m_canRunning = true;
}

void LogExportThread::exportToHtmlPublic(QString fileName, QList<LOG_MSG_BOOT> jList, QStringList labels)
{
    m_fileName = fileName;
    m_bootList = jList;
    m_labels = labels;
    m_runMode = HtmlBOOT;
    m_canRunning = true;
}

void LogExportThread::exportToHtmlPublic(QString fileName, QList<LOG_MSG_XORG> jList, QStringList labels)
{
    m_fileName = fileName;
    m_xorgList = jList;
    m_labels = labels;
    m_runMode = HtmlXORG;
    m_canRunning = true;
}

void LogExportThread::exportToHtmlPublic(QString fileName, QList<LOG_MSG_NORMAL> jList, QStringList labels)
{
    m_fileName = fileName;
    m_normalList = jList;
    m_labels = labels;
    m_runMode = HtmlNORMAL;
    m_canRunning = true;
}

void LogExportThread::exportToHtmlPublic(QString fileName, QList<LOG_MSG_KWIN> jList, QStringList labels)
{
    m_fileName = fileName;
    m_kwinList = jList;
    m_labels = labels;
    m_runMode = HtmlKWIN;
    m_canRunning = true;
}

void LogExportThread::exportToHtmlPublic(QString fileName, QList<LOG_MSG_DNF> jList, QStringList labels)
{
    m_fileName = fileName;
    m_dnfList = jList;
    m_labels = labels;
    m_runMode = HtmlDNF;
    m_canRunning = true;
}

void LogExportThread::exportToHtmlPublic(QString fileName, QList<LOG_MSG_DMESG> jList, QStringList labels)
{
    m_fileName = fileName;
    m_dmesgList = jList;
    m_labels = labels;
    m_runMode = HtmlDMESG;
    m_canRunning = true;
}

void LogExportThread::exportToDocPublic(QString fileName, QStandardItemModel *pModel, LOG_FLAG flag)
{
    m_fileName = fileName;
    m_pModel = pModel;
    m_flag = flag;
    m_runMode = DocModel;
    m_canRunning = true;

}

void LogExportThread::exportToDocPublic(QString fileName, QList<LOG_MSG_JOURNAL> jList, QStringList labels, LOG_FLAG iFlag)
{
    m_fileName = fileName;
    m_jList = jList;
    m_flag = iFlag;
    m_labels = labels;
    m_runMode = DocJOURNAL;
    m_canRunning = true;

}

void LogExportThread::exportToDocPublic(QString fileName, QList<LOG_MSG_APPLICATOIN> jList, QStringList labels, QString &iAppName)
{
    m_fileName = fileName;
    m_appList = jList;
    m_labels = labels;
    m_runMode = DocAPP;
    m_canRunning = true;
    m_appName = iAppName;
}

void LogExportThread::exportToDocPublic(QString fileName, QList<LOG_MSG_DPKG> jList, QStringList labels)
{
    m_fileName = fileName;
    m_dpkgList = jList;
    m_labels = labels;
    m_runMode = DocDPKG;
    m_canRunning = true;
}

void LogExportThread::exportToDocPublic(QString fileName, QList<LOG_MSG_BOOT> jList, QStringList labels)
{
    m_fileName = fileName;
    m_bootList = jList;
    m_labels = labels;
    m_runMode = DocBOOT;
    m_canRunning = true;
}

void LogExportThread::exportToDocPublic(QString fileName, QList<LOG_MSG_XORG> jList, QStringList labels)
{
    m_fileName = fileName;
    m_xorgList = jList;
    m_labels = labels;
    m_runMode = DocXORG;
    m_canRunning = true;
}

void LogExportThread::exportToDocPublic(QString fileName, QList<LOG_MSG_NORMAL> jList, QStringList labels)
{
    m_fileName = fileName;
    m_normalList = jList;
    m_labels = labels;
    m_runMode = DocNORMAL;
    m_canRunning = true;
}

void LogExportThread::exportToDocPublic(QString fileName, QList<LOG_MSG_KWIN> jList, QStringList labels)
{
    m_fileName = fileName;
    m_kwinList = jList;
    m_labels = labels;
    m_runMode = DocKWIN;
    m_canRunning = true;
}

void LogExportThread::exportToDocPublic(QString fileName, QList<LOG_MSG_DNF> jList, QStringList labels)
{
    m_fileName = fileName;
    m_dnfList = jList;
    m_labels = labels;
    m_runMode = DocDNF;
    m_canRunning = true;
}

void LogExportThread::exportToDocPublic(QString fileName, QList<LOG_MSG_DMESG> jList, QStringList labels)
{
    m_fileName = fileName;
    m_dmesgList = jList;
    m_labels = labels;
    m_runMode = DocDMESG;
    m_canRunning = true;
}

void LogExportThread::exportToXlsPublic(QString fileName, QStandardItemModel *pModel, LOG_FLAG flag)
{
    m_fileName = fileName;
    m_pModel = pModel;
    m_flag = flag;
    m_runMode = XlsModel;
    m_canRunning = true;
}

void LogExportThread::exportToXlsPublic(QString fileName, QList<LOG_MSG_JOURNAL> jList, QStringList labels, LOG_FLAG iFlag)
{
    m_fileName = fileName;
    m_jList = jList;
    m_flag = iFlag;
    m_labels = labels;
    m_runMode = XlsJOURNAL;
    m_canRunning = true;
}

void LogExportThread::exportToXlsPublic(QString fileName, QList<LOG_MSG_APPLICATOIN> jList, QStringList labels, QString &iAppName)
{
    m_fileName = fileName;
    m_appList = jList;
    m_labels = labels;
    m_runMode = XlsAPP;
    m_canRunning = true;
    m_appName = iAppName;
}

void LogExportThread::exportToXlsPublic(QString fileName, QList<LOG_MSG_DPKG> jList, QStringList labels)
{
    m_fileName = fileName;
    m_dpkgList = jList;
    m_labels = labels;
    m_runMode = XlsDPKG;
    m_canRunning = true;
}

void LogExportThread::exportToXlsPublic(QString fileName, QList<LOG_MSG_BOOT> jList, QStringList labels)
{
    m_fileName = fileName;
    m_bootList = jList;
    m_labels = labels;
    m_runMode = XlsBOOT;
    m_canRunning = true;
}

void LogExportThread::exportToXlsPublic(QString fileName, QList<LOG_MSG_XORG> jList, QStringList labels)
{
    m_fileName = fileName;
    m_xorgList = jList;
    m_labels = labels;
    m_runMode = XlsXORG;
    m_canRunning = true;
}

void LogExportThread::exportToXlsPublic(QString fileName, QList<LOG_MSG_NORMAL> jList, QStringList labels)
{
    m_fileName = fileName;
    m_normalList = jList;
    m_labels = labels;
    m_runMode = XlsNORMAL;
    m_canRunning = true;
}

void LogExportThread::exportToXlsPublic(QString fileName, QList<LOG_MSG_KWIN> jList, QStringList labels)
{
    m_fileName = fileName;
    m_kwinList = jList;
    m_labels = labels;
    m_runMode = XlsKWIN;
    m_canRunning = true;
}

void LogExportThread::exportToXlsPublic(QString fileName, QList<LOG_MSG_DNF> jList, QStringList labels)
{
    m_fileName = fileName;
    m_dnfList = jList;
    m_labels = labels;
    m_runMode = XlsDNF;
    m_canRunning = true;
}

void LogExportThread::exportToXlsPublic(QString fileName, QList<LOG_MSG_DMESG> jList, QStringList labels)
{
    m_fileName = fileName;
    m_dmesgList = jList;
    m_labels = labels;
    m_runMode = XlsDMESG;
    m_canRunning = true;
}

bool LogExportThread::isProcessing()
{
    return m_canRunning;
}

void LogExportThread::stopImmediately()
{
    m_canRunning = false;
}

bool LogExportThread::exportToTxt(QString fileName, QStandardItemModel *pModel, LOG_FLAG flag)
{
    QFile fi(fileName);
    if (!fi.open(QIODevice::WriteOnly)) {
        emit sigResult(false);
        emit sigError(openErroStr);
        return false;
    }
    try {
        QTextStream out(&fi);
        if (flag == APP) {
            for (int row = 0; row < pModel->rowCount(); ++row) {
                if (!m_canRunning) {
                    throw  QString(stopStr);
                }
                out << pModel->horizontalHeaderItem(0)->text() << ": "
                    << pModel->item(row, 0)->data(Qt::UserRole + 6).toString() << " ";
                for (int col = 1; col < pModel->columnCount(); ++col) {
                    out << pModel->horizontalHeaderItem(col)->text() << ": "
                        << pModel->item(row, col)->text() << " ";
                }
                out << "\n";
            }
        } else {
            for (int row = 0; row < pModel->rowCount(); row++) {
                if (!m_canRunning) {
                    throw  QString(stopStr);
                }
                for (int col = 0; col < pModel->columnCount(); col++) {
                    out << pModel->horizontalHeaderItem(col)->text() << ": "
                        << pModel->item(row, col)->text() << " ";
                }
                out << "\n";
            }
        }
    } catch (QString ErrorStr) {
        qDebug() << "Export Stop" << ErrorStr;
        fi.close();
        emit sigResult(false);
        if (ErrorStr != stopStr) {
            emit sigError(QString("export error: %1").arg(ErrorStr));
        }
        return false;
    }
    fi.close();
    emit sigResult(true);
    return true;
}

bool LogExportThread::exportToTxt(QString fileName, QList<LOG_MSG_JOURNAL> jList,  QStringList labels, LOG_FLAG flag)
{
    QFile fi(fileName);
    if (!fi.open(QIODevice::WriteOnly)) {
        emit sigResult(false);
        emit sigError(openErroStr);
        return false;
    }
    try {

        QTextStream out(&fi);
        if (flag == JOURNAL) {
            for (int i = 0; i < jList.count(); i++) {
                if (!m_canRunning) {
                    throw  QString(stopStr);
                }
                LOG_MSG_JOURNAL jMsg = jList.at(i);
                //        out << QString(DApplication::translate("Table", "Date and Time:")) <<
                //        jMsg.dateTime << " "; out << QString(DApplication::translate("Table", "User:")) <<
                //        jMsg.hostName << " "; out << QString(DApplication::translate("Table", "Daemon:"))
                //        << jMsg.daemonName << " "; out << QString(DApplication::translate("Table",
                //        "PID:")) << jMsg.daemonId << " "; out << QString(DApplication::translate("Table",
                //        "Level:")) << jMsg.level << " "; out << QString(DApplication::translate("Table",
                //        "Info:")) << jMsg.msg << " "; //delete bug
                out << QString(DApplication::translate("Table", "Level:")) << jMsg.level << " ";
                out << QString(DApplication::translate("Table", "Process:")) << jMsg.daemonName << " ";
                out << QString(DApplication::translate("Table", "Date and Time:")) << jMsg.dateTime << " ";
                if (jMsg.msg.isEmpty()) {
                    out << QString(DApplication::translate("Table", "Info:"))
                        << QString(DApplication::translate("Table", "Null")) << " ";  // modify for bug
                } else {
                    out << QString(DApplication::translate("Table", "Info:")) << jMsg.msg << " ";
                }
                out << QString(DApplication::translate("Table", "User:")) << jMsg.hostName << " ";
                out << QString(DApplication::translate("Table", "PID:")) << jMsg.daemonId << " ";
                out << "\n";
            }
        } else if (flag == KERN) {
            for (int i = 0; i < jList.count(); i++) {
                if (!m_canRunning) {
                    throw  QString(stopStr);
                }
                int col = 0;
                LOG_MSG_JOURNAL jMsg = jList.at(i);
                out << labels.value(col++, "") << ":" << jMsg.dateTime << " ";
                out << labels.value(col++, "") << ":" << jMsg.hostName << " ";
                out << labels.value(col++, "") << ":" << jMsg.daemonName << " ";
                out << labels.value(col++, "") << ":" << jMsg.msg << " ";
                out << "\n";
            }
        }
        out.setCodec(QTextCodec::codecForName("utf-8"));

    } catch (QString ErrorStr) {
        qDebug() << "Export Stop" << ErrorStr;
        fi.close();
        emit sigResult(false);
        if (ErrorStr != stopStr) {
            emit sigError(QString("export error: %1").arg(ErrorStr));
        }
        return false;
    }
    fi.close();
    emit sigResult(true);
    return true;
}

bool LogExportThread::exportToTxt(QString fileName, QList<LOG_MSG_APPLICATOIN> jList, QStringList labels, QString &iAppName)
{
    QFile fi(fileName);
    if (!fi.open(QIODevice::WriteOnly)) {
        emit sigResult(false);
        emit sigError(openErroStr);
        return false;
    }
    try {
        QTextStream out(&fi);
        out.setCodec(QTextCodec::codecForName("utf-8"));
        for (int i = 0; i < jList.count(); i++) {
            if (!m_canRunning) {
                throw  QString(stopStr);
            }
            int col = 0;
            LOG_MSG_APPLICATOIN jMsg = jList.at(i);
            out << labels.value(col++, "") << ":" << strTranslate(jMsg.level)  << " ";
            out << labels.value(col++, "") << ":" << jMsg.dateTime << " ";
            out << labels.value(col++, "") << ":" << iAppName << " ";
            out << labels.value(col++, "") << ":" << jMsg.msg << " ";
            out << "\n";
        }
    } catch (QString ErrorStr) {
        qDebug() << "Export Stop" << ErrorStr;
        fi.close();
        emit sigResult(false);
        if (ErrorStr != stopStr) {
            emit sigError(QString("export error: %1").arg(ErrorStr));
        }
        return false;
    }
    fi.close();
    emit sigResult(true);
    return true;
}

bool LogExportThread::exportToTxt(QString fileName, QList<LOG_MSG_DPKG> jList, QStringList labels)
{
    QFile fi(fileName);
    if (!fi.open(QIODevice::WriteOnly)) {
        emit sigResult(false);
        emit sigError(openErroStr);
        return false;
    }
    try {
        QTextStream out(&fi);
        out.setCodec(QTextCodec::codecForName("utf-8"));
        for (int i = 0; i < jList.count(); i++) {
            if (!m_canRunning) {
                throw  QString(stopStr);
            }
            int col = 0;
            LOG_MSG_DPKG jMsg = jList.at(i);
            out << labels.value(col++, "") << ":" << jMsg.dateTime << " ";
            out << labels.value(col++, "") << ":" << jMsg.msg << " ";
            out << labels.value(col++, "") << ":" << jMsg.action << " ";
            out << "\n";
        }
    } catch (QString ErrorStr) {
        qDebug() << "Export Stop" << ErrorStr;
        fi.close();
        emit sigResult(false);
        if (ErrorStr != stopStr) {
            emit sigError(QString("export error: %1").arg(ErrorStr));
        }
        return false;
    }
    fi.close();
    emit sigResult(true);
    return true;
}

bool LogExportThread::exportToTxt(QString fileName, QList<LOG_MSG_BOOT> jList, QStringList labels)
{
    QFile fi(fileName);
    if (!fi.open(QIODevice::WriteOnly)) {
        emit sigResult(false);
        emit sigError(openErroStr);
        return false;
    }
    try {
        QTextStream out(&fi);
        out.setCodec(QTextCodec::codecForName("utf-8"));
        for (int i = 0; i < jList.count(); i++) {
            if (!m_canRunning) {
                throw  QString(stopStr);
            }
            LOG_MSG_BOOT jMsg = jList.at(i);
            int col = 0;
            out << labels.value(col++, "") << ":" << jMsg.status << " ";
            out << labels.value(col++, "") << ":" << jMsg.msg << " ";
            out << "\n";
        }
    } catch (QString ErrorStr) {
        qDebug() << "Export Stop" << ErrorStr;
        fi.close();
        emit sigResult(false);
        if (ErrorStr != stopStr) {
            emit sigError(QString("export error: %1").arg(ErrorStr));
        }
        return false;
    }
    fi.close();
    emit sigResult(true);
    return true;
}

bool LogExportThread::exportToTxt(QString fileName, QList<LOG_MSG_XORG> jList, QStringList labels)
{
    QFile fi(fileName);
    if (!fi.open(QIODevice::WriteOnly)) {
        emit sigResult(false);
        emit sigError(openErroStr);
        return false;
    }
    try {
        QTextStream out(&fi);
        out.setCodec(QTextCodec::codecForName("utf-8"));
        for (int i = 0; i < jList.count(); i++) {
            if (!m_canRunning) {
                throw  QString(stopStr);
            }
            LOG_MSG_XORG jMsg = jList.at(i);
            int col = 0;
            out << labels.value(col++, "") << ":" << jMsg.dateTime << " ";
            out << labels.value(col++, "") << ":" << jMsg.msg << " ";
            out << "\n";
        }
    } catch (QString ErrorStr) {
        qDebug() << "Export Stop" << ErrorStr;
        fi.close();
        emit sigResult(false);
        if (ErrorStr != stopStr) {
            emit sigError(QString("export error: %1").arg(ErrorStr));
        }
        return false;
    }
    fi.close();
    emit sigResult(true);
    return true;
}

bool LogExportThread::exportToTxt(QString fileName, QList<LOG_MSG_NORMAL> jList, QStringList labels)
{
    QFile fi(fileName);
    if (!fi.open(QIODevice::WriteOnly)) {
        emit sigResult(false);
        emit sigError(openErroStr);
        return false;
    }
    try {
        QTextStream out(&fi);
        out.setCodec(QTextCodec::codecForName("utf-8"));
        for (int i = 0; i < jList.count(); i++) {
            if (!m_canRunning) {
                throw  QString(stopStr);
            }
            LOG_MSG_NORMAL jMsg = jList.at(i);
            int col = 0;
            out << labels.value(col++, "") << ":" << jMsg.eventType << " ";
            out << labels.value(col++, "") << ":" << jMsg.userName << " ";
            out << labels.value(col++, "") << ":" << jMsg.dateTime << " ";
            out << labels.value(col++, "") << ":" << jMsg.msg << " ";
            out << "\n";
        }
    } catch (QString ErrorStr) {
        qDebug() << "Export Stop" << ErrorStr;
        fi.close();
        emit sigResult(false);
        if (ErrorStr != stopStr) {
            emit sigError(QString("export error: %1").arg(ErrorStr));
        }
        return false;
    }
    fi.close();
    emit sigResult(true);
    return true;
}

bool LogExportThread::exportToTxt(QString fileName, QList<LOG_MSG_KWIN> jList, QStringList labels)
{
    QFile fi(fileName);
    if (!fi.open(QIODevice::WriteOnly)) {
        emit sigResult(false);
        emit sigError(openErroStr);
        return false;
    }
    try {
        QTextStream out(&fi);
        out.setCodec(QTextCodec::codecForName("utf-8"));
        for (int i = 0; i < jList.count(); i++) {
            if (!m_canRunning) {
                throw  QString(stopStr);
            }
            LOG_MSG_KWIN jMsg = jList.at(i);
            int col = 0;
            out << labels.value(col++, "") << ":" << jMsg.msg << " ";
            out << "\n";
        }
    } catch (QString ErrorStr) {
        qDebug() << "Export Stop" << ErrorStr;
        fi.close();
        emit sigResult(false);
        if (ErrorStr != stopStr) {
            emit sigError(QString("export error: %1").arg(ErrorStr));
        }
        return false;
    }
    fi.close();
    emit sigResult(true);
    return true;
}

bool LogExportThread::exportToTxt(QString fileName, QList<LOG_MSG_DNF> jList, QStringList labels)
{
    QFile fi(fileName);
    if (!fi.open(QIODevice::WriteOnly)) {
        emit sigResult(false);
        emit sigError(openErroStr);
        return false;
    }
    try {
        QTextStream out(&fi);
        out.setCodec(QTextCodec::codecForName("utf-8"));
        for (int i = 0; i < jList.count(); i++) {
            if (!m_canRunning) {
                throw  QString(stopStr);
            }
            int col = 0;
            LOG_MSG_DNF jMsg = jList.at(i);
            out << labels.value(col++, "") << ":" << jMsg.level << " ";
            out << labels.value(col++, "") << ":" << jMsg.dateTime << " ";
            out << labels.value(col++, "") << ":" << jMsg.msg << " ";
            out << "\n";
        }
    } catch (QString ErrorStr) {
        qDebug() << "Export Stop" << ErrorStr;
        fi.close();
        emit sigResult(false);
        if (ErrorStr != stopStr) {
            emit sigError(QString("export error: %1").arg(ErrorStr));
        }
        return false;
    }
    fi.close();
    emit sigResult(true);
    return true;
}

bool LogExportThread::exportToTxt(QString fileName, QList<LOG_MSG_DMESG> jList, QStringList labels)
{
    QFile fi(fileName);
    if (!fi.open(QIODevice::WriteOnly)) {
        emit sigResult(false);
        emit sigError(openErroStr);
        return false;
    }
    try {
        QTextStream out(&fi);
        out.setCodec(QTextCodec::codecForName("utf-8"));
        for (int i = 0; i < jList.count(); i++) {
            if (!m_canRunning) {
                throw  QString(stopStr);
            }
            int col = 0;
            LOG_MSG_DMESG jMsg = jList.at(i);
            out << labels.value(col++, "") << ":" << jMsg.level << " ";
            out << labels.value(col++, "") << ":" << jMsg.dateTime << " ";
            out << labels.value(col++, "") << ":" << jMsg.msg << " ";
            out << "\n";
        }
    } catch (QString ErrorStr) {
        qDebug() << "Export Stop" << ErrorStr;
        fi.close();
        emit sigResult(false);
        if (ErrorStr != stopStr) {
            emit sigError(QString("export error: %1").arg(ErrorStr));
        }
        return false;
    }
    fi.close();
    emit sigResult(true);
    return true;
}


bool LogExportThread::exportToDoc(QString fileName, QStandardItemModel *pModel, LOG_FLAG flag)
{
#if 1
    try {
        Docx::Document doc(DOCTEMPLATE);
        Docx::Table *tab = doc.addTable(pModel->rowCount() + 1, pModel->columnCount());
        tab->setAlignment(Docx::WD_TABLE_ALIGNMENT::LEFT);

        for (int col = 0; col < pModel->columnCount(); ++col) {
            auto item = pModel->horizontalHeaderItem(col);
            auto cel = tab->cell(0, col);
            if (item) {
                cel->addText(pModel->horizontalHeaderItem(col)->text());
            }
        }
        if (flag == APP) {
            for (int row = 0; row < pModel->rowCount(); ++row) {
                if (!m_canRunning) {
                    throw  QString(stopStr);
                }
                auto cel = tab->cell(row + 1, 0);
                cel->addText(pModel->item(row, 0)->data(Qt::UserRole + 6).toString());
                for (int col = 1; col < pModel->columnCount(); ++col) {
                    if (!m_canRunning) {
                        throw  QString(stopStr);
                    }
                    auto cel = tab->cell(row + 1, col);
                    cel->addText(pModel->item(row, col)->text());
                }
            }
        } else {
            for (int row = 0; row < pModel->rowCount(); ++row) {
                if (!m_canRunning) {
                    throw  QString(stopStr);
                }
                for (int col = 0; col < pModel->columnCount(); ++col) {
                    if (!m_canRunning) {
                        throw  QString(stopStr);
                    }
                    auto cel = tab->cell(row + 1, col);
                    cel->addText(pModel->item(row, col)->text());
                }
            }
        }
        doc.save(fileName);
    } catch (QString ErrorStr) {
        qDebug() << "Export Stop" << ErrorStr;
        emit sigResult(false);
        if (ErrorStr != stopStr) {
            emit sigError(QString("export error: %1").arg(ErrorStr));
        }
        return false;
    }
#else
    QTextDocumentWriter writer(fileName);
    writer.setFormat("odf");
    QTextDocument *doc = new QTextDocument;
    QString html;
    html.append("<!DOCTYPE html>\n");
    html.append("<html>\n");
    html.append("<body>\n");
    html.append("<table border=\"1\">\n");
    // write title
    html.append("<tr>");
    for (int i = 0; i < pModel->columnCount(); ++i) {
        QString labelInfo = QString("<td>%1</td>").arg(pModel->horizontalHeaderItem(i)->text());
        html.append(labelInfo.toUtf8().data());
    }
    html.append("</tr>");
    // write contents
    for (int row = 0; row < pModel->rowCount(); ++row) {
        html.append("<tr>");
        for (int col = 0; col < pModel->columnCount(); ++col) {
            QString info = QString("<td>%1</td>").arg(pModel->item(row, col)->text());
            html.append(info.toUtf8().data());
        }
        html.append("</tr>");
    }
    html.append("</table>\n");
    html.append("</body>\n");
    html.append("</html>\n");
    doc->setHtml(html);
    writer.write(doc);
#endif
    emit sigResult(true);
    return true;
}

bool LogExportThread::exportToDoc(QString fileName, QList<LOG_MSG_JOURNAL> jList,
                                  QStringList labels, LOG_FLAG iFlag)
{
#if 1
    try {
        Docx::Document doc(DOCTEMPLATE);
        Docx::Table *tab;
        if (iFlag == JOURNAL) {
            tab = doc.addTable(jList.count() + 1, 6);
        } else if (iFlag == KERN) {
            tab = doc.addTable(jList.count() + 1, 4);
        }

        tab->setAlignment(Docx::WD_TABLE_ALIGNMENT::LEFT);

        for (int col = 0; col < labels.count(); ++col) {
            auto cel = tab->cell(0, col);
            cel->addText(labels.at(col));
        }

        for (int row = 0; row < jList.count(); ++row) {
            if (!m_canRunning) {
                throw  QString(stopStr);
            }
            LOG_MSG_JOURNAL message = jList.at(row);
            int col = 0;
            if (iFlag == JOURNAL) {
                tab->cell(row + 1, col++)->addText(message.level);
                tab->cell(row + 1, col++)->addText(message.daemonName);
                tab->cell(row + 1, col++)->addText(message.dateTime);
                tab->cell(row + 1, col++)->addText(message.msg);
                tab->cell(row + 1, col++)->addText(message.hostName);
                tab->cell(row + 1, col++)->addText(message.daemonId);
            } else if (iFlag == KERN) {
                tab->cell(row + 1, col++)->addText(message.dateTime);
                tab->cell(row + 1, col++)->addText(message.hostName);
                tab->cell(row + 1, col++)->addText(message.daemonName);
                tab->cell(row + 1, col++)->addText(message.msg);
            }
        }
        doc.save(fileName);
    } catch (QString ErrorStr) {
        qDebug() << "Export Stop" << ErrorStr;
        emit sigResult(false);
        if (ErrorStr != stopStr) {
            emit sigError(QString("export error: %1").arg(ErrorStr));
        }
        return false;
    }
#else
    QTextDocumentWriter writer(fileName);
    writer.setFormat("odf");
    QTextDocument *doc = new QTextDocument;
    QString html;
    html.append("<!DOCTYPE html>\n");
    html.append("<html>\n");
    html.append("<body>\n");
    html.append("<table border=\"1\">\n");
    QString title = QString(
                        "<tr><td>时间</td><td>主机名</td><td>守护进程</td><td>进程ID</td><td>级别</td><td>消息</"
                        "td></tr>");
    html.append(title.toUtf8().data());
    for (int i = 0; i < jList.count(); i++) {
        LOG_MSG_JOURNAL jMsg = jList.at(i);
        QString info =
            QString("<tr><td>%1</td><td>%2</td><td>%3</td><td>%4</td><td>%5</td><td>%6</td></tr>")
            .arg(jMsg.dateTime)
            .arg(jMsg.hostName)
            .arg(jMsg.daemonName)
            .arg(jMsg.daemonId)
            .arg(jMsg.level)
            .arg(jMsg.msg);
        html.append(info.toUtf8().data());
    }

    html.append("</table>\n");
    html.append("</body>\n");
    html.append("</html>\n");
    doc->setHtml(html);
    writer.write(doc);
#endif
    emit sigResult(true);
    return true;
}

bool LogExportThread::exportToDoc(QString fileName, QList<LOG_MSG_APPLICATOIN> jList, QStringList labels, QString &iAppName)
{
    try {
        Docx::Document doc(DOCTEMPLATE);
        Docx::Table *tab = doc.addTable(jList.count() + 1, 4);
        tab->setAlignment(Docx::WD_TABLE_ALIGNMENT::LEFT);

        for (int col = 0; col < labels.count(); ++col) {
            auto cel = tab->cell(0, col);
            cel->addText(labels.at(col));
        }

        for (int row = 0; row < jList.count(); ++row) {
            if (!m_canRunning) {
                throw  QString(stopStr);
            }
            LOG_MSG_APPLICATOIN message = jList.at(row);
            int col = 0;
            tab->cell(row + 1, col++)->addText(strTranslate(message.level));
            tab->cell(row + 1, col++)->addText(message.dateTime);
            tab->cell(row + 1, col++)->addText(iAppName);
            tab->cell(row + 1, col++)->addText(message.msg);
        }
        //暂时如此，最后的真正写文件之前就发出成功信号关闭弹框，因为如果最后写的时候无法取消
        emit sigResult(true);
        doc.save(fileName);
    } catch (QString ErrorStr) {
        qDebug() << "Export Stop" << ErrorStr;
        emit sigResult(false);
        if (ErrorStr != stopStr) {
            emit sigError(QString("export error: %1").arg(ErrorStr));
        }
        return false;
    }
    //  emit sigResult(true);
    return true;
}

bool LogExportThread::exportToDoc(QString fileName, QList<LOG_MSG_DPKG> jList, QStringList labels)
{
    try {
        Docx::Document doc(DOCTEMPLATE);
        Docx::Table *tab = doc.addTable(jList.count() + 1, 3);
        tab->setAlignment(Docx::WD_TABLE_ALIGNMENT::LEFT);

        for (int col = 0; col < labels.count(); ++col) {
            auto cel = tab->cell(0, col);
            cel->addText(labels.at(col));
        }

        for (int row = 0; row < jList.count(); ++row) {
            if (!m_canRunning) {
                throw  QString(stopStr);
            }
            LOG_MSG_DPKG message = jList.at(row);
            int col = 0;
            tab->cell(row + 1, col++)->addText(message.dateTime);
            tab->cell(row + 1, col++)->addText(message.msg);
            tab->cell(row + 1, col++)->addText(message.action);
        }
        //暂时如此，最后的真正写文件之前就发出成功信号关闭弹框，因为如果最后写的时候无法取消
        emit sigResult(true);
        doc.save(fileName);
    } catch (QString ErrorStr) {
        qDebug() << "Export Stop" << ErrorStr;
        emit sigResult(false);
        if (ErrorStr != stopStr) {
            emit sigError(QString("export error: %1").arg(ErrorStr));
        }
        return false;
    }
    //  emit sigResult(true);
    return true;
}

bool LogExportThread::exportToDoc(QString fileName, QList<LOG_MSG_BOOT> jList, QStringList labels)
{
    try {
        Docx::Document doc(DOCTEMPLATE);
        Docx::Table *tab = doc.addTable(jList.count() + 1, 2);
        tab->setAlignment(Docx::WD_TABLE_ALIGNMENT::LEFT);

        for (int col = 0; col < labels.count(); ++col) {
            auto cel = tab->cell(0, col);
            cel->addText(labels.at(col));
        }

        for (int row = 0; row < jList.count(); ++row) {
            if (!m_canRunning) {
                throw  QString(stopStr);
            }
            LOG_MSG_BOOT message = jList.at(row);
            int col = 0;
            tab->cell(row + 1, col++)->addText(message.status);
            tab->cell(row + 1, col++)->addText(message.msg);
        }
        //暂时如此，最后的真正写文件之前就发出成功信号关闭弹框，因为如果最后写的时候无法取消
        emit sigResult(true);
        doc.save(fileName);
    } catch (QString ErrorStr) {
        qDebug() << "Export Stop" << ErrorStr;
        emit sigResult(false);
        if (ErrorStr != stopStr) {
            emit sigError(QString("export error: %1").arg(ErrorStr));
        }
        return false;
    }
    //  emit sigResult(true);
    return true;
}

bool LogExportThread::exportToDoc(QString fileName, QList<LOG_MSG_XORG> jList, QStringList labels)
{
    try {
        Docx::Document doc(DOCTEMPLATE);
        Docx::Table *tab = doc.addTable(jList.count() + 1, 2);
        tab->setAlignment(Docx::WD_TABLE_ALIGNMENT::LEFT);

        for (int col = 0; col < labels.count(); ++col) {
            auto cel = tab->cell(0, col);
            cel->addText(labels.at(col));
        }

        for (int row = 0; row < jList.count(); ++row) {
            if (!m_canRunning) {
                throw  QString(stopStr);
            }
            LOG_MSG_XORG message = jList.at(row);
            int col = 0;
            tab->cell(row + 1, col++)->addText(message.dateTime);
            tab->cell(row + 1, col++)->addText(message.msg);
        }
        //暂时如此，最后的真正写文件之前就发出成功信号关闭弹框，因为如果最后写的时候无法取消
        emit sigResult(true);
        doc.save(fileName);
    } catch (QString ErrorStr) {
        qDebug() << "Export Stop" << ErrorStr;
        emit sigResult(false);
        if (ErrorStr != stopStr) {
            emit sigError(QString("export error: %1").arg(ErrorStr));
        }
        return false;
    }
    // emit sigResult(true);
    return true;
}

bool LogExportThread::exportToDoc(QString fileName, QList<LOG_MSG_NORMAL> jList, QStringList labels)
{
    try {
        Docx::Document doc(DOCTEMPLATE);
        Docx::Table *tab = doc.addTable(jList.count() + 1, 4);
        tab->setAlignment(Docx::WD_TABLE_ALIGNMENT::LEFT);

        for (int col = 0; col < labels.count(); ++col) {
            auto cel = tab->cell(0, col);
            cel->addText(labels.at(col));
        }

        for (int row = 0; row < jList.count(); ++row) {
            if (!m_canRunning) {
                throw  QString(stopStr);
            }
            LOG_MSG_NORMAL message = jList.at(row);
            int col = 0;
            tab->cell(row + 1, col++)->addText(message.eventType);
            tab->cell(row + 1, col++)->addText(message.userName);
            tab->cell(row + 1, col++)->addText(message.dateTime);
            tab->cell(row + 1, col++)->addText(message.msg);
        }
        //暂时如此，最后的真正写文件之前就发出成功信号关闭弹框，因为如果最后写的时候无法取消
        emit sigResult(true);
        doc.save(fileName);
    } catch (QString ErrorStr) {
        qDebug() << "Export Stop" << ErrorStr;
        emit sigResult(false);
        if (ErrorStr != stopStr) {
            emit sigError(QString("export error: %1").arg(ErrorStr));
        }
        return false;
    }
    // emit sigResult(true);
    return true;
}

bool LogExportThread::exportToDoc(QString fileName, QList<LOG_MSG_KWIN> jList, QStringList labels)
{
    try {
        Docx::Document doc(DOCTEMPLATE);
        Docx::Table *tab = doc.addTable(jList.count() + 1, 1);
        tab->setAlignment(Docx::WD_TABLE_ALIGNMENT::LEFT);

        for (int col = 0; col < labels.count(); ++col) {
            auto cel = tab->cell(0, col);
            cel->addText(labels.at(col));
        }

        for (int row = 0; row < jList.count(); ++row) {
            if (!m_canRunning) {
                throw  QString(stopStr);
            }
            LOG_MSG_KWIN message = jList.at(row);
            int col = 0;
            tab->cell(row + 1, col++)->addText(message.msg);
        }
        //暂时如此，最后的真正写文件之前就发出成功信号关闭弹框，因为如果最后写的时候无法取消
        emit sigResult(true);
        doc.save(fileName);
    } catch (QString ErrorStr) {
        qDebug() << "Export Stop" << ErrorStr;
        emit sigResult(false);
        if (ErrorStr != stopStr) {
            emit sigError(QString("export error: %1").arg(ErrorStr));
        }
        return false;
    }
    //emit sigResult(true);
    return true;
}

bool LogExportThread::exportToDoc(QString fileName, QList<LOG_MSG_DNF> jList, QStringList labels)
{
    try {
        Docx::Document doc(DOCTEMPLATE);
        Docx::Table *tab = doc.addTable(jList.count() + 1, 3);
        tab->setAlignment(Docx::WD_TABLE_ALIGNMENT::LEFT);

        for (int col = 0; col < labels.count(); ++col) {
            auto cel = tab->cell(0, col);
            cel->addText(labels.at(col));
        }

        for (int row = 0; row < jList.count(); ++row) {
            if (!m_canRunning) {
                throw  QString(stopStr);
            }
            LOG_MSG_DNF message = jList.at(row);
            int col = 0;
            tab->cell(row + 1, col++)->addText(message.level);
            tab->cell(row + 1, col++)->addText(message.dateTime);
            tab->cell(row + 1, col++)->addText(message.msg);

        }
        //暂时如此，最后的真正写文件之前就发出成功信号关闭弹框，因为如果最后写的时候无法取消
        emit sigResult(true);
        doc.save(fileName);
    } catch (QString ErrorStr) {
        qDebug() << "Export Stop" << ErrorStr;
        emit sigResult(false);
        if (ErrorStr != stopStr) {
            emit sigError(QString("export error: %1").arg(ErrorStr));
        }
        return false;
    }
    //  emit sigResult(true);
    return true;
}

bool LogExportThread::exportToDoc(QString fileName, QList<LOG_MSG_DMESG> jList, QStringList labels)
{
    try {
        Docx::Document doc(DOCTEMPLATE);
        Docx::Table *tab = doc.addTable(jList.count() + 1, 3);
        tab->setAlignment(Docx::WD_TABLE_ALIGNMENT::LEFT);

        for (int col = 0; col < labels.count(); ++col) {
            auto cel = tab->cell(0, col);
            cel->addText(labels.at(col));
        }

        for (int row = 0; row < jList.count(); ++row) {
            if (!m_canRunning) {
                throw  QString(stopStr);
            }
            LOG_MSG_DMESG message = jList.at(row);
            int col = 0;
            tab->cell(row + 1, col++)->addText(message.level);
            tab->cell(row + 1, col++)->addText(message.dateTime);
            tab->cell(row + 1, col++)->addText(message.msg);

        }
        //暂时如此，最后的真正写文件之前就发出成功信号关闭弹框，因为如果最后写的时候无法取消
        emit sigResult(true);
        doc.save(fileName);
    } catch (QString ErrorStr) {
        qDebug() << "Export Stop" << ErrorStr;
        emit sigResult(false);
        if (ErrorStr != stopStr) {
            emit sigError(QString("export error: %1").arg(ErrorStr));
        }
        return false;
    }
    //  emit sigResult(true);
    return true;
}





bool LogExportThread::exportToHtml(QString fileName, QStandardItemModel *pModel, LOG_FLAG flag)
{
    QFile html(fileName);
    if (!html.open(QIODevice::WriteOnly)) {
        emit sigResult(false);
        emit sigError(openErroStr);
        return false;
    }
    try {
        html.write("<!DOCTYPE html>\n");
        html.write("<html>\n");
        html.write("<body>\n");
        html.write("<table border=\"1\">\n");
        // write title
        html.write("<tr>");
        for (int i = 0; i < pModel->columnCount(); ++i) {
            QString labelInfo = QString("<td>%1</td>").arg(pModel->horizontalHeaderItem(i)->text());
            html.write(labelInfo.toUtf8().data());
        }
        html.write("</tr>");
        // write contents
        if (flag == APP) {
            for (int row = 0; row < pModel->rowCount(); ++row) {
                if (!m_canRunning) {
                    throw  QString(stopStr);
                }
                html.write("<tr>");

                QString info =
                    QString("<td>%1</td>").arg(pModel->item(row, 0)->data(Qt::UserRole + 6).toString());
                html.write(info.toUtf8().data());

                for (int col = 1; col < pModel->columnCount(); ++col) {
                    QString info = QString("<td>%1</td>").arg(pModel->item(row, col)->text());
                    html.write(info.toUtf8().data());
                }
                html.write("</tr>");
            }
        } else {
            for (int row = 0; row < pModel->rowCount(); ++row) {
                if (!m_canRunning) {
                    throw  QString(stopStr);
                }
                html.write("<tr>");
                for (int col = 0; col < pModel->columnCount(); ++col) {
                    QString info = QString("<td>%1</td>").arg(pModel->item(row, col)->text());
                    html.write(info.toUtf8().data());
                }
                html.write("</tr>");
            }
        }
        html.write("</table>\n");
        html.write("</body>\n");
        html.write("</html>\n");
    } catch (QString ErrorStr) {
        qDebug() << "Export Stop" << ErrorStr;
        html.close();
        emit sigResult(false);
        if (ErrorStr != stopStr) {
            emit sigError(QString("export error: %1").arg(ErrorStr));
        }
        return false;
    }
    html.close();
    emit sigResult(true);
    return true;
}

bool LogExportThread::exportToHtml(QString fileName, QList<LOG_MSG_JOURNAL> jList,  QStringList labels, LOG_FLAG flag)
{
    QFile html(fileName);
    if (!html.open(QIODevice::WriteOnly)) {
        emit sigResult(false);
        emit sigError(openErroStr);
        return false;
    }
    try {

        html.write("<!DOCTYPE html>\n");
        html.write("<html>\n");
        html.write("<body>\n");
        html.write("<table border=\"1\">\n");
        if (flag == JOURNAL) {
            //    QString title = QString(
            //        "<tr><td>时间</td><td>主机名</td><td>守护进程</td><td>进程ID</td><td>级别</td><td>消息</"
            //        "td></tr>");
            //    QString title = QString("<tr><td>") +
            //                    QString(DApplication::translate("Table", "Date and Time")) +
            //                    QString("</td><td>") + QString(DApplication::translate("Table", "User")) +
            //                    QString("</td><td>") + QString(DApplication::translate("Table", "Daemon"))
            //                    + QString("</td><td>") + QString(DApplication::translate("Table", "PID"))
            //                    + QString("</td><td>") + QString(DApplication::translate("Table",
            //                    "Level")) + QString("</td><td>") +
            //                    QString(DApplication::translate("Table", "Info")) + QString("</td></tr>");
            //                    //delete  for bug
            QString title = QString("<tr><td>") + QString(DApplication::translate("Table", "Level")) +
                            QString("</td><td>") + QString(DApplication::translate("Table", "Process")) +
                            QString("</td><td>") +
                            QString(DApplication::translate("Table", "Date and Time")) +
                            QString("</td><td>") + QString(DApplication::translate("Table", "Info")) +
                            QString("</td><td>") + QString(DApplication::translate("Table", "User")) +
                            QString("</td><td>") + QString(DApplication::translate("Table", "PID")) +
                            QString("</td></tr>");
            html.write(title.toUtf8().data());
            for (int i = 0; i < jList.count(); i++) {
                if (!m_canRunning) {
                    throw  QString(stopStr);
                }
                LOG_MSG_JOURNAL jMsg = jList.at(i);
                //        QString info =
                //            QString("<tr><td>%1</td><td>%2</td><td>%3</td><td>%4</td><td>%5</td><td>%6</td></tr>")
                //                .arg(jMsg.dateTime)
                //                .arg(jMsg.hostName)
                //                .arg(jMsg.daemonName)
                //                .arg(jMsg.daemonId)
                //                .arg(jMsg.level)
                //                .arg(jMsg.msg); //delete for bug
                QString info =
                    QString("<tr><td>%1</td><td>%2</td><td>%3</td><td>%4</td><td>%5</td><td>%6</td></tr>")
                    .arg(jMsg.level)
                    .arg(jMsg.daemonName)
                    .arg(jMsg.dateTime)
                    .arg(jMsg.msg)
                    .arg(jMsg.hostName)
                    .arg(jMsg.daemonId);
                html.write(info.toUtf8().data());

            }
        } else if (flag == KERN) {
            html.write("<tr>");
            for (int i = 0; i < labels.count(); ++i) {
                QString labelInfo = QString("<td>%1</td>").arg(labels.value(i));
                html.write(labelInfo.toUtf8().data());
            }
            html.write("</tr>");
            for (int row = 0; row < jList.count(); ++row) {
                if (!m_canRunning) {
                    throw  QString(stopStr);
                }
                LOG_MSG_JOURNAL jMsg = jList.at(row);
                html.write("<tr>");
                QString info = QString("<td>%1</td>").arg(jMsg.dateTime);
                html.write(info.toUtf8().data());
                info = QString("<td>%1</td>").arg(jMsg.hostName);
                html.write(info.toUtf8().data());
                info = QString("<td>%1</td>").arg(jMsg.daemonName);
                html.write(info.toUtf8().data());
                info = QString("<td>%1</td>").arg(jMsg.msg);
                html.write(info.toUtf8().data());
                html.write("</tr>");
            }

        }

        html.write("</table>\n");
        html.write("</body>\n");
        html.write("</html>\n");
    } catch (QString ErrorStr) {
        qDebug() << "Export Stop" << ErrorStr;
        html.close();
        emit sigResult(false);
        if (ErrorStr != stopStr) {
            emit sigError(QString("export error: %1").arg(ErrorStr));
        }
        return false;
    }
    html.close();
    emit sigResult(true);
    return true;
}

bool LogExportThread::exportToHtml(QString fileName, QList<LOG_MSG_APPLICATOIN> jList, QStringList labels, QString &iAppName)
{
    QFile html(fileName);
    if (!html.open(QIODevice::WriteOnly)) {
        emit sigResult(false);
        emit sigError(openErroStr);
        return false;
    }
    try {
        html.write("<!DOCTYPE html>\n");
        html.write("<html>\n");
        html.write("<body>\n");
        html.write("<table border=\"1\">\n");
        html.write("<tr>");
        for (int i = 0; i < labels.count(); ++i) {
            QString labelInfo = QString("<td>%1</td>").arg(labels.value(i));
            html.write(labelInfo.toUtf8().data());
        }
        html.write("</tr>");
        for (int row = 0; row < jList.count(); ++row) {
            if (!m_canRunning) {
                throw  QString(stopStr);
            }
            LOG_MSG_APPLICATOIN jMsg = jList.at(row);
            html.write("<tr>");
            QString info = QString("<td>%1</td>").arg(strTranslate(jMsg.level));
            html.write(info.toUtf8().data());
            info = QString("<td>%1</td>").arg(jMsg.dateTime);
            html.write(info.toUtf8().data());
            info = QString("<td>%1</td>").arg(iAppName);
            html.write(info.toUtf8().data());
            info = QString("<td>%1</td>").arg(jMsg.msg);
            html.write(info.toUtf8().data());
            html.write("</tr>");
        }

        html.write("</table>\n");
        html.write("</body>\n");
        html.write("</html>\n");
    } catch (QString ErrorStr) {
        qDebug() << "Export Stop" << ErrorStr;
        html.close();
        emit sigResult(false);
        if (ErrorStr != stopStr) {
            emit sigError(QString("export error: %1").arg(ErrorStr));
        }
        return false;
    }
    html.close();
    emit sigResult(true);
    return true;
}

bool LogExportThread::exportToHtml(QString fileName, QList<LOG_MSG_DPKG> jList, QStringList labels)
{
    QFile html(fileName);
    if (!html.open(QIODevice::WriteOnly)) {
        emit sigResult(false);
        emit sigError(openErroStr);
        return false;
    }
    try {
        html.write("<!DOCTYPE html>\n");
        html.write("<html>\n");
        html.write("<body>\n");
        html.write("<table border=\"1\">\n");
        html.write("<tr>");
        for (int i = 0; i < labels.count(); ++i) {
            QString labelInfo = QString("<td>%1</td>").arg(labels.value(i));
            html.write(labelInfo.toUtf8().data());
        }
        html.write("</tr>");
        for (int row = 0; row < jList.count(); ++row) {
            if (!m_canRunning) {
                throw  QString(stopStr);
            }
            LOG_MSG_DPKG jMsg = jList.at(row);
            html.write("<tr>");
            QString info = QString("<td>%1</td>").arg(jMsg.dateTime);
            html.write(info.toUtf8().data());
            info = QString("<td>%1</td>").arg(jMsg.msg);
            html.write(info.toUtf8().data());
            info = QString("<td>%1</td>").arg(jMsg.action);
            html.write(info.toUtf8().data());
            html.write("</tr>");
        }

        html.write("</table>\n");
        html.write("</body>\n");
        html.write("</html>\n");
    } catch (QString ErrorStr) {
        qDebug() << "Export Stop" << ErrorStr;
        html.close();
        emit sigResult(false);
        if (ErrorStr != stopStr) {
            emit sigError(QString("export error: %1").arg(ErrorStr));
        }
        return false;
    }
    html.close();
    emit sigResult(true);
    return true;
}

bool LogExportThread::exportToHtml(QString fileName, QList<LOG_MSG_BOOT> jList, QStringList labels)
{
    QFile html(fileName);
    if (!html.open(QIODevice::WriteOnly)) {
        emit sigResult(false);
        emit sigError(openErroStr);
        return false;
    }
    try {
        html.write("<!DOCTYPE html>\n");
        html.write("<html>\n");
        html.write("<body>\n");
        html.write("<table border=\"1\">\n");
        html.write("<tr>");
        for (int i = 0; i < labels.count(); ++i) {
            QString labelInfo = QString("<td>%1</td>").arg(labels.value(i));
            html.write(labelInfo.toUtf8().data());
        }
        html.write("</tr>");
        for (int row = 0; row < jList.count(); ++row) {
            if (!m_canRunning) {
                throw  QString(stopStr);
            }
            LOG_MSG_BOOT jMsg = jList.at(row);
            html.write("<tr>");
            QString info = QString("<td>%1</td>").arg(jMsg.status);
            html.write(info.toUtf8().data());
            info = QString("<td>%1</td>").arg(jMsg.msg);
            html.write(info.toUtf8().data());
            html.write("</tr>");
        }

        html.write("</table>\n");
        html.write("</body>\n");
        html.write("</html>\n");
    } catch (QString ErrorStr) {
        qDebug() << "Export Stop" << ErrorStr;
        html.close();
        emit sigResult(false);
        if (ErrorStr != stopStr) {
            emit sigError(QString("export error: %1").arg(ErrorStr));
        }
        return false;
    }
    html.close();
    emit sigResult(true);
    return true;
}

bool LogExportThread::exportToHtml(QString fileName, QList<LOG_MSG_XORG> jList, QStringList labels)
{
    QFile html(fileName);
    if (!html.open(QIODevice::WriteOnly)) {
        emit sigResult(false);
        emit sigError(openErroStr);
        return false;
    }
    try {
        html.write("<!DOCTYPE html>\n");
        html.write("<html>\n");
        html.write("<body>\n");
        html.write("<table border=\"1\">\n");
        html.write("<tr>");
        for (int i = 0; i < labels.count(); ++i) {
            QString labelInfo = QString("<td>%1</td>").arg(labels.value(i));
            html.write(labelInfo.toUtf8().data());
        }
        html.write("</tr>");
        for (int row = 0; row < jList.count(); ++row) {
            if (!m_canRunning) {
                throw  QString(stopStr);
            }
            LOG_MSG_XORG jMsg = jList.at(row);
            html.write("<tr>");
            QString info = QString("<td>%1</td>").arg(jMsg.dateTime);
            html.write(info.toUtf8().data());
            info = QString("<td>%1</td>").arg(jMsg.msg);
            html.write(info.toUtf8().data());
            html.write("</tr>");
        }

        html.write("</table>\n");
        html.write("</body>\n");
        html.write("</html>\n");
    } catch (QString ErrorStr) {
        qDebug() << "Export Stop" << ErrorStr;
        html.close();
        emit sigResult(false);
        if (ErrorStr != stopStr) {
            emit sigError(QString("export error: %1").arg(ErrorStr));
        }
        return false;
    }
    html.close();
    emit sigResult(true);
    return true;
}

bool LogExportThread::exportToHtml(QString fileName, QList<LOG_MSG_NORMAL> jList, QStringList labels)
{
    QFile html(fileName);
    if (!html.open(QIODevice::WriteOnly)) {
        emit sigResult(false);
        emit sigError(openErroStr);
        return false;
    }
    try {
        html.write("<!DOCTYPE html>\n");
        html.write("<html>\n");
        html.write("<body>\n");
        html.write("<table border=\"1\">\n");
        html.write("<tr>");
        for (int i = 0; i < labels.count(); ++i) {
            QString labelInfo = QString("<td>%1</td>").arg(labels.value(i));
            html.write(labelInfo.toUtf8().data());
        }
        html.write("</tr>");
        for (int row = 0; row < jList.count(); ++row) {
            if (!m_canRunning) {
                throw  QString(stopStr);
            }
            LOG_MSG_NORMAL jMsg = jList.at(row);
            html.write("<tr>");
            QString info = QString("<td>%1</td>").arg(jMsg.eventType);
            html.write(info.toUtf8().data());
            info = QString("<td>%1</td>").arg(jMsg.userName);
            html.write(info.toUtf8().data());
            info = QString("<td>%1</td>").arg(jMsg.dateTime);
            html.write(info.toUtf8().data());
            info = QString("<td>%1</td>").arg(jMsg.msg);
            html.write(info.toUtf8().data());
            html.write("</tr>");
        }

        html.write("</table>\n");
        html.write("</body>\n");
        html.write("</html>\n");
    } catch (QString ErrorStr) {
        qDebug() << "Export Stop" << ErrorStr;
        html.close();
        emit sigResult(false);
        if (ErrorStr != stopStr) {
            emit sigError(QString("export error: %1").arg(ErrorStr));
        }
        return false;
    }
    html.close();
    emit sigResult(true);
    return true;
}

bool LogExportThread::exportToHtml(QString fileName, QList<LOG_MSG_KWIN> jList, QStringList labels)
{
    QFile html(fileName);
    if (!html.open(QIODevice::WriteOnly)) {
        emit sigResult(false);
        emit sigError(openErroStr);
        return false;
    }
    try {
        html.write("<!DOCTYPE html>\n");
        html.write("<html>\n");
        html.write("<body>\n");
        html.write("<table border=\"1\">\n");
        html.write("<tr>");
        for (int i = 0; i < labels.count(); ++i) {
            QString labelInfo = QString("<td>%1</td>").arg(labels.value(i));
            html.write(labelInfo.toUtf8().data());
        }
        html.write("</tr>");
        for (int row = 0; row < jList.count(); ++row) {
            if (!m_canRunning) {
                throw  QString(stopStr);
            }
            LOG_MSG_KWIN jMsg = jList.at(row);
            html.write("<tr>");
            QString info = QString("<td>%1</td>").arg(jMsg.msg);
            html.write(info.toUtf8().data());
            html.write("</tr>");
        }

        html.write("</table>\n");
        html.write("</body>\n");
        html.write("</html>\n");
    } catch (QString ErrorStr) {
        qDebug() << "Export Stop" << ErrorStr;
        html.close();
        emit sigResult(false);
        if (ErrorStr != stopStr) {
            emit sigError(QString("export error: %1").arg(ErrorStr));
        }
        return false;
    }
    html.close();
    emit sigResult(true);
    return true;
}

bool LogExportThread::exportToHtml(QString fileName, QList<LOG_MSG_DNF> jList, QStringList labels)
{
    QFile html(fileName);
    if (!html.open(QIODevice::WriteOnly)) {
        emit sigResult(false);
        emit sigError(openErroStr);
        return false;
    }
    try {
        html.write("<!DOCTYPE html>\n");
        html.write("<html>\n");
        html.write("<body>\n");
        html.write("<table border=\"1\">\n");
        html.write("<tr>");
        for (int i = 0; i < labels.count(); ++i) {
            QString labelInfo = QString("<td>%1</td>").arg(labels.value(i));
            html.write(labelInfo.toUtf8().data());
        }
        html.write("</tr>");
        for (int row = 0; row < jList.count(); ++row) {
            if (!m_canRunning) {
                throw  QString(stopStr);
            }
            LOG_MSG_DNF jMsg = jList.at(row);
            html.write("<tr>");
            QString info = QString("<td>%1</td>").arg(jMsg.level);
            html.write(info.toUtf8().data());
            info = QString("<td>%1</td>").arg(jMsg.dateTime);
            html.write(info.toUtf8().data());
            //此style为使元素内\n换行符起效
            info = QString("<td style='white-space: pre-line;'>%1</td>").arg(jMsg.msg);
            html.write(info.toUtf8().data());
            html.write("</tr>");
        }

        html.write("</table>\n");
        html.write("</body>\n");
        html.write("</html>\n");
    } catch (QString ErrorStr) {
        qDebug() << "Export Stop" << ErrorStr;
        html.close();
        emit sigResult(false);
        if (ErrorStr != stopStr) {
            emit sigError(QString("export error: %1").arg(ErrorStr));
        }
        return false;
    }
    html.close();
    emit sigResult(true);
    return true;
}

bool LogExportThread::exportToHtml(QString fileName, QList<LOG_MSG_DMESG> jList, QStringList labels)
{
    QFile html(fileName);
    if (!html.open(QIODevice::WriteOnly)) {
        emit sigResult(false);
        emit sigError(openErroStr);
        return false;
    }
    try {
        html.write("<!DOCTYPE html>\n");
        html.write("<html>\n");
        html.write("<body>\n");
        html.write("<table border=\"1\">\n");
        html.write("<tr>");
        for (int i = 0; i < labels.count(); ++i) {
            QString labelInfo = QString("<td>%1</td>").arg(labels.value(i));
            html.write(labelInfo.toUtf8().data());
        }
        html.write("</tr>");
        for (int row = 0; row < jList.count(); ++row) {
            if (!m_canRunning) {
                throw  QString(stopStr);
            }
            LOG_MSG_DMESG jMsg = jList.at(row);
            html.write("<tr>");
            QString info = QString("<td>%1</td>").arg(jMsg.level);
            html.write(info.toUtf8().data());
            info = QString("<td>%1</td>").arg(jMsg.dateTime);
            html.write(info.toUtf8().data());
            //此style为使元素内\n换行符起效
            info = QString("<td style='white-space: pre-line;'>%1</td>").arg(jMsg.msg);
            html.write(info.toUtf8().data());
            html.write("</tr>");
        }

        html.write("</table>\n");
        html.write("</body>\n");
        html.write("</html>\n");
    } catch (QString ErrorStr) {
        qDebug() << "Export Stop" << ErrorStr;
        html.close();
        emit sigResult(false);
        if (ErrorStr != stopStr) {
            emit sigError(QString("export error: %1").arg(ErrorStr));
        }
        return false;
    }
    html.close();
    emit sigResult(true);
    return true;
}



bool LogExportThread::exportToXls(QString fileName, QStandardItemModel *pModel, LOG_FLAG flag)
{
    try {
        auto currentXlsRow = 1;
        QXlsx::Document xlsx;

        for (int col = 0; col < pModel->columnCount(); ++col) {
            auto item = pModel->horizontalHeaderItem(col);
            if (item) {
                QXlsx::Format boldFont;
                boldFont.setFontBold(true);
                xlsx.write(currentXlsRow, col + 1, item->text(), boldFont);
            }
        }
        ++currentXlsRow;

        if (flag == APP) {
            for (int row = 0; row < pModel->rowCount(); ++row) {
                if (!m_canRunning) {
                    throw  QString(stopStr);
                }
                xlsx.write(currentXlsRow, 0 + 1,
                           pModel->item(row, 0)->data(Qt::UserRole + 6).toString());
                for (int col = 1; col < pModel->columnCount(); ++col) {
                    if (!m_canRunning) {
                        throw  QString(stopStr);
                    }
                    xlsx.write(currentXlsRow, col + 1, pModel->item(row, col)->text());
                }
                ++currentXlsRow;
            }
        } else {
            for (int row = 0; row < pModel->rowCount(); ++row) {
                if (!m_canRunning) {
                    throw  QString(stopStr);
                }
                for (int col = 0; col < pModel->columnCount(); ++col) {
                    if (!m_canRunning) {
                        throw  QString(stopStr);
                    }
                    xlsx.write(currentXlsRow, col + 1, pModel->item(row, col)->text());
                }
                ++currentXlsRow;

            }
        }

        ++currentXlsRow;

        xlsx.saveAs(fileName);
    } catch (QString ErrorStr) {
        qDebug() << "Export Stop" << ErrorStr;
        emit sigResult(false);
        if (ErrorStr != stopStr) {
            emit sigError(QString("export error: %1").arg(ErrorStr));
        }
        return false;
    }
    emit sigResult(true);
    return true;
}

bool LogExportThread::exportToXls(QString fileName, QList<LOG_MSG_JOURNAL> jList,
                                  QStringList labels, LOG_FLAG iFlag)
{
    try {
        auto currentXlsRow = 1;
        QXlsx::Document xlsx;
        for (int col = 0; col < labels.count(); ++col) {
            QXlsx::Format boldFont;
            boldFont.setFontBold(true);
            xlsx.write(currentXlsRow, col + 1, labels.at(col), boldFont);
        }
        ++currentXlsRow;
        for (int row = 0; row < jList.count(); ++row) {
            if (!m_canRunning) {
                throw  QString(stopStr);
            }
            LOG_MSG_JOURNAL message = jList.at(row);
            int col = 1;

            if (iFlag == JOURNAL) {
                xlsx.write(currentXlsRow, col++, message.level);
                xlsx.write(currentXlsRow, col++, message.daemonName);
                xlsx.write(currentXlsRow, col++, message.dateTime);
                xlsx.write(currentXlsRow, col++, message.msg);
                xlsx.write(currentXlsRow, col++, message.hostName);
                xlsx.write(currentXlsRow, col++, message.daemonId);
            } else if (iFlag == KERN) {
                xlsx.write(currentXlsRow, col++, message.dateTime);
                xlsx.write(currentXlsRow, col++, message.hostName);
                xlsx.write(currentXlsRow, col++, message.daemonName);
                xlsx.write(currentXlsRow, col++, message.msg);
            }

            ++currentXlsRow;
        }
        ++currentXlsRow;
        xlsx.saveAs(fileName);
    } catch (QString ErrorStr) {
        qDebug() << "Export Stop" << ErrorStr;
        emit sigResult(false);
        if (ErrorStr != stopStr) {
            emit sigError(QString("export error: %1").arg(ErrorStr));
        }
        return false;
    }
    emit sigResult(true);
    return true;
}

bool LogExportThread::exportToXls(QString fileName, QList<LOG_MSG_APPLICATOIN> jList, QStringList labels, QString &iAppName)
{
    try {
        auto currentXlsRow = 1;
        QXlsx::Document xlsx;
        for (int col = 0; col < labels.count(); ++col) {
            QXlsx::Format boldFont;
            boldFont.setFontBold(true);
            xlsx.write(currentXlsRow, col + 1, labels.at(col), boldFont);
        }
        ++currentXlsRow;
        for (int row = 0; row < jList.count(); ++row) {
            if (!m_canRunning) {
                throw  QString(stopStr);
            }
            LOG_MSG_APPLICATOIN message = jList.at(row);
            int col = 1;
            xlsx.write(currentXlsRow, col++, strTranslate(message.level));
            xlsx.write(currentXlsRow, col++, message.dateTime);
            xlsx.write(currentXlsRow, col++, iAppName);
            xlsx.write(currentXlsRow, col++, message.msg);
            ++currentXlsRow;
        }
        ++currentXlsRow;
        xlsx.saveAs(fileName);
    } catch (QString ErrorStr) {
        qDebug() << "Export Stop" << ErrorStr;
        emit sigResult(false);
        if (ErrorStr != stopStr) {
            emit sigError(QString("export error: %1").arg(ErrorStr));
        }
        return false;
    }
    emit sigResult(true);
    return true;
}

bool LogExportThread::exportToXls(QString fileName, QList<LOG_MSG_DPKG> jList, QStringList labels)
{
    try {
        auto currentXlsRow = 1;
        QXlsx::Document xlsx;
        for (int col = 0; col < labels.count(); ++col) {
            QXlsx::Format boldFont;
            boldFont.setFontBold(true);
            xlsx.write(currentXlsRow, col + 1, labels.at(col), boldFont);
        }
        ++currentXlsRow;
        for (int row = 0; row < jList.count(); ++row) {
            if (!m_canRunning) {
                throw  QString(stopStr);
            }
            LOG_MSG_DPKG message = jList.at(row);
            int col = 1;
            xlsx.write(currentXlsRow, col++, message.dateTime);
            xlsx.write(currentXlsRow, col++, message.msg);
            xlsx.write(currentXlsRow, col++, message.action);
            ++currentXlsRow;
        }
        ++currentXlsRow;
        xlsx.saveAs(fileName);
    } catch (QString ErrorStr) {
        qDebug() << "Export Stop" << ErrorStr;
        emit sigResult(false);
        if (ErrorStr != stopStr) {
            emit sigError(QString("export error: %1").arg(ErrorStr));
        }
        return false;
    }
    emit sigResult(true);
    return true;
}

bool LogExportThread::exportToXls(QString fileName, QList<LOG_MSG_BOOT> jList, QStringList labels)
{
    try {
        auto currentXlsRow = 1;
        QXlsx::Document xlsx;
        for (int col = 0; col < labels.count(); ++col) {
            QXlsx::Format boldFont;
            boldFont.setFontBold(true);
            xlsx.write(currentXlsRow, col + 1, labels.at(col), boldFont);
        }
        ++currentXlsRow;
        for (int row = 0; row < jList.count(); ++row) {
            if (!m_canRunning) {
                throw  QString(stopStr);
            }
            LOG_MSG_BOOT message = jList.at(row);
            int col = 1;
            xlsx.write(currentXlsRow, col++, message.status);
            xlsx.write(currentXlsRow, col++, message.msg);
            ++currentXlsRow;
        }
        ++currentXlsRow;
        xlsx.saveAs(fileName);
    } catch (QString ErrorStr) {
        qDebug() << "Export Stop" << ErrorStr;
        emit sigResult(false);
        if (ErrorStr != stopStr) {
            emit sigError(QString("export error: %1").arg(ErrorStr));
        }
        return false;
    }
    emit sigResult(true);
    return true;
}

bool LogExportThread::exportToXls(QString fileName, QList<LOG_MSG_XORG> jList, QStringList labels)
{
    try {
        auto currentXlsRow = 1;
        QXlsx::Document xlsx;
        for (int col = 0; col < labels.count(); ++col) {
            QXlsx::Format boldFont;
            boldFont.setFontBold(true);
            xlsx.write(currentXlsRow, col + 1, labels.at(col), boldFont);
        }
        ++currentXlsRow;
        for (int row = 0; row < jList.count(); ++row) {
            if (!m_canRunning) {
                throw  QString(stopStr);
            }
            LOG_MSG_XORG message = jList.at(row);
            int col = 1;
            xlsx.write(currentXlsRow, col++, message.dateTime);
            xlsx.write(currentXlsRow, col++, message.msg);
            ++currentXlsRow;
        }
        ++currentXlsRow;
        xlsx.saveAs(fileName);
    } catch (QString ErrorStr) {
        qDebug() << "Export Stop" << ErrorStr;
        emit sigResult(false);
        if (ErrorStr != stopStr) {
            emit sigError(QString("export error: %1").arg(ErrorStr));
        }
        return false;
    }
    emit sigResult(true);
    return true;
}

bool LogExportThread::exportToXls(QString fileName, QList<LOG_MSG_NORMAL> jList, QStringList labels)
{
    try {
        auto currentXlsRow = 1;
        QXlsx::Document xlsx;
        for (int col = 0; col < labels.count(); ++col) {
            QXlsx::Format boldFont;
            boldFont.setFontBold(true);
            xlsx.write(currentXlsRow, col + 1, labels.at(col), boldFont);
        }
        ++currentXlsRow;
        for (int row = 0; row < jList.count(); ++row) {
            if (!m_canRunning) {
                throw  QString(stopStr);
            }
            LOG_MSG_NORMAL message = jList.at(row);
            int col = 1;
            xlsx.write(currentXlsRow, col++, message.eventType);
            xlsx.write(currentXlsRow, col++, message.userName);
            xlsx.write(currentXlsRow, col++, message.dateTime);
            xlsx.write(currentXlsRow, col++, message.msg);
            ++currentXlsRow;
        }
        ++currentXlsRow;
        xlsx.saveAs(fileName);
    } catch (QString ErrorStr) {
        qDebug() << "Export Stop" << ErrorStr;
        emit sigResult(false);
        if (ErrorStr != stopStr) {
            emit sigError(QString("export error: %1").arg(ErrorStr));
        }
        return false;
    }
    emit sigResult(true);
    return true;
}

bool LogExportThread::exportToXls(QString fileName, QList<LOG_MSG_KWIN> jList, QStringList labels)
{
    try {
        auto currentXlsRow = 1;
        QXlsx::Document xlsx;
        for (int col = 0; col < labels.count(); ++col) {
            QXlsx::Format boldFont;
            boldFont.setFontBold(true);
            xlsx.write(currentXlsRow, col + 1, labels.at(col), boldFont);
        }
        ++currentXlsRow;
        for (int row = 0; row < jList.count(); ++row) {
            if (!m_canRunning) {
                throw  QString(stopStr);
            }
            LOG_MSG_KWIN message = jList.at(row);
            int col = 1;
            xlsx.write(currentXlsRow, col++, message.msg);
            ++currentXlsRow;
        }
        ++currentXlsRow;
        xlsx.saveAs(fileName);
    } catch (QString ErrorStr) {
        qDebug() << "Export Stop" << ErrorStr;
        emit sigResult(false);
        if (ErrorStr != stopStr) {
            emit sigError(QString("export error: %1").arg(ErrorStr));
        }
        return false;
    }
    emit sigResult(true);
    return true;
}

bool LogExportThread::exportToXls(QString fileName, QList<LOG_MSG_DNF> jList, QStringList labels)
{
    try {
        auto currentXlsRow = 1;
        QXlsx::Document xlsx;
        for (int col = 0; col < labels.count(); ++col) {
            QXlsx::Format boldFont;
            boldFont.setFontBold(true);
            xlsx.write(currentXlsRow, col + 1, labels.at(col), boldFont);
        }
        ++currentXlsRow;
        for (int row = 0; row < jList.count(); ++row) {
            if (!m_canRunning) {
                throw  QString(stopStr);
            }
            LOG_MSG_DNF message = jList.at(row);
            int col = 1;
            xlsx.write(currentXlsRow, col++, message.level);
            xlsx.write(currentXlsRow, col++, message.dateTime);
            xlsx.write(currentXlsRow, col++, message.msg);
            ++currentXlsRow;
        }
        ++currentXlsRow;
        xlsx.saveAs(fileName);
    } catch (QString ErrorStr) {
        qDebug() << "Export Stop" << ErrorStr;
        emit sigResult(false);
        if (ErrorStr != stopStr) {
            emit sigError(QString("export error: %1").arg(ErrorStr));
        }
        return false;
    }
    emit sigResult(true);
    return true;
}

bool LogExportThread::exportToXls(QString fileName, QList<LOG_MSG_DMESG> jList, QStringList labels)
{
    try {
        auto currentXlsRow = 1;
        QXlsx::Document xlsx;
        for (int col = 0; col < labels.count(); ++col) {
            QXlsx::Format boldFont;
            boldFont.setFontBold(true);
            xlsx.write(currentXlsRow, col + 1, labels.at(col), boldFont);
        }
        ++currentXlsRow;
        for (int row = 0; row < jList.count(); ++row) {
            if (!m_canRunning) {
                throw  QString(stopStr);
            }
            LOG_MSG_DMESG message = jList.at(row);
            int col = 1;
            xlsx.write(currentXlsRow, col++, message.level);
            xlsx.write(currentXlsRow, col++, message.dateTime);
            xlsx.write(currentXlsRow, col++, message.msg);
            ++currentXlsRow;
        }
        ++currentXlsRow;
        xlsx.saveAs(fileName);
    } catch (QString ErrorStr) {
        qDebug() << "Export Stop" << ErrorStr;
        emit sigResult(false);
        if (ErrorStr != stopStr) {
            emit sigError(QString("export error: %1").arg(ErrorStr));
        }
        return false;
    }
    emit sigResult(true);
    return true;
}

void LogExportThread::initMap()
{
    m_levelStrMap.clear();
    m_levelStrMap.insert("Emergency", DApplication::translate("Level", "Emergency"));
    m_levelStrMap.insert("Alert", DApplication::translate("Level", "Alert"));
    m_levelStrMap.insert("Critical", DApplication::translate("Level", "Critical"));
    m_levelStrMap.insert("Error", DApplication::translate("Level", "Error"));
    m_levelStrMap.insert("Warning", DApplication::translate("Level", "Warning"));
    m_levelStrMap.insert("Notice", DApplication::translate("Level", "Notice"));
    m_levelStrMap.insert("Info", DApplication::translate("Level", "Info"));
    m_levelStrMap.insert("Debug", DApplication::translate("Level", "Debug"));
}

QString LogExportThread::strTranslate(QString &iLevelStr)
{
    return m_levelStrMap.value(iLevelStr, iLevelStr);
}





void LogExportThread::run()
{
    switch (m_runMode) {
    case TxtModel: {
        exportToTxt(m_fileName, m_pModel, m_flag);
        break;
    }
    case TxtJOURNAL: {
        exportToTxt(m_fileName, m_jList, m_labels, m_flag);
        break;
    }
    case TxtAPP: {
        exportToTxt(m_fileName, m_appList, m_labels, m_appName);
        break;
    }
    case TxtDPKG: {
        exportToTxt(m_fileName, m_dpkgList, m_labels);
        break;
    }
    case TxtBOOT: {
        exportToTxt(m_fileName, m_bootList, m_labels);
        break;
    }
    case TxtXORG: {
        exportToTxt(m_fileName, m_xorgList, m_labels);
        break;
    }
    case TxtNORMAL: {
        exportToTxt(m_fileName, m_normalList, m_labels);
        break;
    }
    case TxtKWIN: {
        exportToTxt(m_fileName, m_kwinList, m_labels);
        break;
    }
    case TxtDNF: {
        exportToTxt(m_fileName, m_dnfList, m_labels);
        break;
    }
    case TxtDMESG: {
        exportToTxt(m_fileName, m_dmesgList, m_labels);
        break;
    }
    case HtmlModel: {
        exportToHtml(m_fileName, m_pModel, m_flag);
        break;
    }
    case HtmlJOURNAL: {
        exportToHtml(m_fileName, m_jList, m_labels, m_flag);
        break;
    }
    case HtmlAPP: {
        exportToHtml(m_fileName, m_appList, m_labels, m_appName);
        break;
    }
    case HtmlDPKG: {
        exportToHtml(m_fileName, m_dpkgList, m_labels);
        break;
    }
    case HtmlBOOT: {
        exportToHtml(m_fileName, m_bootList, m_labels);
        break;
    }
    case HtmlXORG: {
        exportToHtml(m_fileName, m_xorgList, m_labels);
        break;
    }
    case HtmlNORMAL: {
        exportToHtml(m_fileName, m_normalList, m_labels);
        break;
    }
    case HtmlKWIN: {
        exportToHtml(m_fileName, m_kwinList, m_labels);
        break;
    }
    case HtmlDNF: {
        exportToHtml(m_fileName, m_dnfList, m_labels);
        break;
    }
    case HtmlDMESG: {
        exportToHtml(m_fileName, m_dmesgList, m_labels);
        break;
    }
    case DocModel: {
        exportToDoc(m_fileName, m_pModel, m_flag);
        break;
    }
    case DocJOURNAL: {
        exportToDoc(m_fileName, m_jList, m_labels, m_flag);
        break;
    }
    case DocAPP: {
        exportToDoc(m_fileName, m_appList, m_labels, m_appName);
        break;
    }
    case DocDPKG: {
        exportToDoc(m_fileName, m_dpkgList, m_labels);
        break;
    }
    case DocBOOT: {
        exportToDoc(m_fileName, m_bootList, m_labels);
        break;
    }
    case DocXORG: {
        exportToDoc(m_fileName, m_xorgList, m_labels);
        break;
    }
    case DocNORMAL: {
        exportToDoc(m_fileName, m_normalList, m_labels);
        break;
    }
    case DocKWIN: {
        exportToDoc(m_fileName, m_kwinList, m_labels);
        break;
    }
    case DocDNF: {
        exportToDoc(m_fileName, m_dnfList, m_labels);
        break;
    }
    case DocDMESG: {
        exportToDoc(m_fileName, m_dmesgList, m_labels);
        break;
    }
    case XlsModel: {
        exportToXls(m_fileName, m_pModel, m_flag);
        break;
    }
    case XlsJOURNAL: {
        exportToXls(m_fileName, m_jList, m_labels, m_flag);
        break;
    }
    case XlsAPP: {
        exportToXls(m_fileName, m_appList, m_labels, m_appName);
        break;
    }
    case XlsDPKG: {
        exportToXls(m_fileName, m_dpkgList, m_labels);
        break;
    }
    case XlsBOOT: {
        exportToXls(m_fileName, m_bootList, m_labels);
        break;
    }
    case XlsXORG: {
        exportToXls(m_fileName, m_xorgList, m_labels);
        break;
    }
    case XlsNORMAL: {
        exportToXls(m_fileName, m_normalList, m_labels);
        break;
    }
    case XlsKWIN: {
        exportToXls(m_fileName, m_kwinList, m_labels);
        break;
    }
    case XlsDNF: {
        exportToXls(m_fileName, m_dnfList, m_labels);
        break;
    }
    case XlsDMESG: {
        exportToXls(m_fileName, m_dmesgList, m_labels);
        break;
    }
    default:
        break;
    }
    m_canRunning = false;
}
