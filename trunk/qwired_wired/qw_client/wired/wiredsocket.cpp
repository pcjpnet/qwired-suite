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
#include "../general/wiredcommon.h"

WiredSocket::WiredSocket(QObject *parent)
 : QObject(parent)
{
	// Initialize the socket
	pSocket = new QSslSocket(this);
	pSocket->setProtocol(QSsl::TlsV1);
	pClientName = "Qwired SVN";
	pClientVersion = QWIRED_VERSION;
	connect( pSocket, SIGNAL(encrypted()), this, SLOT(on_socket_encrypted()) );
	connect( pSocket, SIGNAL(readyRead()), this, SLOT(on_socket_readyRead()) );
	connect( pSocket, SIGNAL(sslErrors(QList<QSslError>)), this, SLOT(on_socket_sslErrors(QList<QSslError>))); 
	connect( pSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(on_socket_error()) );
	pInvitedUserID = 0;
	pIzCaturday = false; // :< no iz caturday?
}

// Called by the socket and indicates the an SSL error has occoured.
void WiredSocket::on_socket_sslErrors(const QList<QSslError> & errors) {
	qDebug() << "WiredSocket.on_socket_sslErrors(): "<<errors;
	pSocket->ignoreSslErrors();
}

// Disconnect from the server and clean up
void WiredSocket::disconnectFromServer() {
	pSocket->disconnectFromHost();
	pAdminGroups.clear();
	pAdminUsers.clear();
	pInvitedUserID = 0;
	pBuffer.clear();
	pUsers.clear();

	QMutableListIterator<QPointer<WiredTransferSocket> > i(pTransferSockets);
	while(i.hasNext()) {
		QPointer<WiredTransferSocket> p = i.next();
		p->deleteLater();
		i.remove();
	}
	
}

// Called by on_socket_readyRead() after splitting a message off the internal
// buffer. We have to extract the command ID here and decide what to do with
// the message.
void WiredSocket::do_handle_wiredmessage(QByteArray theData) {
	int tmpPos = theData.indexOf(" ");
	int tmpCmdID = 0;
	if( tmpPos>-1 ) tmpCmdID = theData.left(tmpPos).toInt();
		else tmpCmdID = theData.toInt();
		
	QList<QByteArray> tmpParams = GetFieldArray(theData);
	qDebug() << "Got command" << tmpCmdID;

	// If in tracker mode, we should block commands that are not included in the tracker protocol.
	if(pSocketType==Wired::TrackerSocket && !(tmpCmdID==200 || tmpCmdID==500 || tmpCmdID==501
		  || tmpCmdID==502 || tmpCmdID==503 || tmpCmdID==511 || tmpCmdID==516
	      || tmpCmdID==530 || tmpCmdID==700 || tmpCmdID==710 || tmpCmdID==711
		  || tmpCmdID==720 || tmpCmdID==721) )
		return;

	
	switch(tmpCmdID) {
		case 200:
			// Server Information
			pServerAppVersion = QString::fromUtf8(tmpParams.value(0));
			pServerProtoVersion = QString::fromUtf8(tmpParams.value(1));
			pServerName = QString::fromUtf8(tmpParams.value(2));
			pServerDescription = QString::fromUtf8(tmpParams.value(3));
			pServerStartTime = QDateTime::fromString( QString::fromUtf8(tmpParams.value(4)), Qt::ISODate );
			if(pServerProtoVersion=="1.1" && pSocketType==Wired::WiredSocket) {
				pServerFileCount = tmpParams.value(5).toInt();
				pServerFileSize = tmpParams.value(6).toLongLong();
			}
			emit onServerInformation();
			if(pSocketType==Wired::WiredSocket) { // Wired
				do_send_user_login();
			} else { // Tracker
				tracker_request_servers();
			}
			qDebug() << "WiredSocket: Session established with '"<<pServerName<<"'";
			break;

		case 201: // Login successful
			sessionUser.pUserID = tmpParams.value(0).toInt();
			qDebug() << "WiredSocket: Login successful with ID"<<sessionUser.pUserID;
			emit onServerLoginSuccessful();
			do_request_user_list(1);
			getServerBanner();
			getPrivileges();
			break;

		case 203: on_server_banner(tmpParams); break;
		case 300: emit onServerChat(tmpParams.value(0).toInt(), tmpParams.value(1).toInt(), QString::fromUtf8(tmpParams.value(2)), false); break;
		case 301: emit onServerChat(tmpParams.value(0).toInt(), tmpParams.value(1).toInt(), tmpParams.value(2), true); break;
		case 302: on_server_userlist_joined(tmpParams); break;
		case 303: on_server_userlist_left(tmpParams); break;
		case 304: on_server_userlist_changed(tmpParams); break;
		case 305: emit onPrivateMessage(getUserByID(tmpParams.value(0).toInt()), QString::fromUtf8(tmpParams.value(1)) ); break;
		case 306: on_server_userlist_kicked(tmpParams); break;
		case 307: on_server_userlist_banned(tmpParams); break;
		case 308: on_server_userinfo(tmpParams); break;
		case 309: on_server_broadcast(tmpParams); break;
		case 310: on_server_userlist_item(tmpParams); break;
		case 311: emit onServerUserlistDone(tmpParams.value(0,"").toInt()); break;

		case 320: // News Post
			emit onServerNews(QString::fromUtf8(tmpParams.value(0)),
					QString::fromAscii(tmpParams.value(1)),
					QString::fromUtf8(tmpParams.value(2)) );
			break;

		case 322: // News Posted
			emit onServerNewsPosted(QString::fromUtf8(tmpParams.value(0)),
						QString::fromAscii(tmpParams.value(1)),
						QString::fromUtf8(tmpParams.value(2)) );
			break;
		case 321: emit onServerNewsDone(); break;
		case 330: on_server_new_chat_created(tmpParams); break;
		case 331: emit onServerPrivateChatInvitation( tmpParams.value(0).toInt(), getUserByID(tmpParams.value(1).toInt()) ); break;
		case 340: on_server_userlist_imagechanged(tmpParams); break;

		case 341: // Chat Topic
			emit onChatTopic ( tmpParams.value(0).toInt(), // chat id
					QString::fromUtf8( tmpParams.value(1)), // nickname
					QString::fromUtf8( tmpParams.value(2)), // login
					QHostAddress( QString::fromAscii(tmpParams.value(3)) ), // ip
						QDateTime::fromString( tmpParams.value(4), Qt::ISODate ), // date-time
						QString::fromUtf8( tmpParams.value(5) ) ); // topic
						break;


		case 400: on_server_transfer_ready(tmpParams); break; // File transfer ready
		case 401: on_server_transfer_queued(tmpParams); break; // File transfer queued/status
		case 402: on_server_file_info(tmpParams); break; // File Information
		case 410: emit onFilesListItem( ClassWiredFile(tmpParams) ); break; // File listing item
		case 411: emit onFilesListDone( QString::fromUtf8( tmpParams.value(0) ), tmpParams.value(1).toLongLong() ); break;

		case 420: on_server_search_listing(tmpParams); break;
		case 421: on_server_search_done(tmpParams); break;

		case 500: emit errorOccoured(tmpCmdID); emit errorCommandFailed(); break;
		case 501: emit errorOccoured(tmpCmdID); emit errorCommandNotRecognized(); break;
		case 502: emit errorOccoured(tmpCmdID); emit errorCommandNotImplemented(); break;
		case 503: emit errorOccoured(tmpCmdID); emit errorCommandSyntaxError(); break;
		case 510: // Login failed
			pSocket->disconnectFromHost();
			emit errorOccoured(tmpCmdID);
			emit errorLoginFailed();
			break;
		case 511: // Banned
			pSocket->disconnectFromHost();
			emit errorOccoured(tmpCmdID);
			emit errorBanned();
			break;
		case 512: emit errorOccoured(tmpCmdID); emit errorClientNotFound(); break;
		case 513: emit errorOccoured(tmpCmdID); emit errorAccountNotFound(); break;
		case 514: emit errorOccoured(tmpCmdID); emit errorAccountExists(); break;
		case 515: emit errorOccoured(tmpCmdID); emit errorCanNotBeDisconnected(); break;
		case 516: emit errorOccoured(tmpCmdID); emit errorPermissionDenied(); break;
		case 520: emit errorOccoured(tmpCmdID); emit errorFileNotFound(); break;
		case 521: emit errorOccoured(tmpCmdID); emit errorFileExists(); break;
		case 522: emit errorOccoured(tmpCmdID); emit errorChecksumMismatch(); break;
		case 523: emit errorOccoured(tmpCmdID); emit errorQueueLimitExceeded(); break;

		case 600: on_server_user_spec(tmpParams); break;
		case 601: on_server_group_spec(tmpParams); break;
		case 602: on_server_privileges(tmpParams); break;
		case 610: on_server_users_listing(tmpParams); break;
		case 611: on_server_users_done();
		case 620: on_server_groups_listing(tmpParams); break;
		case 621: on_server_groups_done(); break;

		// Tracker protocol
		//
		case 720: on_tracker_listing_item(tmpParams); break;
		case 721: on_tracker_listing_done(); break;

		default: qDebug() << "WiredSocket: Unhandled command:"<<tmpCmdID; break;

	}
}

