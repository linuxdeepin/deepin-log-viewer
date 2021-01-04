/*
* Copyright (C) 2019 ~ 2021 Uniontech Software Technology Co.,Ltd.
*
* Author:     linxun <linxun@uniontech.com>
*
* Maintainer: linxun <linxun@uniontech.com>
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
#include "dldbushandler.h"

DLDBusHandler *DLDBusHandler::m_statichandeler = nullptr;

DLDBusHandler *DLDBusHandler::instance(QObject *parent)
{
    if (parent != nullptr && m_statichandeler == nullptr) {
        m_statichandeler = new DLDBusHandler(parent);
    }
    return m_statichandeler;
}

DLDBusHandler::~DLDBusHandler()
{
    quit();
}

DLDBusHandler::DLDBusHandler(QObject *parent)
    : QObject(parent)
{
    m_dbus = new DeepinLogviewerInterface("com.deepin.logviewer",
                                          "/com/deepin/logviewer",
                                          QDBusConnection::systemBus(),
                                          this);
    //Note: when dealing with remote objects, it is not always possible to determine if it exists when creating a QDBusInterface.
    if (!m_dbus->isValid() && !m_dbus->lastError().message().isEmpty()) {
        qDebug() << "m_dbus isValid false error:" << m_dbus->lastError() << m_dbus->lastError().message();
    }
    qDebug() << "m_dbus isValid true";
}

/*!
 * \~chinese \brief DLDBusHandler::readLog 读取日志文件
 * \~chinese \param filePath 文件路径
 * \~chinese \return 读取的日志
 */
QString DLDBusHandler::readLog(const QString &filePath)
{
    return m_dbus->readLog(filePath);
}

/*!
 * \~chinese \brief DLDBusHandler::exitCode 返回进程状态
 * \~chinese \return 进程返回值
 */
int DLDBusHandler::exitCode()
{
    return m_dbus->exitCode();
}

/*!
 * \~chinese \brief DLDBusHandler::quit 退出服务端程序
 */
void DLDBusHandler::quit()
{
    m_dbus->quit();
}
