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

 
#include "wiredsocket.h"

WiredSocket::WiredSocket(QObject *parent)
	: QObject(parent)
{
	// Initialize the socket
	pSocket = new QSslSocket(this);
	pSocket->setProtocol(QSsl::TlsV1);
	connect( pSocket, SIGNAL(encrypted()), this, SLOT(on_socket_encrypted()) );
	connect( pSocket, SIGNAL(readyRead()), this, SLOT(on_socket_readyRead()) );
	connect( pSocket, SIGNAL(sslErrors(QList<QSslError>)), this, SLOT(on_socket_sslErrors(QList<QSslError>)));
	connect( pSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(on_socket_error()) );

}

WiredSocket::~WiredSocket()
{ }


// Called by the socket once it has received some data.
// We will read the buffer, put the data in our own buffer and then read
// message after message from it before passing it to do_handle_wiredmessage().
void WiredSocket::on_socket_readyRead() {
	pBuffer += pSocket->readAll();
	int tmpPos = pBuffer.indexOf("\x04");
	while( tmpPos != -1 ) {
		QByteArray tmpLine = pBuffer.left(tmpPos);
		pBuffer = pBuffer.mid(tmpPos+1);
		do_handle_wiredmessage(tmpLine);
		tmpPos = pBuffer.indexOf("\x04");
	}
}

// Called by the socket and indicates the an SSL error has occoured.
void WiredSocket::on_socket_sslErrors(const QList<QSslError> & errors) {
	qDebug() << "WiredSocket.on_socket_sslErrors(): "<<errors;
	pSocket->ignoreSslErrors();
}

// Called by on_socket_readyRead() after splitting a message off the internal
// buffer. We have to extract the command ID here and decide what to do with
// the message.
void WiredSocket::do_handle_wiredmessage(QByteArray theData) {
}

void WiredSocket::on_socket_error() {
	QString theErrReason = pSocket->errorString();
	int theErr = pSocket->error();
	pSocket->disconnectFromHost();
}

// Send a Wired message
void WiredSocket::sendWiredCommand(const QByteArray theData) {
	if(!pSocket->isOpen()) return;
	QByteArray tmpBuffer;
	tmpBuffer += theData;
	tmpBuffer += kEOF;
	pSocket->write(tmpBuffer);
}

