#ifndef QWCSOCKET_H
#define QWCSOCKET_H

#include "QwSocket.h"
#include "QwServerInfo.h"

//#include <QtGui>
//#include <QtCore>
//#include <QSslSocket>

#include "QwRoom.h"
#include "QwcUserInfo.h"
#include "QwcFiletransferInfo.h"
#include "QwcFileInfo.h"
#include "QwcTrackerServerInfo.h"
#include "QwcFiletransferSocket.h"

class QwcSocket : public QwSocket
{
    Q_OBJECT

public:
    QwcSocket(QObject *parent = 0);
    ~QwcSocket();

    /*! Information about the server, as provided during handshake. */
    QwServerInfo serverInfo;
    /*! The banner of the server as transmitted during handshake. */
    QPixmap pServerBanner;
    /*! The name of the client (software name). */
    QString pClientName;
    /*! The version of the client software. */
    QString pClientVersion;
    /*! The client user object which keeps track of available information of the current session. */
    QwcUserInfo sessionUser;
    /*! This contains all users registered for the public user list (1). */
    QHash<int, QwcUserInfo> users;
    /*! A list of all currently used chat rooms. Chat rooms contain user lists and refer to them by
        their user ID. */
    QHash<int, QwRoom> rooms;

    // User Information
    void setNickname(QString);
    void setUserAccount(QString, QString);

    // No further comment on those, and, no, you can not has cheezeburger.
    bool pIzCaturday;
    QString tranzlate(QString);

    int userCountByChat(const int theChatID);
    int userIndexByID(const int theID, const int theChat=1);
    const QwcUserInfo userByIndex(const int theChatID, const int theIndex);

    void connectToWiredServer(QString hostName, int port=2000);


    QPointer<QSslSocket> pSocket;
    QList<QPointer<QwcFiletransferSocket> > pTransferSockets;

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
    void banClient(int userId, QString reason);
    void cancelTransfer(QwcFiletransferInfo);
    void clearNews();
    void createChatWithClient(int firstInvitedUser = 0);
    void createFolder(const QString thePath);

    void createUser(QwcUserInfo user);
    void deleteFile(const QString thePath);

    void deleteUser(QString theLogin);
    void disconnectFromServer();

    void editUser(QwcUserInfo);
    void getClientInfo(int userId);
    void getFile(const QString thePath, const QString theLocalPath, const bool queueLocally);
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
    void putFile(const QString theLocalPath, const QString theRemotePath, const bool queueLocally);
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

    void runTransferQueue(WiredTransfer::TransferType type);


private slots:
    void handleMessageReceived(const QwMessage &message);

    // Connection
    void handleSocketConnected();
    void handleSocketConnectionLost();

    void do_request_user_list(int roomId);


    void cleanTransfers();

    // Tracker
    //
    void on_tracker_listing_item(QList<QByteArray> theParams);
    void on_tracker_listing_done();

    // Transfers
    //
    void fileTransferFileDone(const QwcFiletransferInfo);


signals:
    /*! A protocol error was received from the server. This signal is emitted for every 5xx-type
        message, but some handling (such as login errors, etc.) is done inside the socket before
        the signal is emitted. */
    void protocolError(Qw::ProtocolError error);


    void receivedUserlist(int theChatID);


    void onServerInformation();

    void onServerLoginSuccessful();
    void onServerBanner(const QPixmap);


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

    void onServerNews(QString theNick, QString theTime, QString thePost);
    void onServerNewsPosted(QString theNick, QString theTime, QString thePost);
    void newsDone();


    void fileSearchDone(QList<QwcFileInfo> theResults);
    void onFilesListItem(QwcFileInfo file);
    void onFilesListDone(QString thePath, qlonglong theFreeSpace);
    void onFilesListRecursiveDone(const QList<QwcFileInfo>);
    void onServerFileTransferReady(QwcFiletransferInfo theTransfer);
    void onServerFileTransferQueued(QwcFiletransferInfo theTransfer);
    void fileInformation(QwcFileInfo theFile);

    void fileTransferDone(const QwcFiletransferInfo theTransfer);
    void fileTransferStarted(const QwcFiletransferInfo theTransfer);
    void fileTransferError(const QwcFiletransferInfo theTransfer);
    void fileTransferStatus(const QwcFiletransferInfo theTransfer);

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
