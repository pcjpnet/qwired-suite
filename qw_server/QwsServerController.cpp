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
#include "wiredsocket.h"

#include <QtSql>

QwsServerController::QwsServerController(QObject *parent) : QObject(parent)
{
    pCore = new QwsConnection(this);
    sessionIdCounter = 0;
    roomIdCounter = 10;

    // Initialize the main room (public chat, server user list)
    QwsRoom *publicRoom = new QwsRoom;
    publicRoom->pChatId = 1;
    publicRoom->pTopic = "Welcome to Qwired Server!";
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
    WiredSocket *clientSocket = new WiredSocket(this);
    clientSocket->user.pUserID = ++sessionIdCounter;
    pCore->registerClient(clientSocket);
    clientSocket->setSslSocket(pTcpServer->nextPendingSslSocket());

    connect(clientSocket, SIGNAL(requestedUserlist(int)),
            this, SLOT(handleUserlistRequest(int)), Qt::QueuedConnection);
    connect(clientSocket, SIGNAL(disconnected()),
            this, SLOT(handleSocketDisconnected()), Qt::QueuedConnection);
    connect(clientSocket, SIGNAL(sessionStateChanged(Qws::SessionState)),
            this, SLOT(handleSocketSessionStateChanged(Qws::SessionState)), Qt::QueuedConnection);
    connect(clientSocket, SIGNAL(requestedChatRelay(int,QString,bool)),
            this, SLOT(relayChatToRoom(int,QString,bool)), Qt::QueuedConnection);
    connect(clientSocket, SIGNAL(requestedMessageRelay(int,QString)),
            this, SLOT(relayMessageToUser(int,QString)), Qt::QueuedConnection);
    connect(clientSocket, SIGNAL(broadcastedMessage(QwMessage,int)),
            this, SLOT(broadcastMessage(QwMessage,int,bool)), Qt::QueuedConnection);
    connect(clientSocket, SIGNAL(userStatusChanged()),
            this, SLOT(relayUserStatusChanged()), Qt::QueuedConnection);
    connect(clientSocket, SIGNAL(requestedRoomTopicChange(int,QString)),
            this, SLOT(changeRoomTopic(int,QString)), Qt::QueuedConnection);
    connect(clientSocket, SIGNAL(requestedRoomTopic(int)),
            this, SLOT(sendRoomTopic(int)), Qt::QueuedConnection);
    connect(clientSocket, SIGNAL(requestedNewRoom()),
            this, SLOT(createNewRoom()), Qt::QueuedConnection);
    connect(clientSocket, SIGNAL(requestedUserInviteToRoom(int,int)),
            this, SLOT(inviteUserToRoom(int,int)), Qt::QueuedConnection);
    connect(clientSocket, SIGNAL(requestedUserJoinRoom(int)),
            this, SLOT(joinUserToRoom(int)), Qt::QueuedConnection);


    sockets[clientSocket->user.pUserID] = clientSocket;
}


/*! This slot is called whenever a client disconnects. Here we clean up behind the client and
    make sure everyone knows about it.
*/
void QwsServerController::handleSocketDisconnected()
{
    WiredSocket *client = qobject_cast<WiredSocket*>(sender());
    if (!client) { return; }
    qDebug() << this << "Sending all clients the client-left event.";

    // Remove the user from the chat rooms
    QHashIterator<int, QwsRoom*> i(rooms);
    i.toBack();
    while (i.hasPrevious()) {
        QwsRoom *itemRoom = i.value();
        if (itemRoom && itemRoom->pUsers.contains(client->user.pUserID)) {
            removeUserFromRoom(i.key(), client->user.pUserID);
        }
    }

    // Remove the client from the list of clients
    WiredSocket *deadClient = sockets.take(client->user.pUserID);
    deadClient->deleteLater();
}


/*! This is called whenever the state of a client changes.
*/
void QwsServerController::handleSocketSessionStateChanged(const Qws::SessionState state)
{
    WiredSocket *client = qobject_cast<WiredSocket*>(sender());
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
    WiredSocket *client = qobject_cast<WiredSocket*>(sender());
    if (!client) { return; }
    QwMessage reply("304");
    client->user.userListEntry(reply);
    broadcastMessage(reply, 1, true);
}


