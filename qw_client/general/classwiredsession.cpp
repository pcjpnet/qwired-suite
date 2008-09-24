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

#include "gui/widgetconnect.h"
#include "gui/modelfiletransfers.h"

#include "gui/widgetfilebrowser.h"

#include <QMessageBox>
#include <QtGui>



ClassWiredSession::ClassWiredSession(QObject *parent)
	: QObject(parent)
{
	initWiredSocket();
	initMainWindow();
	setConnectionToolButtonsEnabled(false);
	reloadPreferences();
}

void ClassWiredSession::initMainWindow()
{
	pConnWindow = new ConnWindow();
	connect(pConnWindow, SIGNAL(destroyed(QObject*)), this, SLOT(connectionWindowDestroyed(QObject*)) );
	pConnWindow->show();
	
	pMainChat = new WidgetForum();
	pMainChat->setContentsMargins(0,0,0,0);

	// Connect window
	pConnectWindow = new WidgetConnect();
	connect(pConnectWindow, SIGNAL(onConnnectReady(QString,QString,QString)), this, SLOT(onDoConnect(QString,QString,QString)) );
	connect(pConnectWindow, SIGNAL(onConnectAborted()), this, SLOT(onConnectAborted()));

	// Set up the container widget
 	pContainerWidget = new QWidget();
	pContainerLayout = new QStackedLayout(pContainerWidget);
	pContainerWidget->setContentsMargins(6, 6, 6, 6);
 	pContainerWidget->setLayout(pContainerLayout);

	// Create the tab bar
	pMainTabWidget = new QTabWidget(pContainerWidget);
	pMainTabWidget->clear();
	pMainTabWidget->setVisible(false);
	connect(pMainTabWidget, SIGNAL(currentChanged(int)),
			this, SLOT(onTabBarCurrentChanged(int)) );

	// The tab bar's close button, too
	QToolButton *tmpTabCloseButton = new QToolButton(pMainTabWidget);
	tmpTabCloseButton->setIcon(QIcon(":/icons/icn_close.png"));
	tmpTabCloseButton->setShortcut(QKeySequence("ctrl+w"));
	tmpTabCloseButton->setEnabled(false);
	connect( tmpTabCloseButton, SIGNAL(clicked()), this, SLOT(onTabBarCloseButtonClicked()) );
	pMainTabWidget->setCornerWidget(tmpTabCloseButton);

	// Connection window/Forum
	pUserListModel = new ModelUserList(pMainChat);
	pUserListModel->setWiredSocket(pWiredSocket);
	pMainChat->setSession(this);
	pMainChat->setUserListModel(pUserListModel);
	
	// Add the widgets to the stacked layout
	pContainerWidget->layout()->addWidget(pConnectWindow);
	pContainerWidget->layout()->addWidget(pMainTabWidget);
	
	// Set the virtual widget
 	pConnWindow->setCentralWidget(pContainerWidget);
	pConnWindow->show();

	setupConnections();
}


/// The user list was completely received after connecting.
void ClassWiredSession::onUserlistComplete(int chatId)
{
	if(chatId!=1 && pContainerLayout->currentIndex()!=0) return;
	pContainerLayout->setCurrentIndex(1);
	setConnectionToolButtonsEnabled(true);
}


/// Tab bar close button clicked. Close the current widget.
void ClassWiredSession::onTabBarCloseButtonClicked()
{
	int tmpIdx = pMainTabWidget->currentIndex();
	if(!tmpIdx) return;
	pMainTabWidget->currentWidget()->close();
	pMainTabWidget->removeTab(tmpIdx);
}


void ClassWiredSession::onTabBarCurrentChanged(int index)
{
	QWidget *tmpBtn = pMainTabWidget->cornerWidget();
	tmpBtn->setEnabled(index>0);
	
	// Icon removal for private chats
	QWidget *tmpWid = pMainTabWidget->widget(index);
	WidgetForum *tmpChat = qobject_cast<WidgetForum*>(tmpWid);
	if(tmpChat && tmpChat->pChatID!=1) {
		pMainTabWidget->setTabIcon(index, QIcon(":/icons/tab-idle.png"));
	}
}


ClassWiredSession::~ClassWiredSession()
{
	pConnWindow->deleteLater();
	pContainerWidget->deleteLater();
	pMainChat->deleteLater();

}


