#ifndef QWCSOCKET_H
#define QWCSOCKET_H

#include "QwSocket.h"
#include "QwServerInfo.h"

#include "QwRoom.h"
#include "QwcUserInfo.h"
#include "QwcTransferInfo.h"
#include "QwcFileInfo.h"
#include "QwcTrackerServerInfo.h"
#include "QwcTransferSocket.h"

class QwcSocket : public QwSocket
{
    Q_OBJECT

public:
    QwcSocket(QObject *parent = 0);
    ~QwcSocket();

    /*! Attempt to establish a connection to the specified server. */
    void connectToWiredServer(QString hostName, int port=2000);

    /*! Information about the server, as provided during handshake. */
    QwServerInfo serverInfo;
    /*! The banner of the server as transmitted during handshake. */
    QImage serverImage;
    /*! The name of the client (software name). */
    QString clientName;
    /*! The version of the client software. */
    QString clientSoftwareVersion;
    /*! The client user object which keeps track of available information of the current session. */
    QwcUserInfo sessionUser;
    /*! This contains all users registered for the public user list (1). */
    QHash<int, QwcUserInfo> users;
    /*! A list of all currently used chat rooms. Chat rooms contain user lists and refer to them by
        their user ID. */
    QHash<int, QwRoom> rooms;

    /*! The user entered host name of the server. */
    QString serverAddress;
    /*! The user entered server port. */
    int serverPort;

    // User Information
    void setNickname(QString);
    void setUserAccount(QString, QString);

    /*! Returns a const list of pointers to all active transfer sockets. */
    const QList<QwcTransferSocket*> allTransferSockets() const { return transferSockets; }
    /*! Returns a const reference to a list of all queued transfers. */
    const QList<QwcTransferInfo>& allQueuedTransfers() const { return transferPool; }





    // File Transfers
    bool pIndexingFiles; // true if the client is indexing server fails
    QList<QwcFileInfo> pRecursiveFileListing;
    QString pRecursivePath;


    void proceedFolderDownload(QwcTransferSocket *);
    void proceedFolderUpload(QwcTransferSocket *);


public slots:
    void pauseTransfer(const QwcTransferInfo &transfer);
    void disconnectSocketFromServer();


    void createGroup(QwcUserInfo user);
    void editGroup(QwcUserInfo user);
    void deleteGroup(QString theName);

    // Wired Subsystem (not for Trackers)
    //
    void banClient(int userId, QString reason);
    void clearNews();
    void createChatWithClient(int firstInvitedUser = 0);
    void createFolder(const QString thePath);

    void createUser(QwcUserInfo user);
    void deleteFile(const QString thePath);

    void deleteUser(QString theLogin);
    void disconnectFromServer();

    void editUser(QwcUserInfo);
    void getClientInfo(int userId);
    void getFile(QString remotePath, QString localPath);
    void getFolder(const QString &remotePath, const QString &localPath, const bool &queueLocally);
    void getFileList(QString thePath);
    void getFileListRecusive(const QString &path);
    void getGroups();
    void getNews();
    void getUsers();
    void inviteClientToChat(int chatId, int userId);
    void joinChat(int theChatID);
    void kickClient(int userId, QString reason);
    void leaveChat(int theChatID);
    void moveFile(const QString source, const QString destination);
    void postNews(QString thePost);
    void putFile(const QString localPath, const QString remotePath);
    void putFolder(const QString theLocalPath, const QString theRemotePath, const bool queueLocally);
    void readGroup(QString theName);
    void readUser(QString theName);
    void rejectChat(int theChatID);
    void searchFiles(const QString theSearch);
    void sendChatToRoom(int theChatID, QString theText, bool theIsAction);
    void sendPrivateMessage(int userId, QString message);
    void setCaturday(bool);
    void setChatTopic(int chatId, QString topic);
    void setIconImage(QImage icon);
    void setUserStatus(QString theStatus);
    void statFile(const QString thePath);

private slots:
    void handleTransferDone(const QwcTransferInfo &transfer);
    void handleTransferError(const QwcTransferInfo &transfer);

    void handleMessageReceived(const QwMessage &message);

    // Connection
    void handleSocketConnected();
    void handleSocketConnectionLost();

    void do_request_user_list(int roomId);


    void cleanTransfers();



signals:
    /*! A protocol error was received from the server. This signal is emitted for every 5xx-type
        message, but some handling (such as login errors, etc.) is done inside the socket before
        the signal is emitted. */
    void protocolError(Qw::ProtocolError error);


    /*! Relay signal which origns from the transfer socket of any active transfer. */
    void fileTransferDone(const QwcTransferInfo &transfer);
    /*! Relay signal which origns from the transfer socket of any active transfer. */
    void fileTransferError(const QwcTransferInfo &transfer);
    /*! Relay signal which origns from the transfer socket of any active transfer. */
    void fileTransferStarted(const QwcTransferInfo &transfer);
    /*! Relay signal which origns from the transfer socket of any active transfer. */
    void fileTransferStatus(const QwcTransferInfo &transfer);
    /*! Signal which is emitted after the transfer queue has changed (added or removed a transfer). */
    void fileTransferQueueChanged(const QwcTransferInfo &transfer);


