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

 
#include "wiredcommon.h"
#include "classwiredsession.h"
#include "gui/widgetsendprivmsg.h"
#include "gui/widgetuserinfo.h"
#include "gui/widgetfilebrowser.h"
#include "gui/widgetconnect.h"
#include "gui/modelfiletransfers.h"

#include <QMessageBox>
#include <QtGui>




/**
 * Constructor
 */
ClassWiredSession::ClassWiredSession(QObject *parent) : QObject(parent) {
	pWiredSocket = new WiredSocket(this);
	pConnWindow = new ConnWindow();
	pMainChat = new WidgetForum();
	
	enableConnToolButtons(false);
	
	// Show the connect dialog initially
	pConnectWindow = new WidgetConnect();
	pConnWindow->setCentralWidget(pConnectWindow);
	
	// Set up the user list model
	pUserListModel = new ModelUserList(this);
	pUserListModel->setWiredSocket(pWiredSocket);
	pMainChat->pSession = this;
	pMainChat->setUserListModel(pUserListModel);
	
	doSetupConnections();
	
	QSettings settings;
	if( settings.contains("general/icon") ) { // Custom icon
		QPixmap tmpIcon = settings.value("general/icon").value<QPixmap>();
		pWiredSocket->setUserIcon(tmpIcon);
	} else { // Default icon
		QPixmap tmpIcon(":/icons/qwired_logo_32.png");
		pWiredSocket->setUserIcon(tmpIcon);
	}
	pWiredSocket->setUserStatus( settings.value("general/status","Qwired Newbie").toString() );
	pWiredSocket->setNickname(settings.value("general/nickname", "Unnamed").toString());

	reloadPrefs();
	pConnWindow->show();
}

/**
 * Destructor
 */
ClassWiredSession::~ClassWiredSession() {
	if(pMainChat) pMainChat->deleteLater();
	if(pConnWindow) pConnWindow->deleteLater();
	if(pWiredSocket) pWiredSocket->deleteLater();
}


/**
 * Called when the connection window is destroyed/closed. Allows the manager class to do
 * some clean up work.
 */
void ClassWiredSession::connectionWindowDestroyed(QObject*) {
	this->deleteLater();
}


/**
 * Create a new connection and append it to the singleton object.
 */
void ClassWiredSession::createNewConnection() {
	WiredSingleton *tmpS = &WSINGLETON::Instance();
	ClassWiredSession *tmpNew = new ClassWiredSession();
	tmpS->addSession(tmpNew);

}

/**
 * Set up connections between objects in this class.
 */
