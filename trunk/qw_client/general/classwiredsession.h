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

 
#ifndef WSESSIONMANAGER_H
#define WSESSIONMANAGER_H

#include <QObject>

#include <QStackedLayout>

#include "gui/widgetforum.h"
#include "gui/connwindow.h"
#include "wired/wiredsocket.h"

#include "gui/widgetnews.h"
#include "gui/modeluserlist.h"
#include "gui/widgetprefs.h"
#include "gui/widgetconnect.h"
#include "gui/widgettransfers.h"
#include "gui/WidgetFileSearch.h"
#include "gui/widgetsendprivmsg.h"
#include "gui/widgetaccounts.h"
#include "gui/WidgetTracker.h"
#include "gui/WidgetFileInfo.h"
#include "gui/widgetserverinfo.h"



namespace Qwired {
	enum Event { ServerConnected, ServerDisconnected, ServerError, UserJoined, UserChangedNick, UserChangedStatus,
		UserLeft, ChatReceived, MessageReceived, NewsPosted, BroadcastReceived, TransferStarted, TransferFinished };
};


class WidgetFileBrowser;

/**
	@author Bastian Bense <bastibense@gmail.com>
 */
class ClassWiredSession
	: public QObject
{
Q_OBJECT
public:
	ClassWiredSession(QObject *parent = 0);
	~ClassWiredSession();

        WiredSocket* wiredSocket() { return pWiredSocket; }

	// Main window widgets
	void initMainWindow();
	QPointer<QWidget> pContainerWidget;
	QPointer<QStackedLayout> pContainerLayout;
	QPointer<QTabWidget> pMainTabWidget;

	// Wired Socket
	void initWiredSocket();
	
	
	QPointer<ConnWindow> pConnWindow;
	QPointer<WidgetNews> pWinNews;
	QPointer<ModelUserList> pUserListModel;
	QHash<int, QPointer<WidgetForum> > pChats;
	QHash<QString,QPointer<WidgetFileInfo> > pFileInfos;
	QPointer<WidgetForum> pMainChat;
	QPointer<WidgetServerInfo> pServerWindow;
	QPointer<WidgetPrefs> pPrefsWindow;
	QPointer<WidgetConnect> pConnectWindow;
	QPointer<WidgetTransfers> pTranfersWindow;
	QPointer<WidgetFileSearch> pFileSearch;
	QPointer<WidgetAccounts> pWinAccounts;
	QPointer<WidgetTracker> pWinTrackers;
        QPointer<WidgetFileBrowser> pWinFileBrowser;

	QPointer<QMenu> pTrayMenuItem;
	

	QHash<int,QPointer<WidgetSendPrivMsg> > pMsgWindows;
	
private:
	QPointer<WiredSocket> pWiredSocket;
	
	void setupConnections();
	void setConnectionToolButtonsEnabled(bool);


private slots:

	// Main window widgets
	void onTabBarCloseButtonClicked();
	void onTabBarCurrentChanged(int index);


	// Connect widget
	void onConnectAborted();


	// Chat Window
	void onUserlistComplete(int chatId);
	
	void connectionWindowDestroyed(QObject *obj);
	void reloadPreferences();
	void onSocketPrivileges(ClassWiredUser);
	void doActionTrackers();

	void onServerFileInfo(ClassWiredFile theFile);

	void userJoined(int theChat, ClassWiredUser theUser);
	void userLeft(int theChat, ClassWiredUser theUser);
	void userChanged(ClassWiredUser theOld, ClassWiredUser theNew);
	void newsPosted(QString theNick, QString theTime, QString thePost);

	
	void transferStarted(ClassWiredTransfer);
	void transferDone(ClassWiredTransfer);
	void transferSocketError(QAbstractSocket::SocketError);
	void transferError(ClassWiredTransfer);

	void fileListingRecursiveDone(const QList<ClassWiredFile>);
	
public slots:
	void triggerEvent(QString event, QStringList parameters);
	void setTrayMenuAction(QMenu*);
	
	void do_handle_chat_message(int, int, QString, bool);

	// Acount buttons from connect window.
	void doActionAccounts();
	void doActionDisconnect();
        void doActionNews();
	void doActionServerInfo();
	void doActionNewConnection();
	void doActionBroadcast();
	void doActionFiles(QString thePath="/");
	
	// Toolbar handlers

	void doActionPreferences();
	void doActionTransfers();
	void doActionFileSearch();
	
	
	void search_download_file(QString);
	void search_reveal_file(QString);
	
	// Forum handlers
	void doHandlePublicChatInput(QString theText, bool theIsAction);
	
	// Connect window
	void onDoConnect(QString theHost, QString theLogin, QString thePassword);
	
	// Socket handlers
	void doHandleChatTopic(int theChatID, QString theNick, QString theLogin, QHostAddress theIP, QDateTime theDateTime, QString theTopic);
	void doHandlePrivMsg(ClassWiredUser theUser, QString theMessage);
	void doHandleBroadcast(ClassWiredUser theUser, QString theMessage);
	void doHandleUserInfo(ClassWiredUser theUser);
	void doHandlePrivateChatInvitation(int theChatID, ClassWiredUser theUser);
	void doCreateNewChat(int theChatID);
	
	void onSocketError(QAbstractSocket::SocketError);
	void onSocketServerInfo();
	void onLoginSuccessful();
	void onSocketLoginFailed();
	
	void setBannerView(const QPixmap theBanner);
	void handleErrorOccoured(int theError);

	void downloadFile(const QString &remotePath, const QString &localPath);
	void downloadFolder(const QString &remotePath, const QString &localPath);
	void uploadFile(const QString &localPath, const QString &remotePath);
	void uploadFolder(const QString &localPath, const QString &remotePath);
	
	
};

#endif