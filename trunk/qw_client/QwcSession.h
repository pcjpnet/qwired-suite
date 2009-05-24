#ifndef QWCSESSION_H
#define QWCSESSION_H

#include <QObject>
#include <QStackedLayout>

#include "QwcChatWidget.h"
#include "QwcConnectionMainWindow.h"
#include "QwcSocket.h"

#include "QwcEventFilter.h"

#include "QwcNewsWidget.h"
#include "QwcUserlistModel.h"
#include "QwcPreferencesWidget.h"
#include "QwcConnectWidget.h"
#include "QwcFiletransferWidget.h"
#include "QwcFileSearchWidget.h"
#include "QwcAccountsWidget.h"
#include "QwcTrackerlistWidget.h"
#include "QwcFileInfoWidget.h"
#include "QwcServerInfoWidget.h"
#include "QwcPrivateMessager.h"


namespace Qwired {
    enum Event { ServerConnected,
                 ServerDisconnected,
                 ServerError,
                 UserJoined,
                 UserChangedNick,
                 UserChangedStatus,
                 UserLeft,
                 ChatReceived,
                 MessageReceived,
                 NewsPosted,
                 BroadcastReceived,
                 TransferStarted,
                 TransferFinished };
};


class QwcFileBrowserWidget;

class QwcSession : public QObject
{
    Q_OBJECT

public:
    QwcSession(QObject *parent = 0);
    ~QwcSession();



    /*! Returns a pointer to the underlying protocol socket. */
    QwcSocket* wiredSocket() { return socket; }

    // Main window widgets
    void initMainWindow();
//    QPointer<QWidget> pContainerWidget;
//    QPointer<QStackedLayout> pContainerLayout;
//    QPointer<QTabWidget> pMainTabWidget;
    QPointer<QwcPrivateMessager> privateMessager;

    // Wired Socket
    void initWiredSocket();


    // Connection dialog widgets
    QPointer<QwcConnectionMainWindow> connectionWindow;
    QPointer<QwcConnectWidget> connectWidget;
    QPointer<QTabWidget> connectionTabWidget;
    QPointer<QwcChatWidget> mainChatWidget;
    QPointer<QStackedWidget> connectionStackedWidget;



    QPointer<QwcNewsWidget> pWinNews;
    QPointer<QwcUserlistModel> pUserListModel;
    QHash<int, QPointer<QwcChatWidget> > pChats;
    QHash<QString,QPointer<QwcFileInfoWidget> > pFileInfos;

    QPointer<QwcServerInfoWidget> pServerWindow;
    QPointer<QwcPreferencesWidget> pPrefsWindow;
//    QPointer<QwcConnectWidget> pConnectWindow;
    QPointer<QwcFiletransferWidget> pTranfersWindow;
    QPointer<QwcFileSearchWidget> pFileSearch;
    QPointer<QwcAccountsWidget> pWinAccounts;
    QPointer<QwcTrackerlistWidget> pWinTrackers;
    QPointer<QwcFileBrowserWidget> pWinFileBrowser;

    QWidget *bannerSpace;
    QLabel *bannerView;
    QWidget *bannerSpace2;

    QwcEventFilter *pEventFilter;

    QPointer<QMenu> pTrayMenuItem;

    // Misc
    bool confirmDisconnection();

private:
    QPointer<QwcSocket> socket;
    void setupConnections();
    void setConnectionToolButtonsEnabled(bool);


private slots:
    // Protocol + Socket
    void handleProtocolError(Qw::ProtocolError error);
    void handleSocketError(QAbstractSocket::SocketError error);


    // Main window widgets
    void onTabBarCloseButtonClicked();
    void onTabBarCurrentChanged(int index);
    void onTabBarCloseRequested(int index);


    // Connect widget
    void onConnectAborted();


    // Chat Window
    void onUserlistComplete(int chatId);

    void connectionWindowDestroyed(QObject *obj);
    void reloadPreferences();
    void onSocketPrivileges(QwcUserInfo);
    void doActionTrackers();

    void fileInformation(QwcFileInfo theFile);

    void userJoined(int theChat, QwcUserInfo theUser);
    void userLeft(int theChat, QwcUserInfo theUser);
    void userChanged(QwcUserInfo theOld, QwcUserInfo theNew);
    void newsPosted(QString theNick, QString theTime, QString thePost);


    void transferStarted(QwcTransferInfo);
    void transferDone(QwcTransferInfo);
    void transferSocketError(QAbstractSocket::SocketError);
    void transferError(QwcTransferInfo);

//    void fileListingRecursiveDone(const QList<QwcFileInfo>);



public slots:

    void showMessagerForUser(const QwcUserInfo targetUser);

    void triggerEvent(QString event, QStringList parameters);
    void setTrayMenuAction(QMenu*);

    void do_handle_chat_message(int, int, QString, bool);

    // Acount buttons from connect window.
    void doActionAccounts();
    void doActionDisconnect();
    void doActionNews();
    void doActionServerInfo();
    void doActionNewConnection();
    void doActionBroadcast();
    void doActionFiles(QString thePath="/");
    void doActionPublicChat();

    // Toolbar handlers
    void doActionPreferences();
    void doActionTransfers();
    void doActionFileSearch();


    void search_download_file(QString);
    void search_reveal_file(QString);



    // Connect window
    void onDoConnect(QString theHost, QString theLogin, QString thePassword);

    // Socket handlers
    void doHandleChatTopic(int theChatID, QString theNick, QString theLogin, QHostAddress theIP, QDateTime theDateTime, QString theTopic);
    void doHandleUserInfo(QwcUserInfo theUser);
    void doHandlePrivateChatInvitation(int theChatID, QwcUserInfo theUser);
    void doCreateNewChat(int theChatID);

    void onSocketServerInfo();
    void onLoginSuccessful();

    void setBannerView(const QImage theBanner);

    void downloadFolder(const QString &remotePath, const QString &localPath);
    void uploadFolder(const QString &localPath, const QString &remotePath);
//    void checkTransferQueue();


};

#endif