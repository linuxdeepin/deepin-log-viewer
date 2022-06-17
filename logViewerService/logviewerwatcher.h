/*
* Copyright (C) 2019 ~ 2021 Uniontech Software Technology Co.,Ltd.
*
* Author:     linxun <linxun@uniontech.com>
*
* Maintainer: linxun <linxun@uniontech.com>
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

#ifndef LOGVIEWERWATCHER_H
#define LOGVIEWERWATCHER_H

#include <QObject>
#include <QTimer>

/**
 * @class LogViewerWatcher
 * @brief 监控客户端类
 */
class LogViewerWatcher :public QObject
{
    Q_OBJECT
public:
    explicit LogViewerWatcher();

public Q_SLOTS:
    void onTimeOut();
private:
    QString executCmd(const QString &strCmd);
private:
    QTimer *m_Timer=nullptr;
};

#endif // LOGVIEWERWATCHER_H
