#include "viewapplication.h"
#include "authsharedmemorymanager.h"

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
#include <QFile>
#include <QTimer>

#include <iostream>
#include<signal.h>


namespace {
std::function<void(int)> shutdown_handler;
void signal_handler(int signal) { shutdown_handler(signal); }
} // namespace
ViewApplication::ViewApplication(int &argc, char **argv)
    : QCoreApplication(argc, argv)
    , mFile(nullptr)
    , mMem(nullptr)
    , mSize(0)
{
    QCommandLineParser parser;
    parser.process(*this);
    const QStringList fileList = parser.positionalArguments();
    //    qDebug() << fileList << "***" << fileList.count();
    if (fileList.count() < 2) {
        qDebug() << "less than 2";
        return ;
    }


    AuthSharedMemoryManager::instance()->initRunnableTagMem(fileList[1]);
    QString tag;
//   QString path = "/home/zyc/Documents/tech/同方内核日志没有/kern.log";
    QString path = "/var/log/kern.log";
    if (doReadFileWork(path)) {
        if (AuthSharedMemoryManager::instance()->addDataInfo(mSize, mMem, tag)) {
            qInfo() << QString("load file complete:%1").arg(1);
        }
    }

    m_timer = new QTimer(this);
    m_timer->setInterval(1000);
    m_timer->setSingleShot(false);
    connect(m_timer, &QTimer::timeout, this, &ViewApplication::checkClose);
    m_timer->start();



}

ViewApplication::~ViewApplication()
{
    close();
    AuthSharedMemoryManager::instance()->releaseAllMem();
    qDebug() << "~ViewApplication";
}



bool ViewApplication::doReadFileWork(const QString &iFilePath)
{
    if (mFile) {
        close();
    }
    QString path = iFilePath;
    path = "/home/zyc/Documents/tech/同方内核日志没有/kern.log";
    mFile = new QFile(path);
    if (!mFile->open(QIODevice::ReadOnly)) {
        qDebug() << "failed to open file as read only";
        close();
        return false;
    }

    mSize = mFile->size();
    if (mSize <= 0) {
        qDebug() << "file is empty";
        close();
        return false;
    }

    auto mem = mFile->map(0, mSize, QFileDevice::MapPrivateOption);
    //由于map出的内存是不带\0的，所以可能会导致越界访问
    //一个临时的修复方法是把最后一个字节改为\0
    mem[mSize - 1] = '\0';
    mMem = (char *)(mem);
    return  true;
}

void ViewApplication::close()
{
    if (mFile) {
        mFile->unmap((uchar *)mMem);
        mFile->close();
        delete mFile;
        mFile = nullptr;
    }
}

void ViewApplication::checkClose()
{
    ShareMemoryInfo info =  AuthSharedMemoryManager::instance()->getRunnableTag();

    if (!info.isStart) {
        AuthSharedMemoryManager::instance()->releaseAllMem();
        close();
        quit();
    }
}







