// SPDX-FileCopyrightText: 2021 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "logviewerplugin.h"

#include <qdatetime.h>

LogViewerPlugin::LogViewerPlugin()
{
    initConnections();
}

LogViewerPlugin::~LogViewerPlugin()
{

}

/**
 * @brief DisplayContent::generateAppFile 触发获取应用日志数据线程
 * @param path 要获取的某一个应用的日志的日志文件路径
 * @param id 时间筛选id 对应BUTTONID枚举,0表示全部,1是今天,2是3天内,3是筛选1周内数据,4是筛选一个月内的,5是三个月
 * @param lId 等级筛选id,对应PRIORITY枚举,直接传入获取系统接口,-1表示全部等级不筛选,
 * @param iSearchStr 搜索关键字,现阶段不用,保留参数
 */
void LogViewerPlugin::generateAppFile(const QString &path, int id, int lId, const QString &iSearchStr)
{
    Q_UNUSED(iSearchStr)
    appList.clear();
    appListOrigin.clear();
    clearAllFilter();
    clearAllDatalist();
    //setLoadState(DATA_LOADING);
    m_firstLoadPageData = true;
    m_isDataLoadComplete = false;
    QDateTime dt = QDateTime::currentDateTime();
    dt.setTime(QTime()); // get zero time
    //createAppTableForm();
    APP_FILTERS appFilter;
    appFilter.path = path;
    appFilter.lvlFilter = lId;
    switch (id) {
    case ALL:
        m_appCurrentIndex = m_logFileParse.parseByApp(appFilter);
        break;
    case ONE_DAY: {
        QDateTime dtStart = dt;
        QDateTime dtEnd = dt;
        dtEnd.setTime(QTime(23, 59, 59, 999));
        appFilter.timeFilterBegin = dtStart.toMSecsSinceEpoch();
        appFilter.timeFilterEnd = dtEnd.toMSecsSinceEpoch();
        m_appCurrentIndex = m_logFileParse.parseByApp(appFilter);
    } break;
    case THREE_DAYS: {
        QDateTime dtStart = dt;
        QDateTime dtEnd = dt;
        dtEnd.setTime(QTime(23, 59, 59, 999));
        appFilter.timeFilterBegin = dtStart.addDays(-2).toMSecsSinceEpoch();
        appFilter.timeFilterEnd = dtEnd.toMSecsSinceEpoch();
        m_appCurrentIndex = m_logFileParse.parseByApp(appFilter);

    } break;
    case ONE_WEEK: {
        QDateTime dtStart = dt;
        QDateTime dtEnd = dt;
        dtEnd.setTime(QTime(23, 59, 59, 999));
        appFilter.timeFilterBegin = dtStart.addDays(-6).toMSecsSinceEpoch();
        appFilter.timeFilterEnd = dtEnd.toMSecsSinceEpoch();
        m_appCurrentIndex = m_logFileParse.parseByApp(appFilter);
    } break;
    case ONE_MONTH: {
        QDateTime dtStart = dt;
        QDateTime dtEnd = dt;
        dtEnd.setTime(QTime(23, 59, 59, 999));
        appFilter.timeFilterBegin = dtStart.addMonths(-1).toMSecsSinceEpoch();
        appFilter.timeFilterEnd = dtEnd.toMSecsSinceEpoch();
        m_appCurrentIndex = m_logFileParse.parseByApp(appFilter);
    } break;
    case THREE_MONTHS: {
        QDateTime dtStart = dt;
        QDateTime dtEnd = dt;
        dtEnd.setTime(QTime(23, 59, 59, 999));
        appFilter.timeFilterBegin = dtStart.addMonths(-3).toMSecsSinceEpoch();
        appFilter.timeFilterEnd = dtEnd.toMSecsSinceEpoch();
        m_appCurrentIndex = m_logFileParse.parseByApp(appFilter);
    } break;
    default:
        break;
    }
}

void LogViewerPlugin::clearAllFilter()
{
    m_bootFilter = {"", ""};
    m_currentSearchStr.clear();
    m_currentKwinFilter = {""};
    m_normalFilter.searchstr = "";
}

/**
 * @brief LogViewerPlugin::clearAllDatalist 清空所有获取的数据list
 */
void LogViewerPlugin::clearAllDatalist()
{
//    m_detailWgt->cleanText();
//    m_pModel->clear();
    jList.clear();
    jListOrigin.clear();
    dList.clear();
    dListOrigin.clear();
    xList.clear();
    xListOrigin.clear();
    bList.clear();
    currentBootList.clear();
    kList.clear();
    kListOrigin.clear();
    appList.clear();
    appListOrigin.clear();
    norList.clear();
    nortempList.clear();
    m_currentKwinList.clear();
    m_kwinList.clear();
    jBootList.clear();
    jBootListOrigin.clear();
    dnfList.clear();
    dnfListOrigin.clear();
//    malloc_trim(0);
}

/**
 * @brief DisplayContent::initConnections 初始化槽函数信号连接
 */
void LogViewerPlugin::initConnections()
{
    connect(&m_logFileParse, &LogFileParser::appFinished, this,
            &LogViewerPlugin::sigAppFinished);
    connect(&m_logFileParse, &LogFileParser::appData, this,
            &LogViewerPlugin::sigAppData);
}
