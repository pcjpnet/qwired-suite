/*! \class QwsClientSocket
    \author Bastian Bense <bastibense@gmail.com>
    \brief General client management and local data store access.
    \date 2009-02-22

    \todo (A) Add support for comments and folder types
    \todo (B) Add support for separate user base directories

*/

#include "QwsClientSocket.h"
#include "QwsServerController.h"
#include "QwMessage.h"

#include <QDateTime>
#include <QDirIterator>
#include <QtSql>

#ifdef Q_OS_UNIX
#include <sys/statvfs.h>
#endif

#ifdef Q_OS_WIN32
#include <Windows.h>
#include <WinBase.h>
#endif

QwsClientSocket::QwsClientSocket(QObject *parent) : QwSocket(parent)
{
    qRegisterMetaType<QwMessage>();

    // A new socket is always in teh inactive state.
    m_sessionState = Qws::StateInactive;

    serverInfo = NULL;

    // Set up the idle timer and connect it to the respective handlers
    pIdleTimer = new QTimer(this);
    pIdleTimer->setInterval(1000*60*10); // 10 minutes
    pIdleTimer->setSingleShot(true);
    connect(pIdleTimer, SIGNAL(timeout()),
            this, SLOT(idleTimerTriggered()));


}


QwsClientSocket::~QwsClientSocket()
{
    qDebug() << "[qws] Destroying"<<user.pUserID;
}


/**
 * The idle timer was triggered. Check if the user is not idle and mark him/her
 * as idle. Afterwards a signal will be emitted so that other clients get updated.
 */
void QwsClientSocket::idleTimerTriggered()
{
    if (user.pIdle) { return; }
    user.pIdle = true;
    emit userStatusChanged();
}


/*! The user shows some activity. Check if he/she is idle and un-idle and change the user status
    if needed.
*/
void QwsClientSocket::resetIdleTimer()
{
    pIdleTimer->start();
    user.pIdleTime = QDateTime::currentDateTime();
    if (!user.pIdle) { return; }
    user.pIdle = false;
    emit userStatusChanged();
}

void QwsClientSocket::resolveHostname()
{
    qDebug() << this << "Beginning host lookup for:" << user.userIpAddress;
    QHostInfo::lookupHost(user.userIpAddress, this, SLOT(handleHostLookupResult(QHostInfo)));
}


// Called by the socket and indicates the an SSL error has occoured.
void QwsClientSocket::on_socket_sslErrors(const QList<QSslError> & errors)
{
        qDebug() << "QwsClientSocket.on_socket_sslErrors(): "<<errors;
	pSocket->ignoreSslErrors();
}


/*! A socket error ocurred.
*/
void QwsClientSocket::on_socket_error()
{
    qDebug() << "[qws] socket error:"<<pSocket->errorString()<<pSocket->error();
    disconnectClient();
}


/*! Handles the response of an asynchronous DNS lookup on the user's IP address.
*/
void QwsClientSocket::handleHostLookupResult(QHostInfo hostInfo)
{
    qDebug() << "Received host lookup response:" << hostInfo.hostName();
    user.userHostName = hostInfo.hostName();
}



