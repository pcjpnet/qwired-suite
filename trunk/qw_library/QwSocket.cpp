#include "QwSocket.h"


/*! \fn void QwSocket::messageReceived(const CPMessage &message)
    Dieses Signal wird emittiert sobald eine neue Nachricht vom Verbindungspartner empfangen wurde.
    \sa socketDataReceived()
*/

QwSocket::QwSocket(QObject *parent) :
        QObject(parent)
{
    socket = NULL;
}

QwSocket::~QwSocket()
{ }

/*! Sende eine Nachricht zum Verbindungspartner. Der Inhalt von message wird serialisiert und über
    das Central Park Protokoll an den Verbindungspartner gesendet.
*/
void QwSocket::sendMessage(const QwMessage &message)
{
    if (!socket) { return; }
    if (!socket->isOpen()) { return; }
    QByteArray buffer;
    QDataStream stream(&buffer, QIODevice::WriteOnly);
    buffer += message.generateFrame();
    buffer.append('\x04');
    socket->write(buffer);
    socket->flush();
}


/*! Return a const pointer to the underlying SSL-socket.
*/
const QSslSocket* QwSocket::sslSocket() const
{
    return socket;
}


/*! Attach the provided QSslSocket and connect required signals to make use of the socket. This
    object automatically becomes the parent for the socket object.
*/
void QwSocket::setSslSocket(QSslSocket *socket)
{
    if (!socket) return;
    this->socket = socket;
    socket->setParent(this);
    connect(socket, SIGNAL(readyRead()),
            this, SLOT(socketDataReceived()), Qt::QueuedConnection );
    connect(socket, SIGNAL(disconnected()),
            this, SIGNAL(connectionLost()), Qt::QueuedConnection );
}


/*! This method is called whenever data is received on the socket. First it is written to a buffer
    and then parsed to see if any complete frames are available.
*/
void QwSocket::socketDataReceived()
{
    tcpBuffer += socket->readAll();

    if ( tcpBuffer.size() > (1024 * 1024) )  {
        // Limit frame size to 1 MB
        socket->close();
    }

    int eotIndex = tcpBuffer.indexOf('\x04');

    QByteArray frameData;
    while (eotIndex > -1) {
        QByteArray frameData(tcpBuffer.left(eotIndex));
        tcpBuffer = tcpBuffer.mid(eotIndex+1);
        emit messageReceived(QwMessage(frameData));
        eotIndex = tcpBuffer.indexOf('\x04');
    }
}

