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

#include <QtGui>
#include <QtCore>
#include <QSslSocket>

#include "classwireduser.h"
#include "classwiredtransfer.h"
#include "classwiredfile.h"
#include "classtrackerserver.h"
#include "wiredtransfersocket.h"

/**
	@author Bastian Bense <bastibense@gmail.com>
 */

namespace Wired {
	enum SocketType { WiredSocket, TrackerSocket };
}

const int kEOF = 0x04;
const int kFS = 0x1C;

class WiredSocket : public QObject
{
	Q_OBJECT
						
	public:

		// Server Information
		QString		pServerAppVersion;
		QString		pServerProtoVersion;
		QString		pServerName;
		QString		pServerDescription;
		QDateTime	pServerStartTime;
		QPixmap		pServerBanner;
		int			pServerFileCount;
		int			pServerFileSize;
		QString		pClientName;
		QString		pClientVersion;
		
		Wired::SocketType pSocketType;

		/// The class that contains most of the user-specific information for this
		/// session. We use this to keep the clutter of the class to a minimum.
		ClassWiredUser sessionUser;

		/// This contains all users registered for the public user list (1) and 
		/// all private chats. It automatically maintained and changes are
		/// signalled.
		QHash<int, QList<ClassWiredUser> > pUsers;
	
		// User Information
		void setUserNick(QString);
		void setUserAccount(QString, QString);
		ClassWiredUser getUserByID(int);

		// No further comment on those, and, no, you can not has cheezeburger.
		bool pIzCaturday;
		QString tranzlate(QString);
				
		const int userCountByChat(const int theChatID);
		const int userIndexByID(const int theID, const int theChat=1);
		const ClassWiredUser userByIndex(const int theChatID, const int theIndex);
		
		void connectToWiredServer(QString theHostName, int thePort=2000);
		
		WiredSocket(QObject *parent = 0);
		~WiredSocket();
		QPointer<QSslSocket> pSocket;
		QList<QPointer<WiredTransferSocket> > pTransferSockets;

		// Tracker subsystem
		//
		void connectToTracker(QString theHostName, int thePort=2002);
		
		
	public slots:

		// Wired Subsystem (not for Trackers)
		//
		void banClient(int theUserID, QString theReason);
		void cancelTransfer(ClassWiredTransfer);
		void clearNews();
		void createChatWithClient(int theUserID);
		void createFolder(const QString thePath);
		void createGroup(ClassWiredUser);
		void createUser(ClassWiredUser);
		void deleteFile(const QString thePath);
		void deleteGroup(QString theName);
		void deleteUser(QString theLogin);
		void disconnectFromServer();
		void editGroup(ClassWiredUser);
		void editUser(ClassWiredUser);
		void getClientInfo(int theUserID);
		void getFile(const QString thePath, const QString theLocalPath);
		void getFileList(QString thePath);
		void getGroups();
		void getNews();
		void getPrivileges();
		void getServerBanner();
		void getUsers();
		void inviteClientToChat(int theChatID, int theUserID);
		void joinChat(int theChatID);
		void kickClient(int theUserID, QString theReason);
		void leaveChat(int theChatID);
		void moveFile(const QString thePath, const QString theDestination);
		void postNews(QString thePost);
		void putFile(const QString theLocalPath, const QString theRemotePath);
		void readGroup(QString theName);
		void readUser(QString theName);
		void rejectChat(int theChatID);
		void searchFiles(const QString theSearch);
		void sendChat(int theChatID, QString theText, bool theIsAction);
		void sendPrivateMessage(int theUserID, QString theMessage);
		void setCaturday(bool);
		void setChatTopic(int theChatID, QString theTopic);
		void setUserIcon(QPixmap theIcon);
		void setUserStatus(QString theStatus);
		void statFile(const QString thePath);