void ClassWiredSession::doSetupConnections() {
	// Connect Window
	//
	connect(pConnectWindow, SIGNAL(onConnnectReady(QString,QString,QString)), this, SLOT(onDoConnect(QString,QString,QString)) );
	connect(pConnWindow, SIGNAL(destroyed(QObject*)), this, SLOT(connectionWindowDestroyed(QObject*)) );
	
	
	// Socket connections
	//
	connect( pWiredSocket, SIGNAL(onMotdReceived(QString)), this, SLOT(displayMotd(QString)));
	connect(pWiredSocket, SIGNAL(onConferenceChanged(const int &, const QString &, const int &, const bool &, const int &)),
			this, SLOT(handleConferenceChanged(const int &, const QString &, const int &, const bool &, const int &)));;

	
	connect( pWiredSocket, SIGNAL(onServerChat(int,int,QString,bool)), this, SLOT(do_handle_chat_message(int,int,QString,bool)) );
	connect( pWiredSocket, SIGNAL(onChatTopic(int, QString, QString, QHostAddress, QDateTime, QString)), this, SLOT(doHandleChatTopic(int, QString, QString, QHostAddress, QDateTime, QString)) );
	connect( pWiredSocket, SIGNAL(onPrivateMessage(ClassWiredUser,QString)), this, SLOT(doHandlePrivMsg(ClassWiredUser,QString)) );
	connect( pWiredSocket, SIGNAL(onServerBroadcast(ClassWiredUser,QString)), this, SLOT(doHandleBroadcast(ClassWiredUser,QString)) );
	connect( pWiredSocket, SIGNAL(onServerUserInfo(ClassWiredUser)), this, SLOT(doHandleUserInfo(ClassWiredUser)) );
	connect( pWiredSocket, SIGNAL(onServerPrivateChatInvitation(int,ClassWiredUser)), this, SLOT(doHandlePrivateChatInvitation(int,ClassWiredUser)) );
	connect( pWiredSocket, SIGNAL(onServerPrivateChatCreated(int)), this, SLOT(doCreateNewChat(int)) );
	
	connect( pWiredSocket, SIGNAL(onSocketError(QString,int)), this, SLOT(onSocketError(QString,int)) );
	connect( pWiredSocket, SIGNAL(onServerInformation()), this, SLOT(onSocketServerInfo()) );
	connect( pWiredSocket, SIGNAL(onServerLoginSuccessful()), this, SLOT(onSocketLoginSuccessful()) );
		
	connect(pWiredSocket, SIGNAL(onServerBanner(QPixmap)), this, SLOT(setBannerView(QPixmap)) );
	connect(pWiredSocket, SIGNAL(errorOccoured(int)), this, SLOT(handleErrorOccoured(int)) );
	connect(pWiredSocket, SIGNAL(errorLoginFailed()), this, SLOT(onSocketLoginFailed()) );
	connect(pWiredSocket, SIGNAL(userPrivileges(ClassWiredUser)), this, SLOT(onSocketPrivileges(ClassWiredUser)) );
	connect(pWiredSocket, SIGNAL(onServerFileInfo(ClassWiredFile)), this, SLOT(onServerFileInfo(ClassWiredFile)) );

	connect(pWiredSocket, SIGNAL(onServerUserJoined(int,ClassWiredUser)), this, SLOT(userJoined(int,ClassWiredUser)) );
	connect(pWiredSocket, SIGNAL(onServerUserLeft(int,ClassWiredUser)), this, SLOT(userLeft(int,ClassWiredUser)) );
	connect(pWiredSocket, SIGNAL(onServerUserChanged(ClassWiredUser,ClassWiredUser)), this, SLOT(userChanged(ClassWiredUser,ClassWiredUser)) );
	connect(pWiredSocket, SIGNAL(onServerNewsPosted(QString, QString, QString)), this, SLOT(newsPosted(QString,QString,QString)) );
	connect(pWiredSocket, SIGNAL(fileTransferDone(const ClassWiredTransfer)), this, SLOT(transferDone(ClassWiredTransfer)) );
	connect(pWiredSocket, SIGNAL(fileTransferStarted(const ClassWiredTransfer)), this, SLOT(transferStarted(ClassWiredTransfer)) );
	connect(pWiredSocket, SIGNAL(fileTransferSocketError(QAbstractSocket::SocketError)), this, SLOT(transferSocketError(QAbstractSocket::SocketError)));
	
	// Main Window actions
	//
	connect( pConnWindow->actionNewConnection, SIGNAL(triggered()), this, SLOT(createNewConnection()) );
	connect( pConnWindow->actionNews, SIGNAL(triggered()), this, SLOT(getNews()) );
	connect( pConnWindow->actionServerInfo, SIGNAL(triggered(bool)), this, SLOT(do_show_serverinfo()) );
	connect( pConnWindow->actionBroadcast, SIGNAL(triggered(bool)), this, SLOT(do_new_broadcastmsg()) );
	connect( pConnWindow->actionFiles, SIGNAL(triggered(bool)), this, SLOT(do_new_filebrowser()) );
	connect( pConnWindow->actionPreferences, SIGNAL(triggered(bool)), this, SLOT(do_show_prefs()));
	//connect( pConnWindow->actionConnect, SIGNAL(triggered(bool)), this, SLOT(do_show_connect()));
	connect( pConnWindow->actionDisconnect, SIGNAL(triggered(bool)), this, SLOT(disconnectFromServer()));
	connect( pConnWindow->actionTransfers, SIGNAL(triggered(bool)), this, SLOT(showTransfers()) );
	connect( pConnWindow->actionSearch, SIGNAL(triggered(bool)), this, SLOT(showSearch()) );
	connect( pConnWindow->actionAccounts, SIGNAL(triggered(bool)), this, SLOT(showAccounts()) );
	connect( pConnWindow->actionTrackers, SIGNAL(triggered(bool)), this, SLOT(showTrackers()) );
	


	// Notification manager
	WiredSingleton *tmpS = &WSINGLETON::Instance();
	connect( tmpS, SIGNAL(prefsChanged()), this, SLOT(reloadPrefs()) );
	
}


/**
 * Display a message of the day.
 * @param text 
 */
void ClassWiredSession::displayMotd(const QString text) {
	if(!pWinMotd) {
		pWinMotd = new WidgetMotd();
		pWinMotd->setParent(pConnWindow, Qt::Window);
		pWinMotd->setMotd(text);
		int tmpIdx = pConnWindow->pTabWidget->addTab(pWinMotd, QIcon(), tr("MOTD"));
		pConnWindow->pTabWidget->setCurrentIndex(tmpIdx);
	} else {
		int tmpIdx = pConnWindow->pTabWidget->indexOf(pWinMotd);
		pConnWindow->pTabWidget->setCurrentIndex(tmpIdx);
	}

}



/**
 * Received information about a file (get info) from the server. Create a new window or
 * display the old window respectively.
 * @param theFile The information about the file.
 */
void ClassWiredSession::onServerFileInfo(ClassWiredFile theFile) {
	WidgetFileInfo *win=0;
	
	if(pFileInfos.contains(theFile.path)) {
		win = pFileInfos[theFile.path];
	}
	
	if(!win) {
		win = new WidgetFileInfo();
		win->setParent(pConnWindow, Qt::Window);
		pFileInfos[theFile.path] = win;
	}
	
	if(win) {
		win->loadFromFile(theFile);
		win->fName->setReadOnly(!pWiredSocket->sessionUser.privAlterFiles);
		win->fComments->setReadOnly(!pWiredSocket->sessionUser.privAlterFiles);
		win->show();
		win->raise();
	}
}


