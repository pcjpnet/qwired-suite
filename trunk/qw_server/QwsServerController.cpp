/***************************************************************************
 *   Copyright (C) 2007 by Bastian Bense   *
 *   bb@bense.de   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/




#include "QwsServerController.h"
#include "QwsClientSocket.h"

#include <QtSql>

QwsServerController::QwsServerController(QObject *parent) : QObject(parent)
{
    sessionIdCounter = 20;
    roomIdCounter = 10;

    // Initialize the main room (public chat, server user list)
    QwRoom *publicRoom = new QwRoom;
    publicRoom->pChatId = 1;
    publicRoom->pTopic = tr("Welcome to Qwired Server!");
    rooms[1] = publicRoom;
}


QwsServerController::~QwsServerController()
{
}


/*! Read the configuration file from the disk and store it in our configuration hash for quick
    parameter access during runtime.
 */
bool QwsServerController::loadConfiguration()
{
    QStringList tmpCmdArgs = QCoreApplication::arguments();

    // Override database path, if the user specifies it.
    QString databasePath = "qwired_server.db";
    if (int index = tmpCmdArgs.indexOf("-c") > -1) {
        databasePath = tmpCmdArgs.value(index+1);
    }

    qwLog(QString("Loading configuration database at '%1'").arg(databasePath));

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(databasePath);
    if (!db.open()) {
        qwLog(QString("Warning: Unable to open configuration database. (%1)")
              .arg(db.lastError().text()));
        return false;
    } else {
        qwLog("Successfully opened database.");
    }

    return true;
}


/*! Return a configuration pamater identified by \a key. If the configuration parameter does not
    exist in the database, \a defaultValue is return. If there is a database error, a Null-QVariant
    will be returned.
*/
QVariant QwsServerController::getConfigurationParam(const QString key, const QVariant defaultValue)
{
    QSqlQuery query;
    query.prepare("SELECT conf_value FROM qws_config WHERE conf_key=:_key");
    query.bindValue(":_key", key);
    if (query.exec()) {
        // Check if there were any results.
        query.first();
        if (query.isValid()) {
            return query.value(0);
        } else {
            // Return the default value instead.
            return defaultValue;
        }
    } else {
        qwLog(QString("Fatal: Unable to read configuration parameter '%1' from database. (SQL: %2)")
              .arg(key).arg(query.lastError().text()));
        return QVariant();
    }
}


/*! Start the TCP listener in order to accept connections.
*/
bool QwsServerController::startServer()
{
    int tmpPort = getConfigurationParam("server/port", 2000).toInt();
    QString tmpAddress = getConfigurationParam("server/address", "0.0.0.0").toString();
    qwLog(QString("Starting server on  %2:%1...").arg(tmpPort).arg(tmpAddress));

    pTcpServer = new QwSslTcpServer(this);
    connect(pTcpServer, SIGNAL(newSslConnection()), this, SLOT(acceptSslConnection()));

    QString certificateFile = getConfigurationParam("server/certificate", "qwired_server.crt").toString();
    if (!pTcpServer->setCertificateFromFile(certificateFile)) {
        return false;
    }

    if(!pTcpServer->listen( QHostAddress(tmpAddress), tmpPort)) {
        qwLog(QString("Fatal: Unable to listen on TCP port %1. %2. Terminating.").arg(tmpPort).arg(pTcpServer->errorString()));
        return false;
    }

    return true;
}


/**
 * Write a message to the log (and console)
 */
void QwsServerController::qwLog(QString theMessage) {
        std::cout << QString("[%1] %2").arg(QDateTime::currentDateTime().toString()).arg(theMessage).toStdString() << std::endl;
}



