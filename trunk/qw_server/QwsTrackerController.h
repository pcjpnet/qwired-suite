#ifndef QWSTRACKERCONTROLLER_H
#define QWSTRACKERCONTROLLER_H

#include "QwTrackerClientSocket.h"
#include "QwServerInfo.h"

#include <QObject>
#include <QList>

class QwsTrackerController : public QObject
{
public:
    QwsTrackerController(QObject *parent = 0);
    void setServerInformation(QwServerInfo *newInfo);
    void resetController();
    void addTrackerServer(QString address, int port = 2002);

private:
    QwServerInfo *serverInfo;
    QList<QwTrackerClientSocket*> trackerSockets;

protected:
    void timerEvent(QTimerEvent *event);
};

#endif // QWSTRACKERCONTROLLER_H