void ClassWiredSession::disconnectFromServer() {
	pWiredSocket->disconnectFromServer();
	pConnWindow->setEnabled(false);
}

// Enable/Disable GUI elements depending on the privileges
void ClassWiredSession::onSocketPrivileges(ClassWiredUser s) {
	pConnWindow->actionAccounts->setEnabled(s.privEditAccounts || s.privCreateAccounts || s.privDeleteAccounts);
}


void ClassWiredSession::getNews() {
	// Request the news from the server.
	
	if( !pWinNews ) {
		pWinNews = new WidgetNews();
		connect( pWiredSocket, SIGNAL(onServerNews(QString, QString, QString)), pWinNews, SLOT(addNewsItem(QString, QString, QString)) );
		connect( pWiredSocket, SIGNAL(onServerNewsPosted(QString, QString, QString)), pWinNews, SLOT(addFreshNewsItem(QString, QString, QString)) );
		
		connect( pWiredSocket, SIGNAL(onServerNewsDone()), pWinNews, SLOT(onServerNewsDone()) );
		connect( pWinNews, SIGNAL(doRefreshNews()), pWiredSocket, SLOT(getNews()) );
		connect( pWinNews, SIGNAL(doPostNews(QString)), pWiredSocket, SLOT(postNews(QString)) );
		connect( pWinNews, SIGNAL(onDeleteNews()), pWiredSocket, SLOT(deleteNews()) );
		
		// Display the widget using a Tab
		int tmpIdx = pConnWindow->pTabWidget->addTab(pWinNews, QIcon(), tr("News"));
		pConnWindow->pTabWidget->setCurrentIndex(tmpIdx);
		
		//pWinNews->show();
		pWiredSocket->getNews();
	} else {
		int tmpIdx = pConnWindow->pTabWidget->indexOf(pWinNews);
		pConnWindow->pTabWidget->setCurrentIndex(tmpIdx);
		//pWinNews->raise();
	}
}

void ClassWiredSession::do_handle_chat_message(int theChat, int theUserID, QString theText, bool theIsAction) {
	// A chat message arrived. Write it to the specific chat window.
	ClassWiredUser tmpUsr = pWiredSocket->getUserByID(theUserID); // Find the user
	qDebug() << "from:"<<theUserID;
	if( theChat==0 ) { // Public chat
		pMainChat->writeToChat(tmpUsr.pNick, theText, theIsAction);
		
		QStringList tmpParams;
		tmpParams << tmpUsr.pNick;
		if(theIsAction) tmpParams << QString("*** %1 %2").arg(tmpUsr.pNick).arg(theText);
			else tmpParams << theText;
		triggerEvent("ChatReceived", tmpParams);

	} else { // Handle private chat
		if(!pChats.contains(theChat)) return;
		WidgetForum *chat = pChats[theChat];
		chat->writeToChat(tmpUsr.pNick, theText, theIsAction);
		
		// Find the index on the tab panel
		int tmpIdx = pConnWindow->pTabWidget->indexOf(chat);
		if( tmpIdx>-1 && pConnWindow->pTabWidget->currentIndex()!=tmpIdx ) {
			pConnWindow->pTabWidget->setTabIcon( tmpIdx, QIcon(":/icons/tab-content.png") );
		}
		
	}
}



void ClassWiredSession::doHandleChatTopic(int theChatID, QString theNick, QString theLogin, QHostAddress theIP, QDateTime theDateTime, QString theTopic) {

}


// Handle user public chat input.
void ClassWiredSession::doHandlePublicChatInput(QString theText, bool theIsAction) {
	pWiredSocket->sendChat(1, theText, theIsAction);
}


/**
 * A new private message arrived. Handle the message.
 */
void ClassWiredSession::doHandlePrivMsg(ClassWiredUser theUser, QString theMessage) {
	if(!pWinMessages) {
		pWinMessages = new WidgetSendPrivMsg();
		pWinMessages->setParent(pMainChat, Qt::Window);
		pWinMessages->setSocket(pWiredSocket);
		int tmpIdx = pConnWindow->pTabWidget->addTab(pWinMessages, QIcon(), tr("Messages"));
		pConnWindow->pTabWidget->setCurrentIndex(tmpIdx);
	}
// 	else {
// 		int tmpIdx = pConnWindow->pTabWidget->indexOf(pWinMessages);
// 		pConnWindow->pTabWidget->setCurrentIndex(tmpIdx);
// 	}

	if(pWinMessages) {
		pWinMessages->handleNewMessage(theUser, theMessage);
	}	
	
// 	WidgetSendPrivMsg *msg;
// 	if(!pMsgWindows.contains(theUser.pUserID)) {
// 		// Create a new message dialog
// 		msg = new WidgetSendPrivMsg(pConnWindow);
// 		msg->setParent(pMainChat, Qt::Window);
// 		msg->pTargetID = theUser.pUserID;
// 		msg->fMsg->setReadOnly(true);
// 		msg->setWindowTitle( theUser.pNick);
// 		msg->setWindowIcon( theUser.iconAsPixmap() );
// 		connect( msg, SIGNAL(newMessageEntered(int,QString)), pWiredSocket, SLOT(sendMessage(int,QString)) );
// 		//msg->show();
// 	} else {
// 		// Recover the old window
// 		msg = pMsgWindows.value(theUser.pUserID);
// 		pMsgWindows[theUser.pUserID] = msg;
// 	}
// 
// 	// Write to the chat
// 	if(msg) {
// 		msg->addText(theMessage,1);
// 		if(!msg->isVisible()) {
// 			msg->show();
// 		}
// 	}

	QStringList tmpParams;
	tmpParams << theUser.pNick;
	tmpParams << theMessage;
	triggerEvent("MessageReceived", tmpParams);


}

