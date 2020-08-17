#include <QCommandLineParser>
#include <QCoreApplication>
#include <QDebug>
#include <QFile>
#include <QProcess>
#include <QString>
#include <QStringList>
#include <QThread>
#include <QDir>
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
    QString fileDirStr = fileList[0];
    qDebug() << fileDirStr;
    if (fileDirStr != "/var/log/kern.log" && fileDirStr != "/var/log/boot.log" && fileDirStr != "/var/log/dnf.log" && fileDirStr != "/var/log/Xorg.0.log" && fileDirStr != (QDir::homePath() + "/.kwin.log")) {
        qDebug() << "log file is not illegal";
        return 0;
    }
    QStringList arg;
    arg << "-c" << QString("truncate -s 0 %1").arg(fileList[0]);
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
