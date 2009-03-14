/*
  Things that need to be done:
  - implement speed limits
  - ask morris about those new limit parameters in the accounts
  - file transfers, multi-threaded sockets
  - fix STAT command and checksumming

*/

/*! \class QwsClientSocket
    \author Bastian Bense <bastibense@gmail.com>
    \brief General client management and local data store access.
    \date 2009-02-22

*/

#include "QwsClientSocket.h"
#include "QwsServerController.h"
#include "QwMessage.h"

#include <QDateTime>
#include <QDirIterator>
#include <QtSql>

QwsClientSocket::QwsClientSocket(QObject *parent) : QwSocket(parent)
{
    qRegisterMetaType<QwMessage>();

    connect(this, SIGNAL(messageReceived(QwMessage)),
            this, SLOT(handleIncomingMessage(QwMessage)), Qt::QueuedConnection);

    // A new socket is always in teh inactive state.
    sessionState = Qws::StateInactive;

    // Set up the idle timer and connect it to the respective handlers
    pIdleTimer = new QTimer(this);
    pIdleTimer->setInterval(1000*60*10); // 10 minutes
    pIdleTimer->setSingleShot(true);
    connect(pIdleTimer, SIGNAL(timeout()), this, SLOT(idleTimerTriggered()));
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
    if (!user.pIdle) { return; }
    user.pIdle = false;
    emit userStatusChanged();
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


/*! This method handles any incoming messages, checks the command name and passes the control to
    the respective handler method. Each command has its own handler command, even it if just has
    to emit a simple signal. This makes it easier to implement command checks and other things
    later on.
*/
void QwsClientSocket::handleIncomingMessage(QwMessage message)
{
     qDebug() << "RECEIVED:" << message.commandName << message.arguments;

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
    } else if (message.commandName == "STAT") {        handleMessageSTAT(message);
    } else if (message.commandName == "FOLDER") {      handleMessageFOLDER(message);
    } else if (message.commandName == "DELETE") {      handleMessageDELETE(message);
    } else if (message.commandName == "MOVE") {        handleMessageMOVE(message);
    }
 }


 /*! HELLO command (Handshake request)
 */
 void QwsClientSocket::handleMessageHELLO(QwMessage &message)
 {
     Q_UNUSED(message);
     qDebug() << this << "Received a HELLO handshake request.";
     sessionState = Qws::StateConnected;
     emit sessionStateChanged(sessionState);
     sendServerInfo();
 }


 /*! CLIENT command (Client software version and name)
 */
 void QwsClientSocket::handleMessageCLIENT(QwMessage &message)
 {
     qDebug() << this << "Received client information.";
     user.pClientVersion = message.getStringArgument(0);
 }


 /*! NICK command (User nickname)
 */
 void QwsClientSocket::handleMessageNICK(QwMessage &message)
 {
     resetIdleTimer();
     qDebug() << this << "Received user nickname.";
     user.userNickname = message.getStringArgument(0);
     if (sessionState == Qws::StateActive) {
         emit userStatusChanged();
     }

 }


 /*! PASS command (User password). During handshake/connecting this actually triggers the user
     authentication and causes the server to check the password and login, also load the specific
     account information from the database and tell all other clients that a user has logged in.
 */
 void QwsClientSocket::handleMessagePASS(QwMessage &message)
 {
     if (sessionState == Qws::StateConnected && !user.userNickname.isEmpty()) {
         // We need a handshake first and a nickname. Send the client the session id of its own
         // session and proceed.
         user.pPassword = message.getStringArgument(0);

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
                 sessionState = Qws::StateActive;
                 emit sessionStateChanged(sessionState);
                 emit requestedRoomTopic(1);
                 return;
             }
         }
     }

     // If in doubt, fail
     sendError(Qws::ErrorLoginFailed);
     disconnectClient();
     return;
 }


 /*! USER command (User login name)
 */
 void QwsClientSocket::handleMessageUSER(QwMessage &message)
 {
     user.name = message.getStringArgument(0);
     if (user.name.isEmpty()) {
         // Guest fallback
         user.name = "guest";
     }
     qDebug() << this << "Received user login name:" << user.name;
 }


 /*! PING command (Keep-alive request)
 */
 void QwsClientSocket::handleMessagePING(QwMessage &message)
 {
     Q_UNUSED(message);
     sendMessage(QwMessage("202 Pong"));
 }


 /*! STATUS command (Keep-alive request)
 */
 void QwsClientSocket::handleMessageSTATUS(QwMessage &message)
 {
     resetIdleTimer();
     qDebug() << this << "Received user status:" << message.getStringArgument(0);
     user.userStatus = message.getStringArgument(0);
     if (sessionState == Qws::StateActive) {
         emit userStatusChanged();
     }
 }


 /*! WHO command (User list request)
 */
 void QwsClientSocket::handleMessageWHO(QwMessage &message)
 {
     int roomId = message.getStringArgument(0).toInt();
     qDebug() << this << "Requested user list for room #" << roomId;
     emit requestedUserlist(roomId);
 }


