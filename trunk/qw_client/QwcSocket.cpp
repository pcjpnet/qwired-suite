#include "QwcSocket.h"
#include "QwcGlobals.h"

#include <QtCore/QBuffer>
#include <QtCore/QProcess>

QwcSocket::QwcSocket(QObject *parent) :
        QwSocket(parent)
{
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
    m_clientName = "Qwired SVN";
    m_clientVersion = QWIRED_VERSION;
    m_caturdayFlag = false;
    m_localTransferQueueEnabled = false;

    m_invitedUserId = 0;

    pingTimerId = startTimer(60000); // 1 minute

}

QwcSocket::~QwcSocket()
{ }


/*! Returns a const reference to the current session user information. */
const QwcUserInfo & QwcSocket::sessionUser() const
{ return m_sessionUser; }

/*! Returns a const reference to the server-provided information. */
const QwServerInfo & QwcSocket::serverInformation() const
{ return m_serverInformation; }

/*! Returns a const reference to the servers banner data. */
const QImage & QwcSocket::serverBanner() const
{ return m_serverBanner; }

/*! Returns the PING latency between the client and the server. */
int QwcSocket::pingLatency() const
{ return m_pingLatency; }

/*! Returns a const reference to the list of currently used chat rooms. */
const QHash<int, QwRoom> & QwcSocket::chatRooms() const
{ return m_chatRooms; }

/*! Returns a const reference to the list of users connected to the remote server. */
const QHash<int, QwcUserInfo> & QwcSocket::users() const
{ return m_users; }

void QwcSocket::handleMessageReceived(const QwMessage &message)
{
    int commandId = message.commandName.toInt();
    if (commandId == 200) {           handleMessage200(message); // Login Successful
    } else if (commandId == 201) {    handleMessage201(message); // Server Information
    } else if (commandId == 202) {    handleMessage202(message); // Ping Reply
    } else if (commandId == 203) {    handleMessage203(message); // Server Banner
    } else if (commandId == 300) {    handleMessage300(message); // Chat
    } else if (commandId == 301) {    handleMessage301(message); // Action Chat
    } else if (commandId == 302) {    handleMessage302(message); // Client Join
    } else if (commandId == 303) {    handleMessage303(message); // Client Leave
    } else if (commandId == 304) {    handleMessage304(message); // Status Change
    } else if (commandId == 305) {    handleMessage305(message); // Private Message
    } else if (commandId == 306) {    handleMessage306(message); // Client Kicked
    } else if (commandId == 307) {    handleMessage307(message); // Client Banned
    } else if (commandId == 308) {    handleMessage308(message); // Client Information
    } else if (commandId == 309) {    handleMessage309(message); // Broadcast Message

    } else if (commandId == 310) {    handleMessage310(message); // User List
    } else if (commandId == 311) {    handleMessage311(message); // User List Done
    } else if (commandId == 320) {    handleMessage320(message); // News
    } else if (commandId == 321) {    handleMessage321(message); // News Done
    } else if (commandId == 322) {    handleMessage322(message); // News Posted
    } else if (commandId == 330) {    handleMessage330(message); // Private Chat Created
    } else if (commandId == 331) {    handleMessage331(message); // Private Chat Invitation
    } else if (commandId == 332) {    handleMessage332(message); // Private Chat Declined

    } else if (commandId == 340) {    handleMessage340(message); // Client Image Change
    } else if (commandId == 341) {    handleMessage341(message); // Chat Topic
    } else if (commandId == 400) {    handleMessage400(message); // Transfer Ready
    } else if (commandId == 401) {    handleMessage401(message); // Transfer Queued
    } else if (commandId == 402) {    handleMessage402(message); // File Information/STAT
    } else if (commandId == 410) {    handleMessage410(message); // File Listing
    } else if (commandId == 411) {    handleMessage411(message); // File Listing Done
    } else if (commandId == 420) {    handleMessage420(message); // Search Listing
    } else if (commandId == 421) {    handleMessage421(message); // Search Listing Done
    } else if (commandId >= 500 && commandId <= 599) {
                                      handleMessage5xx(commandId); // Server Errors
    } else if (commandId == 600) {    handleMessage600(message); // User Specification
    } else if (commandId == 601) {    handleMessage601(message); // Group Specification
    } else if (commandId == 602) {    handleMessage602(message); // Privileges Specification
    } else if (commandId == 610) {    handleMessage610(message); // User Listing
    } else if (commandId == 611) {    handleMessage611(message); // User Listing Done
    } else if (commandId == 620) {    handleMessage620(message); // Group Listing
    } else if (commandId == 621) {    handleMessage621(message); // Group Listing Done
    }
}


