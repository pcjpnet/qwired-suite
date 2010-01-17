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
#include <QtGui/QSound>
#include <QtCore/QProcess>

QwcSession::QwcSession(QObject *parent) :
        QObject(parent)
{
    QwcSingleton *singleton = &WSINGLETON::Instance();
    connect(singleton, SIGNAL(applicationSettingsChanged()),
            this, SLOT(reloadPreferences()));

    initializeSocket();
    initializeMainWindow();
    reloadPreferences();
}

QwcSession::~QwcSession()
{
    m_mainWindow->deleteLater();
}


QwcSocket* QwcSession::socket()
{ return m_socket; }


/*! Initialize the QwcSocket object, which is the gateway to the server.
*/
void QwcSession::initializeSocket()
{
    m_socket = new QwcSocket(this);

    connect(m_socket->sslSocket(), SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(handleSocketError(QAbstractSocket::SocketError)));

    connect(m_socket, SIGNAL(receivedUserlist(int)),
            this, SLOT(handleSocketUserlistComplete(int)));
    connect(m_socket, SIGNAL(protocolError(Qw::ProtocolError)),
            this, SLOT(handleProtocolError(Qw::ProtocolError)));
    connect(m_socket, SIGNAL(receivedChatMessage(int,int,QString,bool)),
            this, SLOT(handleSocketChatMessage(int,int,QString,bool)));

    connect(m_socket, SIGNAL(userJoinedRoom(int,QwcUserInfo)),
            this, SLOT(userJoined(int,QwcUserInfo)) );
    connect(m_socket, SIGNAL(userLeftRoom(int,QwcUserInfo)),
            this, SLOT(userLeft(int,QwcUserInfo)));
    connect(m_socket, SIGNAL(userChanged(QwcUserInfo,QwcUserInfo)),
            this, SLOT(userChanged(QwcUserInfo,QwcUserInfo)));

    connect(m_socket, SIGNAL(userInformation(QwcUserInfo)),
            this, SLOT(handleUserInformation(QwcUserInfo)));
    connect(m_socket, SIGNAL(privateChatInvitation(int,QwcUserInfo)),
            this, SLOT(handleSocketChatInvitation(int,QwcUserInfo)) );
    connect(m_socket, SIGNAL(privateChatCreated(int)),
            this, SLOT(createChatWidget(int)));
    connect(m_socket, SIGNAL(fileInformation(QwcFileInfo)),
            this, SLOT(handleFileInformation(QwcFileInfo)));

    connect(m_socket, SIGNAL(privateMessage(QwcUserInfo,QString)),
            this, SLOT(handlePrivateMessage(QwcUserInfo,QString)));
}


/*! Initialize the main connection window.
*/
void QwcSession::initializeMainWindow()
{
    m_mainWindow = new QwcConnectionMainWindow();
    m_mainWindow->installEventFilter(this);
    m_mainWindow->setSocket(m_socket);
    connect(m_mainWindow, SIGNAL(actionTriggered(QwcConnectionMainWindow::TriggeredAction)),
            this, SLOT(handleMainWindowAction(QwcConnectionMainWindow::TriggeredAction)));
    connect(m_mainWindow, SIGNAL(destroyed(QObject*)),
            this, SLOT(connectionWindowDestroyed(QObject*)) );

    // Use a stacked widget for the GUI switching
    connectionStackedWidget = new QStackedWidget(m_mainWindow);
    m_mainWindow->setCentralWidget(connectionStackedWidget);

#ifdef Q_WS_X11
    connectionStackedWidget->setContentsMargins(6, 6, 6, 6);
#endif

    // Create the login dialog
    m_connectWidget = new QwcConnectWidget(connectionStackedWidget);
    m_connectWidget->setSocket(m_socket);
    connectionStackedWidget->addWidget(m_connectWidget);
    connectionStackedWidget->setCurrentIndex(0);


    // Create the tab bar for the normal program use
    connectionTabWidget = new QTabWidget(connectionStackedWidget);
    connectionTabWidget->setMovable(true);
#ifdef Q_WS_MAC
    connectionTabWidget->setDocumentMode(true);
#endif
    connectionTabWidget->setTabsClosable(true);
    connect(connectionTabWidget, SIGNAL(tabCloseRequested(int)),
            this, SLOT(onTabBarCloseRequested(int)));
    connect(connectionTabWidget, SIGNAL(currentChanged(int)),
            this, SLOT(onTabBarCurrentChanged(int)) );
    connectionStackedWidget->addWidget(connectionTabWidget);

    // Create the initial tab for the main chat
    m_publicChat = new QwcChatWidget(m_mainWindow);
    m_publicChat->setAttribute(Qt::WA_DeleteOnClose, false);
    connect(m_publicChat, SIGNAL(requestedNewPrivateMessage(QwcUserInfo,QString)),
            this, SLOT(showMessagerForUser(QwcUserInfo,QString)));
    /*: Text of the main connection tab in the connection window. */
    connectionTabWidget->addTab(m_publicChat, tr("Chat"));
    m_publicChat->setSocket(m_socket);

    // Messenger
    m_privateMessagerWidget = new QwcPrivateMessager();
    m_privateMessagerWidget->setParent(m_mainWindow, Qt::Window);
    m_privateMessagerWidget->setSocket(m_socket);

    m_mainWindow->show();
}




