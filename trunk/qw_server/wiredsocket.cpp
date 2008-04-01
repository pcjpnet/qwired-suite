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

WiredSocket::~WiredSocket() {
	qDebug() << "[qws] Destroying"<<pSessionUser.pUserID;
}

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
			qDebug() << "[qws] Received password:"<<tmpParams.value(0);
			pSessionUser.pPassword = tmpParams.value(0);
			emit loginReceived(pSessionUser.pUserID, pSessionUser.pLogin, pSessionUser.pPassword);
			
		} else if(tmpCmd=="PING") {
			sendWiredCommand("202 Pong");
			
		} else if(tmpCmd=="USER") {
			qDebug() << "[qws] Received login:"<<tmpParams.value(0);
			pSessionUser.pLogin = tmpParams.value(0);
			
		} else if(tmpCmd=="NICK") {
			qDebug() << "Received nick:"<<tmpParams.value(0);
			pSessionUser.pNick = tmpParams.value(0);
			if(isLoggedIn()) emit userStatusChanged(pSessionUser);
		
			
		} else if(tmpCmd=="ICON") {
			pSessionUser.pIcon = tmpParams.value(0).toInt();
			pSessionUser.pImage = tmpParams.value(1);
			if(isLoggedIn()) emit userStatusChanged(pSessionUser);
			
		} else if(tmpCmd=="STATUS") {
			pSessionUser.pStatus = tmpParams.value(0);
			if(isLoggedIn()) emit userStatusChanged(pSessionUser);
			
		} else if(tmpCmd=="SAY") {
			emit receivedChat(pSessionUser.pUserID,
						tmpParams.value(0).toInt(),
						QString::fromUtf8(tmpParams.value(1)) );
			
		} else if(tmpCmd=="WHO") {
			emit requestedUserlist(pSessionUser.pUserID, tmpParams.value(0).toInt());
			
		} else {
			
// 			qwSendCommandNotImplemented();
		}
		
	}
	
}

/**
 * A socket error occoured.
 */
void WiredSocket::on_socket_error() {
	qDebug() << "[qws] socket error:"<<pSocket->errorString()<<pSocket->error();
	disconnectClient();
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
	ba += pServerName.toUtf8(); ba += kFS;
	ba += pServerDesc.toUtf8(); ba += kFS;
	ba += QDateTime::currentDateTime().toString(Qt::ISODate); ba += kFS;
	ba += "0"; ba += kFS;
	ba += "0"; ba += kFS;
	sendWiredCommand(ba);
}

/**
 * The user login and password were checked and accepted.
 * @param userId 
 */
void WiredSocket::qwSendLoginSuccessful(int userId) {
	QByteArray ba("201 ");
	ba += QByteArray::number(userId);
	sendWiredCommand(ba);
}

void WiredSocket::qwSendCommandNotImplemented() {
	sendWiredCommand("502 Command Not Implemented");
}


void WiredSocket::setUserId(int userId) {
	pSessionUser.pUserID = userId;
}

void WiredSocket::setServerInformation(QString serverName, QString serverDesc) {
	pServerName = serverName;
	pServerDesc = serverDesc;
	if(pSocket->isOpen())
		qwSendServerInformation();
}



void WiredSocket::sendUserlistItem(const int chat, const ClassWiredUser item) {
	QByteArray ba("310 ");
	ba += QByteArray::number(chat);
	ba += kFS;
	ba += item.userListEntry();
	sendWiredCommand(ba);
}

void WiredSocket::sendUserlistDone(const int chat) {
	QByteArray ba("311 ");
	ba += QByteArray::number(chat);
	sendWiredCommand(ba);
}

/**
 * The server accepted the sent login and password.
 */
void WiredSocket::acceptLogin() {
	QByteArray ba("201 ");
	ba += QByteArray::number(pSessionUser.pUserID);
	sendWiredCommand(ba);
}

/**
 * The login failed. Crashes Wired if we disconnect before.
 */
void WiredSocket::rejectLogin() {
	sendWiredCommand("510 Login Failed");
// 	pSocket->waitForBytesWritten(500);
// 	disconnectClient();
}

void WiredSocket::disconnectClient() {
	emit clientDisconnected(pSessionUser.pUserID);
	pSocket->disconnectFromHost();
	this->deleteLater();
}

/**
 * Send a line of text chat to the connected client.
 * @param chatId The ID of the chat this message was sent in.
 * @param userId The ID of the user who originated the chat text.
 * @param text The text of the chat.
 */
void WiredSocket::sendChat(const int chatId, const int userId, const QString text) {
	QByteArray ba("300 ");
	ba += QByteArray::number(chatId); ba += kFS;
	ba += QByteArray::number(userId); ba += kFS;
	ba += text.toUtf8();
	sendWiredCommand(ba);
}

/**
 * Notify that client that a user has joined the server (if chatId=1) or
 * a private chat (if chatId>1).
 * @param chat The chat ID (1 for server).
 * @param user The user information to be sent.
 */
void WiredSocket::sendClientJoin(const int chatId, const ClassWiredUser user) {
	if(user.pUserID==pSessionUser.pUserID) return;
	QByteArray ba("302 ");
	ba += QByteArray::number(chatId); ba += kFS;
	ba += user.userListEntry();
	sendWiredCommand(ba);
}

void WiredSocket::sendClientLeave(const int chatId, const int id) {
	if(id==pSessionUser.pUserID) return;
	QByteArray ba("303 ");
	ba += QByteArray::number(chatId); ba += kFS;
	ba += QByteArray::number(id);
	sendWiredCommand(ba);
}

/**
 * The user changed his/her nick, status, icon id or idle flags.
 * @param user The user object with the new information.
 */
void WiredSocket::sendUserStatusChanged(const ClassWiredUser user) {
	QByteArray ba("304 ");
	ba += user.userStatusEntry();
	sendWiredCommand(ba);
}




