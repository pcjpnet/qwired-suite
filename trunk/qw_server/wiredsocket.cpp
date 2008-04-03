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
 * The idle timer was triggered. Check if the user is not idle and mark him/her
 * as idle. Afterwards a signal will be emitted so that other clients get updated.
 */
void WiredSocket::idleTimerTriggered() {
	if(!pSessionUser.pIdle) {
		pSessionUser.pIdle = true;
		emit userStatusChanged(pSessionUser);
	}
}

/**
 * The user shows some activity. Check if he/she is idle and un-idle.
 */
void WiredSocket::resetIdleTimer() {
	pIdleTimer->start();
	if(pSessionUser.pIdle) {
		pSessionUser.pIdle = false;
		emit userStatusChanged(pSessionUser);
	}
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

	qDebug() << userId()<<"| Incoming command:"<<tmpCmd;
	if(!pHandshakeOK) { // No handshake received yet. The only thing we accept is a HELLO.
		pHandshakeOK = tmpCmd=="HELLO";
		if(!pHandshakeOK) {
			QWServerController::qwLog("Fatal: Handshake failed. Dropping connection.");
			pSocket->disconnectFromHost();
		} else {
			emit handshakeComplete(userId());
		}

	} else {
		// Handshake exists, handle the command.


		if(tmpCmd=="BROADCAST") {
			if(tmpParams.count()!=1) { sendErrorSyntaxError(); return; }
			if(!isLoggedIn()) { sendErrorPermissionDenied(); return; }
			if(!pSessionUser.privBroadcast) { sendErrorPermissionDenied(); return; }
			emit receivedBroadcastMessage(userId(), QString::fromUtf8(tmpParams.value(0)));
			resetIdleTimer();

		} else if(tmpCmd=="BAN") {
			if(tmpParams.count()!=2) { sendErrorSyntaxError(); return; }
			if(!isLoggedIn()) { sendErrorPermissionDenied(); return; }
			if(!pSessionUser.privBanUsers) { sendErrorPermissionDenied(); return; }
			emit userKicked(userId(), tmpParams.value(0).toInt(), QString::fromUtf8(tmpParams.value(1)), true );
			resetIdleTimer();
			
		} else if(tmpCmd=="BANNER") {
			if(tmpParams.count()!=0) { sendErrorSyntaxError(); return; }
			if(!isLoggedIn()) { sendErrorPermissionDenied(); return; }
			emit requestedBanner(userId());

		} else if(tmpCmd=="CLEARNEWS") {
			if(tmpParams.count()!=0) { sendErrorSyntaxError(); return; }
			if(!isLoggedIn()) { sendErrorPermissionDenied(); return; }
			if(!pSessionUser.privClearNews) { sendErrorPermissionDenied(); return; }
			emit clearedNews(userId());

		} else if(tmpCmd=="DELETEUSER") {
			if(tmpParams.count()!=1) { sendErrorSyntaxError(); return; }
			if(!isLoggedIn()) { sendErrorPermissionDenied(); return; }
			if(!pSessionUser.privDeleteAccounts) { sendErrorPermissionDenied(); return; }
			emit deletedUser(userId(), QString::fromUtf8(tmpParams.value(0)));

		} else if(tmpCmd=="DELETEGROUP") {
			if(tmpParams.count()!=1) { sendErrorSyntaxError(); return; }
			if(!isLoggedIn()) { sendErrorPermissionDenied(); return; }
			if(!pSessionUser.privDeleteAccounts) { sendErrorPermissionDenied(); return; }
			emit deletedGroup(userId(), QString::fromUtf8(tmpParams.value(0)));

		} else if(tmpCmd=="CREATEUSER") {
			if(tmpParams.count()!=26) { sendErrorSyntaxError(); return; }
			if(!isLoggedIn()) { sendErrorPermissionDenied(); return; }
			if(!pSessionUser.privCreateAccounts) { sendErrorPermissionDenied(); return; }
			ClassWiredUser tmpUser;
			tmpUser.pLogin = QString::fromUtf8(tmpParams.value(0));
			tmpUser.pPassword = QString::fromUtf8(tmpParams.value(1));
			tmpUser.pGroupName = QString::fromUtf8(tmpParams.value(2));
			tmpParams.removeAt(0);
			tmpParams.removeAt(0);
			tmpParams.removeAt(0);
			tmpUser.setFromPrivileges(tmpParams);
			emit createdUser(userId(), tmpUser);

		} else if(tmpCmd=="CREATEGROUP") {
			if(tmpParams.count()!=24) { sendErrorSyntaxError(); return; }
			if(!isLoggedIn()) { sendErrorPermissionDenied(); return; }
			if(!pSessionUser.privCreateAccounts) { sendErrorPermissionDenied(); return; }
			ClassWiredUser tmpUser;
			tmpUser.pGroupName = QString::fromUtf8(tmpParams.value(0));
			tmpParams.removeAt(0);
			tmpUser.setFromPrivileges(tmpParams);
			emit createdGroup(userId(), tmpUser);
			
		} else if(tmpCmd=="CLIENT") {
			if(tmpParams.count()!=1) { sendErrorSyntaxError(); return; }
			emit receivedClientInfo(userId(), QString::fromUtf8(tmpParams.value(0)) );
			
		} else if(tmpCmd=="DECLINE") {
			if(tmpParams.count()!=1) { sendErrorSyntaxError(); return; }
			if(!isLoggedIn()) { sendErrorPermissionDenied(); return; }
			emit declinedPrivateChat(userId(), tmpParams.value(0).toInt() );

		} else if(tmpCmd=="EDITUSER") {
			if(tmpParams.count()!=26) { sendErrorSyntaxError(); return; }
			if(!isLoggedIn()) { sendErrorPermissionDenied(); return; }
			if(!pSessionUser.privEditAccounts) { sendErrorPermissionDenied(); return; }
			ClassWiredUser tmpUser;
			tmpUser.pLogin = QString::fromUtf8(tmpParams.value(0));
			tmpUser.pPassword = QString::fromUtf8(tmpParams.value(1));
			tmpUser.pGroupName = QString::fromUtf8(tmpParams.value(2));
			tmpParams.removeAt(0);
			tmpParams.removeAt(0);
			tmpParams.removeAt(0);
			qDebug() << tmpParams;
			tmpUser.setFromPrivileges(tmpParams);
			emit editedUser(userId(), tmpUser);

		} else if(tmpCmd=="EDITGROUP") {
			if(tmpParams.count()!=24) { sendErrorSyntaxError(); return; }
			if(!isLoggedIn()) { sendErrorPermissionDenied(); return; }
			if(!pSessionUser.privEditAccounts) { sendErrorPermissionDenied(); return; }
			ClassWiredUser tmpUser;
			tmpUser.pGroupName = QString::fromUtf8(tmpParams.value(0));
			tmpParams.removeAt(0);
			tmpUser.setFromPrivileges(tmpParams);
			emit editedGroup(userId(), tmpUser);
			
		} else if(tmpCmd=="PING") {
			if(tmpParams.count()!=0) { sendErrorSyntaxError(); return; }
			sendWiredCommand("202 Pong");

		} else if(tmpCmd=="GROUPS") {
			if(tmpParams.count()!=0) { sendErrorSyntaxError(); return; }
			if(!isLoggedIn()) { sendErrorCommandFailed(); return; }
			if(!pSessionUser.privEditAccounts) { sendErrorPermissionDenied(); return; }
			emit requestedGroupsList(userId());
			resetIdleTimer();
			
		} else if(tmpCmd=="INFO") {
			if(tmpParams.count()!=1) { sendErrorSyntaxError(); return; }
			if(!isLoggedIn()) { sendErrorPermissionDenied(); return; }
			if(!pSessionUser.privGetUserInfo) { sendErrorPermissionDenied(); return; }
			emit requestedUserInfo(userId(), tmpParams.value(0).toInt() );
			resetIdleTimer();

		} else if(tmpCmd=="INVITE") {
			if(tmpParams.count()!=2) { sendErrorSyntaxError(); return; }
			if(!isLoggedIn()) { sendErrorPermissionDenied(); return; }
			emit invitedUserToChat(userId(), tmpParams.value(0).toInt(), tmpParams.value(1).toInt() );
			resetIdleTimer();
			
		} else if(tmpCmd=="JOIN") {
			if(tmpParams.count()!=1) { sendErrorSyntaxError(); return; }
			if(!isLoggedIn()) { sendErrorPermissionDenied(); return; }
			emit joinedPrivateChat(userId(), tmpParams.value(0).toInt() );
			resetIdleTimer();

		} else if(tmpCmd=="KICK") {
			if(tmpParams.count()!=2) { sendErrorSyntaxError(); return; }
			if(!isLoggedIn()) { sendErrorPermissionDenied(); return; }
			if(!pSessionUser.privKickUsers) { sendErrorPermissionDenied(); return; }
			emit userKicked(userId(), tmpParams.value(0).toInt(), QString::fromUtf8(tmpParams.value(1)), false );
			resetIdleTimer();
			
		} else if(tmpCmd=="LEAVE") {
			if(tmpParams.count()!=1) { sendErrorSyntaxError(); return; }
			if(!isLoggedIn()) { sendErrorPermissionDenied(); return; }
			emit privateChatLeft(userId(), tmpParams.value(0).toInt());
			resetIdleTimer();

		} else if(tmpCmd=="NEWS") {
			if(tmpParams.count()!=0) { sendErrorSyntaxError(); return; }
			if(!isLoggedIn()) { sendErrorPermissionDenied(); return; }
			emit requestedNews(userId());
			resetIdleTimer();
			
		} else if(tmpCmd=="NICK") {
			if(tmpParams.count()!=1) { sendErrorSyntaxError(); return; }
			pSessionUser.pNick = QString::fromUtf8(tmpParams.value(0));
			if(isLoggedIn()) emit userStatusChanged(pSessionUser);
			resetIdleTimer();

		} else if(tmpCmd=="ME") {
			if(tmpParams.count()!=2) { sendErrorSyntaxError(); return; }
			emit receivedChat(pSessionUser.pUserID, tmpParams.value(0).toInt(), QString::fromUtf8(tmpParams.value(1)), true );
			resetIdleTimer();
			
		} else if(tmpCmd=="MSG") {
			if(tmpParams.count()!=2) { sendErrorSyntaxError(); return; }
			if(!isLoggedIn()) { sendErrorPermissionDenied(); return; }
			emit privateMessageReceived(userId(), tmpParams.value(0).toInt(), QString::fromUtf8(tmpParams.value(1)));
			resetIdleTimer();

		} else if(tmpCmd=="ICON") {
			if(tmpParams.count()!=2) { sendErrorSyntaxError(); return; }
			pSessionUser.pIcon = tmpParams.value(0).toInt();
			pSessionUser.pImage = tmpParams.value(1);
			if(isLoggedIn()) {
				emit userStatusChanged(pSessionUser);
				emit userImageChanged(pSessionUser);
				resetIdleTimer();
			}
			
		} else if(tmpCmd=="PASS") {
			if(tmpParams.count()!=1) { sendErrorSyntaxError(); return; }
			if(isLoggedIn()) { sendErrorCommandFailed(); return; }
			pSessionUser.pPassword = QString::fromUtf8(tmpParams.value(0));
			emit loginReceived(pSessionUser.pUserID, pSessionUser.pLogin, pSessionUser.pPassword);

		} else if(tmpCmd=="POST") {
			if(tmpParams.count()!=1) { sendErrorSyntaxError(); return; }
			if(!isLoggedIn()) { sendErrorCommandFailed(); return; }
			if(!pSessionUser.privPostNews) { sendErrorPermissionDenied(); return; }
			resetIdleTimer();
			emit newsPosted(userId(), QString::fromUtf8(tmpParams.value(0)));
			
		} else if(tmpCmd=="PRIVCHAT") {
			if(tmpParams.count()!=0) { sendErrorSyntaxError(); return; }
			if(!isLoggedIn()) { sendErrorCommandFailed(); return; }
			emit requestedPrivateChat(userId());
			resetIdleTimer();

		} else if(tmpCmd=="PRIVILEGES") {
			if(tmpParams.count()!=0) { sendErrorSyntaxError(); return; }
			if(!isLoggedIn()) { sendErrorCommandFailed(); return; }
			sendPrivileges();
			
		} else if(tmpCmd=="STATUS") {
			if(tmpParams.count()!=1) { sendErrorSyntaxError(); return; }
			pSessionUser.pStatus = tmpParams.value(0);
			if(isLoggedIn()) {
				emit userStatusChanged(pSessionUser);
				resetIdleTimer();
			}
			
		} else if(tmpCmd=="TOPIC") {
			if(tmpParams.count()!=2) { sendErrorSyntaxError(); return; }
			if(!isLoggedIn()) { sendErrorCommandFailed(); return; }
			if(!pSessionUser.privChangeTopic) { sendErrorPermissionDenied(); return; }
			emit topicChanged(sessionUser(), tmpParams.value(0).toInt(), QString::fromUtf8(tmpParams.value(1)) );
			resetIdleTimer();

		} else if(tmpCmd=="READUSER") {
			if(tmpParams.count()!=1) { sendErrorSyntaxError(); return; }
			if(!isLoggedIn()) { sendErrorCommandFailed(); return; }
			if(!pSessionUser.privEditAccounts) { sendErrorPermissionDenied(); return; }
			emit requestedReadUser(userId(), QString::fromUtf8(tmpParams.value(0)) );
			resetIdleTimer();

		} else if(tmpCmd=="READGROUP") {
			if(tmpParams.count()!=1) { sendErrorSyntaxError(); return; }
			if(!isLoggedIn()) { sendErrorCommandFailed(); return; }
			if(!pSessionUser.privEditAccounts) { sendErrorPermissionDenied(); return; }
			emit requestedReadGroup(userId(), QString::fromUtf8(tmpParams.value(0)) );
			resetIdleTimer();
		
		} else if(tmpCmd=="SAY") {
			if(tmpParams.count()!=2) { sendErrorSyntaxError(); return; }
			if(!isLoggedIn()) { sendErrorCommandFailed(); return; }
			emit receivedChat(pSessionUser.pUserID, tmpParams.value(0).toInt(), QString::fromUtf8(tmpParams.value(1)), false );
			resetIdleTimer();
			
		} else if(tmpCmd=="USER") {
			if(tmpParams.count()!=1) { sendErrorSyntaxError(); return; }
			if(isLoggedIn()) { sendErrorCommandFailed(); return; }
			pSessionUser.pLogin = tmpParams.value(0);

		} else if(tmpCmd=="USERS") {
			if(tmpParams.count()!=0) { sendErrorSyntaxError(); return; }
			if(!isLoggedIn()) { sendErrorCommandFailed(); return; }
			if(!pSessionUser.privEditAccounts) { sendErrorPermissionDenied(); return; }
			emit requestedAccountsList(userId());
			resetIdleTimer();
			
		} else if(tmpCmd=="WHO") {
			if(tmpParams.count()!=1) { sendErrorSyntaxError(); return; }
			if(!isLoggedIn()) { sendErrorCommandFailed(); return; }
			emit requestedUserlist(pSessionUser.pUserID, tmpParams.value(0).toInt());

		} else {
			qDebug() << "Unknown Command |"<<userId()<<"|"<<tmpCmd;
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
// 	qDebug() << userId()<<"|"<<"Sending: "<<theData;
	QByteArray tmpBuffer;
	tmpBuffer += pSendBuffer;
	tmpBuffer += theData;
	tmpBuffer += kEOF;
	pSendBuffer.clear();
	pSocket->write(tmpBuffer);
}

void WiredSocket::sendWiredCommandBuffer(const QByteArray theData) {
	qDebug() << userId()<<"|"<<"Sending (buffer): "<<theData;
	pSendBuffer += theData;
	pSendBuffer += kEOF;
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
	if(tmpPos!=-1) {
		tmpData=tmpData.mid(tmpPos+1);
		return tmpData.split(28);
	}
	return QList<QByteArray>();
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
void WiredSocket::sendLoginSuccessful() {
	pSessionUser.pLoginTime = QDateTime::currentDateTime();
	pLoggedIn = true;
	QByteArray ba("201 ");
	ba += QByteArray::number(pSessionUser.pUserID);
	sendWiredCommand(ba);
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
 * Send a server banner to the client.
 * @param banner The PNG data of the banner.
 */
void WiredSocket::sendBanner(const QByteArray banner) {
	QByteArray ba("203 ");
	ba += banner.toBase64();
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
 * The user changed his/her nick, status, icon id or idle flags.
 * @param user The user object with the new information.
 */
void WiredSocket::sendUserStatusChanged(const ClassWiredUser user) {
	QByteArray ba("304 ");
	ba += user.userStatusEntry();
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
void WiredSocket::sendPrivateMessage(const int userId, const QString text) {
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
	QByteArray ba("341 ");
	ba += QByteArray::number(chat.pChatId); ba += kFS;
	ba += chat.pTopicSetter.pNick.toUtf8(); ba += kFS;
	ba += chat.pTopicSetter.pLogin.toUtf8(); ba += kFS;
	ba += chat.pTopicSetter.pIP.toUtf8(); ba += kFS;
	ba += chat.pTopicDate.toString(Qt::ISODate); ba += "+00:00"; ba += kFS;
	ba += chat.pTopic.toUtf8();
	sendWiredCommand(ba);
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



void WiredSocket::sendErrorClientNotFound() { sendWiredCommand("512 Client Not Found"); }
void WiredSocket::sendErrorPermissionDenied() { sendWiredCommand("516 Permission Denied"); }
void WiredSocket::sendErrorSyntaxError() { sendWiredCommand("503 Syntax Error"); }
void WiredSocket::sendErrorLoginFailed() { sendWiredCommand("510 Login Failed"); }
void WiredSocket::sendErrorCommandNotImplemented() { sendWiredCommand("502 Command Not Implemented"); }
void WiredSocket::sendErrorCommandFailed() { sendWiredCommand("500 Command Failed"); }
void WiredSocket::sendErrorCannotBeDisconnected() { sendWiredCommand("515 Cannot Be Disconnected"); }

void WiredSocket::sendErrorAccountNotFound() { sendWiredCommand("513 Account Not Found"); }
void WiredSocket::sendErrorAccountExists() { sendWiredCommand("514 Account Exists"); }