/*! ICON command (Set user icon)
*/
void QwsClientSocket::handleMessageICON(QwMessage &message)
{
    resetIdleTimer();
    qDebug() << this << "Received icon and image";
    if (user.pIcon != message.getStringArgument(0).toInt()) {
        user.pIcon = message.getStringArgument(0).toInt();
        if (sessionState == Qws::StateActive) {
            emit userStatusChanged();
        }
    }

    // Check if the user image changed
    if (message.arguments.value(1) != user.pImage) {
        user.pImage = message.arguments.value(1).toAscii();
        QwMessage reply("340");
        reply.appendArg(QString::number(user.pUserID));
        reply.appendArg(message.arguments.value(1));
        if (sessionState == Qws::StateActive) {
            emit broadcastedMessage(reply, 1, true);
        }
    }

}


/*! BANNER command (banner data request)
*/
void QwsClientSocket::handleMessageBANNER(QwMessage &message)
{
    Q_UNUSED(message);
    QSqlQuery query("SELECT conf_value FROM qws_config WHERE conf_key='server/banner'");
    query.first();
    QwMessage reply("203");
    if (query.isValid()) {
        reply.appendArg(query.value(0).toString());
    } else {
        qDebug() << this << "Unable to load banner from database:" << query.lastError().text();
        reply.appendArg("");
    }
    sendMessage(reply);
}


/*! INFO command (Get user info)
*/
void QwsClientSocket::handleMessageINFO(QwMessage &message)
{
    if (!user.privGetUserInfo) { sendError(Qws::ErrorPermissionDenied); return; }
    emit receivedMessageINFO(message.getStringArgument(0).toInt());
}


/*! PRIVILEGES command (get privileges mask)
*/
void QwsClientSocket::handleMessagePRIVILEGES(QwMessage &message)
{
    Q_UNUSED(message);
    QwMessage reply("602");
    user.appendPrivilegeFlagsForREADUSER(reply);
    sendMessage(reply);
}


/* === Communication == */


/*! SAY command (Relay chat to room)
*/
void QwsClientSocket::handleMessageSAY(QwMessage &message)
{
    resetIdleTimer();
    emit requestedChatRelay(message.getStringArgument(0).toInt(), message.getStringArgument(1), false);
}


/*! ME command (Relay chat to room as emote)
*/
 void QwsClientSocket::handleMessageME(QwMessage &message)
{
     resetIdleTimer();
     emit requestedChatRelay(message.getStringArgument(0).toInt(), message.getStringArgument(1), true);
}


/*! MSG command (Relay private message to other user)
*/
void QwsClientSocket::handleMessageMSG(QwMessage &message)
{
    resetIdleTimer();
    emit requestedMessageRelay(message.getStringArgument(0).toInt(), message.getStringArgument(1));
}


/*! BROADCAST command (Relay broadcast message to other users)
*/
void QwsClientSocket::handleMessageBROADCAST(QwMessage &message)
{
    resetIdleTimer();
    if (!user.privBroadcast) { sendError(Qws::ErrorPermissionDenied); return; }
    QwMessage reply("309");
    reply.appendArg(QString::number(user.pUserID));
    reply.appendArg(message.getStringArgument(0));
    emit broadcastedMessage(reply, 1, true);
}


