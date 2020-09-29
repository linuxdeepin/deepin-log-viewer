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

/**
 * @brief The LogApplicationHelper class 获取应用日志文件路径信息工具类
 */
class LogApplicationHelper : public QObject
{
    Q_OBJECT
public:
    /**
     * @brief instance 全局单列模式实现
     * @return 此类的唯一对象
     */
    static LogApplicationHelper *instance()
    {
        LogApplicationHelper *sin = m_instance.load();
        if (!sin) {
            //加锁 线程安全
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
    /**
     * @brief m_en_log_map 应用包名-日志路径键值对
     */
    QMap<QString, QString> m_en_log_map;
    /**
     * @brief m_en_trans_map 应用包名-应用显示文本键值对
     */
    QMap<QString, QString> m_en_trans_map;
    /**
     * @brief m_trans_log_map 应用显示文本-日志路径键值对
     */
    QMap<QString, QString> m_trans_log_map;
    /**
     * @brief m_desktop_files 所有符合条件的应用的desktop文件路径
     */
    QStringList m_desktop_files;
    /**
     * @brief m_log_files 所有符合条件的应用的日志文件路径
     */
    QStringList m_log_files;
    /**
     * @brief m_current_system_language 系统语言
     */
    QString m_current_system_language;
    /**
     * @brief m_instance 单例用的本类指针的原子性封装
     */
    static std::atomic<LogApplicationHelper *> m_instance;
    /**
     * @brief m_mutex 单例用的锁
     */
    static std::mutex m_mutex;
};

#endif  // LOGAPPLICATIONHELPER_H
