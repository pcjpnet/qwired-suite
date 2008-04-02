/***************************************************************************
 *   Copyright (C) 2007 by Bastian Bense   *
 *   bb@bense.de   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include "qwservercore.h"


QWServerCore::QWServerCore(QObject *parent)
 : QObject(parent)
{
	pStartTime = QDateTime::currentDateTime().toUTC();
}


QWServerCore::~QWServerCore()
{
}

/**
 * Return a unique user identification number used for user list management.
 */
int QWServerCore::getUniqueUserId() {
	static int tmpUserId=0;
	return ++tmpUserId;
}

int QWServerCore::getUniqueChatId() {
	static int tmpChatId=10;
	return ++tmpChatId;
}


void QWServerCore::registerClient(WiredSocket *socket) {
	if(!socket) return;
	int tmpId = getUniqueUserId();
	socket->setUserId(tmpId);
	pClients[tmpId] = socket;
	connect(socket, SIGNAL(declinedPrivateChat(int,int)), this, SLOT(declinePrivateChat(int,int)) );
	connect(socket, SIGNAL(handshakeComplete(int)), this, SLOT(sendServerInfo(int)) );
	connect(socket, SIGNAL(invitedUserToChat(int,int,int)), this, SLOT(inviteUserToChat(int,int,int)) );
	connect(socket, SIGNAL(joinedPrivateChat(int,int)), this, SLOT(joinPrivateChat(int,int)) );
	connect(socket, SIGNAL(loginReceived(int, QString, QString)), this, SLOT(checkLogin(int, QString, QString)) );
	connect(socket, SIGNAL(privateChatLeft(int,int)), this, SLOT(removeFromPrivateChat(int,int)) );
	connect(socket, SIGNAL(requestedUserInfo(int,int)), this, SLOT(sendUserInfo(int,int)) );
	connect(socket, SIGNAL(requestedUserlist(int,int)), this, SLOT(sendUserlist(int,int)) );
	connect(socket, SIGNAL(receivedBroadcastMessage(int,QString)), this, SLOT(broadcastBroadcast(int,QString)) );
	connect(socket, SIGNAL(receivedChat(int,int,QString,bool)), this, SLOT(broadcastChat(int,int,QString,bool)) );
	connect(socket, SIGNAL(requestedBanner(int)), this, SLOT(sendServerBanner(int)) );
	connect(socket, SIGNAL(requestedPrivateChat(int)), this, SLOT(createPrivateChat(int)) );
	connect(socket, SIGNAL(clientDisconnected(int)), this, SLOT(removeClient(int)) );
	connect(socket, SIGNAL(userImageChanged(ClassWiredUser)), this, SLOT(broadcastUserImageChanged(ClassWiredUser)) );
	connect(socket, SIGNAL(userStatusChanged(ClassWiredUser)), this, SLOT(broadcastUserStatusChanged(ClassWiredUser)) );
	connect(socket, SIGNAL(privateMessageReceived(int,int,QString)), this, SLOT(deliverPrivateMessage(int,int,QString)) );
	qDebug() << "[core] registered client with id"<<socket->userId();
}

/**
 * The client just connected and requested some information about the server.
 * @param id
 */
void QWServerCore::sendServerInfo(const int id) {
	pClients[id]->sendServerInformation("Qwired Server Alpha", "A very early Qwired Server build.", pStartTime, 666, 0);
}


/**
 * A client requested the list of users for a particular chat.
 * @param id The ID of the user.
 * @param chatId The ID of the chat. 1 is public chat.
 */
void QWServerCore::sendUserlist(const int id, const int chatId) {
	qDebug() << "[core] sending user list to client"<<id<<"of chat"<<chatId;
	WiredSocket *tmpSock = pClients[id];
	if(chatId==1) {
		// Public user list
		QHashIterator<int,QPointer<WiredSocket> > i(pClients);
		while(i.hasNext()) { i.next();
			if(i.value()) tmpSock->sendUserlistItem(chatId, i.value()->sessionUser());
		}
		
	} else {
		// Private chat list
		if(!pPrivateChats.contains(chatId)) return;
		QListIterator<int> i(pPrivateChats[chatId].pUsers);
		while(i.hasNext()) {
			int tmpId = i.next();
			if(pClients.contains(tmpId))
				tmpSock->sendUserlistItem(chatId, pClients[tmpId]->sessionUser());
		}
	}
	tmpSock->sendUserlistDone(chatId);
}