/*! Accepts a new connection. Connected to the newConnect() signal of pTcpServer.
*/
void QwsServerController::acceptSslConnection()
{
    QSslSocket *newSocket = pTcpServer->nextPendingSslSocket();

    QwsClientSocket *clientSocket = new QwsClientSocket(this);
    clientSocket->user.pUserID = ++sessionIdCounter;
    clientSocket->setSslSocket(newSocket);

    connect(clientSocket, SIGNAL(connectionLost()), this, SLOT(handleSocketDisconnected()));

    qRegisterMetaType<QwMessage>();
    qRegisterMetaType<Qws::SessionState>();

    connect(clientSocket, SIGNAL(requestedUserlist(int)),
            this, SLOT(handleUserlistRequest(int)));
    connect(clientSocket, SIGNAL(sessionStateChanged(Qws::SessionState)),
            this, SLOT(handleSocketSessionStateChanged(Qws::SessionState)));
    connect(clientSocket, SIGNAL(requestedChatRelay(int,QString,bool)),
            this, SLOT(relayChatToRoom(int,QString,bool)));
    connect(clientSocket, SIGNAL(requestedMessageRelay(int,QString)),
            this, SLOT(relayMessageToUser(int,QString)));
    connect(clientSocket, SIGNAL(broadcastedMessage(QwMessage,int,bool)),
            this, SLOT(broadcastMessage(QwMessage,int,bool)));
    connect(clientSocket, SIGNAL(userStatusChanged()),
            this, SLOT(relayUserStatusChanged()));
    connect(clientSocket, SIGNAL(requestedRoomTopicChange(int,QString)),
            this, SLOT(changeRoomTopic(int,QString)));
    connect(clientSocket, SIGNAL(requestedRoomTopic(int)),
            this, SLOT(sendRoomTopic(int)));
    connect(clientSocket, SIGNAL(requestedNewRoom()),
            this, SLOT(createNewRoom()));
    connect(clientSocket, SIGNAL(requestedUserInviteToRoom(int,int)),
            this, SLOT(inviteUserToRoom(int,int)));
    connect(clientSocket, SIGNAL(receivedMessageJOIN(int)),
            this, SLOT(handleMessageJOIN(int)));
    connect(clientSocket, SIGNAL(receivedMessageDECLINE(int)),
            this, SLOT(handleMessageDECLINE(int)));
    connect(clientSocket, SIGNAL(receivedMessageLEAVE(int)),
            this, SLOT(handleMessageLEAVE(int)));
    connect(clientSocket, SIGNAL(receivedMessageBAN_KICK(int,QString,bool)),
            this, SLOT(handleMessageBAN_KICK(int,QString,bool)));
    connect(clientSocket, SIGNAL(receivedMessageINFO(int)),
            this, SLOT(handleMessageINFO(int)) );
    connect(clientSocket, SIGNAL(modifiedUserAccount(QString)),
            this, SLOT(handleModifiedUserAccount(QString)));
    connect(clientSocket, SIGNAL(modifiedUserGroup(QString)),
            this, SLOT(handleModifiedUserGroup(QString)));

    clientSocket->filesRootPath = getConfigurationParam("files/root", "./files").toString();

    sockets[clientSocket->user.pUserID] = clientSocket;
}


/*! This slot is called whenever a client disconnects. Here we clean up behind the client and
    make sure everyone knows about it.
*/
void QwsServerController::handleSocketDisconnected()
{
    QwsClientSocket *client = qobject_cast<QwsClientSocket*>(sender());
    if (!client) { return; }
    qDebug() << this << "Sending all clients the client-left event.";

    // Remove the user from the chat rooms
    QHashIterator<int, QwRoom*> i(rooms);
    i.toBack();
    while (i.hasPrevious()) {
        i.previous();
        QwRoom *itemRoom = i.value();
        if (itemRoom && itemRoom->pUsers.contains(client->user.pUserID)) {
            removeUserFromRoom(i.key(), client->user.pUserID);
        }
    }

    // Remove the client from the list of clients
    sockets.remove(client->user.pUserID);
    sender()->deleteLater();
}


/*! This is called whenever the state of a client changes.
*/
void QwsServerController::handleSocketSessionStateChanged(const Qws::SessionState state)
{
    QwsClientSocket *client = qobject_cast<QwsClientSocket*>(sender());
    if (!client) { return; }
    if (state == Qws::StateActive) {
        // Client became active (logged in)
        addUserToRoom(1, client->user.pUserID);
    }
}


/*! This is called when the status (and information) about a user changes.
*/
void QwsServerController::relayUserStatusChanged()
{
    QwsClientSocket *client = qobject_cast<QwsClientSocket*>(sender());
    if (!client) { return; }
    QwMessage reply("304");
    client->user.userStatusEntry(reply);
    broadcastMessage(reply, 1, true);
}


/*! This slot is called when a client requests information about another client.
*/
void QwsServerController::handleMessageINFO(const int userId)
{
    QwsClientSocket *user = qobject_cast<QwsClientSocket*>(sender());
    if (!sockets.contains(userId)) {
        user->sendError(Qws::ErrorClientNotFound);
        return;
    }
    QwsClientSocket *target = sockets[userId];
    if (!target) { return; }

    // Send the information
    QwMessage reply("308");
    target->user.userInfoEntry(reply);
    user->sendMessage(reply);
}