// Handle a broadcast
void ClassWiredSession::doHandleBroadcast(ClassWiredUser theUser, QString theMessage) {
	Q_UNUSED(theUser)

	QStringList tmpParams;
	tmpParams << theUser.pNick;
	tmpParams << theMessage;
	triggerEvent("BroadcastReceived",tmpParams);
			
	WidgetSendPrivMsg *msg = new WidgetSendPrivMsg();
	msg->setParent(pMainChat, Qt::Window);
	msg->setWindowTitle(tr("Broadcast Message"));
	msg->fMsg->setReadOnly(true);
	msg->fMsg->setText(theMessage);
	msg->setSocket(pWiredSocket);
	msg->show();

	QStringList tmpParms;
	tmpParms << theUser.pNick;
	tmpParms << theMessage;
	triggerEvent("BroadcastReceived", tmpParms);
}

// Display received user info in a new window.
void ClassWiredSession::doHandleUserInfo(ClassWiredUser theUser){
	
	for(int i=0; i<pConnWindow->pTabWidget->count(); i++) {
		WidgetUserInfo *info = dynamic_cast<WidgetUserInfo*>(pConnWindow->pTabWidget->widget(i));
		if(info && info->pUserID==theUser.pUserID) { // Found existing window
			pConnWindow->pTabWidget->setCurrentIndex(i);
			info->setUser(theUser);
			return;
	} }
	
	// Not found, create a new window
	WidgetUserInfo *info = new WidgetUserInfo(pConnWindow);
	info->setUser(theUser);
	QPixmap tmpIcnPx;
 	tmpIcnPx.loadFromData(theUser.pImage);
	int tmpIdx = pConnWindow->pTabWidget->addTab(info, QIcon(tmpIcnPx), tr("Info: %1").arg(theUser.pNick));
	
	pConnWindow->pTabWidget->setCurrentIndex(tmpIdx);
	
}

// Received an invitation to a private chat.
void ClassWiredSession::doHandlePrivateChatInvitation(int theChatID, ClassWiredUser theUser) {
	QMessageBox messageBox(pConnWindow);
	messageBox.setWindowTitle(tr("Private Chat Invitation"));
	messageBox.setText(tr("%1 has invited you to a private chat.\nJoin to open a separate private chat with %1.").arg(theUser.pNick) );
	messageBox.setIconPixmap( QPixmap(":/icons/btn_chat.png") );
	QAbstractButton *ignoreButton = messageBox.addButton(tr("Ignore"), QMessageBox::DestructiveRole);
	QAbstractButton *rejectButton = messageBox.addButton(tr("Reject"), QMessageBox::RejectRole);
	QAbstractButton *joinButton = messageBox.addButton(tr("Join"), QMessageBox::AcceptRole);

	Q_UNUSED(ignoreButton)
	
	messageBox.exec();
	if (messageBox.clickedButton() == joinButton) {
		// Create a new chat
		doCreateNewChat(theChatID);
		pWiredSocket->joinChat(theChatID);
	} else if( messageBox.clickedButton() == rejectButton ) {
		// Reject the chat.
		pWiredSocket->rejectChat(theChatID);
	} else {
		// Ignore
	}
}

// Create a new private chat window and set the chat id on it.
void ClassWiredSession::doCreateNewChat(int theChatID) {
	WidgetForum *chat = new WidgetForum(pConnWindow);
	ModelUserList *model = new ModelUserList(chat);
	chat->pSession = this;
	chat->pChatID = theChatID;
	chat->setUserListModel(model);
	chat->fBtnChat->setVisible(false);		
	pChats[theChatID] = chat;
	
	int tmpIdx = pConnWindow->pTabWidget->addTab(chat, tr("Private Chat"));
	pConnWindow->pTabWidget->setCurrentIndex(tmpIdx);
	pConnWindow->pTabWidget->setTabIcon(tmpIdx, QIcon(":/icons/tab-idle.png"));
}