// Find a user by the ID specified in the list of publically connected users.
ClassWiredUser WiredSocket::getUserByID(int theID) {
	QList<ClassWiredUser> tmpList = pUsers[1]; // server user list
	QListIterator<ClassWiredUser> i(tmpList);
	while (i.hasNext()) {
		ClassWiredUser usr = i.next();
		if( usr.pUserID==theID )
			return usr;
	}
	
	ClassWiredUser wu; // Fallback object
	wu.pNick = tr("(invalid user)");
	return wu;
}


// Called whenever a type 310 message arrives.
// Process an incoming user list entry, append it to the list of chats.
void WiredSocket::on_server_userlist_item(QList<QByteArray> theParams) {
	int tmpChannel = theParams.value(0,"").toInt();
	ClassWiredUser tmpUsr(theParams);
	QList<ClassWiredUser> &tmpList = pUsers[tmpChannel];
	tmpList.append(tmpUsr);
	emit onServerUserlistItem(tmpChannel, tmpUsr);
}

// Set the username and password for the login sequence.
void WiredSocket::setUserAccount(QString theAcct, QString thePass) {
	sessionUser.pLogin = theAcct;
	sessionUser.pPassword = thePass;
}

// Do I really have to comment this? :D
void WiredSocket::setCaturday(bool b) {
	 // everyday iz caturday!  :3
	if(b) {
		pTranzlator.clear();
		setUserIcon( QPixmap(":/icons/icon_happycat.png") );
		setUserStatus(tr("kittehday nait fevrar"));
		QString tmpNick = sessionUser.pNick;
		tmpNick = tmpNick.replace("s","z");
		tmpNick = tmpNick.replace("e","3");
		tmpNick = tmpNick.replace("i","ie");
		tmpNick = tmpNick.replace("y","eh");
		setUserNick(tmpNick);
		QFile tmpF(":/general/tranzlator.txt");
		if(tmpF.exists() && tmpF.open(QIODevice::ReadOnly)) {
			while(!tmpF.atEnd()) {
				QString tmpL = QString::fromUtf8(tmpF.readLine()).trimmed();
				QString tmpKey = tmpL.section(": ", 0, 0);
				QString tmpVal = tmpL.section(": ", 1, 1);
				pTranzlator[tmpKey]=tmpVal;
			}
			tmpF.close();
			qDebug() << "Loaded lolspeak: "<<pTranzlator.count();
			pIzCaturday = true;
			return;
		}
	}
	pIzCaturday = false;
}

// Tracker subsystem
//
void WiredSocket::connectToTracker(QString theHostName, int thePort) {
	QString tmpHost;
	pSocketType = Wired::TrackerSocket;
	int tmpPort;
	if( theHostName.contains(":") ) { // Has port defined
		tmpHost = theHostName.section(":",0,0);
		tmpPort = theHostName.section(":",1,1).toInt();
	} else { // No port defined
		tmpPort = thePort;
		tmpHost = theHostName;
	}
	qDebug() << "WiredSocket: Connecting to tracker at"<<tmpHost<<"port"<<tmpPort;
	pSocket->connectToHostEncrypted(tmpHost, tmpPort);
}

// Wrapper that allows other code to tell this socket to connect to a remote server.
void WiredSocket::connectToWiredServer(QString theHostName, int thePort) {
	QString tmpHost;
	pSocketType = Wired::WiredSocket;
	int tmpPort;
	if( theHostName.contains(":") ) { // Has port defined
		tmpHost = theHostName.section(":",0,0);
		tmpPort = theHostName.section(":",1,1).toInt();
	} else { // No port defined
		tmpPort = thePort;
		tmpHost = theHostName;
	}
	qDebug() << "WiredSocket: Connecting to wired server at"<<tmpHost<<"port"<<tmpPort;
	pSocket->connectToHostEncrypted(tmpHost, tmpPort);
}

// Called by the socket once it has received some data.
// We will read the buffer, put the data in our own buffer and then read
// message after message from it before passing it to do_handle_wiredmessage().
void WiredSocket::on_socket_readyRead() {
	pBuffer += pSocket->readAll();
	int tmpPos = pBuffer.indexOf("\x04");
	while( tmpPos != -1 ) {
		QByteArray tmpLine = pBuffer.left(tmpPos);
		pBuffer = pBuffer.mid(tmpPos+1);
		do_handle_wiredmessage(tmpLine);
		tmpPos = pBuffer.indexOf("\x04");
	}
}

