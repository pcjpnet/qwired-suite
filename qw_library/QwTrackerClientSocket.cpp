#include "QwTrackerClientSocket.h"

QwTrackerClientSocket::QwTrackerClientSocket(QObject *parent) : QwSocket(parent)
{
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


void QwTrackerClientSocket::handleSocketConnected()
{
    qDebug() << this << "Connection established - sending handshake";
    sendMessage(QwMessage("HELLO"));
}


void QwTrackerClientSocket::handleSocketError(QAbstractSocket::SocketError error)
{
    qDebug() << this << "Socket error:" << socket->errorString();
}


void QwTrackerClientSocket::connectToTracker(const QString host, const int port)
{
    qDebug() << this << "Connecting to remote tracker at:" << host << port;
    socket->connectToHostEncrypted(host, port);
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
