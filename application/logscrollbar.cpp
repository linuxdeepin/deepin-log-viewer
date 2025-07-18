// SPDX-FileCopyrightText: 2019 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "logscrollbar.h"
#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(logApp)

LogScrollBar::LogScrollBar(QWidget *parent)
    : QScrollBar(parent)
{
    qCDebug(logApp) << "LogScrollBar constructor called with parent";
}

LogScrollBar::LogScrollBar(Qt::Orientation o, QWidget *parent)
    : QScrollBar(o, parent)
{
    qCDebug(logApp) << "LogScrollBar constructor called with orientation:" << o;
}

void LogScrollBar::mousePressEvent(QMouseEvent *event)
{
    qCDebug(logApp) << "LogScrollBar mouse press event";
    m_isOnPress = true;
    QScrollBar::mousePressEvent(event);
}
