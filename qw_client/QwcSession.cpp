#include "QwcGlobals.h"
#include "QwcSession.h"
#include "QwcUserInfoWidget.h"
#include "QwcConnectWidget.h"
#include "QwcFiletransferModel.h"
#include "QwcFileBrowserWidget.h"
#include "QwcConnectionMainWindow.h"
#include "QwcChatWidget.h"
#include "QwcPrivateMessager.h"
#include "QwcNewsWidget.h"
#include "QwcAccountsWidget.h"
#include "QwcServerInfoWidget.h"
#include "QwcPreferencesWidget.h"
#include "QwcFiletransferWidget.h"

#include <QtGui/QMessageBox>
#include <QtCore/QProcess>

QwcSession::QwcSession(QObject *parent) :
        QObject(parent)
{
    initWiredSocket();
    initMainWindow();
    reloadPreferences();
}


QwcSession::~QwcSession()
{
    m_mainWindow->deleteLater();
    m_fileBrowserWidget->deleteLater();
}


QwcSocket* QwcSession::socket()
{ return m_socket; }


/*! The main window was closed. We should clean up here and say good bye.
*/
void QwcSession::connectionWindowDestroyed(QObject *)
{
    this->deleteLater();
}


/*! This is the main event filter, responsible to catch events such as closing the main window and
    keyboard input.
*/
bool QwcSession::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == m_mainWindow) {
        if (event->type() == QEvent::Close) {

            // If the socket is still connected, confirm the disconnection first.
            if (m_socket->sslSocket()->state() == QAbstractSocket::ConnectedState) {
                QMessageBox::StandardButton result = QMessageBox::question(m_mainWindow,
                    tr("Are you sure you want to disconnect?"),
                    tr("Closing this connection window will disconnect you from the remote server. "
                       "Are you sure you want to disconnect? Any ongoing file transfers will be aborted."),
                       QMessageBox::Close | QMessageBox::Cancel,
                       QMessageBox::Close);

                if (result == QMessageBox::Cancel) {
                    event->ignore();
                    return true;
                }
            }

            // Save the window geometry in order to restore it later on
            QSettings settings;
            settings.setValue("window_states/session", m_mainWindow->saveGeometry());
        }

    }
    return false;
}


/*! Handle a protocol-related error and display it to the user.
*/
void QwcSession::handleProtocolError(Qw::ProtocolError error)
{
    if (error == Qw::ErrorLoginFailed) {
        m_connectWidget->resetForm();
        connectionStackedWidget->setCurrentIndex(0);
    }

    // Display an error message
    QString errorText(tr("An unknown server error occoured. The error code is %1.").arg(error));
    switch (error) {
    case Qw::ErrorCommandFailed:
        errorText = tr("Command Failed. An undefined internal server error has occoured."); break;
    case Qw::ErrorCommandNotRecognized:
        errorText = tr("Command Not Recognized. Qwired sent a command that is unknown by the server."); break;
    case Qw::ErrorCommandNotImplemented:
        errorText = tr("Command Not Implemented. The last command is not implemented by the server."); break;
    case Qw::ErrorSyntaxError:
        errorText = tr("Syntax Error. The last command was not formatted correctly."); break;
    case Qw::ErrorLoginFailed:
        errorText = tr("Login Failed. Username and password were not accepted by the server."); break;
    case Qw::ErrorBanned:
        errorText = tr("Banned. You have been banned from the server. Please try connecting later."); break;
    case Qw::ErrorClientNotFound:
        errorText = tr("Client Not Found. The server could not find the client referred to."); break;
    case Qw::ErrorAccountNotFound:
        errorText = tr("Account Not Found. The server could not find the account referred to."); break;
    case Qw::ErrorAccountExists:
        errorText = tr("Account Exists. Could not create the account you specified."); break;
    case Qw::ErrorCannotBeDisconnected:
        errorText = tr("User can not be disconnected. The specified user can not be disconnected."); break;
    case Qw::ErrorPermissionDenied:
        errorText = tr("Permission Denied. You don't have sufficient privileges to execute the last command."); break;
    case Qw::ErrorFileOrDirectoryNotFound:
        errorText = tr("File or Directory not found. The last command could not be completed because the file or directory could not be found.");
        if (m_fileBrowserWidget != 0 && connectionTabWidget->currentWidget() == m_fileBrowserWidget) {
            m_fileBrowserWidget->stackedWidget->setCurrentWidget(m_fileBrowserWidget->pageBrowser);
        }
        break;
    case Qw::ErrorFileOrDirectoryExists:
        errorText = tr("The last command could not be completed because the file or directory already exists."); break;
    case Qw::ErrorChecksumMismatch:
        errorText = tr("Checksum Mismatch.");
    case Qw::ErrorQueueLimitExceeded:
        errorText = tr("Queue Limit Exceeded. Could not complete the last command because the server queue is full."); break;
    }

    // Dispatch an event
    triggerEvent("ServerError", QStringList(errorText));
    QMessageBox::warning(m_mainWindow, tr("Server Error"), errorText);
}


