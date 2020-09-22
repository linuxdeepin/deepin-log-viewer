#ifndef LOGAPPLICATION_H
#define LOGAPPLICATION_H
#include <DApplication>

DWIDGET_USE_NAMESPACE
class QKeyEvent;
class LogCollectorMain;
class LogApplication: public DApplication
{
public:
    LogApplication(int &argc, char **argv);
    void setMainWindow(LogCollectorMain *iMainWindow);
protected:
    bool notify(QObject *obj, QEvent *event) Q_DECL_OVERRIDE;
private:
    LogCollectorMain *m_mainWindow{nullptr};
};

#endif // LOGAPPLICATION_H