/*! This method handles any incoming messages, checks the command name and passes the control to
    the respective handler method. Each command has its own handler command, even it if just has
    to emit a simple signal. This makes it easier to implement command checks and other things
    later on.
*/
void QwsClientSocket::handleMessageReceived(const QwMessage &message)
{

     // Handshaking/Protocol
     if (message.commandName == "HELLO") {             handleMessageHELLO(message);
     } else if (message.commandName == "CLIENT") {     handleMessageCLIENT(message);
     } else if (message.commandName == "NICK") {       handleMessageNICK(message);
     } else if (message.commandName == "PASS") {       handleMessagePASS(message);
     } else if (message.commandName == "USER") {       handleMessageUSER(message);
     } else if (message.commandName == "PING") {       handleMessagePING(message);
     } else if (message.commandName == "STATUS") {     handleMessageSTATUS(message);
     } else if (message.commandName == "WHO") {        handleMessageWHO(message);
     } else if (message.commandName == "ICON") {       handleMessageICON(message);
     } else if (message.commandName == "BANNER") {     handleMessageBANNER(message);
     } else if (message.commandName == "INFO") {       handleMessageINFO(message);
     } else if (message.commandName == "PRIVILEGES") { handleMessagePRIVILEGES(message);

     // Communication
     } else if (message.commandName == "SAY") {        handleMessageSAY(message);
     } else if (message.commandName == "ME") {         handleMessageME(message);
     } else if (message.commandName == "MSG") {        handleMessageMSG(message);
     } else if (message.commandName == "BROADCAST") {  handleMessageBROADCAST(message);
     } else if (message.commandName == "TOPIC") {      handleMessageTOPIC(message);
     } else if (message.commandName == "PRIVCHAT") {   handleMessagePRIVCHAT(message);
     } else if (message.commandName == "INVITE") {     handleMessageINVITE(message);
     } else if (message.commandName == "JOIN") {       handleMessageJOIN(message);
     } else if (message.commandName == "DECLINE") {    handleMessageDECLINE(message);
     } else if (message.commandName == "LEAVE") {      handleMessageLEAVE(message);

     // News
     } else if (message.commandName == "NEWS") {       handleMessageNEWS(message);
     } else if (message.commandName == "POST") {       handleMessagePOST(message);
     } else if (message.commandName == "CLEARNEWS") {  handleMessageCLEARNEWS(message);

     // Administration
     } else if (message.commandName == "KICK") {       handleMessageKICK(message);
     } else if (message.commandName == "BAN") {        handleMessageBAN(message);
     } else if (message.commandName == "USERS") {      handleMessageUSERS(message);
     } else if (message.commandName == "GROUPS") {     handleMessageGROUPS(message);
     } else if (message.commandName == "READUSER") {   handleMessageREADUSER(message);
     } else if (message.commandName == "EDITUSER") {   handleMessageEDITUSER(message);
     } else if (message.commandName == "CREATEUSER") { handleMessageCREATEUSER(message);
     } else if (message.commandName == "DELETEUSER") { handleMessageDELETEUSER(message);
     } else if (message.commandName == "READGROUP") {  handleMessageREADGROUP(message);
     } else if (message.commandName == "CREATEGROUP"){ handleMessageCREATEGROUP(message);
     } else if (message.commandName == "EDITGROUP") {  handleMessageEDITGROUP(message);
     } else if (message.commandName == "DELETEGROUP"){ handleMessageDELETEGROUP(message);

     // Files
    } else if (message.commandName == "LIST") {        handleMessageLIST(message);
    } else if (message.commandName == "LISTRECURSIVE"){handleMessageLISTRECURSIVE(message);
    } else if (message.commandName == "STAT") {        handleMessageSTAT(message);
    } else if (message.commandName == "FOLDER") {      handleMessageFOLDER(message);
    } else if (message.commandName == "DELETE") {      handleMessageDELETE(message);
    } else if (message.commandName == "MOVE") {        handleMessageMOVE(message);
    } else if (message.commandName == "GET") {         handleMessageGET(message);
    } else if (message.commandName == "PUT") {         handleMessagePUT(message);
    } else if (message.commandName == "SEARCH") {      handleMessageSEARCH(message);
    } else if (message.commandName == "COMMENT") {     handleMessageCOMMENT(message);
    } else if (message.commandName == "TYPE") {        handleMessageTYPE(message);
    }
 }


 /*! HELLO command (Handshake request)
 */
 void QwsClientSocket::handleMessageHELLO(const QwMessage &message)
 {
     Q_UNUSED(message);
     qDebug() << this << "Received a HELLO handshake request.";
     m_sessionState = Qws::StateConnected;
     emit sessionStateChanged(m_sessionState);
     sendServerInfo();
 }


 /*! CLIENT command (Client software version and name)
 */
 void QwsClientSocket::handleMessageCLIENT(const QwMessage &message)
 {
     qDebug() << this << "Received client information.";
     user.pClientVersion = message.stringArg(0);
 }


 /*! NICK command (User nickname)
 */
 void QwsClientSocket::handleMessageNICK(const QwMessage &message)
 {
     resetIdleTimer();
     qDebug() << this << "Received user nickname.";
     user.userNickname = message.stringArg(0);
     if (m_sessionState == Qws::StateActive) {
         emit userStatusChanged();
     }

 }


 /*! PASS command (User password). During handshake/connecting this actually triggers the user
     authentication and causes the server to check the password and login, also load the specific
     account information from the database and tell all other clients that a user has logged in.
 */
 void QwsClientSocket::handleMessagePASS(const QwMessage &message)
 {
     if (m_sessionState == Qws::StateConnected && !user.userNickname.isEmpty()) {
         // We need a handshake first and a nickname. Send the client the session id of its own
         // session and proceed.
         user.pPassword = message.stringArg(0);

         // Check the login and password
         QSqlQuery query;
         query.prepare("SELECT id FROM qws_accounts WHERE acc_name=:_name AND acc_secret=:_secret");
         query.bindValue(":_name", user.name);
         query.bindValue(":_secret", user.pPassword);
         if (query.exec()) {
             // Query was OK. Check if there was a result.
             query.first();
             if (query.isValid() && user.loadFromDatabase()) {
                 user.pLoginTime = QDateTime::currentDateTime();
                 user.pIdleTime = QDateTime::currentDateTime();

                 // Increment counter (we don't care so much if this fails)
                 query.clear();
                 query.prepare("UPDATE qws_accounts SET acc_logincount=acc_logincount+1 WHERE acc_name=:_name");
                 query.bindValue(":_name", user.name);
                 query.exec();

                 // Login successful
                 QwMessage reply("201");
                 reply.appendArg(QString::number(user.pUserID));
                 sendMessage(reply);
                 m_sessionState = Qws::StateActive;
                 emit sessionStateChanged(m_sessionState);
                 emit requestedRoomTopic(1);
                 return;
             }
         }
     }

     // If in doubt, fail
     sendError(Qw::ErrorLoginFailed);
     disconnectClient();
     return;
 }


 /*! USER command (User login name)
 */
 void QwsClientSocket::handleMessageUSER(const QwMessage &message)
 {
     user.name = message.stringArg(0);
     if (user.name.isEmpty()) {
         // Guest fallback
         user.name = "guest";
     }
     qDebug() << this << "Received user login name:" << user.name;
 }


 /*! PING command (Keep-alive request)
 */
 void QwsClientSocket::handleMessagePING(const QwMessage &message)
 {
     Q_UNUSED(message);
     sendMessage(QwMessage("202 Pong"));
 }


 /*! STATUS command (Keep-alive request)
 */
 void QwsClientSocket::handleMessageSTATUS(const QwMessage &message)
 {
     resetIdleTimer();
     qDebug() << this << "Received user status:" << message.stringArg(0);
     user.userStatus = message.stringArg(0);
     if (m_sessionState == Qws::StateActive) {
         emit userStatusChanged();
     }
 }


 /*! WHO command (User list request)
 */
 void QwsClientSocket::handleMessageWHO(const QwMessage &message)
 {
     int roomId = message.stringArg(0).toInt();
     qDebug() << this << "Requested user list for room #" << roomId;
     emit requestedUserlist(roomId);
 }


/*! ICON command (Set user icon)
*/
void QwsClientSocket::handleMessageICON(const QwMessage &message)
{
    resetIdleTimer();
    qDebug() << this << "Received icon and image";
    if (user.pIcon != message.stringArg(0).toInt()) {
        user.pIcon = message.stringArg(0).toInt();
        if (m_sessionState == Qws::StateActive) {
            emit userStatusChanged();
        }
    }

    // Check if the user image changed
    if (message.arguments.value(1) != user.pImage) {
        user.pImage = message.arguments.value(1).toAscii();
        QwMessage reply("340");
        reply.appendArg(QString::number(user.pUserID));
        reply.appendArg(message.arguments.value(1));
        if (m_sessionState == Qws::StateActive) {
            emit broadcastedMessage(reply, 1, true);
        }
    }

}


/*! BANNER command (banner data request)
*/
void QwsClientSocket::handleMessageBANNER(const QwMessage &message)
{
    Q_UNUSED(message);
    QSqlQuery query("SELECT conf_value FROM qws_config WHERE conf_key='server/banner'");
    query.first();
    QwMessage reply("203");
    if (query.isValid()) {
        QByteArray bannerData = query.value(0).toByteArray();
        reply.appendArg(bannerData.toBase64());
    } else {
        qDebug() << this << "Unable to load banner from database:" << query.lastError().text();
        reply.appendArg(QString());
    }
    sendMessage(reply);
}


