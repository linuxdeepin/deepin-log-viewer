// SPDX-FileCopyrightText: 2019 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef STRUCTDEF_H
#define STRUCTDEF_H
#include <QString>
#include <QDir>

#define DPKG_TABLE_DATA "dpkgItemData"
#define XORG_TABLE_DATA "XorgItemData"
#define BOOT_TABLE_DATA "bootItemData"
#define KERN_TABLE_DATA "kernItemData"
#define JOUR_TABLE_DATA "journalItemData"
#define BOOT_KLU_TABLE_DATA "bootKluItemData"
#define APP_TABLE_DATA "applicationItemData"
#define LAST_TABLE_DATA "lastItemData"  // add by Airy
#define KWIN_TABLE_DATA "kwinItemData"
#define DMESG_TABLE_DATA "dmesgItemData"
#define DNF_TABLE_DATA "dnfItemData"
#define OOC_TABLE_DATA "OOCItemData"

#define JOUR_TREE_DATA "journalctl"
#define BOOT_KLU_TREE_DATA "bootklu"
#define DPKG_TREE_DATA "/var/log/dpkg.log"
#define XORG_TREE_DATA "/var/log/Xorg.0.log"
#define KWIN_TREE_DATA QDir::homePath() + "/.kwin.log"
#define BOOT_TREE_DATA "/var/log/boot.log"
#define KERN_TREE_DATA "/var/log/kern.log"
#define APP_TREE_DATA "application"
#define LAST_TREE_DATA "last"  // add by Airy
#define DNF_TREE_DATA "/var/log/dnf.log"
#define DMESG_TREE_DATA "dmesg"
#define OTHER_TREE_DATA "other log"
#define CUSTOM_TREE_DATA "custom log"

#define ITEM_DATE_ROLE (Qt::UserRole + 66)
#define ICONPREFIX "://images/"
#define ICONLIGHTPREFIX "://images/light/"
#define ICONDARKPREFIX "://images/dark/"
#define DOCTEMPLATE "://doc_template/template.doc"

enum PRIORITY { LVALL = -1,
                EMER,
                ALERT,
                CRI,
                ERR,
                WARN,
                NOTICE,
                INF,
                DEB };
enum DNFPRIORITY { DNFLVALL = -1,
                   TRACE,
                   SUBDEBUG,
                   DDEBUG,
                   DEBUG,
                   INFO,
                   WARNING,
                   ERROR,
                   CRITICAL,
                   SUPERCRITICAL };
Q_DECLARE_METATYPE(DNFPRIORITY)
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

struct LOG_MSG_DNF {
    QString dateTime;
    QString level;
    QString msg;
};

struct LOG_MSG_DMESG {
    QString level;
    QString dateTime;
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
    QString detailInfo;
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
struct LOG_MSG_KWIN {
    QString msg;
};
struct LOG_FILE_OTHERORCUSTOM {
    QString name;
    QString path;
    QString dateTimeModify;  
};

//kwin筛选条件，kwin日志只有信息，没有任何可筛选的，但是先放在这，以后统一化
struct KWIN_FILTERS {
    QString msg;
};
struct XORG_FILTERS {
    qint64 timeFilterBegin = -1 ;
    qint64 timeFilterEnd = -1;
};
struct DKPG_FILTERS {
    qint64 timeFilterBegin = -1 ;
    qint64 timeFilterEnd = -1;
};
struct APP_FILTERS {
    qint64 timeFilterBegin = -1 ;
    qint64 timeFilterEnd = -1;
    int lvlFilter;
    QString path;
};
struct JOURNAL_FILTERS {
    int eventTypeFilter = -99;
    int timeFilter = -99;

};
struct DMESG_FILTERS {
    qint64 timeFilter;
    PRIORITY levelFilter;
};
struct DNF_FILTERS {
    qint64 timeFilter;
    DNFPRIORITY levelfilter;
};

/**
 * @brief The NORMAL_FILTERS struct 开关机日志筛选条件
 */
struct NORMAL_FILTERS {
    qint64 timeFilterBegin = -1 ; //筛选开始时间
    qint64 timeFilterEnd = -1; //筛选结束时间
    int eventTypeFilter = 0; //筛选类型, 有 0全部 1登陆 2开机 3关机
    QString searchstr = ""; //搜索关键字
};

struct KERN_FILTERS {
    qint64 timeFilterBegin = -1 ;
    qint64 timeFilterEnd = -1;
};
/**
 * @brief The BOOT_FILTERS struct 启动日志筛选条件
 */
struct BOOT_FILTERS {
    QString searchstr;//搜索关键字
    QString statusFilter; //状态筛选,有 ALL OK failed

};

/**
 * @brief The FILTER_CONFIG struct 筛选控件中的筛选情况记录结构体
 */
struct FILTER_CONFIG {
    int levelCbx = INF + 1; //等级筛选的筛选值
    QString appListCbx = ""; //应用日志路径
    int statusCbx = 0; //启动日志状态筛选下拉框的值
    int dateBtn = 0; //时间筛选按钮当前选择筛选按钮对应BUTTONID
    int typeCbx = 0;
    int dnfCbx = 5;
};
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
    Kwin,
    BOOT_KLU,
    Dnf,
    Dmesg,
    OtherLog,
    CustomLog,
    NONE = 9999
}; // modified by
// Airy
namespace Log_Item_SPACE {
enum LogItemDataRole {
    levelRole = Qt::UserRole + 6
};
}
namespace JOURNAL_SPACE {
enum JOURNAL_DISPLAY_COLUMN {
    journalLevelColumn = 0,
    journalDaemonNameColumn,
    journalDateTimeColumn,
    journalMsgColumn,
    journalHostNameColumn,
    journalDaemonIdColumn
};
}
namespace KERN_SPACE {
enum KERN_DISPLAY_COLUMN {
    kernDateTimeColumn = 0,
    kernHostNameColumn,
    kernDaemonNameColumn,
    kernMsgColumn
};
}
namespace DKPG_SPACE {
enum DKPG_DISPLAY_COLUMN {
    dkpgDateTimeColumn = 0,
    dkpgMsgColumn
};
}
namespace XORG_SPACE {
enum XORG_DISPLAY_COLUMN {
    xorgDateTimeColumn = 0,
    xorgMsgColumn
};
}
namespace APP_SPACE {
enum APP_DISPLAY_COLUMN {
    appLevelColumn = 0,
    appDateTimeColumn,
    appSrcColumn,
    appMsgColumn
};
}
namespace NORMAL_SPACE {
enum NORMAL_DISPLAY_COLUMN {
    normalEventTypeColumn = 0,
    normalUserNameColumn,
    normalDateTimeColumn,
    normalMsgColumn
};
}

namespace DNF_SPACE {
enum DNF_DISPLAY_COLUMN {
    dnfLvlColumn = 0,
    dnfDateTimeColumn,
    dnfMsgColumn
};
}
namespace DMESG_SPACE {
enum DMESG_DISPLAY_COLUMN {
    dmesgLevelColumn = 0,
    dmesgDateTimeColumn,
    dmesgMsgColumn
};
}

#endif  // STRUCTDEF_H
