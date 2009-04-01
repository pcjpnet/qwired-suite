#include "QwTrackerClientSocket.h"

QwTrackerClientSocket::QwTrackerClientSocket(QObject *parent) : QwSocket(parent)
{
    udpSocket = new QUdpSocket(this);
    mode = Qw::TrackerClientSocketModeManual;

    // Connect the tracker update timer
    trackerUpdateTimer.setInterval(10*1000); // 60 seconds
    connect(&trackerUpdateTimer, SIGNAL(timeout()),
            this, SLOT(sendCommandUPDATE()));

    // Create a new SslSocket for this tracker client.
    QSslSocket *newSocket = new QSslSocket(this);
    newSocket->setPeerVerifyMode(QSslSocket::QueryPeer);
    newSocket->setProtocol(QSsl::TlsV1);
    connect(newSocket, SIGNAL(encrypted()),
            this, SLOT(handleSocketConnected()));
    connect(newSocket, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(handleSocketError(QAbstractSocket::SocketError)));
    connect(this, SIGNAL(messageReceived(const QwMessage)),
            this, SLOT(handleMessageReceived(const QwMessage)));
    setSslSocket(newSocket);
}

/*! Send the UPDATE command to the tracker. This command does not require a SSL connection, since
    the UPDATE command is sent via UDP.
*/
void QwTrackerClientSocket::sendCommandUPDATE()
{
    qDebug() << this << "Sending UPDATE to" << trackerIp << "port" << trackerPort;
    QwMessage reply("UPDATE");
    reply.appendArg(localServerInfo.registrationHash);
    reply.appendArg(QString::number(localServerInfo.userCount));
    reply.appendArg(QString::number(localServerInfo.canGuests));
    reply.appendArg(QString::number(localServerInfo.canDownload));
    reply.appendArg(QString::number(localServerInfo.filesCount));
    reply.appendArg(QString::number(localServerInfo.filesSize));
    QByteArray datagram = reply.generateFrame().append('\x04');
    qDebug() << datagram.toHex();
    qDebug() << "Write Datagram:" << udpSocket->writeDatagram(datagram, trackerIp, trackerPort);
}


void QwTrackerClientSocket::handleSocketConnected()
{
    qDebug() << this << "Connection established - sending handshake";
    trackerIp = socket->peerAddress();
    sendMessage(QwMessage("HELLO"));
}


void QwTrackerClientSocket::handleSocketError(QAbstractSocket::SocketError error)
{
    qDebug() << this << "Socket error:" << socket->errorString();
}


void QwTrackerClientSocket::connectToTracker(const QString host, const int port)
{
    trackerHost = host;
    trackerPort = port;

    qDebug() << this << "Connecting to remote tracker at:" << host << port;
    socket->connectToHostEncrypted(trackerHost, trackerPort);
}


void QwTrackerClientSocket::handleMessageReceived(const QwMessage &message)
{
    qDebug() << this << "Received message:" << message.commandName;
    if (message.commandName == "200") {         handleMessage200(message);
    } else if (message.commandName == "700") {  handleMessage700(message);
    }
}


/*! 200 Tracker Information
    Sent from the server if the HELLO-command was successful.
*/
void QwTrackerClientSocket::handleMessage200(const QwMessage &message)
{
    trackerVersion = message.getStringArgument(0);
    trackerProtocolVersion = message.getStringArgument(1);
    trackerName = message.getStringArgument(2);
    trackerDescription = message.getStringArgument(3);
    trackerStartTime = QDateTime::fromString(message.getStringArgument(4), Qt::ISODate);

    qDebug() << this << "Handshake complete - connected to" << trackerName;
    sendCommandCLIENT();

    if (mode == Qw::TrackerClientSocketModeAutomatic) {
        sendCommandREGISTER();
    }

}


/*! 700 Registered
    Sent from the server if the registration was successful. Set serverRegistrationHash to the new
    hash and emit a receivedRegistrationHash() signal.
*/
void QwTrackerClientSocket::handleMessage700(const QwMessage &message)
{ 
    localServerInfo.registrationHash = message.getStringArgument(0);
    qDebug() << this << "Registration OK:" << localServerInfo.registrationHash;
    emit receivedRegistrationHash(localServerInfo.registrationHash);

    localServerInfo.userCount = 2581;

    if (mode == Qw::TrackerClientSocketModeAutomatic) {
        qDebug() << this << "Disconnecting from tracker server, beginning automatic update";
        socket->disconnectFromHost();
        trackerUpdateTimer.start();
    }
}


/*! Send the REGISTER command to a tracker server. The tracker server will respond with a registration
    hash which can be used in UDP-UPDATE messages later on.
    \todo Implement CLIENT message.
*/
void QwTrackerClientSocket::sendCommandCLIENT()
{

}


/*! Send the REGISTER command to a tracker server. The tracker server will respond with a registration
    hash which can be used in UDP-UPDATE messages later on.
*/
void QwTrackerClientSocket::sendCommandREGISTER()
{
    qDebug() << this << "Sending REGISTER command";
    QwMessage reply("REGISTER");
    reply.appendArg(localServerInfo.category);
    reply.appendArg(localServerInfo.url);
    reply.appendArg(localServerInfo.name);
    reply.appendArg(QString::number(localServerInfo.bandwidth));
    reply.appendArg(localServerInfo.description);
    sendMessage(reply);
}
