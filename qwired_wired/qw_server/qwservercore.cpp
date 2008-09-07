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
#include "qwservercontroller.h"

QWServerCore::QWServerCore(QObject *parent)
 : QObject(parent)
{
	pStartTime = QDateTime::currentDateTime().toUTC();
	pPublicChat.pChatId = 1;
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
	connect(socket, SIGNAL(clearedNews(int)), this, SLOT(clearNews(int)) );
	connect(socket, SIGNAL(createdUser(int,ClassWiredUser)), this, SLOT(createUser(int,ClassWiredUser)) );
	connect(socket, SIGNAL(createdGroup(int,ClassWiredUser)), this, SLOT(createGroup(int,ClassWiredUser)) );
	connect(socket, SIGNAL(editedUser(int,ClassWiredUser)), this, SLOT(editUser(int,ClassWiredUser)) );
	connect(socket, SIGNAL(editedGroup(int,ClassWiredUser)), this, SLOT(editGroup(int,ClassWiredUser)) );
	connect(socket, SIGNAL(deletedUser(int,QString)), this, SLOT(deleteUser(int,QString)) );
	connect(socket, SIGNAL(deletedGroup(int,QString)), this, SLOT(deleteGroup(int,QString)) );
	connect(socket, SIGNAL(declinedPrivateChat(int,int)), this, SLOT(declinePrivateChat(int,int)) );
	connect(socket, SIGNAL(handshakeComplete(int)), this, SLOT(sendServerInfo(int)) );
	connect(socket, SIGNAL(invitedUserToChat(int,int,int)), this, SLOT(inviteUserToChat(int,int,int)) );
	connect(socket, SIGNAL(joinedPrivateChat(int,int)), this, SLOT(joinPrivateChat(int,int)) );
	connect(socket, SIGNAL(loginReceived(int, QString, QString)), this, SLOT(checkLogin(int, QString, QString)) );
	connect(socket, SIGNAL(newsPosted(int,QString)), this, SLOT(postNews(int,QString)));
	connect(socket, SIGNAL(privateChatLeft(int,int)), this, SLOT(removeFromPrivateChat(int,int)) );
	connect(socket, SIGNAL(requestedUserInfo(int,int)), this, SLOT(sendUserInfo(int,int)) );
	connect(socket, SIGNAL(requestedUserlist(int,int)), this, SLOT(sendUserlist(int,int)) );
	connect(socket, SIGNAL(receivedClientInfo(int,QString)), this, SLOT(setClientInfo(int,QString)) );
	connect(socket, SIGNAL(receivedBroadcastMessage(int,QString)), this, SLOT(broadcastBroadcast(int,QString)) );
	connect(socket, SIGNAL(receivedChat(int,int,QString,bool)), this, SLOT(broadcastChat(int,int,QString,bool)) );
	connect(socket, SIGNAL(requestedAccountsList(int)), this, SLOT(sendAccountsList(int)) );
	connect(socket, SIGNAL(requestedGroupsList(int)), this, SLOT(sendGroupsList(int)) );
	connect(socket, SIGNAL(requestedBanner(int)), this, SLOT(sendServerBanner(int)) );
	connect(socket, SIGNAL(requestedFileList(int,QString)), this, SLOT(sendFileList(int,QString)));
	connect(socket, SIGNAL(requestedFileStat(int,QString)), this, SLOT(sendFileStat(int,QString)));
	connect(socket, SIGNAL(requestedNews(int)), this, SLOT(sendNews(int)) );
	connect(socket, SIGNAL(requestedPrivateChat(int)), this, SLOT(createPrivateChat(int)) );
	connect(socket, SIGNAL(requestedReadUser(int,QString)), this, SLOT(sendUserSpec(int,QString)));
	connect(socket, SIGNAL(requestedReadGroup(int,QString)), this, SLOT(sendGroupSpec(int,QString)));
	connect(socket, SIGNAL(topicChanged(ClassWiredUser,int,QString)), this, SLOT(broadcastChatTopic(ClassWiredUser,int,QString)) );
	connect(socket, SIGNAL(clientDisconnected(int)), this, SLOT(removeClient(int)) );
	connect(socket, SIGNAL(userImageChanged(ClassWiredUser)), this, SLOT(broadcastUserImageChanged(ClassWiredUser)) );
	connect(socket, SIGNAL(userKicked(int,int,QString,bool)), this, SLOT(kickUser(int,int,QString,bool)) );
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
void QWServerCore::checkLogin(const int id, const QString login, const QString password) {
	WiredSocket *socket = pClients[id];

	QSqlQuery query;
	query.prepare("SELECT privileges, groupname FROM qw_accounts WHERE login=:login AND password=:password");
	query.bindValue(":login", login);
	query.bindValue(":password", password);
	if(query.exec()) {
		query.first();
		if(query.isValid()) {
			socket->pSessionUser.pLogin = login;
			socket->pSessionUser.pPassword = password;
			socket->pSessionUser.pGroupName = query.value(1).toString();
			socket->pSessionUser.setPrivilegesFromAccount(query.value(0).toString());
			socket->sendLoginSuccessful();
			socket->sendChatTopic(pPublicChat);
		} else { // Login failed
			socket->sendErrorLoginFailed();
			return;
		}
	} else {
		qDebug() << "SQL error:"<<query.lastError().text();
		socket->sendErrorLoginFailed();
		return;
	}

	// Announce the user
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

/**
 * A client needs to be disconnected from the server.
 * @param id The ID of the killer.
 * @param userId The ID of the victim.
 * @param reason A reason. (if any)
 * @param banned If true, the client was temporarily banned.
 */
void QWServerCore::kickUser(const int id, const int userId, const QString reason, const bool banned) {
	qDebug() << "[core] kicking user"<<userId<<"by"<<id<<"banned:"<<banned<<", for reason:"<<reason;

	// Check if the victim can be kicked at all.
	if(pClients[userId]->sessionUser().privCannotBeKicked) {
		pClients[id]->sendErrorCannotBeDisconnected();
		return;
	}
	
	// Check for private chats
	QHashIterator<int,QWClassPrivateChat > j(pPrivateChats);
	while(j.hasNext()) { j.next();
		if(j.value().pUsers.contains(id)) {
			qDebug() << "[core] removed user"<<id<<"from private chat"<<j.key();
			removeFromPrivateChat(userId, j.key());
		}
	}
	
	// Notify everyone else
	QHashIterator<int,QPointer<WiredSocket> > i(pClients);
	while(i.hasNext()) { i.next();
		if(i.value()) i.value()->sendClientKicked(id, userId, reason, banned);
	}

	// FOR GREAT JUSTICE!
	if(pClients.contains(userId)) pClients[userId]->disconnectClient();
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
	pClients[id]->sendBanner(pBannerData);
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
	tmpChat.pChatId = tmpId;
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
	pClients[id]->sendChatTopic(pPrivateChats[chatId]);
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

/**
 * The client sent some information about itself.
 * @param id The ID of the client.
 * @param info The client-version string of the client.
 */
void QWServerCore::setClientInfo(const int id, const QString info) {
	qDebug() << "[core] setting client info for client"<<id;
	pClients[id]->setClientInfo(info);
}

/**
 * A user has changed the topic of a chat.
 * @param user The ID of the user.
 * @param chatId The ID of the chat.
 * @param topic The new chat topic.
 */
void QWServerCore::broadcastChatTopic(const ClassWiredUser user, const int chatId, const QString topic) {
	qDebug() << "[core] setting chat topic of chat"<<chatId<<"by user"<<user.pUserID;
	QWClassPrivateChat *chat = 0;
	if(chatId==1) {
		chat = &pPublicChat;
	} else if(chatId>0 && pPrivateChats.contains(chatId)) {
		chat = &pPrivateChats[chatId];
	}
	if(!chat) return;
	chat->pTopic = topic;
	chat->pTopicSetter = user;
	chat->pTopicDate = QDateTime::currentDateTime();
	if(chatId!=1) { // Private chat
		QWClassPrivateChat &tmpUsers = pPrivateChats[chatId];
		QListIterator<int> i(tmpUsers.pUsers);
		while(i.hasNext()) {
			pClients[i.next()]->sendChatTopic(*chat);
		}
	} else { // Public chat
		QHashIterator<int,QPointer<WiredSocket> > i(pClients);
		while(i.hasNext()) { i.next();
			if(i.value()) i.value()->sendChatTopic(*chat);
		}
	}
}

/**
 * A user has posted some news. Add the news to the database.
 * @param id The ID of the user who posted the message.
 * @param news The text of the news item.
 */
void QWServerCore::postNews(const int id, const QString news) {
	qDebug() << "[core] broadcasting new news-post from user"<<id;
	QString tmpNick = QString("%1 [%2]").arg(pClients[id]->sessionUser().pNick).arg(pClients[id]->sessionUser().pLogin);
	QSqlQuery query;
	query.prepare("INSERT INTO qw_news (message, date, user, deleted) VALUES (:message, :date, :user, 0);");
	query.bindValue(":message", news);
	query.bindValue(":date", QDateTime::currentDateTime().toUTC().toString(Qt::ISODate));
	query.bindValue(":user", tmpNick);
	if(query.exec()) {
		QHashIterator<int,QPointer<WiredSocket> > i(pClients);
		while(i.hasNext()) { i.next();
			if(i.value()) i.value()->sendNewsPosted(tmpNick, news);
		}
	} else {
		qDebug() << "SQL Error:"<<query.lastError().text();
	}
}

/**
 * A client requested the latest news.query.
 * @param id The ID of the user.
 */
void QWServerCore::sendNews(const int id) {
	qDebug() << "[core] sending news to"<<id;
	WiredSocket *socket = pClients[id];
	QSqlQuery query("SELECT user,date,message FROM qw_news WHERE deleted=0 ORDER BY id DESC LIMIT 100");
	//query.last();
	while(query.next()) {
		qDebug() << "[core] sending item";
		socket->sendNews( query.value(0).toString(), query.value(1).toDateTime(), query.value(2).toString() );
	}
	socket->sendNewsDone();
}

/**
 * Remove all items from the news.
 * @param id The ID of the user who clears the news.
 */
void QWServerCore::clearNews(const int id) {
	qDebug() << "[core] cleared all news by user"<<id;
	QSqlQuery query("UPDATE qw_news SET deleted=1;");
}

/**
 * Send the list of accounts to a user.
 * @param id The ID of the user who requsted the list of accounts.
 */
void QWServerCore::sendAccountsList(const int id) {
	WiredSocket *socket = pClients[id];
	QSqlQuery query("SELECT login FROM qw_accounts;");
	qDebug() << "[core] sending accounts list";
	while(query.next()) {
		qDebug() << "[core] sending accounts list item";
		socket->sendAccountListing(query.value(0).toString());
	}
	qDebug() << "[core] sending accounts done";
	socket->sendAccountListingDone();
}


/**
 * Send the list of groups to a user.
 * @param id The ID of the user who requsted the list of accounts.
 */
void QWServerCore::sendGroupsList(const int id) {
	WiredSocket *socket = pClients[id];
	QSqlQuery query("SELECT groupname FROM qw_groups;");
	qDebug() << "[core] sending groups list";
	while(query.next()) {
		qDebug() << "[core] sending groups list item";
		socket->sendGroupListing(query.value(0).toString());
	}
	qDebug() << "[core] sending groups list done";
	socket->sendGroupListingDone();
}

/**
 * The user requsted a user account specification.
 * @param id The ID of the requesting user.
 * @param name The name of the requsted account.
 */
void QWServerCore::sendUserSpec(const int id, const QString name) {
	QSqlQuery query;
	query.prepare("SELECT password,groupname,privileges FROM qw_accounts WHERE login=:login;");
	query.bindValue(":login", name);
	query.exec(); query.first();
	if(query.isValid()) {
		ClassWiredUser tmpUser;
		tmpUser.pLogin = name;
		tmpUser.pPassword = query.value(0).toString();
		tmpUser.pGroupName = query.value(1).toString();
		tmpUser.setPrivilegesFromAccount(query.value(2).toString());
		pClients[id]->sendUserSpec(tmpUser);
	} else {
		pClients[id]->sendErrorAccountNotFound();
	}
}

/**
 * The user requsted a group specification.
 * @param id The ID of the requesting user.
 * @param name The name of the requsted group.
 */
void QWServerCore::sendGroupSpec(const int id, const QString name) {
	QSqlQuery query;
	query.prepare("SELECT privileges FROM qw_groups WHERE groupname=:name;");
	query.bindValue(":name", name);
	query.exec(); query.first();
	if(query.isValid()) {
		ClassWiredUser tmpUser;
		tmpUser.pGroupName = name;
		tmpUser.setPrivilegesFromAccount(query.value(0).toString());
		pClients[id]->sendGroupSpec(tmpUser);
	} else {
		pClients[id]->sendErrorAccountNotFound();
	}
}

/**
 * Create a user account in the database.
 * @param id The id of the creating user.
 * @param user The object containing the account data.
 */
void QWServerCore::createUser(const int id, const ClassWiredUser user) {
	QSqlQuery query;
	query.prepare("SELECT * FROM qw_accounts WHERE login=:login;");
	query.bindValue(":login", user.pLogin);
	query.exec(); query.first();
	if(query.isValid()) {
		// Account already exists
		pClients[id]->sendErrorAccountExists();
		return;
	} else {
		query.prepare("INSERT INTO qw_accounts (login,password,groupname,privileges) VALUES (:login, :password, :groupname, :privileges);");
		query.bindValue(":login", user.pLogin);
		query.bindValue(":password", user.pPassword);
		query.bindValue(":groupname", user.pGroupName);
		query.bindValue(":privileges", user.privilegesFlags().replace(0x1C,":"));
		if(!query.exec()) qDebug() << "SQL error:"<<query.lastError().text();
	}
}

/**
 * Create a group in the database.
 * @param id The id of the creating user.
 * @param user The object containing the account data.
 */
void QWServerCore::createGroup(const int id, const ClassWiredUser group) {
	QSqlQuery query;
	query.prepare("SELECT * FROM qw_groups WHERE groupname=:name;");
	query.bindValue(":name", group.pGroupName);
	query.exec(); query.first();
	if(query.isValid()) {
		// Group already exists
		pClients[id]->sendErrorAccountExists();
		return;
	} else {
		query.prepare("INSERT INTO qw_groups (groupname,privileges) VALUES (:groupname, :privileges);");
		query.bindValue(":groupname", group.pGroupName);
		query.bindValue(":privileges", group.privilegesFlags().replace(0x1C,":"));
		if(!query.exec()) qDebug() << "SQL error:"<<query.lastError().text();
	}
}


/**
 * Create a user account in the database.
 * @param id The id of the creating user.
 * @param user The object containing the account data.
 */
void QWServerCore::editUser(const int id, const ClassWiredUser user) {
	QSqlQuery query;
	query.prepare("SELECT * FROM qw_accounts WHERE login=:login;");
	query.bindValue(":login", user.pLogin);
	query.exec(); query.first();
	if(!query.isValid()) {
		// Account does not exist
		pClients[id]->sendErrorAccountNotFound();
		return;
	} else {
		query.prepare("UPDATE qw_accounts SET password=:password,groupname=:groupname,privileges=:privileges WHERE login=:login;");
		query.bindValue(":login", user.pLogin);
		query.bindValue(":password", user.pPassword);
		query.bindValue(":groupname", user.pGroupName);
		query.bindValue(":privileges", user.privilegesFlags().replace(0x1C,":"));
		if(!query.exec()) qDebug() << "SQL error:"<<query.lastError().text();

		// Update connected users
		QHashIterator<int,QPointer<WiredSocket> > i(pClients);
		while(i.hasNext()) { i.next();
			WiredSocket *client = i.value();
			if(client && client->sessionUser().pLogin==user.pLogin) {
				client->pSessionUser.setPrivilegesFromAccount(user.privilegesFlags().replace(0x1C,":"));
				broadcastUserStatusChanged(client->sessionUser());
			}
		}
		
	}
}

/**
 * Create a group in the database.
 * @param id The id of the creating user.
 * @param user The object containing the account data.
 */
void QWServerCore::editGroup(const int id, const ClassWiredUser group) {
	QSqlQuery query;
	query.prepare("SELECT * FROM qw_groups WHERE groupname=:name;");
	query.bindValue(":name", group.pGroupName);
	query.exec(); query.first();
	if(!query.isValid()) {
		// Group already exists
		pClients[id]->sendErrorAccountNotFound();
		return;
	} else {
		query.prepare("UPDATE qw_groups SET privileges=:privileges WHERE groupname=:name;");
		query.bindValue(":name", group.pGroupName);
		query.bindValue(":privileges", group.privilegesFlags().replace(0x1C,":"));
		if(!query.exec()) qDebug() << "SQL error:"<<query.lastError().text();

		// Update connected users
		QHashIterator<int,QPointer<WiredSocket> > i(pClients);
		while(i.hasNext()) { i.next();
			WiredSocket *client = i.value();
			if(client && client->sessionUser().pGroupName==group.pGroupName) {
				client->pSessionUser.setPrivilegesFromAccount(group.privilegesFlags().replace(0x1C,":"));
				broadcastUserStatusChanged(client->sessionUser());
			}
		}
		
	}
}

/**
 * Delete a user account from the database.
 * @param id The ID of the user who deleted the account.
 * @param name The name of the account to be deleted.
 */
void QWServerCore::deleteUser(const int id, const QString name) {
	QSqlQuery query;
	query.prepare("DELETE FROM qw_accounts WHERE login=:login;");
	query.bindValue(":login",name);
	if(!query.exec()) qDebug() << "SQL error:"<<query.lastError().text();
}

/**
 * Delete a group from the database.
 * @param id The ID of the user who deleted the group.
 * @param name The name of the group.
 */
void QWServerCore::deleteGroup(const int id, const QString name) {
	QSqlQuery query;
	query.prepare("DELETE FROM qw_groups WHERE groupname=:name;");
	query.bindValue(":name",name);
	if(!query.exec()) qDebug() << "SQL error:"<<query.lastError().text();
}

/**
 * Send a file info/stat to the client.
 * @param id The ID of the requesting user.
 * @param path The path of the requested file.
 */
void QWServerCore::sendFileStat(const int id, const QString path) {
	
}

/**
 * Send a list of files to the requesting client.
 * @param id The ID of the requesting user.
 * @param path The path of the requested directory.
 */
void QWServerCore::sendFileList(const int id, const QString path) {
	WiredSocket *socket = pClients[id];
	QDir tmpDir(QString("%1/%2").arg(pFilesRoot).arg(path));
	if(tmpDir.canonicalPath().startsWith(pFilesRoot)) {
		QDirIterator it(tmpDir);
		while(it.hasNext()) {
			QString tmpPath = it.next();
			ClassWiredFile file;
			QFileInfo tmpFile(tmpPath);
			file.path = QDir::cleanPath(QString("%1/%2").arg(path).arg(tmpFile.fileName()));
			if(tmpFile.isDir()) {
				QDir tmpDir(tmpPath);
				file.size = tmpDir.count();
				file.type = 1;
			} else {
				file.size = tmpFile.size();
				file.type = 0;
			}
			file.created = tmpFile.created();
			file.modified = tmpFile.lastModified();
			file.checksum = "";
			file.comment = "";
			if(!tmpFile.isHidden())	socket->sendFileListing(file);
			it.next();
		}
		socket->sendFileListingDone(path,0);
		
	} else {
		// Attempt to get over the fence?
		socket->sendErrorFileNotFound();
		return;
	}
}