/*! TOPIC command (Change topic of chat)
*/
void QwsClientSocket::handleMessageTOPIC(QwMessage &message)
{
    qDebug() << this << "Changing topic of chat" << message.getStringArgument(0);
    resetIdleTimer();
    if (!user.privChangeTopic) { sendError(Qws::ErrorPermissionDenied); return; }
    emit requestedRoomTopicChange(message.getStringArgument(0).toInt(), message.getStringArgument(1));
}


/*! PRIVCHAT command (Create a new chat room)
*/
void QwsClientSocket::handleMessagePRIVCHAT(QwMessage &message)
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
void QwsClientSocket::handleMessageINVITE(QwMessage &message)
{
    qDebug() << this << "Invited user to chat room" << message.getStringArgument(0);
    resetIdleTimer();
    emit requestedUserInviteToRoom(message.getStringArgument(0).toInt(),
                                   message.getStringArgument(1).toInt());
}


/*! JOIN command (Join a previously invited room)
*/
void QwsClientSocket::handleMessageJOIN(QwMessage &message)
{
    qDebug() << this << "Joining chat room" << message.getStringArgument(0);
    resetIdleTimer();
    emit receivedMessageJOIN(message.getStringArgument(0).toInt());
}


/*! The user has declined an invitation to a private chat (0).
*/
void QwsClientSocket::handleMessageDECLINE(QwMessage &message)
{
    qDebug() << this << "Declined room invitation" << message.getStringArgument(0);
    resetIdleTimer();
    emit receivedMessageDECLINE(message.getStringArgument(0).toInt());
}


/*! The user has left a chat room.
*/
void QwsClientSocket::handleMessageLEAVE(QwMessage &message)
{
    qDebug() << this << "Left room" << message.getStringArgument(0);
    resetIdleTimer();
    emit receivedMessageLEAVE(message.getStringArgument(0).toInt());
}


/* === NEWS === */

 /*! NEWS command (News list request)
 */
 void QwsClientSocket::handleMessageNEWS(QwMessage &message)
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
         sendError(Qws::ErrorComandFailed);
         return;
     }
     sendMessage(QwMessage("321 News Done"));
 }


 /*! POST command (News list request)
 */
 void QwsClientSocket::handleMessagePOST(QwMessage &message)
 {
     resetIdleTimer();
     if (!user.privPostNews) { sendError(Qws::ErrorPermissionDenied); return; }
     qDebug() << this << "Posted news";
     QSqlQuery query;
     query.prepare("INSERT INTO qws_news (news_username, news_date, news_text) "
                   "VALUES (:_login, :_date, :_text)");
     query.bindValue(":_login", QString("%1 [%2]").arg(user.userNickname).arg(user.name));
     query.bindValue(":_date", QDateTime::currentDateTime().toTimeSpec(Qt::UTC).toString(Qt::ISODate));
     query.bindValue(":_text", message.getStringArgument(0));

     if (query.exec()) {
         // Send all clients a notification.
         QwMessage reply("322");
         reply.appendArg(query.boundValue(":_login").toString());
         reply.appendArg(query.boundValue(":_date").toString()+"+00:00");
         reply.appendArg(query.boundValue(":_text").toString());
         emit broadcastedMessage(reply, 1, true);
     } else {
         qDebug() << this << "Could not post news:" << query.lastError().text();
         sendError(Qws::ErrorComandFailed);
         return;
     }

 }


 /*! CLEARNEWS command (clear news list request)
 */
 void QwsClientSocket::handleMessageCLEARNEWS(QwMessage &message)
 {
     Q_UNUSED(message);
     resetIdleTimer();
     if (!user.privClearNews) { sendError(Qws::ErrorPermissionDenied); return; }
     qDebug() << this << "Cleared news";
     QSqlQuery query;
     query.prepare("DELETE FROM qws_news");
     if (!query.exec()) {
         qDebug() << this << "Could not clear news:" << query.lastError().text();
         sendError(Qws::ErrorComandFailed);
     }
 }



/* === ADMINISTRATION === */

