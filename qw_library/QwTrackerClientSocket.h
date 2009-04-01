#ifndef QWSTRACKERCLIENTSOCKET_H
#define QWSTRACKERCLIENTSOCKET_H

#include <QDateTime>
#include <QUdpSocket>
#include <QTimer>

#include "QwSocket.h"
#include "QwTrackerServerInfo.h"

namespace Qw {
    enum TrackerClientSocketMode { TrackerClientSocketModeManual,
                                   /*! Automatic mode enables the socket to automatically register
                                       the server on the tracker and keep sending UDP packages. */
                                   TrackerClientSocketModeAutomatic };
};

class QwTrackerClientSocket : QwSocket
{
    Q_OBJECT

public:
    QwTrackerClientSocket(QObject *parent = 0);
    Qw::TrackerClientSocketMode mode;

    // Information fields when registering a server.
    QwTrackerServerInfo localServerInfo;

    // Information about the tracker
    QString trackerVersion;
    QString trackerProtocolVersion;
    QString trackerName;
    QString trackerDescription;
    QDateTime trackerStartTime;

    // Connectivity
    QTimer trackerUpdateTimer;
    quint16 trackerPort;
    QString trackerHost;
    /*! This is for sending UDP datagrams, as QUdpSocket does not provide name lookups. */
    QHostAddress trackerIp;


private:
    /*! The UDP socket for sending update datagrams to the tracker. */
    QUdpSocket *udpSocket;

public slots:
    void connectToTracker(const QString host, const int port=2002);
    void sendCommandCLIENT();
    void sendCommandREGISTER();
    void sendCommandUPDATE();


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
