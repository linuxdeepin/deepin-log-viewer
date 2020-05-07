/*
* Copyright (C) 2019 ~ 2020 Deepin Technology Co., Ltd.
*
* Author:     zhangyichi <zhangyichi@uniontech.com>
* Maintainer: zhangyichi <zhangyichi@uniontech.com>
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* any later version.
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "log_sort_filter_proxy_model.h"
#include "structdef.h"
#include "common/collator.h"
#include <DApplication>
#include <QDateTime>
#include <QDebug>
DWIDGET_USE_NAMESPACE
LogSortFilterProxyModel::LogSortFilterProxyModel(QObject *parent)
    : QSortFilterProxyModel(parent)
{

}

void LogSortFilterProxyModel::intDateFomulaList()
{
    m_DateFomulaList.clear();
    m_DateFomulaList.append("yyyy-MM-dd hh:mm:ss");
    m_DateFomulaList.append("MMM dd hh:mm:ss");
}

bool LogSortFilterProxyModel::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
    if (left.data(Qt::UserRole + 1).toString() == JOUR_TABLE_DATA) {
        return  journalLessThan(left, right);
    } else if (left.data(Qt::UserRole + 1).toString() == KERN_TABLE_DATA) {
        return  kernLessThan(left, right);
    } else if (left.data(Qt::UserRole + 1).toString() == BOOT_TABLE_DATA) {
        return  bootLessThan(left, right);
    } else if (left.data(Qt::UserRole + 1).toString() == XORG_TABLE_DATA) {
        return  xorgLessThan(left, right);
    } else if (left.data(Qt::UserRole + 1).toString() == DPKG_TABLE_DATA) {
        return  dpkgLessThan(left, right);
    } else if (left.data(Qt::UserRole + 1).toString() == APP_TABLE_DATA) {
        return  appLessThan(left, right);
    } else if (left.data(Qt::UserRole + 1).toString() == LAST_TABLE_DATA) {
        return  normalLessThan(left, right);
    }
    return QSortFilterProxyModel::lessThan(left, right);
}

bool LogSortFilterProxyModel::journalLessThan(const QModelIndex &left, const QModelIndex &right) const
{
    switch (sortColumn()) {
    case JOURNAL_SPACE::journalLevelColumn: {
        QString leftLevelStr =  left.data(Log_Item_SPACE::levelRole).toString();
        QString rightLevelStr =  right.data(Log_Item_SPACE::levelRole).toString();
        int leftLevel = levelStr2Int(leftLevelStr);
        int rightLevel = levelStr2Int(rightLevelStr);
        if (leftLevel == rightLevel) {
            return  dateTimeSiblingLessThan(left, right, JOURNAL_SPACE::journalDateTimeColumn);
        } else {
            return  leftLevel < rightLevel;
        }
    }
    case JOURNAL_SPACE::journalDaemonNameColumn: {
        QString leftStr = left.data(Qt::DisplayRole).toString();
        QString rightStr = right.data(Qt::DisplayRole).toString();
        int compareResult = Collator::instance()->compare(leftStr, rightStr);
        if (compareResult == 0) {
            return dateTimeSiblingLessThan(left, right, JOURNAL_SPACE::journalDateTimeColumn);
        } else {
            return compareResult == -1;
        }
    }
    case JOURNAL_SPACE::journalDateTimeColumn: {
        return  dateTimeSiblingLessThan(left, right, JOURNAL_SPACE::journalDateTimeColumn);
    }
    case JOURNAL_SPACE::journalMsgColumn:
    case JOURNAL_SPACE::journalHostNameColumn:
    case JOURNAL_SPACE::journalDaemonIdColumn: {
        QString leftStr = left.data(Qt::DisplayRole).toString();
        QString rightStr = right.data(Qt::DisplayRole).toString();
        int compareResult = Collator::instance()->compare(leftStr, rightStr);
        if (compareResult == 0) {
            return dateTimeSiblingLessThan(left, right, JOURNAL_SPACE::journalDateTimeColumn);
        } else {
            return compareResult == -1;
        }
    }
    default:
        break;
    }

    return QSortFilterProxyModel::lessThan(left, right);
}

bool LogSortFilterProxyModel::kernLessThan(const QModelIndex &left, const QModelIndex &right) const
{
    switch (sortColumn()) {
    case KERN_SPACE::kernDateTimeColumn: {
        return  dateTimeSiblingLessThan(left, right, KERN_SPACE::kernDateTimeColumn);
    }
    case KERN_SPACE::kernHostNameColumn:
    case KERN_SPACE::kernDaemonNameColumn:
    case KERN_SPACE::kernMsgColumn: {
        QString leftStr = left.data(Qt::DisplayRole).toString();
        QString rightStr = right.data(Qt::DisplayRole).toString();
        int compareResult = Collator::instance()->compare(leftStr, rightStr);
        if (compareResult == 0) {
            return dateTimeSiblingLessThan(left, right, KERN_SPACE::kernDateTimeColumn);
        } else {
            return compareResult == -1;
        }
    }
    default:
        break;
    }
    return QSortFilterProxyModel::lessThan(left, right);
}

bool LogSortFilterProxyModel::bootLessThan(const QModelIndex &left, const QModelIndex &right) const
{
    return QSortFilterProxyModel::lessThan(left, right);
}

bool LogSortFilterProxyModel::xorgLessThan(const QModelIndex &left, const QModelIndex &right) const
{
    switch (sortColumn()) {
    case XORG_SPACE::xorgDateTimeColumn: {
        return  dateTimeSiblingLessThan(left, right, XORG_SPACE::xorgDateTimeColumn);
    }
    case XORG_SPACE::xorgMsgColumn: {
        QString leftStr = left.data(Qt::DisplayRole).toString();
        QString rightStr = right.data(Qt::DisplayRole).toString();
        int compareResult = Collator::instance()->compare(leftStr, rightStr);
        if (compareResult == 0) {
            return dateTimeSiblingLessThan(left, right, XORG_SPACE::xorgDateTimeColumn);
        } else {
            return compareResult == -1;
        }
    }
    default:
        break;
    }
    return QSortFilterProxyModel::lessThan(left, right);
}

bool LogSortFilterProxyModel::dpkgLessThan(const QModelIndex &left, const QModelIndex &right) const
{
    switch (sortColumn()) {
    case DKPG_SPACE::dkpgDateTimeColumn: {
        return  dateTimeSiblingLessThan(left, right, DKPG_SPACE::dkpgDateTimeColumn);
    }
    case DKPG_SPACE::dkpgMsgColumn: {
        QString leftStr = left.data(Qt::DisplayRole).toString();
        QString rightStr = right.data(Qt::DisplayRole).toString();
        int compareResult = Collator::instance()->compare(leftStr, rightStr);
        if (compareResult == 0) {
            return dateTimeSiblingLessThan(left, right, DKPG_SPACE::dkpgDateTimeColumn);
        } else {
            return compareResult == -1;
        }
    }
    default:
        break;
    }
    return QSortFilterProxyModel::lessThan(left, right);
}

bool LogSortFilterProxyModel::appLessThan(const QModelIndex &left, const QModelIndex &right) const
{
    switch (sortColumn()) {
    case APP_SPACE::appLevelColumn: {
        QString leftLevelStr =  left.data(Log_Item_SPACE::levelRole).toString();
        QString rightLevelStr =  right.data(Log_Item_SPACE::levelRole).toString();
        int leftLevel = levelStr2Int(leftLevelStr);
        int rightLevel = levelStr2Int(rightLevelStr);
        if (leftLevel == rightLevel) {
            return  dateTimeSiblingLessThan(left, right, APP_SPACE::appDateTimeColumn);
        } else {
            return  leftLevel < rightLevel;
        }
    }
    case APP_SPACE::appDateTimeColumn: {
        return  dateTimeSiblingLessThan(left, right, APP_SPACE::appDateTimeColumn);
    }
    case APP_SPACE::appSrcColumn:
    case APP_SPACE::appMsgColumn: {
        QString leftStr = left.data(Qt::DisplayRole).toString();
        QString rightStr = right.data(Qt::DisplayRole).toString();
        int compareResult = Collator::instance()->compare(leftStr, rightStr);
        if (compareResult == 0) {
            return dateTimeSiblingLessThan(left, right, APP_SPACE::appDateTimeColumn);
        } else {
            return compareResult == -1;
        }
    }
    default:
        break;
    }

    return QSortFilterProxyModel::lessThan(left, right);
}

bool LogSortFilterProxyModel::normalLessThan(const QModelIndex &left, const QModelIndex &right) const
{
    switch (sortColumn()) {

    case NORMAL_SPACE::normalDateTimeColumn: {
        return  dateTimeSiblingLessThan(left, right, NORMAL_SPACE::normalDateTimeColumn);
    }
    case NORMAL_SPACE::normalEventTypeColumn: {
        QString leftStr = left.data(Qt::DisplayRole).toString();
        QString rightStr = right.data(Qt::DisplayRole).toString();
        int leftType = normalEventTypeStr2Int(leftStr);
        int rightype = normalEventTypeStr2Int(rightStr);
        int compareResult = leftType - rightype;
        if (compareResult == 0) {
            return dateTimeSiblingLessThan(left, right, NORMAL_SPACE::normalDateTimeColumn);
        } else {
            return compareResult < 0;
        }
    }
    case NORMAL_SPACE::normalUserNameColumn: {
        QString leftStr = left.data(Qt::DisplayRole).toString();
        QString rightStr = right.data(Qt::DisplayRole).toString();
        int compareResult = Collator::instance()->compare(leftStr, rightStr);
        if (compareResult == 0) {
            return dateTimeSiblingLessThan(left, right, NORMAL_SPACE::normalDateTimeColumn);
        } else {
            return compareResult == -1;
        }
    }
    case NORMAL_SPACE::normalMsgColumn: {
        QString leftStr = left.data(Qt::DisplayRole).toString();
        QString rightStr = right.data(Qt::DisplayRole).toString();
        int compareResult = Collator::instance()->compare(leftStr, rightStr);
        if (compareResult == 0) {
            return dateTimeSiblingLessThan(left, right, NORMAL_SPACE::normalDateTimeColumn);
        } else {
            return compareResult == -1;
        }
    }
    default:
        break;
    }
    return QSortFilterProxyModel::lessThan(left, right);
}

int LogSortFilterProxyModel::levelStr2Int(const QString &iLevelStr) const
{
    int level = -99;
    if (iLevelStr == DApplication::translate("Level", "Emergency")) {
        level = 0;
    } else if (iLevelStr == DApplication::translate("Level", "Alert")) {
        level = 1;
    } else if (iLevelStr == DApplication::translate("Level", "Critical")) {
        level = 2;
    } else if (iLevelStr == DApplication::translate("Level", "Error")) {
        level = 3;
    } else if (iLevelStr == DApplication::translate("Level", "Warning")) {
        level = 4;
    } else if (iLevelStr == DApplication::translate("Level", "Notice")) {
        level = 5;
    } else if (iLevelStr == DApplication::translate("Level", "Info")) {
        level = 6;
    } else if (iLevelStr == DApplication::translate("Level", "Debug")) {
        level = 7;
    }
    return level;
}

int LogSortFilterProxyModel::normalEventTypeStr2Int(const QString &iLevelStr) const
{
    int level = -99;
    if (iLevelStr == "Login") {
        level = 0;
    } else if (iLevelStr == "Boot") {
        level = 1;
    } else if (iLevelStr == "shutdown") {
        level = 2;
    }
    return level;
}

QDateTime LogSortFilterProxyModel::str2DateTime(const QString &iLevelStr) const
{
    QDateTime resultDateTime;
    foreach (QString fomulastr, m_DateFomulaList) {
        resultDateTime = QDateTime::fromString(iLevelStr, fomulastr);
        if (resultDateTime.isValid()) {
            return resultDateTime;
        }
    }
    return resultDateTime;
}

bool LogSortFilterProxyModel::dateTimeSiblingLessThan(const QModelIndex &left, const QModelIndex &right, int row) const
{
    QDateTime leftDateTime = str2DateTime(left.sibling(left.row(), row)
                                          .data(Qt::DisplayRole).toString());
    QDateTime rightDateTime = str2DateTime(right.sibling(right.row(), row)
                                           .data(Qt::DisplayRole).toString());
    if (!leftDateTime.isValid()) {
        return  true;
    }
    if (!rightDateTime.isValid()) {
        return  false;
    }
    return  leftDateTime > rightDateTime;
}

