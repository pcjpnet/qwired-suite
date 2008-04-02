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

 
#ifndef WIREDSOCKET_H
#define WIREDSOCKET_H

#include "classwireduser.h"
#include <QtCore>
#include <QtNetwork>


/**
	@author Bastian Bense <bastibense@gmail.com>
 */

namespace Wired {

}

const int kEOF = 0x04;
const int kFS = 0x1C;

class WiredSocket : public QObject
{
	Q_OBJECT

	public:
		WiredSocket(QObject *parent = 0);
		~WiredSocket();
		void setWiredSocket(QSslSocket *socket);

		ClassWiredUser sessionUser() { return pSessionUser; };
		int userId() { return pSessionUser.pUserID; }
		bool pLoggedIn;
		bool isLoggedIn() { return pLoggedIn; };
		
		
	signals:
		/**
		 * The client has sent the login and password and the server should now check if these are correct.
		 * Call sendLoginSuccessful() or sendErrorLoginFailed() to accept or reject the connection.
		 * @param id The ID of the connection.
		 * @param login The login name.
		 * @param password The password.
		 */
		void clientDisconnected(const int id);
		void declinedPrivateChat(const int id, const int chatId);
		void handshakeComplete(const int id);
		void invitedUserToChat(const int id, const int userId, const int chatId);
		void joinedPrivateChat(const int id, const int chatId);
		void loginReceived(const int id, QString login, QString password);
		void privateChatLeft(const int id, const int chatId);
		void receivedChat(const int id, const int chatId, const QString text, const bool emote);
		void receivedBroadcastMessage(const int id, const QString text);
		void requestedBanner(const int id);
		void requestedPrivateChat(const int id);
		void requestedUserInfo(const int id, const int userId);
		void requestedUserlist(const int id, const int chat);
		void userImageChanged(const ClassWiredUser user);
		void userStatusChanged(const ClassWiredUser user);
		
		void privateMessageReceived(const int id, const int targetId, const QString text);
		
	public slots:
		void disconnectClient();
		void sendLoginSuccessful();
		void sendErrorLoginFailed();
		
		void sendServerInformation(const QString serverName, const QString serverDescr, const QDateTime startTime, const int fileCount, const int fileTotalSize);
		void setUserId(int userId);

		void sendBanner(const QByteArray banner);
		void sendBroadcastMessage(const int userId, const QString text);
		void sendChat(const int chatId, const int userId, const QString text, const bool emote);
		void sendClientDeclinedChat(const int chatId, const int userId);
		void sendClientJoin(const int chatId, const ClassWiredUser user);
		void sendClientLeave(const int chatId, const int id);
		void sendInviteToChat(const int chatId, const int id);
		void sendPrivateMessage(const int userId, const QString text);
		void sendPrivateChatCreated(const int chatId);
		void sendUserlistItem(const int chatId, const ClassWiredUser item);
		void sendUserlistDone(const int chatId);

		
		void sendUserImageChanged(const ClassWiredUser user);
		void sendUserInfo(const ClassWiredUser user);
		void sendUserStatusChanged(const ClassWiredUser user);
		
		
		void sendErrorPermissionDenied();
		void sendErrorClientNotFound();
		void sendErrorCommandFailed();
		void sendErrorSyntaxError();


		
	private slots:
		void readDataFromSocket();
		void on_socket_sslErrors(const QList<QSslError> & errors);
		void handleWiredMessage(QByteArray theData);
		void on_socket_error();


		// Protocol Stack
		void sendErrorCommandNotImplemented();

		
	private:
		// Session state parameters
		//


		QString pServerName;
		QString pServerDesc;
		
		ClassWiredUser pSessionUser;
		bool pHandshakeOK;
		
		/// Our function that formats a Wired message and sends it to the server.
		void sendWiredCommand(const QByteArray);

		/// This is our TCP buffer. Could possibly be optimized, but works for now.
		QByteArray pBuffer;
		QPointer<QSslSocket> pSocket;
		QList<QByteArray> splitMessageFields(QByteArray theData);
		
		
    
};

#endif