/*! 200 Server Information
    Received right after login and HELLO request.
*/
void QwcSocket::handleMessage200(const QwMessage &message)
{
    // Server Information
    m_serverInformation.serverVersion = message.stringArg(0);
    m_serverInformation.protocolVersion = message.stringArg(1);
    m_serverInformation.name = message.stringArg(2);
    m_serverInformation.description = message.stringArg(3);
    m_serverInformation.startTime = QDateTime::fromString(message.stringArg(4), Qt::ISODate);

    if (m_serverInformation.protocolVersion == "1.1") {
        m_serverInformation.filesCount = message.stringArg(5).toInt();
        m_serverInformation.filesSize = message.stringArg(6).toLongLong();
    }

    // Send login sequence
    sendMessageINFO();
    setNickname(m_sessionUser.userNickname);
    setUserIcon(m_sessionUser.userImage());
    setUserStatus(m_sessionUser.userStatus);
    sendMessage(QwMessage("USER").appendArg(m_sessionUser.name));
    sendMessage(QwMessage("PASS").appendArg(m_sessionUser.cryptedPassword()));
    emit onServerInformation();
}


/*! 201 Login Succeeded
    Received right after login succeeded and the session is active.
*/
void QwcSocket::handleMessage201(const QwMessage &message)
{
    m_sessionUser.pUserID = message.stringArg(0).toInt();
    sendMessage(QwMessage("WHO").appendArg(1));
    sendMessage(QwMessage("BANNER"));
    sendMessage(QwMessage("PRIVILEGES"));
    emit onServerLoginSuccessful();
}


/*! 202 Ping Reply
    Received in response to the sendPing() command.
*/
void QwcSocket::handleMessage202(const QwMessage &)
{
    m_pingLatency = pingLagTimer.elapsed();
    emit pingLatency(m_pingLatency);
}


/*! 203 Server Banner
    Received in response to BANNER.
*/
void QwcSocket::handleMessage203(const QwMessage &message)
{
    m_serverBanner.loadFromData(QByteArray::fromBase64(message.stringArg(0).toAscii()));
    emit serverBannerReceived(QPixmap::fromImage(m_serverBanner));
}


/*! 300 Chat
    A chat message has been posted in a room.
*/
void QwcSocket::handleMessage300(const QwMessage &message)
{
    emit receivedChatMessage(message.stringArg(0).toInt(),
                             message.stringArg(1).toInt(),
                             message.stringArg(2),
                             false);
}


/*! 301 Action Chat
    A "action" chat message has been posted in a room.
*/
void QwcSocket::handleMessage301(const QwMessage &message)
{
    emit receivedChatMessage(message.stringArg(0).toInt(), message.stringArg(1).toInt(),
                             message.stringArg(2), true);
}


/*! 302 Client Join
    A client has joined a room.
*/
void QwcSocket::handleMessage302(const QwMessage &message)
{
    int roomId = message.stringArg(0).toInt();
    QwcUserInfo newUser = QwcUserInfo::fromMessage310(&message);
    if (roomId == 1) {
        m_users[newUser.pUserID] = newUser;
        m_chatRooms[1].pUsers.append(newUser.pUserID);
    } else {
        if (!m_chatRooms.contains(roomId)) { return; }
        m_chatRooms[roomId].pUsers.append(newUser.pUserID);
    }
    emit userJoinedRoom(roomId, newUser);
}


/*! 303 Client Leave
    A client has left a room.
*/
void QwcSocket::handleMessage303(const QwMessage &message)
{
    int roomId = message.stringArg(0).toInt();
    int userId = message.stringArg(1).toInt();
    QwcUserInfo targetUser = m_users.value(userId);
    if (!m_chatRooms.contains(roomId)) { return; }
    if (roomId == 1) {
        // If the client leaves room 1, it has disconnected and should be removed from all rooms.
        QMutableHashIterator<int, QwRoom> i(m_chatRooms);
        while (i.hasNext()) {
            i.next();
            i.value().pUsers.removeAll(userId);
            emit userLeftRoom(i.key(), targetUser);
        }
    } else {
        // Remove the user from a single room
        QwRoom &targetRoom = m_chatRooms[roomId];
        if (targetRoom.pUsers.contains(userId)) {
            targetRoom.pUsers.removeAll(userId);
            emit userLeftRoom(roomId, targetUser);
        }
    }
}


/*! 304 Status Change
    A client has changed nickame or other parameters.
*/
void QwcSocket::handleMessage304(const QwMessage &message)
{
    int userId = message.stringArg(0).toInt();
    if (!m_users.contains(userId)) { return; }
    QwcUserInfo targetUserOld = m_users[userId];
    QwcUserInfo &targetUser = m_users[userId];
    targetUser.pIdle = message.stringArg(1).toInt();
    targetUser.pAdmin = message.stringArg(2).toInt();
    targetUser.pIcon = message.stringArg(3).toInt();
    targetUser.userNickname = message.stringArg(4);
    targetUser.userStatus = message.stringArg(5);
    emit userChanged(targetUserOld, targetUser);
}


