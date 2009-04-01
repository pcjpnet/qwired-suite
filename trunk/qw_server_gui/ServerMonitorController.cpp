#include "ServerMonitorController.h"


ServerMonitorController::ServerMonitorController(QObject *parent) : QObject(parent)
{
    socket = new QwmConsoleSocket(this);
    connect(socket, SIGNAL(receivedResponseSTAT(QHash<QString,QString>)),
            this, SLOT(handleCommandSTAT(QHash<QString,QString>)));
    connect(socket, SIGNAL(receivedLogMessage(const QString)),
            this, SLOT(handleLogMessage(const QString)));

}


void ServerMonitorController::startMonitor()
{
    monitorWindow = new ServerMonitorWindow;
    monitorWindow->show();

    socket->connectToConsole("127.0.0.1", 2010);
}


void ServerMonitorController::handleCommandSTAT(QHash<QString,QString> parameters)
{
    monitorWindow->fStatsUsers->setText(parameters["USERS"]);
    monitorWindow->fStatsTransfers->setText(parameters["TRANSFERS"]);
    monitorWindow->fStatsDownloadTotal->setText(humanReadableSize(parameters["TOTAL_BYTES_SENT"].toLongLong()));
    monitorWindow->fStatsUploadTotal->setText(humanReadableSize(parameters["TOTAL_BYTES_RECEIVED"].toLongLong()));
}


void ServerMonitorController::handleLogMessage(const QString logMessage)
{
    qDebug() << "Got log Message:" << logMessage;
    monitorWindow->fStatsLog->append(logMessage);
}


QString ServerMonitorController::humanReadableSize(qlonglong theBytes) {
        qlonglong a=1024;
        float b=1024;

        if(theBytes<0) {
            return QString("-");
        } else if(theBytes < a) {
            return QString("%1").arg(theBytes);
        } else if(theBytes < a*a) {
            return QString("%1 KB").arg(float(theBytes/b), 0, 'f', 2);
        } else if(theBytes < a*a*a) {
            return QString("%1 MB").arg(float(theBytes/b/b), 0, 'f', 2);
        } else if(theBytes < a*a*a*a) {
            return QString("%1 GB").arg(float(theBytes/b/b/b), 0, 'f', 2);
        } else if(theBytes < a*a*a*a*a) {
            return QString("%1 TB").arg(float(theBytes/b/b/b/b), 0, 'f', 2);
        }
        return "?";
    }