/*! The main window was closed. We should clean up here and say good bye.
*/
void QwcSession::connectionWindowDestroyed(QObject *)
{ this->deleteLater(); }

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
        errorText = tr("File or Directory not found. The last command could not be completed because the file or directory could not be found."); break;
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
        connectionStackedWidget->setCurrentIndex(0);
        m_connectWidget->resetForm();
        m_connectWidget->startReconnecting();
        triggerEvent("ServerDisconnected",QStringList());
    }
}


void QwcSession::handleSocketUserlistComplete(int chatId)
{
    if (chatId != Qwc::PUBLIC_CHAT) { return; }
    connectionStackedWidget->setCurrentIndex(1);
    triggerEvent("ServerConnected", QStringList());
}


/*! Handle a new private message from the server.
*/
void QwcSession::handlePrivateMessage(QwcUserInfo sender, QString text)
{
//    // Update the tab icon to indicate activity
//    if (connectionTabWidget->indexOf(m_privateMessagerWidget) > -1) {
//        if (connectionTabWidget->currentWidget() == m_privateMessagerWidget) { return; }
//        connectionTabWidget->setTabIcon(connectionTabWidget->indexOf(m_privateMessagerWidget),
//                                        QIcon(":/icons/tab-content.png"));
//    }
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
    QWidget *targetWidget = connectionTabWidget->widget(index);
    if (!targetWidget) { return; }

    // We check if the chat tab is the current one so that not to close it
    QwcChatWidget *chatWidget = qobject_cast<QwcChatWidget*>(targetWidget);
    if (chatWidget && chatWidget->chatId() != Qwc::PUBLIC_CHAT) {
        chatWidget->close();
    }

    connectionTabWidget->removeTab(index);
}






/*! Handle a 'action' from the main window.
*/
void QwcSession::handleMainWindowAction(QwcConnectionMainWindow::TriggeredAction action)
{
    if (action == QwcConnectionMainWindow::TriggeredActionAccounts) {
        if(!m_accountsWidget) {
            m_accountsWidget = new QwcAccountsWidget();
            m_accountsWidget->setParent(m_mainWindow, Qt::Window);
            m_accountsWidget->setSocket(m_socket);
        }

        // Display the widget if it is not in the tab widget
        if (connectionTabWidget->indexOf(m_accountsWidget) == -1) {
            connectionTabWidget->addTab(m_accountsWidget, tr("Accounts"));
        }
        connectionTabWidget->setCurrentWidget(m_accountsWidget);


    } else if (action == QwcConnectionMainWindow::TriggeredActionDisconnect) {
        m_connectWidget->resetForm();
        m_socket->disconnectFromServer();
        connectionStackedWidget->setCurrentIndex(0); // go to connect dialog


    } else if (action == QwcConnectionMainWindow::TriggeredActionMessages) {
        showMessagerForUser(QwcUserInfo(), QString());

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

    } else if (action == QwcConnectionMainWindow::TriggeredActionFiles) {
        if (!m_fileBrowserWidget) {
            m_fileBrowserWidget = new QwcFileBrowserWidget();
            m_fileBrowserWidget->setParent(m_mainWindow, Qt::Window);
            m_fileBrowserWidget->setSocket(m_socket);
        }

        int index = connectionTabWidget->indexOf(m_fileBrowserWidget);
        if (index == -1) {
            index = connectionTabWidget->addTab(m_fileBrowserWidget, tr("Files"));
        }
        connectionTabWidget->setCurrentIndex(index);

    }


}