// Display the server information dialog.
void ClassWiredSession::do_show_serverinfo() {	
	if( !pServerWindow ) {
		pServerWindow = new WidgetServerInfo();
		pServerWindow->loadInfo(this);
		pConnWindow->pTabWidget->addTab(pServerWindow, tr("Server Info"));
	} else {
		pServerWindow->raise();
	}
}

// Open a new broadcast message dialog.
void ClassWiredSession::do_new_broadcastmsg() {
	WidgetSendPrivMsg *msg = new WidgetSendPrivMsg();
	msg->setParent(pConnWindow, Qt::Window);
// 	msg->setTarget ( this, 0 );
// 	msg->fTitle->setText(tr("Broadcast Message"));
	//msg->pTargetID = 0;
	msg->setSocket(pWiredSocket);
	//msg->move( pConnWindow->pos() );
	msg->show();
}


void ClassWiredSession::do_new_filebrowser(QString thePath) {
	// Open a new file browser and request the list of files (/) from the server.
	WidgetFileBrowser *browser = new WidgetFileBrowser();
	browser->setParent(pConnWindow, Qt::Window);
	browser->move( pConnWindow->pos() );
	browser->initWithConnection(this);
	browser->setPath(thePath);
	browser->pModel->pWaitingForList = true;
	browser->show();
	
	// Request the root list of files
	 pWiredSocket->getFileList(thePath);

}

void ClassWiredSession::do_show_prefs() {
	if( pPrefsWindow==0 ) {
		pPrefsWindow = new WidgetPrefs();
		pPrefsWindow->setParent(pConnWindow, Qt::Window);
		//int tmpIdx = pConnWindow->pTabWidget->addTab(pPrefsWindow, QIcon(), tr("Preferences"));
		//pConnWindow->pTabWidget->setCurrentIndex(tmpIdx);
 		pPrefsWindow->move(pConnWindow->pos() );
 		pPrefsWindow->show();
	} else {
		pPrefsWindow->raise();
	}
}

// Status update on 200 Server Information
void ClassWiredSession::onSocketServerInfo() {
	if( pConnectWindow>0 )
		pConnectWindow->setProgressBar(1,3);

	if(pTrayMenuItem) {
		pTrayMenuItem->setTitle(pWiredSocket->pServerName);

	}
	
}

// Status update on successful login
void ClassWiredSession::onSocketLoginSuccessful() {
	if( pConnectWindow>0 ) {
		if(pConnectWindow) pConnectWindow->hide();
		pConnWindow->setCentralWidget(pConnWindow->pTabWidget);
		pConnWindow->pTabWidget->addTab(pMainChat, "Chat");
		pConnWindow->pTabWidget->setVisible(true);
		//pMainChat->show();
		enableConnToolButtons(true);

		triggerEvent("ServerConnected",QStringList());
	}
}

// Login attempt failed.
void ClassWiredSession::onSocketLoginFailed() {
	pConnectWindow->setEnabled(true);
	pConnectWindow->setProgressBar(0,0);
	enableConnToolButtons(false);
}

// Connect to the remote server.
void ClassWiredSession::onDoConnect(QString theHost, QString theLogin, QString thePassword) {
	if(theLogin=="") pWiredSocket->setUserAccount("guest","");
		else pWiredSocket->setUserAccount(theLogin,thePassword);
	pWiredSocket->connectToWiredServer(theHost);
	pConnectWindow->setEnabled(false);
}

void ClassWiredSession::onSocketError(QString theErrorReason, int theError) {
	if( pConnectWindow>0 ) {
		// Error occoured while connecting from the connecting window
		pConnectWindow->setEnabled(true);
		pConnectWindow->setProgressBar(0,0);
	} else {
		// Disconnected suddenly
		pConnWindow->setEnabled(false);
		pConnWindow->setWindowTitle( pConnWindow->windowTitle()+tr(" [Disconnected]") );
		
		triggerEvent("ServerDisconnected",QStringList());
	}
	enableConnToolButtons(false);
	QMessageBox::critical(pMainChat, tr("Socket Error"), tr("A socket error occoured.\nReason: %1 (%2).").arg(theErrorReason).arg(theError));
}

void ClassWiredSession::showTransfers() {
	// Display the transfers window
	if( !pTranfersWindow ) {
		pTranfersWindow = new WidgetTransfers(pConnWindow);
		int tmpIdx = pConnWindow->pTabWidget->addTab(pTranfersWindow, QIcon(), tr("Transfers"));
		pConnWindow->pTabWidget->setCurrentIndex(tmpIdx);
		// Model
		ModelFileTransfers *tmpModel = new ModelFileTransfers(pTranfersWindow->fTransfers);
		tmpModel->setSocket(pWiredSocket);
		pTranfersWindow->fTransfers->setModel(tmpModel);
		pTranfersWindow->init();
		connect(pTranfersWindow, SIGNAL(transferCancelled(ClassWiredTransfer)), pWiredSocket, SLOT(cancelTransfer(ClassWiredTransfer)) );
		
	} else {
		int tmpIdx = pConnWindow->pTabWidget->indexOf(pTranfersWindow);
		pConnWindow->pTabWidget->setCurrentIndex(tmpIdx);
	}
}

