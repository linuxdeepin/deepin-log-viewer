#include "logapplication.h"
#include "logcollectormain.h"

#include <DWidgetUtil>

#include <QKeyEvent>
#include <QDebug>
#include <QEvent>

LogApplication::LogApplication(int &argc, char **argv): DApplication(argc, argv)
{

}

void LogApplication::setMainWindow(LogCollectorMain *iMainWindow)
{
    m_mainWindow = iMainWindow;
}

bool LogApplication::notify(QObject *obj, QEvent *evt)
{
    switch (evt->type()) {
    case QEvent::KeyPress: {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(evt);
        if (keyEvent->key() == Qt::Key_Tab || keyEvent->key() == Qt::Key_Backtab) {
            if (m_mainWindow) {
                bool rs =  m_mainWindow->handleApplicationTabEventNotify(obj, keyEvent);
                if (rs) {
                    return rs;
                } else {
                    return  DApplication::notify(obj, evt) ;
                }
            }
        }
        break;
    }
    }
    return  DApplication::notify(obj, evt) ;
}