/*! INFO command (Get user info)
*/
void QwsClientSocket::handleMessageINFO(const QwMessage &message)
{
    if (!user.privileges().testFlag(Qws::PrivilegeGetUserInfo)) { sendError(Qw::ErrorPermissionDenied); return; }
    emit receivedMessageINFO(message.stringArg(0).toInt());
}


/*! PRIVILEGES command (get privileges mask)
*/
void QwsClientSocket::handleMessagePRIVILEGES(const QwMessage &message)
{
    Q_UNUSED(message);
    QwMessage reply("602");
    user.appendPrivilegeFlagsForREADUSER(reply);
    sendMessage(reply);
}


/* === Communication == */


/*! SAY command (Relay chat to room)
*/
void QwsClientSocket::handleMessageSAY(const QwMessage &message)
{
    resetIdleTimer();
    emit requestedChatRelay(message.stringArg(0).toInt(), message.stringArg(1), false);
}


/*! ME command (Relay chat to room as emote)
*/
 void QwsClientSocket::handleMessageME(const QwMessage &message)
{
     resetIdleTimer();
     emit requestedChatRelay(message.stringArg(0).toInt(), message.stringArg(1), true);
}


/*! MSG command (Relay private message to other user)
*/
void QwsClientSocket::handleMessageMSG(const QwMessage &message)
{
    resetIdleTimer();
    emit requestedMessageRelay(message.stringArg(0).toInt(), message.stringArg(1));
}


/*! BROADCAST command (Relay broadcast message to other users)
*/
void QwsClientSocket::handleMessageBROADCAST(const QwMessage &message)
{
    resetIdleTimer();
    if (!user.privileges().testFlag(Qws::PrivilegeSendBroadcast)) {
        sendError(Qw::ErrorPermissionDenied); return; }
    QwMessage reply("309");
    reply.appendArg(QString::number(user.pUserID));
    reply.appendArg(message.stringArg(0));
    emit broadcastedMessage(reply, 1, true);
}


/*! TOPIC command (Change topic of chat)
*/
void QwsClientSocket::handleMessageTOPIC(const QwMessage &message)
{
    resetIdleTimer();
    if (!user.privileges().testFlag(Qws::PrivilegeChangeChatTopic)) {
        sendError(Qw::ErrorPermissionDenied); return; }
    emit requestedRoomTopicChange(message.stringArg(0).toInt(), message.stringArg(1));
}


/*! PRIVCHAT command (Create a new chat room)
*/
void QwsClientSocket::handleMessagePRIVCHAT(const QwMessage &message)
{
    Q_UNUSED(message);
    qDebug() << this << "Requested new chat room";
    resetIdleTimer();
    emit requestedNewRoom();
}


/*! INVITE command (Invite a user to a room).
    The INVITE command creates a new room on the server
    and automatically adds the inviting user to the room. After that it returns the ID of the
    new room and expects the user to send an INVITE command for another user(s).
*/
void QwsClientSocket::handleMessageINVITE(const QwMessage &message)
{
    qDebug() << this << "Invited user to chat room" << message.stringArg(0);
    resetIdleTimer();
    emit requestedUserInviteToRoom(message.stringArg(0).toInt(),
                                   message.stringArg(1).toInt());
}


/*! JOIN command (Join a previously invited room)
*/
void QwsClientSocket::handleMessageJOIN(const QwMessage &message)
{
    qDebug() << this << "Joining chat room" << message.stringArg(0);
    resetIdleTimer();
    emit receivedMessageJOIN(message.stringArg(0).toInt());
}


/*! The user has declined an invitation to a private chat (0).
*/
void QwsClientSocket::handleMessageDECLINE(const QwMessage &message)
{
    qDebug() << this << "Declined room invitation" << message.stringArg(0);
    resetIdleTimer();
    emit receivedMessageDECLINE(message.stringArg(0).toInt());
}


/*! The user has left a chat room.
*/
void QwsClientSocket::handleMessageLEAVE(const QwMessage &message)
{
    qDebug() << this << "Left room" << message.stringArg(0);
    resetIdleTimer();
    emit receivedMessageLEAVE(message.stringArg(0).toInt());
}


/* === NEWS === */

 /*! NEWS command (News list request)
 */
 void QwsClientSocket::handleMessageNEWS(const QwMessage &message)
 {
     Q_UNUSED(message);
     qDebug() << this << "Requested news";
     QSqlQuery query;
     query.prepare("SELECT id, news_username, news_date, news_text FROM qws_news ORDER BY id DESC");
     if (query.exec()) {
         while (query.next()) {
             QwMessage reply("320");
             reply.appendArg(query.value(1).toString());
             reply.appendArg(query.value(2).toString());
             reply.appendArg(query.value(3).toString());
             sendMessage(reply);
         }
     } else {
         qDebug() << this << "Could not send news:" << query.lastError().text();
         sendError(Qw::ErrorCommandFailed);
         return;
     }
     sendMessage(QwMessage("321 News Done"));
 }


 /*! POST command (News list request)
 */
 void QwsClientSocket::handleMessagePOST(const QwMessage &message)
 {
     resetIdleTimer();
     if (!user.privileges().testFlag(Qws::PrivilegePostNews)) {
         sendError(Qw::ErrorPermissionDenied); return; }
     qDebug() << this << "Posted news";
     QSqlQuery query;
     query.prepare("INSERT INTO qws_news (news_username, news_date, news_text) "
                   "VALUES (:_login, :_date, :_text)");
     query.bindValue(":_login", QString("%1 [%2]").arg(user.userNickname).arg(user.name));
     query.bindValue(":_date", QDateTime::currentDateTime().toUTC().toString(Qt::ISODate));
     query.bindValue(":_text", message.stringArg(0));

     if (query.exec()) {
         // Send all clients a notification.
         QwMessage reply("322");
         reply.appendArg(query.boundValue(":_login").toString());
         reply.appendArg(query.boundValue(":_date").toString()+"+00:00");
         reply.appendArg(query.boundValue(":_text").toString());
         emit broadcastedMessage(reply, 1, true);
     } else {
         qDebug() << this << "Could not post news:" << query.lastError().text();
         sendError(Qw::ErrorCommandFailed);
         return;
     }

 }


 /*! CLEARNEWS command (clear news list request)
 */
 void QwsClientSocket::handleMessageCLEARNEWS(const QwMessage &message)
 {
     Q_UNUSED(message);
     resetIdleTimer();
     if (!user.privileges().testFlag(Qws::PrivilegeClearNews)) {
         sendError(Qw::ErrorPermissionDenied); return; }

     QSqlQuery query;
     query.prepare("DELETE FROM qws_news");
     if (!query.exec()) {
         qDebug() << this << "Could not clear news:" << query.lastError().text();
         sendError(Qw::ErrorCommandFailed);
     }

 }



