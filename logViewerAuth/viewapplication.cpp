// SPDX-FileCopyrightText: 2019 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "viewapplication.h"

#include <QDebug>
#include <QFile>
#include <QProcess>
#include <QString>
#include <QStringList>
#include <QThread>
#include <QCommandLineParser>
#include <QSharedMemory>
#include <QBuffer>
#include <QDataStream>

#include <iostream>
#include<signal.h>

ViewApplication::ViewApplication(int &argc, char **argv): QCoreApplication(argc, argv),m_commondM(new QSharedMemory())
{
    QCommandLineParser parser;
    parser.process(*this);
    const QStringList fileList = parser.positionalArguments();
    if (fileList.count() < 2) {
        qDebug() << "less than 2";
        return ;
    }
    QStringList arg;
    if (fileList[0] == "dmesg") {
        arg << "-c"
            << "dmesg -r";
    } else if(fileList[0] == "coredumpctl-list"){
        arg << "-c"
            << "coredumpctl list";
    } else if(fileList[0] == "coredumpctl-info"){
        arg << "-c"
            << QString("coredumpctl info %1").arg(fileList[1]);
    } else {
        arg << "-c" << QString("cat %1").arg(fileList[0]);
    }
    m_proc = new QProcess(this);

    m_commondM->setKey(fileList[1]);
    m_commondM->create(sizeof(ShareMemoryInfo));
    if (m_commondM->isAttached())      //检测程序当前是否关联共享内存
        m_commondM->detach();          //解除关联
    m_commondM->attach(QSharedMemory::ReadOnly);

    connect(m_proc, &QProcess::readyReadStandardOutput, this, [ = ] {
        if (!getControlInfo().isStart)
        {
            m_proc->kill();
            releaseMemery();
            exit(0);
        }
        QByteArray byte =   m_proc->readAll();
        std::cout << byte.replace('\u0000', "").data();
    });
    m_proc->start("/bin/bash", arg);

    m_proc->waitForFinished(-1);
    m_proc->close();
}

ViewApplication::~ViewApplication()
{
    releaseMemery();
}

void ViewApplication::releaseMemery()
{
    if (m_commondM) {
        //  m_commondM->unlock();
        if (m_commondM->isAttached())      //检测程序当前是否关联共享内存
            m_commondM->detach();          //解除关联
        m_commondM->deleteLater();
    }
}

ViewApplication::ShareMemoryInfo ViewApplication::getControlInfo()
{
    ShareMemoryInfo defaultInfo;
    defaultInfo.isStart = false;
    if (m_commondM && m_commondM->isAttached()) {
        m_commondM->lock();
        ShareMemoryInfo   *m_pShareMemoryInfo = static_cast<ShareMemoryInfo *>(m_commondM->data());
        defaultInfo = m_pShareMemoryInfo ? *m_pShareMemoryInfo : defaultInfo;
        m_commondM->unlock();
        return defaultInfo;

    }
    return defaultInfo;
}
