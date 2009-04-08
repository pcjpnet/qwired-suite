/*! \author Bastian Bense <bastibense@gmail.com>
    \date 2009-03-05

    This class implementes a single connection (session) to a server, manages the GUI elements
    such as the connection window, and also manages the file transfers.
*/

#include "QwcGlobals.h"
#include "QwcSession.h"
#include "QwcUserInfoWidget.h"
#include "QwcConnectWidget.h"
#include "QwcFiletransferModel.h"
#include "QwcFileBrowserWidget.h"
#include <QMessageBox>
#include <QSound>

QwcSession::QwcSession(QObject *parent) : QObject(parent)
{
    initWiredSocket();
    initMainWindow();
    setConnectionToolButtonsEnabled(false);
    reloadPreferences();
}

QwcSession::~QwcSession()
{
    pConnWindow->deleteLater();
    pContainerWidget->deleteLater();
    pMainChat->deleteLater();

}


/*! Handle a protocol-related error and display it to the user.
*/
void QwcSession::handleProtocolError(Qw::ProtocolError error)
{
    if (error == Qw::ErrorLoginFailed) {
        pConnectWindow->resetForm();
        pContainerLayout->setCurrentIndex(0);
        setConnectionToolButtonsEnabled(false);
    }

    // Display an error message
    QString errorText(tr("An unknown server error occoured. The error code is %1.").arg(error));
    switch (error) {
        case Qw::ErrorCommandFailed: errorText=tr("Command Failed. An undefined internal server error has occoured."); break;
        case Qw::ErrorCommandNotRecognized: errorText=tr("Command Not Recognized. Qwired sent a command that is unknown by the server."); break;
        case Qw::ErrorCommandNotImplemented: errorText=tr("Command Not Implemented. The last command is not implemented by the server."); break;
        case Qw::ErrorSyntaxError: errorText=tr("Syntax Error. The last command was not formatted correctly."); break;
        case Qw::ErrorLoginFailed: errorText=tr("Login Failed. Username and password were not accepted by the server."); break;
        case Qw::ErrorBanned: errorText=tr("Banned. You have been banned from the server. Please try connecting later."); break;
        case Qw::ErrorClientNotFound: errorText=tr("Client Not Found. The server could not find the client referred to."); break;
        case Qw::ErrorAccountNotFound: errorText=tr("Account Not Found. The server could not find the account referred to."); break;
        case Qw::ErrorAccountExists: errorText=tr("Account Exists. Could not create the account you specified."); break;
        case Qw::ErrorCannotBeDisconnected: errorText=tr("User can not be disconnected. The specified user can not be disconnected."); break;
        case Qw::ErrorPermissionDenied: errorText=tr("Permission Denied. You don't have sufficient privileges to execute the last command."); break;
        case Qw::ErrorFileOrDirectoryNotFound: errorText=tr("File or Directory not found. The last command could not be completed because the file or directory could not be found."); break;
        case Qw::ErrorFileOrDirectoryExists: errorText=tr("The last command could not be completed because the file or directory already exists."); break;
        case Qw::ErrorChecksumMismatch: errorText=tr("Checksum Mismatch.");
        case Qw::ErrorQueueLimitExceeded: errorText=tr("Queue Limit Exceeded. Could not complete the last command because the server queue is full."); break;
    }

    // Dispatch an event
    triggerEvent("ServerError", QStringList(errorText));
    QMessageBox::warning(pConnWindow, tr("Server Error"), errorText);
}


