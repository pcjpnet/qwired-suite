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

#include "QwGlobals.h"
#include <QTimerEvent>

namespace Qwc {
    const int PUBLIC_CHAT = 1;
}

class QwcSocket
    : public QwSocket
{
    Q_OBJECT

public:
    QwcSocket(QObject *parent = 0);
    ~QwcSocket();

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

    int pingLatency() const;



    /*! Contains the default directory where downloads should go to. (Default: ~) */
    QString defaultDownloadDirectory;

    // User Information
    void setNickname(QString);
    void setUserAccount(QString, QString);

    /*! This member contains all active and locally queued transfer sockets. */
    QList<QwcTransferSocket*> m_transfers;



public slots:

    // Session
    //
    void connectToServer(const QString &address, int port = 2000);
    void disconnectFromServer();
    void setUserIcon(QImage icon);
    void setUserStatus(QString theStatus);
    void setCaturdayMode(bool);
    void sendPing();


    // Chats and Communication
    //
    void clearNews();
    void getNews();
    void postNews(const QString &text);
    void leaveChat(int theChatID);
    void rejectChat(int theChatID);
    void createChatWithClient(int firstInvitedUser = 0);
    void setChatTopic(int chatId, QString topic);
    void sendPrivateMessage(int userId, QString message);
    void writeToChat(int chat, QString text, bool emote);
    void inviteClientToChat(int chatId, int userId);
    void joinChat(int theChatID);

    // Administration
    //
    void getClientInformation(int userId);
    void readGroup(QString theName);
    void createGroup(QwcUserInfo user);
    void editGroup(QwcUserInfo user);
    void deleteGroup(QString theName);
    void readUser(QString theName);
    void createUser(QwcUserInfo user);
    void editUser(QwcUserInfo);
    void deleteUser(QString theLogin);
    void getGroups();
    void getUsers();
    void kickClient(int userId, const QString &reason);
    void banClient(int userId, const QString &reason);

    // Files and Transfers
    //
    void createFolder(const QString &path);
    void deleteFile(const QString &path);
    void pauseTransfer(const QwcTransferInfo &transfer);
    void resumeTransfer(const QwcTransferInfo &transfer);
    void setFileComment(QString path, QString comment);
    void getFileInformation(const QString &path);
    void downloadFileOrFolder(QwcFileInfo fileInfo);
    void uploadFileOrFolder(QwcFileInfo fileInfo);
    void searchFiles(const QString theSearch);
    void getFileList(QString thePath);
    void getFileListRecusive(const QString &path);
    void moveFile(const QString &source, const QString &destination);


private slots:
    void handleTransferDone(QwcTransferSocket *transferSocket);
    void handleTransferError(QwcTransferSocket *transferSocket);
    void handleTransferStatus(QwcTransferSocket *transferSocket);
    void handleTransferStarted(QwcTransferSocket *transferSocket);

    void handleMessageReceived(const QwMessage &message);

    // Connection
    void handleSocketConnected();
    void handleSocketConnectionLost();

    void getUserlist(int roomId);


    void cleanTransfers();



signals:
    /*! Emitted when a SSL/TCP connection has been established, but no login has occurred yet. */
    void socketConnected();

    /*! A protocol error was received from the server. This signal is emitted for every 5xx-type
        message, but some handling (such as login errors, etc.) is done inside the socket before
        the signal is emitted. */
    void protocolError(Qw::ProtocolError error);

    /*! This signal is emitted after a PING response has been received. \e latency defines how much
        time has passed since the PING message was sent in milliseconds. */
    void pingLatency(int latency);

    // Signals for file transfers
    // These should be used by the main application (non Qwired-code).

    /*! Relay signal which origns from the transfer socket of any active transfer. */
    void fileTransferDone(const QwcTransferInfo &transfer);
    /*! Relay signal which origns from the transfer socket of any active transfer. */
    void fileTransferError(const QwcTransferInfo &transfer);
    /*! Relay signal which origns from the transfer socket of any active transfer. */
    void fileTransferStarted(const QwcTransferInfo &transfer);
    /*! Relay signal which origns from the transfer socket of any active transfer. */
    void fileTransferStatus(const QwcTransferInfo &transfer);
    /*! Signal which is emitted after the transfer queue has changed (added or removed a transfer). */
    void fileTransferQueueChanged();


    /*! A news item was received from the server and should be added to the list of news. A series
        of newsListingItem() signals is followed by a final newsListingDone() signal. */
    void newsListingItem(QString nickname, QDateTime time, QString text);
    /*! Emitted after a series of newsListingItem() signals, indicating that all news items have
        been received. */
    void newsListingDone();
    /*! Emitted when another user posts a new news item. */
    void newsPosted(QString nickname, QDateTime time, QString text);



    void receivedUserlist(int theChatID);


    void onServerInformation();

    void onServerLoginSuccessful();
    void serverBannerReceived(const QPixmap banner);


    void userChanged(const QwcUserInfo theOld, const QwcUserInfo theNew);
    void userJoinedRoom(int theChatID, const QwcUserInfo theUser);
    void userLeftRoom(int theChatID, const QwcUserInfo theUser);
    void userKicked(QwcUserInfo theVictim, QwcUserInfo theKiller, QString theReason);
    void userBanned(QwcUserInfo theVictim, QwcUserInfo theKiller, QString theReason);
    void userInformation(QwcUserInfo theUser);

    void receivedChatMessage(int chatId, int userId, const QString &text, bool isEmote);
    void onChatTopic(int chatId, const QString &nickname, const QString &login, QHostAddress ip,
                     QDateTime dateTime, const QString &topic);
    void privateMessage(QwcUserInfo theUser, QString theMessage);
    void broadcastMessage(QwcUserInfo theUser, QString theMessage);

    void privateChatCreated(int theChatID);
    void privateChatInvitation(int theChatID, QwcUserInfo theUser);



    /*! This signal is emitted for each returned search result of the last. */
    void fileSearchResultListItem(QwcFileInfo item);
    /*! This signal is emitted when all results have been received. */
    void fileSearchResultListDone();

    /*! This signal is emitted when a new file list item has been received. */
    void onFilesListItem(QwcFileInfo file);
    /*! This signal is emitted when the file listing is complete and also provides some information
        about the server. */
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

    void receivedUserPrivileges();
    void fileTransferSocketError(QAbstractSocket::SocketError);

    // TRACKER
    //
    void trackerServersReceived(QList<QwcTrackerServerInfo>);


private:
    // Message Handlers
    void handleMessage200(const QwMessage &message);
    void handleMessage201(const QwMessage &message);
    void handleMessage202(const QwMessage &message);
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

    // No further comment on those, and, no, you can not has cheezburger.
    bool m_caturdayFlag;
    QString tranzlate(QString);

    /*! If this member is set to true, a LISTRECURSIVE command likely has been issued and results
        are collected. */
    bool indexingFiles;
    /*! This list contains all file information items returned during recursive listing. */
    QList<QwcFileInfo> indexingResults;

    // Buffers while receiving the list of groups and users (admin mode)
    QStringList m_groupListingCache;
    QStringList m_accountListingCache;
    QHash<QString,QString> m_tranzlator;

    /*! The ID of the last invited user. We have to cache this due to the lack of transactions. */
    int m_invitedUserId;

    /*! Contains the number of milliseconds between PING and reply commands. sendPing() has to be
        sent at least once before this value is valid.*/
    int m_pingLatency;

    /*! Timer used to measure the PING response time. */
    QTime pingLagTimer;
    /*! The PING timer used to send keep-alive PING messages. */
    int pingTimerId;
    void timerEvent(QTimerEvent *event);
};

#endif
