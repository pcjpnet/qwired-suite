#include "QwcSocket.h"
#include "QwcGlobals.h"

#include <QProcess>
#include <QBuffer>
#include <QDirIterator>


/*! \class QwcSocket
    \author Bastian Bense <bastibense@gmail.com>
    \date 2009-03-06
    \brief This class implements the basic protocol and state handling for the client.
*/

QwcSocket::QwcSocket(QObject *parent) : QwSocket(parent)
{
    connect(this, SIGNAL(messageReceived(QwMessage)),
            this, SLOT(handleMessageReceived(QwMessage)));

    // Initialize the SSL socket
    QSslSocket *newSocket = new QSslSocket(this);
    newSocket->setProtocol(QSsl::TlsV1);
    newSocket->setPeerVerifyMode(QSslSocket::QueryPeer);
    setSslSocket(newSocket);

    connect(newSocket, SIGNAL(encrypted()),
            this, SLOT(handleSocketConnected()));
    connect(this, SIGNAL(connectionLost()),
            this, SLOT(handleSocketConnectionLost()));

    // Set some basic information
    pClientName = "Qwired SVN";
    pClientVersion = QWIRED_VERSION;
    pIzCaturday = false;

    // Initialize the socket
//    pSocket = new QSslSocket(this);

    pIndexingFiles = false;
    pInvitedUserID = 0;




//    connect( pSocket, SIGNAL(readyRead()), this, SLOT(on_socket_readyRead()) );
//    connect( pSocket, SIGNAL(sslErrors(QList<QSslError>)), this, SLOT(on_socket_sslErrors(QList<QSslError>)));
//    connect( pSocket, SIGNAL(error(QAbstractSocket::SocketError)),
//             this, SLOT(on_socket_error(QAbstractSocket::SocketError)) );

}


void QwcSocket::handleMessageReceived(const QwMessage &message)
{
    qDebug() << this << "Received message:" << message.commandName;
    int commandId = message.commandName.toInt();
    if (commandId == 200) {           handleMessage200(message); // Login Successful
    } else if (commandId == 201) {    handleMessage201(message); // Server Information
    } else if (commandId == 203) {    handleMessage203(message); // Server Banner
    } else if (commandId == 300) {    handleMessage300(message); // Chat
    } else if (commandId == 301) {    handleMessage301(message); // Action Chat
    } else if (commandId == 302) {    handleMessage302(message); // Client Join
    } else if (commandId == 303) {    handleMessage303(message); // Client Leave
    } else if (commandId == 304) {    handleMessage304(message); // Status Change
    } else if (commandId == 306) {    handleMessage306(message); // Client Kicked
    } else if (commandId == 307) {    handleMessage307(message); // Client Banned
    } else if (commandId == 310) {    handleMessage310(message); // User List
    } else if (commandId == 311) {    handleMessage311(message); // User List Done
    } else if (commandId == 602) {    handleMessage602(message); // 602 Privileges Specification

    }
}


/*! 200 Server Information
    Received right after login and HELLO request.
*/
void QwcSocket::handleMessage200(const QwMessage &message)
{
    // Server Information
    serverInfo.serverVersion = message.getStringArgument(0);
    serverInfo.protocolVersion = message.getStringArgument(1);
    serverInfo.name = message.getStringArgument(2);
    serverInfo.description = message.getStringArgument(3);
    serverInfo.startTime = QDateTime::fromString(message.getStringArgument(4), Qt::ISODate);

    if (serverInfo.protocolVersion == "1.1") {
        serverInfo.filesCount = message.getStringArgument(5).toInt();
        serverInfo.filesSize = message.getStringArgument(6).toLongLong();
    }

    // Send login sequence
    sendMessageINFO();
    setUserNick(sessionUser.userNickname);
    setUserIcon(sessionUser.userImage);
    setUserStatus(sessionUser.userStatus);
    sendMessage(QwMessage("USER").appendArg(sessionUser.name));
    sendMessage(QwMessage("PASS").appendArg(sessionUser.cryptedPassword()));
    emit onServerInformation();
}


/*! 201 Login Succeeded
    Received right after login succeeded and the session is active.
*/
void QwcSocket::handleMessage201(const QwMessage &message)
{
    sessionUser.pUserID = message.getStringArgument(0).toInt();
    sendMessage(QwMessage("WHO").appendArg("1"));
    sendMessage(QwMessage("BANNER"));
    sendMessage(QwMessage("PRIVILEGES"));
    emit onServerLoginSuccessful();
}


/*! 203 Server Banner
    Received in response to BANNER.
*/
void QwcSocket::handleMessage203(const QwMessage &message)
{
    pServerBanner.loadFromData(QByteArray::fromBase64(message.getStringArgument(0).toAscii()));
    emit onServerBanner(pServerBanner);
}


/*! 300 Chat
    A chat message has been posted in a room.
*/
void QwcSocket::handleMessage300(const QwMessage &message)
{
    emit receivedChatMessage(message.getStringArgument(0).toInt(),
                             message.getStringArgument(1).toInt(),
                             message.getStringArgument(2),
                             false);
}


/*! 301 Action Chat
    A "action" chat message has been posted in a room.
*/
void QwcSocket::handleMessage301(const QwMessage &message)
{
    emit receivedChatMessage(message.getStringArgument(0).toInt(),
                             message.getStringArgument(1).toInt(),
                             message.getStringArgument(2),
                             true);
}


/*! 302 Client Join
    A client has joined a room.
*/
void QwcSocket::handleMessage302(const QwMessage &message)
{
    int roomId = message.getStringArgument(0).toInt();
    QwcUserInfo newUser = QwcUserInfo::fromMessage310(message);
    pUsers[roomId].append(newUser);
    emit userJoinedRoom(roomId, newUser);
}


/*! 303 Client Leave
    A client has left a room.
*/
void QwcSocket::handleMessage303(const QwMessage &message)
{
    // The user left a chat or the server.
    int roomId = message.getStringArgument(0).toInt();
    int userId = message.getStringArgument(1).toInt();

    QMutableHashIterator<int,QList<QwcUserInfo> > i(pUsers);
    while (i.hasNext()) {
        i.next();

        // If tmpChatID==1, the user has disconnected completely. Remove him/her form all chats, respectively.
        if (roomId == 1 || (roomId == i.key()) ) {
            QList<QwcUserInfo> tmpList = i.value();
            QMutableListIterator<QwcUserInfo> j(tmpList);
            while(j.hasNext()) {
                QwcUserInfo tmpUsr = j.next();
                if(tmpUsr.pUserID == userId) {
                    j.remove();
                    emit userLeftRoom(roomId, tmpUsr);
                }
            }
            i.setValue(tmpList);
        }
    }
}


/*! 304 Status Change
    A client has changed nickame or other parameters.
*/
void QwcSocket::handleMessage304(const QwMessage &message)
{
    int userId = message.getStringArgument(0).toInt();

    // We have to run through all chats and update the user items accordingly.
    QMutableHashIterator<int,QList<QwcUserInfo> > i(pUsers);
    while(i.hasNext()) {
        i.next();
        QList<QwcUserInfo> tmpList = i.value();
        QMutableListIterator<QwcUserInfo> j(tmpList);
        while(j.hasNext()) {
            QwcUserInfo itemUser = j.next();
            if (itemUser.pUserID == userId) {
                QwcUserInfo newUser = itemUser;
                newUser.pIdle = message.getStringArgument(1).toInt();
                newUser.pAdmin = message.getStringArgument(2).toInt();
                newUser.pIcon = message.getStringArgument(3).toInt();
                newUser.userNickname = message.getStringArgument(4);
                newUser.userStatus = message.getStringArgument(4);
                j.setValue(newUser);

                // Fire a signal, only for the main list!
                if(i.key() == 1) {
                    emit userChanged(itemUser, newUser);
                }

            }
        }
        i.setValue(tmpList);
    }
}


