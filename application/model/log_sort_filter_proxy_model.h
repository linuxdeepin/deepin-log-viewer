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
#ifndef LOG_SORT_FILTER_PROXY_MODEL_H
#define LOG_SORT_FILTER_PROXY_MODEL_H

#include <QSortFilterProxyModel>
class QDateTime;
class LogSortFilterProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    explicit LogSortFilterProxyModel(QObject *parent = nullptr);
public:
    void intDateFomulaList();
public:
    QStringList m_DateFomulaList;
protected:
    bool lessThan(const QModelIndex &left, const QModelIndex &right) const override;
private:
    bool journalLessThan(const QModelIndex &left, const QModelIndex &right) const;
    bool kernLessThan(const QModelIndex &left, const QModelIndex &right) const;
    bool bootLessThan(const QModelIndex &left, const QModelIndex &right) const;
    bool xorgLessThan(const QModelIndex &left, const QModelIndex &right) const;
    bool dpkgLessThan(const QModelIndex &left, const QModelIndex &right) const;
    bool appLessThan(const QModelIndex &left, const QModelIndex &right) const;
    bool normalLessThan(const QModelIndex &left, const QModelIndex &right) const;
    int levelStr2Int(const QString &iLevelStr) const;
    int normalEventTypeStr2Int(const QString &iLevelStr) const;
    QDateTime str2DateTime(const QString &iLevelStr) const;
    bool dateTimeSiblingLessThan(const QModelIndex &left, const QModelIndex &right, int row) const;



};

#endif // LOG_SORT_FILTER_PROXY_MODEL_H