// First off, let's send the HELLO command for the handshake.
// We will get a 200 or 511 response to this.
void WiredSocket::on_socket_encrypted() {
	sendWiredCommand("HELLO");
	qDebug() << "WiredSocket: SSL/TLS connection established.";
}

WiredSocket::~WiredSocket()
{ }

/**
 * Get the number of users in the chat specified by theChatID.
 * @param theChatID The ID of the chat.
 * @return The number of users in the given chat. If theChatID is invalid, 0 will be returned.
 */
const int WiredSocket::userCountByChat(const int theChatID) {
	if( pUsers.contains(theChatID) ) {
		QList<ClassWiredUser> *tmpList = &pUsers[theChatID];
		//qDebug() << "WiredSocket::userCountByChat channel"<<theChatID<<"="<<tmpList->size();
		return tmpList->size();
	}
	return 0;
}

/**
 * Find a user within a specific chat by id and return it.
 * @param theChatID The ID of the chat.
 * @param theIndex The index of the user within the chat.
 * @return The ClassWiredUser object. If the user is not found, a default object will be returned with user id -1.
 */
const ClassWiredUser WiredSocket::userByIndex(const int theChatID, const int theIndex) {
	if( pUsers.contains(theChatID) ) {
		QList<ClassWiredUser> *tmpList = &pUsers[theChatID];
		return tmpList->value(theIndex);
	}

	ClassWiredUser wu; // Fallback user item
	wu.pUserID = -1;
	wu.pNick = "(invalid user)";
	return wu;
}



// A user/status has changed on the server. Update the user element accordingly.
void WiredSocket::on_server_userlist_changed(QList<QByteArray> theParams) {
	int tmpID = theParams.value(0).toInt();

	// We have to run through all chats and update the user items accordingly.
	QMutableHashIterator<int,QList<ClassWiredUser> > i(pUsers);
	while(i.hasNext()) {
		i.next();
		QList<ClassWiredUser> tmpList = i.value();
		QMutableListIterator<ClassWiredUser> j(tmpList);
		while(j.hasNext()) {
			ClassWiredUser tmpUsrOld = j.next();
			if(tmpUsrOld.pUserID==tmpID) {
				ClassWiredUser tmpUsr = tmpUsrOld;
				tmpUsr.pIdle = theParams.value(1).toInt();
				tmpUsr.pAdmin = theParams.value(2).toInt();
				tmpUsr.pIcon = theParams.value(3).toInt();
				tmpUsr.pNick = QString::fromUtf8( theParams.value(4) );
				tmpUsr.pStatus = QString::fromUtf8( theParams.value(5) );
				j.setValue(tmpUsr);
				
				// Fire a signal, only for the main list!
				if(i.key()==1)
					emit onServerUserChanged(tmpUsrOld, tmpUsr);
			}
		}
		i.setValue(tmpList);
	}
}

void WiredSocket::on_server_userlist_imagechanged(QList< QByteArray > theParams)
{
	// A user has changed his/her user image.
	int tmpID = theParams.value(0).toInt();

	// We have to run through all chats and update the user items accordingly.
	QMutableHashIterator<int,QList<ClassWiredUser> > i(pUsers);
	while(i.hasNext()) {
		i.next();
		QList<ClassWiredUser> tmpList = i.value();
		QMutableListIterator<ClassWiredUser> j(tmpList);
		while(j.hasNext()) {
			ClassWiredUser tmpUsrOld = j.next();
			if(tmpUsrOld.pUserID==tmpID) {
				ClassWiredUser tmpUsr = tmpUsrOld;
				tmpUsr.pImage = QByteArray::fromBase64(theParams.value(1,""));
				j.setValue(tmpUsr);
				
				// Fire a signal, only for the main list!
				if(i.key()==1)
					emit onServerUserChanged(tmpUsrOld, tmpUsr);
			}
		}
		i.setValue(tmpList);
	}
}

void WiredSocket::on_server_userlist_joined(QList< QByteArray > theParams )
{
	// A user has joined the server/chat.
	int tmpChatID = theParams.value(0).toInt();
	
	ClassWiredUser usr;
	usr.pUserID = theParams.value(1).toInt();
	usr.pIdle = theParams.value(2).toInt();
	usr.pAdmin = theParams.value(3).toInt();
	usr.pIcon = theParams.value(4).toInt();
	usr.pNick = QString::fromUtf8(theParams.value(5));
	usr.pLogin = QString::fromUtf8(theParams.value(6));
	usr.pIP = QString::fromUtf8(theParams.value(7));
	usr.pHost = QString::fromAscii(theParams.value(8));
	usr.pStatus = QString::fromAscii(theParams.value(9));
	usr.pImage = QByteArray::fromBase64(theParams.value(10));
	
	QList<ClassWiredUser> &tmpList = pUsers[tmpChatID];
	tmpList.append(usr);
	
	emit onServerUserJoined(tmpChatID, usr);
}

// Find a user by the user id and return the index.
const int WiredSocket::userIndexByID(const int theID, const int theChat) {
	if( pUsers.contains(theChat) ) {
		QList<ClassWiredUser> tmpList = pUsers[theChat];
		QListIterator<ClassWiredUser> j(tmpList);
		int tmpIdx = 0;
		while(j.hasNext()) {
			ClassWiredUser tmpUsr = j.next();
			if( tmpUsr.pUserID==theID )
				return tmpIdx;
			tmpIdx++;
		}
	}
	return -1;
}

void WiredSocket::on_server_userlist_left(QList<QByteArray> theParams ) {
	// The user left a chat or the server.
	int tmpChatID = theParams.value(0).toInt();
	int tmpUserID = theParams.value(1).toInt();
	
	QMutableHashIterator<int,QList<ClassWiredUser> > i(pUsers);
	while(i.hasNext()) { i.next();
		// If tmpChatID==1, the user has disconnected completely. Remove him/her form all chats, respectively.
		if( tmpChatID==1 || (tmpChatID==i.key()) ) {
			QList<ClassWiredUser> tmpList = i.value();
			QMutableListIterator<ClassWiredUser> j(tmpList);
			while(j.hasNext()) {
				ClassWiredUser tmpUsr = j.next();
				if(tmpUsr.pUserID==tmpUserID) {
					j.remove();
					emit onServerUserLeft(tmpChatID, tmpUsr);
				}
			}
			i.setValue(tmpList);
			
		}
	}
	
}

