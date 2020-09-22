#ifndef DBUSMANAGER_H
#define DBUSMANAGER_H

#include <QObject>
#include <QVariantMap>
class DBusManager : public QObject
{
    Q_OBJECT
public:
    explicit DBusManager(QObject *parent = nullptr);
    static QString getSystemInfo();
    static bool isGetedKlu ;
    static QString isklusystemName ;

signals:

public slots:
};

#endif // DBUSMANAGER_H
