#ifndef VIEWAPPLICATION_H
#define VIEWAPPLICATION_H
#include <QCoreApplication>
class QProcess;
class QSharedMemory;
class ViewApplication : public QCoreApplication
{
    struct ShareMemoryInfo {
        bool isStart = true ;
    };
public:
    ViewApplication(int &argc, char **argv);
    ~ViewApplication();
    QProcess *m_proc;
    QSharedMemory *m_commondM;
public slots:
    //    void dataRecived();
    void releaseMemery();
    ShareMemoryInfo getControlInfo();

};

#endif // VIEWAPPLICATION_H
