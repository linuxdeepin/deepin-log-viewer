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
#ifndef LOGALLEXPORTTHREAD_H
#define LOGALLEXPORTTHREAD_H

#include "structdef.h"

#include <QObject>
#include <QRunnable>

class LogAllExportThread : public QObject
    , public QRunnable
{
    Q_OBJECT
public:
    explicit LogAllExportThread(const QStringList &types, const QString &outfile, QObject *parent = nullptr);
public slots:
    void slot_cancelExport() { m_cancel = true; }

protected:
    void run() override;
signals:
    void updatecurrentProcess(int current);
    void updateTolProcess(int tol);
    void exportFinsh(bool success = true);

private:
    QStringList m_types;
    QString m_outfile {""};

    bool m_cancel {false};
};

#endif // LOGALLEXPORTTHREAD_H