/*! Handle file information (STAT) returned from the server.
    Normally this is passed to a file browser, which then displays the information about a file.
*/
void QwcSession::handleFileInformation(QwcFileInfo file)
{
    if (!m_fileBrowserWidget) { return; }
    m_fileBrowserWidget->setFileInformation(file);
}



/*! Handle a chat message and change the icon of the chat tabs.
*/
void QwcSession::handleSocketChatMessage(int chatId, int userId, const QString &text, bool isEmote)
{
    const QwcUserInfo &sender = m_socket->users()[userId]; //socket->getUserByID(theUserID); // Find the user

    // Trigger the event
    QStringList eventParams;
    eventParams << sender.userNickname;
    if (isEmote) {
        eventParams << QString("*** %1 %2").arg(sender.userNickname).arg(text);
    } else {
        eventParams << text;
    }
    triggerEvent("ChatReceived", eventParams);

    if (m_chatWidgets.contains(chatId)) {
        // Change the icon of the chat tab to indicate some activity
        QwcChatWidget *targetChat = m_chatWidgets[chatId];
        if (!targetChat) { return; }
        // Find the index on the tab panel
        int tabIndex = connectionTabWidget->indexOf(targetChat);
        if (tabIndex != -1 && connectionTabWidget->currentIndex() != tabIndex) {
            connectionTabWidget->setTabIcon(tabIndex, QIcon(":/icons/tab-content.png"));
        }
    }
}


/*! Handle information about a user (INFO).
*/
void QwcSession::handleUserInformation(QwcUserInfo user)
{
    QString widgetName = QString("UserInfo_%1").arg(user.pUserID);
    QwcUserInfoWidget *infoWidget = m_mainWindow->findChild<QwcUserInfoWidget*>(widgetName);

    if (!infoWidget) {
        infoWidget = new QwcUserInfoWidget(m_mainWindow);
        infoWidget->setUser(user);
        infoWidget->setObjectName(widgetName);
    }
    if (!infoWidget) { return; }

    if (connectionTabWidget->indexOf(infoWidget) == -1) {
        int index = connectionTabWidget->addTab(infoWidget, user.userNickname);
        connectionTabWidget->setCurrentIndex(index);
    } else {
        connectionTabWidget->setCurrentWidget(infoWidget);
    }
}


// Received an invitation to a private chat.
void QwcSession::handleSocketChatInvitation(int chatId, QwcUserInfo inviter)
{
    QMessageBox messageBox(m_mainWindow);
    messageBox.setWindowTitle(tr("Private Chat Invitation"));
    messageBox.setText(tr("%1 has invited you to a private chat.\n"
                          "Join to open a separate private chat with %1.")
                       .arg(inviter.userNickname) );
    messageBox.setIconPixmap(QPixmap(":/icons/btn_chat.png"));
    QAbstractButton *rejectButton = messageBox.addButton(tr("Reject"), QMessageBox::RejectRole);
    QAbstractButton *joinButton = messageBox.addButton(tr("Join"), QMessageBox::AcceptRole);

    messageBox.exec();
    if (messageBox.clickedButton() == joinButton) {
        // Create a new chat
        createChatWidget(chatId);
        m_socket->joinChat(chatId);
    } else if( messageBox.clickedButton() == rejectButton ) {
        // Reject the chat.
        m_socket->rejectChat(chatId);
    }
}


