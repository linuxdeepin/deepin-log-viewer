/*
* Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
*
* Author:     zyc <zyc@uniontech.com>
*
* Maintainer:  zyc <zyc@uniontech.com>
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

#include "logsettings.h"
#include <QStandardPaths>
#include <QApplication>
#include <QDir>
#include <QDebug>
#define MAINWINDOW_HEIGHT_NAME "logMainWindowHeightName"
#define MAINWINDOW_WIDTH_NAME "logMainWindowWidthName"
std::atomic<LogSettings *> LogSettings::m_instance;
std::mutex LogSettings::m_mutex;
/**
 * @brief LogSettings::LogSettings 构造函数从配置文件初始化配置
 * @param parent　父对象
 */
LogSettings::LogSettings(QObject *parent)
    : QObject(parent),
      m_winInfoConfig(nullptr),
      m_logDirConfig(nullptr),
      m_configPath(""),
      m_logDirPath("")
{
    QDir infoPath(QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation));
    if (!infoPath.exists()) {
        infoPath.mkpath(QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation));
    }

    m_configPath = infoPath.filePath("wininfo-config.conf");
    m_winInfoConfig = new QSettings(m_configPath, QSettings::IniFormat, this);

    m_logDirPath = infoPath.filePath("logdir-config.conf");
    m_logDirConfig = new QSettings(m_logDirPath, QSettings::IniFormat, this);


}

/**
 * @brief LogSettings::getConfigWinSize　通过配置文件获取之前保存的窗口大小
 * @return　之前保存的窗口大小
 */
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

/**
 * @brief LogSettings::saveConfigWinSize 保存当前窗口尺寸到配置文件
 * @param w　当前窗口宽度
 * @param h　当前窗口高度
 */
void LogSettings::saveConfigWinSize(int w, int h)
{
    int winWidth = w > MAINWINDOW_WIDTH ? w : MAINWINDOW_WIDTH;
    int winHeight = h > MAINWINDOW_HEIGHT ? h : MAINWINDOW_HEIGHT;
    m_winInfoConfig->setValue(MAINWINDOW_HEIGHT_NAME, winHeight);
    m_winInfoConfig->setValue(MAINWINDOW_WIDTH_NAME, winWidth);
    m_winInfoConfig->sync();
}


