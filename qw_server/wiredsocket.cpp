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
#include "qwservercontroller.h"

WiredSocket::WiredSocket(QObject *parent)
	: QObject(parent)
{
	pHandshakeOK = false;
}

WiredSocket::~WiredSocket()
{ }

// Called by the socket and indicates the an SSL error has occoured.
void WiredSocket::on_socket_sslErrors(const QList<QSslError> & errors) {
	qDebug() << "WiredSocket.on_socket_sslErrors(): "<<errors;
	pSocket->ignoreSslErrors();
}

// Called by readDataFromSocket() after splitting a message off the internal
// buffer. We have to extract the command ID here and decide what to do with
// the message.
void WiredSocket::handleWiredMessage(QByteArray theData) {
	int tmpPos = theData.indexOf(" ");
	QString tmpCmd = tmpPos>-1 ? QString::fromUtf8(theData.left(tmpPos)) : QString::fromUtf8(theData);
	QList<QByteArray> tmpParams = splitMessageFields(theData);

	qDebug() << ">> Incoming command:"<<tmpCmd;

	if(!pHandshakeOK) { // No handshake received yet. The only thing we accept is a HELLO.
		pHandshakeOK = tmpCmd=="HELLO";
		if(!pHandshakeOK) {
			QWServerController::qwLog("Fatal: Handshake failed. Dropping connection.");
			pSocket->disconnectFromHost();
		} else {
			qwSendServerInformation();
		}
		
	} else {
		// Handshake exists, handle the command.
		if(tmpCmd=="PASS") {
			qDebug() << "Received password:"<<tmpParams.value(0);
			pSessionUser.pPassword = tmpParams.value(0);
			qwSendLoginSuccessful(20);

		} else if(tmpCmd=="USER") {
			qDebug() << "Received login:"<<tmpParams.value(0);
			pSessionUser.pLogin = tmpParams.value(0);
		} else if(tmpCmd=="NICK") {
			qDebug() << "Received nick:"<<tmpParams.value(0);
			pSessionUser.pNick = tmpParams.value(0);
		} else if(tmpCmd=="ICON") {
			qDebug() << "Received icon:";
			pSessionUser.pImage = tmpParams.value(0);
		} else if(tmpCmd=="WHO") {
			qwSendUserlist(1);
		} else {
			qwSendCommandNotImplemented();
		}
		
	}
	
}

/**
 * A socket error occoured.
 */
void WiredSocket::on_socket_error() {
	qDebug() << "SSL error:"<<pSocket->errorString()<<pSocket->error();
	pSocket->disconnectFromHost();
}


/**
 * Send a Wired-formatted message block to the peer.
 * @param theData The payload of the message.
 */
void WiredSocket::sendWiredCommand(const QByteArray theData) {
	if(!pSocket->isOpen()) return;
	QByteArray tmpBuffer;
	tmpBuffer += theData;
	tmpBuffer += kEOF;
	pSocket->write(tmpBuffer);
}

/**
 * Connected to the readyRead() signal of the SslSocket. Reads data from the
 * TCP buffer and appends it to the local buffer for further processing.
 */
void WiredSocket::readDataFromSocket() {
	pBuffer += pSocket->readAll();
	int tmpPos = pBuffer.indexOf("\x04");
	while( tmpPos != -1 ) {
		QByteArray tmpLine = pBuffer.left(tmpPos);
		pBuffer = pBuffer.mid(tmpPos+1);
		handleWiredMessage(tmpLine);
		tmpPos = pBuffer.indexOf("\x04");
	}
}

/**
 * Return a list of parameters from the message, automatically skipping the command identifier.
 */
QList<QByteArray> WiredSocket::splitMessageFields(QByteArray theData) {
	QByteArray tmpData(theData);
	int tmpPos = theData.indexOf(" "); // Remove the identifier
	if( tmpPos!=-1 ) tmpData=tmpData.mid(tmpPos+1);
	QList<QByteArray> tmpParams = tmpData.split(28);
	return tmpParams;
}

/**
 * Accept an established and handshaken SSL socket.
 * @param socket The socket to be taken over.
 */
void WiredSocket::setWiredSocket(QSslSocket *socket) {
	pSocket = socket;
	pSocket->setParent(this);
	connect(pSocket, SIGNAL(readyRead()), this, SLOT(readDataFromSocket()) );
	connect(pSocket, SIGNAL(sslErrors(QList<QSslError>)), this, SLOT(on_socket_sslErrors(QList<QSslError>)));
	connect(pSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(on_socket_error()) );
}

void WiredSocket::qwSendServerInformation() {
	QByteArray ba("200 ");
	ba += "Qwired Server/1.0.0"; ba += kFS;
	ba += "1.1"; ba += kFS;
	ba += "Qwired Test Server"; ba += kFS;
	ba += "Just a test"; ba += kFS;
	ba += QDateTime::currentDateTime().toString(Qt::ISODate); ba += kFS;
	ba += "0"; ba += kFS;
	ba += "0"; ba += kFS;
	sendWiredCommand(ba);
}

void WiredSocket::qwSendLoginSuccessful(int userId) {
	QByteArray ba("201 ");
	ba += QByteArray::number(userId);
	sendWiredCommand(ba);
}

void WiredSocket::qwSendCommandNotImplemented() {
	sendWiredCommand("502 Command Not Implemented");
}

/**
 * Send the user list in response to the WHO command.
 * @param chatId The chat ID.
 */
void WiredSocket::qwSendUserlist(int chatId) {
	QByteArray ba("310 ");
	ba += QByteArray::number(chatId); ba += kFS;
	ba += pSessionUser.userListEntry();
	sendWiredCommand(ba);

	ba.clear();
	ba += "311 ";
	ba += QByteArray::number(chatId);
	sendWiredCommand(ba);
}

