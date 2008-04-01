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

void QWServerCore::registerClient(WiredSocket *socket) {
	if(!socket) return;
	int tmpId = getUniqueUserId();
	socket->setUserId(tmpId);
	pClients[tmpId] = socket;
	connect(socket, SIGNAL(loginReceived(int, QString, QString)), this, SLOT(checkLogin(int, QString, QString)));
	connect(socket, SIGNAL(requestedUserlist(int,int)), this, SLOT(sendUserlist(int,int)));
	connect(socket, SIGNAL(receivedChat(int,int,QString)), this, SLOT(broadcastChat(int,int,QString)));
	connect(socket, SIGNAL(clientDisconnected(int)), this, SLOT(removeClient(int)) );
	connect(socket, SIGNAL(userStatusChanged(ClassWiredUser)), this, SLOT(broadcastUserStatusChanged(ClassWiredUser)) );
	qDebug() << "[core] registered client with id"<<socket->userId();
}

/**
 * A client requested the list of users for a particular chat.
 * @param id The ID of the user.
 * @param chatId The ID of the chat. 1 is public chat.
 */
void QWServerCore::sendUserlist(const int id, const int chatId) {
	qDebug() << "[core] sending user list to client"<<id<<"of chat"<<chatId;
	WiredSocket *tmpSock = pClients[id];
	QHashIterator<int,QPointer<WiredSocket> > i(pClients);
	while(i.hasNext()) { i.next();
		if(i.value()) tmpSock->sendUserlistItem(chatId, i.value()->sessionUser());
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
	socket->acceptLogin();
	//socket->rejectLogin();

	QHashIterator<int,QPointer<WiredSocket> > i(pClients);
	while(i.hasNext()) { i.next();
		if(i.value()) i.value()->sendClientJoin(1, socket->sessionUser() );
	}
}

/**
 * Send a line of chat to all clients on the server (if chatId is 1) or to
 * participants of a private chat (identified by chatId>0).
 * @param id The ID of the originating user.
 * @param chatId The ID of the target chat. 1 is the public chat.
 * @param text The text of the chat line.
 */
void QWServerCore::broadcastChat(const int id, const int chatId, const QString text) {
	qDebug() << "[core] broadcasting chat from"<<id<<"to chat"<<chatId;
	QHashIterator<int,QPointer<WiredSocket> > i(pClients);
	while(i.hasNext()) { i.next();
		if(i.value()) i.value()->sendChat(chatId, id, text);
	}
}

/**
 * Remove a client from the list of clients (after error or disconnection) and send all
 * clients a client-left message.
 * @param id The ID of the user that left.
 */
void QWServerCore::removeClient(const int id) {
	qDebug() << "[core] broadcasting user-left from"<<id;
	QHashIterator<int,QPointer<WiredSocket> > i(pClients);
	while(i.hasNext()) { i.next();
		if(i.value()) i.value()->sendClientLeave(1, id);
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