/*! Handle a low-level socket error.
*/
void QwcSession::handleSocketError(QAbstractSocket::SocketError error)
{
    if(error == QAbstractSocket::ConnectionRefusedError) {
        // Error occoured while connecting from the connecting window
        pConnectWindow->resetForm();
        QMessageBox::critical(pMainChat, tr("Connection Refused"),
                              tr("The connection was refused by the remote host. "
                                 "This normally means that there is no Wired server running on the remote machine."));
    } else if(error == QAbstractSocket::HostNotFoundError) {
        pConnectWindow->resetForm();
        QMessageBox::critical(pMainChat, tr("Host not found"),
                              tr("Could not connect to the remote server because the host name could not be resolved."));
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


void QwcSession::initMainWindow()
{
    bannerSpace = 0;
    bannerView = 0;
    bannerSpace2 = 0;

    // Init the main window (which contains the toolbar and other things)
    pConnWindow = new QwcConnectionMainWindow;
    connect(pConnWindow, SIGNAL(destroyed(QObject*)), this, SLOT(connectionWindowDestroyed(QObject*)) );
    pConnWindow->show();

    // Init the main chat window (chat 0)
    pMainChat = new QwcChatWidget;
    pMainChat->setContentsMargins(0, 0, 0, 0);

    // Init the Connect window (where the user enters host name, login, password)
    pConnectWindow = new QwcConnectWidget;
    connect(pConnectWindow, SIGNAL(userFinished(QString,QString,QString)),
            this, SLOT(onDoConnect(QString,QString,QString)) );
    connect(pConnectWindow, SIGNAL(onConnectAborted()),
            this, SLOT(onConnectAborted()));

    // Set up the container widget
    pContainerWidget = new QWidget();
    pContainerLayout = new QStackedLayout(pContainerWidget);
//#ifndef Q_WS_MAC
    pContainerWidget->setContentsMargins(6, 6, 6, 6);
//#endif
    pContainerWidget->setLayout(pContainerLayout);

    // Create the tab bar
    pMainTabWidget = new QTabWidget(pContainerWidget);
    pMainTabWidget->clear();
    pMainTabWidget->setVisible(false);
//#ifdef Q_WS_MAC
//    pMainTabWidget->setDocumentMode(true);
//#endif
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
    pUserListModel = new QwcUserlistModel(pMainChat);
    pUserListModel->setWiredSocket(socket);
    pMainChat->setSession(this);
    pMainChat->setUserListModel(pUserListModel);

    // Private messenger
    privateMessager = new QwcPrivateMessager;
    privateMessager->setParent(pMainChat, Qt::Window);

    // Add the widgets to the stacked layout
    pContainerWidget->layout()->addWidget(pConnectWindow);
    pContainerWidget->layout()->addWidget(pMainTabWidget);

    // Set the virtual widget
    pConnWindow->setCentralWidget(pContainerWidget);
    pConnWindow->show();

    setupConnections();

    // Install the event filter in pConnWindow
    pEventFilter = new QwcEventFilter;
    pConnWindow->installEventFilter(pEventFilter);
    pEventFilter->setSocket(socket);
}


/// The user list was completely received after connecting.
void QwcSession::onUserlistComplete(int chatId)
{
    if (chatId != 1 && pContainerLayout->currentIndex() != 0) return;
    pContainerLayout->setCurrentIndex(1);
    setConnectionToolButtonsEnabled(true);
}


/// Tab bar close button clicked. Close the current widget.
void QwcSession::onTabBarCloseButtonClicked()
{
    int tmpIdx = pMainTabWidget->currentIndex();
    // We check if the chat tab is the current one so that not to close it
    if(pMainTabWidget->currentWidget() == pMainChat) return;
    pMainTabWidget->currentWidget()->close();
    pMainTabWidget->removeTab(tmpIdx);
}


void QwcSession::onTabBarCurrentChanged(int index)
{
    QWidget *tmpBtn = pMainTabWidget->cornerWidget();

    // We check if the chat tab is the current one so that not to close it
    if(pMainTabWidget->currentWidget() != pMainChat) {
        tmpBtn->setEnabled(true);
    } else {
        tmpBtn->setEnabled(false);
    }

    // Icon removal for private chats
    QWidget *tmpWid = pMainTabWidget->widget(index);
    QwcChatWidget *tmpChat = qobject_cast<QwcChatWidget*>(tmpWid);
    if(tmpChat && tmpChat->pChatID!=1) {
        pMainTabWidget->setTabIcon(index, QIcon(":/icons/tab-idle.png"));
    }
}





/// Called when the connection window is destroyed/closed. Allows the manager class to do
/// some clean up work.
void QwcSession::connectionWindowDestroyed(QObject*)
{
    this->deleteLater();
}




/// Set up connections between objects in this class.
void QwcSession::setupConnections()
{
    // Socket connections
    //
    connect(socket, SIGNAL(protocolError(Qw::ProtocolError)),
            this, SLOT(handleProtocolError(Qw::ProtocolError)));


    // Messager
    //

    connect(socket, SIGNAL(onPrivateMessage(QwcUserInfo,QString)),
            privateMessager, SLOT(handleNewMessage(QwcUserInfo, QString)) );
    connect(privateMessager, SIGNAL(enteredNewMessage(int,QString)),
            socket, SLOT(sendPrivateMessage(int,QString)));
    connect(pMainChat, SIGNAL(userDoubleClicked(const QwcUserInfo)),
            this, SLOT(showMessagerForUser(const QwcUserInfo)));
    connect(socket, SIGNAL(userChanged(QwcUserInfo, QwcUserInfo)),
            privateMessager, SLOT(handleUserChanged(QwcUserInfo, QwcUserInfo)) );
    connect(socket, SIGNAL(userLeftRoom(int, QwcUserInfo)),
            privateMessager, SLOT(handleUserLeft(int, QwcUserInfo)));


    connect(socket, SIGNAL(receivedChatMessage(int,int,QString,bool)), this, SLOT(do_handle_chat_message(int,int,QString,bool)) );
    connect(socket, SIGNAL(onChatTopic(int, QString, QString, QHostAddress, QDateTime, QString)),
            this,   SLOT(doHandleChatTopic(int, QString, QString, QHostAddress, QDateTime, QString)) );

    connect(socket, SIGNAL(onServerLoginSuccessful()), this, SLOT(onLoginSuccessful()) );

    connect(socket, SIGNAL(receivedUserlist(int)), this, SLOT(onUserlistComplete(int)) );

    connect(socket, SIGNAL(userInformation(QwcUserInfo)), this, SLOT(doHandleUserInfo(QwcUserInfo)) );
    connect(socket, SIGNAL(privateChatInvitation(int,QwcUserInfo)), this, SLOT(doHandlePrivateChatInvitation(int,QwcUserInfo)) );
    connect(socket, SIGNAL(privateChatCreated(int)), this, SLOT(doCreateNewChat(int)) );

    connect(socket, SIGNAL(onServerInformation()), this, SLOT(onSocketServerInfo()) );


    connect(socket, SIGNAL(onServerBanner(QPixmap)), this, SLOT(setBannerView(QPixmap)) );

    connect(socket, SIGNAL(receivedUserPrivileges(QwcUserInfo)), this, SLOT(onSocketPrivileges(QwcUserInfo)) );
    connect(socket, SIGNAL(fileInformation(QwcFileInfo)), this, SLOT(fileInformation(QwcFileInfo)) );

    connect(socket, SIGNAL(userJoinedRoom(int,QwcUserInfo)), this, SLOT(userJoined(int,QwcUserInfo)) );
    connect(socket, SIGNAL(userLeftRoom(int,QwcUserInfo)), this, SLOT(userLeft(int,QwcUserInfo)) );
    connect(socket, SIGNAL(userChanged(QwcUserInfo,QwcUserInfo)), this, SLOT(userChanged(QwcUserInfo,QwcUserInfo)) );
    connect(socket, SIGNAL(onServerNewsPosted(QString, QString, QString)), this, SLOT(newsPosted(QString,QString,QString)) );

    // File transfer signals
    connect(socket, SIGNAL(fileTransferDone(const QwcFiletransferInfo)), this, SLOT(transferDone(QwcFiletransferInfo)) );
    connect(socket, SIGNAL(fileTransferStarted(const QwcFiletransferInfo)), this, SLOT(transferStarted(QwcFiletransferInfo)) );
    connect(socket, SIGNAL(fileTransferSocketError(QAbstractSocket::SocketError)), this, SLOT(transferSocketError(QAbstractSocket::SocketError)));
    connect(socket, SIGNAL(fileTransferError(const QwcFiletransferInfo)), this, SLOT(transferError(QwcFiletransferInfo)) );
    connect(socket, SIGNAL(onFilesListRecursiveDone(const QList<QwcFileInfo>)), this, SLOT(fileListingRecursiveDone(const QList<QwcFileInfo>)));

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
    QwcSingleton *tmpS = &WSINGLETON::Instance();
    connect( tmpS, SIGNAL(prefsChanged()), this, SLOT(reloadPreferences()) );

}


/**
 * Received information about a file (get info) from the server. Create a new window or
 * display the old window respectively.
 * @param theFile The information about the file.
 */
void QwcSession::fileInformation(QwcFileInfo theFile)
{
    QwcFileInfoWidget *win=0;

    if(pFileInfos.contains(theFile.path)) {
        win = pFileInfos[theFile.path];
    }

    if(!win) {
        win = new QwcFileInfoWidget();
        win->setParent(pConnWindow, Qt::Window);
        pFileInfos[theFile.path] = win;
    }

    if(win) {
        win->loadFromFile(theFile);
        win->fName->setReadOnly(!socket->sessionUser.privAlterFiles);
        win->fComments->setReadOnly(!socket->sessionUser.privAlterFiles);
        win->show();
        win->raise();
    }
}



// Enable/Disable GUI elements depending on the privileges
void QwcSession::onSocketPrivileges(QwcUserInfo s)
{
    pConnWindow->actionAccounts->setEnabled(s.privEditAccounts);
}


/// A chat message was received, handle it.
void QwcSession::do_handle_chat_message(int theChat, int theUserID, QString theText, bool theIsAction)
{
    QwcUserInfo tmpUsr = socket->users[theUserID]; //socket->getUserByID(theUserID); // Find the user
    if(theChat==1) {
        // Public chat
        pMainChat->writeToChat(tmpUsr.userNickname, theText, theIsAction);

        // Trigger the event
        QStringList tmpParams;
        tmpParams << tmpUsr.userNickname;
        if(theIsAction) tmpParams << QString("*** %1 %2").arg(tmpUsr.userNickname).arg(theText);
        else tmpParams << theText;
        triggerEvent("ChatReceived", tmpParams);

    } else {
        // Handle private chat
        if(!pChats.contains(theChat)) {
            qDebug() << "QwcSession: Warning: Unknown chat with id"<<theChat;
            return;
        }
        QwcChatWidget *chat = pChats[theChat];
        chat->writeToChat(tmpUsr.userNickname, theText, theIsAction);

        // Find the index on the tab panel
        int tmpIdx = pMainTabWidget->indexOf(chat);
        if( tmpIdx>-1 && pMainTabWidget->currentIndex()!=tmpIdx )
            pMainTabWidget->setTabIcon( tmpIdx, QIcon(":/icons/tab-content.png") );
    }
}


/// The chat topic was changed or received. Update the chat window.
void QwcSession::doHandleChatTopic(int theChatID, QString theNick, QString theLogin, QHostAddress theIP, QDateTime theDateTime, QString theTopic) {
    Q_UNUSED(theLogin);
    Q_UNUSED(theIP);
    if( theChatID==1 ) {
        // Public chat
        pMainChat->fTopic->setText(tr("Topic: %1\nSet By: %2 --- %3").arg(theTopic).arg(theNick).arg(theDateTime.toString()));
    } else if(pChats.contains(theChatID)) {
        // Topic of private chat
        QwcChatWidget *chat = pChats[theChatID];
        chat->fTopic->setText(tr("Topic: %1\nSet By: %2 --- %3").arg(theTopic).arg(theNick).arg(theDateTime.toString()));
    }
}


// Handle user public chat input.
void QwcSession::doHandlePublicChatInput(QString theText, bool theIsAction)
{
    socket->sendChatToRoom(1, theText, theIsAction);
}

// Display received user info in a new window.
void QwcSession::doHandleUserInfo(QwcUserInfo theUser)
{

    for(int i=0; i<pMainTabWidget->count(); i++) {
        QwcUserInfoWidget *info = dynamic_cast<QwcUserInfoWidget*>(pMainTabWidget->widget(i));
        if(info && info->pUserID==theUser.pUserID) { // Found existing window
            pMainTabWidget->setCurrentIndex(i);
            info->setUser(theUser);
            return;
        } }

}


// Received an invitation to a private chat.
void QwcSession::doHandlePrivateChatInvitation(int theChatID, QwcUserInfo theUser)
{
    QMessageBox messageBox(pConnWindow);
    messageBox.setWindowTitle(tr("Private Chat Invitation"));
    messageBox.setText(tr("%1 has invited you to a private chat.\nJoin to open a separate private chat with %1.").arg(theUser.userNickname) );
    messageBox.setIconPixmap( QPixmap(":/icons/btn_chat.png") );
    QAbstractButton *ignoreButton = messageBox.addButton(tr("Ignore"), QMessageBox::DestructiveRole);
    QAbstractButton *rejectButton = messageBox.addButton(tr("Reject"), QMessageBox::RejectRole);
    QAbstractButton *joinButton = messageBox.addButton(tr("Join"), QMessageBox::AcceptRole);

    Q_UNUSED(ignoreButton)

            messageBox.exec();
    if (messageBox.clickedButton() == joinButton) {
        // Create a new chat
        doCreateNewChat(theChatID);
        socket->joinChat(theChatID);
    } else if( messageBox.clickedButton() == rejectButton ) {
        // Reject the chat.
        socket->rejectChat(theChatID);
    } else {
        // Ignore
    }
}


// Create a new private chat window and set the chat id on it.
void QwcSession::doCreateNewChat(int theChatID)
{
    QwcChatWidget *chat = new QwcChatWidget(pConnWindow);
    QwcUserlistModel *model = new QwcUserlistModel(chat);
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
void QwcSession::onSocketServerInfo()
{
    pConnectWindow->setStatus(tr("Connecting. Starting session..."));
    if(pConnectWindow>0) pConnectWindow->setProgressBar(1,3);
    if(pTrayMenuItem) pTrayMenuItem->setTitle(socket->serverInfo.name);
}



/// Connect to the remote server.
void QwcSession::onDoConnect(QString theHost, QString theLogin, QString thePassword)
{
    if (theLogin.isEmpty()) {
        // Log in as guest if no login/password defined.
        socket->setUserAccount("guest", "");
    } else {
        socket->setUserAccount(theLogin,thePassword);
    }
    socket->connectToWiredServer(theHost);
}





/// Enable/Disable connection-related toolbar items (true if connected)
void QwcSession::setConnectionToolButtonsEnabled(bool theEnable)
{
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
void QwcSession::setBannerView(const QPixmap theBanner)
{
    if(bannerSpace) {
        delete bannerSpace;
        bannerSpace = 0;
    }
    if(bannerView) {
        delete bannerView;
        bannerView = 0;
    }
    if(bannerSpace2) {
        delete bannerSpace2;
        bannerSpace2 = 0;
    }

    bannerSpace = new QWidget(pConnWindow->fToolBar);
    bannerSpace->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    pConnWindow->fToolBar->addWidget(bannerSpace);

    bannerView = new QLabel(pConnWindow->fToolBar);
    bannerView->setPixmap(theBanner);
    pConnWindow->fToolBar->addWidget(bannerView);

    bannerSpace2 = new QWidget(pConnWindow->fToolBar);
    bannerSpace2->setFixedWidth(10);
    pConnWindow->fToolBar->addWidget(bannerSpace2);

}

/// Connected to the download button in the search dialog.
void QwcSession::search_download_file(QString thePath)
{
    QSettings settings;
    doActionTransfers();
    QString tmpName = thePath.section("/",-1,-1);
    QDir tmpDownloadFolder(settings.value("files/download_dir", QDir::homePath()).toString());
    downloadFile(thePath, tmpDownloadFolder.absoluteFilePath(tmpName));
}


void QwcSession::search_reveal_file(QString thePath)
{
    doActionFiles(thePath.section("/",0,-2));
}



void QwcSession::setTrayMenuAction(QMenu *action)
{
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
void QwcSession::triggerEvent(QString event, QStringList params)
{
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
        QwcSingleton *tmpS = &WSINGLETON::Instance();
        tmpS->pTrayIcon->showMessage( socket->serverInfo.name, tmpMsg );
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


void QwcSession::userJoined(int theChat, QwcUserInfo theUser)
{
    if(theChat!=1) return;
    triggerEvent("UserJoined", QStringList() << theUser.userNickname);
}


void QwcSession::userLeft(int theChat, QwcUserInfo theUser)
{
    if(theChat!=1) return;
    triggerEvent("UserLeft", QStringList() << theUser.userNickname);
}


void QwcSession::userChanged(QwcUserInfo theOld, QwcUserInfo theNew)
{
    if(theOld.userNickname != theNew.userNickname)
        triggerEvent("UserChangedNick", QStringList() << theOld.userNickname << theNew.userNickname);
    if(theOld.userStatus != theNew.userStatus)
        triggerEvent("UserChangedStatus", QStringList() << theNew.userNickname << theNew.userStatus);
}


void QwcSession::newsPosted(QString theNick, QString, QString thePost)
{
    triggerEvent("NewsPosted", QStringList() << theNick << thePost);
}


void QwcSession::transferStarted(QwcFiletransferInfo transfer)
{
    triggerEvent("TransferStarted", QStringList(transfer.fileName()));
}


void QwcSession::transferDone(QwcFiletransferInfo transfer)
{
    QSettings s;
    if(s.value("files/queue_local", false).toBool()) {
        qDebug() << this<< "Transfer completed. Unfreezing next.";
        socket->runTransferQueue(transfer.pTransferType);
    }

    triggerEvent("TransferFinished", QStringList(transfer.fileName()));
}


void QwcSession::transferSocketError(QAbstractSocket::SocketError error)
{
    QStringList tmpParams;
    tmpParams << tr("The file transfer failed due to a connection error. Error ID is: %1").arg(error);
    triggerEvent("ServerError",tmpParams);
}


/// The login was successful, switch to forum view.
void QwcSession::onLoginSuccessful()
{
    if(!pConnectWindow) return;
    pMainTabWidget->addTab(pMainChat, "Chat");
    pConnectWindow->setStatus(tr("Receiving user list..."));
    pConnectWindow->setProgressBar(2,3);
    triggerEvent("ServerConnected",QStringList());
}


/// Initialize the main socket and load settings
void QwcSession::initWiredSocket()
{
    socket = new QwcSocket(this);

    QSettings settings;
    socket->setUserStatus(settings.value("general/status","Qwired Newbie").toString());
    socket->setNickname(settings.value("general/nickname", "Unnamed").toString());

    QImage tmpIcon = settings.value("general/icon", QImage(":/icons/qwired_logo_32.png")).value<QImage>();
    socket->setIconImage(tmpIcon);

    connect(socket->sslSocket(), SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(handleSocketError(QAbstractSocket::SocketError)));
}


/// Reload the preferences if the user changed those.
void QwcSession::reloadPreferences()
{
    QSettings s;

    if (socket->sessionUser.userNickname != s.value("general/nickname", "Unnamed").toString()) {
        socket->setNickname(s.value("general/nickname").toString());
    }

    if (socket->sessionUser.userStatus != s.value("general/status", "Qwired Newbie").toString()) {
        socket->setUserStatus(s.value("general/status").toString());
    }

    QImage tmpNew = s.value("general/icon", QImage()).value<QImage>();
    socket->setIconImage(tmpNew);
}


void QwcSession::onConnectAborted()
{
    socket->disconnectFromServer();
}

// Prompt user to confirm disconnection
bool QwcSession::confirmDisconnection()
{
    if(!pEventFilter->disconnectionPrompt()) {
        return true;
    }
    return false;
}


// === ACTIONS FROM THE MAIN WINDOW === //
// ==================================== //

/// The disconnect button has been clicked. Disconnect from the server.
void QwcSession::doActionDisconnect()
{
    // First we ask for confirmation
    bool reallyDisconnect = confirmDisconnection();

    // And then we disconnect
    if(reallyDisconnect) {
        setConnectionToolButtonsEnabled(false);
        pMainChat->resetForm();
        pConnectWindow->resetForm();
        socket->disconnectFromServer();
        pContainerLayout->setCurrentIndex(0); // go to connect dialog
        socket->serverInfo = QwServerInfo();
    }
}


/// Show the list of accounts and groups in a window.
void QwcSession::doActionAccounts()
{
    if(!pWinAccounts) {
        pWinAccounts = new QwcAccountsWidget(pConnWindow);
        int tmpIdx = pMainTabWidget->addTab(pWinAccounts, tr("Accounts"));
        pMainTabWidget->setCurrentIndex(tmpIdx);
        connect( socket, SIGNAL(receivedAccountList(QStringList)), pWinAccounts, SLOT(appendUserNames(QStringList)) );
        connect( socket, SIGNAL(receivedAccountGroupList(QStringList)), pWinAccounts, SLOT(appendGroupNames(QStringList)) );
        connect( socket, SIGNAL(userSpecReceived(QwcUserInfo)), pWinAccounts, SLOT(loadUserSpec(QwcUserInfo)) );
        connect( socket, SIGNAL(groupSpecReceived(QwcUserInfo)), pWinAccounts, SLOT(loadGroupSpec(QwcUserInfo)) );
        connect( pWinAccounts, SIGNAL(userSpecRequested(QString)), socket, SLOT(readUser(QString)) );
        connect( pWinAccounts, SIGNAL(groupSpecRequested(QString)), socket, SLOT(readGroup(QString)) );
        connect( pWinAccounts, SIGNAL(userDeleted(QString)), socket, SLOT(deleteUser(QString)) );
        connect( pWinAccounts, SIGNAL(groupDeleted(QString)), socket, SLOT(deleteGroup(QString)) );

        connect( pWinAccounts, SIGNAL(createUser(QwcUserInfo)), socket, SLOT(createUser(QwcUserInfo)) );
        connect( pWinAccounts, SIGNAL(editUser(QwcUserInfo)), socket, SLOT(editUser(QwcUserInfo)) );
        connect( pWinAccounts, SIGNAL(createGroup(QwcUserInfo)), socket, SLOT(createGroup(QwcUserInfo)) );
        connect( pWinAccounts, SIGNAL(editGroup(QwcUserInfo)), socket, SLOT(editGroup(QwcUserInfo)) );
        socket->getGroups();
        socket->getUsers();
    } else {
        int tmpIdx = pMainTabWidget->indexOf(pWinAccounts);
        pMainTabWidget->setCurrentIndex(tmpIdx);
    }
}


/// Request the news from the server.
void QwcSession::doActionNews()
{
    if( !pWinNews ) {
        pWinNews = new QwcNewsWidget();
        connect( socket, SIGNAL(onServerNews(QString, QString, QString)), pWinNews, SLOT(addNewsItem(QString, QString, QString)) );
        connect( socket, SIGNAL(onServerNewsPosted(QString, QString, QString)), pWinNews, SLOT(addFreshNewsItem(QString, QString, QString)) );
        connect( socket, SIGNAL(newsDone()), pWinNews, SLOT(newsDone()) );
        connect( pWinNews, SIGNAL(doRefreshNews()), socket, SLOT(getNews()) );

        // We check for the proper purrmissions
        // Also, we don't want the button to be active if there are no news to clear

        qDebug("News counter: %u",pWinNews->newsCount());

        if(socket->sessionUser.privClearNews && pWinNews->newsCount()) {
            connect( pWinNews, SIGNAL(onDeleteNews()), socket, SLOT(clearNews()) );
            connect( pWinNews, SIGNAL(onDeleteNews()), pWinNews, SLOT(clearTextArea()));
        } else {
            pWinNews->setDisabledClearButton(true);
        }

        if(socket->sessionUser.privPostNews) {
            connect( pWinNews, SIGNAL(doPostNews(QString)), socket, SLOT(postNews(QString)) );
        } else {
            pWinNews->setDisabledPostButton(true);
        }



        // Display the widget using a Tab
        int tmpIdx = pMainTabWidget->addTab(pWinNews, QIcon(), tr("News"));
        pMainTabWidget->setCurrentIndex(tmpIdx);

        socket->getNews();
    } else {
        int tmpIdx = pMainTabWidget->indexOf(pWinNews);
        pMainTabWidget->setCurrentIndex(tmpIdx);
        //pWinNews->raise();
    }
}


/// Display the server information dialog.
void QwcSession::doActionServerInfo()
{
    if( !pServerWindow ) {
        pServerWindow = new QwcServerInfoWidget();
        pServerWindow->loadInfo(socket);
        pMainTabWidget->addTab(pServerWindow, tr("Server Info"));
    } else {
        pServerWindow->raise();
    }
}


/// Open a new broadcast message dialog.
void QwcSession::doActionBroadcast()
{
//    SendPrivateMessageWidget *msg = new SendPrivateMessageWidget();
//    msg->setParent(pConnWindow, Qt::Window);
//    msg->move( pConnWindow->pos() );
//    msg->show();
}


/// Open a new file browser and request the list of files (/) from the server.
void QwcSession::doActionFiles(QString thePath)
{
    if(!pWinFileBrowser) {
        pWinFileBrowser = new QwcFileBrowserWidget;
        pWinFileBrowser->setParent(pConnWindow, Qt::Window);
        pWinFileBrowser->initWithConnection(this);
        pWinFileBrowser->setPath(thePath);
        pWinFileBrowser->pModel->pWaitingForList = true;
        socket->getFileList(thePath);

        // Display the widget using a Tab
        pMainTabWidget->setCurrentIndex(pMainTabWidget->addTab(pWinFileBrowser, QIcon(), tr("Files")));

    } else {
        int tmpIdx = pMainTabWidget->indexOf(pWinFileBrowser);
        pMainTabWidget->setCurrentIndex(tmpIdx);
    }
}


/// Create a new connection and append it to the singleton object.
void QwcSession::doActionNewConnection()
{
    QwcSingleton *tmpS = &WSINGLETON::Instance();
    QwcSession *tmpNew = new QwcSession();
    tmpS->addSession(tmpNew);
}


/// Show the preferences dialog.
void QwcSession::doActionPreferences()
{
    if( pPrefsWindow==0 ) {
        pPrefsWindow = new QwcPreferencesWidget;
        pPrefsWindow->setParent(pConnWindow, Qt::Window);
        pPrefsWindow->move(pConnWindow->pos() );
        pPrefsWindow->show();
    } else {
        pPrefsWindow->raise();
    }
}


/// Show the list of trackers and their servers.
void QwcSession::doActionTrackers()
{
    if(!pWinTrackers) {
        QwcSingleton *tmpS = &WSINGLETON::Instance();
        pWinTrackers = new QwcTrackerlistWidget();
        pWinTrackers->setParent(pConnWindow, Qt::Window);
        connect(pWinTrackers, SIGNAL(newConnectionRequested(QString)), tmpS, SLOT(makeNewConnection(QString)));
        pWinTrackers->show();
    } else {
        pWinTrackers->raise();
    }
}


/// Show the file search dialog.
void QwcSession::doActionFileSearch()
{
    if(!pFileSearch) {
        pFileSearch = new QwcFileSearchWidget(pConnWindow);
        int tmpIdx = pMainTabWidget->addTab(pFileSearch, tr("File Search"));
        pMainTabWidget->setCurrentIndex(tmpIdx);
        connect( socket, SIGNAL(fileSearchDone(QList<QwcFileInfo>)),
                 pFileSearch, SLOT(updateResults(QList<QwcFileInfo>)) );
        connect( pFileSearch, SIGNAL(search(QString)), socket, SLOT(searchFiles(QString)) );
        connect( pFileSearch, SIGNAL(downloadFile(QString)), this, SLOT(search_download_file(QString)) );
        connect( pFileSearch, SIGNAL(revealFile(QString)), this, SLOT(search_reveal_file(QString)) );
    } else {
        int tmpIdx = pMainTabWidget->indexOf(pFileSearch);
        pMainTabWidget->setCurrentIndex(tmpIdx);
    }
}


/// Show the list of transfers.
void QwcSession::doActionTransfers()
{
    if( !pTranfersWindow ) {
        pTranfersWindow = new QwcFiletransferWidget(pConnWindow);
        int tmpIdx = pMainTabWidget->addTab(pTranfersWindow, QIcon(), tr("Transfers"));
        pMainTabWidget->setCurrentIndex(tmpIdx);
        // Model
        QwcFiletransferModel *tmpModel = new QwcFiletransferModel(pTranfersWindow->fTransfers);
        tmpModel->setSocket(socket);
        pTranfersWindow->fTransfers->setModel(tmpModel);
        pTranfersWindow->init();
        connect(pTranfersWindow, SIGNAL(transferCancelled(QwcFiletransferInfo)),
                socket, SLOT(cancelTransfer(QwcFiletransferInfo)) );

    } else {
        int tmpIdx = pMainTabWidget->indexOf(pTranfersWindow);
        pMainTabWidget->setCurrentIndex(tmpIdx);
    }
}


void QwcSession::transferError(QwcFiletransferInfo transfer)
{
    QMessageBox::critical(pConnWindow, tr("File Transfer Error"),
                          tr("The transfer of file '%1' could not be completed because of an error.\n"
                             "Possibly the file transfer TCP port is blocked by a firewall or the server "
                             "is configured incorrectly.").arg(transfer.fileName()));
}


void QwcSession::downloadFile(const QString &remotePath, const QString &localPath)
{
    QSettings s;
    socket->getFile(remotePath, localPath, true);
    bool isTransferring = socket->isTransferringFileOfType(WiredTransfer::TypeDownload);
    bool prefQueueEnabled = s.value("files/queue_local", false).toBool();
    if(!prefQueueEnabled || (prefQueueEnabled && !isTransferring))
        socket->runTransferQueue(WiredTransfer::TypeDownload);
}


void QwcSession::downloadFolder(const QString &remotePath, const QString &localPath)
{
    QSettings s;
    socket->getFolder(remotePath, localPath, true);
    bool isTransferring = socket->isTransferringFileOfType(WiredTransfer::TypeFolderDownload);
    bool prefQueueEnabled = s.value("files/queue_local", false).toBool();
    if(!prefQueueEnabled || (prefQueueEnabled && !isTransferring))
        socket->runTransferQueue(WiredTransfer::TypeFolderDownload);
}


void QwcSession::uploadFile(const QString &localPath, const QString &remotePath)
{
    QSettings s;
    socket->putFile(localPath, remotePath, true);
    bool isTransferring = socket->isTransferringFileOfType(WiredTransfer::TypeUpload);
    bool prefQueueEnabled = s.value("files/queue_local", false).toBool();
    if(!prefQueueEnabled || (prefQueueEnabled && !isTransferring))
        socket->runTransferQueue(WiredTransfer::TypeUpload);
}


void QwcSession::uploadFolder(const QString &localPath, const QString &remotePath)
{
    QSettings s;
    socket->putFolder(localPath, remotePath, true);
    bool isTransferring = socket->isTransferringFileOfType(WiredTransfer::TypeFolderUpload);
    bool prefQueueEnabled = s.value("files/queue_local", false).toBool();
    if(!prefQueueEnabled || (prefQueueEnabled && !isTransferring))
        socket->runTransferQueue(WiredTransfer::TypeFolderUpload);
}


void QwcSession::fileListingRecursiveDone(const QList<QwcFileInfo> items)
{
    qlonglong totalSize = 0;
    int totalFolders = 0;
    int totalFiles = 0;
    QListIterator<QwcFileInfo> i(items);
    while(i.hasNext()) {
        QwcFileInfo file = i.next();
        totalSize += file.size;
        totalFolders += file.type==WiredTransfer::Directory || file.type==WiredTransfer::Uploads
                        || file.type==WiredTransfer::DropBox ? 1 : 0;
        totalFiles += file.type==WiredTransfer::RegularFile ? 1 : 0;
    }

    QMessageBox::StandardButton button = QMessageBox::question(0,
        tr("Folder Download"),
        tr("You are about to download %1 file(s) and %2 folder(s) which occupy a total of %3.\nDo you want to begin the transfer?")
            .arg(totalFiles).arg(totalFolders).arg(QwcFileInfo::humanReadableSize(totalSize)),
        QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);

    if(button == QMessageBox::No) { return; }
}


/*! Show the private messenger and select the provided user, so that a message can be sent to the
    target user. This is usually connected to the double-click events of user lists.
*/
void QwcSession::showMessagerForUser(const QwcUserInfo targetUser)
{
    if (!privateMessager) { return; }
    if (pMainTabWidget->indexOf(privateMessager) == -1) {
        pMainTabWidget->setCurrentIndex(pMainTabWidget->addTab(privateMessager, tr("Private Messages")));
    } else {
        pMainTabWidget->setCurrentIndex(pMainTabWidget->indexOf(privateMessager));
    }
    privateMessager->handleNewMessage(targetUser, QString());
}