void ClassWiredSession::showSearch() {
	if(!pFileSearch) {
		pFileSearch = new WidgetFileSearch(pConnWindow);
		int tmpIdx = pConnWindow->pTabWidget->addTab(pFileSearch, tr("File Search"));
		pConnWindow->pTabWidget->setCurrentIndex(tmpIdx);
		connect( pWiredSocket, SIGNAL(fileSearchDone(QList<ClassWiredFile>)), pFileSearch, SLOT(updateResults(QList<ClassWiredFile>)) );
		connect( pFileSearch, SIGNAL(search(QString)), pWiredSocket, SLOT(searchFiles(QString)) );
		connect( pFileSearch, SIGNAL(downloadFile(QString)), this, SLOT(search_download_file(QString)) );
		connect( pFileSearch, SIGNAL(revealFile(QString)), this, SLOT(search_reveal_file(QString)) );
	} else {
		int tmpIdx = pConnWindow->pTabWidget->indexOf(pFileSearch);
		pConnWindow->pTabWidget->setCurrentIndex(tmpIdx);
	}
}

void ClassWiredSession::showAccounts() {
	if(!pWinAccounts) {
		pWinAccounts = new WidgetAccounts(pConnWindow);
		int tmpIdx = pConnWindow->pTabWidget->addTab(pWinAccounts, tr("Accounts"));
		pConnWindow->pTabWidget->setCurrentIndex(tmpIdx);
		connect( pWiredSocket, SIGNAL(usersListingDone(QStringList)), pWinAccounts, SLOT(appendUserNames(QStringList)) );
		connect( pWiredSocket, SIGNAL(groupsListingDone(QStringList)), pWinAccounts, SLOT(appendGroupNames(QStringList)) );
		connect( pWiredSocket, SIGNAL(userSpecReceived(ClassWiredUser)), pWinAccounts, SLOT(loadUserSpec(ClassWiredUser)) );
		connect( pWiredSocket, SIGNAL(groupSpecReceived(ClassWiredUser)), pWinAccounts, SLOT(loadGroupSpec(ClassWiredUser)) );
		connect( pWinAccounts, SIGNAL(userSpecRequested(QString)), pWiredSocket, SLOT(readUser(QString)) );
		connect( pWinAccounts, SIGNAL(groupSpecRequested(QString)), pWiredSocket, SLOT(readGroup(QString)) );
		connect( pWinAccounts, SIGNAL(userDeleted(QString)), pWiredSocket, SLOT(deleteUser(QString)) );
		connect( pWinAccounts, SIGNAL(groupDeleted(QString)), pWiredSocket, SLOT(deleteGroup(QString)) );

		connect( pWinAccounts, SIGNAL(createUser(ClassWiredUser)), pWiredSocket, SLOT(createUser(ClassWiredUser)) );
		connect( pWinAccounts, SIGNAL(editUser(ClassWiredUser)), pWiredSocket, SLOT(editUser(ClassWiredUser)) );
		connect( pWinAccounts, SIGNAL(createGroup(ClassWiredUser)), pWiredSocket, SLOT(createGroup(ClassWiredUser)) );
		connect( pWinAccounts, SIGNAL(editGroup(ClassWiredUser)), pWiredSocket, SLOT(editGroup(ClassWiredUser)) );
		pWiredSocket->getGroups();
		pWiredSocket->getUsers();
	} else {
		int tmpIdx = pConnWindow->pTabWidget->indexOf(pWinAccounts);
		pConnWindow->pTabWidget->setCurrentIndex(tmpIdx);
	}
}

// Enable/Disable connection-related toolbar items (true if connected)
void ClassWiredSession::enableConnToolButtons(bool theEnable) {
	pConnWindow->actionReconnect->setEnabled(!theEnable);
	pConnWindow->actionDisconnect->setEnabled(theEnable);
	pConnWindow->actionServerInfo->setEnabled(theEnable);
	pConnWindow->actionChat->setEnabled(theEnable);
	pConnWindow->actionNews->setEnabled(theEnable);
	pConnWindow->actionFiles->setEnabled(theEnable);
	pConnWindow->actionTransfers->setEnabled(theEnable);
	pConnWindow->actionAccounts->setEnabled(theEnable);
	pConnWindow->actionInformation->setEnabled(theEnable);
	pConnWindow->actionNewsPost->setEnabled(theEnable);
	pConnWindow->actionBroadcast->setEnabled(theEnable);
	pConnWindow->actionSearch->setEnabled(theEnable);
}

// Set the banner image to the toolbar of the main window.
void ClassWiredSession::setBannerView(const QPixmap theBanner) {
	QWidget *tmpSpace = new QWidget(pConnWindow->fToolBar);
	tmpSpace->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
	pConnWindow->fToolBar->addWidget(tmpSpace);
	QLabel *tmpView = new QLabel(pConnWindow->fToolBar);
	tmpView->setPixmap(theBanner);
	pConnWindow->fToolBar->addWidget(tmpView);

	QWidget *tmpSpace2 = new QWidget(pConnWindow->fToolBar);
	tmpSpace2->setFixedWidth(10);
	pConnWindow->fToolBar->addWidget(tmpSpace2);

}

