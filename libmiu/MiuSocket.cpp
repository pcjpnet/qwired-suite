/***************************************************************************
 * QWired - a free Wired client.
 * Copyright (C) 2008 Bastian Bense
 ***************************************************************************
 * This program is free software; you can redistribute it and/or modify it *
 * under the terms of the GNU General Public License as published by the   *
 * Free Software Foundation; either version 2 of the License, or           *
 * (at your option) any later version.                                     *
 *                                                                         *
 * This program is distributed in the hope that it will be useful, but     *
 * WITHOUT ANY WARRANTY; without even the implied warranty of              *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU        *
 * General Public License for more details.                                *
 *                                                                         *
 * You should have received a copy of the GNU General Public License along *
 * with this program; if not, write to the                                 *
 * Free Software Foundation, Inc.,                                         *
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA                   *
 ***************************************************************************/


#include "MiuSocket.h"

MiuSocket::MiuSocket(QObject * parent):QObject(parent)
{
}

MiuSocket::~MiuSocket()
{
    qDebug() << "MiuSocket: Destructor called";
}

/**
 * Accept an established and handshaken SSL socket. (server only)
 * @param socket The socket to be taken over.
 */
void MiuSocket::setMiuSocket(QSslSocket * socket)
{
    pSocket = socket;
    pSocket->setParent(this);
    connect(pSocket, SIGNAL(readyRead()),
            this, SLOT(readDataFromSocket()), Qt::QueuedConnection);
    connect(pSocket, SIGNAL(sslErrors(QList < QSslError >)),
            this, SLOT(on_socket_sslErrors(QList < QSslError >)));
    connect(pSocket, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(handleSocketError()));
}


/**
 * Connected to the readyRead() signal of the SslSocket. Reads data from the
 * TCP buffer and appends it to the local buffer for further processing.
 */
void MiuSocket::readDataFromSocket()
{
    pBuffer += pSocket->readAll();
    qDebug() << "MiuSocket: Received data, buffer now" << pBuffer.length();

    quint32 tmpLen;

    while (bufferHasMessage(pBuffer))
    {
        QDataStream stream(pBuffer);

        stream >> tmpLen;
        QByteArray tmpMessage = pBuffer.left(tmpLen);
        MiuTransaction t;

        if (t.parseFromData(tmpMessage))
        {
            qDebug() << "MiuSocket: = Got transaction: S=" << tmpLen << "/" <<
                t.sequence << " T=" << t.type << " O=" << t.objects.count();
            emit transactionReceived(t);

        }
        else
        {
            qDebug() << "MiuSocket: Dropping corrupted transaction.";
        }

        pBuffer = pBuffer.mid(tmpLen);
    }

}


// Called by the socket and indicates the an SSL error has occoured.
void MiuSocket::handleSocketSslError(const QList < QSslError > &errors)
{
    qDebug() << "MiuSocket.handleSocketSslError(): " << errors;
    pSocket->ignoreSslErrors();
}


/**
 * A socket error occoured.
 */
void MiuSocket::handleSocketError()
{
    qDebug() << "MiuSocket: socket error:" << pSocket->
        errorString() << pSocket->error();
    disconnectClient();
}




/**
 * Returns true if there are more transactions in the buffer.
 */
bool MiuSocket::bufferHasMessage(QByteArray & buffer)
{
    if (buffer.length() < 14)
        return false;
    QDataStream stream(buffer);
    quint32 tmpLen;

    stream >> tmpLen;
    return (quint32) buffer.length() >= tmpLen;
}


/**
 * The client needs to be removed from the server. Delete the socket and this object.
 */
void MiuSocket::disconnectClient()
{
    qDebug() << "MiuSocket: Called disconnectClient()";
    emit clientDisconnected();

    pSocket->disconnectFromHost();
    this->deleteLater();
}

/**
 * Send a transaction package to the remote peer.
 */
void MiuSocket::sendTransaction(const MiuTransaction & t)
{
    qDebug() << "MiuSocket: Sending transaction of type" << t.type;
    pSocket->write(t.toData());
}