void WiredSocket::sendPrivateMessage(int theUserID, QString theMessage) {
	if(pIzCaturday)
		theMessage = tranzlate(theMessage);
	
	if( theUserID==0 ) {
		// Broadcast Message
		QByteArray buf("BROADCAST ");
		buf += theMessage.toUtf8();
		sendWiredCommand(buf);
	} else {
		// Send a private message to a user.
		QByteArray buf("MSG ");
		buf += QByteArray::number(theUserID).append(kFS).append(theMessage.toUtf8());
		sendWiredCommand(buf);
	}
}





void WiredSocket::leaveChat(int theChatID)
{
	// Leave a private chat.
	QByteArray buf("LEAVE ");
	buf += QByteArray::number(theChatID);
	sendWiredCommand(buf);
	
	// Remove chat array
	if( pUsers.contains(theChatID) )
		pUsers.remove(theChatID);
}




void WiredSocket::on_server_userlist_kicked(QList<QByteArray> theParams)
{
	// A user got kicked from the server.
	ClassWiredUser tmpVictim = getUserByID(theParams.value(0).toInt());
	ClassWiredUser tmpKiller = getUserByID(theParams.value(1).toInt());
	QString tmpReason = QString::fromUtf8(theParams.value(2));
	
	QMutableHashIterator<int,QList<ClassWiredUser> > i(pUsers);
	while(i.hasNext()) { i.next();
		// If tmpChatID==1, the user has disconnected completely. Remove him/her form all chats, respectively.
			QList<ClassWiredUser> tmpList = i.value();
			QMutableListIterator<ClassWiredUser> j(tmpList);
			while(j.hasNext()) {
				ClassWiredUser tmpUsr = j.next();
				if(tmpUsr.pUserID==tmpVictim.pUserID) {
					j.remove();
					emit onServerUserLeft(i.key(), tmpUsr);
				}
			}
			i.setValue(tmpList);
	}
	emit onServerUserKicked(tmpVictim, tmpKiller, tmpReason);
}

void WiredSocket::on_server_userlist_banned(QList< QByteArray > theParams)
{
	// A user got kicked from the server.
	ClassWiredUser tmpVictim = getUserByID(theParams.value(0).toInt());
	ClassWiredUser tmpKiller = getUserByID(theParams.value(1).toInt());
	QString tmpReason = QString::fromUtf8(theParams.value(2));
	
	QMutableHashIterator<int,QList<ClassWiredUser> > i(pUsers);
	while(i.hasNext()) { i.next();
		// If tmpChatID==1, the user has disconnected completely. Remove him/her form all chats, respectively.
		QList<ClassWiredUser> tmpList = i.value();
		QMutableListIterator<ClassWiredUser> j(tmpList);
		while(j.hasNext()) {
			ClassWiredUser tmpUsr = j.next();
			if(tmpUsr.pUserID==tmpVictim.pUserID) {
				j.remove();
				emit onServerUserLeft(i.key(), tmpUsr);
			}
		}
		i.setValue(tmpList);
	}
	emit onServerUserBanned(tmpVictim, tmpKiller, tmpReason);
}

void WiredSocket::on_server_userinfo(QList< QByteArray > theParams)
{
	// Received user info from the server.
	ClassWiredUser usr = ClassWiredUser::fromUserInfo(theParams);
	emit onServerUserInfo(usr);
	
	
}

void WiredSocket::createChatWithClient(int theUserID)
{
	// Initiate a new private chat with a specific user.
	pInvitedUserID = theUserID;
	sendWiredCommand("PRIVCHAT");
}

void WiredSocket::on_server_new_chat_created(QList<QByteArray> theParams)
{
	// Server opened a new private chat for us. We now invite the user we have stored in pInvitedUserID before.
	int tmpChatID = theParams.value(0).toInt();
	
	//ClassWiredUser tmpUsr = getUserByID();
	//qDebug() << "Adding"<<tmpUsr.pNick<<"to Chat"<<tmpChatID;
	QList<ClassWiredUser> &tmpList = pUsers[tmpChatID];
	tmpList.append(sessionUser);
	emit onServerPrivateChatCreated(tmpChatID);
	inviteClientToChat(tmpChatID, pInvitedUserID);
	pInvitedUserID = 0;
}

// Invite a user to a private chat.
void WiredSocket::inviteClientToChat(int theChatID, int theUserID) {
	QByteArray buf("INVITE ");
	buf += QByteArray::number(theUserID);
	buf += kFS;
	buf += QByteArray::number(theChatID);
	sendWiredCommand(buf);
}

// Request the server banner.
void WiredSocket::getServerBanner() {
	 sendWiredCommand("BANNER");
}

void WiredSocket::on_server_banner(QList< QByteArray > theParams) {
	QPixmap banner;
	banner.loadFromData( QByteArray::fromBase64(theParams.value(0)) );
	if( !banner.isNull() ) {
		pServerBanner = banner;
		emit onServerBanner(banner);
	}
}

void WiredSocket::on_server_broadcast(QList< QByteArray > theParams)
{
	// Handle Server Broadcast
	ClassWiredUser usr = getUserByID( theParams.value(0).toInt() );
	QString msg = QString::fromUtf8(theParams.value(1));
	emit onServerBroadcast(usr, msg);
}

// Request to clear the news from the server's database
void WiredSocket::clearNews() {
	sendWiredCommand("CLEARNEWS");
}

void WiredSocket::getFileList(QString thePath) {
	// Request a file listing of all files at thePath.
	// Results in a number of 410 and one 411 responses.
	QByteArray buf("LIST ");
	buf += thePath.toUtf8();
	sendWiredCommand(buf);
}

void WiredSocket::setUserIcon(QPixmap theIcon) {
	// Send a new user icon to the server
	QByteArray tmpCmd;
	if( !theIcon.isNull() ) {
		QImage tmpImg = theIcon.toImage();
		QByteArray ba;
		QBuffer buf(&ba);
		buf.open(QIODevice::WriteOnly);
		tmpImg.save(&buf, "PNG");
		tmpCmd += "ICON 0"; // unused since 1.1
		tmpCmd += kFS;
		tmpCmd += ba.toBase64();
		sessionUser.pImage = ba;
		sendWiredCommand(tmpCmd); 
	}
}



void WiredSocket::on_socket_error()
{
	// Handle a socket error and generate a signal
	QString theErrReason = pSocket->errorString();
	int theErr = pSocket->error();
	pSocket->disconnectFromHost();
	emit onSocketError(theErrReason,theErr);
}

/**
 * Request a file download slot from the server. WiredSocket checks if the file already exists, 
 * calculates a checksum for it and resumes it, if possible. Internally WiredSocket will request
 * a STAT on the file before starting the transfer.
 * @param thePath The remote path of the file to be downloaded.
 * @param theLocalPath The destination path of the file on the local system.
 */