/*! Handle a low-level socket error.
*/
void QwcSession::handleSocketError(QAbstractSocket::SocketError error)
{
    if(error == QAbstractSocket::ConnectionRefusedError) {
        // Error occoured while connecting from the connecting window
        m_connectWidget->resetForm();
        QMessageBox::critical(m_publicChat, tr("Connection Refused"),
                              tr("The connection was refused by the remote host. "
                                 "This normally means that there is no Wired server running on the remote machine."));
    } else if(error == QAbstractSocket::HostNotFoundError) {
        m_connectWidget->resetForm();
        QMessageBox::critical(m_publicChat, tr("Host not found"),
                              tr("Could not connect to the remote server because the host name could not be resolved."));
    } else {
        // Disconnected suddenly
        qDebug() << error;
        connectionStackedWidget->setCurrentIndex(0);
        m_connectWidget->resetForm();
        m_connectWidget->startReconnecting();
        triggerEvent("ServerDisconnected",QStringList());
    }
}


/*! Initialize the main connection window.
*/
void QwcSession::initMainWindow()
{
    m_mainWindow = new QwcConnectionMainWindow();
    m_mainWindow->installEventFilter(this);

    // Restore the window geometry
    QSettings settings;
    if (settings.contains("window_states/session")) {
        m_mainWindow->restoreGeometry(settings.value("window_states/session").toByteArray());
    }

    connect(m_mainWindow, SIGNAL(destroyed(QObject*)),
            this, SLOT(connectionWindowDestroyed(QObject*)) );

    // Use a stacked widget for the GUI switching
    connectionStackedWidget = new QStackedWidget(m_mainWindow);
    m_mainWindow->setCentralWidget(connectionStackedWidget);

    // Create the login dialog
    m_connectWidget = new QwcConnectWidget(connectionStackedWidget);
    connectionStackedWidget->addWidget(m_connectWidget);
    connectionStackedWidget->setCurrentIndex(0);

    connect(m_connectWidget, SIGNAL(userFinished(QString,QString,QString)),
            this, SLOT(onDoConnect(QString,QString,QString)) );
    connect(m_connectWidget, SIGNAL(onConnectAborted()),
            this, SLOT(onConnectAborted()));

    // Create the tab bar for the normal program use
    connectionTabWidget = new QTabWidget(connectionStackedWidget);
    connect(connectionTabWidget, SIGNAL(tabCloseRequested(int)),
            this, SLOT(onTabBarCloseRequested(int)));
    connect(connectionTabWidget, SIGNAL(currentChanged(int)),
            this, SLOT(onTabBarCurrentChanged(int)) );
    connectionStackedWidget->addWidget(connectionTabWidget);
    connectionTabWidget->setDocumentMode(true);
    connectionTabWidget->setTabsClosable(true);

    // Create the initial tab for the main chat
    m_publicChat = new QwcChatWidget(m_mainWindow);
    m_publicChat->setAttribute(Qt::WA_DeleteOnClose, false);
    /*: Text of the main connection tab in the connection window. */
    connectionTabWidget->addTab(m_publicChat, tr("Chat"));
    m_publicChat->setSession(this);

    // Messenger
    m_privateMessagerWidget = new QwcPrivateMessager();
    m_privateMessagerWidget->setParent(m_mainWindow, Qt::Window);
    m_privateMessagerWidget->setSocket(m_socket);

    setupConnections();
    m_mainWindow->show();
}


