#include <QCommandLineParser>
#include <QCoreApplication>
#include <QDebug>
#include <QFile>
#include <QProcess>
#include <QString>
#include <QStringList>
#include <QThread>
#include <iostream>

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    QCommandLineParser parser;
    parser.process(app);
    const QStringList fileList = parser.positionalArguments();
    //    qDebug() << fileList << "***" << fileList.count();
    if (fileList.count() < 1) {
        qDebug() << "less than 1";
        return 0;
    }
    QStringList arg;
    //    "/var/log/boot.log";//开机的时候系统核心去侦测与启动，接下来开始各种核心支援的功能启动等；
    if (fileList[0] == "dmesg") {
        arg << "-c" << "dmesg -r";
    } else {
        arg << "-c" << QString("cat %1").arg(fileList[0]);
    }


    QProcess proc;
    proc.start("/bin/bash", arg);
    proc.waitForFinished(-1);
    //直接
    QByteArray byte =   proc.readAllStandardOutput();
//    QTextStream stream(&byte);
//    QByteArray encode;
//    stream.setCodec(encode);
//    QString str = stream.readAll();
    //必须要replace \u0000,不然QByteArray会忽略这以后的内容
    std::cout << byte.replace('\u0000', "").data();
    proc.close();
    return 0;
}
