#include "QwsTrackerController.h"

QwsTrackerController::QwsTrackerController(QObject *parent) : QObject(parent)
{
    startTimer(10000);
}


/*! Set a pointer to a \a QwServerInfo object containing the current information about the server.
*/
void QwsTrackerController::setServerInformation(QwServerInfo *newInfo)
{
    Q_ASSERT(newInfo != NULL);
    serverInfo = newInfo;
}


/*! Delete all registered tracker servers and free the memory.
*/
void QwsTrackerController::resetController()
{
    foreach (QwTrackerClientSocket *socket, trackerSockets) {
        socket->deleteLater();
    }
    trackerSockets.clear();
}


/*! Add a new tracker server to the list of updated/registered trackers.
*/
void QwsTrackerController::addTrackerServer(QString address, int port)
{
    qDebug() << this << "Added tracker" << address;
    QwTrackerClientSocket *newSocket = new QwTrackerClientSocket(this);
    newSocket->autoCommand = Qw::TrackerClientAutoCommandREGISTER;
    newSocket->localServerInfo = serverInfo;
    newSocket->trackerHost = address,
    newSocket->trackerPort = port;
    trackerSockets.append(newSocket);
}


void QwsTrackerController::timerEvent(QTimerEvent *event)
{
    qDebug() << "Bing!";
    foreach (QwTrackerClientSocket *socket, trackerSockets) {
        qDebug() << this << "Registering tracker" << socket;
        socket->connectToTracker();
    }

}
