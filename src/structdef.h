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

#ifndef STRUCTDEF_H
#define STRUCTDEF_H
#include <QString>

struct LOG_MSG_JOURNAL {
    // include dateTime level type detailInfo...
    QString dateTime;
    QString hostName;
    QString daemonName;
    QString daemonId;
    QString level;
    QString msg;
};

struct LOG_MSG_DPKG {
    QString dateTime;
    QString action;
    QString msg;
};

struct LOG_MSG_BOOT {
    QString status;
    QString msg;
};

struct LOG_MSG_APPLICATOIN {
    QString dateTime;
    QString level;
    QString src;
    QString msg;
};

struct LOG_MSG_XORG {
    QString dateTime;
    QString msg;
};

// add by Airy
struct LOG_MSG_NORMAL {
    QString eventType;
    QString userName;
    QString dateTime;
    QString msg;
};

enum PRIORITY { LVALL = -1, EMER, ALERT, CRI, ERR, WARN, NOTICE, INF, DEB };

enum BUTTONID {
    ALL = 0,
    ONE_DAY,
    THREE_DAYS,
    ONE_WEEK,
    ONE_MONTH,
    THREE_MONTHS,
    RESET,
    EXPORT,
    INVALID = 9999
};

enum LOG_FLAG {
    JOURNAL = 0,
    KERN,
    BOOT,
    XORG,
    DPKG,
    APP,
    Normal,
    NONE = 9999
};  // modified by
    // Airy

#define DPKG_TABLE_DATA "dpkgItemData"
#define XORG_TABLE_DATA "XorgItemData"
#define BOOT_TABLE_DATA "bootItemData"
#define KERN_TABLE_DATA "kernItemData"
#define JOUR_TABLE_DATA "journalItemData"
#define APP_TABLE_DATA "applicationItemData"
#define LAST_TABLE_DATA "lastItemData"  // add by Airy

#define JOUR_TREE_DATA "journalctl"
#define DPKG_TREE_DATA "/var/log/dpkg.log"
#define XORG_TREE_DATA "/var/log/Xorg.0.log"
#define BOOT_TREE_DATA "/var/log/boot.log"
#define KERN_TREE_DATA "/var/log/kern.log"
#define APP_TREE_DATA "application"

#define LAST_TREE_DATA "last"  // add by Airy
#define ITEM_DATE_ROLE (Qt::UserRole + 66)

#endif  // STRUCTDEF_H