/// Called when the connection window is destroyed/closed. Allows the manager class to do
/// some clean up work.
void ClassWiredSession::connectionWindowDestroyed(QObject*) {
	this->deleteLater();
}




/// Set up connections between objects in this class.
void ClassWiredSession::setupConnections() {
	// Socket connections
	//
	connect(pWiredSocket, SIGNAL(onServerChat(int,int,QString,bool)), this, SLOT(do_handle_chat_message(int,int,QString,bool)) );
	connect(pWiredSocket, SIGNAL(onChatTopic(int, QString, QString, QHostAddress, QDateTime, QString)),
			  this,   SLOT(doHandleChatTopic(int, QString, QString, QHostAddress, QDateTime, QString)) );
	connect(pWiredSocket, SIGNAL(onPrivateMessage(ClassWiredUser,QString)), this, SLOT(doHandlePrivMsg(ClassWiredUser,QString)) );
	connect(pWiredSocket, SIGNAL(onServerBroadcast(ClassWiredUser,QString)), this, SLOT(doHandleBroadcast(ClassWiredUser,QString)) );
	connect(pWiredSocket, SIGNAL(onServerLoginSuccessful()), this, SLOT(onLoginSuccessful()) );

	connect(pWiredSocket, SIGNAL(onServerUserlistDone(int)), this, SLOT(onUserlistComplete(int)) );
	
	connect(pWiredSocket, SIGNAL(onServerUserInfo(ClassWiredUser)), this, SLOT(doHandleUserInfo(ClassWiredUser)) );
	connect(pWiredSocket, SIGNAL(onServerPrivateChatInvitation(int,ClassWiredUser)), this, SLOT(doHandlePrivateChatInvitation(int,ClassWiredUser)) );
	connect(pWiredSocket, SIGNAL(onServerPrivateChatCreated(int)), this, SLOT(doCreateNewChat(int)) );
	
	connect(pWiredSocket, SIGNAL(onSocketError(QAbstractSocket::SocketError)), this, SLOT(onSocketError(QAbstractSocket::SocketError)) );
	connect(pWiredSocket, SIGNAL(onServerInformation()), this, SLOT(onSocketServerInfo()) );
	
		
	connect(pWiredSocket, SIGNAL(onServerBanner(QPixmap)), this, SLOT(setBannerView(QPixmap)) );
	connect(pWiredSocket, SIGNAL(errorOccoured(int)), this, SLOT(handleErrorOccoured(int)) );
	connect(pWiredSocket, SIGNAL(errorLoginFailed()), this, SLOT(onSocketLoginFailed()) );
	connect(pWiredSocket, SIGNAL(userPrivileges(ClassWiredUser)), this, SLOT(onSocketPrivileges(ClassWiredUser)) );
	connect(pWiredSocket, SIGNAL(onServerFileInfo(ClassWiredFile)), this, SLOT(onServerFileInfo(ClassWiredFile)) );

	connect(pWiredSocket, SIGNAL(onServerUserJoined(int,ClassWiredUser)), this, SLOT(userJoined(int,ClassWiredUser)) );
	connect(pWiredSocket, SIGNAL(onServerUserLeft(int,ClassWiredUser)), this, SLOT(userLeft(int,ClassWiredUser)) );
	connect(pWiredSocket, SIGNAL(onServerUserChanged(ClassWiredUser,ClassWiredUser)), this, SLOT(userChanged(ClassWiredUser,ClassWiredUser)) );
	connect(pWiredSocket, SIGNAL(onServerNewsPosted(QString, QString, QString)), this, SLOT(newsPosted(QString,QString,QString)) );

	// File transfer signals
	connect(pWiredSocket, SIGNAL(fileTransferDone(const ClassWiredTransfer)), this, SLOT(transferDone(ClassWiredTransfer)) );
	connect(pWiredSocket, SIGNAL(fileTransferStarted(const ClassWiredTransfer)), this, SLOT(transferStarted(ClassWiredTransfer)) );
	connect(pWiredSocket, SIGNAL(fileTransferSocketError(QAbstractSocket::SocketError)), this, SLOT(transferSocketError(QAbstractSocket::SocketError)));
	
	// Main Window actions
	//
	connect( pConnWindow->actionNewConnection, SIGNAL(triggered()), this, SLOT(doActionNewConnection()) );

	connect( pConnWindow->actionDisconnect, SIGNAL(triggered(bool)), this, SLOT(doActionDisconnect()));
	connect( pConnWindow->actionAccounts, SIGNAL(triggered(bool)), this, SLOT(doActionAccounts()) );
	connect( pConnWindow->actionNews, SIGNAL(triggered()), this, SLOT(doActionNews()) );
	connect( pConnWindow->actionServerInfo, SIGNAL(triggered(bool)), this, SLOT(doActionServerInfo()) );
	connect( pConnWindow->actionBroadcast, SIGNAL(triggered(bool)), this, SLOT(doActionBroadcast()) );
	connect( pConnWindow->actionFiles, SIGNAL(triggered(bool)), this, SLOT(doActionFiles()) );
	connect( pConnWindow->actionPreferences, SIGNAL(triggered(bool)), this, SLOT(doActionPreferences()));
	connect( pConnWindow->actionTrackers, SIGNAL(triggered(bool)), this, SLOT(doActionTrackers()) );
	connect( pConnWindow->actionSearch, SIGNAL(triggered(bool)), this, SLOT(doActionFileSearch()) );
	connect( pConnWindow->actionTransfers, SIGNAL(triggered(bool)), this, SLOT(doActionTransfers()) );
	//connect( pConnWindow->actionConnect, SIGNAL(triggered(bool)), this, SLOT(do_show_connect()));

	// Notification manager
	//
	WiredSingleton *tmpS = &WSINGLETON::Instance();
	connect( tmpS, SIGNAL(prefsChanged()), this, SLOT(reloadPreferences()) );
	
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



// Enable/Disable GUI elements depending on the privileges
void ClassWiredSession::onSocketPrivileges(ClassWiredUser s) {
	pConnWindow->actionAccounts->setEnabled(s.privEditAccounts || s.privCreateAccounts || s.privDeleteAccounts);
}





/// A chat message was received, handle it.
void ClassWiredSession::do_handle_chat_message(int theChat, int theUserID, QString theText, bool theIsAction) {
	ClassWiredUser tmpUsr = pWiredSocket->getUserByID(theUserID); // Find the user
	if(theChat==1) {
		// Public chat
		pMainChat->writeToChat(tmpUsr.pNick, theText, theIsAction);
		
		// Trigger the event
		QStringList tmpParams;
		tmpParams << tmpUsr.pNick;
		if(theIsAction) tmpParams << QString("*** %1 %2").arg(tmpUsr.pNick).arg(theText);
			else tmpParams << theText;
		triggerEvent("ChatReceived", tmpParams);

	} else {
		// Handle private chat
		if(!pChats.contains(theChat)) {
			qDebug() << "ClassWiredSession: Warning: Unknown chat with id"<<theChat;
			return;
		}
		WidgetForum *chat = pChats[theChat];
		chat->writeToChat(tmpUsr.pNick, theText, theIsAction);
		
		// Find the index on the tab panel
		int tmpIdx = pMainTabWidget->indexOf(chat);
		if( tmpIdx>-1 && pMainTabWidget->currentIndex()!=tmpIdx )
			pMainTabWidget->setTabIcon( tmpIdx, QIcon(":/icons/tab-content.png") );
	}
}


/// The chat topic was changed or received. Update the chat window.
void ClassWiredSession::doHandleChatTopic(int theChatID, QString theNick, QString theLogin, QHostAddress theIP, QDateTime theDateTime, QString theTopic) {
	Q_UNUSED(theLogin);
	Q_UNUSED(theIP);
	if( theChatID==1 ) {
		// Public chat
		pMainChat->fTopic->setText(tr("Topic: %1\nSet By: %2 --- %3").arg(theTopic).arg(theNick).arg(theDateTime.toString()));
	} else if(pChats.contains(theChatID)) {
		// Topic of private chat
		WidgetForum *chat = pChats[theChatID];
		chat->fTopic->setText(tr("Topic: %1\nSet By: %2 --- %3").arg(theTopic).arg(theNick).arg(theDateTime.toString()));
	}
}


// Handle user public chat input.
void ClassWiredSession::doHandlePublicChatInput(QString theText, bool theIsAction) {
	pWiredSocket->sendChat(1, theText, theIsAction);
}


/// A private message was received. Display it to the user.
void ClassWiredSession::doHandlePrivMsg(ClassWiredUser theUser, QString theMessage) {
	WidgetSendPrivMsg *msg;
	if(!pMsgWindows.contains(theUser.pUserID)) {
		// Create a new message dialog
		msg = new WidgetSendPrivMsg(pConnWindow);
		msg->setParent(pMainChat, Qt::Window);
		msg->pTargetID = theUser.pUserID;
		msg->fMsg->setReadOnly(true);
		msg->setWindowTitle( theUser.pNick);
		msg->setWindowIcon( theUser.iconAsPixmap() );
		connect( msg, SIGNAL(newMessage(int,QString)), pWiredSocket, SLOT(sendPrivateMessage(int,QString)) );
	} else {
		// Recover the old window
		msg = pMsgWindows.value(theUser.pUserID);
		pMsgWindows[theUser.pUserID] = msg;
	}

	// Write to the chat
	if(msg) {
		msg->addText(theMessage,1);
		if(!msg->isVisible()) {
			msg->show();
		}
	}

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
	msg->show();

	QStringList tmpParms;
	tmpParms << theUser.pNick;
	tmpParms << theMessage;
	triggerEvent("BroadcastReceived", tmpParms);
}

// Display received user info in a new window.
void ClassWiredSession::doHandleUserInfo(ClassWiredUser theUser){
	
	for(int i=0; i<pMainTabWidget->count(); i++) {
		WidgetUserInfo *info = dynamic_cast<WidgetUserInfo*>(pMainTabWidget->widget(i));
		if(info && info->pUserID==theUser.pUserID) { // Found existing window
			pMainTabWidget->setCurrentIndex(i);
			info->setUser(theUser);
			return;
	} }
	
	// Not found, create a new window
	WidgetUserInfo *info = new WidgetUserInfo(pConnWindow);
	info->setUser(theUser);
	QPixmap tmpIcnPx;
 	tmpIcnPx.loadFromData(theUser.pImage);
	int tmpIdx = pMainTabWidget->addTab(info, QIcon(tmpIcnPx), tr("Info: %1").arg(theUser.pNick));
	
	pMainTabWidget->setCurrentIndex(tmpIdx);
	
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
	chat->setSession(this);
	chat->pChatID = theChatID;
	chat->setUserListModel(model);
	chat->fBtnChat->setVisible(false);		
	pChats[theChatID] = chat;
	
	int tmpIdx = pMainTabWidget->addTab(chat, tr("Private Chat"));
	pMainTabWidget->setCurrentIndex(tmpIdx);
	pMainTabWidget->setTabIcon(tmpIdx, QIcon(":/icons/tab-idle.png"));
}










/// Received the server info header. Update the connect window.
void ClassWiredSession::onSocketServerInfo() {
	pConnectWindow->setStatus(tr("Connecting. Starting session..."));
	if(pConnectWindow>0) pConnectWindow->setProgressBar(1,3);
	if(pTrayMenuItem) pTrayMenuItem->setTitle(pWiredSocket->pServerName);
}


/// Login attempt failed.
void ClassWiredSession::onSocketLoginFailed()
{
	pConnectWindow->resetForm();
	pContainerLayout->setCurrentIndex(0);
	setConnectionToolButtonsEnabled(false);
}


/// Connect to the remote server.
void ClassWiredSession::onDoConnect(QString theHost, QString theLogin, QString thePassword) {
	if(theLogin=="") pWiredSocket->setUserAccount("guest","");
		else pWiredSocket->setUserAccount(theLogin,thePassword);
	pWiredSocket->connectToWiredServer(theHost);
	//pConnectWindow->setEnabled(false);
}


/// A connection error occoured.
void ClassWiredSession::onSocketError(QAbstractSocket::SocketError error) {
	if(error == QAbstractSocket::ConnectionRefusedError) {
		// Error occoured while connecting from the connecting window
		pConnectWindow->resetForm();
		QMessageBox::critical(pMainChat, tr("Connection Refused"), tr("The connection was refused by the remote host. "
				"This normally means that there is no Wired server running on the remote machine."));
	} else if(error == QAbstractSocket::HostNotFoundError) {
		pConnectWindow->resetForm();
		QMessageBox::critical(pMainChat, tr("Host not found"), tr("Could not connect to the remote server because the host name could not be resolved."));
	} else {
		// Disconnected suddenly
		qDebug() << error;
		pContainerLayout->setCurrentIndex(0);
		pConnectWindow->resetForm();
		pConnectWindow->startReconnecting();
		
		//pConnWindow->setEnabled(false);
		//pConnWindow->setWindowTitle( pConnWindow->windowTitle()+tr(" [Disconnected]") );
		
		triggerEvent("ServerDisconnected",QStringList());
	}
	setConnectionToolButtonsEnabled(false);
	
}

/// Enable/Disable connection-related toolbar items (true if connected)
void ClassWiredSession::setConnectionToolButtonsEnabled(bool theEnable) {
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


/// Set the banner image to the toolbar of the main window.
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

/// Connected to the download button in the search dialog.
void ClassWiredSession::search_download_file(QString thePath) {
	QSettings settings;
	doActionTransfers();
	QString tmpName = thePath.section("/",-1,-1);
	
	QDir tmpDownloadFolder( settings.value("files/download_dir", QDir::homePath()).toString() );
	pWiredSocket->getFile( thePath, tmpDownloadFolder.absoluteFilePath(tmpName) );;
}


void ClassWiredSession::search_reveal_file(QString thePath) {
	doActionFiles(thePath.section("/",0,-2));
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





void ClassWiredSession::setTrayMenuAction(QMenu *action) {
	pTrayMenuItem = action;
	pTrayMenuItem->setTitle(pConnWindow->windowTitle());

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

	// Write to the chat
	if(conf.value(QString("events/%1/chat").arg(event)).toBool())
		pMainChat->writeEventToChat(tmpMsg);

	// Show a message in the system tray
	if(conf.value(QString("events/%1/traymsg").arg(event)).toBool()) {
		WiredSingleton *tmpS = &WSINGLETON::Instance();
		tmpS->pTrayIcon->showMessage( pWiredSocket->pServerName, tmpMsg );
	}
	
	if(conf.contains(QString("events/%1/sound").arg(event)) ) {
		
		qDebug() << conf.value(QString("events/%1/sound").arg(event)).toString();
		QSound::play( conf.value(QString("events/%1/sound").arg(event)).toString() );
	}

	// Execute a system command
	if(conf.value(QString("events/%1/syscmd_enabled").arg(event), false).toBool()) {
		QString tmpCmd = conf.value(QString("events/%1/syscmd").arg(event)).toString();
		if(!tmpCmd.isEmpty()) {
			QProcess tmpProc;
			tmpProc.start(tmpCmd);
		}
	}
	
	
}


void ClassWiredSession::userJoined(int theChat, ClassWiredUser theUser) {
	if(theChat!=1) return;
	triggerEvent("UserJoined", QStringList() << theUser.pNick);
}


void ClassWiredSession::userLeft(int theChat, ClassWiredUser theUser) {
	if(theChat!=1) return;
	triggerEvent("UserLeft", QStringList() << theUser.pNick);
}


void ClassWiredSession::userChanged(ClassWiredUser theOld, ClassWiredUser theNew) {
	if(theOld.pNick != theNew.pNick)
		triggerEvent("UserChangedNick", QStringList() << theOld.pNick << theNew.pNick);
	if(theOld.pStatus != theNew.pStatus)
		triggerEvent("UserChangedStatus", QStringList() << theNew.pNick << theNew.pStatus);
}


void ClassWiredSession::newsPosted(QString theNick, QString, QString thePost) {
	triggerEvent("NewsPosted", QStringList() << theNick << thePost);
}


void ClassWiredSession::transferStarted(ClassWiredTransfer transfer) {
	triggerEvent("TransferStarted", QStringList(transfer.fileName()));
}


void ClassWiredSession::transferDone(ClassWiredTransfer transfer) {
	triggerEvent("TransferFinished", QStringList(transfer.fileName()));
}


void ClassWiredSession::transferSocketError(QAbstractSocket::SocketError error) {
	QStringList tmpParams;
	tmpParams << tr("The file transfer failed due to a connection error. Error ID is: %1").arg(error);
	triggerEvent("ServerError",tmpParams);
}


/// The login was successful, switch to forum view.
void ClassWiredSession::onLoginSuccessful() {
	if(!pConnectWindow) return;
	pMainTabWidget->addTab(pMainChat, "Chat");
	pConnectWindow->setStatus(tr("Receiving user list..."));
	pConnectWindow->setProgressBar(2,3);
	triggerEvent("ServerConnected",QStringList());
}


/// Initialize the main socket and load settings
void ClassWiredSession::initWiredSocket()
{
	pWiredSocket = new WiredSocket(this);
	
	QSettings settings;
	pWiredSocket->setUserStatus(settings.value("general/status","Qwired Newbie").toString());
	pWiredSocket->setUserNick(settings.value("general/nickname", "Unnamed").toString());
		
	QPixmap tmpIcon = settings.value("general/icon", QPixmap(":/icons/qwired_logo_32.png")).value<QPixmap>();
	pWiredSocket->setUserIcon(tmpIcon);
}


/// Reload the preferences if the user changed those.
void ClassWiredSession::reloadPreferences()
{
	QSettings s;
	if(pWiredSocket->sessionUser.pNick!=s.value("general/nickname", "Unnamed").toString())
		pWiredSocket->setUserNick(s.value("general/nickname").toString());

	if(pWiredSocket->sessionUser.pStatus!=s.value("general/status", "Qwired Newbie").toString())
		pWiredSocket->setUserStatus(s.value("general/status").toString());

	QPixmap tmpNew = s.value("general/icon", QPixmap()).value<QPixmap>();
	pWiredSocket->setUserIcon(tmpNew);
}


void ClassWiredSession::onConnectAborted()
{
	pWiredSocket->disconnectSocketFromServer();
}


// === ACTIONS FROM THE MAIN WINDOW === //
// ==================================== //

/// The disconnect button has been clicked. Disconnect from the server.
void ClassWiredSession::doActionDisconnect() {
	setConnectionToolButtonsEnabled(false);
	pMainChat->resetForm();
	pConnectWindow->resetForm();
	pWiredSocket->disconnectFromServer();
	pContainerLayout->setCurrentIndex(0); // go to connect dialog
}


/// Show the list of accounts and groups in a window.
void ClassWiredSession::doActionAccounts() {
	if(!pWinAccounts) {
		pWinAccounts = new WidgetAccounts(pConnWindow);
		int tmpIdx = pMainTabWidget->addTab(pWinAccounts, tr("Accounts"));
		pMainTabWidget->setCurrentIndex(tmpIdx);
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
		int tmpIdx = pMainTabWidget->indexOf(pWinAccounts);
		pMainTabWidget->setCurrentIndex(tmpIdx);
	}
}


/// Request the news from the server.
void ClassWiredSession::doActionNews() {
	if( !pWinNews ) {
		pWinNews = new WidgetNews();
		connect( pWiredSocket, SIGNAL(onServerNews(QString, QString, QString)), pWinNews, SLOT(addNewsItem(QString, QString, QString)) );
		connect( pWiredSocket, SIGNAL(onServerNewsPosted(QString, QString, QString)), pWinNews, SLOT(addFreshNewsItem(QString, QString, QString)) );
		connect( pWiredSocket, SIGNAL(onServerNewsDone()), pWinNews, SLOT(onServerNewsDone()) );
		connect( pWinNews, SIGNAL(doRefreshNews()), pWiredSocket, SLOT(getNews()) );
		connect( pWinNews, SIGNAL(doPostNews(QString)), pWiredSocket, SLOT(postNews(QString)) );
		connect( pWinNews, SIGNAL(onDeleteNews()), pWiredSocket, SLOT(deleteNews()) );
		
		// Display the widget using a Tab
		int tmpIdx = pMainTabWidget->addTab(pWinNews, QIcon(), tr("News"));
		pMainTabWidget->setCurrentIndex(tmpIdx);
		
		//pWinNews->show();
		pWiredSocket->getNews();
	} else {
		int tmpIdx = pMainTabWidget->indexOf(pWinNews);
		pMainTabWidget->setCurrentIndex(tmpIdx);
		//pWinNews->raise();
	}
}


/// Display the server information dialog.
void ClassWiredSession::doActionServerInfo() {	
	if( !pServerWindow ) {
		pServerWindow = new WidgetServerInfo();
		pServerWindow->loadInfo(pWiredSocket);
		pMainTabWidget->addTab(pServerWindow, tr("Server Info"));
	} else {
		pServerWindow->raise();
	}
}


/// Open a new broadcast message dialog.
void ClassWiredSession::doActionBroadcast() {
	WidgetSendPrivMsg *msg = new WidgetSendPrivMsg();
	msg->setParent(pConnWindow, Qt::Window);
	msg->move( pConnWindow->pos() );
	msg->show();
}


/// Open a new file browser and request the list of files (/) from the server.
void ClassWiredSession::doActionFiles(QString thePath) {
	if(!pWinFileBrowser) {
		pWinFileBrowser = new WidgetFileBrowser();
		pWinFileBrowser->setParent(pConnWindow, Qt::Window);
		pWinFileBrowser->initWithConnection(this);
		pWinFileBrowser->setPath(thePath);
		pWinFileBrowser->pModel->pWaitingForList = true;
		pWiredSocket->getFileList(thePath);

		// Display the widget using a Tab
		pMainTabWidget->setCurrentIndex(pMainTabWidget->addTab(pWinFileBrowser, QIcon(), tr("Files")));
		
	} else {
		int tmpIdx = pMainTabWidget->indexOf(pWinFileBrowser);
		pMainTabWidget->setCurrentIndex(tmpIdx);
	}
// 	browser->move( pConnWindow->pos() );
	
	
	
// 	browser->show();
	 // Request the root list of files
}


/// Create a new connection and append it to the singleton object.
void ClassWiredSession::doActionNewConnection() {
	WiredSingleton *tmpS = &WSINGLETON::Instance();
	ClassWiredSession *tmpNew = new ClassWiredSession();
	tmpS->addSession(tmpNew);
}


/// Show the preferences dialog.
void ClassWiredSession::doActionPreferences() {
	if( pPrefsWindow==0 ) {
		pPrefsWindow = new WidgetPrefs();
		pPrefsWindow->setParent(pConnWindow, Qt::Window);
		pPrefsWindow->move(pConnWindow->pos() );
		pPrefsWindow->show();
	} else {
		pPrefsWindow->raise();
	}
}


/// Show the list of trackers and their servers.
void ClassWiredSession::doActionTrackers() {
	if(!pWinTrackers) {
		pWinTrackers = new WidgetTracker();
		pWinTrackers->setParent(pConnWindow, Qt::Window);
		pWinTrackers->show();
	} else {
		pWinTrackers->raise();
	}
}


/// Show the file search dialog.
void ClassWiredSession::doActionFileSearch() {
	if(!pFileSearch) {
		pFileSearch = new WidgetFileSearch(pConnWindow);
		int tmpIdx = pMainTabWidget->addTab(pFileSearch, tr("File Search"));
		pMainTabWidget->setCurrentIndex(tmpIdx);
		connect( pWiredSocket, SIGNAL(fileSearchDone(QList<ClassWiredFile>)),
				 pFileSearch, SLOT(updateResults(QList<ClassWiredFile>)) );
		connect( pFileSearch, SIGNAL(search(QString)), pWiredSocket, SLOT(searchFiles(QString)) );
		connect( pFileSearch, SIGNAL(downloadFile(QString)), this, SLOT(search_download_file(QString)) );
		connect( pFileSearch, SIGNAL(revealFile(QString)), this, SLOT(search_reveal_file(QString)) );
	} else {
		int tmpIdx = pMainTabWidget->indexOf(pFileSearch);
		pMainTabWidget->setCurrentIndex(tmpIdx);
	}
}


/// Show the list of transfers.
void ClassWiredSession::doActionTransfers() {
	if( !pTranfersWindow ) {
		pTranfersWindow = new WidgetTransfers(pConnWindow);
		int tmpIdx = pMainTabWidget->addTab(pTranfersWindow, QIcon(), tr("Transfers"));
		pMainTabWidget->setCurrentIndex(tmpIdx);
		// Model
		ModelFileTransfers *tmpModel = new ModelFileTransfers(pTranfersWindow->fTransfers);
		tmpModel->setSocket(pWiredSocket);
		pTranfersWindow->fTransfers->setModel(tmpModel);
		pTranfersWindow->init();
		connect(pTranfersWindow, SIGNAL(transferCancelled(ClassWiredTransfer)),
				pWiredSocket, SLOT(cancelTransfer(ClassWiredTransfer)) );
		
	} else {
		int tmpIdx = pMainTabWidget->indexOf(pTranfersWindow);
		pMainTabWidget->setCurrentIndex(tmpIdx);
	}
}
