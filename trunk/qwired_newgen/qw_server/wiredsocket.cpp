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
	pLoggedIn = false;
	
	pIdleTimer = new QTimer(this);
	pIdleTimer->setInterval(1000*60*10);
	pIdleTimer->setSingleShot(true);
	connect(pIdleTimer, SIGNAL(timeout()), this, SLOT(idleTimerTriggered()));
}

WiredSocket::~WiredSocket() {
	qDebug() << "[qws] Destroying"<<pSessionUser.pUserID;
}

/**
 * Accept an established and handshaken SSL socket.
 * @param socket The socket to be taken over.
 */
void WiredSocket::setWiredSocket(QSslSocket *socket) {
	pSocket = socket;
	pSocket->setParent(this);
	connect(pSocket, SIGNAL(readyRead()), this, SLOT(readDataFromSocket()), Qt::QueuedConnection );
	connect(pSocket, SIGNAL(sslErrors(QList<QSslError>)), this, SLOT(on_socket_sslErrors(QList<QSslError>)));
	connect(pSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(on_socket_error()) );
}

/**
 * Connected to the readyRead() signal of the SslSocket. Reads data from the
 * TCP buffer and appends it to the local buffer for further processing.
 */
void WiredSocket::readDataFromSocket() {
	pBuffer += pSocket->readAll();
	qDebug() << "Received data, buffer now"<<pBuffer.length();

	quint32 tmpLen;
	while(bufferHasMessage(pBuffer)) {
		QDataStream stream(pBuffer);
		stream >> tmpLen;
		QByteArray tmpMessage = pBuffer.left(tmpLen);
		QWTransaction t;
		if( t.parseFromData(tmpMessage) ) {
			qDebug() << "= Got packet: S="<<tmpLen<<"/"<<t.sequence<<" T="<<t.type<<" O="<<t.objects.count();
			handleTransaction(t);
			//emit transactionReceived(userId(), t);
		} else {
			qDebug() << "Dropping corrupted transaction.";
		}
		
		pBuffer = pBuffer.mid(tmpLen);
	}

}



/**
 * The idle timer was triggered. Check if the user is not idle and mark him/her
 * as idle. Afterwards a signal will be emitted so that other clients get updated.
 */
void WiredSocket::idleTimerTriggered() {
	if(!pSessionUser.pIdle) {
		pSessionUser.pIdle = true;
		emit userStatusChanged(pSessionUser, 16);
	}
}

/**
 * The user shows some activity. Check if he/she is idle and un-idle.
 */
void WiredSocket::resetIdleTimer() {
	pIdleTimer->start();
	if(pSessionUser.pIdle) {
		pSessionUser.pIdle = false;
		emit userStatusChanged(pSessionUser, 16);
	}
}

// Called by the socket and indicates the an SSL error has occoured.
void WiredSocket::on_socket_sslErrors(const QList<QSslError> & errors) {
	qDebug() << "WiredSocket.on_socket_sslErrors(): "<<errors;
	pSocket->ignoreSslErrors();
}


