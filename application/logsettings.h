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
#ifndef LOGSETTINGS_H
#define LOGSETTINGS_H

#include <QObject>
#include <QSettings>

#include <mutex>
#define MAINWINDOW_WIDTH 1024
#define MAINWINDOW_HEIGHT 736
class LogSettings : public QObject
{
    Q_OBJECT
public:
    explicit LogSettings(QObject *parent = nullptr);

    static LogSettings *instance()
    {
        LogSettings *sin = m_instance.load();
        if (!sin) {
            std::lock_guard<std::mutex> lock(m_mutex);
            sin = m_instance.load();
            if (!sin) {
                sin = new LogSettings();
                m_instance.store(sin);
            }
        }
        return sin;
    }






    QSettings *m_winInfoConfig = nullptr;
    QString m_configPath;
    QSize getConfigWinSize();
    void saveConfigWinSize(int w, int h);
signals:

public slots:
private:
    static std::atomic<LogSettings *> m_instance;
    static std::mutex m_mutex;
};

#endif // LOGSETTINGS_H