/* === ADMINISTRATION === */

/*! BAN (ban user from server)
*/
void QwsClientSocket::handleMessageBAN(const QwMessage &message)
{
    resetIdleTimer();
    if (!user.privileges().testFlag(Qws::PrivilegeBanUsers)) {
        sendError(Qw::ErrorPermissionDenied); return; }
    emit receivedMessageBAN_KICK(message.stringArg(0).toInt(),
                                 message.stringArg(1), true);
}


/*! KICK (user from server) - same as ban without the ban
*/
void QwsClientSocket::handleMessageKICK(const QwMessage &message)
{
    resetIdleTimer();
    if (!user.privileges().testFlag(Qws::PrivilegeKickUsers)) {
        sendError(Qw::ErrorPermissionDenied); return; }
    emit receivedMessageBAN_KICK(message.stringArg(0).toInt(),
                                 message.stringArg(1), false);
}


/*! USERS (list user accounts)
*/
void QwsClientSocket::handleMessageUSERS(const QwMessage &message)
{
    resetIdleTimer();
    if (!user.privileges().testFlag(Qws::PrivilegeEditAccounts)) {
        sendError(Qw::ErrorPermissionDenied); return; }

    Q_UNUSED(message);
    qDebug() << this << "Listing user accounts";

    QSqlQuery query;
    query.prepare("SELECT acc_name FROM qws_accounts ORDER BY acc_name");
    if (!query.exec()) {
        qDebug() << this << "Unable to list user accounts:" << query.lastError().text();
        sendError(Qw::ErrorCommandFailed);
        return;
    } else {
        query.first();
        while (query.isValid()) {
            QwMessage reply("610");
            reply.appendArg(query.value(0).toString());
            sendMessage(reply);
            query.next();
        }
        // Send end of list
        sendMessage(QwMessage("611"));
    }

}


/*! GROUPS (list user accounts)
*/
void QwsClientSocket::handleMessageGROUPS(const QwMessage &message)
{
    resetIdleTimer();
    if (!user.privileges().testFlag(Qws::PrivilegeEditAccounts)) {
        sendError(Qw::ErrorPermissionDenied); return; }

    Q_UNUSED(message);
    qDebug() << this << "Listing groups";

    QSqlQuery query;
    query.prepare("SELECT group_name FROM qws_groups ORDER BY group_name");
    if (!query.exec()) {
        qDebug() << this << "Unable to list user groups:" << query.lastError().text();
        sendError(Qw::ErrorCommandFailed);
        return;
    } else {
        query.first();
        while (query.isValid()) {
            QwMessage reply("620");
            reply.appendArg(query.value(0).toString());
            sendMessage(reply);
            query.next();
        }
        // Send end of list
        sendMessage(QwMessage("621"));
    }

}


/*! READUSER (requested details of user account)
*/
void QwsClientSocket::handleMessageREADUSER(const QwMessage &message)
{
    resetIdleTimer();
    if (!user.privileges().testFlag(Qws::PrivilegeEditAccounts)) {
        sendError(Qw::ErrorPermissionDenied); return; }

    qDebug() << this << "Reading user" << message.stringArg(0);
    QwsUser targetAccount;
    targetAccount.name = message.stringArg(0);
    if (targetAccount.loadFromDatabase()) {
        QwMessage reply("600");
        reply.appendArg(targetAccount.name);
        reply.appendArg(targetAccount.pPassword);
        reply.appendArg(targetAccount.pGroupName);
        targetAccount.appendPrivilegeFlagsForREADUSER(reply);
        sendMessage(reply);
    } else {
        sendError(Qw::ErrorAccountNotFound);
    }
}


/*! EDITUSER (write changes to user account)
*/
void QwsClientSocket::handleMessageEDITUSER(const QwMessage &message)
{
    resetIdleTimer();
    if (!user.privileges().testFlag(Qws::PrivilegeEditAccounts)) {
        sendError(Qw::ErrorPermissionDenied); return; }

    qDebug() << this << "Editing user" << message.stringArg(0);
    QwsUser targetAccount;
    targetAccount.name = message.stringArg(0);
    if (!targetAccount.loadFromDatabase()) {
        // User does not exist (or error)
        sendError(Qw::ErrorAccountNotFound);
    } else {
        targetAccount.setPrivilegesFromEDITUSER(message, 3);
        targetAccount.pPassword = message.stringArg(1);
        targetAccount.pGroupName = message.stringArg(2);
        if (!targetAccount.writeToDatabase()) {
            sendError(Qw::ErrorAccountNotFound);
        } else {
            emit modifiedUserAccount(targetAccount.name);
        }

    }
}


/*! CREATEUSER (create a new account)
*/
void QwsClientSocket::handleMessageCREATEUSER(const QwMessage &message)
{
    resetIdleTimer();
    if (!user.privileges().testFlag(Qws::PrivilegeCreateAccounts)) {
        sendError(Qw::ErrorPermissionDenied); return; }

    qDebug() << this << "Creating user" << message.stringArg(0);
    QwsUser targetUser;
    targetUser.name = message.stringArg(0);
    if (targetUser.loadFromDatabase()) {
        // User exists already!
        sendError(Qw::ErrorAccountExists);
    } else {
        // Create account and update it
        QSqlQuery query;
        query.prepare("INSERT INTO qws_accounts (acc_name) VALUES (:_name)");
        query.bindValue(":_name", message.stringArg(0));
        if (!query.exec()) {
            qDebug() << this << "Unable to create (insert) user account:" << query.lastError().text();
            sendError(Qw::ErrorCommandFailed);
            return;
        }
        handleMessageEDITUSER(message);
    }
}