/// The user list was completely received after connecting.
void QwcSession::onUserlistComplete(int chatId)
{
    if (chatId == Qwc::PUBLIC_CHAT) { return; }
    if (connectionStackedWidget->currentIndex() == 0) { return; }
    connectionStackedWidget->setCurrentIndex(1);
}


/*! Handle a new private message from the server.
*/
void QwcSession::handlePrivateMessage(QwcUserInfo sender, QString text)
{
    m_privateMessagerWidget->handleNewMessage(sender, text);
    if (connectionTabWidget->indexOf(m_privateMessagerWidget) > -1) {
        if (connectionTabWidget->currentWidget() != m_privateMessagerWidget) {
            connectionTabWidget->setTabIcon(connectionTabWidget->indexOf(m_privateMessagerWidget),
                                            QIcon(":/icons/tab-content.png"));
        }
    } else {
        showMessagerForUser(sender);
        m_privateMessagerWidget->handleNewMessage(sender, text);
    }
}


/// Tab bar close button clicked. Close the current widget.
void QwcSession::onTabBarCloseButtonClicked()
{
    int index = connectionTabWidget->currentIndex();
    // We check if the chat tab is the current one so that not to close it
    if(connectionTabWidget->currentWidget() == m_publicChat) return;
    connectionTabWidget->currentWidget()->close();
    connectionTabWidget->removeTab(index);
}


void QwcSession::onTabBarCurrentChanged(int index)
{
    // Icon removal for private chats
    QWidget *widget = connectionTabWidget->widget(index);
    QwcChatWidget *chatWidget = qobject_cast<QwcChatWidget*>(widget);
    if (chatWidget && chatWidget->chatId() != Qwc::PUBLIC_CHAT) {
        connectionTabWidget->setTabIcon(index, QIcon(":/icons/tab-idle.png"));
    }
    if (widget == m_privateMessagerWidget) {
        connectionTabWidget->setTabIcon(index, QIcon(":/icons/tab-idle.png"));
    }
}


/*! The user clicked a close box of a tab in the tab widget.
*/
void QwcSession::onTabBarCloseRequested(int index)
{
    QWidget *tabWidget = connectionTabWidget->widget(index);
    if (!tabWidget) { return; }
    connectionTabWidget->removeTab(index);
}


