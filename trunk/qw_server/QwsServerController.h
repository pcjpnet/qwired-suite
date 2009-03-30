/* \class QwsServerController
   \author Bastian Bense <bb@bense.de>

   This class implements the connection and session manager for the Qwired server.
*/

#ifndef QWSSERVERCONTROLLER_H
#define QWSSERVERCONTROLLER_H

#include <iostream>
#include <QtCore>
#include <QtNetwork>
#include <QQueue>

#include "QwsClientSocket.h"
#include "QwSslTcpServer.h"
#include "QwsClientTransferSocket.h"
#include "QwsTransferInfo.h"

#include "QwsTransferPool.h"

namespace Qws {
    enum LogType { LogTypeInfo, LogTypeWarning, LogTypeFatal, LogTypeDebug };
}

class QwsServerController : public QObject
{

    Q_OBJECT

public:
    QwsServerController(QObject *parent = 0);
    virtual ~QwsServerController();

    static void qwLog(QString message, Qws::LogType type=Qws::LogTypeInfo);

    // Database access/configuration
    QVariant getConfigurationParam(const QString key, const QVariant defaultValue=QVariant());

private:
    int sessionIdCounter;
    int roomIdCounter;
    int maxTransfersPerClient;



    QPointer<QwSslTcpServer> sessionTcpServer;
    QPointer<QwSslTcpServer> transferTcpServer;
    QwsTransferPool *transferPool;

    /*! The list of active transfer sockets. Active transfers are not in the \a transfers list. */
    QList<QPointer<QwsClientTransferSocket> > transferSockets;

    QHash<int, QwsClientSocket*> sockets;
    QHash<int, QwRoom*> rooms;

    QList<QwsClientTransferSocket*> transfersWithUserId(int userId);
    void checkTransferQueue(int userId);


signals:
    void qwBroadcastChat(int chatId, int userId, QString text, bool isEmote);


private slots:

    // Socket Management
    void acceptSessionSslConnection();
    void acceptTransferSslConnection();

    // Protocol
    void handleSocketDisconnected();
    void handleSocketSessionStateChanged(const Qws::SessionState state);
    void broadcastMessage(const QwMessage message, const int roomId, const bool sendToSelf);
    void relayUserStatusChanged();
    void handleMessageINFO(const int userId);
    void handleUserlistRequest(const int roomId);

    // Communication
    void addUserToRoom(const int roomId, const int userId);
    void removeUserFromRoom(const int roomId, const int userId);
    void relayChatToRoom(const int roomId, const QString text, const bool isEmote);
    void relayMessageToUser(const int userId, const QString text);
    void changeRoomTopic(const int roomId, const QString topic);
    void sendRoomTopic(const int roomId);
    void createNewRoom();
    void inviteUserToRoom(const int userId, const int roomId);
    void handleMessageJOIN(const int roomId);
    void handleMessageDECLINE(const int roomId);
    void handleMessageLEAVE(const int roomId);

    // Administration
    void handleMessageBAN_KICK(const int userId, const QString reason, const bool isBan);
    void handleModifiedUserAccount(const QString name);
    void handleModifiedUserGroup(const QString name);

    // File transfers
    void handleMessageGET(const QwsFile file);
    void handleMessagePUT(const QwsFile file);
    void handleTransferDone(const QwsTransferInfo transfer);
    void handleTransferError(Qws::TransferSocketError error, const QwsTransferInfo transfer);

public slots:
    bool loadConfiguration();
    bool startServer();


};

#endif
