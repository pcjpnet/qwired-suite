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
#include "classwiredfile.h"
#include "qwclassprivatechat.h"
#include <QtCore>
#include <QtNetwork>

#include "qwtransaction.h"


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
		ClassWiredUser pSessionUser;
		QTimer* pIdleTimer;

		bool bufferHasMessage(QByteArray &buffer);
		void handleTransaction(const QWTransaction & t);
		void sendTransaction(const QWTransaction &t);
		
		
	signals:
		void conferenceOptionsChanged(const int id, const QWTransaction &t);
		void newsGroupsRequested(const int id, const QWTransaction &t);
		void privateMessageReceived(const int id, const QWTransaction &t);
		void receivedChat(const int id, const QWTransaction &t);
		void requestedMotd(const int id, const QWTransaction &t);
		void requestedBanner(const int id, const QWTransaction &t);
		void requestedUserlist(const int id, const QWTransaction &t);
		void transactionReceived(const int id, const QWTransaction &t);
		void userInfoRequested(const int id, const QWTransaction &t);
		
		/**
		 * The client has sent the login and password and the server should now check if these are correct.
		 * Call sendLoginSuccessful() or sendErrorLoginFailed() to accept or reject the connection.
		 * @param id The ID of the connection.
		 * @param login The login name.
		 * @param password The password.
		 */
		void clearedNews(const int id);
		void clientDisconnected(const int id);
		void createdUser(const int id, const ClassWiredUser user);
		void createdGroup(const int id, const ClassWiredUser group);
		void editedUser(const int id, const ClassWiredUser user);
		void editedGroup(const int id, const ClassWiredUser group);
		void deletedUser(const int id, const QString name);
		void deletedGroup(const int id, const QString name);
		void declinedPrivateChat(const int id, const int chatId);
		void handshakeComplete(const int id);
		void invitedUserToChat(const int id, const int userId, const int chatId);
		void joinedPrivateChat(const int id, const int chatId);
		void loginReceived(const int id, const QWTransaction &t);
		void newsPosted(const int id, const QString news);
		void privateChatLeft(const int id, const int chatId);
		void receivedClientInfo(const int id, const QString info);
		
		void receivedBroadcastMessage(const int id, const QString text);
		void requestedAccountsList(const int id);
		
		void requestedFileList(const int id, const QString path);
		void requestedFileStat(const int id, const QString path);
		void requestedGroupsList(const int id);
		void requestedNews(const int id);
		void requestedPrivateChat(const int id);
		void requestedReadUser(const int id, const QString name);
		void requestedReadGroup(const int id, const QString name);
		void requestedUserInfo(const int id, const int userId);
		
		
		void userImageChanged(const ClassWiredUser user);
		void userKicked(const int id, const int userId, const QString reason, const bool banned);
		void userStatusChanged(const ClassWiredUser user, const int changeFlags);
		
		
		
	public slots:
		void disconnectClient();
		void sendErrorLoginFailed();
		
		void sendServerInformation(const QString serverName, const QString serverDescr, const QDateTime startTime, const int fileCount, const int fileTotalSize);
		void setClientInfo(const QString info);
		void setUserId(int userId);

		void sendAccountListing(const QString name);
		void sendAccountListingDone();
		void sendGroupListing(const QString name);
		void sendGroupListingDone();
		void sendUserSpec(const ClassWiredUser account);
		void sendGroupSpec(const ClassWiredUser group);

		void sendFileListing(const ClassWiredFile file);
		void sendFileListingDone(const QString path, const int free);
		void sendFileStat(const ClassWiredFile file);
		
		void sendBroadcastMessage(const int userId, const QString text);
		void sendChat(const int chatId, const int userId, const QString text, const bool emote);
		void sendChatTopic(const QWClassPrivateChat chat);
		void sendClientKicked(const int killerId, const int victimId, const QString reason, const bool banned);
		void sendClientDeclinedChat(const int chatId, const int userId);
		void sendClientJoin(const int chatId, const ClassWiredUser user);
		void sendClientLeave(const int chatId, const int id);
		void sendInviteToChat(const int chatId, const int id);
		void sendNews(const QString nickname, const QDateTime date, const QString news);
		void sendNewsDone();
		void sendNewsPosted(const QString nickname, const QString news);
		void sendMessage(const int userId, const QString text);
		void sendPrivateChatCreated(const int chatId);
		void sendPrivileges();

		
		void sendUserImageChanged(const ClassWiredUser user);
		void sendUserInfo(const ClassWiredUser user);
		
		void idleTimerTriggered();

		void sendErrorAccountNotFound();
		void sendErrorAccountExists();
		void sendErrorPermissionDenied();
		void sendErrorClientNotFound();
		void sendErrorCommandFailed();
		void sendErrorSyntaxError();
		void sendErrorCannotBeDisconnected();
		void sendErrorFileNotFound();


		
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

		
		bool pHandshakeOK;
		
		void resetIdleTimer();
		
		/// Our function that formats a Wired message and sends it to the server.
		void sendWiredCommand(const QByteArray);
		void sendWiredCommandBuffer(const QByteArray);
		
		
		/// This is our TCP buffer. Could possibly be optimized, but works for now.
		QByteArray pBuffer;

		/// This is the send TCP buffer. It is used when sending lists and to prevent the event loop
		/// from fireing command handling during list sending.
		QByteArray pSendBuffer;
		
		QPointer<QSslSocket> pSocket;
		QList<QByteArray> splitMessageFields(QByteArray theData);
		
		
    
};

#endif