/*! Handle a 'action' from the main window.
*/
void QwcSession::handleMainWindowAction(QwcConnectionMainWindow::TriggeredAction action)
{
    if (action == QwcConnectionMainWindow::TriggeredActionAccounts) {
        if(!m_accountsWidget) {
            m_accountsWidget = new QwcAccountsWidget(m_mainWindow);

            connect( m_socket, SIGNAL(receivedAccountList(QStringList)),
                     m_accountsWidget, SLOT(appendUserNames(QStringList)));
            connect( m_socket, SIGNAL(receivedAccountGroupList(QStringList)),
                     m_accountsWidget, SLOT(appendGroupNames(QStringList)));

            connect(m_socket, SIGNAL(userSpecReceived(QwcUserInfo)),
                    m_accountsWidget, SLOT(loadFromAccount(QwcUserInfo)));
            connect(m_socket, SIGNAL(groupSpecReceived(QwcUserInfo)),
                    m_accountsWidget, SLOT(loadFromAccount(QwcUserInfo)));

            connect(m_accountsWidget, SIGNAL(userSpecRequested(QString)),
                    m_socket, SLOT(readUser(QString)));
            connect(m_accountsWidget, SIGNAL(groupSpecRequested(QString)),
                    m_socket, SLOT(readGroup(QString)));


            connect(m_accountsWidget, SIGNAL(accountCreated(QwcUserInfo)),
                    m_socket, SLOT(createUser(QwcUserInfo)));
            connect(m_accountsWidget, SIGNAL(accountEdited(QwcUserInfo)),
                    m_socket, SLOT(editUser(QwcUserInfo)));
            connect(m_accountsWidget, SIGNAL(accountDeleted(QString)),
                    m_socket, SLOT(deleteUser(QString)));

            connect(m_accountsWidget, SIGNAL(groupCreated(QwcUserInfo)),
                    m_socket, SLOT(createGroup(QwcUserInfo)));
            connect(m_accountsWidget, SIGNAL(groupEdited(QwcUserInfo)),
                    m_socket, SLOT(editGroup(QwcUserInfo)));
            connect(m_accountsWidget, SIGNAL(groupDeleted(QString)),
                    m_socket, SLOT(deleteGroup(QString)));

            connect(m_accountsWidget, SIGNAL(refreshedAccountsAndGroups()),
                    m_socket, SLOT(getGroups()));
            connect(m_accountsWidget, SIGNAL(refreshedAccountsAndGroups()),
                    m_socket, SLOT(getUsers()));

            m_socket->getGroups();
            m_socket->getUsers();
        }

        // Display the widget if it is not in the tab widget
        if (connectionTabWidget->indexOf(m_accountsWidget) == -1) {
            connectionTabWidget->addTab(m_accountsWidget, tr("Accounts"));
        }
        connectionTabWidget->setCurrentWidget(m_accountsWidget);


    } else if (action == QwcConnectionMainWindow::TriggeredActionDisconnect) {
        m_publicChat->resetForm();
        m_connectWidget->resetForm();
        m_socket->disconnectFromServer();
        connectionStackedWidget->setCurrentIndex(0); // go to connect dialog
        m_socket->serverInfo = QwServerInfo();


    } else if (action == QwcConnectionMainWindow::TriggeredActionMessages) {
        showMessagerForUser(QwcUserInfo());

    } else if (action == QwcConnectionMainWindow::TriggeredActionPreferences) {
        if (!m_preferencesWidget) {
            m_preferencesWidget = new QwcPreferencesWidget;
            m_preferencesWidget->setParent(m_mainWindow, Qt::Window);
            m_preferencesWidget->move(m_mainWindow->pos());
            m_preferencesWidget->show();
        } else {
            m_preferencesWidget->raise();
        }

    } else if (action == QwcConnectionMainWindow::TriggeredActionNews) {
        if (!m_newsWidget) {
            m_newsWidget = new QwcNewsWidget();
            m_newsWidget->setParent(m_mainWindow, Qt::Window);
            m_newsWidget->setSocket(m_socket);
            m_newsWidget->setupFromUser(m_socket->sessionUser);
            m_socket->getNews();
        }

        if (connectionTabWidget->indexOf(m_newsWidget) == -1) {
            connectionTabWidget->addTab(m_newsWidget, tr("News"));
        }
        connectionTabWidget->setCurrentWidget(m_newsWidget);


    } else if (action == QwcConnectionMainWindow::TriggeredActionServerInformation) {
        if (!m_serverInfoWidget) {
            m_serverInfoWidget = new QwcServerInfoWidget();
            m_serverInfoWidget->setInformationFromSocket(m_socket);
        }
        if (connectionTabWidget->indexOf(m_serverInfoWidget) == -1) {
            connectionTabWidget->addTab(m_serverInfoWidget, tr("Server Info"));
        }
        connectionTabWidget->setCurrentWidget(m_serverInfoWidget);

    } else if (action == QwcConnectionMainWindow::TriggeredActionChat) {
        Q_ASSERT(m_publicChat);
        if (connectionTabWidget->indexOf(m_publicChat) == -1) {
            connectionTabWidget->addTab(m_publicChat, tr("Chat"));
        }
        connectionTabWidget->setCurrentWidget(m_publicChat);


    } else if (action == QwcConnectionMainWindow::TriggeredActionTransfers) {
        if (!m_transfersWidget) {
            m_transfersWidget = new QwcFiletransferWidget();
            // Connect the signals
            connect(m_transfersWidget, SIGNAL(transferStopped(QwcTransferInfo)),
                    m_socket, SLOT(pauseTransfer(QwcTransferInfo)));
            connect(m_transfersWidget, SIGNAL(transferResumed(QwcTransferInfo)),
                    m_socket, SLOT(resumeTransfer(QwcTransferInfo)));

            // Ensure that the model can access data from the socket, like the current list of transfers.
            m_transfersWidget->transferModel()->setSocket(m_socket);
        }
        if (connectionTabWidget->indexOf(m_transfersWidget) == -1) {
            connectionTabWidget->addTab(m_transfersWidget, tr("Transfers"));
        }
        connectionTabWidget->setCurrentWidget(m_transfersWidget);


    } else if (action == QwcConnectionMainWindow::TriggeredActionNewConnection) {
        QwcSingleton *singleton = &WSINGLETON::Instance();
        QwcSession *newSession = new QwcSession();
        singleton->addSession(newSession);
    }


}


