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
#ifndef BASEDATAFILTER_H
#define BASEDATAFILTER_H

#include <QObject>
#include <QThread>
template < typename DataType, typename FilterType >
class BaseDataFilter : public QThread
{
    // Q_OBJECT
public:
    explicit BaseDataFilter(QObject *parent = nullptr)
    {

    }
    ~BaseDataFilter() override
    {

    }

signals:

public slots:

public:

};

#endif // BASEDATAFILTER_H
