#ifndef QWCSOCKET_H
#define QWCSOCKET_H

#include "QwSocket.h"
#include "QwServerInfo.h"

//#include <QtGui>
//#include <QtCore>
//#include <QSslSocket>

#include "QwcUserInfo.h"
#include "QwcFiletransferInfo.h"
#include "QwcFileInfo.h"
#include "QwcTrackerServerInfo.h"
#include "QwcFiletransferSocket.h"


namespace Wired {
    enum SocketType { QwcSocket, TrackerSocket };
}

const int kEOF = 0x04;
const int kFS = 0x1C;

class QwcSocket : public QwSocket
{
    Q_OBJECT

public:
    QwcSocket(QObject *parent = 0);
    ~QwcSocket();

    QwServerInfo serverInfo;

    QPixmap pServerBanner;

    /*! The name of the client (software name). */
    QString pClientName;
    /*! The version of the client software. */
    QString pClientVersion;

    Wired::SocketType pSocketType;

    /// The class that contains most of the user-specific information for this
    /// session. We use this to keep the clutter of the class to a minimum.
    QwcUserInfo sessionUser;

    /// This contains all users registered for the public user list (1) and
    /// all private chats. It automatically maintained and changes are
    /// signalled.
    QHash<int, QList<QwcUserInfo> > pUsers;

    // User Information
    void setUserNick(QString);
    void setUserAccount(QString, QString);
    QwcUserInfo getUserByID(int);

    // No further comment on those, and, no, you can not has cheezeburger.
    bool pIzCaturday;
    QString tranzlate(QString);

    int userCountByChat(const int theChatID);
    int userIndexByID(const int theID, const int theChat=1);
    const QwcUserInfo userByIndex(const int theChatID, const int theIndex);

    void connectToWiredServer(QString hostName, int port=2000);


    QPointer<QSslSocket> pSocket;
    QList<QPointer<QwcFiletransferSocket> > pTransferSockets;

    // Tracker subsystem
    //
    void connectToTracker(QString theHostName, int thePort=2002);

    // File Transfers
    bool pIndexingFiles; // true if the client is indexing server fails
    QList<QwcFileInfo> pRecursiveFileListing;
    QString pRecursivePath;

    bool isTransferringFileOfType(WiredTransfer::TransferType type);

    void proceedFolderDownload(QwcFiletransferSocket *);
    void proceedFolderUpload(QwcFiletransferSocket *);


public slots:
    void disconnectSocketFromServer();


    void createGroup(QwcUserInfo user);
    void editGroup(QwcUserInfo user);
    void deleteGroup(QString theName);

    // Wired Subsystem (not for Trackers)
    //
    void banClient(int theUserID, QString theReason);
    void cancelTransfer(QwcFiletransferInfo);
    void clearNews();
    void createChatWithClient(int theUserID);
    void createFolder(const QString thePath);

    void createUser(QwcUserInfo);
    void deleteFile(const QString thePath);

    void deleteUser(QString theLogin);
    void disconnectFromServer();

    void editUser(QwcUserInfo);
    void getClientInfo(int theUserID);
    void getFile(const QString thePath, const QString theLocalPath, const bool queueLocally);
    void getFolder(const QString &remotePath, const QString &localPath, const bool &queueLocally);
    void getFileList(QString thePath);
    void getFileListRecusive(const QString &path);
    void getGroups();
    void getNews();
    void getUsers();
    void inviteClientToChat(int theChatID, int theUserID);
    void joinChat(int theChatID);
    void kickClient(int theUserID, QString theReason);
    void leaveChat(int theChatID);
    void moveFile(const QString thePath, const QString theDestination);
    void postNews(QString thePost);
    void putFile(const QString theLocalPath, const QString theRemotePath, const bool queueLocally);
    void putFolder(const QString theLocalPath, const QString theRemotePath, const bool queueLocally);
    void readGroup(QString theName);
    void readUser(QString theName);
    void rejectChat(int theChatID);
    void searchFiles(const QString theSearch);
    void sendChatToRoom(int theChatID, QString theText, bool theIsAction);
    void sendPrivateMessage(int theUserID, QString theMessage);
    void setCaturday(bool);
    void setChatTopic(int theChatID, QString theTopic);
    void setUserIcon(QImage icon);
    void setUserStatus(QString theStatus);
    void statFile(const QString thePath);

    void runTransferQueue(WiredTransfer::TransferType type);


private slots:
    void handleMessageReceived(const QwMessage &message);

    // Connection
    void handleSocketConnected();
    void handleSocketConnectionLost();


    QList<QByteArray> GetFieldArray(QByteArray theData);
    void do_handle_wiredmessage(QByteArray);
    void do_request_user_list(int theChannel);

    void on_socket_readyRead();
    void on_server_filelist_item(QList<QByteArray>);
    void on_server_filelist_done(QList<QByteArray>);
    void on_server_userlist_imagechanged(QList<QByteArray>);
    void on_server_userinfo(QList<QByteArray> theParams);
    void on_server_new_chat_created(QList<QByteArray> theParams);
    void on_server_broadcast(QList<QByteArray> theParams);
    void on_server_transfer_ready(QList<QByteArray> theParams);
    void on_server_transfer_queued(QList<QByteArray> theParams);
    void on_server_file_info(QList<QByteArray> theParams);