/// Set up connections between objects in this class.
void QwcSession::setupConnections()
{
    // Socket connections
    //
    connect(m_socket, SIGNAL(protocolError(Qw::ProtocolError)),
            this, SLOT(handleProtocolError(Qw::ProtocolError)));

    // Messager
    //
    connect(m_socket, SIGNAL(privateMessage(QwcUserInfo,QString)),
            this, SLOT(handlePrivateMessage(QwcUserInfo,QString)));
    connect(m_publicChat, SIGNAL(userDoubleClicked(const QwcUserInfo)),
            this, SLOT(showMessagerForUser(const QwcUserInfo)));

    connect(m_socket, SIGNAL(receivedChatMessage(int,int,QString,bool)),
            this, SLOT(do_handle_chat_message(int,int,QString,bool)) );

    connect(m_socket, SIGNAL(onChatTopic(int, QString, QString, QHostAddress, QDateTime, QString)),
            this,   SLOT(handleChatTopic(int, QString, QString, QHostAddress, QDateTime, QString)) );

    connect(m_socket, SIGNAL(onServerLoginSuccessful()),
            this, SLOT(onLoginSuccessful()) );

    connect(m_socket, SIGNAL(receivedUserlist(int)),
            this, SLOT(onUserlistComplete(int)) );

    connect(m_socket, SIGNAL(userInformation(QwcUserInfo)),
            this, SLOT(handleUserInformation(QwcUserInfo)));

    connect(m_socket, SIGNAL(privateChatInvitation(int,QwcUserInfo)),
            this, SLOT(doHandlePrivateChatInvitation(int,QwcUserInfo)) );
    connect(m_socket, SIGNAL(privateChatCreated(int)),
            this, SLOT(createChatWidget(int)) );

    connect(m_socket, SIGNAL(onServerInformation()),
            this, SLOT(handleServerInformation()) );

    connect(m_socket, SIGNAL(fileInformation(QwcFileInfo)),
            this, SLOT(handleFileInformation(QwcFileInfo)) );

    connect(m_socket, SIGNAL(broadcastMessage(QwcUserInfo, QString)),
            this, SLOT(handleBroadcastMessage(QwcUserInfo,QString)));

    connect(m_socket, SIGNAL(userJoinedRoom(int,QwcUserInfo)),
            this, SLOT(userJoined(int,QwcUserInfo)) );
    connect(m_socket, SIGNAL(userLeftRoom(int,QwcUserInfo)),
            this, SLOT(userLeft(int,QwcUserInfo)) );
    connect(m_socket, SIGNAL(userChanged(QwcUserInfo,QwcUserInfo)),
            this, SLOT(userChanged(QwcUserInfo,QwcUserInfo)) );


    // Main Window actions
    //
    connect(m_mainWindow, SIGNAL(ActionTriggered(QwcConnectionMainWindow::TriggeredAction)),
            this, SLOT(handleMainWindowAction(QwcConnectionMainWindow::TriggeredAction)));




    // Notification manager
    //
    QwcSingleton *tmpS = &WSINGLETON::Instance();
    connect(tmpS, SIGNAL(applicationSettingsChanged()),
            this, SLOT(reloadPreferences()));

}


/*! Handle file information (STAT) returned from the server.
    Normally this is passed to a file browser, which then displays the information about a file.
*/
void QwcSession::handleFileInformation(QwcFileInfo file)
{
    if (!m_fileBrowserWidget) { return; }
    m_fileBrowserWidget->setFileInformation(file);
}



