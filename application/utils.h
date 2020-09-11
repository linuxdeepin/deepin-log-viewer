/*
 * Copyright (C) 2019 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     LZ <zhou.lu@archermind.com>
 *
 * Maintainer: LZ <zhou.lu@archermind.com>
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

/*
 * Copyright (C) 2017 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     rekols <rekols@foxmail.com>
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

#ifndef UTILS_H
#define UTILS_H

#include <QObject>
#include <QHash>
/**
 * @brief 公用工具静态函数类
 */
class Utils : public QObject
{
    Q_OBJECT

public:
    Utils(QObject *parent = nullptr);
    ~Utils();
    /**
     * @brief The CommandErrorType enum
     * QProccess命令返回日志的过程中如果发生报错的类型
     */
    enum CommandErrorType {
        NoError = 0x0000,//无错误，预备，一般不用
        PermissionError = 0x0001,//权限问题
        RetryError = 0x0002//返回请重试的情况
    };
    static QHash<QString, QPixmap> m_imgCacheHash;
    static QHash<QString, QString> m_fontNameCache;

    static QString getQssContent(const QString &filePath);
    static QString getConfigPath();
    static bool isFontMimeType(const QString &filePath);
    static QString suffixList();
    static QPixmap renderSVG(const QString &filePath, const QSize &size);
    static QString loadFontFamilyFromFiles(const QString &fontFileName);
    static QByteArray replaceEmptyByteArray(QByteArray &iReplaceStr);
    static CommandErrorType isErroCommand(QString str);
    static bool checkAndDeleteDir(const QString &iFilePath);
    static bool deleteDir(const QString &iFilePath);
    static void replaceColorfulFont(QString *iStr);
};


#endif