/*! 305 Private Message
    Another client has sent a private message.
*/
void QwcSocket::handleMessage305(const QwMessage &message)
{
    emit onPrivateMessage(getUserByID(message.getStringArgument(0).toInt()),
                          message.getStringArgument(1) );
}


/*! 306 Client Kicked
    A client was kicked by an administrator.
*/
void QwcSocket::handleMessage306(const QwMessage &message)
{
    QwcUserInfo victim = getUserByID(message.getStringArgument(0).toInt());
    QwcUserInfo killer = getUserByID(message.getStringArgument(1).toInt());
    QString reason = message.getStringArgument(2);

    QMutableHashIterator<int,QList<QwcUserInfo> > i(pUsers);
    while(i.hasNext()) {
        i.next();
        QList<QwcUserInfo> tmpList = i.value();
        QMutableListIterator<QwcUserInfo> j(tmpList);
        while(j.hasNext()) {
            QwcUserInfo itemUser = j.next();
            if (itemUser.pUserID == victim.pUserID) {
                j.remove();
                emit userLeftRoom(i.key(), itemUser);
            }
        }
        i.setValue(tmpList);
    }
    emit userKicked(victim, killer, reason);
}


/*! 307 Client Banned
    A client was banned by an administrator.
*/
void QwcSocket::handleMessage307(const QwMessage &message)
{
    QwcUserInfo victim = getUserByID(message.getStringArgument(0).toInt());
    QwcUserInfo killer = getUserByID(message.getStringArgument(1).toInt());
    QString reason = message.getStringArgument(2);

    QMutableHashIterator<int,QList<QwcUserInfo> > i(pUsers);
    while(i.hasNext()) {
        i.next();
        QList<QwcUserInfo> tmpList = i.value();
        QMutableListIterator<QwcUserInfo> j(tmpList);
        while(j.hasNext()) {
            QwcUserInfo itemUser = j.next();
            if (itemUser.pUserID == victim.pUserID) {
                j.remove();
                emit userLeftRoom(i.key(), itemUser);
            }
        }
        i.setValue(tmpList);
    }
    emit userBanned(victim, killer, reason);
}


/*! 310 User List
    One item of the user list of a specific room. In response to WHO.
*/
void QwcSocket::handleMessage310(const QwMessage &message)
{
    int tmpChannel = message.getStringArgument(0).toInt();
    pUsers[tmpChannel].append(QwcUserInfo::fromMessage310(message));
}


/*! 311 User List Done
    Received after a list of 310 messages and indicates the end of the list.
*/
void QwcSocket::handleMessage311(const QwMessage &message)
{
    int tmpChannel = message.getStringArgument(0).toInt();
    emit receivedUserlist(tmpChannel);
}


/*! 602 Privileges Specification
    Received from the server or after PRIVILEGES command.
*/
void QwcSocket::handleMessage602(const QwMessage &message)
{
    sessionUser.setPrivilegesFromMessage602(message);
    emit receivedUserPrivileges(sessionUser);
}

/// Disconnect from the server and clean up
void QwcSocket::disconnectFromServer() {
    pSocket->disconnectFromHost();
    pAdminGroups.clear();
    pAdminUsers.clear();
    pInvitedUserID = 0;
    pBuffer.clear();
    pUsers.clear();

    QMutableListIterator<QPointer<QwcFiletransferSocket> > i(pTransferSockets);
    while(i.hasNext()) {
        QPointer<QwcFiletransferSocket> p = i.next();
        p->deleteLater();
        i.remove();
    }

}

// Called by on_socket_readyRead() after splitting a message off the internal
// buffer. We have to extract the command ID here and decide what to do with
// the message.
void QwcSocket::do_handle_wiredmessage(QByteArray theData) {
    int tmpPos = theData.indexOf(" ");
    int tmpCmdID = 0;
    if( tmpPos>-1 ) tmpCmdID = theData.left(tmpPos).toInt();
    else tmpCmdID = theData.toInt();

    QList<QByteArray> tmpParams = GetFieldArray(theData);
    // 	qDebug() << "Got command" << tmpCmdID;

    // If in tracker mode, we should block commands that are not included in the tracker protocol.
    if(pSocketType==Wired::TrackerSocket && !(tmpCmdID==200 || tmpCmdID==500 || tmpCmdID==501
                                              || tmpCmdID==502 || tmpCmdID==503 || tmpCmdID==511 || tmpCmdID==516
                                              || tmpCmdID==530 || tmpCmdID==700 || tmpCmdID==710 || tmpCmdID==711
                                              || tmpCmdID==720 || tmpCmdID==721) )
        return;



    switch(tmpCmdID) {

                case 308: on_server_userinfo(tmpParams); break;
                case 309: on_server_broadcast(tmpParams); break;

                case 320: // News Post
                    emit onServerNews(QString::fromUtf8(tmpParams.value(0)),
                                      QString::fromAscii(tmpParams.value(1)),
                                      QString::fromUtf8(tmpParams.value(2)) );
                    break;
                case 321: emit onServerNewsDone(); break;

                case 322: // News Posted
                    emit onServerNewsPosted(QString::fromUtf8(tmpParams.value(0)),
                                            QString::fromAscii(tmpParams.value(1)),
                                            QString::fromUtf8(tmpParams.value(2)) );
                    break;

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
                case 410: on_server_filelist_item(tmpParams); break;
                case 411: on_server_filelist_done(tmpParams); break;

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
                case 610: on_server_users_listing(tmpParams); break;
                case 611: on_server_users_done();
                case 620: on_server_groups_listing(tmpParams); break;
                case 621: on_server_groups_done(); break;

                    // Tracker protocol
                    //
                case 720: on_tracker_listing_item(tmpParams); break;
                case 721: on_tracker_listing_done(); break;

                default: qDebug() << "QwcSocket: Unhandled command:"<<tmpCmdID; break;

                }
}


// Find a user by the ID specified in the list of publically connected users.
QwcUserInfo QwcSocket::getUserByID(int theID) {
    QList<QwcUserInfo> tmpList = pUsers[1]; // server user list
    QListIterator<QwcUserInfo> i(tmpList);
    while (i.hasNext()) {
        QwcUserInfo usr = i.next();
        if( usr.pUserID==theID )
            return usr;
    }

    QwcUserInfo wu; // Fallback object
    wu.userNickname = tr("(invalid user)");
    return wu;
}


// Set the username and password for the login sequence.
void QwcSocket::setUserAccount(QString theAcct, QString thePass) {
    sessionUser.name = theAcct;
    sessionUser.pPassword = thePass;
}


