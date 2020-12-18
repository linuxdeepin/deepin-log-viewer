/*
* Copyright (C) 2019 ~ 2020 UnionTech Software Technology Co.,Ltd
*
* Author:     zyc <zyc@uniontech.com>
* Maintainer:  zyc <zyc@uniontech.com>
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
#ifndef LOGTREEBASEMODEL_H
#define LOGTREEBASEMODEL_H

#include <QObject>
#include <QAbstractTableModel>
#include "fileloader/logdatacontrol.h"
template < typename DataType, typename FilterType>
class LogTreeBaseModel : public QAbstractTableModel
{
public:
    explicit LogTreeBaseModel(QObject *parent = nullptr)
    {

    }

    int rowCount(const QModelIndex &parent = {}) const override;
    void setDataControl(LoghDataControl<DataType, FilterType>  iDataControl);
    Qt::ItemFlags flags(const QModelIndex &index) const override
    {
        if (!index.isValid())
            return Qt::NoItemFlags;

        return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    }
    void refreshPage(bool isAdd, bool iReset = false)
    {
        beginResetModel();
        if (m_DataControl->refreshPage(isAdd, iReset)) {
            m_DataControl->m_currentData;
        }

        endResetModel();
    }
signals:

public slots:
public:
    LoghDataControl<DataType, FilterType> *m_DataControl;
};

#endif // LOGTREEBASEMODEL_H