/**
 * The client sent the login name and password. Now we have to check them for validity
 * and send a response.
 * @param id The ID of the user.
 * @param login The login name of the user.
 * @param password The password of the user.
 */
void QWServerCore::checkLogin(const int id, QString login, QString password) {
	WiredSocket *socket = pClients[id];
	socket->sendLoginSuccessful();
	
	QHashIterator<int,QPointer<WiredSocket> > i(pClients);
	while(i.hasNext()) { i.next();
		if(i.key()!=id && i.value())
			i.value()->sendClientJoin(1, socket->sessionUser() );
	}
}

/**
 * Send a line of chat to all clients on the server (if chatId is 1) or to
 * participants of a private chat (identified by chatId>0).
 * @param id The ID of the originating user.
 * @param chatId The ID of the target chat. 1 is the public chat.
 * @param text The text of the chat line.
 */
void QWServerCore::broadcastChat(const int id, const int chatId, const QString text, const bool emote) {
	qDebug() << "[core] broadcasting chat from"<<id<<"to chat"<<chatId;
	QHashIterator<int,QPointer<WiredSocket> > i(pClients);
	while(i.hasNext()) { i.next();
		if(i.value()) i.value()->sendChat(chatId, id, text, emote);
	}
}

/**
 * Remove a client from the list of clients (after error or disconnection) and send all
 * clients a client-left message.
 * @param id The ID of the user that left.
 */
void QWServerCore::removeClient(const int id) {
	qDebug() << "[core] broadcasting user-left from"<<id;

	// Check for private chats
	QHashIterator<int,QWClassPrivateChat > j(pPrivateChats);
	while(j.hasNext()) { j.next();
		if(j.value().pUsers.contains(id)) {
			qDebug() << "[core] removed user"<<id<<"from private chat"<<j.key();
			removeFromPrivateChat(id, j.key());
		}
	}

	// Notify everyone else
	QHashIterator<int,QPointer<WiredSocket> > i(pClients);
	while(i.hasNext()) { i.next();
		if(i.value()) i.value()->sendClientLeave(1, id);
	}

	
}

void QWServerCore::broadcastUserImageChanged(const ClassWiredUser user) {
	qDebug() << "[core] broadcasting user-image-changed from"<<user.pUserID;
	QHashIterator<int,QPointer<WiredSocket> > i(pClients);
	while(i.hasNext()) { i.next();
		if(i.value()) i.value()->sendUserImageChanged(user);
	}
}


/**
 * The user status changed (changed nick, status, icon or other parameters).
 * @param user The user object that contains the new information.
 */
void QWServerCore::broadcastUserStatusChanged(const ClassWiredUser user) {
	qDebug() << "[core] broadcasting user-status-changed from"<<user.pUserID;
	QHashIterator<int,QPointer<WiredSocket> > i(pClients);
	while(i.hasNext()) { i.next();
		if(i.value()) i.value()->sendUserStatusChanged(user);
	}
}

/**
 * A user sent a private message to another user.
 * @param id The ID of the sender.
 * @param targetId The ID of the receiver.
 * @param text The text of the message.
 */
void QWServerCore::deliverPrivateMessage(const int id, const int targetId, const QString text) {
	if(!pClients.contains(targetId)) // User not found on the server.
			pClients[id]->sendErrorClientNotFound();
	else	pClients[targetId]->sendPrivateMessage(id, text);
}

/**
 * Send the server banner to the client.
 * @param id The ID of the user.
 */
void QWServerCore::sendServerBanner(const int id) {
	qDebug() << "[core] sending server banner to"<<id;
	pClients[id]->sendBanner(pServerBanner);
}

void QWServerCore::broadcastBroadcast(const int id, const QString text) {
	qDebug() << "[core] broadcasting message from"<<id;
	QHashIterator<int,QPointer<WiredSocket> > i(pClients);
	while(i.hasNext()) { i.next();
		if(i.value()) i.value()->sendBroadcastMessage(id, text);
	}
}