/*! DELETEUSER (delete user account)
*/
void QwsClientSocket::handleMessageDELETEUSER(const QwMessage &message)
{
    resetIdleTimer();
    if (!user.privileges().testFlag(Qws::PrivilegeDeleteAccounts)) {
        sendError(Qw::ErrorPermissionDenied); return; }

    qDebug() << this << "Editing user" << message.stringArg(0);
    QwsUser targetAccount;
    targetAccount.name = message.stringArg(0);
    if (!targetAccount.deleteFromDatabase()) {
        sendError(Qw::ErrorAccountNotFound);
    }
}


/*! READGROUP (requested details of user group)
*/
void QwsClientSocket::handleMessageREADGROUP(const QwMessage &message)
{
    resetIdleTimer();
    if (!user.privileges().testFlag(Qws::PrivilegeEditAccounts)) {
        sendError(Qw::ErrorPermissionDenied); return; }

    qDebug() << this << "Reading group" << message.stringArg(0);
    QwsUser targetGroup;
    targetGroup.name = message.stringArg(0);
    targetGroup.userType = Qws::UserTypeGroup;
    if (!targetGroup.loadFromDatabase()) {
        sendError(Qw::ErrorAccountNotFound);
    } else {
         QwMessage reply("601");
         reply.appendArg(targetGroup.name);
         targetGroup.appendPrivilegeFlagsForREADUSER(reply);
         sendMessage(reply);
    }
}


/*! CREATEGROUP (create a new group)
*/
void QwsClientSocket::handleMessageCREATEGROUP(const QwMessage &message)
{
    resetIdleTimer();
    if (!user.privileges().testFlag(Qws::PrivilegeCreateAccounts)) {
        sendError(Qw::ErrorPermissionDenied); return; }

    qDebug() << this << "Creating group" << message.stringArg(0);
    QwsUser targetGroup;
    targetGroup.userType = Qws::UserTypeGroup;
    targetGroup.name = message.stringArg(0);
    if (targetGroup.loadFromDatabase()) {
        // User exists already!
        sendError(Qw::ErrorAccountExists);
    } else {
        // Create account and update it
        QSqlQuery query;
        query.prepare("INSERT INTO qws_groups (group_name) VALUES (:_name)");
        query.bindValue(":_name", targetGroup.name);
        if (!query.exec()) {
            qDebug() << this << "Unable to create (insert) user account:" << query.lastError().text();
            sendError(Qw::ErrorCommandFailed);
            return;
        }
        handleMessageEDITGROUP(message);
    }
}


/*! EDITGROUP (write changes to user group)
*/
void QwsClientSocket::handleMessageEDITGROUP(const QwMessage &message)
{
    resetIdleTimer();
    if (!user.privileges().testFlag(Qws::PrivilegeEditAccounts)) {
        sendError(Qw::ErrorPermissionDenied); return; }
    qDebug() << this << "Editing group" << message.stringArg(0);
    QwsUser targetGroup;
    targetGroup.name = message.stringArg(0);
    targetGroup.userType = Qws::UserTypeGroup;
    if (!targetGroup.loadFromDatabase()) {
        sendError(Qw::ErrorAccountNotFound);
    } else {
        targetGroup.setPrivilegesFromEDITUSER(message, 1);
        if (!targetGroup.writeToDatabase()) {
            sendError(Qw::ErrorAccountNotFound);
        } else {
            emit modifiedUserGroup(targetGroup.name);
        }
    }
}


/*! DELETEGROUP (delete user group)
*/
void QwsClientSocket::handleMessageDELETEGROUP(const QwMessage &message)
{
    resetIdleTimer();
    if (!user.privileges().testFlag(Qws::PrivilegeDeleteAccounts)) {
        sendError(Qw::ErrorPermissionDenied); return; }
    qDebug() << this << "Deleting group" << message.stringArg(0);
    QwsUser targetGroup;
    targetGroup.name = message.stringArg(0);
    targetGroup.userType = Qws::UserTypeGroup;
    if (!targetGroup.deleteFromDatabase()) {
        sendError(Qw::ErrorAccountNotFound);
    }
}


/* === FILES === */

/*! LIST (list files)
*/
void QwsClientSocket::handleMessageLIST(const QwMessage &message)
{
    resetIdleTimer();

    QwsFile targetDirectory;
    targetDirectory.localFilesRoot = filesRootPath;
    targetDirectory.setRemotePath(message.stringArg(0));

    // Check if target is valid
    if (!targetDirectory.updateLocalPath(false)) {
        sendError(Qw::ErrorFileOrDirectoryNotFound);
        return;
    }

    // Check if the target is a directory
    if (!targetDirectory.type > Qw::FileTypeRegular) {
        sendError(Qw::ErrorFileOrDirectoryNotFound);
        return;
    }


    qDebug() << this << "Trying to LIST" << targetDirectory.localPath();
    QDirIterator it(targetDirectory.localPath(),
                    QDir::AllEntries | QDir::NoDotAndDotDot,
                    QDirIterator::FollowSymlinks);
    while (it.hasNext()) {
        it.next();

        QwsFile itemFile;
        itemFile.localFilesRoot = targetDirectory.localPath();
        itemFile.setRemotePath(it.fileName());

        if (itemFile.updateLocalPath(true)) {
            QwMessage replyItem("410");
            replyItem.appendArg(QDir::cleanPath(targetDirectory.remotePath() + "/" + itemFile.remotePath()));
            replyItem.appendArg(QString::number(itemFile.type));
            replyItem.appendArg(QString::number(itemFile.size()));
            replyItem.appendArg(itemFile.created.toUTC().toString(Qt::ISODate)+"+00:00");
            replyItem.appendArg(itemFile.modified.toUTC().toString(Qt::ISODate)+"+00:00");
            sendMessage(replyItem);
        }
    }

    // End of list
    QwMessage reply("411");
    reply.appendArg(targetDirectory.remotePath());


#ifdef Q_OS_WIN32
    ULARGE_INTEGER totalFreeSpace;
    GetDiskFreeSpaceEx(NULL, &totalFreeSpace, NULL, NULL);
    reply.appendArg(QString::number(totalFreeSpace.QuadPart));
#endif

#ifdef Q_OS_UNIX
    struct statvfs diskInfo;
    if (statvfs("/", &diskInfo) == 0) {
        reply.appendArg(QString::number((qlonglong)diskInfo.f_bfree * (qlonglong)diskInfo.f_frsize));
    } else {
        reply.appendArg(0);
    }    
#endif

    sendMessage(reply);
}


