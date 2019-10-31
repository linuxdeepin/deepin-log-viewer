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
    arg.append(fileList[0]);
    //    arg << "/var/log/boot.log";
    QProcess proc;

    proc.start("cat", arg);
    proc.waitForFinished(-1);
    std::cout << proc.readAllStandardOutput().data();

    proc.close();

    return 0;
}