    void receivedUserlist(int theChatID);


    void onServerInformation();

    void onServerLoginSuccessful();
    void onServerBanner(const QImage banner);


    void userChanged(const QwcUserInfo theOld, const QwcUserInfo theNew);
    void userJoinedRoom(int theChatID, const QwcUserInfo theUser);
    void userLeftRoom(int theChatID, const QwcUserInfo theUser);
    void userKicked(QwcUserInfo theVictim, QwcUserInfo theKiller, QString theReason);
    void userBanned(QwcUserInfo theVictim, QwcUserInfo theKiller, QString theReason);
    void userInformation(QwcUserInfo theUser);

    void receivedChatMessage(int theChatID, int theUserID, QString theText, bool theIsAction);
    void onChatTopic(int theChatID, QString theNick, QString theLogin, QHostAddress theIP, QDateTime theDateTime, QString theTopic);
    void onPrivateMessage(QwcUserInfo theUser, QString theMessage);
    void broadcastMessage(QwcUserInfo theUser, QString theMessage);

    void privateChatCreated(int theChatID);
    void privateChatInvitation(int theChatID, QwcUserInfo theUser);




    void fileSearchDone(QList<QwcFileInfo> theResults);
    void onFilesListItem(QwcFileInfo file);
    void onFilesListDone(QString thePath, qlonglong theFreeSpace);
    void onFilesListRecursiveDone(const QList<QwcFileInfo>);
    void onServerFileTransferReady(QwcTransferInfo theTransfer);
    void onServerFileTransferQueued(QwcTransferInfo theTransfer);
    void fileInformation(QwcFileInfo theFile);



    void receivedAccountGroupList(QStringList theGroups);
    void receivedAccountList(QStringList theAccounts);

    /// Received the details of a group.
    void userSpecReceived(QwcUserInfo);

    void groupSpecReceived(QwcUserInfo);

    void receivedUserPrivileges(const QwcUserInfo theSession);
    void fileTransferSocketError(QAbstractSocket::SocketError);

    // TRACKER
    //
    void trackerServersReceived(QList<QwcTrackerServerInfo>);


private:
    // Message Handlers
    void handleMessage200(const QwMessage &message);
    void handleMessage201(const QwMessage &message);
    void handleMessage203(const QwMessage &message);
    void handleMessage300(const QwMessage &message);
    void handleMessage301(const QwMessage &message);
    void handleMessage302(const QwMessage &message);
    void handleMessage303(const QwMessage &message);
    void handleMessage304(const QwMessage &message);
    void handleMessage305(const QwMessage &message);
    void handleMessage306(const QwMessage &message);
    void handleMessage307(const QwMessage &message);
    void handleMessage308(const QwMessage &message);
    void handleMessage309(const QwMessage &message);
    void handleMessage310(const QwMessage &message);
    void handleMessage311(const QwMessage &message);
    void handleMessage320(const QwMessage &message);
    void handleMessage321(const QwMessage &message);
    void handleMessage322(const QwMessage &message);
    void handleMessage330(const QwMessage &message);
    void handleMessage331(const QwMessage &message);
    void handleMessage340(const QwMessage &message);
    void handleMessage341(const QwMessage &message);
    void handleMessage400(const QwMessage &message);
    void handleMessage401(const QwMessage &message);
    void handleMessage402(const QwMessage &message);
    void handleMessage410(const QwMessage &message);
    void handleMessage411(const QwMessage &message);
    void handleMessage420(const QwMessage &message);
    void handleMessage421(const QwMessage &message);
    void handleMessage5xx(const int &errorId);
    void handleMessage600(const QwMessage &message);
    void handleMessage601(const QwMessage &message);
    void handleMessage602(const QwMessage &message);
    void handleMessage610(const QwMessage &message);
    void handleMessage611(const QwMessage &message);
    void handleMessage620(const QwMessage &message);
    void handleMessage621(const QwMessage &message);

    // Commands
    void sendMessageINFO();

    // Transfers
    void checkTransferQueue();
    QList<QwcTransferInfo> transferPool;
    QList<QwcTransferSocket*> transferSockets;

    // No further comment on those, and, no, you can not has cheezeburger.
    bool pIzCaturday;
    QString tranzlate(QString);

    // Buffers while receiving the list of groups and users (admin mode)
    QStringList pAdminGroups;
    QStringList pAdminUsers;
    QHash<QString,QString> pTranzlator;

    /// Buffer for the user id we have invited before. Unfortunately Wired has no
    /// transactions, so we have to hope that this works well enough for now.
    int pInvitedUserID;

    /// The temporary list of items for the search results.
    QList<QwcFileInfo> pSearchResults;
    QList<QwcTrackerServerInfo> pTrackerServers;



    /// This is our TCP buffer. Could possibly be optimized, but works for now.
    QByteArray pBuffer;
};

#endif