/*! This method adds a user to a given room and notifies all other users in the same room about this
    change, respectively sending out "client joined" messages.
*/
void QwsServerController::addUserToRoom(const int roomId, const int userId)
{
    if (!rooms.contains(roomId)) {
        qDebug() << this << "Unable to add user" << userId << "to room" << roomId << "(no such room)";
        return;
    }
    QwRoom *room = rooms[roomId];
    if (!sockets.contains(userId)) {
        qDebug() << this << "Unable to add user" << userId << "to room" << roomId << "(no such user)";
        return;
    }
    QwsClientSocket *user = sockets[userId];
    room->pUsers.append(userId);

    // Notify the other users
    QwMessage reply("302");
    reply.appendArg(QByteArray::number(roomId));
    user->user.userListEntry(reply);
    broadcastMessage(reply, roomId, false);
}

/*! This method removes a user from the a room and notifies all other users in the same room about
    this change, respectively sending out "client left" messages.
*/
void QwsServerController::removeUserFromRoom(const int roomId, const int userId)
{
    qDebug() << this << "Removing user" << userId << "from room" << roomId;
    if (!rooms.contains(roomId)) { return; }
    QwRoom *room = rooms[roomId];
    if (!room->pUsers.contains(userId)) { return; }

    if (room->pUsers.contains(userId)) {
        room->pUsers.removeAll(userId);
        // Notify the users.
        QwMessage reply("303");
        reply.appendArg(QString::number(roomId));
        reply.appendArg(QString::number(userId));
        broadcastMessage(reply, roomId, false);
    }
    room->pInvitedUsers.removeOne(userId);

    // Delete the chat if it's the last one
    if (roomId != 1 && room->pUsers.isEmpty()) {
        qDebug() << this << "Deleting empty room" << roomId;
        rooms.remove(roomId);
        delete room;
    }
}


/*! This method handles the request for a user list of a specific room/chat. Since rooms are within
    server-scope, the request is handled here.
*/
void QwsServerController::handleUserlistRequest(const int roomId)
{
    if (!rooms.contains(roomId)) {
        qDebug() << this << "Warning: Invalid room id:" << roomId;
        return;
    }

    QwsClientSocket *user = qobject_cast<QwsClientSocket*>(sender());
    if (!user) {
        qDebug() << this << "Warning: Invalid sender socket. Can not cast.";
        return;
    }

    // Run trough the list of users in the room
    QwRoom *room = rooms[roomId];
    QListIterator<int> i(room->pUsers);
    while (i.hasNext()) {
        int itemId = i.next();
        if (sockets.contains(itemId)) {
            QwsClientSocket *itemSocket = sockets[itemId];
            if (itemSocket && itemSocket->sessionState == Qws::StateActive) {
                // Send a user list item
                QwMessage reply("310");
                reply.appendArg(QByteArray::number(roomId));
                itemSocket->user.userListEntry(reply);
                user->sendMessage(reply);
            }
        }
    }

    // Send end of list
    QwMessage reply("311");
    reply.appendArg(QByteArray::number(roomId));
    user->sendMessage(reply);

}


/*! This method relays a chat mesage to all participants of a chat room. If \a isEmote is true
    the message will be sent as a emote-message (ME).
*/
void QwsServerController::relayChatToRoom(const int roomId, const QString text, const bool isEmote)
{
    if (!rooms.contains(roomId)) {
        qDebug() << this << "Warning: Invalid room id:" << roomId;
        return;
    }

    QwsClientSocket *user = qobject_cast<QwsClientSocket*>(sender());
    if (!user) {
        qDebug() << this << "Warning: Invalid sender socket. Can not cast.";
        return;
    }

    // Run trough the list of users in the room
    QwRoom *room = rooms[roomId];
    QListIterator<int> i(room->pUsers);

    // Prepare the message
    QwMessage reply(isEmote ? "301 " : "300 ");
    reply.appendArg(QByteArray::number(roomId));
    reply.appendArg(QByteArray::number(user->user.pUserID));
    reply.appendArg(text.toUtf8());

    while (i.hasNext()) {
        int itemId = i.next();
        if (sockets.contains(itemId)) {
            QwsClientSocket *itemSocket = sockets[itemId];
            if (itemSocket && itemSocket->sessionState == Qws::StateActive) {
                itemSocket->sendMessage(reply);
            }
        }
    }

    // Send end of list
//    QwMessage reply("311");
//    reply.appendArg(QByteArray::number(roomId));
//    user->sendMessage(reply);
}

