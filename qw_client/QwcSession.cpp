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
#include <QProcess>
#include <QDir>

QwcSession::QwcSession(QObject *parent) : QObject(parent)
{
    initWiredSocket();
    initMainWindow();
    setConnectionToolButtonsEnabled(false);
    reloadPreferences();
}

QwcSession::~QwcSession()
{
    connectionWindow->deleteLater();
    mainChatWidget->deleteLater();

}



/*! Handle a protocol-related error and display it to the user.
*/
void QwcSession::handleProtocolError(Qw::ProtocolError error)
{
    if (error == Qw::ErrorLoginFailed) {
        connectWidget->resetForm();
        connectionStackedWidget->setCurrentIndex(0);
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
    QMessageBox::warning(connectionWindow, tr("Server Error"), errorText);
}


/*! Handle a low-level socket error.
*/
void QwcSession::handleSocketError(QAbstractSocket::SocketError error)
{
    if(error == QAbstractSocket::ConnectionRefusedError) {
        // Error occoured while connecting from the connecting window
        connectWidget->resetForm();
        QMessageBox::critical(mainChatWidget, tr("Connection Refused"),
                              tr("The connection was refused by the remote host. "
                                 "This normally means that there is no Wired server running on the remote machine."));
    } else if(error == QAbstractSocket::HostNotFoundError) {
        connectWidget->resetForm();
        QMessageBox::critical(mainChatWidget, tr("Host not found"),
                              tr("Could not connect to the remote server because the host name could not be resolved."));
    } else {
        // Disconnected suddenly
        qDebug() << error;
        connectionStackedWidget->setCurrentIndex(0);
        connectWidget->resetForm();
        connectWidget->startReconnecting();

        //connectionWindow->setEnabled(false);
        //connectionWindow->setWindowTitle( connectionWindow->windowTitle()+tr(" [Disconnected]") );

        triggerEvent("ServerDisconnected",QStringList());
    }
    setConnectionToolButtonsEnabled(false);
}


void QwcSession::initMainWindow()
{

    connectionWindow = new QwcConnectionMainWindow();
    connect(connectionWindow, SIGNAL(destroyed(QObject*)),
            this, SLOT(connectionWindowDestroyed(QObject*)) );

    // Use a stacked widget for the GUI switching
    connectionStackedWidget = new QStackedWidget(connectionWindow);
    connectionWindow->setCentralWidget(connectionStackedWidget);

    // Create the login dialog
    connectWidget = new QwcConnectWidget(connectionStackedWidget);
    connectionStackedWidget->addWidget(connectWidget);
    connectionStackedWidget->setCurrentIndex(0);

    connect(connectWidget, SIGNAL(userFinished(QString,QString,QString)),
            this, SLOT(onDoConnect(QString,QString,QString)) );
    connect(connectWidget, SIGNAL(onConnectAborted()),
            this, SLOT(onConnectAborted()));

    // Create the tab bar for the normal program use
    connectionTabWidget = new QTabWidget(connectionStackedWidget);
    connectionStackedWidget->addWidget(connectionTabWidget);
    connectionTabWidget->setDocumentMode(true);
    connectionTabWidget->setTabsClosable(true);

    connect(connectionTabWidget, SIGNAL(currentChanged(int)),
            this, SLOT(onTabBarCurrentChanged(int)) );

//    QToolButton *tabCloseButton = new QToolButton(stackedWidget);
//    tabCloseButton->setIcon(QIcon(":/icons/icn_close.png"));
//    tabCloseButton->setShortcut(QKeySequence("ctrl+w"));
//    tabCloseButton->setEnabled(false);
//    connect(tabCloseButton, SIGNAL(clicked()),
//            this, SLOT(onTabBarCloseButtonClicked()));
//    connectionTabWidget->setCornerWidget(tabCloseButton);

    // Create the initial tab for the main chat
    mainChatWidget = new QwcChatWidget(connectionWindow);
    /*: Text of the main connection tab in the connection window. */
    connectionTabWidget->addTab(mainChatWidget, tr("Chat"));
        // Connection window/Forum
    pUserListModel = new QwcUserlistModel(mainChatWidget);
    pUserListModel->setWiredSocket(socket);
    mainChatWidget->setSession(this);
    mainChatWidget->setUserListModel(pUserListModel);

    // Private messenger
//    privateMessager = new QwcPrivateMessager;
//    privateMessager->setParent(mainChatWidget, Qt::Window);

    setupConnections();
    connectionWindow->show();

//    return;


//    bannerSpace = 0;
//    bannerView = 0;
//    bannerSpace2 = 0;

    // Init the main window (which contains the toolbar and other things)


    // Init the main chat window (chat 0)
//    mainChatWidget = new QwcChatWidget;
//    mainChatWidget->setContentsMargins(0, 0, 0, 0);

    // Set up the container widget, which makes sure there is some spacing
//    pContainerWidget = new QWidget(connectionWindow);
//    pContainerLayout = new QStackedLayout(connectionWindow); //(pContainerWidget);
//#ifndef Q_WS_MAC
//    pContainerWidget->setContentsMargins(6, 6, 6, 6);
//#endif
//    pContainerWidget->setLayout(pContainerLayout);

    // Create the tab bar
//    connectionTabWidget = new QTabWidget(connectionWindow);
//    connectionTabWidget->clear();
//    connectionTabWidget->setVisible(false);
//#ifdef Q_WS_MAC
//    connectionTabWidget->setDocumentMode(true);
//#endif
//    connect(connectionTabWidget, SIGNAL(currentChanged(int)),
//            this, SLOT(onTabBarCurrentChanged(int)) );

    // The tab bar's close button, too




    // Add the widgets to the stacked layout
//    pContainerWidget->layout()->addWidget(connectWidget);
//    pContainerWidget->layout()->addWidget(connectionTabWidget);

    // Set the virtual widget
//    connectionWindow->setCentralWidget(connectionTabWidget);
//    connectionWindow->show();



    // Install the event filter in connectionWindow
    pEventFilter = new QwcEventFilter;
    connectionWindow->installEventFilter(pEventFilter);
    pEventFilter->setSocket(socket);
}


/// The user list was completely received after connecting.
void QwcSession::onUserlistComplete(int chatId)
{
    if (chatId != 1 && connectionStackedWidget->currentIndex() != 0) return;
    connectionStackedWidget->setCurrentIndex(1);
    setConnectionToolButtonsEnabled(true);
}


/// Tab bar close button clicked. Close the current widget.
void QwcSession::onTabBarCloseButtonClicked()
{
    int tmpIdx = connectionTabWidget->currentIndex();
    // We check if the chat tab is the current one so that not to close it
    if(connectionTabWidget->currentWidget() == mainChatWidget) return;
    connectionTabWidget->currentWidget()->close();
    connectionTabWidget->removeTab(tmpIdx);
}


void QwcSession::onTabBarCurrentChanged(int index)
{
//    return;

//    QWidget *tmpBtn = connectionTabWidget->cornerWidget();

    // We check if the chat tab is the current one so that not to close it
//    if(connectionTabWidget->currentWidget() != mainChatWidget) {
//        tmpBtn->setEnabled(true);
//    } else {
//        tmpBtn->setEnabled(false);
//    }

    // Icon removal for private chats
    QWidget *tmpWid = connectionTabWidget->widget(index);
    QwcChatWidget *tmpChat = qobject_cast<QwcChatWidget*>(tmpWid);
    if(tmpChat && tmpChat->pChatID != 1) {
        connectionTabWidget->setTabIcon(index, QIcon(":/icons/tab-idle.png"));
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
    connect(mainChatWidget, SIGNAL(userDoubleClicked(const QwcUserInfo)),
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

    // News
    connect(socket, SIGNAL(newsPosted(QString,QDateTime,QString)),
            this, SLOT(newsPosted(QString,QDateTime,QString)) );

    // File transfer signals
    connect(socket, SIGNAL(fileTransferDone(const QwcTransferInfo)), this, SLOT(transferDone(QwcTransferInfo)) );
    connect(socket, SIGNAL(fileTransferStarted(const QwcTransferInfo)), this, SLOT(transferStarted(QwcTransferInfo)) );
    connect(socket, SIGNAL(fileTransferSocketError(QAbstractSocket::SocketError)), this, SLOT(transferSocketError(QAbstractSocket::SocketError)));
    connect(socket, SIGNAL(fileTransferError(const QwcTransferInfo)), this, SLOT(transferError(QwcTransferInfo)) );
    connect(socket, SIGNAL(onFilesListRecursiveDone(const QList<QwcFileInfo>)), this, SLOT(fileListingRecursiveDone(const QList<QwcFileInfo>)));

    // Main Window actions
    //
    connect( connectionWindow->actionNewConnection, SIGNAL(triggered()), this, SLOT(doActionNewConnection()) );

    connect( connectionWindow->actionDisconnect, SIGNAL(triggered(bool)), this, SLOT(doActionDisconnect()));
    connect( connectionWindow->actionAccounts, SIGNAL(triggered(bool)), this, SLOT(doActionAccounts()) );
    connect( connectionWindow->actionNews, SIGNAL(triggered()), this, SLOT(doActionNews()) );
    connect( connectionWindow->actionServerInfo, SIGNAL(triggered(bool)), this, SLOT(doActionServerInfo()) );
    connect( connectionWindow->actionBroadcast, SIGNAL(triggered(bool)), this, SLOT(doActionBroadcast()) );
    connect( connectionWindow->actionFiles, SIGNAL(triggered(bool)), this, SLOT(doActionFiles()) );
    connect( connectionWindow->actionPreferences, SIGNAL(triggered(bool)), this, SLOT(doActionPreferences()));
    connect( connectionWindow->actionTrackers, SIGNAL(triggered(bool)), this, SLOT(doActionTrackers()) );
    connect( connectionWindow->actionSearch, SIGNAL(triggered(bool)), this, SLOT(doActionFileSearch()) );
    connect( connectionWindow->actionTransfers, SIGNAL(triggered(bool)), this, SLOT(doActionTransfers()) );
    //connect( connectionWindow->actionConnect, SIGNAL(triggered(bool)), this, SLOT(do_show_connect()));

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
        win->setParent(connectionWindow, Qt::Window);
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
    connectionWindow->actionAccounts->setEnabled(s.privEditAccounts);
}


/// A chat message was received, handle it.
void QwcSession::do_handle_chat_message(int theChat, int theUserID, QString theText, bool theIsAction)
{
    QwcUserInfo tmpUsr = socket->users[theUserID]; //socket->getUserByID(theUserID); // Find the user
    if(theChat==1) {
        // Public chat
        mainChatWidget->writeToChat(tmpUsr.userNickname, theText, theIsAction);

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
        int tmpIdx = connectionTabWidget->indexOf(chat);
        if( tmpIdx>-1 && connectionTabWidget->currentIndex()!=tmpIdx )
            connectionTabWidget->setTabIcon( tmpIdx, QIcon(":/icons/tab-content.png") );
    }
}


/// The chat topic was changed or received. Update the chat window.
void QwcSession::doHandleChatTopic(int theChatID, QString theNick, QString theLogin, QHostAddress theIP, QDateTime theDateTime, QString theTopic) {
    Q_UNUSED(theLogin);
    Q_UNUSED(theIP);
    if( theChatID==1 ) {
        // Public chat
        mainChatWidget->fTopic->setText(tr("Topic: %1\nSet By: %2 --- %3").arg(theTopic).arg(theNick).arg(theDateTime.toString()));
    } else if(pChats.contains(theChatID)) {
        // Topic of private chat
        QwcChatWidget *chat = pChats[theChatID];
        chat->fTopic->setText(tr("Topic: %1\nSet By: %2 --- %3").arg(theTopic).arg(theNick).arg(theDateTime.toString()));
    }
}



// Display received user info in a new window.
void QwcSession::doHandleUserInfo(QwcUserInfo theUser)
{

    for(int i=0; i<connectionTabWidget->count(); i++) {
        QwcUserInfoWidget *info = dynamic_cast<QwcUserInfoWidget*>(connectionTabWidget->widget(i));
        if(info && info->pUserID==theUser.pUserID) { // Found existing window
            connectionTabWidget->setCurrentIndex(i);
            info->setUser(theUser);
            return;
        } }

}


// Received an invitation to a private chat.
void QwcSession::doHandlePrivateChatInvitation(int theChatID, QwcUserInfo theUser)
{
    QMessageBox messageBox(connectionWindow);
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
    QwcChatWidget *chat = new QwcChatWidget(connectionWindow);
    QwcUserlistModel *model = new QwcUserlistModel(chat);
    chat->setSession(this);
    chat->pChatID = theChatID;
    chat->setUserListModel(model);
    chat->fBtnChat->setVisible(false);
    pChats[theChatID] = chat;

    int tmpIdx = connectionTabWidget->addTab(chat, tr("Private Chat"));
    connectionTabWidget->setCurrentIndex(tmpIdx);
    connectionTabWidget->setTabIcon(tmpIdx, QIcon(":/icons/tab-idle.png"));
}


/// Received the server info header. Update the connect window.
void QwcSession::onSocketServerInfo()
{
    connectWidget->setStatus(tr("Connecting. Starting session..."));
    if(connectWidget>0) connectWidget->setProgressBar(1,3);
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
    connectionWindow->actionReconnect->setEnabled(!theEnable);
    connectionWindow->actionDisconnect->setEnabled(theEnable);
    connectionWindow->actionServerInfo->setEnabled(theEnable);
    connectionWindow->actionChat->setEnabled(theEnable);
    connectionWindow->actionNews->setEnabled(theEnable);
    connectionWindow->actionFiles->setEnabled(theEnable);
    connectionWindow->actionTransfers->setEnabled(theEnable);
    connectionWindow->actionAccounts->setEnabled(theEnable);
    connectionWindow->actionInformation->setEnabled(theEnable);
    connectionWindow->actionNewsPost->setEnabled(theEnable);
    connectionWindow->actionBroadcast->setEnabled(theEnable);
    connectionWindow->actionSearch->setEnabled(theEnable);
}


/// Set the banner image to the toolbar of the main window.
void QwcSession::setBannerView(const QImage theBanner)
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

    bannerSpace = new QWidget(connectionWindow->fToolBar);
    bannerSpace->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    connectionWindow->fToolBar->addWidget(bannerSpace);

    bannerView = new QLabel(connectionWindow->fToolBar);
    bannerView->setPixmap(QPixmap::fromImage(theBanner));
    connectionWindow->fToolBar->addWidget(bannerView);

    bannerSpace2 = new QWidget(connectionWindow->fToolBar);
    bannerSpace2->setFixedWidth(10);
    connectionWindow->fToolBar->addWidget(bannerSpace2);

}

/// Connected to the download button in the search dialog.
void QwcSession::search_download_file(QString thePath)
{
    QSettings settings;
    doActionTransfers();
    QString tmpName = thePath.section("/",-1,-1);
    QDir tmpDownloadFolder(settings.value("files/download_dir", QDir::homePath()).toString());
    //downloadFile(thePath, tmpDownloadFolder.absoluteFilePath(tmpName));
}


void QwcSession::search_reveal_file(QString thePath)
{
    doActionFiles(thePath.section("/",0,-2));
}



void QwcSession::setTrayMenuAction(QMenu *action)
{
    pTrayMenuItem = action;
    pTrayMenuItem->setTitle(connectionWindow->windowTitle());

    QAction* tmpShowHide = new QAction(QIcon(":icons/icn_showhide.png"), tr("Show/Hide"), this );
    connect(tmpShowHide, SIGNAL(triggered(bool)), connectionWindow, SLOT(toggleVisible()) );


    pTrayMenuItem->addAction(connectionWindow->actionNews);
    pTrayMenuItem->addAction(connectionWindow->actionFiles);
    pTrayMenuItem->addAction(connectionWindow->actionTransfers);
    pTrayMenuItem->addAction(connectionWindow->actionSearch);
    pTrayMenuItem->addAction(connectionWindow->actionDisconnect);
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
        mainChatWidget->writeEventToChat(tmpMsg);

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


void QwcSession::transferStarted(QwcTransferInfo transfer)
{
    Q_UNUSED(transfer);
    //triggerEvent("TransferStarted", QStringList(transfer.fileName()));
}


void QwcSession::transferDone(QwcTransferInfo transfer)
{
    Q_UNUSED(transfer);
//    QSettings s;
//    if(s.value("files/queue_local", false).toBool()) {
//        qDebug() << this<< "Transfer completed. Unfreezing next.";
//        socket->runTransferQueue(transfer.type);
//    }
//
//    triggerEvent("TransferFinished", QStringList(transfer.fileName()));
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
    if(!connectWidget) return;
    connectionTabWidget->addTab(mainChatWidget, "Chat");
    connectWidget->setStatus(tr("Receiving user list..."));
    connectWidget->setProgressBar(2,3);
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
        mainChatWidget->resetForm();
        connectWidget->resetForm();
        socket->disconnectFromServer();
        connectionStackedWidget->setCurrentIndex(0); // go to connect dialog
        socket->serverInfo = QwServerInfo();
    }
}


/// Show the list of accounts and groups in a window.
void QwcSession::doActionAccounts()
{
    if(!pWinAccounts) {
        pWinAccounts = new QwcAccountsWidget(connectionWindow);
        int tmpIdx = connectionTabWidget->addTab(pWinAccounts, tr("Accounts"));
        connectionTabWidget->setCurrentIndex(tmpIdx);
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
        int tmpIdx = connectionTabWidget->indexOf(pWinAccounts);
        connectionTabWidget->setCurrentIndex(tmpIdx);
    }
}


/// Request the news from the server.
void QwcSession::doActionNews()
{
    if (pWinNews) {
        int tmpIdx = connectionTabWidget->indexOf(pWinNews);
        connectionTabWidget->setCurrentIndex(tmpIdx);
    }

    pWinNews = new QwcNewsWidget();

    connect(socket, SIGNAL(newsListingItem(QString,QDateTime&,QString)),
            pWinNews, SLOT(addNewsItem(QString, QDateTime&, QString)));
    connect(socket, SIGNAL(newsPosted(QString,QDateTime&,QString)),
            pWinNews, SLOT(addNewsItem(QString, QDateTime&, QString)));
    connect(socket, SIGNAL(newsListingDone()),
            pWinNews, SLOT(newsDone()));

    connect(pWinNews, SIGNAL(requestedRefresh()),
            socket, SLOT(getNews()));

    // Enable/Disable GUI elements depending on the privileges
    pWinNews->setupFromUser(socket->sessionUser);


    // We check for the proper purrmissions
    // Also, we don't want the button to be active if there are no news to clear

//    qDebug("News counter: %u",pWinNews->newsCount());
//
//    if (socket->sessionUser.privClearNews && pWinNews->newsCount()) {
//        connect( pWinNews, SIGNAL(userPurgedNews()), socket, SLOT(clearNews()) );
//    } else {
//        pWinNews->setDisabledClearButton(true);
//    }
//
//    if (socket->sessionUser.privPostNews) {
//        connect( pWinNews, SIGNAL(doPostNews(QString)), socket, SLOT(postNews(QString)) );
//    } else {
//        pWinNews->setDisabledPostButton(true);
//    }



    // Display the widget using a Tab
    int tmpIdx = connectionTabWidget->addTab(pWinNews, QIcon(), tr("News"));
    connectionTabWidget->setCurrentIndex(tmpIdx);

    socket->getNews();

}


/// Display the server information dialog.
void QwcSession::doActionServerInfo()
{
    if( !pServerWindow ) {
        pServerWindow = new QwcServerInfoWidget();
        pServerWindow->loadInfo(socket);
        connectionTabWidget->addTab(pServerWindow, tr("Server Info"));
    } else {
        pServerWindow->raise();
    }
}


/// Open a new broadcast message dialog.
void QwcSession::doActionBroadcast()
{
//    SendPrivateMessageWidget *msg = new SendPrivateMessageWidget();
//    msg->setParent(connectionWindow, Qt::Window);
//    msg->move( connectionWindow->pos() );
//    msg->show();
}


/// Open a new file browser and request the list of files (/) from the server.
void QwcSession::doActionFiles(QString thePath)
{
    if(!pWinFileBrowser) {
        pWinFileBrowser = new QwcFileBrowserWidget;
        pWinFileBrowser->setParent(connectionWindow, Qt::Window);
        pWinFileBrowser->initWithConnection(this);
        pWinFileBrowser->setPath(thePath);
        pWinFileBrowser->pModel->pWaitingForList = true;
        socket->getFileList(thePath);

        // Display the widget using a Tab
        connectionTabWidget->setCurrentIndex(connectionTabWidget->addTab(pWinFileBrowser, QIcon(), tr("Files")));

    } else {
        int tmpIdx = connectionTabWidget->indexOf(pWinFileBrowser);
        connectionTabWidget->setCurrentIndex(tmpIdx);
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
        pPrefsWindow->setParent(connectionWindow, Qt::Window);
        pPrefsWindow->move(connectionWindow->pos() );
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
        pWinTrackers->setParent(connectionWindow, Qt::Window);
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
        pFileSearch = new QwcFileSearchWidget(connectionWindow);
        int tmpIdx = connectionTabWidget->addTab(pFileSearch, tr("File Search"));
        connectionTabWidget->setCurrentIndex(tmpIdx);
        connect( socket, SIGNAL(fileSearchDone(QList<QwcFileInfo>)),
                 pFileSearch, SLOT(updateResults(QList<QwcFileInfo>)) );
        connect( pFileSearch, SIGNAL(search(QString)), socket, SLOT(searchFiles(QString)) );
        connect( pFileSearch, SIGNAL(downloadFile(QString)), this, SLOT(search_download_file(QString)) );
        connect( pFileSearch, SIGNAL(revealFile(QString)), this, SLOT(search_reveal_file(QString)) );
    } else {
        int tmpIdx = connectionTabWidget->indexOf(pFileSearch);
        connectionTabWidget->setCurrentIndex(tmpIdx);
    }
}


/// Show the list of transfers.
void QwcSession::doActionTransfers()
{
    if( !pTranfersWindow ) {
        pTranfersWindow = new QwcFiletransferWidget(connectionWindow);
        int tmpIdx = connectionTabWidget->addTab(pTranfersWindow, QIcon(), tr("Transfers"));
        connectionTabWidget->setCurrentIndex(tmpIdx);
        // Model
        QwcFiletransferModel *tmpModel = new QwcFiletransferModel(pTranfersWindow->transferList);
        tmpModel->setSocket(socket);
        pTranfersWindow->transferList->setModel(tmpModel);
        pTranfersWindow->init();

        connect(pTranfersWindow, SIGNAL(transferStopped(QwcTransferInfo)),
                socket, SLOT(pauseTransfer(QwcTransferInfo)) );

    } else {
        int tmpIdx = connectionTabWidget->indexOf(pTranfersWindow);
        connectionTabWidget->setCurrentIndex(tmpIdx);
    }
}


void QwcSession::transferError(QwcTransferInfo transfer)
{
    Q_UNUSED(transfer);
//    QMessageBox::critical(connectionWindow, tr("File Transfer Error"),
//                          tr("The transfer of file '%1' could not be completed because of an error.\n"
//                             "Possibly the file transfer TCP port is blocked by a firewall or the server "
//                             "is configured incorrectly.").arg(transfer.fileName()));
}



void QwcSession::downloadFolder(const QString &remotePath, const QString &localPath)
{
    Q_UNUSED(localPath);
    Q_UNUSED(remotePath);
//    QSettings s;
//    socket->getFolder(remotePath, localPath, true);
//    bool isTransferring = socket->isTransferringFileOfType(Qw::TransferTypeFolderDownload);
//    bool prefQueueEnabled = s.value("files/queue_local", false).toBool();
//    if(!prefQueueEnabled || (prefQueueEnabled && !isTransferring))
//        socket->runTransferQueue(Qw::TransferTypeFolderDownload);
}


void QwcSession::uploadFolder(const QString &localPath, const QString &remotePath)
{
    Q_UNUSED(remotePath);
    Q_UNUSED(localPath);
//    QSettings s;
//    socket->putFolder(localPath, remotePath, true);
//    bool isTransferring = socket->isTransferringFileOfType(Qw::TransferTypeFolderUpload);
//    bool prefQueueEnabled = s.value("files/queue_local", false).toBool();
//    if(!prefQueueEnabled || (prefQueueEnabled && !isTransferring))
//        socket->runTransferQueue(Qw::TransferTypeFolderUpload);
}


///*! Check the local transfer queue and initiate new transfers if there are free slots or local
//    queueing is disabled.
//*/
//void QwcSession::checkTransferQueue()
//{
//    QwcTransferInfo transfer = transferPool.takeFirstFromQueue();
//}


//void QwcSession::fileListingRecursiveDone(const QList<QwcFileInfo> items)
//{
//    qlonglong totalSize = 0;
//    int totalFolders = 0;
//    int totalFiles = 0;
//    QListIterator<QwcFileInfo> i(items);
//    while(i.hasNext()) {
//        QwcFileInfo file = i.next();
//        totalSize += file.size;
//        totalFolders += file.type==Qw::FileTypeFolder || file.type==Qw::FileTypeUploadsFolder
//                        || Qw::FileTypeDropBox ? 1 : 0;
//        totalFiles += file.type == Qw::FileTypeRegular ? 1 : 0;
//    }
//
//    QMessageBox::StandardButton button = QMessageBox::question(0,
//        tr("Folder Download"),
//        tr("You are about to download %1 file(s) and %2 folder(s) which occupy a total of %3.\nDo you want to begin the transfer?")
//            .arg(totalFiles).arg(totalFolders).arg(QwcFileInfo::humanReadableSize(totalSize)),
//        QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
//
//    if(button == QMessageBox::No) { return; }
//}


/*! Show the private messenger and select the provided user, so that a message can be sent to the
    target user. This is usually connected to the double-click events of user lists.
*/
void QwcSession::showMessagerForUser(const QwcUserInfo targetUser)
{
    if (!privateMessager) { return; }
    if (connectionTabWidget->indexOf(privateMessager) == -1) {
        connectionTabWidget->setCurrentIndex(connectionTabWidget->addTab(privateMessager, tr("Private Messages")));
    } else {
        connectionTabWidget->setCurrentIndex(connectionTabWidget->indexOf(privateMessager));
    }
    privateMessager->handleNewMessage(targetUser, QString());
}