/*! 305 Private Message
    Another client has sent a private message.
*/
void QwcSocket::handleMessage305(const QwMessage &message)
{
    QwcUserInfo senderUser = m_users[message.stringArg(0).toInt()];
    QString text = message.stringArg(1);
    emit privateMessage(senderUser, text);
}


/*! 306 Client Kicked
    A client was kicked by an administrator.
*/
void QwcSocket::handleMessage306(const QwMessage &message)
{
    QwcUserInfo victim = m_users[message.stringArg(0).toInt()];
    QwcUserInfo killer = m_users[message.stringArg(1).toInt()];
    QString reason = message.stringArg(2);

    // User message handler for 303 Client Left to remove the user
    emit userKicked(victim, killer, reason);
    handleMessage303(QwMessage().appendArg(1)
                     .appendArg(QString::number(victim.pUserID)));
}


/*! 307 Client Banned
    A client was banned by an administrator.
*/
void QwcSocket::handleMessage307(const QwMessage &message)
{
    QwcUserInfo victim = m_users[message.stringArg(0).toInt()];
    QwcUserInfo killer = m_users[message.stringArg(1).toInt()];
    QString reason = message.stringArg(2);

    // User message handler for 303 Client Left to remove the user
    handleMessage303(QwMessage().appendArg(1)
                     .appendArg(QString::number(victim.pUserID)));
    emit userBanned(victim, killer, reason);
}


/*! 308 Client Information
    Client information, in response to INFO. */
void QwcSocket::handleMessage308(const QwMessage &message)
{ emit userInformation(QwcUserInfo::fromMessage308(&message)); }

/*! 309 Broadcast Message
    A administrator sent a broadcast message. */
void QwcSocket::handleMessage309(const QwMessage &message)
{
    QwcUserInfo senderUser = m_users[message.stringArg(0).toInt()];
    emit broadcastMessage(senderUser, message.stringArg(1));
}


/*! 310 User List
    One item of the user list of a specific room. In response to WHO.
*/
void QwcSocket::handleMessage310(const QwMessage &message)
{
    int roomId = message.stringArg(0).toInt();
    QwcUserInfo targetUser = QwcUserInfo::fromMessage310(&message);
    if (roomId == 1) {
        m_users[targetUser.pUserID] = targetUser;
    }
    m_chatRooms[roomId].pUsers.append(targetUser.pUserID);
}


/*! 311 User List Done
    Received after a list of 310 messages and indicates the end of the list.
*/
void QwcSocket::handleMessage311(const QwMessage &message)
{
    int tmpChannel = message.stringArg(0).toInt();
    emit receivedUserlist(tmpChannel);
}


/*! 320 News
    Received after a NEWS request.
    \todo Qt's ISODate parser does not support the timezone offsets. This needs to be worked around
          in the future.
*/
void QwcSocket::handleMessage320(const QwMessage &message)
{
    QDateTime date = QDateTime::fromString(message.stringArg(1), Qt::ISODate);
    emit newsListingItem(message.stringArg(0), date, message.stringArg(2));
}


/*! 321 News Done */
void QwcSocket::handleMessage321(const QwMessage &)
{ emit newsListingDone(); }


/*! 322 News Posted */
void QwcSocket::handleMessage322(const QwMessage &message)
{
    QDateTime date = QDateTime::fromString(message.stringArg(1), Qt::ISODate);
    emit newsPosted(message.stringArg(0), date, message.stringArg(2));
}


/*! 330 Private Chat Created */
void QwcSocket::handleMessage330(const QwMessage &message)
{
    int roomId = message.stringArg(0).toInt();

    QwRoom newRoom;
    newRoom.pChatId = roomId;
    m_chatRooms[roomId] = newRoom;

    emit privateChatCreated(roomId);

    if (m_invitedUserId > 0) {
        inviteClientToChat(roomId, m_invitedUserId);
        m_invitedUserId = 0;
    }
}


/*! 331 Private Chat Invitation
    User was invited to a private chat.
*/
void QwcSocket::handleMessage331(const QwMessage &message)
{
    QwcUserInfo senderUser = m_users[message.stringArg(1).toInt()];
    emit privateChatInvitation(message.stringArg(0).toInt(), senderUser);
}

/*! 332 Private Chat Declined
    Invited user declined the invitation to a private chat.
*/
void QwcSocket::handleMessage332(const QwMessage &message)
{
    QwcUserInfo senderUser = m_users[message.stringArg(1).toInt()];
    emit privateChatDeclined(message.stringArg(0).toInt(), senderUser);
}