/*! This method relays a private message to another user.
*/
void QwsServerController::relayMessageToUser(const int userId, const QString text)
{
    QwsClientSocket *user = qobject_cast<QwsClientSocket*>(sender());
    if (!user) {
        qDebug() << this << "Warning: Invalid sender socket. Can not cast.";
        return;
    }
    
    if (sockets.contains(userId)) {
        QwsClientSocket *targetUser = sockets[userId];
        QwMessage reply("305");
        reply.appendArg(QByteArray::number(userId));
        reply.appendArg(text.toUtf8());
        targetUser->sendMessage(reply);
    } else {
        user->sendError(Qws::ErrorClientNotFound);
    }
}


/*! This method relays a message to other clients in a room. If \a sendToSelf is true, the message
    will also be sent to the sender's client.
*/
void QwsServerController::broadcastMessage(const QwMessage message, const int roomId, const bool sendToSelf)
{
    if (!rooms.contains(roomId)) {
        qDebug() << this << "Warning: Invalid room id:" << roomId;
        return;
    }

    QwsClientSocket *user = qobject_cast<QwsClientSocket*>(sender());
    if (!user) {
        qDebug() << this << "Warning: Invalid sender socket. Can not cast.";
        return;
    }

    // Run trough the list of users in the room
    QwRoom *room = rooms[roomId];
    QListIterator<int> i(room->pUsers);
    while (i.hasNext()) {
        int itemId = i.next();
        if (!sockets.contains(itemId)) { continue; }
        QwsClientSocket *itemSocket = sockets[itemId];
        if (itemSocket && itemSocket->sessionState == Qws::StateActive) {
            if (sendToSelf || itemSocket != user) {
                itemSocket->sendMessage(message);
            }
        }
    }
}


/*! This method changes the topic of a room and notifies all clients within that room about it.
*/
void QwsServerController::changeRoomTopic(const int roomId, const QString topic)
{
    QwsClientSocket *user = qobject_cast<QwsClientSocket*>(sender());
    if (!user) {
        return;
    }
    if (!rooms.contains(roomId)) {
        user->sendError(Qws::ErrorComandFailed);
        return;
    }
    QwRoom *room = rooms[roomId];
    if (!room->pUsers.contains(user->user.pUserID)) {
        user->sendError(Qws::ErrorComandFailed);
        return;
    }
    // Update the room
    room->pTopic = topic;
    room->pTopicDate = QDateTime::currentDateTime().toTimeSpec(Qt::UTC);
    room->pTopicSetter = user->user;
    // Notify everyone in the same room
    QwMessage reply("341");
    reply.appendArg(QString::number(roomId));
    reply.appendArg(room->pTopicSetter.userNickname);
    reply.appendArg(room->pTopicSetter.name);
    reply.appendArg(room->pTopicSetter.userIpAddress);
    reply.appendArg(room->pTopicDate.toString(Qt::ISODate)+"+00:00");
    reply.appendArg(room->pTopic);
    broadcastMessage(reply, roomId, true);
}


/*! This method sends the room topic of the room identified by \a roomId back to the user.
*/
void QwsServerController::sendRoomTopic(const int roomId)
{
    QwsClientSocket *user = qobject_cast<QwsClientSocket*>(sender());
    if (!user) { return; }
    if (!rooms.contains(roomId)) {
        user->sendError(Qws::ErrorComandFailed);
        return;
    }
    QwRoom *room = rooms[roomId];
    if (!room->pUsers.contains(user->user.pUserID)) {
        user->sendError(Qws::ErrorComandFailed);
        return;
    }

    // Send the information
    QwMessage reply("341");
    reply.appendArg(QString::number(roomId));
    reply.appendArg(room->pTopicSetter.userNickname);
    reply.appendArg(room->pTopicSetter.name);
    reply.appendArg(room->pTopicSetter.userIpAddress);
    reply.appendArg(room->pTopicDate.toString(Qt::ISODate)+"+00:00");
    reply.appendArg(room->pTopic);
    user->sendMessage(reply);
}


