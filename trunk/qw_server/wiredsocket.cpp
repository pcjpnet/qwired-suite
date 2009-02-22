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


/*
  Things that need to be done:

  - actually check the privileges, login and password on login
  - broadcast changes of privileges if the user is logged in
  - implement speed limits
  - ask morris about those new limit parameters in the accounts
  - file transfers, multi-threaded sockets
  - fix STAT command and checksumming

*/

#include "wiredsocket.h"
#include "QwsServerController.h"

#include "QwMessage.h"

#include <QDateTime>
#include <QDirIterator>

WiredSocket::WiredSocket(QObject *parent) : QwSocket(parent)
{
    qRegisterMetaType<QwMessage>();

    connect(this, SIGNAL(messageReceived(QwMessage)),
            this, SLOT(handleIncomingMessage(QwMessage)), Qt::QueuedConnection);


    // A new socket is always in teh inactive state.
    sessionState = Qws::StateInactive;


    pIdleTimer = new QTimer(this);
    pIdleTimer->setInterval(1000*60*10);
    pIdleTimer->setSingleShot(true);
    connect(pIdleTimer, SIGNAL(timeout()), this, SLOT(idleTimerTriggered()));
}


WiredSocket::~WiredSocket()
{
    qDebug() << "[qws] Destroying"<<pSessionUser.pUserID;
}


/**
 * The idle timer was triggered. Check if the user is not idle and mark him/her
 * as idle. Afterwards a signal will be emitted so that other clients get updated.
 */
void WiredSocket::idleTimerTriggered()
{
    if(!pSessionUser.pIdle) {
        pSessionUser.pIdle = true;
        emit userStatusChanged();
    }
}


/*!  The user shows some activity. Check if he/she is idle and un-idle.
*/
void WiredSocket::resetIdleTimer()
{
    pIdleTimer->start();
    if(pSessionUser.pIdle) {
        pSessionUser.pIdle = false;
        emit userStatusChanged();
    }
}


// Called by the socket and indicates the an SSL error has occoured.
void WiredSocket::on_socket_sslErrors(const QList<QSslError> & errors)
{
	qDebug() << "WiredSocket.on_socket_sslErrors(): "<<errors;
	pSocket->ignoreSslErrors();
}


/*! A socket error ocurred.
*/
void WiredSocket::on_socket_error()
{
    qDebug() << "[qws] socket error:"<<pSocket->errorString()<<pSocket->error();
    disconnectClient();
}