/*! BAN (ban user from server)
*/
void QwsClientSocket::handleMessageBAN(QwMessage &message)
{
    resetIdleTimer();
    if (!user.privBanUsers) { sendError(Qws::ErrorPermissionDenied); return; }
    qDebug() << this << "Banning user"<<message.getStringArgument(0)<<"from server";
    emit receivedMessageBAN_KICK(message.getStringArgument(0).toInt(),
                                 message.getStringArgument(1), true);
}


/*! KICK (user from server) - same as ban without the ban
*/
void QwsClientSocket::handleMessageKICK(QwMessage &message)
{
    resetIdleTimer();
    if (!user.privKickUsers) { sendError(Qws::ErrorPermissionDenied); return; }
    qDebug() << this << "Kicking user"<<message.getStringArgument(0)<<"from server";
    emit receivedMessageBAN_KICK(message.getStringArgument(0).toInt(),
                                 message.getStringArgument(1), false);
}


/*! USERS (list user accounts)
*/
void QwsClientSocket::handleMessageUSERS(QwMessage &message)
{
    resetIdleTimer();
    if (!user.privEditAccounts) { sendError(Qws::ErrorPermissionDenied); return; }

    Q_UNUSED(message);
    qDebug() << this << "Listing user accounts";

    QSqlQuery query;
    query.prepare("SELECT acc_name FROM qws_accounts ORDER BY acc_name");
    if (!query.exec()) {
        qDebug() << this << "Unable to list user accounts:" << query.lastError().text();
        sendError(Qws::ErrorComandFailed);
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
void QwsClientSocket::handleMessageGROUPS(QwMessage &message)
{
    resetIdleTimer();
    if (!user.privEditAccounts) { sendError(Qws::ErrorPermissionDenied); return; }

    Q_UNUSED(message);
    qDebug() << this << "Listing groups";

    QSqlQuery query;
    query.prepare("SELECT group_name FROM qws_groups ORDER BY group_name");
    if (!query.exec()) {
        qDebug() << this << "Unable to list user groups:" << query.lastError().text();
        sendError(Qws::ErrorComandFailed);
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
void QwsClientSocket::handleMessageREADUSER(QwMessage &message)
{
    resetIdleTimer();
    if (!user.privEditAccounts) { sendError(Qws::ErrorPermissionDenied); return; }

    qDebug() << this << "Reading user" << message.getStringArgument(0);
    QwsUser targetAccount;
    targetAccount.name = message.getStringArgument(0);
    if (targetAccount.loadFromDatabase()) {
        QwMessage reply("600");
        reply.appendArg(targetAccount.name);
        reply.appendArg(targetAccount.pPassword);
        reply.appendArg(targetAccount.pGroupName);
        targetAccount.appendPrivilegeFlagsForREADUSER(reply);
        sendMessage(reply);
    } else {
        sendError(Qws::ErrorAccountNotFound);
    }
}


/*! EDITUSER (write changes to user account)
*/
void QwsClientSocket::handleMessageEDITUSER(QwMessage &message)
{
    resetIdleTimer();
    if (!user.privEditAccounts) { sendError(Qws::ErrorPermissionDenied); return; }

    qDebug() << this << "Editing user" << message.getStringArgument(0);
    QwsUser targetAccount;
    targetAccount.name = message.getStringArgument(0);
    if (!targetAccount.loadFromDatabase()) {
        // User does not exist (or error)
        sendError(Qws::ErrorAccountNotFound);
    } else {
        targetAccount.setPrivilegesFromEDITUSER(message, 3);
        targetAccount.pPassword = message.getStringArgument(1);
        targetAccount.pGroupName = message.getStringArgument(2);
        if (!targetAccount.writeToDatabase()) {
            sendError(Qws::ErrorAccountNotFound);
        } else {
            emit modifiedUserAccount(targetAccount.name);
        }

    }
}


/*! CREATEUSER (create a new account)
*/
void QwsClientSocket::handleMessageCREATEUSER(QwMessage &message)
{
    resetIdleTimer();
    if (!user.privCreateAccounts) { sendError(Qws::ErrorPermissionDenied); return; }

    qDebug() << this << "Creating user" << message.getStringArgument(0);
    QwsUser targetUser;
    targetUser.name = message.getStringArgument(0);
    if (targetUser.loadFromDatabase()) {
        // User exists already!
        sendError(Qws::ErrorAccountExists);
    } else {
        // Create account and update it
        QSqlQuery query;
        query.prepare("INSERT INTO qws_accounts (acc_name) VALUES (:_name)");
        query.bindValue(":_name", message.getStringArgument(0));
        if (!query.exec()) {
            qDebug() << this << "Unable to create (insert) user account:" << query.lastError().text();
            sendError(Qws::ErrorComandFailed);
            return;
        }
        handleMessageEDITUSER(message);
    }
}


/*! DELETEUSER (delete user account)
*/
void QwsClientSocket::handleMessageDELETEUSER(QwMessage &message)
{
    resetIdleTimer();
    if (!user.privDeleteAccounts) { sendError(Qws::ErrorPermissionDenied); return; }

    qDebug() << this << "Editing user" << message.getStringArgument(0);
    QwsUser targetAccount;
    targetAccount.name = message.getStringArgument(0);
    if (!targetAccount.deleteFromDatabase()) {
        sendError(Qws::ErrorAccountNotFound);
    }
}


/*! READGROUP (requested details of user group)
*/
void QwsClientSocket::handleMessageREADGROUP(QwMessage &message)
{
    resetIdleTimer();
    if (!user.privEditAccounts) { sendError(Qws::ErrorPermissionDenied); return; }

    qDebug() << this << "Reading group" << message.getStringArgument(0);
    QwsUser targetGroup;
    targetGroup.name = message.getStringArgument(0);
    targetGroup.userType = Qws::UserTypeGroup;
    if (!targetGroup.loadFromDatabase()) {
        sendError(Qws::ErrorAccountNotFound);
    } else {
         QwMessage reply("601");
         reply.appendArg(targetGroup.name);
         targetGroup.appendPrivilegeFlagsForREADUSER(reply);
         sendMessage(reply);
    }
}


/*! CREATEGROUP (create a new group)
*/
void QwsClientSocket::handleMessageCREATEGROUP(QwMessage &message)
{
    resetIdleTimer();
    if (!user.privCreateAccounts) { sendError(Qws::ErrorPermissionDenied); return; }

    qDebug() << this << "Creating group" << message.getStringArgument(0);
    QwsUser targetGroup;
    targetGroup.userType = Qws::UserTypeGroup;
    targetGroup.name = message.getStringArgument(0);
    if (targetGroup.loadFromDatabase()) {
        // User exists already!
        sendError(Qws::ErrorAccountExists);
    } else {
        // Create account and update it
        QSqlQuery query;
        query.prepare("INSERT INTO qws_groups (group_name) VALUES (:_name)");
        query.bindValue(":_name", targetGroup.name);
        if (!query.exec()) {
            qDebug() << this << "Unable to create (insert) user account:" << query.lastError().text();
            sendError(Qws::ErrorComandFailed);
            return;
        }
        handleMessageEDITGROUP(message);
    }
}


/*! EDITGROUP (write changes to user group)
*/
void QwsClientSocket::handleMessageEDITGROUP(QwMessage &message)
{
    resetIdleTimer();
    if (!user.privEditAccounts) { sendError(Qws::ErrorPermissionDenied); return; }
    qDebug() << this << "Editing group" << message.getStringArgument(0);
    QwsUser targetGroup;
    targetGroup.name = message.getStringArgument(0);
    targetGroup.userType = Qws::UserTypeGroup;
    if (!targetGroup.loadFromDatabase()) {
        sendError(Qws::ErrorAccountNotFound);
    } else {
        targetGroup.setPrivilegesFromEDITUSER(message, 1);
        if (!targetGroup.writeToDatabase()) {
            sendError(Qws::ErrorAccountNotFound);
        } else {
            emit modifiedUserGroup(targetGroup.name);
        }
    }
}


/*! DELETEGROUP (delete user group)
*/
void QwsClientSocket::handleMessageDELETEGROUP(QwMessage &message)
{
    resetIdleTimer();
    if (!user.privDeleteAccounts) { sendError(Qws::ErrorPermissionDenied); return; }
    qDebug() << this << "Deleting group" << message.getStringArgument(0);
    QwsUser targetGroup;
    targetGroup.name = message.getStringArgument(0);
    targetGroup.userType = Qws::UserTypeGroup;
    if (!targetGroup.deleteFromDatabase()) {
        sendError(Qws::ErrorAccountNotFound);
    }
}


/* === FILES === */

/*! LIST (list files)
*/
void QwsClientSocket::handleMessageLIST(QwMessage &message)
{
    resetIdleTimer();
    QwsFile targetDirectory;
    targetDirectory.localFilesRoot = filesRootPath;
    targetDirectory.path = message.getStringArgument(0);

    // Check if target is valid
    if (!targetDirectory.updateLocalPath(false)) {
        sendError(Qws::ErrorFileOrDirectoryNotFound);
        return;
    }

    // Check if the target is a directory
    if (!targetDirectory.type > Qws::FileTypeRegular) {
        sendError(Qws::ErrorFileOrDirectoryNotFound);
        return;
    }

    QDirIterator it(targetDirectory.localAbsolutePath, QDirIterator::FollowSymlinks);
    while (it.hasNext()) {
        it.next();

        QwsFile itemFile;
        itemFile.localFilesRoot = targetDirectory.localAbsolutePath;
        itemFile.path = it.fileName();

        if (itemFile.updateLocalPath(true)) {
            QwMessage replyItem("410");
            replyItem.appendArg(QDir::cleanPath(targetDirectory.path + "/" + itemFile.path));
            replyItem.appendArg(QString::number(itemFile.type));
            replyItem.appendArg(QString::number(itemFile.size));
            replyItem.appendArg(itemFile.created.toTimeSpec(Qt::UTC).toString(Qt::ISODate)+"+00:00");
            replyItem.appendArg(itemFile.modified.toTimeSpec(Qt::UTC).toString(Qt::ISODate)+"+00:00");
            sendMessage(replyItem);
        }
    }

    // End of list
    QwMessage reply("411");
    reply.appendArg(targetDirectory.path);
    reply.appendArg("0");
    sendMessage(reply);
}


/*! STAT (return file/dir information)
*/
void QwsClientSocket::handleMessageSTAT(QwMessage &message)
{
    resetIdleTimer();
    QwsFile targetFile;
    targetFile.localFilesRoot = filesRootPath;
    targetFile.path = message.getStringArgument(0);

    if (!targetFile.updateLocalPath()) {
        sendError(Qws::ErrorFileOrDirectoryNotFound);
        return;
    }

    // Send file info reply
    QwMessage reply("402");
    reply.appendArg(targetFile.path);
    reply.appendArg(QString::number(targetFile.type));
    reply.appendArg(QString::number(targetFile.size));
    reply.appendArg(targetFile.created.toTimeSpec(Qt::UTC).toString(Qt::ISODate)+"+00:00");
    reply.appendArg(targetFile.modified.toTimeSpec(Qt::UTC).toString(Qt::ISODate)+"+00:00");
    reply.appendArg(targetFile.checksum);
    reply.appendArg(targetFile.comment);
    sendMessage(reply);
}


/*! FOLDER (create a directory/folder)
*/
void QwsClientSocket::handleMessageFOLDER(QwMessage &message)
{
    resetIdleTimer();
    if (!user.privCreateFolders) { sendError(Qws::ErrorPermissionDenied); return; }

    QString folderBasePath = message.getStringArgument(0).section('/', 0, -2, QString::SectionSkipEmpty);
    QString folderBaseName = message.getStringArgument(0).section('/', -1, -1, QString::SectionSkipEmpty);

    QwsFile targetBaseFolder;
    targetBaseFolder.path = folderBasePath;
    targetBaseFolder.localFilesRoot = filesRootPath;

    if (!targetBaseFolder.updateLocalPath(true)) {
        sendError(Qws::ErrorFileOrDirectoryNotFound);
        return;
    }

    QDir localBaseDir(targetBaseFolder.localAbsolutePath);
    if (!localBaseDir.mkdir(folderBaseName)) {
        sendError(Qws::ErrorFileOrDirectoryExists);
        return;
    }

    qDebug() << "Creating folder at" << folderBasePath << "with name" << folderBaseName;

}


/*! DELETE (delete a file/folder)
*/
void QwsClientSocket::handleMessageDELETE(QwMessage &message)
{
    resetIdleTimer();
    if (!user.privDeleteFiles) { sendError(Qws::ErrorPermissionDenied); return; }

    QString folderBasePath = message.getStringArgument(0).section('/', 0, -2, QString::SectionSkipEmpty);
    QString folderBaseName = message.getStringArgument(0).section('/', -1, -1, QString::SectionSkipEmpty);

    qDebug() << this << "Path=" << folderBasePath << "Item=" << folderBaseName;

    QwsFile targetBase;
    targetBase.path = message.getStringArgument(0);
    targetBase.localFilesRoot = filesRootPath;
    if (!targetBase.updateLocalPath(true)) {
        sendError(Qws::ErrorFileOrDirectoryNotFound);
        return;
    }

    QFileInfo targetInfo(targetBase.localAbsolutePath);
    QDir targetFolder(targetBase.localAbsolutePath);
    if (targetInfo.isDir()) {
        // Directory

        QDirIterator it(targetFolder, QDirIterator::Subdirectories);
        while (it.hasNext()) {
            it.next();

            qDebug() << "Deleting:" << it.filePath();
        }

    } else {
        // File
        QDir parentDir(targetBase.localAbsolutePath);
        if (!parentDir.cdUp()) {
            sendError(Qws::ErrorFileOrDirectoryNotFound);
            return;
        }
        if (!parentDir.remove(folderBaseName)) {
            qDebug() << this << "Unable to delete file" << targetBase.localAbsolutePath;
            sendError(Qws::ErrorFileOrDirectoryNotFound);
            return;
        }
    }

}


void QwsClientSocket::handleMessageMOVE(QwMessage &message)
{

}


/* === PRIVATE SLOTS === */




/*! Send information about the server. Happens during handshake or asynchronously.
*/
void QwsClientSocket::sendServerInfo()
{
     QwMessage response("200");
     response.appendArg("1.0"); // app-version
     response.appendArg("1.1"); // proto-version
     response.appendArg("Qwired Server");
     response.appendArg("A very early Qwired Server build.");
     response.appendArg(""); // start time
     response.appendArg(0); // file count
     response.appendArg(0); // total size
     sendMessage(response);
}



/*! The client needs to be removed from the server. Delete the socket and this object.
*/
void QwsClientSocket::disconnectClient()
{
    qDebug() << this << "Called disconnectClient()";
    pSocket->disconnectFromHost();
    this->deleteLater();
}


/*! Send an error message to the client if something goes wrong.
*/
void QwsClientSocket::sendError(const Qws::ProtocolError error)
{
    QByteArray errorString;
    switch (error) {
        case Qws::ErrorComandFailed: errorString = "500 Command Failed"; break;
        case Qws::ErrorCommandNotRecognized: errorString = "501 Command Not Recognized"; break;
        case Qws::ErrorCommandNotImplemented: errorString = "502 Command Not Implemented"; break;
        case Qws::ErrorSyntaxError: errorString = "503 Syntax Error"; break;
        case Qws::ErrorLoginFailed: errorString = "510 Login Failed"; break;
        case Qws::ErrorBanned: errorString = "511 Banned"; break;
        case Qws::ErrorClientNotFound: errorString = "512 Client Not Found"; break;
        case Qws::ErrorAccountNotFound: errorString = "513 Account Not Found"; break;
        case Qws::ErrorAccountExists: errorString = "514 Account Exists"; break;
        case Qws::ErrorCannotBeDisconnected: errorString = "515 Cannot Be Disconnected"; break;
        case Qws::ErrorPermissionDenied: errorString = "516 Permission Denied"; break;
        case Qws::ErrorFileOrDirectoryNotFound: errorString = "520 File or Directory Not Found"; break;
        case Qws::ErrorFileOrDirectoryExists: errorString = "521 File or Directory Exists"; break;
        case Qws::ErrorChecksumMismatch: errorString = "522 Checksum Mismatch"; break;
        case Qws::ErrorQueueLimitExceeded: errorString = "523 Queue Limit Exceeded"; break;
        default: errorString = "500 Command Failed"; break;
    }
    QwMessage reply(errorString);
    sendMessage(reply);
}

