#ifndef QWSTRACKERCLIENTSOCKET_H
#define QWSTRACKERCLIENTSOCKET_H

#include <QDateTime>

#include "QwSocket.h"
#include "QwTrackerServerInfo.h"

class QwTrackerClientSocket : QwSocket
{
    Q_OBJECT

public:
    QwTrackerClientSocket(QObject *parent = 0);

    // Information fields when registering a server.
    QwTrackerServerInfo localServerInfo;

    // Information about the tracker
    QString trackerVersion;
    QString trackerProtocolVersion;
    QString trackerName;
    QString trackerDescription;
    QDateTime trackerStartTime;

public slots:
    void connectToTracker(const QString host, const int port=2002);
    void sendCommandCLIENT();
    void sendCommandREGISTER();

private slots:
    void handleSocketConnected();
    void handleSocketError(QAbstractSocket::SocketError error);
    void handleMessageReceived(const QwMessage &message);
    void handleMessage200(const QwMessage &message);
    void handleMessage700(const QwMessage &message);

signals:
    void receivedRegistrationHash(QString hash);

};

#endif // QWSTRACKERCLIENTSOCKET_H
