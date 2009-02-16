#ifndef SERVERMONITORCONTROLLER_H
#define SERVERMONITORCONTROLLER_H

#include <QObject>
#include <QPointer>
#include "ServerMonitorWindow.h"

class ServerMonitorController : public QObject
{

    Q_OBJECT

private:
    QPointer<ServerMonitorWindow> monitorWindow;

public:
    ServerMonitorController(QObject *parent=0);

public slots:
    void startMonitor();

};

#endif // SERVERMONITORCONTROLLER_H
