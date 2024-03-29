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
#ifndef QWSERVERCORE_H
#define QWSERVERCORE_H

#include <QObject>
#include <QtSql>
#include <QHash>
#include "wiredsocket.h"
#include "qwclassprivatechat.h"
#include "classwiredfile.h"

/**
	@author Bastian Bense <bb@bense.de>
*/

class QWServerCore : public QObject
{
Q_OBJECT
public:
    QWServerCore(QObject *parent = 0);
    ~QWServerCore();
	QDateTime pStartTime;
	QHash<int,QPointer<WiredSocket> > pClients;
	int getUniqueUserId();
	int getUniqueChatId();
	

	QWClassPrivateChat pPublicChat;
	QHash<int, QWClassPrivateChat > pPrivateChats;
	QSqlDatabase pDatabase;

	QByteArray pBannerData;
	QString pFilesRoot;

	
private slots:
	void clearNews(const int id);
	void createPrivateChat(const int id);
	void createUser(const int id, const ClassWiredUser user);
	void createGroup(const int id, const ClassWiredUser group);
	void editUser(const int id, const ClassWiredUser user);
	void editGroup(const int id, const ClassWiredUser group);
	void deleteUser(const int id, const QString name);
	void deleteGroup(const int id, const QString name);
	void checkLogin(const int id, const QString login, const QString password);
	void declinePrivateChat(const int id, const int chatId);
	void broadcastBroadcast(const int id, const QString text);
	void broadcastChat(const int id, const int chatId, const QString text, const bool emote);
	void broadcastChatTopic(const ClassWiredUser user, const int chatId, const QString topic);
	void broadcastUserImageChanged(const ClassWiredUser);
	void broadcastUserStatusChanged(const ClassWiredUser);
	void deliverPrivateMessage(const int id, const int targetId, const QString text);
	void inviteUserToChat(const int id, const int userId, const int chatId);
	void joinPrivateChat(const int id, const int chatId);
	void kickUser(const int id, const int userId, const QString reason, const bool banned);
	void postNews(const int id, const QString news);
	void sendNews(const int id);
	void removeClient(const int id);
	void removeFromPrivateChat(const int userId, const int chatId);
	void setClientInfo(const int id, const QString info);
	void sendAccountsList(const int id);
	void sendGroupsList(const int id);
	void sendUserSpec(const int id, const QString name);
	void sendFileList(const int id, const QString path);
	void sendFileStat(const int id, const QString path);
	void sendGroupSpec(const int id, const QString name);
	void sendUserInfo(const int id, const int userId);
	void sendUserlist(const int id, const int chatId);
	void sendServerBanner(const int id);
	void sendServerInfo(const int id);
	
public slots:
	void registerClient(WiredSocket* socket);

};

#endif