void WiredSocket::handleTransaction(const QWTransaction & t) {
	qDebug() << "WiredSocket: Incoming Transaction:"<<t.type;
	
	QWTransaction response = t.toResponse();
	if(t.type == Qwired::T_HandshakeRequest) {
		response.addObject("server", QString("Qwired Server"));
		response.addObject("version", QString("SVN Snapshot"));
		sendTransaction(response);

	} else if(t.type == Qwired::T_SetUserInfo) {
		int tmpChangeFlags = 0;
		if(t.hasObject("nick")) { pSessionUser.pNick = t.getObjectString("nick"); tmpChangeFlags |= 1; }
		if(t.hasObject("status")) { pSessionUser.pStatus = t.getObjectString("status"); tmpChangeFlags |= 2; }
		if(t.hasObject("image")) { pSessionUser.pImage = t.getObject("image"); tmpChangeFlags |= 4; }
		qDebug() << "WiredSocket: Received credentials for"<<pSessionUser.pNick;
		if(isLoggedIn()) {
			emit userStatusChanged(pSessionUser, tmpChangeFlags);
 			resetIdleTimer();
		} 
		
	} else if(t.type == Qwired::T_LoginRequest) {
		emit loginReceived(userId(), t);

	} else if(t.type == Qwired::T_ServerBannerRequest) {
		emit requestedBanner(userId(), t);

	} else if(t.type == Qwired::T_UserListRequest) {
		if(!isLoggedIn()) { sendErrorCommandFailed(); return; }
		emit requestedUserlist(userId(), t);
		
	} else if(t.type == 1012) { // MOTD
		if(!isLoggedIn()) { sendErrorCommandFailed(); return; }
		emit requestedMotd(userId(), t);

	} else if(t.type == 1021) { // Get User Info
		if(!isLoggedIn()) { sendErrorCommandFailed(); return; }
		emit userInfoRequested(userId(), t);

	} else if(t.type == 2001) { // Private message/Broadcast
		emit privateMessageReceived(userId(), t);
		resetIdleTimer();
		
	} else if(t.type == 2002) { // Chat message
		if(!isLoggedIn()) { sendErrorCommandFailed(); return; }
		emit receivedChat(userId(), t);
		resetIdleTimer();

	} else if(t.type == 2013) { // Set conference options
		if(!isLoggedIn()) { sendErrorCommandFailed(); return; }
		emit conferenceOptionsChanged(userId(), t);
		resetIdleTimer();

	} else if(t.type == 2020) { // Request list of conference groups
		if(!isLoggedIn()) { sendErrorCommandFailed(); return; }
		emit newsGroupsRequested(userId(), t);
		resetIdleTimer();

	} else if(t.type == 2021) { // Request list of available news group articles.
		if(!isLoggedIn()) { sendErrorCommandFailed(); return; }
		emit newsGroupArticlesRequested(userId(), t);
		resetIdleTimer();
	}


	
}



// Called by readDataFromSocket() after splitting a message off the internal
// buffer. We have to extract the command ID here and decide what to do with
// the message.
void WiredSocket::handleWiredMessage(QByteArray theData) { }

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
void WiredSocket::sendWiredCommand(const QByteArray theData) { }

void WiredSocket::sendWiredCommandBuffer(const QByteArray theData) {
	qDebug() << userId()<<"|"<<"Sending (buffer): "<<theData;
	pSendBuffer += theData;
	pSendBuffer += kEOF;
}




/**
 * Return if there is a transaction left in the TCP buffer.
 */
bool WiredSocket::bufferHasMessage(QByteArray & buffer) {
	if(buffer.length()<14)
		return false;
	QDataStream stream(buffer);
	quint32 tmpLen;
	stream >> tmpLen;
	return (quint32)buffer.length()>=tmpLen;
}

void WiredSocket::setUserId(int userId) {
	pSessionUser.pUserID = userId;
}

/**
 * The client needs to be removed from the server. Delete the socket and this object.
 */
void WiredSocket::disconnectClient() {
	emit clientDisconnected(pSessionUser.pUserID);
	pSocket->disconnectFromHost();
	this->deleteLater();
}

/**
 * The topic changed (or was requested) of a specific chat.
 * @param chat The object containing the chat information.
 */
void WiredSocket::sendChatTopic(const QWClassPrivateChat chat) {
	QWTransaction t(2511);
	t.addObject("cid", chat.pChatId);
	t.addObject("topic", QString("%1 [%2]").arg(chat.pTopic).arg(chat.pTopicSetter.pNick));
	t.addObject("users", chat.pUsers.count());
	t.addObject("protected", !chat.pPassword.isEmpty());
//	t.addObject("type", chat.pProtected);
	sendTransaction(t);

}

void WiredSocket::sendTransaction(const QWTransaction & t) {
	qDebug() << "Sending transaction:"<<t.type;
	pSocket->write(t.toData());
}





