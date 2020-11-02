#ifndef VIEWAPPLICATION_H
#define VIEWAPPLICATION_H
#include <QCoreApplication>
class QProcess;
class QSharedMemory;
class ViewApplication : public QCoreApplication
{
public:
    ViewApplication(int &argc, char **argv);
    ~ViewApplication();
    QProcess *m_proc;
    QSharedMemory *m_commondM;
public slots:
    void dataRecived();
    void releaseMemery();
    bool doReadFileWork();
};

#endif // VIEWAPPLICATION_H
