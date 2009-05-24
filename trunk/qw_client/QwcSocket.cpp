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
    clientName = "Qwired SVN";
    clientSoftwareVersion = QWIRED_VERSION;
    pIzCaturday = false;

    pIndexingFiles = false;
    pInvitedUserID = 0;

}


/*! A transfer socket has finished. Remove the socket from the list of sockets.
*/
void QwcSocket::handleTransferDone(const QwcTransferInfo &transfer)
{
    QMutableListIterator<QwcTransferSocket*> i(transferSockets);
    while (i.hasNext()) {
        QwcTransferSocket *item = i.next();
        if (item->transferInfo.hash == transfer.hash) {
            qDebug() << this << "Removing completed transfer socket.";
            i.remove();
            emit fileTransferDone(transfer);
            item->deleteLater();
            checkTransferQueue();
            break;
        }
    }
}


/*! A transfer socket has failed. Remove the socket from the list of sockets.
*/
void QwcSocket::handleTransferError(const QwcTransferInfo &transfer)
{
    QMutableListIterator<QwcTransferSocket*> i(transferSockets);
    while (i.hasNext()) {
        QwcTransferSocket *item = i.next();
        if (item->transferInfo.hash == transfer.hash) {
            qDebug() << this << "Removing failed transfer socket.";
            i.remove();
            emit fileTransferError(transfer);
            item->deleteLater();
            break;
        }
    }
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
    } else if (commandId == 308) {    handleMessage308(message); // Client Information
    } else if (commandId == 310) {    handleMessage310(message); // User List
    } else if (commandId == 311) {    handleMessage311(message); // User List Done
    } else if (commandId == 320) {    handleMessage320(message); // News
    } else if (commandId == 321) {    handleMessage321(message); // News Done
    } else if (commandId == 330) {    handleMessage330(message); // Private Chat Created
    } else if (commandId == 331) {    handleMessage331(message); // Private Chat Invitation
    } else if (commandId == 340) {    handleMessage340(message); // Client Image Change
    } else if (commandId == 341) {    handleMessage341(message); // Chat Topic
    } else if (commandId == 400) {    handleMessage400(message); // Transfer Ready
    } else if (commandId == 401) {    handleMessage401(message); // Transfer Queued
    } else if (commandId == 402) {    handleMessage402(message); // File Information
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
    setNickname(sessionUser.userNickname);
    setIconImage(sessionUser.userImage);
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
    serverImage.loadFromData(QByteArray::fromBase64(message.getStringArgument(0).toAscii()));
    emit onServerBanner(serverImage);
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
    if (roomId == 1) {
        users[newUser.pUserID] = newUser;
        rooms[1].pUsers.append(newUser.pUserID);
    } else {
        if (!rooms.contains(roomId)) { return; }
        rooms[roomId].pUsers.append(newUser.pUserID);
    }
    emit userJoinedRoom(roomId, newUser);
}


