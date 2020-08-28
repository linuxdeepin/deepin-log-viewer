#include <QCommandLineParser>
#include <QCoreApplication>
#include <QDebug>
#include <QFile>
#include <QProcess>
#include <QString>
#include <QStringList>
#include <QThread>
#include <iostream>
#include "viewapplication.h"
int main(int argc, char *argv[])
{
    ViewApplication app(argc, argv);

    return 0;
}