void WiredSocket::getFile(const QString thePath, const QString theLocalPath){
	
	// Check for duplicate downloads
	QListIterator<QPointer<WiredTransferSocket> > i(pTransferSockets);
	while( i.hasNext() ) {
		WiredTransferSocket *tmpT = i.next();
		if(tmpT) {
			ClassWiredTransfer tmpTrans = tmpT->pTransfer;
			if( tmpTrans.pRemotePath==thePath ) {
				qDebug() << "Transfer Phase 1:"<<thePath<<": already transferring, abort!";
				return;
			}
		}
	}

	// Create a record for the status tracking
	// We first set this to stauts 0 (waiting for stat) since we need the checksum of the file
	// for local comparisation!
	ClassWiredTransfer tmpTrans;
	tmpTrans.pStatus = 0; // waiting for stat
	tmpTrans.pTransferType = 0; // download
	tmpTrans.pRemotePath = thePath;
	tmpTrans.pLocalPath = theLocalPath;
		
	// And the socket...
	WiredTransferSocket *tmpSock = new WiredTransferSocket;
	qDebug() << "Current creator thread:"<<QThread::currentThread();
	connect(tmpSock, SIGNAL(fileTransferDone(ClassWiredTransfer)), this, SIGNAL(fileTransferDone(ClassWiredTransfer)));
	connect(tmpSock, SIGNAL(fileTransferError(ClassWiredTransfer)), this, SIGNAL(fileTransferError(ClassWiredTransfer)));
	connect(tmpSock, SIGNAL(fileTransferStatus(ClassWiredTransfer)), this, SIGNAL(fileTransferStatus(ClassWiredTransfer)));
	connect(tmpSock, SIGNAL(fileTransferStarted(ClassWiredTransfer)), this, SIGNAL(fileTransferStarted(ClassWiredTransfer)));
	connect(tmpSock, SIGNAL(destroyed()), this, SLOT(cleanTransfers()));
	
	tmpSock->setServer( pSocket->peerAddress().toString(), pSocket->peerPort() );
	tmpSock->pTransfer = tmpTrans;
	pTransferSockets.append(tmpSock);
	statFile(thePath);
	qDebug() << "Transfer Phase 1/3: STAT'ing "<<thePath<<"to"<<theLocalPath;
}



// File transfer is ready. The client should now connect and send or recieve the
// requested file.
void WiredSocket::on_server_transfer_ready(QList<QByteArray> theParams) {
	QString tmpPath = QString::fromUtf8(theParams.value(0));
	qlonglong tmpOffset = theParams.value(1).toLongLong();
	QString tmpHash = QString::fromUtf8(theParams.value(2));
	
	QListIterator<QPointer<WiredTransferSocket> > i(pTransferSockets);
	while( i.hasNext() ) {
		WiredTransferSocket *tmpT = i.next();
		if(tmpT) {
			ClassWiredTransfer tmpTrans = tmpT->pTransfer;
			if( tmpTrans.pRemotePath==tmpPath && tmpTrans.pStatus==1 ) {
				qDebug() << "Transfer Phase 2/3: Requesting transfer slot for"<<tmpPath<<"with hash"<<tmpHash<<"offset"<<tmpOffset;
				tmpT->pTransfer.pOffset = tmpOffset;
				tmpT->pTransfer.pHash = tmpHash;
				tmpT->pTransfer.pQueuePosition = 0;
				tmpT->pTransfer.pStatus = 2;
				qDebug() << "Current thread is"<<QThread::currentThread()<<"starting"<<tmpT;
				tmpT->start();
				return;
			}
		}
	}
	
	// Duh! Something went wrong.
	qDebug() << "Transfer Phase 2/3: Transfer ready, but we didn't request it?";
}


// The transfer has been queued on the server. Update the transfers respectively.
void WiredSocket::on_server_transfer_queued(QList< QByteArray > theParams) {
	QString tmpPath = QString::fromUtf8(theParams.value(0));
	int tmpPosition = theParams.value(1).toInt();
	qDebug() << "Transfer Phase 2/3: File"<<tmpPath<<"queued at position"<<tmpPosition<<" . o ( Zz..zzzZ..zzZ )";
	
	QListIterator<QPointer<WiredTransferSocket> > i(pTransferSockets);
	while( i.hasNext() ) {
		WiredTransferSocket *tmpT = i.next();
		if(tmpT) {
			ClassWiredTransfer tmpTrans = tmpT->pTransfer;
			if( tmpTrans.pRemotePath==tmpPath && tmpTrans.pStatus==1 ) {
				tmpT->pTransfer.pQueuePosition = tmpPosition;
				tmpT->pTransfer.pStatus = 1;
				emit fileTransferStatus(tmpTrans);
				return;
			}
		}
	}
	
	
}

void WiredSocket::on_server_search_listing(QList<QByteArray> theParams) {
	ClassWiredFile tmpFile;
	tmpFile.path = QString::fromUtf8(theParams.value(0));
	tmpFile.type = theParams.value(1).toInt();
	tmpFile.size = theParams.value(2).toLongLong();
	tmpFile.created = QDateTime::fromString( QString::fromUtf8(theParams.value(3)), Qt::ISODate );
	tmpFile.modified = QDateTime::fromString( QString::fromUtf8(theParams.value(4)), Qt::ISODate );
	if(pSearchResults.count()<1024)
		pSearchResults.append(tmpFile);
}

void WiredSocket::on_server_search_done(QList<QByteArray> ) {
	emit fileSearchDone(pSearchResults);
	pSearchResults.clear();
}


void WiredSocket::on_server_groups_listing(QList< QByteArray > theParams) {
	pAdminGroups.append( QString::fromUtf8( theParams.value(0) ) );
}

void WiredSocket::on_server_groups_done() {
	emit groupsListingDone(pAdminGroups);
	pAdminGroups.clear();
}

void WiredSocket::on_server_users_listing(QList< QByteArray > theParams) {
	pAdminUsers.append( QString::fromUtf8(theParams.value(0)) );
}

void WiredSocket::on_server_users_done() {
	emit usersListingDone(pAdminUsers);
	pAdminUsers.clear();
}

void WiredSocket::on_server_user_spec(QList< QByteArray > theParams) {
	ClassWiredUser tmpAcct;
	tmpAcct.pAccountType = 0;
	tmpAcct.pLogin = QString::fromUtf8( theParams.value(0) );
	tmpAcct.pPassword = QString::fromUtf8( theParams.value(1) );
	tmpAcct.pGroupName =  QString::fromUtf8( theParams.value(2) );
	theParams.removeFirst();
	theParams.removeFirst();
	theParams.removeFirst();
	tmpAcct.setFromPrivileges(theParams);
	emit userSpecReceived(tmpAcct);
}

