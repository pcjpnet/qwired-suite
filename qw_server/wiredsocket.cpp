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


void WiredSocket::sendServerInformation(const QString serverName, const QString serverDescr, const QDateTime startTime, const int fileCount, const int fileTotalSize) {
	QByteArray ba("200 ");
	ba += "Qwired Server/1.0.0 (Unknown; Unknown; Unknown)"; ba += kFS;
	ba += "1.1"; ba += kFS;
	ba += serverName.toUtf8(); ba += kFS;
	ba += serverDescr.toUtf8(); ba += kFS;
	ba += startTime.toString(Qt::ISODate); ba += "+00:00"; ba += kFS;
	ba += QByteArray::number(fileCount); ba += kFS;
	ba += QByteArray::number(fileTotalSize); ba += kFS;
	sendWiredCommand(ba);
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
 * Send a broadcast message to the client.
 * @param userId The ID of the originator.
 * @param text The text of the broadcast message.
 */
void WiredSocket::sendBroadcastMessage(const int userId, const QString text) {
	QByteArray ba("309 ");
	ba += QByteArray::number(userId); ba += kFS;
	ba += text.toUtf8();
	sendWiredCommand(ba);
}


/**
 * Send a line of text chat to the connected client.
 * @param chatId The ID of the chat this message was sent in.
 * @param userId The ID of the user who originated the chat text.
 * @param text The text of the chat.
 */
void WiredSocket::sendChat(const int chatId, const int userId, const QString text, const bool emote) {
	QByteArray ba(emote ? "301 " : "300 " );
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

/**
 * A client left the server (or private chat).
 * @param chatId The ID of the chat. 1 is public chat.
 * @param id The ID of the user who left.
 */
void WiredSocket::sendClientLeave(const int chatId, const int id) {
	if(id==pSessionUser.pUserID) return;
	QByteArray ba("303 ");
	ba += QByteArray::number(chatId); ba += kFS;
	ba += QByteArray::number(id);
	sendWiredCommand(ba);
}


/**
 * The user changed his/her user image (seen in the user list).
 * @param user The user object with the new information.
 */
void WiredSocket::sendUserImageChanged(const ClassWiredUser user) {
	QByteArray ba("340 ");
	ba += QByteArray::number(user.pIcon);
	ba += kFS;
	ba += user.pImage;
	sendWiredCommand(ba);
}




/**
 * The new private chat has been created and the user has been automatically
 * added to it.
 * @param chatId The ID of the newly created chat.
 */
void WiredSocket::sendPrivateChatCreated(const int chatId) {
	QByteArray ba("330 ");
	ba += QByteArray::number(chatId);
	sendWiredCommand(ba);
}

/**
 * Invite a user to a private, existing chat room.
 * @param chatId The ID of the private chat.
 * @param id The ID of the inviting user.
 */
void WiredSocket::sendInviteToChat(const int chatId, const int id) {
	QByteArray ba("331 ");
	ba += QByteArray::number(chatId); ba += kFS;
	ba += QByteArray::number(id);
	sendWiredCommand(ba);
}


/**
 * Deliver a private message to this user.
 * @param userId The ID of the originator.
 * @param text The text of the message.
 */
void WiredSocket::sendMessage(const int userId, const QString text) {
	QByteArray ba("305 ");
	ba += QByteArray::number(userId); ba += kFS;
	ba += text.toUtf8();
	sendWiredCommand(ba);
}

/**
 * Send the information about a user to the client.
 * @param user The user object containing the requested information.
 */
void WiredSocket::sendUserInfo(const ClassWiredUser user) {
	QByteArray ba("308 ");
	ba += user.userInfoEntry();
	sendWiredCommand(ba);
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
	t.addObject("type", chat.pProtected);
	sendTransaction(t);

}

/**
 * A used declined a private chat invitation. Let the inviting users know of this.
 * @param chatId The ID of the chat.
 * @param userId The ID of the user who rejected the invitation.
 */
void WiredSocket::sendClientDeclinedChat(const int chatId, const int userId) {
	QByteArray ba("332 ");
	ba += QByteArray::number(chatId); ba += kFS;
	ba += QByteArray::number(userId);
	sendWiredCommand(ba);
}

/**
 * Set the client-version string of this client to info. Also set some information about the used
 * encryption, etc...
 * @param info The client-version string.
 */
void WiredSocket::setClientInfo(const QString info) {
	pSessionUser.pClientVersion = info;
}

/**
 * Send a list of privileges to the client.
 */
void WiredSocket::sendPrivileges() {
	QByteArray ba("602 ");
	ba += pSessionUser.privilegesFlags();
	sendWiredCommand(ba);
}


/**
 * A administrator kicked or banned a user.
 * @param killerId The ID of the administrator.
 * @param victimId The ID of the kicked user.
 * @param reason The reason (if any).
 * @param banned If true, the user was temporarily banned.
 */
void WiredSocket::sendClientKicked(const int killerId, const int victimId, const QString reason, const bool banned) {
	QByteArray ba( banned ? "307 " : "306 " );
	ba += QByteArray::number(victimId); ba += kFS;
	ba += QByteArray::number(killerId); ba += kFS;
	ba += reason.toUtf8();
	sendWiredCommand(ba);
}

/**
 * A user just posted a news item. Send a notification to this client.
 * @param nickname The nickname of the user who posted the news item.
 * @param news The text of the news item.
 */
void WiredSocket::sendNewsPosted(const QString nickname, const QString news) {
	QByteArray ba("322 ");
	ba += nickname.toUtf8(); ba += kFS;
	ba += QDateTime::currentDateTime().toUTC().toString(Qt::ISODate).append("+00:00"); ba += kFS;
	ba += news.toUtf8();
	sendWiredCommand(ba);
}

/**
 * Transmit a news entry.
 * @param nickname The nickname of the user who posted the news item.
 * @param date The date this news item was posted.
 * @param news The text of the news item.
 */
void WiredSocket::sendNews(const QString nickname, const QDateTime date, const QString news) {
	QByteArray ba("320 ");
	ba += nickname.toUtf8(); ba += kFS;
	ba += date.toUTC().toString(Qt::ISODate).append("+00:00"); ba += kFS;
	ba += news.toUtf8();
	sendWiredCommandBuffer(ba);
}


/**
 * The list of news has been transmitted.
 */
void WiredSocket::sendNewsDone() { sendWiredCommand("321 Done"); }

/**
 * Send a account listing item to the client.
 * @param name The name of the account.
 */
void WiredSocket::sendAccountListing(const QString name) {
	QByteArray ba("610 ");
	ba += name.toUtf8();
	sendWiredCommandBuffer(ba);
}

/**
 * The list of accounts has been transmitted.
 */
void WiredSocket::sendAccountListingDone() { sendWiredCommand("611 Done"); }

/**
 * Send a account listing item to the client.
 * @param name The name of the account.
 */
void WiredSocket::sendGroupListing(const QString name) {
	QByteArray ba("620 ");
	ba += name.toUtf8();
	sendWiredCommandBuffer(ba);
}

/**
 * The list of accounts has been transmitted.
 */
void WiredSocket::sendGroupListingDone() { sendWiredCommand("621 Done"); }



/**
 * Send the user account specification.
 * @param account The object holding the account data.
 */
void WiredSocket::sendUserSpec(const ClassWiredUser account) {
	QByteArray ba("600 ");
	ba += account.pLogin.toUtf8(); ba += kFS;
	ba += account.pPassword.toUtf8(); ba += kFS;
	ba += account.pGroupName.toUtf8(); ba += kFS;
	ba += account.privilegesFlags();
	sendWiredCommand(ba);
}

/**
 * Send the group specification.
 * @param account The object holding the group data.
 */
void WiredSocket::sendGroupSpec(const ClassWiredUser group) {
	QByteArray ba("601 ");
	ba += group.pGroupName.toUtf8(); ba += kFS;
	ba += group.privilegesFlags();
	sendWiredCommand(ba);
}


void WiredSocket::sendFileListing(const ClassWiredFile file) {
	QByteArray ba("410 ");
	ba += file.path.toUtf8(); ba += kFS;
	ba += QByteArray::number(file.type); ba += kFS;
	ba += QByteArray::number(file.size); ba += kFS;
	ba += file.created.toString(Qt::ISODate); ba += "+00:00"; ba += kFS;
	ba += file.modified.toString(Qt::ISODate); ba += "+00:00"; ba += kFS;
	ba += file.checksum.toUtf8(); ba += kFS;
	ba += file.comment.toUtf8();
	sendWiredCommandBuffer(ba);
}

void WiredSocket::sendFileStat(const ClassWiredFile file) {
	QByteArray ba("402 ");
	ba += file.path.toUtf8(); ba += kFS;
	ba += QByteArray::number(file.type); ba += kFS;
	ba += QByteArray::number(file.size); ba += kFS;
	ba += file.created.toString(Qt::ISODate); ba += "+00:00"; ba += kFS;
	ba += file.modified.toString(Qt::ISODate); ba += "+00:00"; ba += kFS;
	ba += file.checksum.toUtf8(); ba += kFS;
	ba += file.comment.toUtf8();
	sendWiredCommandBuffer(ba);
}

void WiredSocket::sendFileListingDone(const QString path, const int free) {
	QByteArray ba("411 ");
	ba += path.toUtf8(); ba += kFS;
	ba += QByteArray::number(free);
	sendWiredCommand(ba);
}



void WiredSocket::sendErrorClientNotFound() { sendWiredCommand("512 Client Not Found"); }
void WiredSocket::sendErrorPermissionDenied() { sendWiredCommand("516 Permission Denied"); }
void WiredSocket::sendErrorSyntaxError() { sendWiredCommand("503 Syntax Error"); }
void WiredSocket::sendErrorLoginFailed() { sendWiredCommand("510 Login Failed"); }
void WiredSocket::sendErrorCommandNotImplemented() { sendWiredCommand("502 Command Not Implemented"); }
void WiredSocket::sendErrorCommandFailed() { sendWiredCommand("500 Command Failed"); }
void WiredSocket::sendErrorCannotBeDisconnected() { sendWiredCommand("515 Cannot Be Disconnected"); }
void WiredSocket::sendErrorAccountNotFound() { sendWiredCommand("513 Account Not Found"); }
void WiredSocket::sendErrorAccountExists() { sendWiredCommand("514 Account Exists"); }
void WiredSocket::sendErrorFileNotFound() { sendWiredCommand("520 File or Directory Not Found"); }

void WiredSocket::sendTransaction(const QWTransaction & t) {
	qDebug() << "Sending transaction:"<<t.type;
	pSocket->write(t.toData());
}