/*! Create a new chat widget and add it to the tab panel.
*/
void QwcSession::createChatWidget(int chatId)
{
    QwcChatWidget *chat = new QwcChatWidget();
    chat->setParent(m_mainWindow, Qt::Window);
    connect(chat, SIGNAL(requestedNewPrivateMessage(QwcUserInfo)),
            this, SLOT(showMessagerForUser(QwcUserInfo)));
    chat->setSocket(m_socket);
    chat->setChatId(chatId);
    m_chatWidgets[chatId] = chat;

    int index = connectionTabWidget->addTab(chat, tr("Private Chat"));
    connectionTabWidget->setCurrentIndex(index);
    connectionTabWidget->setTabIcon(index, QIcon(":/icons/tab-idle.png"));
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
        m_publicChat->writeEventToChat(eventMessage);
    }

    // Show a message in the system tray
    if(conf.value(QString("events/%1/traymsg").arg(event)).toBool()) {
        QwcSingleton *tmpS = &WSINGLETON::Instance();
        tmpS->pTrayIcon->showMessage( m_socket->serverInformation().name, eventMessage );
    }

    // Play sound
    if(conf.contains(QString("events/%1/sound").arg(event)) ) {
        QSound::play(conf.value(QString("events/%1/sound").arg(event)).toString());
    }

    // Execute a system command
    if(conf.value(QString("events/%1/syscmd_enabled").arg(event), false).toBool()) {
        QString command = conf.value(QString("events/%1/syscmd").arg(event)).toString();
        // Protect the noobs.
        if (command.contains("rm", Qt::CaseSensitive)) { return; }
        if (command.isEmpty()) { return; }
        QProcess process;
        process.start(command);
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
    if (theOld.userNickname != theNew.userNickname) {
        triggerEvent("UserChangedNick", QStringList() << theOld.userNickname << theNew.userNickname);
    }

    if (theOld.userStatus != theNew.userStatus) {
        triggerEvent("UserChangedStatus", QStringList() << theNew.userNickname << theNew.userStatus);
    }
}


void QwcSession::newsPosted(QString nickname, QDateTime time, QString post)
{
    Q_UNUSED(time);
    triggerEvent("NewsPosted", QStringList() << nickname << post);
}


void QwcSession::handleBroadcastMessage(QwcUserInfo theUser, QString theMessage)
{
    Q_ASSERT(m_publicChat);
    triggerEvent("BroadcastMessageReceived", QStringList() << theUser.userNickname << theMessage);
}


/*! Reload the preferences and check what needs to be done to synchronize the new settings with the
    rest of the application.
*/
void QwcSession::reloadPreferences()
{
    QSettings settings;
    if (m_socket->sessionUser().userNickname != settings.value("general/nickname", tr("Unnamed")).toString()) {
        m_socket->setNickname(settings.value("general/nickname").toString());
    }
    if (m_socket->sessionUser().userStatus != settings.value("general/status", tr("Qwired Newbie")).toString()) {
        m_socket->setUserStatus(settings.value("general/status").toString());
    }
    QPixmap newIcon = settings.value("general/icon", QPixmap(":/icons/qwired_logo_32.png")).value<QPixmap>();
    m_socket->setUserIcon(newIcon.toImage());
}


/*! Show the private messenger and select the provided user, so that a message can be sent to the
    target user. This is usually connected to the double-click events of user lists.
*/
void QwcSession::showMessagerForUser(const QwcUserInfo &targetUser, const QString &message)
{
    if (!m_privateMessagerWidget) { return; }
    if (connectionTabWidget->indexOf(m_privateMessagerWidget) == -1) {
        connectionTabWidget->setCurrentIndex(
                connectionTabWidget->addTab(m_privateMessagerWidget, QIcon(":/icons/tab-idle.png"),
                                            tr("Messages")));
    } else {
        connectionTabWidget->setCurrentWidget(m_privateMessagerWidget);
    }
    m_privateMessagerWidget->handleNewMessage(targetUser, message);
}