/*! 340 Client Image Change
    The image of a client was changed.
*/
void QwcSocket::handleMessage340(const QwMessage &message)
{
    int userId = message.stringArg(0).toInt();
    if (!m_users.contains(userId)) { return; }
    QwcUserInfo targetUserOld = m_users[userId];
    m_users[userId].setImageFromData(QByteArray::fromBase64(message.stringArg(1).toAscii()));
    emit userChanged(targetUserOld, m_users[userId]);
}


/*! 341 Chat Topic
    Server informs about the topic of a chat room.
*/
void QwcSocket::handleMessage341(const QwMessage &message)
{
    emit onChatTopic(message.stringArg(0).toInt(), // chat id
                     message.stringArg(1), // nickname
                     message.stringArg(2), // login
                     QHostAddress( message.stringArg(3) ), // ip
                     QDateTime::fromString(message.stringArg(4), Qt::ISODate), // date-time
                     message.stringArg(5)); // topic
}


/*! 400 Transfer Ready
    A transfer is ready and the server is waiting for a transfer connection.
*/
void QwcSocket::handleMessage400(const QwMessage &message)
{
    emit transferReady(message.stringArg(0), message.stringArg(1).toLongLong(),
                       message.stringArg(2));
}


/*! 401 Transfer Queued
    A transfer has changed position in the server queue. */
void QwcSocket::handleMessage401(const QwMessage &message)
{ emit transferQueued(message.stringArg(0), message.stringArg(1).toInt()); }


/*! 402 File Information
    The server returned information about a file. */
void QwcSocket::handleMessage402(const QwMessage &message)
{
    QwFile fileInfo;
    fileInfo.setRemotePath(message.stringArg(0));
    fileInfo.setType((Qw::FileType)message.stringArg(1).toInt());
    fileInfo.setSize(message.stringArg(2).toLongLong());
    fileInfo.setCreated(QDateTime::fromString(message.stringArg(3), Qt::ISODate));
    fileInfo.setModified(QDateTime::fromString(message.stringArg(4), Qt::ISODate));
    fileInfo.setChecksum(message.stringArg(5));
    fileInfo.setComment(message.stringArg(6));
    emit fileInformation(fileInfo);
}


/*! 410 File Listing
    Received a single entry of the requested list of files. */
void QwcSocket::handleMessage410(const QwMessage &message)
{
    QwcFileInfo file;
    file.setFromMessage410(message);
    emit onFilesListItem(file);
}


/*! 411 File Listing Done */
void QwcSocket::handleMessage411(const QwMessage &message)
{ emit onFilesListDone(message.stringArg(0), message.stringArg(1).toLongLong()); }


/*! 420 Search Listing
    Received a result from the current search. */
void QwcSocket::handleMessage420(const QwMessage &message)
{
    QwcFileInfo newInfo;
    newInfo.setRemotePath(message.stringArg(0));
    newInfo.setType((Qw::FileType)message.stringArg(1).toInt());
    newInfo.setSize(message.stringArg(2).toLongLong());
    newInfo.setCreated(QDateTime::fromString(message.stringArg(3), Qt::ISODate));
    newInfo.setModified(QDateTime::fromString(message.stringArg(4), Qt::ISODate));
    emit fileSearchResultListItem(newInfo);
}


/*! 421 Search Listing Done */
void QwcSocket::handleMessage421(const QwMessage &)
{ emit fileSearchResultListDone(); }


/*! 5xx Error Message
    Handle some errors, otherwise pass them to the application code.
*/
void QwcSocket::handleMessage5xx(const int &errorId)
{
    if (errorId == Qw::ErrorLoginFailed) {
        m_socket->disconnectFromHost();
    } else if (errorId == Qw::ErrorBanned) {
        m_socket->disconnectFromHost();
    }
    emit protocolError((Qw::ProtocolError)errorId);
}


/*! 600 User Specification
    Specifies the details of a user account.
*/
void QwcSocket::handleMessage600(const QwMessage &message)
{
    QwMessage newMessage = message;
    QwcUserInfo user;
    user.userType = Qws::UserTypeAccount;
    user.name = newMessage.stringArg(0);
    user.pPassword = newMessage.stringArg(1);
    user.pGroupName = newMessage.stringArg(2);
    newMessage.arguments.removeFirst();
    newMessage.arguments.removeFirst();
    newMessage.arguments.removeFirst();
    user.setPrivilegesFromMessage602(&newMessage);
    emit userSpecReceived(user);
}


/*! 601 Group Specification
    Specifies the details of a user group.
*/
void QwcSocket::handleMessage601(const QwMessage &message)
{
    QwMessage newMessage = message;
    QwcUserInfo user;
    user.userType = Qws::UserTypeGroup;
    user.name = newMessage.stringArg(0);
    newMessage.arguments.removeFirst();
    user.setPrivilegesFromMessage602(&message);
    emit groupSpecReceived(user);
}