/*! LISTRECURSIVE (list files recursively)
*/
void QwsClientSocket::handleMessageLISTRECURSIVE(const QwMessage &message)
{
    resetIdleTimer();
    QwsFile targetDirectory;
    targetDirectory.localFilesRoot = filesRootPath;
    targetDirectory.setRemotePath(message.stringArg(0));

    // Prevent root indexing
    if (targetDirectory.remotePath() == "/") {
        sendError(Qw::ErrorFileOrDirectoryNotFound);
        return;
    }

    // Check if target is valid
    if (!targetDirectory.updateLocalPath(false)) {
        sendError(Qw::ErrorFileOrDirectoryNotFound);
        return;
    }

    // Check if the target is a directory
    if (!targetDirectory.type > Qw::FileTypeRegular) {
        sendError(Qw::ErrorFileOrDirectoryNotFound);
        return;
    }

    QDirIterator it(targetDirectory.localPath(),
                    QDir::AllEntries | QDir::NoDotAndDotDot,
                    QDirIterator::FollowSymlinks | QDirIterator::Subdirectories);
    while (it.hasNext()) {
        it.next();

        QwsFile itemFile;
        itemFile.localFilesRoot = targetDirectory.localPath();
        itemFile.setRemotePath(it.fileName());


        if (itemFile.updateLocalPath(true)) {
            QwMessage replyItem("410");
            replyItem.appendArg(QDir::cleanPath(targetDirectory.remotePath() + "/" + itemFile.remotePath()));
            replyItem.appendArg(QString::number(itemFile.type));
            replyItem.appendArg(QString::number(itemFile.size()));
            replyItem.appendArg(itemFile.created.toUTC().toString(Qt::ISODate)+"+00:00");
            replyItem.appendArg(itemFile.modified.toUTC().toString(Qt::ISODate)+"+00:00");
            sendMessage(replyItem);
        }
    }


    // End of list
    QwMessage reply("411");
    reply.appendArg(targetDirectory.remotePath());

#ifdef Q_OS_WIN32
    ULARGE_INTEGER totalFreeSpace;
    GetDiskFreeSpaceEx(NULL, &totalFreeSpace, NULL, NULL);
    reply.appendArg(QString::number(totalFreeSpace.QuadPart));
#endif

#ifdef Q_OS_UNIX
    struct statvfs diskInfo;
    if (statvfs("/", &diskInfo) == 0) {
        reply.appendArg(QString::number((qlonglong)diskInfo.f_bfree * (qlonglong)diskInfo.f_frsize));
    } else {
        reply.appendArg(0);
    }
#endif

    sendMessage(reply);
}


/*! STAT (return file/dir information)
*/
void QwsClientSocket::handleMessageSTAT(const QwMessage &message)
{
    QwsFile targetFile;
    targetFile.localFilesRoot = filesRootPath;
    targetFile.setRemotePath(message.stringArg(0));

    if (!targetFile.updateLocalPath()) {
        sendError(Qw::ErrorFileOrDirectoryNotFound);
        return;
    }

    // Try to load additional information from the database
    targetFile.loadMetaInformation();

    // Send file info reply
    QwMessage reply("402");
    reply.appendArg(targetFile.remotePath());
    reply.appendArg(QString::number(targetFile.type));
    reply.appendArg(QString::number(targetFile.size()));
    reply.appendArg(targetFile.created.toUTC().toString(Qt::ISODate)+"+00:00");
    reply.appendArg(targetFile.modified.toUTC().toString(Qt::ISODate)+"+00:00");
    reply.appendArg(targetFile.checksum);
    reply.appendArg(targetFile.comment);
    sendMessage(reply);
}


/*! FOLDER (create a directory/folder)
*/
void QwsClientSocket::handleMessageFOLDER(const QwMessage &message)
{
    resetIdleTimer();
    if (!user.privileges().testFlag(Qws::PrivilegeCreateFolders)) {
        sendError(Qw::ErrorPermissionDenied); return; }

    QString folderBasePath = message.stringArg(0).section('/', 0, -2, QString::SectionSkipEmpty);
    QString folderBaseName = message.stringArg(0).section('/', -1, -1, QString::SectionSkipEmpty);

    QwsFile targetBaseFolder;
    targetBaseFolder.setRemotePath(folderBasePath);
    targetBaseFolder.localFilesRoot = filesRootPath;

    if (!targetBaseFolder.updateLocalPath(true)) {
        sendError(Qw::ErrorFileOrDirectoryNotFound);
        return;
    }

    QDir localBaseDir(targetBaseFolder.localPath());
    if (!localBaseDir.mkdir(folderBaseName)) {
        sendError(Qw::ErrorFileOrDirectoryExists);
        return;
    }

    qDebug() << "Creating folder at" << folderBasePath << "with name" << folderBaseName;

}


/*! DELETE (delete a file/folder)
*/
void QwsClientSocket::handleMessageDELETE(const QwMessage &message)
{
    resetIdleTimer();
    if (!user.privileges().testFlag(Qws::PrivilegeDeleteFiles)) {
        sendError(Qw::ErrorPermissionDenied); return; }

    QString folderBasePath = message.stringArg(0).section('/', 0, -2, QString::SectionSkipEmpty);
    QString folderBaseName = message.stringArg(0).section('/', -1, -1, QString::SectionSkipEmpty);

    qDebug() << this << "Path=" << folderBasePath << "Item=" << folderBaseName;

    QwsFile targetBase;
    targetBase.setRemotePath(message.stringArg(0));
    targetBase.localFilesRoot = filesRootPath;
    if (!targetBase.updateLocalPath(true)) {
        sendError(Qw::ErrorFileOrDirectoryNotFound);
        return;
    }

    if (targetBase.localPath() == filesRootPath) {
        sendError(Qw::ErrorFileOrDirectoryNotFound);
        return;
    }

    QFileInfo targetInfo(targetBase.localPath());
    QDir targetFolder(targetBase.localPath());
    if (targetInfo.isDir()) {
        // Directory
        qDebug() << "Deleting a directory.";
        deleteDirRecursive(targetInfo.absoluteFilePath());

    } else {
        // File
        QDir parentDir(targetBase.localPath());
        if (!parentDir.cdUp()) {
            sendError(Qw::ErrorFileOrDirectoryNotFound);
            return;
        }
        if (!parentDir.remove(folderBaseName)) {
            qDebug() << this << "Unable to delete file" << targetBase.localPath();
            sendError(Qw::ErrorFileOrDirectoryNotFound);
            return;
        }
    }

}