// Connected to the download button in the search dialog.
void ClassWiredSession::search_download_file(QString thePath) {
	QSettings settings;
	showTransfers();
	QString tmpName = thePath.section("/",-1,-1);
	
	QDir tmpDownloadFolder( settings.value("files/download_dir", QDir::homePath()).toString() );
	pWiredSocket->getFile( thePath, tmpDownloadFolder.absoluteFilePath(tmpName) );;
}

void ClassWiredSession::search_reveal_file(QString thePath) {
	do_new_filebrowser(thePath.section("/",0,-2));
}

void ClassWiredSession::reloadPrefs() {
	qDebug() << "Reloading prefs.....";
	
	QSettings s;
	if(pWiredSocket->sessionUser.pNick!=s.value("general/nickname", "Unnamed").toString())
		pWiredSocket->setNickname(s.value("general/nickname").toString());

	if(pWiredSocket->sessionUser.pStatus!=s.value("general/status", "Qwired Newbie").toString())
		pWiredSocket->setUserStatus(s.value("general/status").toString());

	QPixmap tmpNew = s.value("general/icon", QPixmap()).value<QPixmap>();
	pWiredSocket->setUserIcon(tmpNew);

}

void ClassWiredSession::handleErrorOccoured(int theError) {
	QString tmpError(tr("An unknown server error occoured. The error code is %1.").arg(theError));
	switch(theError) {
		case 500: tmpError=tr("Command Failed. An undefined internal server error has occoured."); break;
		case 501: tmpError=tr("Command Not Recognized. Qwired sent a command that is unknown by the server."); break;
		case 502: tmpError=tr("Command Not Implemented. The last command is not implemented by the server."); break;
		case 503: tmpError=tr("Syntax Error. The last command was not formatted correctly."); break;
		case 510: tmpError=tr("Login Failed. Username and password were not accepted by the server."); break;
		case 511: tmpError=tr("Banned. You have been banned from the server. Please try connecting later."); break;
		case 512: tmpError=tr("Client Not Found. The server could not find the client referred to."); break;
		case 513: tmpError=tr("Account Not Found. The server could not find the account referred to."); break;
		case 514: tmpError=tr("Account Exists. Could not create the account you specified."); break;
		case 515: tmpError=tr("User can not be disconnected. The specified user can not be disconnected."); break;
		case 516: tmpError=tr("Permission Denied. You don't have sufficient privileges to execute the last command."); break;
		case 520: tmpError=tr("File or Directory not found. The last command could not be completed because the file or directory could not be found."); break;
		case 521: tmpError=tr("The last command could not be completed because the file or directory already exists."); break;
		case 522: tmpError=tr("Checksum Mismatch.");
		case 523: tmpError=tr("Queue Limit Exceeded. Could not complete the last command because the server queue is full."); break;
	}
	
	QStringList tmpParams;
	tmpParams << tmpError;
	triggerEvent("ServerError",tmpParams);
	
	QMessageBox::critical(pConnWindow, tr("Server Error"), tmpError);
}

void ClassWiredSession::showTrackers() {
	if(!pWinTrackers) {
		pWinTrackers = new WidgetTracker();
		pWinTrackers->setParent(pConnWindow, Qt::Window);
		pWinTrackers->show();
	} else {
		pWinTrackers->raise();
	}
}

void ClassWiredSession::setTrayMenuAction(QMenu *action) {
	pTrayMenuItem = action;
	pTrayMenuItem->setTitle(pConnWindow->windowTitle());

// 	pTrayMenuInfoLine = new QAction(this);
// 	pTrayMenuInfoLine->setText(tr("Loading..."));
// 	pTrayMenuInfoLine->setEnabled(false);
// 	pTrayMenuItem->addAction(pTrayMenuInfoLine);

	QAction* tmpShowHide = new QAction(QIcon(":icons/icn_showhide.png"), tr("Show/Hide"), this );
	connect(tmpShowHide, SIGNAL(triggered(bool)), pConnWindow, SLOT(toggleVisible()) );
	
	
	pTrayMenuItem->addAction(pConnWindow->actionNews);
	pTrayMenuItem->addAction(pConnWindow->actionFiles);
	pTrayMenuItem->addAction(pConnWindow->actionTransfers);
	pTrayMenuItem->addAction(pConnWindow->actionSearch);
	pTrayMenuItem->addAction(pConnWindow->actionDisconnect);
	pTrayMenuItem->addAction(tmpShowHide);
}


/**
 * Handle a specific event from the current session.
 * @param event The event that occoured
 * @param parameters A list of parameters which describe detailed information about the event.
 */