/*! 602 Privileges Specification
    Received from the server or after PRIVILEGES command.
*/
void QwcSocket::handleMessage602(const QwMessage &message)
{
    m_sessionUser.setPrivilegesFromMessage602(&message);
    emit receivedUserPrivileges();
}


/*! 610 User Listing */
void QwcSocket::handleMessage610(const QwMessage &message)
{ m_accountListingCache.append(message.stringArg(0)); }


/*! 611 User Listing Done */
void QwcSocket::handleMessage611(const QwMessage &)
{
    emit receivedAccountList(m_accountListingCache);
    m_accountListingCache.clear();
}

/*! 620 Group Listing */
void QwcSocket::handleMessage620(const QwMessage &message)
{ m_groupListingCache.append(message.stringArg(0)); }

/*! 621 Group Listing Done */
void QwcSocket::handleMessage621(const QwMessage &)
{
    emit receivedAccountGroupList(m_groupListingCache);
    m_groupListingCache.clear();
}

/*! Attempt to establish a connection to a remote server.
*/
void QwcSocket::connectToServer(const QString &hostName, int port)
{
    if (hostName.contains(":")) {
        // Port specified
        m_serverAddress = hostName.section(":",0,0);
        m_serverPort = hostName.section(":",1,1).toInt();
    } else {
        // No port defined
        m_serverPort = port;
        m_serverAddress = hostName;
    }
    m_socket->connectToHostEncrypted(m_serverAddress, m_serverPort);
}


/*! Clean up the socket after disconnecting to be prepared for new connections.
*/
void QwcSocket::disconnectFromServer()
{
    m_socket->disconnectFromHost();
    m_groupListingCache.clear();
    m_accountListingCache.clear();
    m_invitedUserId = 0;
    m_users.clear();
    m_chatRooms.clear();
}


void QwcSocket::setLocalTransferQueueEnabled(bool enabled)
{ m_localTransferQueueEnabled = enabled; }

bool QwcSocket::localTransferQueueEnabled() const
{ return m_localTransferQueueEnabled; }

// Set the username and password for the login sequence.
void QwcSocket::setUserAccount(QString theAcct, QString thePass)
{
    m_sessionUser.name = theAcct;
    m_sessionUser.pPassword = thePass;
}


// Do I really have to comment this? :D
void QwcSocket::setCaturdayMode(bool b) {
    // everyday iz caturday!  :3
    if(b) {
        m_tranzlator.clear();
        setUserIcon( QImage(":/icons/icon_happycat.png") );
        setUserStatus(tr("kittehday nait fevrar"));
        QString tmpNick = m_sessionUser.userNickname;
        tmpNick = tmpNick.replace("s","z");
        tmpNick = tmpNick.replace("e","3");
        tmpNick = tmpNick.replace("i","ie");
        tmpNick = tmpNick.replace("y","eh");
        setNickname(tmpNick);
        QFile tmpF(":/tranzlator.txt");
        if(tmpF.exists() && tmpF.open(QIODevice::ReadOnly)) {
            while(!tmpF.atEnd()) {
                QString tmpL = QString::fromUtf8(tmpF.readLine()).trimmed();
                QString tmpKey = tmpL.section(": ", 0, 0);
                QString tmpVal = tmpL.section(": ", 1, 1);
                m_tranzlator[tmpKey]=tmpVal;
            }
            tmpF.close();
            m_caturdayFlag = true;
            return;
        }
    }
    m_caturdayFlag = false;
}


/*! Send a PING command to the remote server. */
void QwcSocket::sendPing()
{
    sendMessage(QwMessage("PING"));
    pingLagTimer.restart();
}

/*! The SSL connection was established. Now send the session request HELLO. */
void QwcSocket::handleSocketConnected()
{
    sendMessage(QwMessage("HELLO"));
    emit socketConnected();
}


/*! The connection to the server was lost.
*/
void QwcSocket::handleSocketConnectionLost()
{
    disconnectFromServer();
    m_socket->disconnectFromHost();
}

/*! Send a private message to the user with the id \a userId. If \a userId is 0, the private is sent
    as a broadcast message to all connected users, provided that the broadcast privilege is set.
*/
void QwcSocket::sendPrivateMessage(int userId, QString message)
{
    if (m_caturdayFlag) {
        message = tranzlate(message);
    }
    if(userId == 0) { // Broadcast Message
        sendMessage(QwMessage("BROADCAST").appendArg(message));
    } else { // Send a private message to a user.
        sendMessage(QwMessage("MSG").appendArg(QString::number(userId)).appendArg(message));
    }
}


/*! Leave a previously joined chat room.
*/
void QwcSocket::leaveChat(int roomId)
{
    sendMessage(QwMessage("LEAVE").appendArg(QString::number(roomId)));
    if (m_chatRooms.contains(roomId)) {
        m_chatRooms.remove(roomId);
    }
}