/*! Recursively delete a directory and its items.
*/
void QwsClientSocket::deleteDirRecursive(QString pathToDir)
{    
    QDirIterator it(pathToDir,
                    QDir::NoDotAndDotDot | QDir::AllEntries,
                    QDirIterator::Subdirectories);
    while (it.hasNext()) {
        QString itemPath = it.next();
        QFileInfo itemInfo = it.fileInfo();
        if (itemInfo.isHidden()) { continue; }
        if (itemInfo.isDir()) {
            qDebug() << "Deleting recurse dir:" << itemInfo.absoluteFilePath();
            deleteDirRecursive(itemInfo.absoluteFilePath());
        } else if (itemInfo.isFile()) {
            qDebug() << "Deleting file:" << itemInfo.absoluteFilePath();
            QFile(itemInfo.absoluteFilePath()).remove();
        }
    }

    QDir().rmdir(pathToDir);
}


/*! MOVE - move a file or directory to another directory
*/
void QwsClientSocket::handleMessageMOVE(const QwMessage &message)
{
    resetIdleTimer();

    // Update the source file to see if it exists
    QwsFile sourceFile;
    sourceFile.localFilesRoot = filesRootPath;
    sourceFile.setRemotePath(message.stringArg(0));
    if (!sourceFile.updateLocalPath()) {
        // Check if the file to move exists
        sendError(Qw::ErrorFileOrDirectoryNotFound);
        return;
    }
    qDebug() << this << "Renaming source:" << sourceFile.localPath();

    // Update the destination (dir or file)
    QwsFile destinationFile;
    destinationFile.localFilesRoot = filesRootPath;
    destinationFile.setRemotePath(message.stringArg(1));

    // Check if we are within the root
    if (!destinationFile.isWithinLocalRoot()) {
        qDebug() << this << "Preventing move outside of jail:" << destinationFile.remotePath();
        sendError(Qw::ErrorFileOrDirectoryNotFound);
        return;
    }

    // Check if the target already exists
    if (destinationFile.updateLocalPath()) {
        sendError(Qw::ErrorFileOrDirectoryNotFound);
        return;
    }
    qDebug() << this << "Renaming target:" << destinationFile.localPath();
    QFile sourceItem(sourceFile.localPath());
    if (!sourceItem.rename(destinationFile.localPath())) {
        sendError(Qw::ErrorFileOrDirectoryNotFound);
        return;
    }
}


/*! GET - Download file from server
*/
void QwsClientSocket::handleMessageGET(const QwMessage &message)
{
    QwsFile targetFile;
    targetFile.localFilesRoot = filesRootPath;
    targetFile.setRemotePath(message.stringArg(0));
    targetFile.offset = message.stringArg(1).toLongLong();
    if (!targetFile.updateLocalPath()) {
        sendError(Qw::ErrorFileOrDirectoryNotFound);
        return;
    }
    emit receivedMessageGET(targetFile);
}


/*! PUT - Upload a file to the server
*/
void QwsClientSocket::handleMessagePUT(const QwMessage &message)
{
    QString targetFileName = message.stringArg(0);
    qint64 targetFileSize = message.stringArg(1).toLongLong();
    QByteArray targetFileChecksum = message.stringArg(2).toAscii();
    qDebug() << this << "Received PUT request:" << targetFileName;

    QwsFile localFile;
    localFile.localFilesRoot = filesRootPath;
    localFile.setRemotePath(targetFileName);

    // Check for jail escape
    if (!localFile.isWithinLocalRoot()) {
        sendError(Qw::ErrorFileOrDirectoryNotFound);
        return;
    }

    QFileInfo targetFileInfo(localFile.localPath());
    if (targetFileInfo.exists()) {
        // The file exists - abort here.
        sendError(Qw::ErrorFileOrDirectoryExists);

    } else {
        // File does not exist yet - check for a file with .WiredTransfer suffix
        localFile.setRemotePath(localFile.remotePath().append(".WiredTransfer"));
        qDebug() << this << "Checking for local partial file:" << localFile.remotePath();

        // Check for jail escape
        if (!localFile.isWithinLocalRoot()) {
            sendError(Qw::ErrorFileOrDirectoryNotFound);
            return;
        }

        targetFileInfo = QFileInfo(localFile.localPath());
        if (targetFileInfo.exists()) {
            // Partial file exists - check if the checksum is the same.
            localFile.updateLocalChecksum();

            if (localFile.checksum != targetFileChecksum) {
                qDebug() << "Checksum mismatch - local =" << localFile.checksum << "expected = " << targetFileChecksum;
                sendError(Qw::ErrorChecksumMismatch);
                return;
            }

            // Checksums are the same - compare the file size to see what needs to be done.
            if (targetFileInfo.size() == targetFileSize) {
                qDebug() << "File exists!";
                sendError(Qw::ErrorFileOrDirectoryExists);
                return;
            }

            // At this point we have a partial file with the same checksum and a file size
            // smaller than the target file. We can resume.
            qDebug() << this << "Resuming file:" << targetFileInfo.absoluteFilePath()
                    << "already got" << targetFileInfo.size() << "of" << targetFileSize;

            localFile.offset = targetFileInfo.size();
            localFile.setSize(targetFileSize);
            localFile.checksum = targetFileChecksum;
            emit receivedMessagePUT(localFile);

        } else {
            // No partial file exists. Check if the parent directory exists and transfer the file.
            if (!targetFileInfo.absoluteDir().exists()) {
                qDebug() << "Parent does not exist!";
                sendError(Qw::ErrorFileOrDirectoryNotFound);
                return;
            }
            qDebug() << this << "Receiving new file:" << targetFileInfo.absoluteFilePath();
            localFile.offset = 0;
            localFile.checksum = targetFileChecksum;
            localFile.setSize(targetFileSize);
            emit receivedMessagePUT(localFile);
        }
    }
}

