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
#include "logsettings.h"
#include <QStandardPaths>
#include <QApplication>
#include <QDir>
#include <QDebug>
#define MAINWINDOW_HEIGHT_NAME "logMainWindowHeightName"
#define MAINWINDOW_WIDTH_NAME "logMainWindowWidthName"
std::atomic<LogSettings *> LogSettings::m_instance;
std::mutex LogSettings::m_mutex;
LogSettings::LogSettings(QObject *parent) : QObject(parent)
{
    QDir winInfoPath(QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation));
    if (!winInfoPath.exists()) {
        winInfoPath.mkpath(QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation));
    }

    m_configPath = winInfoPath.filePath("wininfo-config.conf");
    m_winInfoConfig = new QSettings(m_configPath, QSettings::IniFormat, this);


}

QSize LogSettings::getConfigWinSize()
{
    QVariant tempHeight = m_winInfoConfig->value(MAINWINDOW_HEIGHT_NAME);
    QVariant tempWidth = m_winInfoConfig->value(MAINWINDOW_WIDTH_NAME);
    int winHeight = MAINWINDOW_HEIGHT;
    int winWidth = MAINWINDOW_WIDTH;
    if (tempHeight.isValid()) {

        winHeight = tempHeight.toInt();
        winHeight = winHeight > MAINWINDOW_HEIGHT ? winHeight : MAINWINDOW_HEIGHT;
    }
    if (tempHeight.isValid()) {

        winWidth = tempWidth.toInt();
        winWidth = winWidth > MAINWINDOW_WIDTH ? winWidth : MAINWINDOW_WIDTH;
    }

    return  QSize(winWidth, winHeight);
}

void LogSettings::saveConfigWinSize(int w, int h)
{
    int winWidth = w > MAINWINDOW_WIDTH ? w : MAINWINDOW_WIDTH;
    int winHeight = h > MAINWINDOW_HEIGHT ? h : MAINWINDOW_HEIGHT;
    m_winInfoConfig->setValue(MAINWINDOW_HEIGHT_NAME, winHeight);
    m_winInfoConfig->setValue(MAINWINDOW_WIDTH_NAME, winWidth);
    m_winInfoConfig->sync();
}