/// A chat message was received, handle it.
void QwcSession::do_handle_chat_message(int theChat, int theUserID, QString theText, bool theIsAction)
{
    QwcUserInfo tmpUsr = m_socket->users[theUserID]; //socket->getUserByID(theUserID); // Find the user
    if(theChat==1) {
        // Public chat
        m_publicChat->writeTextToChat(tmpUsr, theText, theIsAction);

        // Trigger the event
        QStringList tmpParams;
        tmpParams << tmpUsr.userNickname;
        if(theIsAction) tmpParams << QString("*** %1 %2").arg(tmpUsr.userNickname).arg(theText);
        else tmpParams << theText;
        triggerEvent("ChatReceived", tmpParams);

    } else {
        // Handle private chat
        if(!m_chatWidgets.contains(theChat)) {
            qDebug() << "QwcSession: Warning: Unknown chat with id"<<theChat;
            return;
        }
        QwcChatWidget *chat = m_chatWidgets[theChat];
        chat->writeTextToChat(tmpUsr, theText, theIsAction);

        // Find the index on the tab panel
        int tmpIdx = connectionTabWidget->indexOf(chat);
        if( tmpIdx >- 1 && connectionTabWidget->currentIndex() != tmpIdx )
            connectionTabWidget->setTabIcon(tmpIdx, QIcon(":/icons/tab-content.png"));
    }
}


/*! Display the chat topic of a certain chat in the chat widget.
*/
void QwcSession::handleChatTopic(int chatId, QString nickname, QString login,
                                 QHostAddress userIp, QDateTime date, QString topic)
{
    if (chatId == 1) {
        // Public chat
        m_publicChat->setTopic(topic, nickname, date);
    } else if (m_chatWidgets.contains(chatId)) {
        m_chatWidgets[chatId]->setTopic(topic, nickname, date);
    }
}


/*! Handle information about a user (INFO).
*/
void QwcSession::handleUserInformation(QwcUserInfo user)
{
    QwcUserInfoWidget *infoWidget = NULL;
    if (m_userInfoWidgets.contains(user.pUserID)) {
        infoWidget = m_userInfoWidgets.value(user.pUserID);
    } else {
        infoWidget = new QwcUserInfoWidget(m_mainWindow);
        m_userInfoWidgets[user.pUserID] = infoWidget;
        connectionTabWidget->addTab(infoWidget, tr("Info: %1").arg(user.userNickname));
        connectionTabWidget->setCurrentWidget(infoWidget);
    }

    if (!infoWidget) { return; }
    infoWidget->setUser(user);
}


// Received an invitation to a private chat.
void QwcSession::doHandlePrivateChatInvitation(int theChatID, QwcUserInfo theUser)
{
    QMessageBox messageBox(m_mainWindow);
    messageBox.setWindowTitle(tr("Private Chat Invitation"));
    messageBox.setText(tr("%1 has invited you to a private chat.\nJoin to open a separate private chat with %1.").arg(theUser.userNickname) );
    messageBox.setIconPixmap( QPixmap(":/icons/btn_chat.png") );
    QAbstractButton *ignoreButton = messageBox.addButton(tr("Ignore"), QMessageBox::DestructiveRole);
    QAbstractButton *rejectButton = messageBox.addButton(tr("Reject"), QMessageBox::RejectRole);
    QAbstractButton *joinButton = messageBox.addButton(tr("Join"), QMessageBox::AcceptRole);

    Q_UNUSED(ignoreButton);

    messageBox.exec();
    if (messageBox.clickedButton() == joinButton) {
        // Create a new chat
        createChatWidget(theChatID);
        m_socket->joinChat(theChatID);
    } else if( messageBox.clickedButton() == rejectButton ) {
        // Reject the chat.
        m_socket->rejectChat(theChatID);
    } else {
        // Ignore
    }
}


/*! Create a new chat widget and add it to the tab panel.
*/
void QwcSession::createChatWidget(int chatId)
{
    QwcChatWidget *chat = new QwcChatWidget();
    chat->setParent(m_mainWindow, Qt::Window);
    chat->setSession(this);
    chat->setChatId(chatId);

    m_chatWidgets[chatId] = chat;

    int index = connectionTabWidget->addTab(chat, tr("Private Chat"));
    connectionTabWidget->setCurrentIndex(index);
    connectionTabWidget->setTabIcon(index, QIcon(":/icons/tab-idle.png"));
}


/*! Server Information was received from the remote server.
*/
void QwcSession::handleServerInformation()
{
    // Update the window title
    /*: This is the translateable string for the server name in the window title bar of the
        connection window. */
    m_mainWindow->setWindowTitle(tr("Qwired - %1").arg(m_socket->serverInfo.name));

    // Update the progress bar
    if (m_connectWidget) {
        m_connectWidget->setProgressBar(1,3);
        m_connectWidget->setStatus(tr("Connecting. Starting session..."));
    }

    // Update the try icon menu
    if (m_systemTrayMenu) {
        m_systemTrayMenu->setTitle(m_socket->serverInfo.name);
    }
}