/*! SEARCH - Search for files on the server.
*/
void QwsClientSocket::handleMessageSEARCH(const QwMessage &message)
{
    QString searchTerm = message.stringArg(0);
    searchTerm.remove("%");
    if (searchTerm.isEmpty()) {
        sendMessage(QwMessage("421 Done"));
    }

    QSqlQuery query("SELECT file_dir_path, file_name, file_size "
                    "FROM qws_files_index "
                    "WHERE file_name LIKE :_term LIMIT 250");
    query.bindValue(":_term", QString("%%1%").arg(searchTerm));
    if (!query.exec()) {
        qDebug() << this << "Unable to query database:" << query.lastError().text();
        sendMessage(QwMessage("421 Done"));
        return;
    }

    while (query.next()) {
        QwMessage response("420");
        response.appendArg(QString("%1/%2")
                           .arg(query.value(0).toString())
                           .arg(query.value(1).toString()));
        response.appendArg(QString::number(Qw::FileTypeRegular));
        response.appendArg(query.value(2).toString());
        response.appendArg(QDateTime::currentDateTime().toUTC().toString(Qt::ISODate)+"+00:00");
        response.appendArg(QDateTime::currentDateTime().toUTC().toString(Qt::ISODate)+"+00:00");
        sendMessage(response);
    }

    // End of results
    sendMessage(QwMessage("421 Done"));
}


/*! COMMENT - Set a comment on a file or directory
*/
void QwsClientSocket::handleMessageCOMMENT(const QwMessage &message)
{
    resetIdleTimer();
    if (!user.privileges().testFlag(Qws::PrivilegeAlterFiles)) {
        sendError(Qw::ErrorPermissionDenied); return; }

    QwsFile localFile;
    localFile.localFilesRoot = this->filesRootPath;
    localFile.setRemotePath(message.stringArg(0));
    if (!localFile.updateLocalPath(true)) {
        sendError(Qw::ErrorFileOrDirectoryNotFound);
        return;
    }

    localFile.comment = message.stringArg(1);
    if (!localFile.saveMetaInformation()) {
        sendError(Qw::ErrorFileOrDirectoryNotFound);
        return;
    }
}


/*! TYPE - Set the type of a file (more commonly of a directory)
*/
void QwsClientSocket::handleMessageTYPE(const QwMessage &message)
{
    resetIdleTimer();
    if (!user.privileges().testFlag(Qws::PrivilegeAlterFiles)) {
        sendError(Qw::ErrorPermissionDenied); return; }

    Qw::FileType targetType = (Qw::FileType)message.stringArg(1).toInt();

    QwsFile localFile;
    localFile.localFilesRoot = this->filesRootPath;
    localFile.setRemotePath(message.stringArg(0));
    if (!localFile.updateLocalPath(true)) {
        sendError(Qw::ErrorFileOrDirectoryNotFound);
        return;
    }

    QFileInfo targetInfo(localFile.localPath());
    if (!targetInfo.isDir()) {
        sendError(Qw::ErrorFileOrDirectoryNotFound);
        return;
    }

    // Check if type is valid
    if (targetType < 0 || targetType > 3) {
        sendError(Qw::ErrorSyntaxError);
        return;
    }

    localFile.loadMetaInformation();
    localFile.type = targetType;
    if (!localFile.saveMetaInformation()) {
        sendError(Qw::ErrorFileOrDirectoryNotFound);
        return;
    }
}



/*! Send information about the server. Happens during handshake or asynchronously.
*/
void QwsClientSocket::sendServerInfo()
{
     QwMessage response("200");
     Q_ASSERT(serverInfo != NULL);
     response.appendArg(serverInfo->serverVersion); // app-version
     response.appendArg(serverInfo->protocolVersion); // proto-version
     response.appendArg(serverInfo->name); // name
     response.appendArg(serverInfo->description); // description
     response.appendArg(serverInfo->startTime.toUTC().toString(Qt::ISODate)+"+00:00"); // start time
     response.appendArg(QString::number(serverInfo->filesCount)); // file count
     response.appendArg(QString::number(serverInfo->filesSize)); // total size
     sendMessage(response);
}


/*! The client needs to be removed from the server. Delete the socket and this object.
*/
void QwsClientSocket::disconnectClient()
{
    qDebug() << this << "Called disconnectClient()";
    pSocket->disconnectFromHost();
    emit connectionLost();
}


/*! Send an error message to the client if something goes wrong.
*/
void QwsClientSocket::sendError(const Qw::ProtocolError error)
{
    QByteArray errorString;
    switch (error) {
        case Qw::ErrorCommandFailed: errorString = "500 Command Failed"; break;
        case Qw::ErrorCommandNotRecognized: errorString = "501 Command Not Recognized"; break;
        case Qw::ErrorCommandNotImplemented: errorString = "502 Command Not Implemented"; break;
        case Qw::ErrorSyntaxError: errorString = "503 Syntax Error"; break;
        case Qw::ErrorLoginFailed: errorString = "510 Login Failed"; break;
        case Qw::ErrorBanned: errorString = "511 Banned"; break;
        case Qw::ErrorClientNotFound: errorString = "512 Client Not Found"; break;
        case Qw::ErrorAccountNotFound: errorString = "513 Account Not Found"; break;
        case Qw::ErrorAccountExists: errorString = "514 Account Exists"; break;
        case Qw::ErrorCannotBeDisconnected: errorString = "515 Cannot Be Disconnected"; break;
        case Qw::ErrorPermissionDenied: errorString = "516 Permission Denied"; break;
        case Qw::ErrorFileOrDirectoryNotFound: errorString = "520 File or Directory Not Found"; break;
        case Qw::ErrorFileOrDirectoryExists: errorString = "521 File or Directory Exists"; break;
        case Qw::ErrorChecksumMismatch: errorString = "522 Checksum Mismatch"; break;
        case Qw::ErrorQueueLimitExceeded: errorString = "523 Queue Limit Exceeded"; break;
        default: errorString = "500 Command Failed"; break;
    }
    QwMessage reply(errorString);
    sendMessage(reply);
}


