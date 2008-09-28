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

		// File Transfers
		bool pIndexingFiles; // true if the client is indexing server fails
		QList<ClassWiredFile> pRecursiveFileListing;
		QString pRecursivePath;
		
		bool isTransferringFileOfType(WiredTransfer::TransferType type);

		void proceedFolderDownload(WiredTransferSocket *);
		
		
	public slots:

		void disconnectSocketFromServer();
		
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
		void getFile(const QString thePath, const QString theLocalPath, const bool queueLocally);
		void getFolder(const QString &remotePath, const QString &localPath, const bool &queueLocally);
		void getFileList(QString thePath);
		void getFileListRecusive(const QString &path);
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
		void putFile(const QString theLocalPath, const QString theRemotePath, const bool queueLocally);
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

		void runTransferQueue(WiredTransfer::TransferType type);


	private slots:
		QList<QByteArray> GetFieldArray(QByteArray theData);
		void do_handle_wiredmessage(QByteArray);
		void do_request_user_list(int theChannel);
		void do_send_user_login();
		void on_socket_encrypted();
		void on_socket_readyRead();
		void on_socket_sslErrors(const QList<QSslError> &errors);
		void on_server_filelist_item(QList<QByteArray>);
		void on_server_filelist_done(QList<QByteArray>);
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
		void on_socket_error(QAbstractSocket::SocketError error);
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

		// Transfers
		//
		void fileTransferFileDone(const ClassWiredTransfer);
		
	signals:
		void onSocketError(QAbstractSocket::SocketError);
		void onServerInformation();
		
		void onServerLoginSuccessful();
		void onServerBanner(const QPixmap);
		
		void onServerUserlistItem(int theChatID, const ClassWiredUser);
		void onServerUserlistDone(int theChatID);
		void onServerUserChanged(const ClassWiredUser theOld, const ClassWiredUser theNew);
		void onServerUserJoined(int theChatID, const ClassWiredUser theUser);
		void onServerUserLeft(int theChatID, const ClassWiredUser theUser);
		void onServerUserKicked(ClassWiredUser theVictim, ClassWiredUser theKiller, QString theReason);
		void onServerUserBanned(ClassWiredUser theVictim, ClassWiredUser theKiller, QString theReason);
		void onServerUserInfo(ClassWiredUser theUser);
		
		void onServerChat(int theChatID, int theUserID, QString theText, bool theIsAction);
		void onChatTopic(int theChatID, QString theNick, QString theLogin, QHostAddress theIP, QDateTime theDateTime, QString theTopic);
		void onPrivateMessage(ClassWiredUser theUser, QString theMessage);
		void onServerBroadcast(ClassWiredUser theUser, QString theMessage);
		
		void onServerPrivateChatCreated(int theChatID);
		void onServerPrivateChatInvitation(int theChatID, ClassWiredUser theUser);
		
		void onServerNews(QString theNick, QString theTime, QString thePost);
		void onServerNewsPosted(QString theNick, QString theTime, QString thePost);
		void onServerNewsDone();


		void fileSearchDone(QList<ClassWiredFile> theResults);
		void onFilesListItem(ClassWiredFile file);
		void onFilesListDone(QString thePath, qlonglong theFreeSpace);
		void onFilesListRecursiveDone(const QList<ClassWiredFile>);
		void onServerFileTransferReady(ClassWiredTransfer theTransfer);
		void onServerFileTransferQueued(ClassWiredTransfer theTransfer);
		void onServerFileInfo(ClassWiredFile theFile);

		void fileTransferDone(const ClassWiredTransfer theTransfer);
		void fileTransferStarted(const ClassWiredTransfer theTransfer);
		void fileTransferError(const ClassWiredTransfer theTransfer);
		void fileTransferStatus(const ClassWiredTransfer theTransfer);

		void groupsListingDone(QStringList theGroups);
		void usersListingDone(QStringList theAccounts);

		/// Received the details of a group.
		void userSpecReceived(ClassWiredUser);

		void groupSpecReceived(ClassWiredUser);
		void errorBanned();
		void errorLoginFailed();
		void errorPermissionDenied();
		void errorFileNotFound();
		void errorClientNotFound();
		void errorQueueLimitExceeded();
		void errorCommandFailed();
		void errorCommandNotRecognized();
		void errorCommandNotImplemented();
		void errorCommandSyntaxError();
		void errorAccountNotFound();
		void errorAccountExists();
		void errorCanNotBeDisconnected();
		void errorFileExists();
		void errorChecksumMismatch();
		void errorOccoured(int theError);
		void userPrivileges(const ClassWiredUser theSession);
		void fileTransferSocketError(QAbstractSocket::SocketError);
		
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
