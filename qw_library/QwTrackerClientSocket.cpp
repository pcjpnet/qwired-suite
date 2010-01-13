#include "QwTrackerClientSocket.h"

QwTrackerClientSocket::QwTrackerClientSocket(QObject *parent) : QwSocket(parent)
{
    autoCommand = Qw::TrackerClientAutoCommandNone;
    localServerInfo = NULL;

    trackerPort = 2002;


    // Create a new SslSocket for this tracker client.
    QSslSocket *newSocket = new QSslSocket(this);
    newSocket->setPeerVerifyMode(QSslSocket::QueryPeer);
    newSocket->setProtocol(QSsl::TlsV1);
    connect(newSocket, SIGNAL(encrypted()),
            this, SLOT(handleSocketConnected()));
    connect(newSocket, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(handleSocketError(QAbstractSocket::SocketError)));
    connect(newSocket, SIGNAL(disconnected()),
            this, SIGNAL(disconnectedFromTracker()));

    connect(this, SIGNAL(messageReceived(const QwMessage)),
            this, SLOT(handleMessageReceived(const QwMessage)));

    setSslSocket(newSocket);
}

/*! Send the UPDATE command to the tracker. This command does not require a SSL connection, since
    the UPDATE command is sent via UDP.
    \todo Implement this or work around using periodic re-registration.
*/
void QwTrackerClientSocket::sendCommandUPDATE()
{
//    qDebug() << this << "Sending UPDATE to" << trackerIp << "port" << trackerPort;
//    QwMessage reply("UPDATE");
//    reply.appendArg(localServerInfo.registrationHash);
//    reply.appendArg(QString::number(localServerInfo.userCount));
//    reply.appendArg(QString::number(localServerInfo.canGuests));
//    reply.appendArg(QString::number(localServerInfo.canDownload));
//    reply.appendArg(QString::number(localServerInfo.filesCount));
//    reply.appendArg(QString::number(localServerInfo.filesSize));
//    QByteArray datagram = reply.generateFrame().append('\x04');
//    qDebug() << datagram.toHex();
//    qDebug() << "Write Datagram:" << udpSocket->writeDatagram(datagram, trackerIp, trackerPort);
}


/*! Send the CATEGORIES command to the tracker server.
*/
void QwTrackerClientSocket::sendCommandCATEGORIES()
{
    qDebug() << this << "Requesting CATEGORIES";
    sendMessage(QwMessage("CATEGORIES"));
}


/*! Send the SERVERS command to the tracker server.
*/
void QwTrackerClientSocket::sendCommandSERVERS()
{
    qDebug() << this << "Requesting SERVERS";
    sendMessage(QwMessage("SERVERS"));
}


/*! Socket is connected, send the HELLO handshake.
*/
void QwTrackerClientSocket::handleSocketConnected()
{
    qDebug() << this << "Connection established - sending handshake";
    sendMessage(QwMessage("HELLO"));
}


void QwTrackerClientSocket::handleSocketError(QAbstractSocket::SocketError error)
{
    Q_UNUSED(error);
    qDebug() << this << "Socket error:" << socket->errorString();
    emit socketError(error);
}


void QwTrackerClientSocket::connectToTracker(const QString host, const int port)
{
    if (!host.isEmpty()) { trackerHost = host; }
    if (port != 2002) { trackerPort = port; }
    categoryListingBuffer.clear();
    serverListingBuffer.clear();

    qDebug() << this << "Connecting to remote tracker at:" << host << port;
    socket->connectToHostEncrypted(trackerHost, trackerPort);
}


void QwTrackerClientSocket::handleMessageReceived(const QwMessage &message)
{
    qDebug() << this << "Received message:" << message.commandName;
    if (message.commandName == "200") {         handleMessage200(message);
    } else if (message.commandName == "700") {  handleMessage700(message);
    } else if (message.commandName == "710") {  handleMessage710(message);
    } else if (message.commandName == "711") {  handleMessage711(message);
    } else if (message.commandName == "720") {  handleMessage720(message);
    } else if (message.commandName == "721") {  handleMessage721(message);
    }
}


/*! 200 Tracker Information
    Sent from the server if the HELLO-command was successful.
*/
void QwTrackerClientSocket::handleMessage200(const QwMessage &message)
{
    trackerVersion = message.stringArg(0);
    trackerProtocolVersion = message.stringArg(1);
    trackerName = message.stringArg(2);
    trackerDescription = message.stringArg(3);
    trackerStartTime = QDateTime::fromString(message.stringArg(4), Qt::ISODate);

    qDebug() << this << "Handshake complete - connected to" << trackerName;
    sendCommandCLIENT();

    switch (autoCommand) {
    case Qw::TrackerClientAutoCommandSERVERS:
        sendCommandSERVERS(); break;
    case Qw::TrackerClientAutoCommandREGISTER:
        sendCommandREGISTER(); break;
    default:
        break;
    }

}


/*! 700 Registered
    Sent from the server if the registration was successful. Set serverRegistrationHash to the new
    hash and emit a receivedRegistrationHash() signal.
*/
void QwTrackerClientSocket::handleMessage700(const QwMessage &message)
{ 
    localServerInfo->registrationHash = message.stringArg(0);
    qDebug() << this << "Registration OK:" << localServerInfo->registrationHash;
    emit receivedRegistrationHash(localServerInfo->registrationHash);

    socket->disconnectFromHost();
}

/*! 710 Category Listing
*/
void QwTrackerClientSocket::handleMessage710(const QwMessage &message)
{
    qDebug() << this << "Received category:" << message.stringArg(0);
    categoryListingBuffer.append(message.stringArg(0));
}


/*! 711 Category Listing Done
*/
void QwTrackerClientSocket::handleMessage711(const QwMessage &message)
{
    Q_UNUSED(message);
    qDebug() << this << "Received end of category name list";
    emit receivedCategories(categoryListingBuffer);
    categoryListingBuffer.clear();
}

/*! 720 Server Listing
*/
void QwTrackerClientSocket::handleMessage720(const QwMessage &message)
{
    qDebug() << this << "Received server listing item:" << message.stringArg(2);
    QwServerInfo newInfo;
    newInfo.category = message.stringArg(0);
    newInfo.url = message.stringArg(1);
    newInfo.name = message.stringArg(2);
    newInfo.userCount = message.stringArg(3).toInt();
    newInfo.bandwidth = message.stringArg(4).toLongLong();
    newInfo.canGuests = message.stringArg(5).toInt();
    newInfo.canDownload = message.stringArg(6).toInt();
    newInfo.filesCount = message.stringArg(7).toLongLong();
    newInfo.filesSize = message.stringArg(8).toLongLong();
    newInfo.description = message.stringArg(9);
    serverListingBuffer.append(newInfo);
}


/*! 721 Server Listing Done
*/
void QwTrackerClientSocket::handleMessage721(const QwMessage &message)
{
    Q_UNUSED(message);
    qDebug() << this << "Received end of server list";
    emit receivedServers(serverListingBuffer);
    serverListingBuffer.clear();

    // Automatically disconnect in auto-mode
    if (autoCommand == Qw::TrackerClientAutoCommandSERVERS) {
        socket->disconnectFromHost();
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
    Q_ASSERT(localServerInfo != NULL);
    QwMessage reply("REGISTER");
    reply.appendArg(localServerInfo->category);
    reply.appendArg(localServerInfo->url);
    reply.appendArg(localServerInfo->name);
    reply.appendArg(QString::number(localServerInfo->bandwidth));
    reply.appendArg(localServerInfo->description);
    sendMessage(reply);
}
