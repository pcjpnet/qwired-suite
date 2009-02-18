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


#include "wiredsocket.h"
#include "QwsServerController.h"

#include "QwMessage.h"

#include <QDateTime>

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


// Called by readDataFromSocket() after splitting a message off the internal
// buffer. We have to extract the command ID here and decide what to do with
// the message.
void WiredSocket::handleWiredMessage(QByteArray theData)
{
	int tmpPos = theData.indexOf(" ");
	QString tmpCmd = tmpPos>-1 ? QString::fromUtf8(theData.left(tmpPos)) : QString::fromUtf8(theData);
        QList<QByteArray> tmpParams; //= splitMessageFields(theData);

	qDebug() << userId()<<"| Incoming command:"<<tmpCmd;
        if (sessionState == Qws::StateInactive) {
            // No handshake received yet. The only thing we accept is a HELLO.

                //pHandshakeOK = tmpCmd=="HELLO";
                if(sessionState==Qws::StateInactive) {
                        QwsServerController::qwLog("Fatal: Handshake failed. Dropping connection.");
			pSocket->disconnectFromHost();
		} else {
			emit handshakeComplete(userId());
		}

	} else {
		// Handshake exists, handle the command.

                if(tmpCmd=="BAN") {
			emit userKicked(userId(), tmpParams.value(0).toInt(), QString::fromUtf8(tmpParams.value(1)), true );
			resetIdleTimer();

		} else if(tmpCmd=="DELETEUSER") {
			emit deletedUser(userId(), QString::fromUtf8(tmpParams.value(0)));

		} else if(tmpCmd=="DELETEGROUP") {
			emit deletedGroup(userId(), QString::fromUtf8(tmpParams.value(0)));

		} else if(tmpCmd=="CREATEUSER") {
			ClassWiredUser tmpUser;
                        tmpUser.userLogin = QString::fromUtf8(tmpParams.value(0));
			tmpUser.pPassword = QString::fromUtf8(tmpParams.value(1));
			tmpUser.pGroupName = QString::fromUtf8(tmpParams.value(2));
			tmpParams.removeAt(0);
			tmpParams.removeAt(0);
			tmpParams.removeAt(0);
			tmpUser.setFromPrivileges(tmpParams);
			emit createdUser(userId(), tmpUser);

		} else if(tmpCmd=="CREATEGROUP") {
			ClassWiredUser tmpUser;
			tmpUser.pGroupName = QString::fromUtf8(tmpParams.value(0));
			tmpParams.removeAt(0);
			tmpUser.setFromPrivileges(tmpParams);
			emit createdGroup(userId(), tmpUser);
			
			
		} else if(tmpCmd=="DECLINE") {
			emit declinedPrivateChat(userId(), tmpParams.value(0).toInt() );

		} else if(tmpCmd=="EDITUSER") {
			ClassWiredUser tmpUser;
                        tmpUser.userLogin = QString::fromUtf8(tmpParams.value(0));
			tmpUser.pPassword = QString::fromUtf8(tmpParams.value(1));
			tmpUser.pGroupName = QString::fromUtf8(tmpParams.value(2));
			tmpParams.removeAt(0);
			tmpParams.removeAt(0);
			tmpParams.removeAt(0);
			qDebug() << tmpParams;
			tmpUser.setFromPrivileges(tmpParams);
			emit editedUser(userId(), tmpUser);

		} else if(tmpCmd=="EDITGROUP") {
			ClassWiredUser tmpUser;
			tmpUser.pGroupName = QString::fromUtf8(tmpParams.value(0));
			tmpParams.removeAt(0);
			tmpUser.setFromPrivileges(tmpParams);
			emit editedGroup(userId(), tmpUser);

		} else if(tmpCmd=="GROUPS") {
			emit requestedGroupsList(userId());
			resetIdleTimer();


		} else if(tmpCmd=="KICK") {
			emit userKicked(userId(), tmpParams.value(0).toInt(), QString::fromUtf8(tmpParams.value(1)), false );
			resetIdleTimer();
			
		} else if(tmpCmd=="LIST") {
			emit requestedFileList(userId(), QString::fromUtf8(tmpParams.value(0)));
			resetIdleTimer();

		} else if(tmpCmd=="PRIVILEGES") {
			sendPrivileges();
			
		} else if(tmpCmd=="STAT") {
			emit requestedFileStat(userId(), QString::fromUtf8(tmpParams.value(0)));
			resetIdleTimer();


		} else if(tmpCmd=="READUSER") {
			emit requestedReadUser(userId(), QString::fromUtf8(tmpParams.value(0)) );
			resetIdleTimer();

		} else if(tmpCmd=="READGROUP") {
			emit requestedReadGroup(userId(), QString::fromUtf8(tmpParams.value(0)) );
			resetIdleTimer();

		} else if(tmpCmd=="USERS") {
			emit requestedAccountsList(userId());
			resetIdleTimer();


		} else {
			qDebug() << "Unknown Command |"<<userId()<<"|"<<tmpCmd;
// 			qwSendCommandNotImplemented();
		}

	}

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






/**
 * The new private chat has been created and the user has been automatically
 * added to it.
 * @param chatId The ID of the newly created chat.
 */
void WiredSocket::sendPrivateChatCreated(const int chatId) {
	QByteArray ba("330 ");
	ba += QByteArray::number(chatId);
//	sendWiredCommand(ba);
}

/**
 * Invite a user to a private, existing chat room.
 * @param chatId The ID of the private chat.
 * @param id The ID of the inviting user.
 */
void WiredSocket::sendInviteToChat(const int chatId, const int id) {
	QByteArray ba("331 ");
	ba += QByteArray::number(chatId); ba += kFS;
	ba += QByteArray::number(id);
//	sendWiredCommand(ba);
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

/**
 * Send the information about a user to the client.
 * @param user The user object containing the requested information.
 */
void WiredSocket::sendUserInfo(const ClassWiredUser user) {
//	QByteArray ba("308 ");
//	ba += user.userInfoEntry();
//	sendWiredCommand(ba);
}



/**
 * A used declined a private chat invitation. Let the inviting users know of this.
 * @param chatId The ID of the chat.
 * @param userId The ID of the user who rejected the invitation.
 */
void WiredSocket::sendClientDeclinedChat(const int chatId, const int userId) {
	QByteArray ba("332 ");
	ba += QByteArray::number(chatId); ba += kFS;
	ba += QByteArray::number(userId);
//	sendWiredCommand(ba);
}

/**
 * Set the client-version string of this client to info. Also set some information about the used
 * encryption, etc...
 * @param info The client-version string.
 */
void WiredSocket::setClientInfo(const QString info) {
	pSessionUser.pClientVersion = info;
}

/**
 * Send a list of privileges to the client.
 */
void WiredSocket::sendPrivileges() {
	QByteArray ba("602 ");
//	ba += pSessionUser.privilegesFlags();
//	sendWiredCommand(ba);
}


/**
 * A administrator kicked or banned a user.
 * @param killerId The ID of the administrator.
 * @param victimId The ID of the kicked user.
 * @param reason The reason (if any).
 * @param banned If true, the user was temporarily banned.
 */
void WiredSocket::sendClientKicked(const int killerId, const int victimId, const QString reason, const bool banned) {
	QByteArray ba( banned ? "307 " : "306 " );
	ba += QByteArray::number(victimId); ba += kFS;
	ba += QByteArray::number(killerId); ba += kFS;
	ba += reason.toUtf8();
//	sendWiredCommand(ba);
}




/**
 * Send a account listing item to the client.
 * @param name The name of the account.
 */
void WiredSocket::sendAccountListing(const QString name) {
	QByteArray ba("610 ");
	ba += name.toUtf8();
//	sendWiredCommandBuffer(ba);
}

/**
 * The list of accounts has been transmitted.
 */
void WiredSocket::sendAccountListingDone() {
//    sendWiredCommand("611 Done");
}

/**
 * Send a account listing item to the client.
 * @param name The name of the account.
 */
void WiredSocket::sendGroupListing(const QString name) {
	QByteArray ba("620 ");
	ba += name.toUtf8();
//	sendWiredCommandBuffer(ba);
}

/**
 * The list of accounts has been transmitted.
 */
void WiredSocket::sendGroupListingDone() {
//    sendWiredCommand("621 Done");
}



/**
 * Send the user account specification.
 * @param account The object holding the account data.
 */
void WiredSocket::sendUserSpec(const ClassWiredUser account) {
//	QByteArray ba("600 ");
//        ba += account.userLogin.toUtf8(); ba += kFS;
//	ba += account.pPassword.toUtf8(); ba += kFS;
//	ba += account.pGroupName.toUtf8(); ba += kFS;
//	ba += account.privilegesFlags();
//	sendWiredCommand(ba);
}

/**
 * Send the group specification.
 * @param account The object holding the group data.
 */
void WiredSocket::sendGroupSpec(const ClassWiredUser group) {
//	QByteArray ba("601 ");
//	ba += group.pGroupName.toUtf8(); ba += kFS;
//	ba += group.privilegesFlags();
//	sendWiredCommand(ba);
}


void WiredSocket::sendFileListing(const ClassWiredFile file) {
	QByteArray ba("410 ");
	ba += file.path.toUtf8(); ba += kFS;
	ba += QByteArray::number(file.type); ba += kFS;
	ba += QByteArray::number(file.size); ba += kFS;
	ba += file.created.toString(Qt::ISODate); ba += "+00:00"; ba += kFS;
	ba += file.modified.toString(Qt::ISODate); ba += "+00:00"; ba += kFS;
	ba += file.checksum.toUtf8(); ba += kFS;
	ba += file.comment.toUtf8();
//	sendWiredCommandBuffer(ba);
}

void WiredSocket::sendFileStat(const ClassWiredFile file) {
	QByteArray ba("402 ");
	ba += file.path.toUtf8(); ba += kFS;
	ba += QByteArray::number(file.type); ba += kFS;
	ba += QByteArray::number(file.size); ba += kFS;
	ba += file.created.toString(Qt::ISODate); ba += "+00:00"; ba += kFS;
	ba += file.modified.toString(Qt::ISODate); ba += "+00:00"; ba += kFS;
	ba += file.checksum.toUtf8(); ba += kFS;
	ba += file.comment.toUtf8();
//	sendWiredCommandBuffer(ba);
}

void WiredSocket::sendFileListingDone(const QString path, const int free) {
	QByteArray ba("411 ");
	ba += path.toUtf8(); ba += kFS;
	ba += QByteArray::number(free);
//	sendWiredCommand(ba);
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