/*! This method is called when a client requests information about a client.
*/
void QwsServerController::sendClientInformation(const int userId)
{
    WiredSocket *user = sockets[userId];
    if (!user) { return; }
    if (!sockets.contains(userId)) {
        user->sendError(Qws::ErrorClientNotFound);
        return;
    }
    WiredSocket *target = sockets[userId];
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
    if (!rooms.contains(roomId) || !sockets.contains(userId)) {
        return;
    }

    QwsRoom *room = rooms[roomId];
    WiredSocket *user = sockets[userId];

    QListIterator<int> i(room->pUsers);
    while (i.hasNext()) {
        int itemId = i.next();
        WiredSocket *itemSocket = sockets[itemId];
        if (itemSocket && itemSocket->sessionState == Qws::StateActive) {
            itemSocket->sendClientJoin(roomId, user->user);
        }
    }

    room->pUsers.append(userId);
}

/*! This method removes a user from the a room and notifies all other users in the same room about
    this change, respectively sending out "client left" messages.
*/
void QwsServerController::removeUserFromRoom(const int roomId, const int userId)
{
    QwsRoom *room = rooms[roomId];
    WiredSocket *user = sockets[userId];
    room->pUsers.removeOne(userId);
    QListIterator<int> i(room->pUsers);
    while (i.hasNext()) {
        int itemId = i.next();
        WiredSocket *itemSocket = sockets[itemId];
        if (itemSocket && itemSocket->sessionState == Qws::StateActive) {
            itemSocket->sendClientLeave(roomId, user->user.pUserID);
        }
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

    WiredSocket *user = qobject_cast<WiredSocket*>(sender());
    if (!user) {
        qDebug() << this << "Warning: Invalid sender socket. Can not cast.";
        return;
    }

    // Run trough the list of users in the room
    QwsRoom *room = rooms[roomId];
    QListIterator<int> i(room->pUsers);
    while (i.hasNext()) {
        int itemId = i.next();
        if (sockets.contains(itemId)) {
            WiredSocket *itemSocket = sockets[itemId];
            if (itemSocket && itemSocket->sessionState == Qws::StateActive) {
                user->sendUserlistItem(roomId, itemSocket->user);
            }
        }
    }
    user->sendUserlistDone(roomId);
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

    WiredSocket *user = qobject_cast<WiredSocket*>(sender());
    if (!user) {
        qDebug() << this << "Warning: Invalid sender socket. Can not cast.";
        return;
    }

    // Run trough the list of users in the room
    QwsRoom *room = rooms[roomId];
    QListIterator<int> i(room->pUsers);
    while (i.hasNext()) {
        int itemId = i.next();
        if (sockets.contains(itemId)) {
            WiredSocket *itemSocket = sockets[itemId];
            if (itemSocket && itemSocket->sessionState == Qws::StateActive) {
                itemSocket->sendChat(roomId, user->user.pUserID, text, isEmote);
            }
        }
    }
    user->sendUserlistDone(roomId);
}

/*! This method relays a private message to another user.
*/
void QwsServerController::relayMessageToUser(const int userId, const QString text)
{
    WiredSocket *user = qobject_cast<WiredSocket*>(sender());
    if (!user) {
        qDebug() << this << "Warning: Invalid sender socket. Can not cast.";
        return;
    }
    
    if (sockets.contains(userId)) {
        WiredSocket *targetUser = sockets[userId];
        targetUser->sendPrivateMessage(user->user.pUserID, text);
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

    WiredSocket *user = qobject_cast<WiredSocket*>(sender());
    if (!user) {
        qDebug() << this << "Warning: Invalid sender socket. Can not cast.";
        return;
    }

    // Run trough the list of users in the room
    QwsRoom *room = rooms[roomId];
    QListIterator<int> i(room->pUsers);
    while (i.hasNext()) {
        int itemId = i.next();
        if (!sockets.contains(itemId)) { continue; }
        WiredSocket *itemSocket = sockets[itemId];
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
    WiredSocket *user = qobject_cast<WiredSocket*>(sender());
    if (!user) {
        return;
    }
    if (!rooms.contains(roomId)) {
        user->sendError(Qws::ErrorComandFailed);
        return;
    }
    QwsRoom *room = rooms[roomId];
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
    reply.appendArg(room->pTopicSetter.userLogin);
    reply.appendArg(room->pTopicSetter.userIpAddress);
    reply.appendArg(room->pTopicDate.toString(Qt::ISODate)+"+00:00");
    reply.appendArg(room->pTopic);
    broadcastMessage(reply, roomId, true);
}


/*! This method sends the room topic of the room identified by \a roomId back to the user.
*/
void QwsServerController::sendRoomTopic(const int roomId)
{
    WiredSocket *user = qobject_cast<WiredSocket*>(sender());
    if (!user) { return; }
    if (!rooms.contains(roomId)) {
        user->sendError(Qws::ErrorComandFailed);
        return;
    }
    QwsRoom *room = rooms[roomId];
    if (!room->pUsers.contains(user->user.pUserID)) {
        user->sendError(Qws::ErrorComandFailed);
        return;
    }

    // Send the information
    QwMessage reply("341");
    reply.appendArg(QString::number(roomId));
    reply.appendArg(room->pTopicSetter.userNickname);
    reply.appendArg(room->pTopicSetter.userLogin);
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
    WiredSocket *user = qobject_cast<WiredSocket*>(sender());
    if (!user) { return; }

    // Create a new room
    QwsRoom *newRoom = new QwsRoom;
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
}


/*! This method invites a user to a room and notifies him of that.
*/
void QwsServerController::inviteUserToRoom(const int userId, const int roomId)
{
    WiredSocket *user = qobject_cast<WiredSocket*>(sender());
    if (!user) { return; }
    if (!rooms.contains(roomId)) {
        user->sendError(Qws::ErrorComandFailed);
        return;
    }
    if (!sockets.contains(userId)) {
        user->sendError(Qws::ErrorClientNotFound);
        return;
    }
    WiredSocket *targetUser = sockets[userId];
    QwsRoom *room = rooms[roomId];
    if (!room->pUsers.contains(userId)) {
        user->sendError(Qws::ErrorPermissionDenied);
        return;
    }
    if (room->pInvitedUsers.contains(userId)) {
        // Ignoring a double-invite here.
        user->sendError(Qws::ErrorComandFailed);
        return;
    }
    room->pInvitedUsers.append(userId);
    // Send the information back to the client
    QwMessage reply("331");
    reply.appendArg(QString::number(roomId));
    reply.appendArg(QString::number(user->user.pUserID));
    targetUser->sendMessage(reply);
}


/*! This method adds a user to a room after a JOIN command.
*/
void QwsServerController::joinUserToRoom(const int roomId)
{
    WiredSocket *user = qobject_cast<WiredSocket*>(sender());
    if (!user) { return; }
    if (!rooms.contains(roomId)) {
        user->sendError(Qws::ErrorComandFailed);
        return;
    }
    QwsRoom *room = rooms[roomId];
    if (!room->pInvitedUsers.contains(user->user.pUserID)) {
        user->sendError(Qws::ErrorPermissionDenied);
        return;
    }
    room->pInvitedUsers.removeAll(user->user.pUserID);
    room->pUsers.append(user->user.pUserID);

    // Send the information back to the client
    QwMessage reply("302");
    reply.appendArg(QString::number(roomId));
    user->user.userListEntry(reply);
    broadcastMessage(reply, roomId, false);
}


/**
 * Load the database from the disk.
 */
/*
void QwsServerController::reloadDatabase() {
	QStringList tmpCmdArgs = QCoreApplication::arguments();
	qwLog("Loading database...");
	QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
	db.setDatabaseName("accounts.db");
	if(!db.open()) {
		qwLog(QString("Fatal: Could not open accounts.db. Reason: %1").arg(QSqlError(db.lastError()).text()));
		exit(1);
	} else {
		pCore->pDatabase = db;

		QSqlQuery query;
		QStringList tmpTables = db.tables();
		if(!tmpTables.contains("qw_news")) {
			qDebug() << "[controller] Creating missing qw_new table.";
			query.exec("CREATE TABLE qw_news (id INTEGER PRIMARY KEY, message TEXT, date TEXT, user TEXT, deleted INTEGER DEFAULT 0);");
		}

		if(!tmpTables.contains("qw_accounts")) {
			qDebug() << "[controller] Creating missing qw_accounts table.";
			query.exec("CREATE TABLE qw_accounts (id INTEGER PRIMARY KEY, login TEXT, password TEXT, groupname TEXT, privileges TEXT);");
			query.exec("INSERT INTO qw_accounts (login,password,groupname,privileges) VALUES ('admin', '', 'admins', '1:1:1:1:1:1:1:1:1:1:1:1:1:1:1:1:1:0:0:0:0:0:1');");
			query.exec("INSERT INTO qw_accounts (login,password,groupname,privileges) VALUES ('guest', '', '', '0:0:0:0:1:1:0:1:0:0:0:0:0:0:0:0:0:0:0:0:0:0:0');");
		}

		// Import accounts from older configuration file
		if(int index=tmpCmdArgs.indexOf("-iu") > -1) {
			QFile tmpFile(tmpCmdArgs.value(index+1));
			if(tmpFile.open(QIODevice::ReadOnly)) {
				qwLog("Importing users from old-world file...");
				query.exec("DELETE FROM qw_accounts;");
				while(!tmpFile.atEnd()) {
					QString tmpLine = tmpFile.readLine();
					if(tmpLine.startsWith("#") || tmpLine.trimmed().isEmpty()) continue;
					QStringList tmpRec = tmpLine.split(":");
					query.prepare("INSERT INTO qw_accounts (login,password,groupname,privileges) VALUES (:login, :password, :groupname, :privileges);");
					query.bindValue(":login", tmpLine.section(":",0,0));
					query.bindValue(":password", tmpLine.section(":",1,1));
					query.bindValue(":groupname", tmpLine.section(":",2,2));
					query.bindValue(":privileges", tmpLine.section(":",3,-1).trimmed());
					if(!query.exec()) qwLog(QString("SQL error: %1").arg(query.lastError().text()));
					qwLog(QString("  Imported user %1").arg(tmpLine.section(":",0,0).trimmed()));
				}
			} else {
				qwLog(QString("Error: Could not open old-world users file: %1").arg(tmpFile.errorString()));
			}
			exit(0);
		}
		

		if(!tmpTables.contains("qw_groups")) {
			qDebug() << "[controller] Creating missing qw_groups table.";
			query.exec("CREATE TABLE qw_groups (id INTEGER PRIMARY KEY, groupname TEXT, privileges TEXT);");
			query.exec("INSERT INTO qw_groups (groupname,privileges) VALUES ('admins', '1:1:1:1:1:1:1:1:1:1:1:1:1:1:1:1:1:0:0:0:0:0:1');");
		}

		// Import accounts from older configuration file
		if(int index=tmpCmdArgs.indexOf("-ig") > -1) {
			QFile tmpFile(tmpCmdArgs.value(index+1));
			if(tmpFile.open(QIODevice::ReadOnly)) {
				qwLog("Importing groups from old-world file...");
				query.exec("DELETE FROM qw_groups;");
				while(!tmpFile.atEnd()) {
					QString tmpLine = tmpFile.readLine();
					if(tmpLine.startsWith("#") || tmpLine.trimmed().isEmpty()) continue;
					QStringList tmpRec = tmpLine.split(":");
					query.prepare("INSERT INTO qw_groups (groupname,privileges) VALUES (:groupname, :privileges);");
					query.bindValue(":groupname", tmpLine.section(":",0,0));
					query.bindValue(":privileges", tmpLine.section(":",1,-1).trimmed());
					if(!query.exec()) qwLog(QString("SQL error: %1").arg(query.lastError().text()));
					qwLog(QString("  Imported group %1").arg(tmpLine.section(":",0,0)));
				}
			} else {
				qwLog(QString("Error: Could not open old-world group file: %1").arg(tmpFile.errorString()));
			}
			exit(0);
		}
		
		qDebug() << "Tables:"<<db.tables();
	}
}

*/