void QWServerCore::createPrivateChat(const int id) {
	int tmpId = getUniqueChatId();
	qDebug() << "[core] creating new chat with id"<<tmpId<<"for user"<<id;
	QWClassPrivateChat tmpChat;
	tmpChat.pUsers.append(id);
	pPrivateChats[tmpId] = tmpChat;
	pClients[id]->sendPrivateChatCreated(tmpId);
}

/**
 * Invite a user to a existing private chat.
 * @param id The ID of the inviting user.
 * @param userId The ID of the invited user.
 * @param chatId The ID of the chat.
 */
void QWServerCore::inviteUserToChat(const int id, const int userId, const int chatId) {
	qDebug() << "[core] inviting user"<<userId<<"to chat"<<chatId<<"from user"<<id;
	if(!pPrivateChats.contains(chatId)) return;
	if(id==userId) return;
	if(!pClients.contains(userId)) { pClients[id]->sendErrorClientNotFound(); return; }
	QWClassPrivateChat &chat = pPrivateChats[chatId];
	if(!chat.pInvitedUsers.contains(userId))
		chat.pInvitedUsers.append(userId);
	pClients[userId]->sendInviteToChat(chatId, id);
	
}

/**
 * Send the information about a user.
 * @param id The ID of the requsting user.
 * @param userId The ID of the user whose information is requsted.
 */
void QWServerCore::sendUserInfo(const int id, const int userId) {
	if(!pClients.contains(userId)) { pClients[id]->sendErrorClientNotFound(); return; }
	pClients[id]->sendUserInfo( pClients[userId]->sessionUser() );
}



/**
 * A user accepted the chat invitation and must be added to the chat.
 * @param id The ID of the user who accepted the chat invitation.
 * @param chatId The ID of the chat.
 */
void QWServerCore::joinPrivateChat(const int id, const int chatId) {
	qDebug() << "[core] client "<<id<<"joins chat"<<chatId;
	if(!pPrivateChats.contains(chatId)) return;
	QWClassPrivateChat &chat = pPrivateChats[chatId];
	if(!chat.pInvitedUsers.contains(id)) return;
	chat.pInvitedUsers.removeAt(chat.pInvitedUsers.indexOf(id));
	chat.pUsers.append(id);

	// Notify other users in the chat
	QListIterator<int> i(chat.pUsers);
	while(i.hasNext()) {
		int tmpId = i.next();
		if(tmpId==id) continue;
		pClients[tmpId]->sendClientJoin(chatId, pClients[id]->sessionUser());
	}
	
}


/**
 * The user declined the invitation to the private chat.
 * @param id The ID of the user who rejected the invitation.
 * @param chatId The ID of the chat.
 */
void QWServerCore::declinePrivateChat(const int id, const int chatId) {
	qDebug() << "[core] client "<<id<<"declined chat"<<chatId;
	if(!pPrivateChats.contains(chatId)) return;
	QWClassPrivateChat &chat = pPrivateChats[chatId];
	if(!chat.pInvitedUsers.contains(id)) return;
	chat.pInvitedUsers.removeAt(chat.pInvitedUsers.indexOf(id));

	// Notify other users in the chat
	QListIterator<int> i(chat.pUsers);
	while(i.hasNext()) {
		pClients[i.next()]->sendClientDeclinedChat(chatId, id);
	}
}


/**
 * A user has left a private chat and needs to be removed from it properly.
 * @param userId The ID of the user to be removed.
 * @param chatId The ID of the chat.
 */
void QWServerCore::removeFromPrivateChat(const int userId, const int chatId) {
	if(!pPrivateChats.contains(chatId)) return;
	QWClassPrivateChat &tmpUsers = pPrivateChats[chatId];
	
	QListIterator<int> i(tmpUsers.pUsers);
	while(i.hasNext()) {
		int tmpId = i.next();
		if(tmpId==userId) continue;
		pClients[tmpId]->sendClientLeave(chatId, userId);
	}
	
	int tmpIndex = tmpUsers.pUsers.indexOf(userId);
	tmpUsers.pUsers.removeAt(tmpIndex);
	qDebug() << "[core] removed user from chat at index"<<tmpIndex;
	if(tmpUsers.pUsers.count()==0) {
		qDebug() << "[core] removing empty chat"<<chatId;
		pPrivateChats.remove(chatId);
	}
}