void WiredSocket::on_server_group_spec(QList< QByteArray > theParams) {
	ClassWiredUser tmpAcct;
	tmpAcct.pLogin = QString::fromUtf8( theParams.value(0) );
	tmpAcct.pAccountType = 1;
	theParams.removeFirst();
	tmpAcct.setFromPrivileges(theParams);
	emit groupSpecReceived(tmpAcct);
}


// Tracker handling code
//

void WiredSocket::on_tracker_listing_item(QList< QByteArray > theParams) {
	ClassTrackerServer ts;
	ts.loadFromTrackerResponse(theParams);
	pTrackerServers.append(ts);
}

void WiredSocket::on_tracker_listing_done() {
	emit trackerServersReceived(pTrackerServers);
	pTrackerServers.clear();
	this->disconnectFromServer();
}








void WiredSocket::on_server_file_info(QList<QByteArray> theParams) {
	// Received file information. This can be the response to a file in the
	// transfer queue or a user request. Let's see...
	
	QString tmpPath = QString::fromUtf8(theParams.value(0));
	qlonglong tmpSize = theParams.value(2).toLongLong();
	QDateTime tmpCreated = QDateTime::fromString( theParams.value(3), Qt::ISODate );
	QDateTime tmpModified = QDateTime::fromString( theParams.value(4), Qt::ISODate );
	QString tmpChecksum = QString::fromUtf8(theParams.value(5));
	QString tmpComment = QString::fromUtf8(theParams.value(6));
	
	// Check if we are waiting for a stat
	QListIterator<QPointer<WiredTransferSocket> > i(pTransferSockets);
	int tmpIdx=0;
	while( i.hasNext() ) {
		WiredTransferSocket *tmpT = i.next();
		if(tmpT) {
			if( tmpT->pTransfer.pStatus==0 && tmpT->pTransfer.pRemotePath==tmpPath ) {
				qDebug() << "Transfer Phase 1/3:"<<tmpPath<<": received stat, requesting transfer:"<<tmpPath<<tmpChecksum;
				
				// Check if we need to resume
				QString tmpFilePath = tmpT->pTransfer.pLocalPath+".WiredTransfer";
				QFile tmpFile(tmpFilePath);
				if(tmpFile.exists()) {
					if(tmpFile.open(QIODevice::ReadOnly)) {
						QByteArray tmpDat = tmpFile.read(1024*1024);
						QString tmpCS = QCryptographicHash::hash(tmpDat, QCryptographicHash::Sha1).toHex();
						qDebug() << "WiredSocket: Download file exists, checksum:"<<tmpCS<<"; server:"<<tmpChecksum;
						if(tmpChecksum==tmpCS && tmpFile.size()<tmpSize) {
							qDebug() << "WiredSocket: Checksums are identical. Offset is"<<tmpFile.size();
							tmpT->pTransfer.pOffset = tmpFile.size();
							tmpT->pTransfer.pDoneSize = tmpFile.size();
						} else {
							qDebug() << "WiredSocket: Checksums are NOT identical, deleting file.";
							tmpFile.close();
							tmpFile.remove();
							tmpT->pTransfer.pOffset = 0;
						}
					}
				}
				
				// Set some more info
				tmpT->pTransfer.pChecksum = tmpChecksum;
				tmpT->pTransfer.pTotalSize = tmpSize;
				tmpT->pTransfer.pStatus = 1; // waiting for transfer
				
				// Request a file download from the server
				QByteArray buf("GET ");
				buf += tmpPath.toUtf8();
				buf += kFS;
				buf += QByteArray::number(tmpT->pTransfer.pOffset);
				sendWiredCommand(buf);
				
				qDebug() << "WiredSocket: GET'ing the file with offset"<<tmpT->pTransfer.pOffset;
				
				return;
			}

		}
		tmpIdx++;
	}
	
	// Not in the transfer queue, pass the event to the user code.
	ClassWiredFile tmpFile;
	tmpFile.setFromStat(theParams);
	emit onServerFileInfo(tmpFile);
	
}

// Request uploading of a file to the server
void WiredSocket::putFile(const QString theLocalPath, const QString theRemotePath) {
	QFile tmpFile(theLocalPath);
	if(tmpFile.exists() && !theRemotePath.isEmpty()) {

		// Check for duplicate uploads
		QListIterator<QPointer<WiredTransferSocket> > i(pTransferSockets);
		while( i.hasNext() ) {
			WiredTransferSocket *tmpT = i.next();
			if(tmpT) {
				ClassWiredTransfer tmpTrans = tmpT->pTransfer;
				if( tmpTrans.pRemotePath==theRemotePath ) {
					qDebug() << "Transfer Phase 1:"<<theRemotePath<<": already transferring, abort!";
					return;
				}
			}
		}
		
		// Create a record for the status tracking
		// We first set this to stauts 0 (waiting for stat) since we need the checksum of the file
		// for local comparisation!
		ClassWiredTransfer tmpTrans;
		tmpTrans.pStatus = 1; // waiting for slot (no stat required)
		tmpTrans.pTransferType = 1; // upload
		tmpTrans.pRemotePath = theRemotePath;
		tmpTrans.pLocalPath = theLocalPath;
		tmpTrans.pTotalSize = tmpFile.size();
		tmpTrans.calcLocalChecksum();
		
		// And the socket...
		WiredTransferSocket *tmpSock = new WiredTransferSocket;
		connect(tmpSock, SIGNAL(fileTransferDone(ClassWiredTransfer)), this, SIGNAL(fileTransferDone(ClassWiredTransfer)));
		connect(tmpSock, SIGNAL(fileTransferError(ClassWiredTransfer)), this, SIGNAL(fileTransferError(ClassWiredTransfer)));
		connect(tmpSock, SIGNAL(fileTransferStatus(ClassWiredTransfer)), this, SIGNAL(fileTransferStatus(ClassWiredTransfer)));
		connect(tmpSock, SIGNAL(fileTransferStarted(ClassWiredTransfer)), this, SIGNAL(fileTransferStarted(ClassWiredTransfer)));
		connect(tmpSock, SIGNAL(fileTransferSocketError(QAbstractSocket::SocketError)), this, SIGNAL(fileTransferSocketError(QAbstractSocket::SocketError)));
		connect(tmpSock, SIGNAL(destroyed()), this, SLOT(cleanTransfers()));
		tmpSock->setServer( pSocket->peerAddress().toString(), pSocket->peerPort() );
		tmpSock->pTransfer = tmpTrans;
		pTransferSockets.append(tmpSock);
		
		// Request upload slot
		qDebug() << "WiredSocket: Upload requested, hash"<<tmpTrans.pChecksum;
		QByteArray tmpBuf("PUT ");
		tmpBuf += theRemotePath.toUtf8();
		tmpBuf += kFS;
		tmpBuf += QByteArray::number(tmpFile.size());
		tmpBuf += kFS;
		tmpBuf += tmpTrans.pChecksum;
		sendWiredCommand(tmpBuf);
	}
}



