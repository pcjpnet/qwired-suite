#include "QwSocket.h"


/*! \fn void QwSocket::messageReceived(const CPMessage &message)
    Dieses Signal wird emittiert sobald eine neue Nachricht vom Verbindungspartner empfangen wurde.
    \sa socketDataReceived()
*/

QwSocket::QwSocket(QObject *parent) :
        QObject(parent)
{
    m_socket = NULL;
}

QwSocket::~QwSocket()
{ }

/*! Sende eine Nachricht zum Verbindungspartner. Der Inhalt von message wird serialisiert und über
    das Central Park Protokoll an den Verbindungspartner gesendet.
*/
void QwSocket::sendMessage(const QwMessage &message)
{
    if (!m_socket) { return; }
    if (!m_socket->isOpen()) { return; }
    QByteArray buffer;
    QDataStream stream(&buffer, QIODevice::WriteOnly);
    buffer += message.generateFrame();
    buffer.append('\x04');
    m_socket->write(buffer);
    m_socket->flush();
}


/*! Return a const pointer to the underlying SSL-socket. */
const QSslSocket* QwSocket::sslSocket() const
{ return m_socket; }


/*! Attach the provided QSslSocket and connect required signals to make use of the socket. This
    object automatically becomes the parent for the socket object.
*/
void QwSocket::setSslSocket(QSslSocket *socket)
{
    if (!socket) return;
    this->m_socket = socket;
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
    tcpBuffer += m_socket->readAll();

    if ( tcpBuffer.size() > (1024 * 1024) )  {
        // Limit frame size to 1 MB
        m_socket->close();
    }

    int eotIndex = tcpBuffer.indexOf('\x04');

    QByteArray frameData;
    while (eotIndex > -1) {
        QByteArray frameData(tcpBuffer.left(eotIndex));
        tcpBuffer = tcpBuffer.mid(eotIndex+1);
        handleMessageReceived(QwMessage(frameData));
        eotIndex = tcpBuffer.indexOf('\x04');
    }
}