    void on_server_search_listing(QList<QByteArray> theParams);
    void on_server_search_done(QList<QByteArray> theParams);
    void on_server_groups_listing(QList<QByteArray> theParams);
    void on_server_groups_done();
    void on_server_users_listing(QList<QByteArray> theParams);
    void on_server_users_done();
    void on_server_user_spec(QList<QByteArray> theParams);
    void on_server_group_spec(QList<QByteArray> theParams);
    void cleanTransfers();

    // Tracker
    //
    void on_tracker_listing_item(QList<QByteArray> theParams);
    void on_tracker_listing_done();

    // Transfers
    //
    void fileTransferFileDone(const QwcFiletransferInfo);


signals:
    void receivedUserlist(int theChatID);


    void onSocketError();
    void onServerInformation();

    void onServerLoginSuccessful();
    void onServerBanner(const QPixmap);


    void userChanged(const QwcUserInfo theOld, const QwcUserInfo theNew);
    void userJoinedRoom(int theChatID, const QwcUserInfo theUser);
    void userLeftRoom(int theChatID, const QwcUserInfo theUser);
    void userKicked(QwcUserInfo theVictim, QwcUserInfo theKiller, QString theReason);
    void userBanned(QwcUserInfo theVictim, QwcUserInfo theKiller, QString theReason);
    void onServerUserInfo(QwcUserInfo theUser);

    void receivedChatMessage(int theChatID, int theUserID, QString theText, bool theIsAction);
    void onChatTopic(int theChatID, QString theNick, QString theLogin, QHostAddress theIP, QDateTime theDateTime, QString theTopic);
    void onPrivateMessage(QwcUserInfo theUser, QString theMessage);
    void onServerBroadcast(QwcUserInfo theUser, QString theMessage);

    void onServerPrivateChatCreated(int theChatID);
    void onServerPrivateChatInvitation(int theChatID, QwcUserInfo theUser);

    void onServerNews(QString theNick, QString theTime, QString thePost);
    void onServerNewsPosted(QString theNick, QString theTime, QString thePost);
    void onServerNewsDone();


    void fileSearchDone(QList<QwcFileInfo> theResults);
    void onFilesListItem(QwcFileInfo file);
    void onFilesListDone(QString thePath, qlonglong theFreeSpace);
    void onFilesListRecursiveDone(const QList<QwcFileInfo>);
    void onServerFileTransferReady(QwcFiletransferInfo theTransfer);
    void onServerFileTransferQueued(QwcFiletransferInfo theTransfer);
    void onServerFileInfo(QwcFileInfo theFile);

    void fileTransferDone(const QwcFiletransferInfo theTransfer);
    void fileTransferStarted(const QwcFiletransferInfo theTransfer);
    void fileTransferError(const QwcFiletransferInfo theTransfer);
    void fileTransferStatus(const QwcFiletransferInfo theTransfer);

    void groupsListingDone(QStringList theGroups);
    void usersListingDone(QStringList theAccounts);

    /// Received the details of a group.
    void userSpecReceived(QwcUserInfo);

    void groupSpecReceived(QwcUserInfo);
    void errorBanned();
    void errorLoginFailed();
    void errorPermissionDenied();
    void errorFileNotFound();
    void errorClientNotFound();
    void errorQueueLimitExceeded();
    void errorCommandFailed();
    void errorCommandNotRecognized();
    void errorCommandNotImplemented();
    void errorCommandSyntaxError();
    void errorAccountNotFound();
    void errorAccountExists();
    void errorCanNotBeDisconnected();
    void errorFileExists();
    void errorChecksumMismatch();
    void errorOccoured(int theError);
    void receivedUserPrivileges(const QwcUserInfo theSession);
    void fileTransferSocketError(QAbstractSocket::SocketError);

    // TRACKER
    //
    void trackerServersReceived(QList<QwcTrackerServerInfo>);


private:
    // Responses
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
    void handleMessage310(const QwMessage &message);
    void handleMessage311(const QwMessage &message);
    void handleMessage602(const QwMessage &message);

    // Comments
    void sendMessageINFO();

    // Tracker subsystem
    //
    void tracker_request_servers();



    // Buffers while receiving the list of groups and users (admin mode)
    QStringList pAdminGroups;
    QStringList pAdminUsers;

    QHash<QString,QString> pTranzlator;

    /// Buffer for the user id we have invited before. Unfortunately Wired has no
    /// transactions, so we have to hope that this works well enough for now.
    int pInvitedUserID;

    /// Our function that formats a Wired message and sends it to the server.
    void sendWiredCommand(const QByteArray);

    /// The temporary list of items for the search results.
    QList<QwcFileInfo> pSearchResults;
    QList<QwcTrackerServerInfo> pTrackerServers;

    /// This is our TCP buffer. Could possibly be optimized, but works for now.
    QByteArray pBuffer;


    
};

#endif