// Do I really have to comment this? :D
void QwcSocket::setCaturday(bool b) {
    // everyday iz caturday!  :3
    if(b) {
        pTranzlator.clear();
        setUserIcon( QImage(":/icons/icon_happycat.png") );
        setUserStatus(tr("kittehday nait fevrar"));
        QString tmpNick = sessionUser.userNickname;
        tmpNick = tmpNick.replace("s","z");
        tmpNick = tmpNick.replace("e","3");
        tmpNick = tmpNick.replace("i","ie");
        tmpNick = tmpNick.replace("y","eh");
        setUserNick(tmpNick);
        QFile tmpF(":/tranzlator.txt");
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
void QwcSocket::connectToTracker(QString theHostName, int thePort) {
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
    qDebug() << "QwcSocket: Connecting to tracker at"<<tmpHost<<"port"<<tmpPort;
    pSocket->connectToHostEncrypted(tmpHost, tmpPort);
}


/*! Attempt to establish a connection to a remote server.
*/
void QwcSocket::connectToWiredServer(QString hostName, int port)
{
    QString tmpHost;
    pSocketType = Wired::QwcSocket;
    int tmpPort;
    if( hostName.contains(":") ) { // Has port defined
        tmpHost = hostName.section(":",0,0);
        tmpPort = hostName.section(":",1,1).toInt();
    } else { // No port defined
        tmpPort = port;
        tmpHost = hostName;
    }
    qDebug() << this << "Connecting to wired server at"<<tmpHost<<"port"<<tmpPort;
    socket->connectToHostEncrypted(tmpHost, tmpPort);
}

// Called by the socket once it has received some data.
// We will read the buffer, put the data in our own buffer and then read
// message after message from it before passing it to do_handle_wiredmessage().
void QwcSocket::on_socket_readyRead() {
    pBuffer += pSocket->readAll();
    int tmpPos = pBuffer.indexOf("\x04");
    while( tmpPos != -1 ) {
        QByteArray tmpLine = pBuffer.left(tmpPos);
        pBuffer = pBuffer.mid(tmpPos+1);
        do_handle_wiredmessage(tmpLine);
        tmpPos = pBuffer.indexOf("\x04");
    }
}


/*! The SSL connection was established. Now send the session request HELLO.
*/
void QwcSocket::handleSocketConnected()
{
    sendMessage(QwMessage("HELLO"));
}


/*! The connection to the server was lost.
*/
void QwcSocket::handleSocketConnectionLost()
{
    disconnectFromServer();
    pSocket->disconnectFromHost();
    emit onSocketError();
}


QwcSocket::~QwcSocket()
{
}


/**
 * Get the number of users in the chat specified by theChatID.
 * @param theChatID The ID of the chat.
 * @return The number of users in the given chat. If theChatID is invalid, 0 will be returned.
 */
int QwcSocket::userCountByChat(const int theChatID) {
    if( pUsers.contains(theChatID) ) {
        QList<QwcUserInfo> *tmpList = &pUsers[theChatID];
        //qDebug() << "QwcSocket::userCountByChat channel"<<theChatID<<"="<<tmpList->size();
        return tmpList->size();
    }
    return 0;
}


/**
 * Find a user within a specific chat by id and return it.
 * @param theChatID The ID of the chat.
 * @param theIndex The index of the user within the chat.
 * @return The QwcUserInfo object. If the user is not found, a default object will be returned with user id -1.
 */
const QwcUserInfo QwcSocket::userByIndex(const int theChatID, const int theIndex) {
    if( pUsers.contains(theChatID) ) {
        QList<QwcUserInfo> *tmpList = &pUsers[theChatID];
        return tmpList->value(theIndex);
    }

    QwcUserInfo wu; // Fallback user item
    wu.pUserID = -1;
    wu.userNickname = "(invalid user)";
    return wu;
}


void QwcSocket::on_server_userlist_imagechanged(QList< QByteArray > theParams)
{
    // A user has changed his/her user image.
    int tmpID = theParams.value(0).toInt();

    // We have to run through all chats and update the user items accordingly.
    QMutableHashIterator<int,QList<QwcUserInfo> > i(pUsers);
    while(i.hasNext()) {
        i.next();
        QList<QwcUserInfo> tmpList = i.value();
        QMutableListIterator<QwcUserInfo> j(tmpList);
        while(j.hasNext()) {
            QwcUserInfo tmpUsrOld = j.next();
            if(tmpUsrOld.pUserID==tmpID) {
                QwcUserInfo tmpUsr = tmpUsrOld;
                tmpUsr.setImageFromData(QByteArray::fromBase64(theParams.value(1,"")));
                j.setValue(tmpUsr);

                // Fire a signal, only for the main list!
                if(i.key()==1)
                    emit userChanged(tmpUsrOld, tmpUsr);
            }
        }
        i.setValue(tmpList);
    }
}


// Find a user by the user id and return the index.
int QwcSocket::userIndexByID(const int theID, const int theChat) {
    if( pUsers.contains(theChat) ) {
        QList<QwcUserInfo> tmpList = pUsers[theChat];
        QListIterator<QwcUserInfo> j(tmpList);
        int tmpIdx = 0;
        while(j.hasNext()) {
            QwcUserInfo tmpUsr = j.next();
            if( tmpUsr.pUserID==theID )
                return tmpIdx;
            tmpIdx++;
        }
    }
    return -1;
}


void QwcSocket::sendPrivateMessage(int theUserID, QString theMessage) {
    if(pIzCaturday)
        theMessage = tranzlate(theMessage);

    if(theUserID==0) { // Broadcast Message
        sendWiredCommand(QByteArray("BROADCAST ")+theMessage.toUtf8());
    } else { // Send a private message to a user.
        QByteArray buf("MSG ");
        buf += QByteArray::number(theUserID).append(kFS).append(theMessage.toUtf8());
        sendWiredCommand(buf);
    }
}

void QwcSocket::leaveChat(int theChatID) {
    sendWiredCommand(QByteArray("LEAVE ")+QByteArray::number(theChatID));
    if( pUsers.contains(theChatID) )
        pUsers.remove(theChatID);
}



void QwcSocket::on_server_userinfo(QList<QByteArray> theParams) {
    // Received user info from the server.
    QwcUserInfo usr = QwcUserInfo::fromUserInfo(theParams);
    emit onServerUserInfo(usr);
}

void QwcSocket::createChatWithClient(int theUserID) {
    // Initiate a new private chat with a specific user.
    pInvitedUserID = theUserID;
    sendWiredCommand("PRIVCHAT");
}

void QwcSocket::on_server_new_chat_created(QList<QByteArray> theParams) {
    // Server opened a new private chat for us. We now invite the user we have stored in pInvitedUserID before.
    int tmpChatID = theParams.value(0).toInt();

    //QwcUserInfo tmpUsr = getUserByID();
    //qDebug() << "Adding"<<tmpUsr.pNick<<"to Chat"<<tmpChatID;
    QList<QwcUserInfo> &tmpList = pUsers[tmpChatID];
    tmpList.append(sessionUser);
    emit onServerPrivateChatCreated(tmpChatID);
    inviteClientToChat(tmpChatID, pInvitedUserID);
    pInvitedUserID = 0;
}

// Invite a user to a private chat.
void QwcSocket::inviteClientToChat(int theChatID, int theUserID) {
    QByteArray buf("INVITE ");
    buf += QByteArray::number(theUserID) + char(kFS);
    buf += QByteArray::number(theChatID);
    sendWiredCommand(buf);
}



void QwcSocket::on_server_broadcast(QList< QByteArray > theParams)
{
    // Handle Server Broadcast
    QwcUserInfo usr = getUserByID( theParams.value(0).toInt() );
    QString msg = QString::fromUtf8(theParams.value(1));
    emit onServerBroadcast(usr, msg);
}

// Request to clear the news from the server's database
void QwcSocket::clearNews() { sendWiredCommand("CLEARNEWS"); }

void QwcSocket::getFileList(QString thePath) {
    sendWiredCommand(QByteArray("LIST ")+thePath.toUtf8());
}

/*! Send a new user icon to the remote server and update the local user info.
*/
void QwcSocket::setUserIcon(QImage icon)
{
    // Send a new user icon to the server
    QByteArray tmpCmd;
    if (icon.isNull()) { return; }
    QByteArray imageData;
    QBuffer imageDataBuffer(&imageData);
    imageDataBuffer.open(QIODevice::WriteOnly);
    icon.save(&imageDataBuffer, "PNG");
    sessionUser.userImage = icon;
    sendMessage(QwMessage("ICON").appendArg("0").appendArg(imageData.toBase64()));
}





void QwcSocket::getFolder(const QString &remotePath, const QString &localPath, const bool &queueLocally)
{
    Q_UNUSED(queueLocally);
    // Check for duplicate downloads
    QListIterator<QPointer<QwcFiletransferSocket> > i(pTransferSockets);
    while(i.hasNext()) {
        QwcFiletransferSocket *tmpT = i.next();
        if(!tmpT) continue;
        if(tmpT->pTransfer.pRemotePath==remotePath) return;
    }

    QwcFiletransferSocket *tmpSock = new QwcFiletransferSocket;
    pTransferSockets.append(tmpSock);

    tmpSock->pTransfer.pTransferType = WiredTransfer::TypeFolderDownload;
    tmpSock->pTransfer.pRemotePath = remotePath;
    // 	tmpSock->pTransfer.pLocalPath = localPath;
    tmpSock->pTransfer.pLocalRoot = localPath;

    connect(tmpSock, SIGNAL(fileTransferDone(QwcFiletransferInfo)), this, SIGNAL(fileTransferDone(QwcFiletransferInfo)));
    connect(tmpSock, SIGNAL(fileTransferError(QwcFiletransferInfo)), this, SIGNAL(fileTransferError(QwcFiletransferInfo)));
    connect(tmpSock, SIGNAL(fileTransferStatus(QwcFiletransferInfo)), this, SIGNAL(fileTransferStatus(QwcFiletransferInfo)));
    connect(tmpSock, SIGNAL(fileTransferFileDone(const QwcFiletransferInfo)), this, SLOT(fileTransferFileDone(const QwcFiletransferInfo))); // for folder transfers
    connect(tmpSock, SIGNAL(destroyed()), this, SLOT(cleanTransfers()));

    tmpSock->setServer( pSocket->peerAddress().toString(), pSocket->peerPort() );
    tmpSock->pTransfer.pStatus = WiredTransfer::StatusQueuedLocal;
    qDebug() << "Transfer Phase 1/3: LocalQueued:"<<remotePath<<"to"<<localPath;
    emit fileTransferStarted(tmpSock->pTransfer);
}


void QwcSocket::putFolder(const QString localPath, const QString remotePath, const bool queueLocally)
{
    Q_UNUSED(queueLocally);
    // Check for duplicate downloads
    QListIterator<QPointer<QwcFiletransferSocket> > i(pTransferSockets);
    while(i.hasNext()) {
        QwcFiletransferSocket *tmpT = i.next();
        if(!tmpT) continue;
        if(tmpT->pTransfer.pRemotePath==remotePath) return;
    }

    QwcFiletransferSocket *tmpSock = new QwcFiletransferSocket;
    pTransferSockets.append(tmpSock);

    tmpSock->pTransfer.pTransferType = WiredTransfer::TypeFolderUpload;
    tmpSock->pTransfer.pRemotePath = remotePath;
    tmpSock->pTransfer.pRemoteFolder = remotePath;
    tmpSock->pTransfer.pLocalRoot = localPath;

    QDirIterator dir(localPath, QDirIterator::Subdirectories);
    while(dir.hasNext()) {
        QString path = dir.next();
        QFileInfo fileInfo(path);
        tmpSock->pTransfer.fileListLocal.append(fileInfo.absoluteFilePath());
        tmpSock->pTransfer.pFolderSize += fileInfo.size();
        tmpSock->pTransfer.pFilesCount += 1;
    }

    // 	qDebug() << "Files:" << tmpSock->pTransfer.fileListLocal;

    connect(tmpSock, SIGNAL(fileTransferDone(QwcFiletransferInfo)), this, SIGNAL(fileTransferDone(QwcFiletransferInfo)));
    connect(tmpSock, SIGNAL(fileTransferError(QwcFiletransferInfo)), this, SIGNAL(fileTransferError(QwcFiletransferInfo)));
    connect(tmpSock, SIGNAL(fileTransferStatus(QwcFiletransferInfo)), this, SIGNAL(fileTransferStatus(QwcFiletransferInfo)));
    connect(tmpSock, SIGNAL(fileTransferFileDone(const QwcFiletransferInfo)), this, SLOT(fileTransferFileDone(const QwcFiletransferInfo))); // for folder transfers
    connect(tmpSock, SIGNAL(destroyed()), this, SLOT(cleanTransfers()));

    tmpSock->setServer( pSocket->peerAddress().toString(), pSocket->peerPort() );
    tmpSock->pTransfer.pStatus = WiredTransfer::StatusQueuedLocal;
    qDebug() << "Transfer Phase 1/3: LocalQueued:"<<localPath<<"to"<<remotePath;
    emit fileTransferStarted(tmpSock->pTransfer);
}


void QwcSocket::getFile(const QString thePath, const QString theLocalPath, const bool queueLocally)
{
    Q_UNUSED(queueLocally);
    // Check for duplicate downloads
    QListIterator<QPointer<QwcFiletransferSocket> > i(pTransferSockets);
    while( i.hasNext() ) {
        QwcFiletransferSocket *tmpT = i.next();
        if(tmpT) {
            QwcFiletransferInfo tmpTrans = tmpT->pTransfer;
            if( tmpTrans.pRemotePath==thePath ) {
                qDebug() << "Transfer Phase 1:"<<thePath<<": already transferring, abort!";
                return;
            }
        }
    }

    QwcFiletransferSocket *tmpSock = new QwcFiletransferSocket;
    pTransferSockets.append(tmpSock);

    tmpSock->pTransfer.pTransferType = WiredTransfer::TypeDownload;
    tmpSock->pTransfer.pRemotePath = thePath;
    tmpSock->pTransfer.pLocalPath = theLocalPath;

    qDebug() << "Current creator thread:"<<QThread::currentThread();
    connect(tmpSock, SIGNAL(fileTransferDone(QwcFiletransferInfo)), this, SIGNAL(fileTransferDone(QwcFiletransferInfo)));
    connect(tmpSock, SIGNAL(fileTransferError(QwcFiletransferInfo)), this, SIGNAL(fileTransferError(QwcFiletransferInfo)));
    connect(tmpSock, SIGNAL(fileTransferStatus(QwcFiletransferInfo)), this, SIGNAL(fileTransferStatus(QwcFiletransferInfo)));
    connect(tmpSock, SIGNAL(destroyed()), this, SLOT(cleanTransfers()));

    tmpSock->setServer( pSocket->peerAddress().toString(), pSocket->peerPort() );
    tmpSock->pTransfer.pStatus = WiredTransfer::StatusQueuedLocal;
    qDebug() << "Transfer Phase 1/3: LocalQueued:"<<thePath<<"to"<<theLocalPath;
    emit fileTransferStarted(tmpSock->pTransfer);
}



// File transfer is ready. The client should now connect and send or recieve the
// requested file.
void QwcSocket::on_server_transfer_ready(QList<QByteArray> theParams) {
    QString tmpPath = QString::fromUtf8(theParams.value(0));
    qlonglong tmpOffset = theParams.value(1).toLongLong();
    QString tmpHash = QString::fromUtf8(theParams.value(2));

    qDebug() << this << "Transfer ready:"<<theParams;

    QListIterator<QPointer<QwcFiletransferSocket> > i(pTransferSockets);
    while(i.hasNext()) {
        QwcFiletransferSocket *tmpT = i.next();
        if(!tmpT) continue;
        QwcFiletransferInfo tmpTrans = tmpT->pTransfer;
        qDebug() << "CHECKING FILE"<<tmpTrans.pRemotePath<<"with main status"<<tmpTrans.pStatus<<"file status"<<tmpTrans.pFileStatus;

        if( tmpTrans.pRemotePath==tmpPath && (
                (tmpTrans.pTransferType==WiredTransfer::TypeUpload && tmpTrans.pStatus==WiredTransfer::StatusWaitingForStat)
                || (tmpTrans.pTransferType==WiredTransfer::TypeDownload && tmpTrans.pStatus==WiredTransfer::StatusQueued)
                || (tmpTrans.pTransferType==WiredTransfer::TypeFolderDownload && tmpTrans.pFileStatus==WiredTransfer::StatusQueued)
                || (tmpTrans.pTransferType==WiredTransfer::TypeFolderUpload && tmpTrans.pFileStatus==WiredTransfer::StatusQueued) )) {
            qDebug() << this << "Transfer Phase 2/3: Transfer ready for"<<tmpPath<<"with hash"<<tmpHash<<"offset"<<tmpOffset;
            tmpT->pTransfer.pOffset = tmpOffset;
            tmpT->pTransfer.pHash = tmpHash;
            tmpT->pTransfer.pQueuePosition = 0;
            if(tmpTrans.pTransferType==WiredTransfer::TypeFolderDownload || tmpTrans.pTransferType==WiredTransfer::TypeFolderUpload)
                tmpT->pTransfer.pFileStatus = WiredTransfer::StatusActive;
            else	tmpT->pTransfer.pStatus = WiredTransfer::StatusActive;
            tmpT->start();
            return;
        }

    }

    // Duh! Something went wrong.
    qDebug() << "Transfer Phase 2/3: Transfer ready, but we didn't request it?";
}


// The transfer has been queued on the server. Update the transfers respectively.
void QwcSocket::on_server_transfer_queued(QList< QByteArray > theParams) {
    QString tmpPath = QString::fromUtf8(theParams.value(0));
    int tmpPosition = theParams.value(1).toInt();
    qDebug() << "Transfer Phase 2/3: File"<<tmpPath<<"queued at position"<<tmpPosition<<" . o ( Zz..zzzZ..zzZ )";

    QListIterator<QPointer<QwcFiletransferSocket> > i(pTransferSockets);
    while( i.hasNext() ) {
        QwcFiletransferSocket *tmpT = i.next();
        if(!tmpT) continue;
        QwcFiletransferInfo &tmpTrans = tmpT->pTransfer;
        if( tmpTrans.pRemotePath==tmpPath && tmpTrans.pStatus==WiredTransfer::StatusQueued ) {
            tmpT->pTransfer.pQueuePosition = tmpPosition;
            emit fileTransferStatus(tmpTrans);
            return;
        }
    }


}

void QwcSocket::on_server_search_listing(QList<QByteArray> theParams) {
    QwcFileInfo tmpFile;
    tmpFile.path = QString::fromUtf8(theParams.value(0));
    tmpFile.type = (WiredTransfer::FileType)theParams.value(1).toInt();
    tmpFile.size = theParams.value(2).toLongLong();
    tmpFile.created = QDateTime::fromString( QString::fromUtf8(theParams.value(3)), Qt::ISODate );
    tmpFile.modified = QDateTime::fromString( QString::fromUtf8(theParams.value(4)), Qt::ISODate );
    if(pSearchResults.count()<1024)
        pSearchResults.append(tmpFile);
}

void QwcSocket::on_server_search_done(QList<QByteArray> ) {
    emit fileSearchDone(pSearchResults);
    pSearchResults.clear();
}


void QwcSocket::on_server_groups_listing(QList< QByteArray > theParams) {
    pAdminGroups.append( QString::fromUtf8( theParams.value(0) ) );
}

void QwcSocket::on_server_groups_done() {
    emit groupsListingDone(pAdminGroups);
    pAdminGroups.clear();
}

void QwcSocket::on_server_users_listing(QList< QByteArray > theParams) {
    pAdminUsers.append( QString::fromUtf8(theParams.value(0)) );
}

void QwcSocket::on_server_users_done() {
    emit usersListingDone(pAdminUsers);
    pAdminUsers.clear();
}

void QwcSocket::on_server_user_spec(QList< QByteArray > theParams) {
    QwcUserInfo tmpAcct;
    tmpAcct.userType = Qws::UserTypeAccount;
    tmpAcct.name = QString::fromUtf8( theParams.value(0) );
    tmpAcct.pPassword = QString::fromUtf8( theParams.value(1) );
    tmpAcct.pGroupName =  QString::fromUtf8( theParams.value(2) );
    theParams.removeFirst();
    theParams.removeFirst();
    theParams.removeFirst();
    tmpAcct.setFromPrivileges(theParams);
    emit userSpecReceived(tmpAcct);
}

void QwcSocket::on_server_group_spec(QList< QByteArray > theParams) {
    QwcUserInfo tmpAcct;
    tmpAcct.name = QString::fromUtf8( theParams.value(0) );
    tmpAcct.userType = Qws::UserTypeGroup;
    theParams.removeFirst();
    tmpAcct.setFromPrivileges(theParams);
    emit groupSpecReceived(tmpAcct);
}


// Tracker handling code
//

void QwcSocket::on_tracker_listing_item(QList< QByteArray > theParams) {
    QwcTrackerServerInfo ts;
    ts.loadFromTrackerResponse(theParams);
    pTrackerServers.append(ts);
}

void QwcSocket::on_tracker_listing_done() {
    emit trackerServersReceived(pTrackerServers);
    pTrackerServers.clear();
    this->disconnectFromServer();
}

void QwcSocket::on_server_file_info(QList<QByteArray> theParams) {
    // Received file information. This can be the response to a file in the
    // transfer queue or a user request. Let's see...

    QString tmpPath = QString::fromUtf8(theParams.value(0));
    qlonglong tmpSize = theParams.value(2).toLongLong();
    QDateTime tmpCreated = QDateTime::fromString( theParams.value(3), Qt::ISODate );
    QDateTime tmpModified = QDateTime::fromString( theParams.value(4), Qt::ISODate );
    QString tmpChecksum = QString::fromUtf8(theParams.value(5));
    QString tmpComment = QString::fromUtf8(theParams.value(6));

    // Check if we are waiting for a stat
    QListIterator<QPointer<QwcFiletransferSocket> > i(pTransferSockets);
    int tmpIdx=0;
    while( i.hasNext() ) {
        QwcFiletransferSocket *tmpT = i.next();
        if(!tmpT) continue;


        if( (tmpT->pTransfer.pTransferType==WiredTransfer::TypeDownload
             && tmpT->pTransfer.pStatus==WiredTransfer::StatusWaitingForStat
             && tmpT->pTransfer.pRemotePath==tmpPath)
            || (tmpT->pTransfer.pTransferType==WiredTransfer::TypeFolderDownload
                && tmpT->pTransfer.pFileStatus==WiredTransfer::StatusWaitingForStat
                && tmpT->pTransfer.pRemotePath==tmpPath) ) {

            qDebug() << "Transfer Phase 1/3:"<<tmpPath<<": received stat, requesting transfer:"<<tmpPath<<tmpChecksum;

            // Check if we need to resume
            tmpT->pTransfer.pOffset = 0;
            tmpT->pTransfer.pDoneSize = 0;

            QString tmpFilePath = tmpT->pTransfer.pLocalPath+".WiredTransfer";
            QFile tmpFile(tmpFilePath);
            if(tmpFile.exists()) {
                if(tmpFile.open(QIODevice::ReadOnly)) {
                    QByteArray tmpDat = tmpFile.read(1024*1024);
                    QString tmpCS = QCryptographicHash::hash(tmpDat, QCryptographicHash::Sha1).toHex();
                    qDebug() << "QwcSocket: Download file exists, checksum:"<<tmpCS<<"; server:"<<tmpChecksum;
                    if(tmpChecksum==tmpCS && tmpFile.size()<tmpSize) {
                        qDebug() << "QwcSocket: Checksums are identical. Offset is"<<tmpFile.size();
                        tmpT->pTransfer.pOffset = tmpFile.size();
                        tmpT->pTransfer.pDoneSize = tmpFile.size();
                    } else {
                        qDebug() << "QwcSocket: Checksums are NOT identical, deleting file.";
                        tmpFile.close();
                        tmpFile.remove();
                    }
                }
            }

            // Set some more info
            tmpT->pTransfer.pChecksum = tmpChecksum;
            tmpT->pTransfer.pTotalSize = tmpSize;

            if(tmpT->pTransfer.pTransferType==WiredTransfer::TypeFolderDownload)
                tmpT->pTransfer.pFileStatus = WiredTransfer::StatusQueued;
            else	tmpT->pTransfer.pStatus = WiredTransfer::StatusQueued;

            // Request a file download from the server
            QByteArray buf("GET ");
            buf += tmpPath.toUtf8() + char(kFS);
            buf += QByteArray::number(tmpT->pTransfer.pOffset);
            sendWiredCommand(buf);
            qDebug() << "QwcSocket: GET'ing the file with offset"<<tmpT->pTransfer.pOffset<<"main status"<<tmpT->pTransfer.pStatus<<"file status"<<tmpT->pTransfer.pFileStatus;
            return;
        }
    }
    tmpIdx++;


    // Not in the transfer queue, pass the event to the user code.
    QwcFileInfo tmpFile;
    tmpFile.setFromStat(theParams);
    emit onServerFileInfo(tmpFile);

}

// Request uploading of a file to the server
void QwcSocket::putFile(const QString theLocalPath, const QString theRemotePath, const bool queueLocally) {
    QFile tmpFile(theLocalPath);
    QString remotePath = theRemotePath;
    if(remotePath.left(2)=="//") remotePath.remove(0,1);
    if(tmpFile.exists() && !remotePath.isEmpty()) {

        // Check for duplicate uploads
        QListIterator<QPointer<QwcFiletransferSocket> > i(pTransferSockets);
        while( i.hasNext() ) {
            QwcFiletransferSocket *tmpT = i.next();
            if(!tmpT) continue;
            if(tmpT->pTransfer.pRemotePath==remotePath) return;
        }

        QwcFiletransferSocket *tmpSock = new QwcFiletransferSocket;
        pTransferSockets.append(tmpSock);
        connect(tmpSock, SIGNAL(fileTransferDone(QwcFiletransferInfo)), this, SIGNAL(fileTransferDone(QwcFiletransferInfo)));
        connect(tmpSock, SIGNAL(fileTransferError(QwcFiletransferInfo)), this, SIGNAL(fileTransferError(QwcFiletransferInfo)));
        connect(tmpSock, SIGNAL(fileTransferStatus(QwcFiletransferInfo)), this, SIGNAL(fileTransferStatus(QwcFiletransferInfo)));
        connect(tmpSock, SIGNAL(fileTransferStarted(QwcFiletransferInfo)), this, SIGNAL(fileTransferStarted(QwcFiletransferInfo)));
        connect(tmpSock, SIGNAL(fileTransferSocketError(QAbstractSocket::SocketError)), this, SIGNAL(fileTransferSocketError(QAbstractSocket::SocketError)));
        connect(tmpSock, SIGNAL(destroyed()), this, SLOT(cleanTransfers()));
        tmpSock->setServer( pSocket->peerAddress().toString(), pSocket->peerPort() );
        tmpSock->pTransfer.pTransferType = WiredTransfer::TypeUpload;
        tmpSock->pTransfer.pRemotePath = remotePath;
        tmpSock->pTransfer.pLocalPath = theLocalPath;
        tmpSock->pTransfer.pTotalSize = tmpFile.size();
        tmpSock->pTransfer.calcLocalChecksum();

        // Request upload slot
        if(queueLocally) {
            tmpSock->pTransfer.pStatus = WiredTransfer::StatusQueuedLocal;
            qDebug() << this << "Queued transfer locally:"<<tmpSock->pTransfer.pRemotePath;
        }
        emit fileTransferStarted(tmpSock->pTransfer);
    }
}



// ///
// /// Request Commands ///
// ///


/*! Set the current user nickname and send it to the server.
*/
void QwcSocket::setUserNick(QString theNick)
{
    sessionUser.userNickname = theNick;
    sendMessage(QwMessage("NICK").appendArg(theNick));
}


/*! Set the current user status and send it to the server.
*/
void QwcSocket::setUserStatus(QString theStatus) {
    sessionUser.userStatus = theStatus;
    sendMessage(QwMessage("STATUS").appendArg(sessionUser.userStatus));
}

// Reject/Decline a private chat.
void QwcSocket::rejectChat(int theChatID) {
    sendWiredCommand(QByteArray("DECLINE ")+QByteArray::number(theChatID));
}

// Join a private chat.
void QwcSocket::joinChat(int theChatID) {
    sendWiredCommand(QByteArray("JOIN ")+QByteArray::number(theChatID));
    do_request_user_list(theChatID);
}

// Set a chat topic.
void QwcSocket::setChatTopic(int theChatID, QString theTopic) {
    QByteArray buf("TOPIC ");
    buf += QByteArray::number(theChatID) + (char)kFS;
    buf += theTopic.toUtf8();
    sendWiredCommand(buf);
}

// Ban a user from the server.
void QwcSocket::banClient(int theUserID, QString theReason) {
    QByteArray tmpBuf("BAN ");
    tmpBuf += QByteArray::number(theUserID) + (char)kFS;
    tmpBuf += theReason.toUtf8();
    sendWiredCommand(tmpBuf);
}


// Request the list of users for a specific channel.
void QwcSocket::do_request_user_list(int theChannel) {
    sendWiredCommand(QByteArray("WHO ")+QByteArray::number(theChannel));
}

void QwcSocket::getNews() { sendWiredCommand("NEWS"); }


// Post news to the news board
void QwcSocket::postNews(QString thePost) {
    sendWiredCommand(QByteArray("POST ")+thePost.toUtf8());
}

// Request info for a user
void QwcSocket::getClientInfo(int theUserID) {
    sendWiredCommand(QByteArray("INFO "+QByteArray::number(theUserID)));
}


/*! Send a chat message to the chat room with id \a theChatID.
*/
void QwcSocket::sendChatToRoom(int theChatID, QString theText, bool theIsAction)
{
    if(pIzCaturday) {
        theText = tranzlate(theText);
    }
    QwMessage reply(theIsAction ? "ME" : "SAY");
    reply.appendArg(QString::number(theChatID));
    reply.appendArg(theText);
    sendMessage(reply);
}


// Request some file information.
void QwcSocket::statFile(const QString thePath) {
    sendWiredCommand(QByteArray("STAT ")+thePath.toUtf8());
}

// Request to create a folder at the specified path
void QwcSocket::createFolder(const QString thePath) {
    sendWiredCommand(QByteArray("FOLDER ")+thePath.toUtf8());
}

// Kick a user off the server.
void QwcSocket::kickClient(int theUserID, QString theReason) {
    QByteArray tmpBuf("KICK ");
    tmpBuf += QByteArray::number(theUserID) + (char)kFS;
    tmpBuf += theReason.toUtf8();
    sendWiredCommand(tmpBuf);
}

// Request to move a file on the server
void QwcSocket::moveFile(const QString thePath, const QString theDestination) {
    QByteArray tmpBuf("MOVE ");
    tmpBuf += thePath.toUtf8() + (char)kFS;
    tmpBuf += theDestination.toUtf8();
    sendWiredCommand(tmpBuf);
}

// Delete a file or folder from the server.
void QwcSocket::deleteFile(const QString thePath) {
    if(thePath=="/" || thePath.isEmpty()) return;
    sendWiredCommand(QByteArray("DELETE ")+thePath.toUtf8());
}

void QwcSocket::getGroups() { sendWiredCommand("GROUPS"); }
void QwcSocket::getUsers() { sendWiredCommand("USERS"); }

// Create a user account on the server.
void QwcSocket::createUser(QwcUserInfo tmpUser) {
    QByteArray tmpBuf("CREATEUSER ");
    tmpBuf += tmpUser.name.toUtf8() + (char)kFS;
    tmpBuf += tmpUser.pPassword.toUtf8() + (char)kFS;
    tmpBuf += tmpUser.pGroupName.toUtf8() + (char)kFS;
    tmpBuf += tmpUser.privilegesFlags();
    sendWiredCommand(tmpBuf);
}

// Modify a user account on the server.
void QwcSocket::editUser(QwcUserInfo tmpUser) {
    QByteArray tmpBuf("EDITUSER ");
    tmpBuf += tmpUser.name.toUtf8() + (char)kFS;
    tmpBuf += tmpUser.pPassword.toUtf8() + (char)kFS;
    tmpBuf += kFS;	tmpBuf += tmpUser.pGroupName.toUtf8() + (char)kFS;
    tmpBuf += tmpUser.privilegesFlags();
    sendWiredCommand(tmpBuf);
}


/*! Send the CREATEGROUP command to the server. Creates a user group on the server (uses login and
    privileges from \a user object.
*/
void QwcSocket::createGroup(QwcUserInfo user)
{
    QwMessage reply("CREATEGROUP");
    reply.appendArg(user.name);
    reply.appendArg(user.privilegesFlags());
    sendMessage(reply);
}


/*! Send the EDITGROUP command to the server. Modify a group on the server (uses login and
    privileges flags).
*/
void QwcSocket::editGroup(QwcUserInfo user)
{
    QwMessage reply("EDITGROUP");
    reply.appendArg(user.name);
    reply.appendArg(user.privilegesFlags());
    sendMessage(reply);
}


/*! Send the DELETEGROUP command to the server. Deletes a user group from the server.
*/
void QwcSocket::deleteGroup(QString theName)
{
    QwMessage reply("DELETEGROUP");
    reply.appendArg(theName);
    sendMessage(reply);
}


/*! Send the DELETEUSER command. Deletes a user account from the database.
*/
void QwcSocket::deleteUser(QString theName)
{
    QwMessage reply("DELETEUSER");
    reply.appendArg(theName);
    sendMessage(reply);
}


void QwcSocket::readUser(QString theName)
{
    QwMessage reply("READUSER");
    reply.appendArg(theName);
    sendMessage(reply);
}


void QwcSocket::readGroup(QString theName)
{
    QwMessage reply("READGROUP");
    reply.appendArg(theName);
    sendMessage(reply);
}


void QwcSocket::tracker_request_servers()
{
    sendMessage(QwMessage("SERVERS"));
}

void QwcSocket::searchFiles(const QString theSearch)
{
    QwMessage reply("SEARCH");
    reply.appendArg(theSearch);
    sendMessage(reply);
}


void QwcSocket::sendMessageINFO()
{
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
    tmpOsVersion = tmpOsVersion.trimmed();
    QProcess shell;
    shell.start("uname -r");
    if(shell.waitForFinished()) {
        tmpOsVersion = shell.readAllStandardOutput();
    }
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

    sendMessage(QwMessage("CLIENT")
                .appendArg(tmpV.arg(pClientName, pClientVersion, tmpOsName, tmpOsVersion, tmpOsArch)));
}

// ///
// /// SESSION PROTOCOL SUPPORT
// ///

// Send a Wired message
void QwcSocket::sendWiredCommand(const QByteArray theData)
{
    if(!pSocket->isOpen()) return;
    pSocket->write(theData + char(kEOF));
}


// Return a list of parameters from the message, automatically skipping the command identifier.
QList<QByteArray> QwcSocket::GetFieldArray(QByteArray theData)
{
    QByteArray tmpData(theData);
    int tmpPos = theData.indexOf(" "); // Remove the identifier
    if( tmpPos!=-1 ) tmpData=tmpData.mid(tmpPos+1);
    QList<QByteArray> tmpParams = tmpData.split(28);
    return tmpParams;
}


void QwcSocket::cleanTransfers()
{
    qDebug() << this << "Cleaning transfers";
    QMutableListIterator<QPointer<QwcFiletransferSocket> > i(pTransferSockets);
    while(i.hasNext()) {
        QPointer<QwcFiletransferSocket> tmpP = i.next();
        if(tmpP.isNull()) i.remove();
    }
}

/**
 * Cancel a running transfer.
 * @param  The transfer to cancel.
 */
void QwcSocket::cancelTransfer(QwcFiletransferInfo theTransfer)
{
    QMutableListIterator<QPointer<QwcFiletransferSocket> > i(pTransferSockets);
    while(i.hasNext()) {
        QPointer<QwcFiletransferSocket> tmpTS = i.next();
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

QString QwcSocket::tranzlate(QString theText)
{
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

/// Disconnect the low level socket from the server.
/// Mainly used during connection cancellation.
void QwcSocket::disconnectSocketFromServer()
{
    if(!pSocket) return;
    qDebug() << this << ": Aborting connection to server.";
    pSocket->abort();
}


/// Activate/start the next locally queued transfer of type
/// type. If a transfer is already running, this will start
/// the next.
void QwcSocket::runTransferQueue(WiredTransfer::TransferType type)
{
    QListIterator<QPointer<QwcFiletransferSocket> > i(pTransferSockets);
    while(i.hasNext()) {
        QwcFiletransferSocket *tmpT = i.next();
        if(!tmpT) continue;
        QwcFiletransferInfo tmpTrans = tmpT->pTransfer;
        if( tmpTrans.pTransferType==type && tmpTrans.pStatus==WiredTransfer::StatusQueuedLocal ) {
            qDebug() << this << "Unfreezing transfer"<<tmpTrans.pRemotePath;
            tmpT->pTransfer.pStatus = WiredTransfer::StatusWaitingForStat;

            if(tmpTrans.pTransferType==WiredTransfer::TypeDownload) {
                statFile(tmpT->pTransfer.pRemotePath);

            } else if(tmpTrans.pTransferType==WiredTransfer::TypeFolderDownload) {
                getFileListRecusive(tmpTrans.pRemotePath);

            } else if(tmpTrans.pTransferType==WiredTransfer::TypeUpload) {

                QByteArray ba("PUT ");
                ba += tmpTrans.pRemotePath.toUtf8() + char(kFS);
                ba += QByteArray::number(tmpTrans.pTotalSize) + char(kFS);
                ba += tmpTrans.pChecksum;
                sendWiredCommand(ba);

            } else if(tmpTrans.pTransferType==WiredTransfer::TypeFolderUpload) {
                tmpT->pTransfer.pStatus = WiredTransfer::StatusActive;
                proceedFolderUpload(tmpT);

            }

            emit fileTransferStarted(tmpT->pTransfer);
            return;
        }

    }
}

bool QwcSocket::isTransferringFileOfType(WiredTransfer::TransferType type)
{
    QListIterator<QPointer<QwcFiletransferSocket> > i(pTransferSockets);
    while(i.hasNext()) {
        QwcFiletransferSocket *tmpT = i.next();
        if(!tmpT) continue;
        if(tmpT->pTransfer.pTransferType==type
           && (tmpT->pTransfer.pStatus==WiredTransfer::StatusActive || tmpT->pTransfer.pStatus==WiredTransfer::StatusWaitingForStat))
            return true;
    }
    return false;
}


void QwcSocket::getFileListRecusive(const QString & path)
{
    qDebug() << this << "Starting recursive indexing of" << path;
    pRecursiveFileListing.clear();
    pRecursivePath = path;
    pIndexingFiles = true;
    getFileList(path);
}


void QwcSocket::on_server_filelist_item(QList<QByteArray> params)
{
    QwcFileInfo file(params);
    if(!pIndexingFiles) {
        emit onFilesListItem(file);
    } else {
        file.isIndexed = false;
        pRecursiveFileListing.append(file);
    }
}


void QwcSocket::on_server_filelist_done(QList<QByteArray> params)
{
    if(!pIndexingFiles) {
        emit onFilesListDone(QString::fromUtf8(params.value(0)), params.value(1).toLongLong() );
    } else {
        // Listing of directory complete. Continue the listing.
        int maxIndexingDepth = 16;
        QMutableListIterator<QwcFileInfo> i(pRecursiveFileListing);
        while(i.hasNext()) {
            QwcFileInfo &file = i.next();
            if(file.type==WiredTransfer::Directory && !file.isIndexed && file.size && file.path.count("/")<=maxIndexingDepth ) {
                // 				qDebug() << this << "Indexing next directory:" << file.path;
                file.isIndexed = true;
                getFileList(file.path);
                return;
            }
        }

        // Find a transfer socket
        QListIterator<QPointer<QwcFiletransferSocket> > j(pTransferSockets);
        while(j.hasNext()) {
            QwcFiletransferSocket *tmpT = j.next();
            if(!tmpT) continue;
            QwcFiletransferInfo &transfer = tmpT->pTransfer;
            if(transfer.pRemotePath==pRecursivePath
               && transfer.pTransferType==WiredTransfer::TypeFolderDownload
               && transfer.pStatus==WiredTransfer::StatusWaitingForStat) {
                qDebug() << this << "Waiting folder transfer socket found.";
                transfer.fileList = pRecursiveFileListing;
                transfer.pFilesCount = pRecursiveFileListing.count();
                transfer.pFilesDone = 0;
                transfer.pFolderDone = 0;
                transfer.pFolderSize = 0;
                QListIterator<QwcFileInfo> k(pRecursiveFileListing);
                while(k.hasNext()) { transfer.pFolderSize+=k.next().size; }

                transfer.pRemoteFolder = pRecursivePath;
                transfer.pStatus = WiredTransfer::StatusActive;
                proceedFolderDownload(tmpT);
                return;
            }
        }

        // Finished, if we are here.
        pIndexingFiles = false;
        pRecursivePath.clear();
        // 		emit onFilesListRecursiveDone(pRecursiveFileListing);

    }
}


void QwcSocket::proceedFolderDownload(QwcFiletransferSocket *socket)
{
    // Take the next item in the folder download and handle it
    if(!socket) return;
    while(1) {
        if(socket->pTransfer.fileList.count()==0) {
            qDebug() << this << "Folder download done.";
            socket->pTransfer.pStatus = WiredTransfer::StatusDone;
            emit fileTransferDone(socket->pTransfer);
            return;
        };
        QwcFileInfo file = socket->pTransfer.fileList.takeFirst();
        qDebug() << this << "Proceeding download:"<<file.path;

        QString remoteRootName = socket->pTransfer.pRemoteFolder.section("/",-1,-1); // the name only of the remote root

        QString relativeRemotePath(file.path); // complete path on server (/Server/Files/file.a)
        relativeRemotePath.remove(socket->pTransfer.pRemoteFolder); // path from remote root to contained file (/Files/file.a)
        relativeRemotePath.prepend(remoteRootName); // prepend local root

        QDir dir(socket->pTransfer.pLocalRoot);
        dir.mkpath(remoteRootName); // Ensure the local root folder exists

        if(file.type == WiredTransfer::Directory || file.type == WiredTransfer::DropBox || file.type == WiredTransfer::Uploads ) { // create the directory
            dir.mkpath(relativeRemotePath);
            socket->pTransfer.pFilesDone++;
            qDebug() << "Creating directory:" << relativeRemotePath;

        } else if(file.type == WiredTransfer::RegularFile) {
            socket->pTransfer.pFileStatus = WiredTransfer::StatusWaitingForStat;
            socket->pTransfer.pQueuePosition = 0;
            socket->pTransfer.pRemotePath = file.path;
            socket->pTransfer.pLocalPath = dir.absoluteFilePath(relativeRemotePath);
            socket->pTransfer.pFilesDone++;
            if(!QFile(socket->pTransfer.pLocalPath).exists()) {
                statFile(file.path);
                qDebug() << "STATing:"<<file.path;
                return;
            }
        }
    }
}


void QwcSocket::proceedFolderUpload(QwcFiletransferSocket *socket)
{
    // Take the next item in the folder upload and handle it
    if(!socket) return;
    while(1) {
        if(socket->pTransfer.fileListLocal.count()==0) {
            qDebug() << this << "Folder upload done.";
            socket->pTransfer.pStatus = WiredTransfer::StatusDone;
            emit fileTransferDone(socket->pTransfer);
            return;
        };

        QString localPath = socket->pTransfer.fileListLocal.takeFirst();
        QFileInfo localInfo(localPath);
        qDebug() << this << "Proceeding upload:"<<localPath;

        // Construct the paths
        qDebug() << this << "Upload: Local root:"<<socket->pTransfer.pLocalRoot;

        QString relativePath = QString(localPath).remove(socket->pTransfer.pLocalRoot);
        QString relativePathRemote = socket->pTransfer.pRemoteFolder;
        relativePathRemote.append(relativePath);
        qDebug() << "RelativePath:"<<relativePathRemote;

        if(!localInfo.isHidden() && localInfo.isReadable()) {
            if(localInfo.isDir()) {
                qDebug() << this<<"Upload: Directory:"<<relativePathRemote;
                createFolder(relativePathRemote);

            } else if(localInfo.isFile()) {
                qDebug() << this<<"Upload: File:"<<relativePathRemote;
                socket->pTransfer.pLocalPath = localPath;
                socket->pTransfer.pRemotePath = relativePathRemote;
                socket->pTransfer.calcLocalChecksum();
                socket->pTransfer.pTotalSize = localInfo.size();
                socket->pTransfer.pOffset = 0;
                socket->pTransfer.pFileStatus = WiredTransfer::StatusQueued;

                // Request upload slot. Server will respond with offset if the file
                // was partially uploaded.
                QByteArray ba("PUT ");
                ba += relativePathRemote.toUtf8() + char(kFS);
                ba += QByteArray::number(localInfo.size()) + char(kFS);
                ba += socket->pTransfer.pChecksum;
                sendWiredCommand(ba);
                return;
            }
        }

    }
}


void QwcSocket::fileTransferFileDone(const QwcFiletransferInfo transfer)
{
    QListIterator<QPointer<QwcFiletransferSocket> > i(pTransferSockets);
    qDebug() << "Proceed upload 1.";
    while(i.hasNext()) {
        QwcFiletransferSocket *socket = i.next();
        if(!socket) continue;
        if(socket->pTransfer.pStatus!=WiredTransfer::StatusActive) continue;
        qDebug() << "Proceed upload x.";
        if(socket->pTransfer.pTransferType==WiredTransfer::TypeFolderDownload) {
            socket->pTransfer.pFolderDone += transfer.pTotalSize;
            proceedFolderDownload(socket);
            return;
        } else if(socket->pTransfer.pTransferType==WiredTransfer::TypeFolderUpload) {
            socket->pTransfer.pFolderDone += transfer.pTotalSize;
            qDebug() << "Proceed upload 2.";
            proceedFolderUpload(socket);
            return;
        }
    }

}