/// Connect to the remote server.
void QwcSession::onDoConnect(QString theHost, QString theLogin, QString thePassword)
{
    if (theLogin.isEmpty()) {
        // Log in as guest if no login/password defined.
        m_socket->setUserAccount("guest", "");
    } else {
        m_socket->setUserAccount(theLogin,thePassword);
    }
    m_socket->connectToServer(theHost);
}


void QwcSession::setTrayMenuAction(QMenu *action)
{
//    m_systemTrayMenu = action;
//    m_systemTrayMenu->setTitle(m_mainWindow->windowTitle());
//
//    QAction* tmpShowHide = new QAction(QIcon(":icons/icn_showhide.png"), tr("Show/Hide"), this );
//    connect(tmpShowHide, SIGNAL(triggered(bool)), m_mainWindow, SLOT(toggleVisible()) );


//    m_systemTrayMenu->addAction(m_mainWindow->actionNews);
//    m_systemTrayMenu->addAction(m_mainWindow->actionFiles);
//    m_systemTrayMenu->addAction(m_mainWindow->actionTransfers);
//    m_systemTrayMenu->addAction(m_mainWindow->actionDisconnect);
//    m_systemTrayMenu->addAction(tmpShowHide);
}


/**
 * Handle a specific event from the current session.
 * @param event The event that occoured
 * @param parameters A list of parameters which describe detailed information about the event.
 */
void QwcSession::triggerEvent(QString event, QStringList params)
{
    QSettings conf;

    QHash<QString,QString> eventMessages;
    eventMessages["ServerConnected"] = tr("Connected");
    eventMessages["ServerDisconnected"] = tr("Disconnected");
    eventMessages["ServerError"] = tr("Server Error: %1");
    eventMessages["UserJoined"] = tr("%1 joined");
    eventMessages["UserChangedNick"] = tr("\"%1\" is now known as \"%2\"");
    eventMessages["UserChangedStatus"] = tr("%1 changed status to \"%2\"");
    eventMessages["UserLeft"] = tr("%1 left");
    eventMessages["ChatReceived"] = tr("Chat from %1:\n%2"); // tray msg only
    eventMessages["MessageReceived"] = tr("Message from %1:\n%2"); // tray msg only
    eventMessages["NewsPosted"] = tr("News Posted from %1:\n%2"); // tray msg only
    eventMessages["BroadcastMessageReceived"] = tr("Broadcast from %1:\n%2"); // tray msg only
    eventMessages["TransferStarted"] = tr("Transfer Started: %1");
    eventMessages["TransferFinished"] = tr("Transfer Finished: %1");

    QString eventMessage = eventMessages[event]
                           .arg(params.value(0))
                           .arg(params.value(1))
                           .arg(params.value(2));


    // Write to the chat
    if (conf.value(QString("events/%1/chat").arg(event)).toBool()) {
        m_publicChat->writeEventToChat(eventMessage, event);
    }

    // Show a message in the system tray
    if(conf.value(QString("events/%1/traymsg").arg(event)).toBool()) {
        QwcSingleton *tmpS = &WSINGLETON::Instance();
        tmpS->pTrayIcon->showMessage( m_socket->serverInfo.name, eventMessage );
    }

    // Play sound
//    if(conf.contains(QString("events/%1/sound").arg(event)) ) {
//        QSound::play(conf.value(QString("events/%1/sound").arg(event)).toString());
//    }

    // Execute a system command
    if(conf.value(QString("events/%1/syscmd_enabled").arg(event), false).toBool()) {
        QString command = conf.value(QString("events/%1/syscmd").arg(event)).toString();
        // Protect the noobs.
        if (command.contains("rm", Qt::CaseSensitive)) { return; }
        if(!command.isEmpty()) {
            QProcess process;
            process.start(command);
        }
    }


}


void QwcSession::userJoined(int theChat, QwcUserInfo theUser)
{
    if (theChat != Qwc::PUBLIC_CHAT) { return; }
    triggerEvent("UserJoined", QStringList() << theUser.userNickname);
}


