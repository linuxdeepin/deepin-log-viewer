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
    /**
     * @brief instance 单例构造
     * @return　本对象全局单例
     */
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





    /**
     * @brief m_winInfoConfig 窗口配置对象
     */
    QSettings *m_winInfoConfig ;
    /**
     * @brief m_logDirConfig 日志文件路径配置对象
     */
    QSettings *m_logDirConfig ;
    /**
     * @brief m_configPath 窗口配置文件路径
     */
    QString m_configPath;
    /**
     * @brief m_logDirPath 日志文件路径配置的文件路径
     */
    QString m_logDirPath;
    QSize getConfigWinSize();
    void saveConfigWinSize(int w, int h);
    void saveLogDir(const QString &iKey, const QString &iDir);
    QString getLogDir(const QString &iKey);
signals:

public slots:
private:
    static std::atomic<LogSettings *> m_instance;
    static std::mutex m_mutex;
};

#endif // LOGSETTINGS_H