/*! This method creates a new private chat room and responds to the client with a confirmation and
    the ID of the newly created room.
*/
void QwsServerController::createNewRoom()
{
    QwsClientSocket *user = qobject_cast<QwsClientSocket*>(sender());
    if (!user) { return; }

    // Create a new room
    QwRoom *newRoom = new QwRoom;
    newRoom->pTopic = tr("%1's Private Chat").arg(user->user.userNickname);
    newRoom->pTopicDate = QDateTime::currentDateTime();
    newRoom->pTopicSetter = user->user;
    newRoom->pUsers.append(user->user.pUserID);
    newRoom->pChatId = ++roomIdCounter;
    rooms[newRoom->pChatId] = newRoom;

    // Send the information back to the client
    QwMessage reply("330");
    reply.appendArg(QString::number(newRoom->pChatId));
    user->sendMessage(reply);

    qDebug() << this << "Created new room with id" << newRoom->pChatId;
}


/*! This method invites a user to a room and notifies him of that.
*/
void QwsServerController::inviteUserToRoom(const int userId, const int roomId)
{
    QwsClientSocket *user = qobject_cast<QwsClientSocket*>(sender());
    if (!user) { return; }

    if (userId == user->user.pUserID) {
        qDebug() << this << "User tried to invite himself to room" << roomId;
        user->sendError(Qws::ErrorComandFailed);
        return;
    }

    if (!rooms.contains(roomId)) {
        user->sendError(Qws::ErrorComandFailed);
        qDebug() << this << "Room with id" << roomId << "does not exist!";
        return;
    }
    if (!sockets.contains(userId)) {
        user->sendError(Qws::ErrorClientNotFound);
        qDebug() << this << "Client with id" << roomId << "does not exist!";
        return;
    }
    QwsClientSocket *targetUser = sockets[userId];
    QwRoom *room = rooms[roomId];
    if (!room->pUsers.contains(user->user.pUserID)) {
        user->sendError(Qws::ErrorPermissionDenied);
        qDebug() << this << "User with id"<< user->user.pUserID<<"was not in room"<<roomId;
        return;
    }
    if (room->pInvitedUsers.contains(userId)) {
        // Ignoring a double-invite here.
        user->sendError(Qws::ErrorComandFailed);
        qDebug() << this << "User with id"<<userId<<"was already in list of invited of room" << roomId;
        return;
    }
    room->pInvitedUsers.append(userId);

    // Notify users in the same room about the invitation
    QwMessage reply2("301");
    reply2.appendArg(QString::number(roomId));
    reply2.appendArg(QString::number(user->user.pUserID));
    reply2.appendArg(QString("invited %1 to join this room... [server]")
                     .arg(sockets[userId]->user.userNickname));
    broadcastMessage(reply2, roomId, true);

    // Send the invitation to the other user.
    QwMessage reply("331"); // 331 Private Chat Invitiation
    reply.appendArg(QString::number(roomId));
    reply.appendArg(QString::number(user->user.pUserID));
    targetUser->sendMessage(reply);
}


/*! This method adds a user to a room after a JOIN command.
*/
void QwsServerController::handleMessageJOIN(const int roomId)
{
    QwsClientSocket *user = qobject_cast<QwsClientSocket*>(sender());
    if (!user) { return; }
    if (!rooms.contains(roomId)) {
        qDebug() << this << "Unable to join channel"<<roomId<<" (does not exist)";
        user->sendError(Qws::ErrorComandFailed);
        return;
    }

    QwRoom *room = rooms[roomId];
    if (!room->pInvitedUsers.contains(user->user.pUserID)) {
        qDebug() << this << "Unable to join channel"<<roomId<<" (user was not invited)";
        user->sendError(Qws::ErrorComandFailed);
        return;
    }
    room->pInvitedUsers.removeAll(user->user.pUserID);
    room->pUsers.append(user->user.pUserID);

    // Send the information back to the client
    QwMessage reply("302"); // 302 Client Join
    reply.appendArg(QString::number(roomId));
    user->user.userListEntry(reply);
    broadcastMessage(reply, roomId, false);

    // Send the client the chat topic
    reply = QwMessage("341");
    reply.appendArg(QString::number(room->pChatId));
    reply.appendArg(room->pTopicSetter.userNickname);
    reply.appendArg(room->pTopicSetter.name);
    reply.appendArg(room->pTopicSetter.userIpAddress);
    reply.appendArg(room->pTopicDate.toTimeSpec(Qt::UTC).toString(Qt::ISODate)+"+00:00");
    reply.appendArg(room->pTopic);
    user->sendMessage(reply);

}


