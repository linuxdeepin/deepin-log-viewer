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
#ifndef LOGAPPLICATIONHELPER_H
#define LOGAPPLICATIONHELPER_H

#include <QMap>
#include <QObject>

#include <mutex>

class LogApplicationHelper : public QObject
{
    Q_OBJECT
public:
    static LogApplicationHelper *instance()
    {
        LogApplicationHelper *sin = m_instance.load();
        if (!sin) {
            std::lock_guard<std::mutex> lock(m_mutex);
            sin = m_instance.load();
            if (!sin) {
                sin = new LogApplicationHelper();
                m_instance.store(sin);
            }
        }
        return sin;
    }

    QMap<QString, QString> getMap();

    QString transName(QString str);

private:
    explicit LogApplicationHelper(QObject *parent = nullptr);

    void init();

    void createDesktopFiles();
    void createLogFiles();

    void parseField(QString path, QString name, bool isDeepin, bool isGeneric, bool isName);

    QString getLogFile(QString path);

signals:

public slots:

private:
    QMap<QString, QString> m_en_log_map;
    QMap<QString, QString> m_en_trans_map;

    QMap<QString, QString> m_trans_log_map;

    QStringList m_desktop_files;
    QStringList m_log_files;

    QString m_current_system_language;

    static std::atomic<LogApplicationHelper *> m_instance;
    static std::mutex m_mutex;
};

#endif  // LOGAPPLICATIONHELPER_H