void QwcSession::userLeft(int theChat, QwcUserInfo theUser)
{
    if (theChat != Qwc::PUBLIC_CHAT) { return; }
    triggerEvent("UserLeft", QStringList() << theUser.userNickname);
}


void QwcSession::userChanged(QwcUserInfo theOld, QwcUserInfo theNew)
{
    if(theOld.userNickname != theNew.userNickname)
        triggerEvent("UserChangedNick", QStringList() << theOld.userNickname << theNew.userNickname);
    if(theOld.userStatus != theNew.userStatus)
        triggerEvent("UserChangedStatus", QStringList() << theNew.userNickname << theNew.userStatus);
}


/*! Connected to the newsPosted() signal of QwcSocket.
*/
void QwcSession::newsPosted(QString nickname, QDateTime time, QString post)
{
    Q_UNUSED(time);
    triggerEvent("NewsPosted", QStringList() << nickname << post);


}


/*! Write a received broadcast message to the chat.
*/
void QwcSession::handleBroadcastMessage(QwcUserInfo theUser, QString theMessage)
{
    triggerEvent("BroadcastMessageReceived", QStringList() << theUser.userNickname << theMessage);
    if (!m_publicChat) { return; }
    m_publicChat->writeBroadcastToChat(theUser, theMessage);
}


/// The login was successful, switch to forum view.
void QwcSession::onLoginSuccessful()
{
    if(!m_connectWidget) return;
    connectionTabWidget->addTab(m_publicChat, "Chat");
    m_connectWidget->setStatus(tr("Receiving user list..."));
    m_connectWidget->setProgressBar(2,3);
    triggerEvent("ServerConnected", QStringList());

}


/// Initialize the main socket and load settings
void QwcSession::initWiredSocket()
{
    m_socket = new QwcSocket(this);

    QSettings settings;
    m_socket->setUserStatus(settings.value("general/status","Qwired Newbie").toString());
    m_socket->setNickname(settings.value("general/nickname", tr("Unnamed")).toString());

    QImage tmpIcon = settings.value("general/icon", QImage(":/icons/qwired_logo_32.png")).value<QImage>();
    m_socket->setUserIcon(tmpIcon);

    connect(m_socket->sslSocket(), SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(handleSocketError(QAbstractSocket::SocketError)));

    connect(m_socket, SIGNAL(fileTransferDone(const QwcTransferInfo &)),
            this, SLOT(handleTransferComplete(const QwcTransferInfo &)));
}


/*! Reload the preferences and check what needs to be done to synchronize the new settings with the
    rest of the application.
*/
void QwcSession::reloadPreferences()
{
    QSettings settings;

    if (m_socket->sessionUser.userNickname != settings.value("general/nickname", tr("Unnamed")).toString()) {
        m_socket->setNickname(settings.value("general/nickname").toString());
    }

    if (m_socket->sessionUser.userStatus != settings.value("general/status", tr("Qwired Newbie")).toString()) {
        m_socket->setUserStatus(settings.value("general/status").toString());
    }

    QPixmap newIcon = settings.value("general/icon",
                                     QPixmap(":/icons/qwired_logo_32.png")).value<QPixmap>();
    m_socket->setUserIcon(newIcon.toImage());
}


void QwcSession::onConnectAborted()
{ m_socket->disconnectFromServer(); }


/*! A file transfer is complete.
    We should check if we have a preview file transfer, then display the file.
*/
void QwcSession::handleTransferComplete(const QwcTransferInfo &transfer)
{
    if (transfer.file.previewFileAfterTransfer) {
        qDebug() << this << "Previewing" << transfer.file.localAbsolutePath;
    }
}



/*! Show the private messenger and select the provided user, so that a message can be sent to the
    target user. This is usually connected to the double-click events of user lists.
*/
void QwcSession::showMessagerForUser(const QwcUserInfo targetUser)
{
    if (!m_privateMessagerWidget) { return; }
    if (connectionTabWidget->indexOf(m_privateMessagerWidget) == -1) {
        connectionTabWidget->setCurrentIndex(
                connectionTabWidget->addTab(m_privateMessagerWidget, QIcon(":/icons/tab-idle.png"),
                                            tr("Messages")));
    } else {
        connectionTabWidget->setCurrentWidget(m_privateMessagerWidget);
    }
    m_privateMessagerWidget->handleNewMessage(targetUser, QString());
}
