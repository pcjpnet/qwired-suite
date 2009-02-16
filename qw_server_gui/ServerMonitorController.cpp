#include "ServerMonitorController.h"

ServerMonitorController::ServerMonitorController(QObject *parent) : QObject(parent)
{
}


void ServerMonitorController::startMonitor()
{
    monitorWindow = new ServerMonitorWindow(0);
    monitorWindow->show();
}
