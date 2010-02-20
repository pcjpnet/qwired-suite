/* \class QwsServerController
   \author Bastian Bense <bb@bense.de>

   This class implements the connection and session manager for the Qwired server.
*/

#ifndef QWSSERVERCONTROLLER_H
#define QWSSERVERCONTROLLER_H


#include "QwsClientSocket.h"
#include "QwSslTcpServer.h"
#include "QwsClientTransferSocket.h"
#include "QwsTransferInfo.h"
#include "QwsTransferPool.h"
#include "QwsFileIndexerThread.h"
#include "QwsTrackerController.h"

#include <QtCore/QDir>

typedef struct lua_State lua_State;

namespace Qws {
    enum LogType { LogTypeInfo, LogTypeWarning, LogTypeFatal, LogTypeDebug };
}

class QwsServerController :
        public QObject
{
    Q_OBJECT

public:
    QwsServerController(QObject *parent = 0);
    ~QwsServerController();

    // Database access/configuration
    QVariant getConfig(const QString key, const QVariant defaultValue=QVariant());

    /*! The total amount of bytes sent to clients in file transfers. */
    qint64 statsTotalSent;
    /*! The total amount of bytes received from clients in file transfers. */
    qint64 statsTotalReceived;


    bool hook_writeAccount(const QwsUser &user, bool deleteAccount = false);
    QwsUser hook_readAccount(const QString &login, Qws::UserType type);
    QList<QwsUser> hook_readAccountsAndGroups();


public slots:
    void reloadTrackerConfiguration();
    void reloadBanlistConfiguration();

protected:
    int sessionIdCounter;
    int roomIdCounter;

    /*! The maximum number of tranfsfers per client. */
    int m_maxTransfersPerClient;
    /*! The time (in seconds) a temporary ban should last. */
    int m_banDuration;

    /*! The object containing the general server information. */
    QwServerInfo m_serverInformation;
    /*! The server root points to a directory where the database, files and other information
        will be stored. By default this will be the current working directory. */
    QDir m_serverRootDirectory;
    /*! If true, the user images will be sent after the login is complete to speed up the login
        sequence on slower machines. */
    bool m_delayedUserImagesEnabled;

    /*! This member contains all permanently and temporarily banned addresses. The \a QDateTime part
        of the pair defines when the ban for the pattern in \a QString expires. Permanent bans have
        a Null-QDateTime object. */
    QList<QPair<QString,QDateTime> > m_banList;

    QPointer<QwSslTcpServer> sessionTcpServer;
    QPointer<QwSslTcpServer> transferTcpServer;

    QPointer<QwsTrackerController> trackerController;

    QwsTransferPool *transferPool;

    /*! The pointer to the currently running indexer thread. If no thread is active, this pointer
        will return NULL. */
    QPointer<QwsFileIndexerThread> filesIndexerThread;

    /*! The list of active transfer sockets. Active transfers are not in the \a transfers list. */
    QList<QPointer<QwsClientTransferSocket> > transferSockets;

    /*! The list of client connection sockets. */
    QHash<int, QwsClientSocket*> m_sockets;
    /*! The list of chat rooms. */
    QHash<int, QwRoom*> rooms;

    QList<QwsClientTransferSocket*> transfersWithUserId(int userId);
    void checkTransferQueue(int userId);

    /*! The Lua state responsible for our configuration file. */
    lua_State *m_lua;


signals:
    void serverLogMessage(const QString message);
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
    void qwLog(QString message);

    bool loadConfiguration();
    bool startServer();
    void reindexFiles();


};

#endif