/*! Request the creation of a new private chat on the server. The server will respond with the ID
    of the new chat room. The user identified by \a firstInvitedUser is the user that is invited
    right after the new room has been created - you can set this to 0 to just create a new room.
*/
void QwcSocket::createChatWithClient(int firstInvitedUser)
{
    m_invitedUserId = firstInvitedUser;
    sendMessage(QwMessage("PRIVCHAT"));
}


/*! Invite a client to a private chat. \a chatId can be obtained by creating a new chat or inviting
    a client to an existing chat room. */
void QwcSocket::inviteClientToChat(int chatId, int userId)
{
    sendMessage(QwMessage("INVITE")
              .appendArg(QString::number(userId))
              .appendArg(QString::number(chatId)));
}


/*! Clear the news on the server. */
void QwcSocket::clearNews()
{sendMessage(QwMessage("CLEARNEWS")); }

/*! Request the list of files at the specified path. */
void QwcSocket::getFileList(const QString &path)
{ sendMessage(QwMessage("LIST").appendArg(path)); }

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
    m_sessionUser.setUserImage(icon);
    sendMessage(QwMessage("ICON").appendArg(0).appendArg(imageData.toBase64()));
}


/*! Set the current user nickname and send it to the server.
*/
void QwcSocket::setNickname(QString theNick)
{
    m_sessionUser.userNickname = theNick;
    sendMessage(QwMessage("NICK").appendArg(theNick));
}


/*! Set the current user status and send it to the server.
*/
void QwcSocket::setUserStatus(QString theStatus)
{
    m_sessionUser.userStatus = theStatus;
    sendMessage(QwMessage("STATUS").appendArg(m_sessionUser.userStatus));
}


/*! Decline an invitation to a private chat. */
void QwcSocket::declineChatInvitation(int roomId)
{ sendMessage(QwMessage("DECLINE").appendArg(QString::number(roomId))); }

/*! Join (accept invitation) to a private chat room. */
void QwcSocket::joinChat(int roomId)
{ sendMessage(QwMessage("JOIN").appendArg(QString::number(roomId))); }

/*! Set the topic of the specified chat room. */
void QwcSocket::setChatTopic(int chatId, QString topic)
{ sendMessage(QwMessage("TOPIC").appendArg(QString::number(chatId)).appendArg(topic)); }

/*! Temporarily ban a user from the server. \a reason is a short text describing the reason
   for the ban. */
void QwcSocket::banClient(int userId, const QString &reason)
{ sendMessage(QwMessage("BAN").appendArg(QString::number(userId)).appendArg(reason)); }

/*! Request the list of connected users of a specific room. */
void QwcSocket::getUserlist(int roomId)
{ sendMessage(QwMessage("WHO").appendArg(QString::number(roomId))); }

/*! Request the list of news articles. */
void QwcSocket::getNews()
{ sendMessage(QwMessage("NEWS")); }

/*! Post a new article to the news discussion board. */
void QwcSocket::postNews(const QString &text)
{ sendMessage(QwMessage("POST").appendArg(text)); }

/*! Request information about a connected client. */
void QwcSocket::getClientInformation(int userId)
{ sendMessage(QwMessage("INFO").appendArg(QString::number(userId))); }

/*! Send a chat message to the chat room with id \a chat.
    If \a emote is true, the text will be sent as action. \a chat must be a valid chat room ID, or
    1 if you want to write to the public chat.
*/
void QwcSocket::writeToChat(int chat, QString text, bool emote)
{
    if (m_caturdayFlag) {
        text = tranzlate(text);
    }
    QwMessage reply(emote ? "ME" : "SAY");
    reply.appendArg(QString::number(chat));
    reply.appendArg(text);
    sendMessage(reply);
}


/*! Request file information (stat) of a specified path. */
void QwcSocket::getFileInformation(const QString &path)
{ sendMessage(QwMessage("STAT").appendArg(path)); }

/*! Set the comment of a file or folder on the server. */
void QwcSocket::setFileComment(const QString &path, const QString &comment)
{ sendMessage(QwMessage("COMMENT").appendArg(path).appendArg(comment)); }



/*! Creates a transfer for this connection. If \e remotePath ends with a slash (/), the transfer is
    considered to be a folder transfer, otherwise it's considered to be a file transfer.
    \returns Returns a unique ID which identifies this transfer for this connection
*/
Qwc::TransferId QwcSocket::downloadPath(const QString &remotePath, const QString &localPath)
{
    Qwc::TransferType transferType;
    if (remotePath.endsWith("/")) {
        transferType = Qwc::TransferTypeFolderDownload;
    } else {
        transferType = Qwc::TransferTypeFileDownload;
    }
    QwcTransfer *newTransfer = new QwcTransfer(transferType, this);
    newTransfer->setSocket(this);
    newTransfer->setRemotePath(remotePath);
    newTransfer->setLocalPath(localPath);

    connect(newTransfer, SIGNAL(transferEnded()),
            this, SLOT(handleTransferEnded()));
    connect(newTransfer, SIGNAL(transferChanged()),
            this, SLOT(handleTransferChanged()));

    m_transfers[newTransfer->id()] = newTransfer;
    emit transferCreated(newTransfer);
    checkTransferQueue();
    return newTransfer->id();
}

