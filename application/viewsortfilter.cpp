/*
* Copyright (C) 2021 ~ 2022 Uniontech Software Technology Co.,Ltd.
*
* Author:     liuyanga <liuyanga@uniontech.com>
*
* Maintainer: liuyanga <liuyanga@uniontech.com>
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
#include "viewsortfilter.h"
#include <QDebug>
#include <QDateTime>

ViewSortfilter::ViewSortfilter(QObject *parent)
    : QSortFilterProxyModel(parent)
{
}

bool ViewSortfilter::lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const
{
    QDateTime time_left = source_left.data(Qt::UserRole + 2).toDateTime();
    QDateTime time_rigiht = source_right.data(Qt::UserRole + 2).toDateTime();
    if (time_left.isValid() && time_rigiht.isValid()) {
        return time_left < time_rigiht;
    }
    return QSortFilterProxyModel::lessThan(source_left, source_right);
}