/*! This method adds a user to a room after a JOIN command.
*/
void QwsServerController::handleMessageDECLINE(const int roomId)
{
    QwsClientSocket *user = qobject_cast<QwsClientSocket*>(sender());
    if (!user) { return; }
    if (!rooms.contains(roomId)) {
        qDebug() << this << "Unable to decline invitation"<<roomId<<" (does not exist)";
        user->sendError(Qws::ErrorComandFailed);
        return;
    }

    QwRoom *room = rooms[roomId];
    if (!room->pInvitedUsers.contains(user->user.pUserID)) {
        qDebug() << this << "Unable to room invitation"<<roomId<<" (user was never invited)";
        user->sendError(Qws::ErrorComandFailed);
        return;
    }
    room->pInvitedUsers.removeAll(user->user.pUserID);

    // Notify the other users that this user declined.
    QwMessage reply("332");
    reply.appendArg(QString::number(roomId));
    reply.appendArg(QString::number(user->user.pUserID));
    broadcastMessage(reply, roomId, true);
}


/*! This method removes a user to a room after a LEAVE command.
*/
void QwsServerController::handleMessageLEAVE(const int roomId)
{
    QwsClientSocket *user = qobject_cast<QwsClientSocket*>(sender());
    if (!user) { return; }
    if (!rooms.contains(roomId)) {
        qDebug() << this << "Unable to part user from room"<<roomId<<" (does not exist)";
        user->sendError(Qws::ErrorComandFailed);
        return;
    }
    QwRoom *room = rooms[roomId];
    if (!room->pUsers.contains(user->user.pUserID)) {
        qDebug() << this << "Unable to part user from room"<<roomId<<" (user was not in room)";
        user->sendError(Qws::ErrorComandFailed);
        return;
    }
    removeUserFromRoom(roomId, user->user.pUserID);
}


/*! This method is called when a user needs to be kicked/banned from the server by an administrator.
*/
void QwsServerController::handleMessageBAN_KICK(const int userId, const QString reason, const bool isBan)
{
    qDebug() << this << "Handling a BAN/KICK for user"<<userId<<"ban="<<isBan;
    QwsClientSocket *user = qobject_cast<QwsClientSocket*>(sender());
    if (!user) { return; }
    if (!sockets.contains(userId)) {
        qDebug() << this << "Unable to kick/ban user"<<userId<<" (does not exist)";
        user->sendError(Qws::ErrorClientNotFound);
        return;
    }
    QwsClientSocket *targetUser = sockets[userId];
    if (targetUser->user.privCannotBeKicked) {
        qDebug() << this << "Unable to kick user - protected!";
        user->sendError(Qws::ErrorCannotBeDisconnected);
        return;
    }

    // Notify the other clients about this
    QwMessage reply(isBan ? "307" : "306");
    reply.appendArg(QString::number(userId));
    reply.appendArg(QString::number(user->user.pUserID));
    reply.appendArg(reason);
    broadcastMessage(reply, 1, true);

    // Now kill him
    targetUser->disconnectClient();
}


/*! This method is called when a user needs to be kicked/banned from the server by an administrator.
*/
void QwsServerController::handleModifiedUserAccount(const QString name)
{
    QHashIterator<int,QwsClientSocket*> i(sockets);
    while (i.hasNext()) {
        i.next();
        QwsClientSocket *item = i.value();
        if (!item) continue;
        if (item->user.name == name) {
            // Reload the account
            item->user.loadFromDatabase();

            // Send new privileges
            QwMessage reply("602");
            item->user.appendPrivilegeFlagsForREADUSER(reply);
            item->sendMessage(reply);

            // Broadcast a change
            reply = QwMessage("304");
            item->user.userStatusEntry(reply);
            broadcastMessage(reply, 1, true);
        }
    }


}


/*! This method is called when a user needs to be kicked/banned from the server by an administrator.
*/
void QwsServerController::handleModifiedUserGroup(const QString name)
{
    QHashIterator<int,QwsClientSocket*> i(sockets);
    while (i.hasNext()) {
        i.next();
        QwsClientSocket *item = i.value();
        if (!item) continue;
        if (item->user.pGroupName == name) {
            // Reload the account
            item->user.loadFromDatabase();

            // Send new privileges
            QwMessage reply("602");
            item->user.appendPrivilegeFlagsForREADUSER(reply);
            item->sendMessage(reply);

            // Broadcast a change
            reply = QwMessage("304");
            item->user.userStatusEntry(reply);
            broadcastMessage(reply, 1, true);
        }
    }

}