// Handle a privileges response/info
void WiredSocket::on_server_privileges(QList< QByteArray > theParams) {
	qDebug() << "WiredSocket: Updating privileges on session";
	sessionUser.setFromPrivileges(theParams);
	emit userPrivileges(sessionUser);
}

// ///
// /// Request Commands ///
// ///

// Send the login sequence to the server (reponses: 201 or 510)
void WiredSocket::do_send_user_login() {
	sendClientInfo();
	setUserNick(sessionUser.pNick.toUtf8());
	setUserIcon(sessionUser.iconAsPixmap());
	setUserStatus(sessionUser.pStatus);
	
	QByteArray tmpCmd;
	tmpCmd += "USER "+sessionUser.pLogin.toUtf8(); sendWiredCommand(tmpCmd); tmpCmd.clear();
	tmpCmd += "PASS "+sessionUser.cryptedPassword().toUtf8(); sendWiredCommand(tmpCmd);
}

// Set the nickname of the current user session.
void WiredSocket::setUserNick(QString theNick) {
	sessionUser.pNick = theNick;
	QByteArray tmpCmd("NICK ");
	tmpCmd += theNick.toUtf8();
	sendWiredCommand(tmpCmd);
}

// Update the user status for the session
void WiredSocket::setUserStatus(QString theStatus) {
	sessionUser.pStatus = theStatus;
	QByteArray buf("STATUS ");
	buf += sessionUser.pStatus.toUtf8();
	sendWiredCommand(buf);
}

// Reject/Decline a private chat.
void WiredSocket::rejectChat(int theChatID) {
	QByteArray buf("DECLINE ");
	buf += QByteArray::number(theChatID);
	sendWiredCommand(buf);
}

// Join a private chat.
void WiredSocket::joinChat(int theChatID) {
	QByteArray buf("JOIN ");
	buf += QByteArray::number(theChatID);
	sendWiredCommand(buf);
	do_request_user_list(theChatID);
}

// Set a chat topic.
void WiredSocket::setChatTopic(int theChatID, QString theTopic) {
	QByteArray buf("TOPIC ");
	buf += QByteArray::number(theChatID);
	buf += kFS;
	buf += theTopic.toUtf8();
	sendWiredCommand(buf);
}

// Ban a user from the server.
void WiredSocket::banClient(int theUserID, QString theReason) {
	QByteArray tmpBuf("BAN ");
	tmpBuf += QByteArray::number(theUserID);
	tmpBuf += kFS;
	tmpBuf += theReason.toUtf8();
	sendWiredCommand(tmpBuf);
}


// Request the list of users for a specific channel.
void WiredSocket::do_request_user_list(int theChannel) {	
	QByteArray tmpCmd("WHO ");
	tmpCmd += QByteArray::number(theChannel);
	sendWiredCommand(tmpCmd);
}

// Request news from the server (reponse will be 320s and one 321)
void WiredSocket::getNews() {
	sendWiredCommand("NEWS");
}

// Request current privileges from the server
void WiredSocket::getPrivileges() { sendWiredCommand("PRIVILEGES"); }

// Post news to the news board
void WiredSocket::postNews(QString thePost) {
	QByteArray data("POST ");
	data += thePost;
	sendWiredCommand(data);
}

// Request info for a user
void WiredSocket::getClientInfo(int theUserID) {
	QByteArray buf("INFO ");
	buf += QByteArray::number(theUserID);
	sendWiredCommand(buf);
}

// Send a chat message to the server.
void WiredSocket::sendChat(int theChatID, QString theText, bool theIsAction) {
	QByteArray buf;
	if(pIzCaturday)
		theText = tranzlate(theText);
	
	if(!theIsAction) buf+="SAY "; else buf+="ME ";
	buf += QByteArray::number(theChatID);
	buf += kFS;
	buf += theText.toUtf8();
	sendWiredCommand(buf);
}

// Request some file information.
void WiredSocket::statFile(const QString thePath) {
	QByteArray buf("STAT ");
	buf += thePath.toUtf8();
	sendWiredCommand(buf);
}

// Request to create a folder at the specified path
void WiredSocket::createFolder(const QString thePath) {
	QByteArray tmpBuf("FOLDER ");
	tmpBuf += thePath.toUtf8();
	sendWiredCommand(tmpBuf);
}

// Kick a user off the server.
void WiredSocket::kickClient(int theUserID, QString theReason) {
	QByteArray tmpBuf("KICK ");
	tmpBuf += QByteArray::number(theUserID);
	tmpBuf += kFS;
	tmpBuf += theReason.toUtf8();
	sendWiredCommand(tmpBuf);
}

// Request to move a file on the server
void WiredSocket::moveFile(const QString thePath, const QString theDestination) {
	QByteArray tmpBuf("MOVE ");
	tmpBuf += thePath.toUtf8();
	tmpBuf += kFS;
	tmpBuf += theDestination.toUtf8();
	sendWiredCommand(tmpBuf);
}

// Delete a file or folder from the server.
void WiredSocket::deleteFile(const QString thePath) {
	if(thePath=="/" || thePath.isEmpty()) return;
	QByteArray tmpBuf("DELETE ");
	tmpBuf += thePath.toUtf8();
	sendWiredCommand(tmpBuf);
}

// Request a list of groups.
void WiredSocket::getGroups() {
	 sendWiredCommand("GROUPS");
}

// Request a list of users.
void WiredSocket::getUsers() {
	sendWiredCommand("USERS");
}

// Create a user account on the server.
void WiredSocket::createUser(ClassWiredUser tmpUser) {
	QByteArray tmpBuf("CREATEUSER ");
	tmpBuf += tmpUser.pLogin.toUtf8();
	tmpBuf += kFS;
	tmpBuf += tmpUser.pPassword.toUtf8();
	tmpBuf += kFS;
	tmpBuf += tmpUser.pGroupName.toUtf8();
	tmpBuf += kFS;
	tmpBuf += tmpUser.privilegesFlags();
	sendWiredCommand(tmpBuf);
}

// Modify a user account on the server.
void WiredSocket::editUser(ClassWiredUser tmpUser) {
	QByteArray tmpBuf("EDITUSER ");
	tmpBuf += tmpUser.pLogin.toUtf8();
	tmpBuf += kFS;
	tmpBuf += tmpUser.pPassword.toUtf8();
	tmpBuf += kFS;
	tmpBuf += tmpUser.pGroupName.toUtf8();
	tmpBuf += kFS;
	tmpBuf += tmpUser.privilegesFlags();
	sendWiredCommand(tmpBuf);
}