/*! 303 Client Leave
    A client has left a room.
*/
void QwcSocket::handleMessage303(const QwMessage &message)
{
    int roomId = message.getStringArgument(0).toInt();
    int userId = message.getStringArgument(1).toInt();
    QwcUserInfo targetUser = users.value(userId);
    if (!rooms.contains(roomId)) { return; }
    if (roomId == 1) {
        // If the client leaves room 1, it has disconnected and should be removed from all rooms.
        QMutableHashIterator<int, QwRoom> i(rooms);
        while (i.hasNext()) {
            i.next();
            i.value().pUsers.removeAll(userId);
            emit userLeftRoom(i.key(), targetUser);
        }
    } else {
        // Remove the user from a single room
        QwRoom &targetRoom = rooms[roomId];
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
    int userId = message.getStringArgument(0).toInt();
    if (!users.contains(userId)) { return; }
    QwcUserInfo targetUserOld = users[userId];
    QwcUserInfo &targetUser = users[userId];
    targetUser.pIdle = message.getStringArgument(1).toInt();
    targetUser.pAdmin = message.getStringArgument(2).toInt();
    targetUser.pIcon = message.getStringArgument(3).toInt();
    targetUser.userNickname = message.getStringArgument(4);
    targetUser.userStatus = message.getStringArgument(5);
    emit userChanged(targetUserOld, targetUser);
}


/*! 305 Private Message
    Another client has sent a private message.
*/
void QwcSocket::handleMessage305(const QwMessage &message)
{
    QwcUserInfo senderUser = users[message.getStringArgument(0).toInt()];
    emit onPrivateMessage(senderUser, message.getStringArgument(1) );
}


/*! 306 Client Kicked
    A client was kicked by an administrator.
*/
void QwcSocket::handleMessage306(const QwMessage &message)
{
    QwcUserInfo victim = users[message.getStringArgument(0).toInt()];
    QwcUserInfo killer = users[message.getStringArgument(1).toInt()];
    // User message handler for 303 Client Left to remove the user
    handleMessage303(QwMessage().appendArg("0").appendArg(QString::number(victim.pUserID)));
    emit userKicked(victim, killer, message.getStringArgument(2));
}


/*! 307 Client Banned
    A client was banned by an administrator.
*/
void QwcSocket::handleMessage307(const QwMessage &message)
{
    QwcUserInfo victim = users[message.getStringArgument(0).toInt()];
    QwcUserInfo killer = users[message.getStringArgument(1).toInt()];
    // User message handler for 303 Client Left to remove the user
    handleMessage303(QwMessage().appendArg("0").appendArg(QString::number(victim.pUserID)));
    emit userBanned(victim, killer, message.getStringArgument(2));
}


/*! 308 Client Information
    Client information, in response to INFO.
*/
void QwcSocket::handleMessage308(const QwMessage &message)
{
    emit userInformation(QwcUserInfo::fromMessage308(message));
}


/*! 309 Broadcast Message
    A administrator sent a broadcast message.
*/
void QwcSocket::handleMessage309(const QwMessage &message)
{
    QwcUserInfo senderUser = users[message.getStringArgument(0).toInt()];
    emit broadcastMessage(senderUser, message.getStringArgument(1));
}


/*! 310 User List
    One item of the user list of a specific room. In response to WHO.
*/
void QwcSocket::handleMessage310(const QwMessage &message)
{
    int roomId = message.getStringArgument(0).toInt();
    QwcUserInfo targetUser = QwcUserInfo::fromMessage310(message);
    if (roomId == 1) {
        users[targetUser.pUserID] = targetUser;
    }
    rooms[roomId].pUsers.append(targetUser.pUserID);
}


/*! 311 User List Done
    Received after a list of 310 messages and indicates the end of the list.
*/
void QwcSocket::handleMessage311(const QwMessage &message)
{
    int tmpChannel = message.getStringArgument(0).toInt();
    emit receivedUserlist(tmpChannel);
}


/*! 320 News
    Received after a NEWS request.
    \todo Qt's ISODate parser does not support the timezone offsets. This needs to be worked around
          in the future.
*/
void QwcSocket::handleMessage320(const QwMessage &message)
{
    qDebug() << message.getStringArgument(1);
    QDateTime date = QDateTime::fromString(message.getStringArgument(1), Qt::ISODate);
    emit newsListingItem(message.getStringArgument(0), date, message.getStringArgument(2));
}


/*! \brief 321 News Done
    Received after a list of 320 News messages.
*/
void QwcSocket::handleMessage321(const QwMessage &message)
{
    Q_UNUSED(message);
    emit newsListingDone();
}


/*! \brief 322 News Posted
    A user has posted a news item.
*/
void QwcSocket::handleMessage322(const QwMessage &message)
{
    QDateTime date = QDateTime::fromString(message.getStringArgument(1), Qt::ISODate);
    emit newsPosted(message.getStringArgument(0), date, message.getStringArgument(2));
}


/*! 330 Private Chat Created
    A new private chat was created on the server.
*/
void QwcSocket::handleMessage330(const QwMessage &message)
{
    int roomId = message.getStringArgument(0).toInt();
    QwRoom newRoom;
    newRoom.pChatId = roomId;
    newRoom.pUsers.append(sessionUser.pUserID);
    rooms[roomId] = newRoom;
    emit privateChatCreated(roomId);
    if (pInvitedUserID > 0) {
        inviteClientToChat(roomId, pInvitedUserID);
        pInvitedUserID = 0;
    }
}


/*! 331 Private Chat Invitation
    User was invited to a private chat..
*/
void QwcSocket::handleMessage331(const QwMessage &message)
{
    QwcUserInfo senderUser = users[message.getStringArgument(1).toInt()];
    emit privateChatInvitation(message.getStringArgument(0).toInt(), senderUser);
}


/*! 340 Client Image Change
    The image of a client was changed.
*/
void QwcSocket::handleMessage340(const QwMessage &message)
{
    int userId = message.getStringArgument(0).toInt();
    if (!users.contains(userId)) { return; }
    QwcUserInfo targetUserOld = users[userId];
    users[userId].setImageFromData(QByteArray::fromBase64(message.getStringArgument(1).toAscii()));
    emit userChanged(targetUserOld, users[userId]);
}


/*! 341 Chat Topic
    Server informs about the topic of a chat room.
*/
void QwcSocket::handleMessage341(const QwMessage &message)
{
    emit onChatTopic(message.getStringArgument(0).toInt(), // chat id
                     message.getStringArgument(1), // nickname
                     message.getStringArgument(2), // login
                     QHostAddress( message.getStringArgument(3) ), // ip
                     QDateTime::fromString(message.getStringArgument(4), Qt::ISODate), // date-time
                     message.getStringArgument(5)); // topic
}


/*! 400 Transfer Ready
    A transfer is ready and the server is waiting for a transfer connection.
*/
void QwcSocket::handleMessage400(const QwMessage &message)
{
    QString filePath = message.getStringArgument(0);
    qint64 transferOffset = message.getStringArgument(1).toLongLong();
    QString transferHash = message.getStringArgument(2);

    QwcTransferInfo transfer;

    // Find the transfer in the pool and remove it.
    QMutableListIterator<QwcTransferInfo> i(transferPool);
    while (i.hasNext()) {
        QwcTransferInfo &item = i.next();
        if (item.file.path == filePath) {
            transfer = item;
            i.remove();
            break;
        }
    }
    transfer.hash = transferHash;
    transfer.bytesTransferred = transferOffset;

    QwcTransferSocket *transferSocket = new QwcTransferSocket(this);
    connect(transferSocket, SIGNAL(fileTransferDone(QwcTransferInfo)),
            this, SLOT(handleTransferDone(QwcTransferInfo)));
    connect(transferSocket, SIGNAL(fileTransferError(QwcTransferInfo)),
            this, SLOT(handleTransferError(QwcTransferInfo)));
    connect(transferSocket, SIGNAL(fileTransferStatus(QwcTransferInfo)),
            this, SIGNAL(fileTransferStatus(QwcTransferInfo)));
    connect(transferSocket, SIGNAL(fileTransferStarted(QwcTransferInfo)),
            this, SIGNAL(fileTransferStarted(QwcTransferInfo)));

    transferSocket->setServer(serverAddress, serverPort);
    transferSockets.append(transferSocket);
    transferSocket->transferInfo = transfer;
    transferSocket->beginTransfer();
}


/*! 401 Transfer Queued
    A transfer has changed position in the server queue.
*/
void QwcSocket::handleMessage401(const QwMessage &message)
{
    QString filePath = message.getStringArgument(0);
    int queuePosition = message.getStringArgument(1).toInt();
    qDebug() << this << "Got transfer queued update for" << filePath << "position" << queuePosition;

    // Find the transfer in the pool and remove it.
    QMutableListIterator<QwcTransferInfo> i(transferPool);
    while (i.hasNext()) {
        QwcTransferInfo &item = i.next();
        if (item.file.path == filePath) {
            qDebug() << "Updated transfer information in pool.";
            item.queuePosition = queuePosition;
            break;
        }
    }
}


/*! 402 File Information
    The server returned information about a file.
*/
void QwcSocket::handleMessage402(const QwMessage &message)
{
    QwcFileInfo fileInfo;
    fileInfo.setFromMessage402(message);


    // Check if this was in response to a transfer
    QMutableListIterator<QwcTransferInfo> i(transferPool);
    while (i.hasNext()) {
        QwcTransferInfo &item = i.next();
        if (item.type == Qw::TransferTypeDownload
            && item.state == Qw::TransferInfoStateWaiting
            && item.file.path == fileInfo.path) {

            qDebug() << this << "Received response to download STAT";
            fileInfo.localAbsolutePath = item.file.localAbsolutePath;
            item.file = fileInfo;

            // Check if the file already exists locally
            QFileInfo localFile(item.file.localAbsolutePath);
            if (localFile.exists()) {
                item.bytesTransferred = localFile.size();
                qDebug() << this << "Resuming partial file at" << localFile.size();
            }

            // Request a download slot
            sendMessage(QwMessage("GET")
                    .appendArg(item.file.path)
                    .appendArg(QString::number(item.bytesTransferred)));
            return;
        }
    }


    // This was not a response to a download.

    emit fileInformation(fileInfo);

//
//    // Check if we are waiting for a stat
//    QListIterator<QPointer<QwcTransferSocket> > i(pTransferSockets);
//    int tmpIdx=0;
//    while( i.hasNext() ) {
//        QwcTransferSocket *tmpT = i.next();
//        if(!tmpT) continue;
//
//
//        if( (tmpT->pTransfer.pTransferType==WiredTransfer::TypeDownload
//             && tmpT->pTransfer.pStatus==WiredTransfer::StatusWaitingForStat
//             && tmpT->pTransfer.pRemotePath==tmpPath)
//            || (tmpT->pTransfer.pTransferType==WiredTransfer::TypeFolderDownload
//                && tmpT->pTransfer.pFileStatus==WiredTransfer::StatusWaitingForStat
//                && tmpT->pTransfer.pRemotePath==tmpPath) ) {
//
//            qDebug() << "Transfer Phase 1/3:"<<tmpPath<<": received stat, requesting transfer:"<<tmpPath<<tmpChecksum;
//
//            // Check if we need to resume
//            tmpT->pTransfer.pOffset = 0;
//            tmpT->pTransfer.pDoneSize = 0;
//
//            QString tmpFilePath = tmpT->pTransfer.pLocalPath+".WiredTransfer";
//            QFile tmpFile(tmpFilePath);
//            if(tmpFile.exists()) {
//                if(tmpFile.open(QIODevice::ReadOnly)) {
//                    QByteArray tmpDat = tmpFile.read(1024*1024);
//                    QString tmpCS = QCryptographicHash::hash(tmpDat, QCryptographicHash::Sha1).toHex();
//                    qDebug() << "QwcSocket: Download file exists, checksum:"<<tmpCS<<"; server:"<<tmpChecksum;
//                    if(tmpChecksum==tmpCS && tmpFile.size()<tmpSize) {
//                        qDebug() << "QwcSocket: Checksums are identical. Offset is"<<tmpFile.size();
//                        tmpT->pTransfer.pOffset = tmpFile.size();
//                        tmpT->pTransfer.pDoneSize = tmpFile.size();
//                    } else {
//                        qDebug() << "QwcSocket: Checksums are NOT identical, deleting file.";
//                        tmpFile.close();
//                        tmpFile.remove();
//                    }
//                }
//            }
//
//            // Set some more info
//            tmpT->pTransfer.pChecksum = tmpChecksum;
//            tmpT->pTransfer.pTotalSize = tmpSize;
//
//            if(tmpT->pTransfer.pTransferType==WiredTransfer::TypeFolderDownload)
//                tmpT->pTransfer.pFileStatus = WiredTransfer::StatusQueued;
//            else	tmpT->pTransfer.pStatus = WiredTransfer::StatusQueued;
//
//            // Request a file download from the server
//            sendMessage(QwMessage("GET").appendArg(tmpPath)
//                        .appendArg(QByteArray::number(tmpT->pTransfer.pOffset)));
//
//            qDebug() << "QwcSocket: GET'ing the file with offset"<<tmpT->pTransfer.pOffset<<"main status"<<tmpT->pTransfer.pStatus<<"file status"<<tmpT->pTransfer.pFileStatus;
//            return;
//        }
//    }
//    tmpIdx++;
//
//
//    // Not in the transfer queue, pass the event to the user code.
//    QwcFileInfo tmpFile;
//    tmpFile.setFromMessage402(message);
//    emit fileInformation(tmpFile);

}


/*! 410 File Listing
    Received a single entry of the requested list of files.
*/
void QwcSocket::handleMessage410(const QwMessage &message)
{
    QwcFileInfo file;
    file.setFromMessage410(message);
    if(!pIndexingFiles) {
        emit onFilesListItem(file);
    } else {
        file.isIndexed = false;
        pRecursiveFileListing.append(file);
    }
}


/*! 411 File Listing Done
    Received from the server or after PRIVILEGES command.
*/
void QwcSocket::handleMessage411(const QwMessage &message)
{
    Q_UNUSED(message);
//    if(!pIndexingFiles) {
//        emit onFilesListDone(message.getStringArgument(0),
//                             message.getStringArgument(1).toLongLong() );
//    } else {
//        // Listing of directory complete. Continue the listing.
//        int maxIndexingDepth = 16;
//        QMutableListIterator<QwcFileInfo> i(pRecursiveFileListing);
//        while(i.hasNext()) {
//            QwcFileInfo &file = i.next();
//            if(file.type == Qw::FileTypeFolder &&
//               !file.isIndexed && file.size && file.path.count("/") <= maxIndexingDepth ) {
//                file.isIndexed = true;
//                getFileList(file.path);
//                return;
//            }
//        }
//
//        // Find a transfer socket
//        QListIterator<QPointer<QwcTransferSocket> > j(pTransferSockets);
//        while(j.hasNext()) {
//            QwcTransferSocket *tmpT = j.next();
//            if(!tmpT) continue;
//            QwcTransferInfo &transfer = tmpT->pTransfer;
//            if(transfer.pRemotePath==pRecursivePath
//               && transfer.pTransferType==WiredTransfer::TypeFolderDownload
//               && transfer.pStatus==WiredTransfer::StatusWaitingForStat) {
//                qDebug() << this << "Waiting folder transfer socket found.";
//                transfer.fileList = pRecursiveFileListing;
//                transfer.pFilesCount = pRecursiveFileListing.count();
//                transfer.pFilesDone = 0;
//                transfer.pFolderDone = 0;
//                transfer.pFolderSize = 0;
//                QListIterator<QwcFileInfo> k(pRecursiveFileListing);
//                while(k.hasNext()) { transfer.pFolderSize+=k.next().size; }
//
//                transfer.pRemoteFolder = pRecursivePath;
//                transfer.pStatus = WiredTransfer::StatusActive;
//                proceedFolderDownload(tmpT);
//                return;
//            }
//        }
//
//        // Finished, if we are here.
//        pIndexingFiles = false;
//        pRecursivePath.clear();
//
//    }
}


/*! 420 Search Listing
    Received a result from the current search.
*/
void QwcSocket::handleMessage420(const QwMessage &message)
{
    QwcFileInfo tmpFile;
    tmpFile.path = message.getStringArgument(0);
    tmpFile.type = (Qw::FileType)message.getStringArgument(1).toInt();
    tmpFile.size = message.getStringArgument(2).toLongLong();
    tmpFile.created = QDateTime::fromString(message.getStringArgument(3), Qt::ISODate );
    tmpFile.modified = QDateTime::fromString(message.getStringArgument(4), Qt::ISODate );
    if(pSearchResults.count()<1024) {
        pSearchResults.append(tmpFile);
    }

}

/*! 421 Search Listing Done
    Search result list complete.
*/
void QwcSocket::handleMessage421(const QwMessage &message)
{
    Q_UNUSED(message);
    emit fileSearchDone(pSearchResults);
    pSearchResults.clear();
}


/*! 5xx Error Message
    Handle some errors, otherwise pass them to the application code.
*/
void QwcSocket::handleMessage5xx(const int &errorId)
{
    if (errorId == Qw::ErrorLoginFailed) {
        socket->disconnectFromHost();
    } else if (errorId == Qw::ErrorBanned) {
        socket->disconnectFromHost();
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
    user.name = newMessage.getStringArgument(0);
    user.pPassword = newMessage.getStringArgument(1);
    user.pGroupName = newMessage.getStringArgument(2);
    newMessage.arguments.removeFirst();
    newMessage.arguments.removeFirst();
    newMessage.arguments.removeFirst();
    user.setPrivilegesFromMessage602(newMessage);
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
    user.name = newMessage.getStringArgument(0);
    newMessage.arguments.removeFirst();
    user.setPrivilegesFromMessage602(message);
    emit groupSpecReceived(user);
}


/*! 602 Privileges Specification
    Received from the server or after PRIVILEGES command.
*/
void QwcSocket::handleMessage602(const QwMessage &message)
{
    sessionUser.setPrivilegesFromMessage602(message);
    emit receivedUserPrivileges(sessionUser);
}


/*! 610 User Listing
    List of user accounts.
*/
void QwcSocket::handleMessage610(const QwMessage &message)
{
    pAdminUsers.append(message.getStringArgument(0));
}


/*! 611 User Listing Done
    End of user account listing.
*/
void QwcSocket::handleMessage611(const QwMessage &message)
{
    Q_UNUSED(message);
    emit receivedAccountList(pAdminUsers);
    pAdminUsers.clear();
}


/*! 620 Group Listing
    List of user account groups.
*/
void QwcSocket::handleMessage620(const QwMessage &message)
{
    pAdminGroups.append(message.getStringArgument(0));
}


/*! 621 Group Listing Done
    End of user account group listing.
*/
void QwcSocket::handleMessage621(const QwMessage &message)
{
    Q_UNUSED(message);
    emit receivedAccountGroupList(pAdminGroups);
    pAdminGroups.clear();
}


/*! Clean up the socket after disconnecting to be prepared for new connections.
*/
void QwcSocket::disconnectFromServer()
{
    socket->disconnectFromHost();
    pAdminGroups.clear();
    pAdminUsers.clear();
    pInvitedUserID = 0;
    pBuffer.clear();
    users.clear();
    rooms.clear();

    QMutableListIterator<QwcTransferSocket*> i(transferSockets);
    while(i.hasNext()) {
        QwcTransferSocket *p = i.next();
        p->deleteLater();
        i.remove();
    }

}


// Set the username and password for the login sequence.
void QwcSocket::setUserAccount(QString theAcct, QString thePass)
{
    sessionUser.name = theAcct;
    sessionUser.pPassword = thePass;
}


// Do I really have to comment this? :D
void QwcSocket::setCaturday(bool b) {
    // everyday iz caturday!  :3
    if(b) {
        pTranzlator.clear();
        setIconImage( QImage(":/icons/icon_happycat.png") );
        setUserStatus(tr("kittehday nait fevrar"));
        QString tmpNick = sessionUser.userNickname;
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


/*! Attempt to establish a connection to a remote server.
*/
void QwcSocket::connectToWiredServer(QString hostName, int port)
{
    if (hostName.contains(":")) {
        serverAddress = hostName.section(":",0,0);
        serverPort = hostName.section(":",1,1).toInt();
    } else { // No port defined
        serverPort = port;
        serverAddress = hostName;
    }
    qDebug() << this << "Connecting to wired server at" << serverAddress << "port" << serverPort;
    socket->connectToHostEncrypted(serverAddress, serverPort);
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
    socket->disconnectFromHost();
}


QwcSocket::~QwcSocket()
{
}


/*! Send a private message to the user with the id \a userId. If \a userId is 0, the private is sent
    as a broadcast message to all connected users, provided that the broadcast privilege is set.
*/
void QwcSocket::sendPrivateMessage(int userId, QString message)
{
    if (pIzCaturday) {
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
    if (rooms.contains(roomId)) {
        rooms.remove(roomId);
    }
}


/*! Request the creation of a new private chat on the server. The server will respond with the ID
    of the new chat room. The user identified by \a firstInvitedUser is the user that is invited
    right after the new room has been created - you can set this to 0 to just create a new room.
*/
void QwcSocket::createChatWithClient(int firstInvitedUser)
{
    pInvitedUserID = firstInvitedUser;
    sendMessage(QwMessage("PRIVCHAT"));
}


/*! Invite a client to a private chat. \a chatId can be obtained by creating a new chat or inviting
    a client to an existing chat room.
*/
void QwcSocket::inviteClientToChat(int chatId, int userId)
{
    sendMessage(QwMessage("INVITE").appendArg(QString::number(userId)).appendArg(QString::number(chatId)));
}


/*! Clear the news on the server.
*/
void QwcSocket::clearNews()
{
    sendMessage(QwMessage("CLEARNEWS"));
}


/*! Request the list of files at the specified path.
*/
void QwcSocket::getFileList(QString path)
{
    sendMessage(QwMessage("LIST").appendArg(path));
}


/*! Send a new user icon to the remote server and update the local user info.
*/
void QwcSocket::setIconImage(QImage icon)
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
    Q_UNUSED(localPath);
    Q_UNUSED(remotePath);
//    // Check for duplicate downloads
//    QListIterator<QPointer<QwcTransferSocket> > i(pTransferSockets);
//    while(i.hasNext()) {
//        QwcTransferSocket *tmpT = i.next();
//        if(!tmpT) continue;
//        if(tmpT->pTransfer.pRemotePath==remotePath) return;
//    }
//
//    QwcTransferSocket *tmpSock = new QwcTransferSocket;
//    pTransferSockets.append(tmpSock);
//
//    tmpSock->pTransfer.pTransferType = WiredTransfer::TypeFolderDownload;
//    tmpSock->pTransfer.pRemotePath = remotePath;
//    // 	tmpSock->pTransfer.pLocalPath = localPath;
//    tmpSock->pTransfer.pLocalRoot = localPath;
//
//    connect(tmpSock, SIGNAL(fileTransferDone(QwcTransferInfo)), this, SIGNAL(fileTransferDone(QwcTransferInfo)));
//    connect(tmpSock, SIGNAL(fileTransferError(QwcTransferInfo)), this, SIGNAL(fileTransferError(QwcTransferInfo)));
//    connect(tmpSock, SIGNAL(fileTransferStatus(QwcTransferInfo)), this, SIGNAL(fileTransferStatus(QwcTransferInfo)));
//    connect(tmpSock, SIGNAL(fileTransferFileDone(const QwcTransferInfo)), this, SLOT(fileTransferFileDone(const QwcTransferInfo))); // for folder transfers
//    connect(tmpSock, SIGNAL(destroyed()), this, SLOT(cleanTransfers()));
//
//    tmpSock->setServer( socket->peerAddress().toString(), socket->peerPort() );
//    tmpSock->pTransfer.pStatus = WiredTransfer::StatusQueuedLocal;
//    qDebug() << "Transfer Phase 1/3: LocalQueued:"<<remotePath<<"to"<<localPath;
//    emit fileTransferStarted(tmpSock->pTransfer);
}


void QwcSocket::putFolder(const QString localPath, const QString remotePath, const bool queueLocally)
{
    Q_UNUSED(queueLocally);
    Q_UNUSED(remotePath);
    Q_UNUSED(localPath);
//    Q_UNUSED(queueLocally);
//    // Check for duplicate downloads
//    QListIterator<QPointer<QwcTransferSocket> > i(pTransferSockets);
//    while(i.hasNext()) {
//        QwcTransferSocket *tmpT = i.next();
//        if(!tmpT) continue;
//        if(tmpT->pTransfer.pRemotePath==remotePath) return;
//    }
//
//    QwcTransferSocket *tmpSock = new QwcTransferSocket;
//    pTransferSockets.append(tmpSock);
//
//    tmpSock->pTransfer.pTransferType = WiredTransfer::TypeFolderUpload;
//    tmpSock->pTransfer.pRemotePath = remotePath;
//    tmpSock->pTransfer.pRemoteFolder = remotePath;
//    tmpSock->pTransfer.pLocalRoot = localPath;
//
//    QDirIterator dir(localPath, QDirIterator::Subdirectories);
//    while(dir.hasNext()) {
//        QString path = dir.next();
//        QFileInfo fileInfo(path);
//        tmpSock->pTransfer.fileListLocal.append(fileInfo.absoluteFilePath());
//        tmpSock->pTransfer.pFolderSize += fileInfo.size();
//        tmpSock->pTransfer.pFilesCount += 1;
//    }
//
//    // 	qDebug() << "Files:" << tmpSock->pTransfer.fileListLocal;
//
//    connect(tmpSock, SIGNAL(fileTransferDone(QwcTransferInfo)), this, SIGNAL(fileTransferDone(QwcTransferInfo)));
//    connect(tmpSock, SIGNAL(fileTransferError(QwcTransferInfo)), this, SIGNAL(fileTransferError(QwcTransferInfo)));
//    connect(tmpSock, SIGNAL(fileTransferStatus(QwcTransferInfo)), this, SIGNAL(fileTransferStatus(QwcTransferInfo)));
//    connect(tmpSock, SIGNAL(fileTransferFileDone(const QwcTransferInfo)), this, SLOT(fileTransferFileDone(const QwcTransferInfo))); // for folder transfers
//    connect(tmpSock, SIGNAL(destroyed()), this, SLOT(cleanTransfers()));
//
//    tmpSock->setServer( socket->peerAddress().toString(), socket->peerPort() );
//    tmpSock->pTransfer.pStatus = WiredTransfer::StatusQueuedLocal;
//    qDebug() << "Transfer Phase 1/3: LocalQueued:"<<localPath<<"to"<<remotePath;
//    emit fileTransferStarted(tmpSock->pTransfer);
}


/*! Queue a download transfer and check the queue.
*/
void QwcSocket::getFile(QString remotePath, QString localPath)
{
    qDebug() << this << "Queueing download for file:" << remotePath << "to" << localPath;
    QwcTransferInfo transfer;
    transfer.type = Qw::TransferTypeDownload;
    transfer.file.path = remotePath;
    transfer.file.localAbsolutePath = localPath;
    transferPool.append(transfer);
    emit fileTransferQueueChanged(transfer);
    checkTransferQueue();
}



/*! Queue and upload a single file to a remote path on the server.
*/
void QwcSocket::putFile(const QString localPath, const QString remotePath)
{
    QFileInfo localFile(localPath);

    qDebug() << this << "Queueing upload of file to" << remotePath;
    QwcTransferInfo transfer;
    transfer.type = Qw::TransferTypeUpload;
    transfer.file.path = remotePath;
    transfer.file.size = localFile.size();
    transfer.file.localAbsolutePath = localPath;
    transfer.file.updateLocalChecksum();
    transferPool.append(transfer);
    emit fileTransferQueueChanged(transfer);
    checkTransferQueue();


 /*   QFile tmpFile(theLocalPath);
    QString remotePath = theRemotePath;
    if(remotePath.left(2)=="//") remotePath.remove(0,1);
    if(tmpFile.exists() && !remotePath.isEmpty()) {

        // Check for duplicate uploads
        QListIterator<QPointer<QwcTransferSocket> > i(pTransferSockets);
        while( i.hasNext() ) {
            QwcTransferSocket *tmpT = i.next();
            if(!tmpT) continue;
            if(tmpT->pTransfer.pRemotePath==remotePath) return;
        }

        QwcTransferSocket *tmpSock = new QwcTransferSocket;
        pTransferSockets.append(tmpSock);
        connect(tmpSock, SIGNAL(fileTransferDone(QwcTransferInfo)), this, SIGNAL(fileTransferDone(QwcTransferInfo)));
        connect(tmpSock, SIGNAL(fileTransferError(QwcTransferInfo)), this, SIGNAL(fileTransferError(QwcTransferInfo)));
        connect(tmpSock, SIGNAL(fileTransferStatus(QwcTransferInfo)), this, SIGNAL(fileTransferStatus(QwcTransferInfo)));
        connect(tmpSock, SIGNAL(fileTransferStarted(QwcTransferInfo)), this, SIGNAL(fileTransferStarted(QwcTransferInfo)));
        connect(tmpSock, SIGNAL(fileTransferSocketError(QAbstractSocket::SocketError)), this, SIGNAL(fileTransferSocketError(QAbstractSocket::SocketError)));
        connect(tmpSock, SIGNAL(destroyed()), this, SLOT(cleanTransfers()));
        tmpSock->setServer( socket->peerAddress().toString(), socket->peerPort() );
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
    */
}



// ///
// /// Request Commands ///
// ///


/*! Set the current user nickname and send it to the server.
*/
void QwcSocket::setNickname(QString theNick)
{
    sessionUser.userNickname = theNick;
    sendMessage(QwMessage("NICK").appendArg(theNick));
}


/*! Set the current user status and send it to the server.
*/
void QwcSocket::setUserStatus(QString theStatus)
{
    sessionUser.userStatus = theStatus;
    sendMessage(QwMessage("STATUS").appendArg(sessionUser.userStatus));
}


/*! Decline an invitation to a private chat.
*/
void QwcSocket::rejectChat(int roomId)
{
    sendMessage(QwMessage("DECLINE").appendArg(QString::number(roomId)));
}


/*! Join (accept invitation) to a private chat room.
*/
void QwcSocket::joinChat(int roomId)
{
    sendMessage(QwMessage("JOIN").appendArg(QString::number(roomId)));
    do_request_user_list(roomId);
}


/*! Set the topic the specified chat room.
*/
void QwcSocket::setChatTopic(int chatId, QString topic)
{
    sendMessage(QwMessage("TOPIC").appendArg(QString::number(chatId)).appendArg(topic));
}

/*! Temporarily ban a user from the server. \a reason is a short text describing the reason for the ban.
*/
void QwcSocket::banClient(int userId, QString reason)
{
    sendMessage(QwMessage("BAN").appendArg(QString::number(userId)).appendArg(reason));
}


/*! Request the list of connected users of a specific room.
*/
void QwcSocket::do_request_user_list(int roomId)
{
    sendMessage(QwMessage("WHO").appendArg(QString::number(roomId)));
}

/*! Request the list of news articles.
*/
void QwcSocket::getNews()
{
    sendMessage(QwMessage("NEWS"));
}


/*! Post a new article to the news discussion board.
*/
void QwcSocket::postNews(QString text)
{
    sendMessage(QwMessage("POST").appendArg(text));
}


/*! Request information about a connected client.
*/
void QwcSocket::getClientInfo(int userId)
{
    sendMessage(QwMessage("INFO").appendArg(QString::number(userId)));
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


/*! Request file information (stat) of a specified path.
*/
void QwcSocket::statFile(const QString path)
{
    sendMessage(QwMessage("STAT").appendArg(path));
}


/*! Create a folder at the specified path.
*/
void QwcSocket::createFolder(const QString path)
{
    sendMessage(QwMessage("FOLDER").appendArg(path));
}


/*! Disconnect (Kick) a user from the server. \a reason provides a short reason why the user was disconnected.
*/
void QwcSocket::kickClient(int userId, QString reason)
{
    sendMessage(QwMessage("KICK").appendArg(QString::number(userId)).appendArg(reason));
}

/*! Move (and rename) a file or directory on the server.
*/
void QwcSocket::moveFile(const QString source, const QString destination)
{
    sendMessage(QwMessage("MOVE").appendArg(source).appendArg(destination));
}

/*! Delete a file or directory from the server.
*/
void QwcSocket::deleteFile(const QString path)
{
    if (path == "/" || path.isEmpty()) { return; }
    sendMessage(QwMessage("DELETE").appendArg(path));
}

void QwcSocket::getGroups() { sendMessage(QwMessage("GROUPS")); }
void QwcSocket::getUsers() { sendMessage(QwMessage("USERS")); }


/*! Create a new user account on the server.
*/
void QwcSocket::createUser(QwcUserInfo user)
{
    QwMessage message("CREATEUSER");
    message.appendArg(user.name).appendArg(user.pPassword).appendArg(user.pGroupName);
    user.appendPrivilegesFlags(message);
    sendMessage(message);
}


/*! Modify an existing account on the server.
*/
void QwcSocket::editUser(QwcUserInfo user)
{
    QwMessage message("EDITUSER");
    message.appendArg(user.name).appendArg(user.pPassword).appendArg(user.pGroupName);
    user.appendPrivilegesFlags(message);
    sendMessage(message);
}


/*! Send the CREATEGROUP command to the server. Creates a user group on the server (uses login and
    privileges from \a user object.
*/
void QwcSocket::createGroup(QwcUserInfo user)
{
    QwMessage reply("CREATEGROUP");
    reply.appendArg(user.name);
    user.appendPrivilegesFlags(reply);
    sendMessage(reply);
}


/*! Send the EDITGROUP command to the server. Modify a group on the server (uses login and
    privileges flags).
*/
void QwcSocket::editGroup(QwcUserInfo user)
{
    QwMessage reply("EDITGROUP");
    reply.appendArg(user.name);
    user.appendPrivilegesFlags(reply);
    sendMessage(reply);
}


/*! Send the DELETEGROUP command to the server. Deletes a user group from the server.
*/
void QwcSocket::deleteGroup(QString theName)
{
    sendMessage(QwMessage("DELETEGROUP").appendArg(theName));
}


/*! Send the DELETEUSER command. Deletes a user account from the database.
*/
void QwcSocket::deleteUser(QString theName)
{
    sendMessage(QwMessage("DELETEUSER").appendArg(theName));
}


void QwcSocket::readUser(QString theName)
{
    sendMessage(QwMessage("READUSER").appendArg(theName));
}


void QwcSocket::readGroup(QString theName)
{
    sendMessage(QwMessage("READGROUP").appendArg(theName));
}

void QwcSocket::searchFiles(const QString theSearch)
{
    sendMessage(QwMessage("SEARCH").appendArg(theSearch));
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
                .appendArg(tmpV.arg(clientName, clientSoftwareVersion, tmpOsName, tmpOsVersion, tmpOsArch)));
}


/*! Check the current transfer queue and start a new transfer if there are free slots.
*/
void QwcSocket::checkTransferQueue()
{
    int maximumTransfers = 1;

    qDebug() << this << "Checking transfer queue...";
    if (transferPool.isEmpty()) { return; }

    // Count the waiting transfers
    int transfersWaitingCount = 0;
    QMutableListIterator<QwcTransferInfo> i(transferPool);
    while (i.hasNext()) {
        QwcTransferInfo item = i.next();
        if (item.state == Qw::TransferInfoStateWaiting) { transfersWaitingCount += 1; }
    }
    if (transfersWaitingCount+transferSockets.count() >= maximumTransfers) { return; }

    // Start a new transfer
    QwcTransferInfo *transfer = NULL;// transferPool.first();
    i.toFront();
    while (i.hasNext()) {
        QwcTransferInfo &item = i.next();
        if (item.state == Qw::TransferInfoStateNone) {
            transfer = &item;
        }
    }

    if (!transfer) { return; }

    qDebug() << this << "Starting new waiting transfer from queue:" << transfer->file.path;

    // Request the transfer on the server
    if (transfer->type == Qw::TransferTypeDownload) {
        transfer->state = Qw::TransferInfoStateWaiting; // waiting for STAT
        sendMessage(QwMessage("STAT").appendArg(transfer->file.path));
        qDebug() << this << "Sending STAT command for download";

    } else if (transfer->type == Qw::TransferTypeUpload) {
        transfer->state = Qw::TransferInfoStateWaiting; // waiting for hash
        sendMessage(QwMessage("PUT").appendArg(transfer->file.path)
                    .appendArg(QString::number(transfer->file.size))
                    .appendArg(transfer->file.checksum));

    }

}


void QwcSocket::cleanTransfers()
{
    qDebug() << this << "Cleaning transfers";
    QMutableListIterator<QwcTransferSocket*> i(transferSockets);
    while (i.hasNext()) {
        QwcTransferSocket *item = i.next();
        if (item) { i.remove(); }
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

/*! Pause/Stop a transfer.
*/
void QwcSocket::pauseTransfer(const QwcTransferInfo &transfer)
{
    QListIterator<QwcTransferSocket*> i(transferSockets);
    while (i.hasNext()) {
        QwcTransferSocket *item = i.next();
        if (!item) { continue; }
        if (item->transferInfo.hash == transfer.hash) {
            item->abortTransfer();
            QwcTransferInfo pausedTransfer = item->transferInfo; // copy to have the most recent information
            pausedTransfer.state = Qw::TransferInfoStatePaused;
            transferPool.append(pausedTransfer);
            return;
        }
    }

    // Try to find the transfer in the pool
    QMutableListIterator<QwcTransferInfo> j(transferPool);
    while (j.hasNext()) {
        QwcTransferInfo &item = j.next();
        if (item.file.path == transfer.file.path) {
            if (item.state == Qw::TransferInfoStatePaused) {
                // If paused already, delete the transfer
                emit fileTransferError(item);
                j.remove();
            } else if (item.state == Qw::TransferInfoStateNone) {
                // If waiting, pause the transfer
                item.state = Qw::TransferInfoStatePaused;
                emit fileTransferStatus(item);
            }
            return;
        }
    }
}


/// Disconnect the low level socket from the server.
/// Mainly used during connection cancellation.
void QwcSocket::disconnectSocketFromServer()
{
    if (!socket) return;
    qDebug() << this << ": Aborting connection to server.";
    socket->abort();
}


void QwcSocket::getFileListRecusive(const QString & path)
{
    qDebug() << this << "Starting recursive indexing of" << path;
    pRecursiveFileListing.clear();
    pRecursivePath = path;
    pIndexingFiles = true;
    getFileList(path);
}