/*! Creates a transfer for this connection. If \e localPath ends with a slash (/), the transfer is
    considered to be a folder transfer, otherwise it's considered to be a file transfer.
    \returns Returns a unique ID which identifies this transfer for this connection
*/
Qwc::TransferId QwcSocket::uploadPath(const QString &localPath, const QString &remotePath)
{
    Qwc::TransferType transferType;
    if (remotePath.endsWith("/")) {
        transferType = Qwc::TransferTypeFolderUpload;
    } else {
        transferType = Qwc::TransferTypeFileUpload;
    }
    QwcTransfer *newTransfer = new QwcTransfer(transferType, this);
    newTransfer->setSocket(this);
    newTransfer->setRemotePath(remotePath);
    newTransfer->setLocalPath(localPath);

    connect(newTransfer, SIGNAL(transferEnded()),
            this, SLOT(handleTransferEnded()));
    connect(newTransfer, SIGNAL(transferChanged()),
            this, SLOT(handleTransferChanged()));

    m_transfers[newTransfer->id()] = newTransfer;
    emit transferCreated(newTransfer);
    checkTransferQueue();
    return newTransfer->id();
}


/*! Remove a transfer from the internal list of transfer tasks, automatically stop it and notify
    other parts about this using the \e transferRemoved() signal. The transfer task object will be
    automatically deleted. Will return true if successful.
*/
bool QwcSocket::removeTransfer(QwcTransfer *transfer)
{
    if (!transfer) { return false; }
    if (!m_transfers.contains(transfer->id())) { return false; }

    transfer->stop();
    m_transfers.remove(transfer->id());
    emit transferRemoved(transfer);
    delete transfer;
    return true;
}


/*! Create a folder at the specified path. */
void QwcSocket::createFolder(const QString &path)
{ sendMessage(QwMessage("FOLDER").appendArg(path)); }

/*! Disconnect (Kick) a user from the server. \a reason provides a short reason why the user was
    disconnected. */
void QwcSocket::kickClient(int userId, const QString &reason)
{ sendMessage(QwMessage("KICK").appendArg(QString::number(userId)).appendArg(reason)); }

/*! Move (or rename) a file or directory on the server. */
void QwcSocket::moveFile(const QString &source, const QString &destination)
{ sendMessage(QwMessage("MOVE").appendArg(source).appendArg(destination)); }

/*! Request a download transfer slot for the remote file at \e path. */
void QwcSocket::getFile(const QString &path, qint64 offset)
{ sendMessage(QwMessage("GET").appendArg(path).appendArg(offset)); }

/*! Request a upload transfer slot for a file, with a given \e size and the data \e checksum. */
void QwcSocket::putFile(const QString &path, qint64 size, const QString &checksum)
{ sendMessage(QwMessage("PUT").appendArg(path).appendArg(size).appendArg(checksum)); }

/*! Set the type of a folder.
    \warning Please note that you can not use the type \e FileTypeRegular for this command. */
void QwcSocket::setFolderType(const QString &path, Qw::FileType type)
{ sendMessage(QwMessage("TYPE").appendArg(path).appendArg((int)type)); }

/*! A transfer has changed. We emit a signal from here to notify the rest of the application code.
*/
void QwcSocket::handleTransferChanged()
{
    QwcTransfer *transfer = dynamic_cast<QwcTransfer*>(sender());
    if (!transfer) { return; }
    emit transferChanged(transfer);
}

/*! A transfer has ended. We have to check if there are other transfers in the queue.
*/
void QwcSocket::handleTransferEnded()
{ checkTransferQueue(); }

/*! Delete a file or directory from the server. */
void QwcSocket::deleteFile(const QString &path)
{
    if (path == "/" || path.isEmpty()) { return; }
    sendMessage(QwMessage("DELETE").appendArg(path));
}

/*! Request the list of account groups from the server. */
void QwcSocket::getGroups()
{ sendMessage(QwMessage("GROUPS")); }

/*! Request the list of accounts from the server. */
void QwcSocket::getUsers()
{ sendMessage(QwMessage("USERS")); }

/*! Create a new user account on the server. */
void QwcSocket::createUser(QwcUserInfo user)
{
    QwMessage message("CREATEUSER");
    message.appendArg(user.name).appendArg(user.pPassword).appendArg(user.pGroupName);
    user.appendPrivilegesFlags(&message);
    sendMessage(message);
}