// Create a group on the server.
void WiredSocket::createGroup(ClassWiredUser tmpUser) {
	QByteArray tmpBuf("CREATEGROUP ");
	tmpBuf += tmpUser.pLogin.toUtf8();
	tmpBuf += kFS;
	tmpBuf += tmpUser.privilegesFlags();
	sendWiredCommand(tmpBuf);
}

// Modify a user account on the server.
void WiredSocket::editGroup(ClassWiredUser tmpUser) {
	QByteArray tmpBuf("EDITGROUP ");
	tmpBuf += tmpUser.pLogin.toUtf8();
	tmpBuf += kFS;
	tmpBuf += tmpUser.privilegesFlags();
	sendWiredCommand(tmpBuf);
}

void WiredSocket::deleteGroup(QString theName) {
	QByteArray tmpBuf("DELETEGROUP ");
	tmpBuf += theName.toUtf8();
	sendWiredCommand(tmpBuf);
}

void WiredSocket::deleteUser(QString theName) {
	QByteArray tmpBuf("DELETEUSER ");
	tmpBuf += theName.toUtf8();
	sendWiredCommand(tmpBuf);
}

void WiredSocket::readUser(QString theName) {
	QByteArray tmpBuf("READUSER ");
	tmpBuf += theName.toUtf8();
	sendWiredCommand(tmpBuf);
}

void WiredSocket::readGroup(QString theName) {
	QByteArray tmpBuf("READGROUP ");
	tmpBuf += theName.toUtf8();
	sendWiredCommand(tmpBuf);
}

// Tracker - request servers
void WiredSocket::tracker_request_servers() {
	sendWiredCommand("SERVERS");
}





/**
 * Search the server for files.
 * @param theSearch The search term.
 */
void WiredSocket::searchFiles(const QString theSearch) {
	QByteArray tmpBuf("SEARCH ");
	tmpBuf += theSearch.toUtf8();
	sendWiredCommand(tmpBuf);
}


void WiredSocket::sendClientInfo() {
	QString tmpV("%1/%2 (%3; %4; %5)");

QString tmpOsVersion("Unknown");
QString tmpOsName("Unknown");
QString tmpOsArch("Unknown");


#ifdef Q_WS_MAC
	tmpOsName = "Mac OS X";
	switch(QSysInfo::MacintoshVersion) {
		case QSysInfo::MV_10_3: tmpOsVersion="10.3"; break;
		case QSysInfo::MV_10_4: tmpOsVersion="10.4"; break;
		case QSysInfo::MV_10_5: tmpOsVersion="10.5"; break;
		default: tmpOsVersion="Unknown"; break;
	}
#endif

#ifdef Q_WS_WIN
	tmpOsName = "Windows";
	switch(QSysInfo::WindowsVersion) {
		case QSysInfo::WV_98: tmpOsVersion="Windows 98"; break;
		case QSysInfo::WV_Me: tmpOsVersion="Windows Me"; break;
		case QSysInfo::WV_NT: tmpOsVersion="Windows NT"; break;
		case QSysInfo::WV_2000: tmpOsVersion="Windows 2000"; break;
		case QSysInfo::WV_XP: tmpOsVersion="Windows XP"; break;
		case QSysInfo::WV_2003: tmpOsVersion="Windows 2003"; break;
		case QSysInfo::WV_VISTA: tmpOsVersion="Windows Vista"; break;  /* <- huh, right, probably unused ;) */
		default: tmpOsVersion="Unknown"; break;
	}
#endif
	
#ifdef Q_OS_LINUX
	tmpOsName = "Linux";
	QProcess shell;
	shell.start("uname -r");
	if(shell.waitForFinished())
		tmpOsVersion = shell.readAllStandardOutput();
	tmpOsVersion = tmpOsVersion.trimmed();

#endif


#ifdef __i386__
	tmpOsArch = "Intel";
#endif
#ifdef __ia64__
	tmpOsArch = "IA64";
#endif
#ifdef __powerpc__
	tmpOsArch = "PowerPC";
#endif
	
	QByteArray tmpBuf("CLIENT ");
	tmpBuf += tmpV.arg(
		pClientName,
  		pClientVersion,
		tmpOsName,
  		tmpOsVersion,
		tmpOsArch
	);
	sendWiredCommand(tmpBuf);
}





// ///
// /// SESSION PROTOCOL SUPPORT
// ///

// Send a Wired message
void WiredSocket::sendWiredCommand(const QByteArray theData) {
	if(!pSocket->isOpen()) return;
	QByteArray tmpBuffer;
	tmpBuffer += theData;
	tmpBuffer += kEOF;
	pSocket->write(tmpBuffer);
}

// Return a list of parameters from the message, automatically skipping the command identifier.
QList<QByteArray> WiredSocket::GetFieldArray(QByteArray theData) {
	QByteArray tmpData(theData);
	int tmpPos = theData.indexOf(" "); // Remove the identifier
	if( tmpPos!=-1 ) tmpData=tmpData.mid(tmpPos+1);
	QList<QByteArray> tmpParams = tmpData.split(28);
	return tmpParams;
}

void WiredSocket::cleanTransfers() {
	QMutableListIterator<QPointer<WiredTransferSocket> > i(pTransferSockets);
	while(i.hasNext()) {
		QPointer<WiredTransferSocket> tmpP = i.next();
		if(tmpP.isNull()) i.remove();
	}
}

/**
 * Cancel a running transfer.
 * @param  The transfer to cancel.
 */
void WiredSocket::cancelTransfer(ClassWiredTransfer theTransfer) {
	QMutableListIterator<QPointer<WiredTransferSocket> > i(pTransferSockets);
	while(i.hasNext()) {
		QPointer<WiredTransferSocket> tmpTS = i.next();
		if(tmpTS) {
			if(tmpTS->pTransfer.pChecksum==theTransfer.pChecksum && tmpTS->pTransfer.pRemotePath==theTransfer.pRemotePath) {
				tmpTS->cancelTransfer();
				i.remove();
				qDebug() << "Deleted transfer socket"<<tmpTS->pTransfer.pHash;
				return;
			}

		}

	}
}

QString WiredSocket::tranzlate(QString theText){
	QString tmpText = theText.toLower();
	QString tmpResult;
	
	QStringList tmpWords = tmpText.split(" ");
	QStringListIterator i(tmpWords);
	while(i.hasNext()) {
		QString tmpWord = i.next();
		if(pTranzlator.contains(tmpWord))
				tmpWord = pTranzlator.value(tmpWord);
		tmpResult += QString(" ")+tmpWord;
	}

	tmpResult = tmpResult.trimmed();
	tmpResult = tmpResult.replace(":d", ":D");
	tmpResult = tmpResult.replace("?", ", plz?");
	tmpResult = tmpResult.replace("!", "?!!11");
	return tmpResult;
}