void ClassWiredSession::triggerEvent(QString event, QStringList params) {
	QSettings conf;
	QString tmpChatMsg;

	QHash<QString,QString> tmpMsgs;
	tmpMsgs["ServerConnected"] = tr("Connected");
	tmpMsgs["ServerDisconnected"] = tr("Disconnected");
	tmpMsgs["ServerError"] = tr("Server Error: %1");
	tmpMsgs["UserJoined"] = tr("%1 joined");
	tmpMsgs["UserChangedNick"] = tr("%1 is now known as %2");
	tmpMsgs["UserChangedStatus"] = tr("%1 changed status to '%2'");
	tmpMsgs["UserLeft"] = tr("%1 left");
	tmpMsgs["ChatReceived"] = tr("Chat from %1:\n%2"); // tray msg only
	tmpMsgs["MessageReceived"] = tr("Message from %1:\n%2"); // tray msg only
	tmpMsgs["NewsPosted"] = tr("News Posted from %1:\n%2"); // tray msg only
	tmpMsgs["BroadcastReceived"] = tr("Broadcast from %1:\n%2"); // tray msg only
	tmpMsgs["TransferStarted"] = tr("Transfer Started: %1");
	tmpMsgs["TransferFinished"] = tr("Transfer Finished: %1");
	QString tmpMsg = tmpMsgs[event].arg(params.value(0)).arg(params.value(1)).arg(params.value(2));
	
	if(conf.value(QString("events/%1/chat").arg(event)).toBool())
		pMainChat->writeEventToChat(tmpMsg);
	
	if(conf.value(QString("events/%1/traymsg").arg(event)).toBool()) {
		WiredSingleton *tmpS = &WSINGLETON::Instance();
		tmpS->pTrayIcon->showMessage( pWiredSocket->pServerName, tmpMsg );
	}
	
	if(conf.contains(QString("events/%1/sound").arg(event)) ) {
//		if(pEventMediaObject) {
//			pEventMediaObject->setCurrentSource(conf.value(QString("events/%1/sound").arg(event)).toString());
//			pEventMediaObject->play();
//		}
		
		//qDebug() << conf.value(QString("events/%1/sound").arg(event)).toString();
		//QSound::play( conf.value(QString("events/%1/sound").arg(event)).toString() );
	}

	if(conf.value(QString("events/%1/syscmd_enabled").arg(event), false).toBool()) {
		QString tmpCmd = conf.value(QString("events/%1/syscmd").arg(event)).toString();
		if(!tmpCmd.isEmpty()) {
			QProcess tmpProc;
			tmpProc.start(tmpCmd);
		}
	}
	
	
}

void ClassWiredSession::userJoined(int theChat, ClassWiredUser theUser) {
	if(theChat!=0) return;
	QStringList tmpParams; tmpParams << theUser.pNick;
	triggerEvent("UserJoined", tmpParams);
}

void ClassWiredSession::userLeft(int theChat, ClassWiredUser theUser) {
	if(theChat!=0) return;
	QStringList tmpParams; tmpParams << theUser.pNick;
	triggerEvent("UserLeft", tmpParams);
}

void ClassWiredSession::userChanged(ClassWiredUser theOld, ClassWiredUser theNew) {
	if(theOld.pNick != theNew.pNick) {
		QStringList tmpParams;
		tmpParams << theOld.pNick;
		tmpParams << theNew.pNick;
		triggerEvent("UserChangedNick", tmpParams);
	}

	if(theOld.pStatus != theNew.pStatus) {
		QStringList tmpParams;
		tmpParams << theNew.pNick;
		tmpParams << theNew.pStatus;
		triggerEvent("UserChangedStatus", tmpParams);
	}

	
}

void ClassWiredSession::newsPosted(QString theNick, QString, QString thePost) {
	QStringList tmpParams;
	tmpParams << theNick;
	tmpParams << thePost;
	triggerEvent("NewsPosted", tmpParams);
}

void ClassWiredSession::transferStarted(ClassWiredTransfer transfer) {
	QStringList tmpParams;
	tmpParams << transfer.fileName();
	triggerEvent("TransferStarted", tmpParams);
}

void ClassWiredSession::transferDone(ClassWiredTransfer transfer) {
	QStringList tmpParams;
	tmpParams << transfer.fileName();
	triggerEvent("TransferFinished", tmpParams);
}

void ClassWiredSession::transferSocketError(QAbstractSocket::SocketError error) {
	QStringList tmpParams;
	tmpParams << tr("The file transfer failed due to a connection error. Error ID is: %1").arg(error);
	triggerEvent("ServerError",tmpParams);
}

/**
 * A chat changed, reflect the changes to the user.
 */
void ClassWiredSession::handleConferenceChanged(const int & cid, const QString & topic, const int & users, const bool & chat_protected, const int & type) {
	qDebug() << "Topic:"<<cid<<topic;
	if( cid==0 ) { // Public chat
		pMainChat->fTopic->setText(tr("Topic: %1").arg(topic));
	} else if( pChats.contains(cid) ) { // Topic of private chat
		WidgetForum *chat = pChats[cid];
		chat->fTopic->setText(tr("Topic: %1").arg(topic));
	}
}
