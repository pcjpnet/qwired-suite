#ifndef QWCSOCKET_H
#define QWCSOCKET_H

#include "QwSocket.h"
#include "QwServerInfo.h"

#include "QwRoom.h"
#include "QwcUserInfo.h"
#include "QwcTransferInfo.h"
#include "QwcFileInfo.h"
#include "QwcTransfer.h"
#include "QwcTrackerServerInfo.h"


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

    const QwcUserInfo & sessionUser() const;
    const QwServerInfo & serverInformation() const;
    const QImage & serverBanner() const;
    int pingLatency() const;

    const QHash<int, QwRoom> & chatRooms() const;
    const QHash<int, QwcUserInfo> & users() const;

    // User Information
    void setNickname(QString);
    void setUserAccount(QString, QString);

    void setLocalTransferQueueEnabled(bool enabled);
    bool localTransferQueueEnabled() const;


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
    void declineChatInvitation(int theChatID);
    void createChatWithClient(int firstInvitedUser = 0);
    void setChatTopic(int chatId, QString topic);
    void sendPrivateMessage(int userId, QString message);
    void writeToChat(int chat, QString text, bool emote);
    void inviteClientToChat(int chatId, int userId);
    void joinChat(int theChatID);
    void getUserlist(int roomId);

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

    // Files
    //
    void createFolder(const QString &path);
    void deleteFile(const QString &path);
    void setFileComment(const QString &path, const QString &comment);
    void getFileInformation(const QString &path);
    void searchFiles(const QString &search);
    void getFileList(const QString &path);
    void getFileListRecusive(const QString &path);
    void moveFile(const QString &source, const QString &destination);
    void getFile(const QString &path, qint64 offset);
    void putFile(const QString &path, qint64 size, const QString &checksum);
    void setFolderType(const QString &path, Qw::FileType type);

    // Transfers
    QwcTransfer* downloadPath(const QString &remotePath, const QString &localPath);
    QwcTransfer* uploadPath(const QString &localPath, const QString &remotePath);

    bool removeTransfer(QwcTransfer *transfer);

    void handleTransferChanged();
    void handleTransferEnded();


private slots:
    void handleMessageReceived(const QwMessage &message);

    // Connection
    void handleSocketConnected();
    void handleSocketConnectionLost();



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

    /*! A transfer task was created by \e downloadPath() or \e uploadPath(). A transfer is the
        "transfer task", for instance, a folder download and not the transfer of a single file. */
    void transferCreated(QwcTransfer *transfer);
    /*! A transfer task was removed from the internal structure. */
    void transferRemoved(QwcTransfer *transfer);
    /*! A transfer changed. */
    void transferChanged(QwcTransfer *transfer);


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
    void privateChatDeclined(int chatId, const QwcUserInfo &user);



    /*! This signal is emitted for each returned search result of the last. */
    void fileSearchResultListItem(QwcFileInfo item);
    /*! This signal is emitted when all results have been received. */
    void fileSearchResultListDone();

    /*! This signal is emitted when a new file list item has been received. */
    void onFilesListItem(const QwcFileInfo &file);
    /*! This signal is emitted when the file listing is complete and also provides some information
        about the server. */
    void onFilesListDone(const QString &path, qint64 freeSpace);
    void onFilesListRecursiveDone(const QList<QwcFileInfo>);

    /*! In response to a GET or PUT command. Indicates that a transfer is ready. */
    void transferReady(const QString &path, qint64 offset, const QString &hash);

    /*! In response to a GET or PUT command. Indicates that the transfer is queued on the server
        at \e position. */
    void transferQueued(const QString &path, int position);

    /*! Emitted when a response to the STAT command is received (get file information). */
    void fileInformation(const QwFile &information);



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


protected:
    /*! The client user object which keeps track of available information of the current session. */
    QwcUserInfo m_sessionUser;
    QwServerInfo m_serverInformation;
    QImage m_serverBanner;
    QString m_clientName;
    QString m_clientVersion;
    QString m_serverAddress;
    int m_serverPort;
    bool m_localTransferQueueEnabled;

    QList<QwcTransfer*> m_transfers;

    /*! A list of all currently used chat rooms. Chat rooms contain user lists and refer to them by
        their user ID. */
    QHash<int, QwRoom> m_chatRooms;

    /*! This contains all users registered for the public user list (1). */
    QHash<int, QwcUserInfo> m_users;

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
    void handleMessage332(const QwMessage &message);
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