	private slots:
		QList<QByteArray> GetFieldArray(QByteArray theData);
		void do_handle_wiredmessage(QByteArray);
		void do_request_user_list(int theChannel);
		void do_send_user_login();
		void on_socket_encrypted();
		void on_socket_readyRead();
		void on_socket_sslErrors(const QList<QSslError> &errors);
		void on_server_userlist_item(QList<QByteArray>);
		void on_server_userlist_changed(QList<QByteArray>);
		void on_server_userlist_imagechanged(QList<QByteArray>);
		void on_server_userlist_joined(QList<QByteArray>);
		void on_server_userlist_left(QList<QByteArray>);
		void on_server_userlist_kicked(QList<QByteArray>);
		void on_server_userlist_banned(QList<QByteArray>);
		void on_server_userinfo(QList<QByteArray> theParams);
		void on_server_new_chat_created(QList<QByteArray> theParams);
		void on_server_banner(QList<QByteArray> theParams);
		void on_server_broadcast(QList<QByteArray> theParams);
		void on_server_transfer_ready(QList<QByteArray> theParams);
		void on_server_transfer_queued(QList<QByteArray> theParams);
		void on_server_file_info(QList<QByteArray> theParams);
		void on_server_privileges(QList<QByteArray> theParams);
		void on_socket_error();
		void on_server_search_listing(QList<QByteArray> theParams);
		void on_server_search_done(QList<QByteArray> theParams);
		void on_server_groups_listing(QList<QByteArray> theParams);
		void on_server_groups_done();
		void on_server_users_listing(QList<QByteArray> theParams);
		void on_server_users_done();
		void on_server_user_spec(QList<QByteArray> theParams);
		void on_server_group_spec(QList<QByteArray> theParams);
		void cleanTransfers();

		// Tracker
		//
		void on_tracker_listing_item(QList<QByteArray> theParams);
		void on_tracker_listing_done();
		
	signals:
		void onSocketError(QString theErrorReason, int theError);
		void onServerInformation();
		
		void onServerLoginSuccessful();
		/// This signal is emitted if the server has sent a server banner.
		void onServerBanner(const QPixmap);
		/// This signal is emitted once of every registered user in the chat identified by theChatID.
		/// After all users have been transmitted, a onServerUserlistDone() signal is emitted.
		void onServerUserlistItem(int theChatID, const ClassWiredUser);
		/// This signal is emitted once all users of a chat have been transmitted.
		void onServerUserlistDone(int theChatID);
		/// This signal is emitted if the status of a user changes on the server.
		/// theOld contains the old user record, theNew contains the new one.
		void onServerUserChanged(const ClassWiredUser theOld, const ClassWiredUser theNew);
		/// This signal is emitted everytime a user joins a specific chat or the server.
		void onServerUserJoined(int theChatID, const ClassWiredUser theUser);
		/// This signal is emitted everytime a user leaves a specific chat or the server.
		void onServerUserLeft(int theChatID, const ClassWiredUser theUser);
		/// This signal is emitted everytime a user writes into the chat idenitified by
		/// theChatID.
		void onServerChat(int theChatID, int theUserID, QString theText, bool theIsAction);
		void onServerNews(QString theNick, QString theTime, QString thePost);
		/// This signal is emitted for each news item in the server's news list.
		/// A onServerNewsDone() signal will follow once all items have been transmitted.
		void onServerNewsPosted(QString theNick, QString theTime, QString thePost);
		/// This signal is emitted after the complete list of news items has been transmitted.
		void onServerNewsDone();

		/// This signal is emitted after receiving the topic of a chat from the server.
		/// This usually happens after a successful chatJoin command or after logging in.
		void onChatTopic(int theChatID, QString theNick, QString theLogin, QHostAddress theIP, QDateTime theDateTime, QString theTopic);
		/// This signal is emitted whenever a user sends a private message.
		void onPrivateMessage(ClassWiredUser theUser, QString theMessage);
		/// This signal is emitted if a user has been kicked off the server.
		void onServerUserKicked(ClassWiredUser theVictim, ClassWiredUser theKiller, QString theReason);
		/// This signal is emitted if a user has been banned from the server.
		void onServerUserBanned(ClassWiredUser theVictim, ClassWiredUser theKiller, QString theReason);
		
