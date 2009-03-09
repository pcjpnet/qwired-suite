#include "QwsClientTransferSocket.h"

/*! \class QwsClientTransferSocket
    \author Bastian Bense <bastibense@gmail.com>
    \date 2009-02-23

    This class implements the simple TRANSFER-protocol for file transfers to clients.
*/

void QwsClientTransferSocket::run()
{
    qDebug() << this << "Starting thread.";


    qDebug() << this << "Finished thread.";
}


/*! This function takes over the socket and moves it to this thread. Also it takes QObject
    ownership.
*/
void QwsClientTransferSocket::setSocket(QSslSocket *socket)
{
    if (!socket) { return; }
    this->socket = socket;
    this->socket->setParent(this);
}


QwsClientTransferSocket::QwsClientTransferSocket(QObject *parent=0) : QThread(parent)
{
    qDebug() << this << "Created new thread object.";
}
