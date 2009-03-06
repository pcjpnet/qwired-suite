#ifndef QWCSOCKET_H
#define QWCSOCKET_H

#include <QtGui>
#include <QtCore>
#include <QSslSocket>

#include "QwcUserInfo.h"
#include "QwcFiletransferInfo.h"
#include "QwcFileInfo.h"
#include "QwcTrackerServerInfo.h"
#include "QwcFiletransferSocket.h"

/**
        @author Bastian Bense <bastibense@gmail.com>
 */

namespace Wired {
    enum SocketType { QwcSocket, TrackerSocket };
}

const int kEOF = 0x04;
const int kFS = 0x1C;

class QwcSocket : public QObject
{
    Q_OBJECT

public:

    // Server Information
    QString pServerAppVersion;
    QString pServerProtoVersion;
    QString pServerName;
    QString pServerDescription;
    QDateTime pServerStartTime;
    QPixmap pServerBanner;
    int pServerFileCount;
    int	pServerFileSize;
    QString pClientName;
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

    void connectToWiredServer(QString theHostName, int thePort=2000);

    QwcSocket(QObject *parent = 0);
    ~QwcSocket();
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

    // Wired Subsystem (not for Trackers)
    //
    void banClient(int theUserID, QString theReason);
    void cancelTransfer(QwcFiletransferInfo);
    void clearNews();
    void createChatWithClient(int theUserID);
    void createFolder(const QString thePath);
    void createGroup(QwcUserInfo);
    void createUser(QwcUserInfo);
    void deleteFile(const QString thePath);
    void deleteGroup(QString theName);
    void deleteUser(QString theLogin);
    void disconnectFromServer();
    void editGroup(QwcUserInfo);
    void editUser(QwcUserInfo);
    void getClientInfo(int theUserID);
    void getFile(const QString thePath, const QString theLocalPath, const bool queueLocally);
    void getFolder(const QString &remotePath, const QString &localPath, const bool &queueLocally);
    void getFileList(QString thePath);
    void getFileListRecusive(const QString &path);
    void getGroups();
    void getNews();
    void getPrivileges();
    void getServerBanner();
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
    void sendChat(int theChatID, QString theText, bool theIsAction);
    void sendPrivateMessage(int theUserID, QString theMessage);
    void setCaturday(bool);
    void setChatTopic(int theChatID, QString theTopic);
    void setUserIcon(QPixmap theIcon);
    void setUserStatus(QString theStatus);
    void statFile(const QString thePath);

    void runTransferQueue(WiredTransfer::TransferType type);


        private slots:
    QList<QByteArray> GetFieldArray(QByteArray theData);
    void do_handle_wiredmessage(QByteArray);
    void do_request_user_list(int theChannel);
    void do_send_user_login();
    void on_socket_encrypted();
    void on_socket_readyRead();
    void on_socket_sslErrors(const QList<QSslError> &errors);
    void on_server_filelist_item(QList<QByteArray>);
    void on_server_filelist_done(QList<QByteArray>);
    void on_server_userlist_item(QList<QByteArray>);
    void on_server_userlist_changed(QList<QByteArray>);
    void on_server_userlist_imagechanged(QList<QByteArray>);
    void on_server_userlist_joined(QList<QByteArray>);
    void on_server_userlist_left(QList<QByteArray>);
    void on_server_userlist_kicked(QList<QByteArray>);
    void on_server_userlist_banned(QList<QByteArray>);
    void on_server_userinfo(QList<QByteArray> theParams);
    void on_server_new_chat_created(QList<QByteArray> theParams);
    void on_server_banner(QList<QByteArray> theParams);
    void on_server_broadcast(QList<QByteArray> theParams);
    void on_server_transfer_ready(QList<QByteArray> theParams);
    void on_server_transfer_queued(QList<QByteArray> theParams);
    void on_server_file_info(QList<QByteArray> theParams);
    void on_server_privileges(QList<QByteArray> theParams);
    void on_socket_error(QAbstractSocket::SocketError error);
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
    void onSocketError(QAbstractSocket::SocketError);
    void onServerInformation();

    void onServerLoginSuccessful();
    void onServerBanner(const QPixmap);

    void onServerUserlistItem(int theChatID, const QwcUserInfo);
    void onServerUserlistDone(int theChatID);
    void onServerUserChanged(const QwcUserInfo theOld, const QwcUserInfo theNew);
    void onServerUserJoined(int theChatID, const QwcUserInfo theUser);
    void onServerUserLeft(int theChatID, const QwcUserInfo theUser);
    void onServerUserKicked(QwcUserInfo theVictim, QwcUserInfo theKiller, QString theReason);
    void onServerUserBanned(QwcUserInfo theVictim, QwcUserInfo theKiller, QString theReason);
    void onServerUserInfo(QwcUserInfo theUser);

    void onServerChat(int theChatID, int theUserID, QString theText, bool theIsAction);
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
    void userPrivileges(const QwcUserInfo theSession);
    void fileTransferSocketError(QAbstractSocket::SocketError);

    // TRACKER
    //
    void trackerServersReceived(QList<QwcTrackerServerInfo>);

private:

    // Tracker subsystem
    //
    void tracker_request_servers();
    void sendClientInfo();

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