/*! Modify an existing account on the server. */
void QwcSocket::editUser(QwcUserInfo user)
{
    QwMessage message("EDITUSER");
    message.appendArg(user.name).appendArg(user.pPassword).appendArg(user.pGroupName);
    user.appendPrivilegesFlags(&message);
    sendMessage(message);
}

/*! Send the CREATEGROUP command to the server. Creates a user group on the server (uses login and
    privileges from \a user object.
*/
void QwcSocket::createGroup(QwcUserInfo user)
{
    QwMessage reply("CREATEGROUP");
    reply.appendArg(user.name);
    user.appendPrivilegesFlags(&reply);
    sendMessage(reply);
}

/*! Send the EDITGROUP command to the server. Modify a group on the server (uses login and
    privileges flags).
*/
void QwcSocket::editGroup(QwcUserInfo user)
{
    QwMessage reply("EDITGROUP");
    reply.appendArg(user.name);
    user.appendPrivilegesFlags(&reply);
    sendMessage(reply);
}

/*! Send the DELETEGROUP command to the server. Deletes a user group from the server.
*/
void QwcSocket::deleteGroup(QString theName)
{ sendMessage(QwMessage("DELETEGROUP").appendArg(theName)); }

/*! Send the DELETEUSER command. Deletes a user account from the database. */
void QwcSocket::deleteUser(QString theName)
{ sendMessage(QwMessage("DELETEUSER").appendArg(theName)); }

/*! Request details about an account. */
void QwcSocket::readUser(QString theName)
{ sendMessage(QwMessage("READUSER").appendArg(theName)); }

/*! Request details about an account group. */
void QwcSocket::readGroup(QString theName)
{ sendMessage(QwMessage("READGROUP").appendArg(theName)); }

/*! Search files on the server.
    This will emit a \a fileSearchDone() signal. */
void QwcSocket::searchFiles(const QString &search)
{ sendMessage(QwMessage("SEARCH").appendArg(search)); }


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
                case QSysInfo::MV_10_6: tmpOsVersion="10.6"; break;
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
    case QSysInfo::WV_VISTA: tmpOsVersion="Windows Vista"; break;  /* <- bwahahahahahaha ;) */
    case QSysInfo::WV_WINDOWS7: tmpOsVersion="Windows 7"; break;
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
                .appendArg(tmpV.arg(m_clientName, m_clientVersion, tmpOsName, tmpOsVersion, tmpOsArch)));
}


/*! Check the current transfer queue and start a new transfer if there are free slots.
*/
void QwcSocket::checkTransferQueue()
{
    if (m_localTransferQueueEnabled) {
        // Check if we have active transfers already
        foreach (QwcTransfer *transfer, m_transfers) {
            if (!transfer) { continue; }
            if (transfer->state() == Qwc::TransferStateRunning
                || transfer->state() == Qwc::TransferStateQueuedOnServer) {
                return;
            }
        }
    }

    // If we can start a new transfer, do so.
    foreach (QwcTransfer *transfer, m_transfers) {
        if (!transfer) { continue; }
        if (transfer->state() == Qwc::TransferStateQueuedOnClient) {
            transfer->start();
            if (m_localTransferQueueEnabled) {
                // Start just *one* new transfer at a time.
                return;
            }
        }
    }
}


/*! We'z got to tranzlate that crap. */
QString QwcSocket::tranzlate(QString theText)
{
    QString tmpText = theText.toLower();
    QString tmpResult;

    QStringList tmpWords = tmpText.split(" ");
    QStringListIterator i(tmpWords);
    while(i.hasNext()) {
        QString tmpWord = i.next();
        if(m_tranzlator.contains(tmpWord))
            tmpWord = m_tranzlator.value(tmpWord);
        tmpResult += QString(" ")+tmpWord;
    }

    tmpResult = tmpResult.trimmed();
    tmpResult = tmpResult.replace(":d", ":3");
    tmpResult = tmpResult.replace("?", "!?");
    tmpResult = tmpResult.replace("!", "?!!11one");
    return tmpResult;
}



/*! Request the list of files recursively from the remote server. This is basically the same as the
    getFileList() command, but will return all directory contents below the specified path. ("/"
    would list all files on the remote server.
*/
void QwcSocket::getFileListRecusive(const QString & path)
{ sendMessage(QwMessage("LISTRECURSIVE").appendArg(path)); }


/*! The QObject's timer event, used to send periodic PING commands to the server.
*/
void QwcSocket::timerEvent(QTimerEvent *event)
{
    if (!event) { return; }
    if (event->timerId() == pingTimerId
        && m_socket->state() == QAbstractSocket::ConnectedState) {
        sendPing();
    }
}