/*! This method handles any incoming messages, checks the command name and passes the control to
    the respective handler method. Each command has its own handler command, even it if just has
    to emit a simple signal. This makes it easier to implement command checks and other things
    later on.
*/
void WiredSocket::handleIncomingMessage(QwMessage message)
{
     qDebug() << "RECEIVED:" << message.commandName;

     // Handshaking/Protocol
     if (message.commandName == "HELLO") {
         handleMessageHELLO(message);
     } else if (message.commandName == "CLIENT") {
         handleMessageCLIENT(message);
     } else if (message.commandName == "NICK") { /* TODO: broadcast update */
         handleMessageNICK(message);
     } else if (message.commandName == "PASS") { /* TODO: broadcast update */
         handleMessagePASS(message);
     } else if (message.commandName == "USER") {
         handleMessageUSER(message);
     } else if (message.commandName == "PING") {
         handleMessagePING(message);
     } else if (message.commandName == "STATUS") { /* TODO: broadcast update */
         handleMessageSTATUS(message);
     } else if (message.commandName == "WHO") {
         handleMessageWHO(message);
     } else if (message.commandName == "ICON") {
         handleMessageICON(message);
     } else if (message.commandName == "BANNER") {
         handleMessageBANNER(message);
     } else if (message.commandName == "INFO") {
         handleMessageINFO(message);
     } else if (message.commandName == "PRIVILEGES") {
         handleMessagePRIVILEGES(message);


     // Communication
     } else if (message.commandName == "SAY") {
         handleMessageSAY(message);
     } else if (message.commandName == "ME") {
         handleMessageME(message);
     } else if (message.commandName == "MSG") {
         handleMessageMSG(message);
     } else if (message.commandName == "BROADCAST") {
         handleMessageBROADCAST(message);
     } else if (message.commandName == "TOPIC") {
         handleMessageTOPIC(message);
     } else if (message.commandName == "PRIVCHAT") {
         handleMessagePRIVCHAT(message);
     } else if (message.commandName == "INVITE") {
         handleMessageINVITE(message);
     } else if (message.commandName == "JOIN") {
         handleMessageJOIN(message);
     } else if (message.commandName == "DECLINE") {
         handleMessageDECLINE(message);
     } else if (message.commandName == "LEAVE") {
         handleMessageLEAVE(message);

     // News
     } else if (message.commandName == "NEWS") {
         handleMessageNEWS(message);
     } else if (message.commandName == "POST") {
         handleMessagePOST(message);
     } else if (message.commandName == "CLEARNEWS") {
         handleMessageCLEARNEWS(message);

     // Administration
     } else if (message.commandName == "KICK") {
         handleMessageKICK(message);
     } else if (message.commandName == "BAN") {
         handleMessageBAN(message);
     } else if (message.commandName == "USERS") {
         handleMessageUSERS(message);
     } else if (message.commandName == "GROUPS") {
         handleMessageGROUPS(message);
     } else if (message.commandName == "READUSER") {
         handleMessageREADUSER(message);
     } else if (message.commandName == "EDITUSER") {
         handleMessageEDITUSER(message);
     } else if (message.commandName == "CREATEUSER") {
         handleMessageCREATEUSER(message);
     } else if (message.commandName == "DELETEUSER") {
         handleMessageDELETEUSER(message);
     } else if (message.commandName == "READGROUP") {
         handleMessageREADGROUP(message);
     } else if (message.commandName == "CREATEGROUP") {
         handleMessageCREATEGROUP(message);
     } else if (message.commandName == "EDITGROUP") {
         handleMessageEDITGROUP(message);
     } else if (message.commandName == "DELETEGROUP") {
         handleMessageDELETEGROUP(message);


     // Files
    } else if (message.commandName == "LIST") {
        handleMessageLIST(message);
    } else if (message.commandName == "STAT") {
        handleMessageSTAT(message);
    }

 }



 /*! HELLO command (Handshake request)
 */
 void WiredSocket::handleMessageHELLO(QwMessage &message)
 {
     Q_UNUSED(message);
     qDebug() << this << "Received a HELLO handshake request.";
     sessionState = Qws::StateConnected;
     emit sessionStateChanged(sessionState);
     sendServerInfo();
 }


 /*! CLIENT command (Client software version and name)
 */
 void WiredSocket::handleMessageCLIENT(QwMessage &message)
 {
     qDebug() << this << "Received client information.";
     user.pClientVersion = message.getStringArgument(0);
 }


 /*! NICK command (User nickname)
 */
 void WiredSocket::handleMessageNICK(QwMessage &message)
 {
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
 void WiredSocket::handleMessagePASS(QwMessage &message)
 {
     if (sessionState == Qws::StateConnected && !user.userNickname.isEmpty()) {
         // We need a handshake first and a nickname. Send the client the session id of its own
         // session and proceed.
         user.pPassword = message.getStringArgument(0);

         QwMessage reply("201");
         reply.appendArg(QString::number(user.pUserID));
         sendMessage(reply);
         sessionState = Qws::StateActive;
         emit sessionStateChanged(sessionState);
         emit requestedRoomTopic(1);

     } else {
         // No handshake before or some information was missing.
         sendMessage(QwMessage("510 Login Failed"));
         disconnectClient();
     }
 }


 /*! USER command (User login name)
 */
 void WiredSocket::handleMessageUSER(QwMessage &message)
 {
     qDebug() << this << "Received user login name:" << message.getStringArgument(0);
     user.userLogin = message.getStringArgument(0);
 }


 /*! PING command (Keep-alive request)
 */
 void WiredSocket::handleMessagePING(QwMessage &message)
 {
     Q_UNUSED(message);
     sendMessage(QwMessage("202 Pong"));
 }


 /*! STATUS command (Keep-alive request)
 */
 void WiredSocket::handleMessageSTATUS(QwMessage &message)
 {
     qDebug() << this << "Received user status:" << message.getStringArgument(0);
     user.userStatus = message.getStringArgument(0);
     if (sessionState == Qws::StateActive) {
         emit userStatusChanged();
     }
 }


 /*! WHO command (User list request)
 */
 void WiredSocket::handleMessageWHO(QwMessage &message)
 {
     int roomId = message.getStringArgument(0).toInt();
     qDebug() << this << "Requested user list for room #" << roomId;
     emit requestedUserlist(roomId);
 }


/*! ICON command (Set user icon)
*/
void WiredSocket::handleMessageICON(QwMessage &message)
{
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
void WiredSocket::handleMessageBANNER(QwMessage &message)
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
void WiredSocket::handleMessageINFO(QwMessage &message)
{
    emit requestedClientInfo(message.getStringArgument(0).toInt());
}


/*! PRIVILEGES command (get privileges mask)
*/
void WiredSocket::handleMessagePRIVILEGES(QwMessage &message)
{
    QwMessage reply("602");
    reply.appendArg("1"); reply.appendArg("1");
    reply.appendArg("1"); reply.appendArg("1");
    reply.appendArg("1"); reply.appendArg("1");
    reply.appendArg("1"); reply.appendArg("1");
    reply.appendArg("1"); reply.appendArg("1");
    reply.appendArg("1"); reply.appendArg("1");
    reply.appendArg("1"); reply.appendArg("1");
    reply.appendArg("1"); reply.appendArg("1");
    reply.appendArg("1"); reply.appendArg("1");

    reply.appendArg("0"); reply.appendArg("0");
    reply.appendArg("0"); reply.appendArg("0");

    reply.appendArg("1");

    sendMessage(reply);
}


/* === Communication == */


/*! SAY command (Relay chat to room)
*/
void WiredSocket::handleMessageSAY(QwMessage &message)
{
     emit requestedChatRelay(message.getStringArgument(0).toInt(), message.getStringArgument(1), false);
}


/*! ME command (Relay chat to room as emote)
*/
 void WiredSocket::handleMessageME(QwMessage &message)
{
     emit requestedChatRelay(message.getStringArgument(0).toInt(), message.getStringArgument(1), true);
}


/*! MSG command (Relay private message to other user)
*/
void WiredSocket::handleMessageMSG(QwMessage &message)
{
     emit requestedMessageRelay(message.getStringArgument(0).toInt(), message.getStringArgument(1));
}


/*! BROADCAST command (Relay broadcast message to other users)
*/
void WiredSocket::handleMessageBROADCAST(QwMessage &message)
{
    QwMessage reply("309");
    reply.appendArg(QString::number(user.pUserID));
    reply.appendArg(message.getStringArgument(0));
    emit broadcastedMessage(reply, 1, true);
}


/*! TOPIC command (Change topic of chat)
*/
void WiredSocket::handleMessageTOPIC(QwMessage &message)
{
    qDebug() << this << "Changing topic of chat" << message.getStringArgument(0);
    emit requestedRoomTopicChange(message.getStringArgument(0).toInt(), message.getStringArgument(1));
}


/*! PRIVCHAT command (Create a new chat room)
*/
void WiredSocket::handleMessagePRIVCHAT(QwMessage &message)
{
    Q_UNUSED(message);
    qDebug() << this << "Requested new chat room";
    emit requestedNewRoom();
}


/*! INVITE command (Invite a user to a room).
    The INVITE command creates a new room on the server
    and automatically adds the inviting user to the room. After that it returns the ID of the
    new room and expects the user to send an INVITE command for another user(s).
*/
void WiredSocket::handleMessageINVITE(QwMessage &message)
{
    qDebug() << this << "Invited user to chat room" << message.getStringArgument(0);
    emit requestedUserInviteToRoom(message.getStringArgument(0).toInt(),
                                   message.getStringArgument(1).toInt());
}


/*! JOIN command (Join a previously invited room)
*/
void WiredSocket::handleMessageJOIN(QwMessage &message)
{
    qDebug() << this << "Joining chat room" << message.getStringArgument(0);
    emit receivedMessageJOIN(message.getStringArgument(0).toInt());
}


/*! The user has declined an invitation to a private chat (0).
*/
void WiredSocket::handleMessageDECLINE(QwMessage &message)
{
    qDebug() << this << "Declined room invitation" << message.getStringArgument(0);
    emit receivedMessageDECLINE(message.getStringArgument(0).toInt());
}


/*! The user has left a chat room.
*/
void WiredSocket::handleMessageLEAVE(QwMessage &message)
{
    qDebug() << this << "Left room" << message.getStringArgument(0);
    emit receivedMessageLEAVE(message.getStringArgument(0).toInt());
}


/* === NEWS === */

 /*! NEWS command (News list request)
 */
 void WiredSocket::handleMessageNEWS(QwMessage &message)
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
 void WiredSocket::handleMessagePOST(QwMessage &message)
 {
     qDebug() << this << "Posted news";
     QSqlQuery query;
     query.prepare("INSERT INTO qws_news (news_username, news_date, news_text) "
                   "VALUES (:_login, :_date, :_text)");
     query.bindValue(":_login", QString("%1 [%2]").arg(user.userNickname).arg(user.userLogin));
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
 void WiredSocket::handleMessageCLEARNEWS(QwMessage &message)
 {
     Q_UNUSED(message);
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
void WiredSocket::handleMessageBAN(QwMessage &message)
{
    qDebug() << this << "Banning user"<<message.getStringArgument(0)<<"from server";
    emit receivedMessageBAN_KICK(message.getStringArgument(0).toInt(),
                                 message.getStringArgument(1), true);
}


/*! KICK (user from server) - same as ban without the ban
*/
void WiredSocket::handleMessageKICK(QwMessage &message)
{
    qDebug() << this << "Kicking user"<<message.getStringArgument(0)<<"from server";
    emit receivedMessageBAN_KICK(message.getStringArgument(0).toInt(),
                                 message.getStringArgument(1), false);
}


/*! USERS (list user accounts)
*/
void WiredSocket::handleMessageUSERS(QwMessage &message)
{
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
void WiredSocket::handleMessageGROUPS(QwMessage &message)
{
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
void WiredSocket::handleMessageREADUSER(QwMessage &message)
{
    qDebug() << this << "Reading user" << message.getStringArgument(0);

    QSqlQuery query;
    query.prepare("SELECT id, acc_secret, acc_privileges, acc_group  FROM qws_accounts WHERE acc_name=:_name LIMIT 1");
    query.bindValue(":_name", message.getStringArgument(0));
    if (!query.exec()) {
        qDebug() << this << "Unable to find user account:" << query.lastError().text();
        sendError(Qws::ErrorComandFailed);
        return;
    } else {
        if (!query.size()) {
            // Account not found
            sendError(Qws::ErrorAccountNotFound);
            return;
        }
        query.first();
        QwMessage reply("600");
        reply.appendArg(message.getStringArgument(0)); // account name
        reply.appendArg(query.value(1).toString()); // password
        reply.appendArg(query.value(3).toString()); // group name

        // We create a dummy user account to ease the management of the privileges flags
        ClassWiredUser dummyUser;
        dummyUser.setPrivilegesFromQwiredSpec(query.value(2).toString());
        dummyUser.appendPrivilegeFlagsForREADUSER(reply);

        sendMessage(reply);
    }
}


/*! EDITUSER (write changes to user account)
*/
void WiredSocket::handleMessageEDITUSER(QwMessage &message)
{
    qDebug() << this << "Editing user" << message.getStringArgument(0);

    QSqlQuery query;
    query.prepare("UPDATE qws_accounts SET "
                  "acc_secret=:_secret, acc_privileges=:_privileges, acc_group=:_group "
                  "WHERE acc_name=:_name");
    query.bindValue(":_name", message.getStringArgument(0));
    query.bindValue(":_secret", message.getStringArgument(1));
    query.bindValue(":_group", message.getStringArgument(2));
    ClassWiredUser dummyUser;
    dummyUser.setPrivilegesFromEDITUSER(message, 3);
    query.bindValue(":_privileges", dummyUser.privilegesFlagsAsQwiredSpec());
    if (!query.exec()) {
        qDebug() << this << "Unable to edit user account:" << query.lastError().text();
        sendError(Qws::ErrorComandFailed);
        return;
    } else {
        if (!query.numRowsAffected()) {
            sendError(Qws::ErrorAccountNotFound);
        }
    }
}





/*! CREATEUSER (create a new account)
*/
void WiredSocket::handleMessageCREATEUSER(QwMessage &message)
{
    qDebug() << this << "Creating user" << message.getStringArgument(0);

    // Check if the user already exists. If it does not exist, create it and call an update
    // on the fresh account.
    QSqlQuery query;
    query.prepare("SELECT 1 FROM qws_accounts WHERE acc_name=:_name");
    query.bindValue(":_name", message.getStringArgument(0));
     if (!query.exec()) {
        qDebug() << this << "Unable to create (check) user account:" << query.lastError().text();
        sendError(Qws::ErrorComandFailed);
        return;
    } else {
        query.first();
        if (query.isValid()) {
            // Account already exists
            sendError(Qws::ErrorAccountExists);
            return;
        } else {
            // Create the new account and call EDITUSER to update it
            query.clear();
            query.prepare("INSERT INTO qws_accounts (acc_name) VALUES (:_name)");
            query.bindValue(":_name", message.getStringArgument(0));
            if (!query.exec()) {
                qDebug() << this << "Unable to create (insert) user account:" << query.lastError().text();
                sendError(Qws::ErrorComandFailed);
                return;
            }
            // Now update the fresh account
            handleMessageEDITUSER(message);
        }
    }
}


/*! DELETEUSER (delete user account)
*/
void WiredSocket::handleMessageDELETEUSER(QwMessage &message)
{
    qDebug() << this << "Editing user" << message.getStringArgument(0);

    QSqlQuery query;
    query.prepare("DELETE FROM qws_accounts WHERE acc_name=:_name");
    query.bindValue(":_name", message.getStringArgument(0));
    if (!query.exec()) {
        qDebug() << this << "Unable to delete user account:" << query.lastError().text();
        sendError(Qws::ErrorComandFailed);
        return;
    } else {
        if (!query.numRowsAffected()) {
            sendError(Qws::ErrorAccountNotFound);
        }
    }
}


/*! READGROUP (requested details of user group)
*/
void WiredSocket::handleMessageREADGROUP(QwMessage &message)
{
    qDebug() << this << "Reading group" << message.getStringArgument(0);

    QSqlQuery query;
    query.prepare("SELECT id, group_name, group_privs FROM qws_groups WHERE group_name=:_name LIMIT 1");
    query.bindValue(":_name", message.getStringArgument(0));
    if (!query.exec()) {
        qDebug() << this << "Unable to find user account:" << query.lastError().text();
        sendError(Qws::ErrorComandFailed);
        return;
    } else {
        if (!query.size()) {
            sendError(Qws::ErrorAccountNotFound);
            return;
        }
        query.first();
        QwMessage reply("601");
        reply.appendArg(message.getStringArgument(0)); // group name

        // We create a dummy user account to ease the management of the privileges flags
        ClassWiredUser dummyUser;
        dummyUser.setPrivilegesFromQwiredSpec(query.value(2).toString());
        dummyUser.appendPrivilegeFlagsForREADUSER(reply);

        sendMessage(reply);
    }
}


/*! CREATEGROUP (create a new group)
*/
void WiredSocket::handleMessageCREATEGROUP(QwMessage &message)
{
    qDebug() << this << "Creating group" << message.getStringArgument(0);

    // Check if the group already exists. If it does not exist, create it and call an update
    // on the fresh group record.
    QSqlQuery query;
    query.prepare("SELECT 1 FROM qws_groups WHERE group_name=:_name");
    query.bindValue(":_name", message.getStringArgument(0));
     if (!query.exec()) {
        qDebug() << this << "Unable to create (check) group:" << query.lastError().text();
        sendError(Qws::ErrorComandFailed);
        return;
    } else {
        query.first();
        if (query.isValid()) {
            // Group already exists
            sendError(Qws::ErrorAccountExists);
            return;
        } else {
            // Create the new account and call EDITGROUP to update it
            query.clear();
            query.prepare("INSERT INTO qws_groups (group_name) VALUES (:_name)");
            query.bindValue(":_name", message.getStringArgument(0));
            if (!query.exec()) {
                qDebug() << this << "Unable to create (insert) group account:" << query.lastError().text();
                sendError(Qws::ErrorComandFailed);
                return;
            }
            // Now update the fresh account
            handleMessageEDITGROUP(message);
        }
    }
}


/*! EDITGROUP (write changes to user group)
*/
void WiredSocket::handleMessageEDITGROUP(QwMessage &message)
{
    qDebug() << this << "Editing group" << message.getStringArgument(0);

    QSqlQuery query;
    query.prepare("UPDATE qws_groups SET group_privs=:_privs WHERE group_name=:_name");
    query.bindValue(":_name", message.getStringArgument(0));

    ClassWiredUser dummyUser;
    dummyUser.setPrivilegesFromEDITUSER(message, 1);
    query.bindValue(":_privs", dummyUser.privilegesFlagsAsQwiredSpec());

    if (!query.exec()) {
        qDebug() << this << "Unable to edit user group:" << query.lastError().text();
        sendError(Qws::ErrorComandFailed);
        return;
    } else {
        if (!query.numRowsAffected()) {
            sendError(Qws::ErrorAccountNotFound);
        }
    }
}


/*! DELETEGROUP (delete user group)
*/
void WiredSocket::handleMessageDELETEGROUP(QwMessage &message)
{
    qDebug() << this << "Deleting group" << message.getStringArgument(0);

    QSqlQuery query;
    query.prepare("DELETE FROM qws_groups WHERE group_name=:_name");
    query.bindValue(":_name", message.getStringArgument(0));
    if (!query.exec()) {
        qDebug() << this << "Unable to delete user group:" << query.lastError().text();
        sendError(Qws::ErrorComandFailed);
        return;
    } else {
        if (!query.numRowsAffected()) {
            sendError(Qws::ErrorAccountNotFound);
        }
    }
}


/* === FILES === */

/*! LIST (list files)
*/
void WiredSocket::handleMessageLIST(QwMessage &message)
{
    QDir userRootDir(filesRootPath);
    QString pathRequested = message.getStringArgument(0);
    QString userCurrent = localPathFromVirtualPath(pathRequested);
    QDir userCurrentDir(userCurrent);

    qDebug() << this << "LIST:" << pathRequested;
    qDebug() << this << "  =>" << userCurrent;

    // Compare the absolute path (including unresolved symlinks) to the base path of the user's
    // account. If it correct, the symlinks will be resolved.
    if (!userCurrentDir.absolutePath().startsWith(userRootDir.absolutePath())) {
        qDebug() << this << "Unable to escape jail list directory:" << userCurrentDir.canonicalPath();
        sendError(Qws::ErrorFileOrDirectoryNotFound);
        return;
    }

    if (!userCurrentDir.exists()) {
        qDebug() << this << "Target dir does not exist!";
        sendError(Qws::ErrorFileOrDirectoryNotFound);
        return;
    }

    QDirIterator it(userCurrentDir, QDirIterator::FollowSymlinks);
    while (it.hasNext()) {
        QString item = it.next();
        QFileInfo itemInfo(item);
        QString itemRemotePath = QDir::cleanPath(pathRequested+"/"+itemInfo.fileName());

        if (itemInfo.isHidden()) {
            continue;
        }

        // File list item
        QwMessage replyItem("410");
        replyItem.appendArg(itemRemotePath);
        if (itemInfo.isDir() || itemInfo.isSymLink()) {
            QDir dirInfo(itemInfo.canonicalFilePath());
            replyItem.appendArg("1"); // Type Directory
            replyItem.appendArg(QString::number(dirInfo.count()));
        } else {
            replyItem.appendArg("0"); // Type File
            replyItem.appendArg(QString::number(itemInfo.size()));
        }
        replyItem.appendArg(itemInfo.created().toTimeSpec(Qt::UTC).toString(Qt::ISODate)+"+00:00");
        replyItem.appendArg(itemInfo.lastModified().toTimeSpec(Qt::UTC).toString(Qt::ISODate)+"+00:00");
        sendMessage(replyItem);
    }

    // End of list
    QwMessage reply("411");
    reply.appendArg(pathRequested);
    reply.appendArg("0");
    sendMessage(reply);
}


/*! STAT (return file/dir information)
*/
void WiredSocket::handleMessageSTAT(QwMessage &message)
{
    QDir filesRootDir(filesRootPath);
    QString pathRequested = message.getStringArgument(0);
    QString targetFile = localPathFromVirtualPath(pathRequested);

    QFileInfo localVirtualFileInfo(targetFile);
    QString localAbsolutePath = localVirtualFileInfo.absoluteFilePath();
    qDebug() << "localAbsolutePath:" << localAbsolutePath;

    qDebug() << "filesRootPath:" << filesRootPath;
    if (!localAbsolutePath.startsWith(filesRootDir.absolutePath())) {
        qDebug() << this << "Preventing access to" << localAbsolutePath;
        sendError(Qws::ErrorFileOrDirectoryNotFound);
        return;
    }

    QString localCanonicalPath = localVirtualFileInfo.canonicalFilePath();
    qDebug() << "localCanonicalPath:" << localCanonicalPath;

    QFileInfo localFileInfo(localCanonicalPath);

    QwMessage reply("402");
    reply.appendArg(pathRequested);
    if (localFileInfo.isDir()) {
        QDir localDir(localCanonicalPath);
        reply.appendArg("1");
        reply.appendArg(QString::number(localDir.count()));
    } else {
        reply.appendArg("0");
        reply.appendArg(QString::number(localFileInfo.size()));
    }
    reply.appendArg(localFileInfo.created().toTimeSpec(Qt::UTC).toString(Qt::ISODate)+"+00:00");
    reply.appendArg(localFileInfo.lastModified().toTimeSpec(Qt::UTC).toString(Qt::ISODate)+"+00:00");

    reply.appendArg(""); // Checksum
    reply.appendArg(""); // Comment
    sendMessage(reply);
}

/* === PRIVATE SLOTS === */


/*! Send a userlist item to the client. This includes some information about the client. The whole
     request is completed with the sendUserlistDone() call.
*/
void WiredSocket::sendUserlistItem(const int roomId, const ClassWiredUser &item)
{
     QwMessage reply("310");
     reply.appendArg(QByteArray::number(roomId));
     item.userListEntry(reply);
     sendMessage(reply);
}


/*! See sendUserlistItem() for more information.
*/
void WiredSocket::sendUserlistDone(const int roomId)
{
    QwMessage reply("311");
    reply.appendArg(QByteArray::number(roomId));
    sendMessage(reply);
}


/*! Sends a 303-type message (client left).
*/
void WiredSocket::sendClientLeave(const int chatId, const int id)
{
    QwMessage reply("303");
    reply.appendArg(QByteArray::number(chatId));
    reply.appendArg(QByteArray::number(id));
    sendMessage(reply);
}


/*! Send information about the server. Happens during handshake or asynchronously.
*/
void WiredSocket::sendServerInfo()
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


/*! Send the chat topic to the client.
*/
void WiredSocket::sendChatTopic(const QwsRoom *chat)
{
    QwMessage reply("341");
    reply.appendArg(QString::number(chat->pChatId));
    reply.appendArg(chat->pTopicSetter.userNickname);
    reply.appendArg(chat->pTopicSetter.userLogin);
    reply.appendArg(chat->pTopicSetter.userIpAddress);
    reply.appendArg(chat->pTopicDate.toTimeSpec(Qt::UTC).toString(Qt::ISODate)+"+00:00");
    reply.appendArg(chat->pTopic);
    sendMessage(reply);
}


/*! The client needs to be removed from the server. Delete the socket and this object.
*/
void WiredSocket::disconnectClient()
{
    qDebug() << this << "Called disconnectClient()";
    emit clientDisconnected(pSessionUser.pUserID);
    pSocket->disconnectFromHost();
    this->deleteLater();
}


/*! Send a chat message for a specific room to the client. If \a isEmote is true, the message will
    be sent as a 301-emote message, otherwise as 300-normal chat.
*/
void WiredSocket::sendChat(const int chatId, const int userId, const QString text, const bool isEmote)
{
    QwMessage reply(isEmote ? "301 " : "300 ");
    reply.appendArg(QByteArray::number(chatId));
    reply.appendArg(QByteArray::number(userId));
    reply.appendArg(text.toUtf8());
    sendMessage(reply);
}


/*! Notify that client that a user has joined the server (if chatId=1) o  a private chat.
*/
void WiredSocket::sendClientJoin(const int chatId, const ClassWiredUser &user)
{
    QwMessage reply("302");
    reply.appendArg(QByteArray::number(chatId));
    user.userListEntry(reply);
    sendMessage(reply);
}


/*! Deliver a private message to this user.
*/
void WiredSocket::sendPrivateMessage(const int userId, const QString text)
{
    QwMessage reply("305");
    reply.appendArg(QByteArray::number(userId));
    reply.appendArg(text.toUtf8());
    sendMessage(reply);
}

/*! Send an error message to the client if something goes wrong.
*/
void WiredSocket::sendError(const Qws::ProtocolError error)
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


/*! This method returns the local disk path of a file or directory depending on the user's root
    directory.
*/
QString WiredSocket::localPathFromVirtualPath(const QString &path)
{
    return QDir(filesRootPath+QDir::separator()+path).absolutePath();
}