		/// This signal is emitted in response to the the Get User Info request. It provides a fully blown user
		/// object with all available user information.
		void onServerUserInfo(ClassWiredUser theUser);
		/// This signal is emitted if another user has invited this user to a private chat.
		/// Provided is the new chat ID and the reference of the inviting user.
		/// The client should respond with a doAcceptPrivateChat() or rejectChat()
		/// command. Another possibility is to ignore the event.
		void onServerPrivateChatInvitation(int theChatID, ClassWiredUser theUser);
		/// This signal is emitted after the server has successfully created a private chat for us.
		void onServerPrivateChatCreated(int theChatID);
		/// This signal is emitted everytime an administrator broadcasts a message to all users on
		/// the server.
		void onServerBroadcast(ClassWiredUser theUser, QString theMessage);
		/// This signal is emitted for each file in a directory after the LIST command.
		void onFilesListItem(ClassWiredFile file);
		/// This signal is emitted after all items of a directory have been listed.
		void onFilesListDone(QString thePath, qlonglong theFreeSpace);

		
		/// A file transfer is ready. In response to PUT and GET.
		void onServerFileTransferReady(ClassWiredTransfer theTransfer);
		/// A file transfer request has been queued or changed it's position in the queue.
		void onServerFileTransferQueued(ClassWiredTransfer theTransfer);
		/// A file information response. In response to STAT.
		void onServerFileInfo(ClassWiredFile theFile);
		
		/// A file transfer is done.
		void fileTransferDone(const ClassWiredTransfer theTransfer);
		/// A file transfer started.
		void fileTransferStarted(const ClassWiredTransfer theTransfer);
		/// A error occoured during a file transfer.
		void fileTransferError(const ClassWiredTransfer theTransfer);
		/// New information is available on a file transfer.
		void fileTransferStatus(const ClassWiredTransfer theTransfer);

		/// A search has completed and theResults contains the list of results.
		void fileSearchDone(QList<ClassWiredFile> theResults);

		/// All groups have been received from the server (admin mode) and can be displayed
		/// to the user.
		void groupsListingDone(QStringList theGroups);

		/// All account names have been received from the server (admin mode) and can be displayed
		/// to the user.
		void usersListingDone(QStringList theAccounts);

		/// Received the details of a group.
		void userSpecReceived(ClassWiredUser);

		/// Received the details of a user account.
		void groupSpecReceived(ClassWiredUser);

		
		/// Emitted after a failed attempt to log into the Wired server because the user was banned.
		void errorBanned();
		/// Login failed. The user provided the wrong name or password.
		void errorLoginFailed();
		/// This signal is emitted on 516 Permission Denied.
		void errorPermissionDenied();
		/// The specified file was not found on the server.
		void errorFileNotFound();
		/// This signal is emitted if a command has been sent to the server, but the target client is not
		/// connected to the server anymore.
		void errorClientNotFound();
		/// The server file transfer queue has been exceeded.
		void errorQueueLimitExceeded();
		/// Undefined internal error caused the last command to fail.
		void errorCommandFailed();
		/// The last command was unknown to the server.
		void errorCommandNotRecognized();
		/// The last command was known, but not implemented in the server. (Version mismatch?)
		void errorCommandNotImplemented();
		/// The command is known by the server, but it has the wrong syntax.
		void errorCommandSyntaxError();
		/// Last request for account specs failed, the account was not found.
		void errorAccountNotFound();
		/// The account could not be created because it already exists.
		void errorAccountExists();
		/// Can not be disconnected (Ban or Kick response)
		void errorCanNotBeDisconnected();
		/// Can not put file, the file already exists.
		void errorFileExists();
		/// The checksums of a file do not match.
		void errorChecksumMismatch();
		/// This signal is emitted whenever a protocol-level error occours (5xx messages) before the
		/// dedicated signal is fired (errorXY). A error number is provided.
		void errorOccoured(int theError);

		/// Emitted when the server sends the privileges of the current session.
		void userPrivileges(const ClassWiredUser theSession);

		// TRACKER
		//
		void trackerServersReceived(QList<ClassTrackerServer>);
		
	private:

		// Tracker subsystem
		//
		void tracker_request_servers();
		
		void sendClientInfo();

		// Buffers while receiving the list of groups and users (admin mode)
		QStringList pAdminGroups;
		QStringList pAdminUsers;

		QHash<QString,QString> pTranzlator;
		
		/// Buffer for the user id we have invited before. Unfortunately Wired has no
		/// transactions, so we have to hope that this works well enough for now.
		int pInvitedUserID;
		
		/// Our function that formats a Wired message and sends it to the server.
		void sendWiredCommand(const QByteArray);
		
		/// The temporary list of items for the search results.
		QList<ClassWiredFile> pSearchResults;
		QList<ClassTrackerServer> pTrackerServers;
		
		/// This is our TCP buffer. Could possibly be optimized, but works for now.
		QByteArray pBuffer;
		
		
    
};

#endif
