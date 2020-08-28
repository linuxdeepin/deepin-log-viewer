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
namespace {
std::function<void(int)> shutdown_handler;
void signal_handler(int signal) { shutdown_handler(signal); }
} // namespace
ViewApplication::ViewApplication(int &argc, char **argv): QCoreApplication(argc, argv)
{
    QCommandLineParser parser;
    parser.process(*this);
    const QStringList fileList = parser.positionalArguments();
    //    qDebug() << fileList << "***" << fileList.count();
    if (fileList.count() < 1) {
        qDebug() << "less than 1";
        return ;
    }
    QStringList arg;
    //    "/var/log/boot.log";//开机的时候系统核心去侦测与启动，接下来开始各种核心支援的功能启动等；
    arg << "-c" << QString("cat %1").arg(fileList[0]);
    m_proc = new QProcess(this);
    shutdown_handler = [&](int signal) {
        if (m_proc) {
            m_proc->kill();
        }
        exit(0);
    };

    m_commondM = new QSharedMemory();
    m_commondM->setKey("AAAA");
    if (m_commondM->isAttached())      //检测程序当前是否关联共享内存
        m_commondM->detach();          //解除关联
    m_commondM->attach(QSharedMemory::ReadOnly);





    connect(m_proc, &QProcess::readyReadStandardOutput, this, [ = ] {
//        signal(SIGKILL, [](int x)
//        {
//            exit(0);
//        });
        QBuffer buffer;                //构建缓冲区
        QDataStream out(&buffer);      //建立数据流对象，并和缓冲区关联
        m_commondM->lock();           //锁定共享内存
        //初始化缓冲区中的数据，setData函数用来初始化缓冲区。
        //该函数如果在open()函数之后被调用，则不起任何作用。
        //buffer.open(QBuffer::ReadOnly);  //解除注释，则setData函数不起作用，无法加载内存中数据
        QString test;
        buffer.setData((char *)m_commondM->constData(), m_commondM->size());
        buffer.open(QBuffer::ReadOnly);    //只读方式打开缓冲区
        out >> test; //将缓冲区的数据写入QImage对象
        m_commondM->unlock();         //释放共享内存
        if (test == "KILL") {}
        m_proc->start();
        QByteArray byte =   m_proc->readAll();
        std::cout << byte.replace('\u0000', "").data();
    });
    m_proc->start("/bin/bash", arg);

    m_proc->waitForFinished(-1);
    //直接

//    QTextStream stream(&byte);
//    QByteArray encode;
//    stream.setCodec(encode);
//    QString str = stream.readAll();
    //必须要replace \u0000,不然QByteArray会忽略这以后的内容
    m_commondM->detach();
    m_commondM->deleteLater();
    m_proc->close();
}

void ViewApplication::dataRecived()
{

}


