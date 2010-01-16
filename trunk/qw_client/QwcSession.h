#ifndef QWCSESSION_H
#define QWCSESSION_H

#include <QObject>
#include <QStackedLayout>

#include "QwcChatWidget.h"
#include "QwcConnectWidget.h"
#include "QwcConnectionMainWindow.h"
#include "QwcNewsWidget.h"
#include "QwcPreferencesWidget.h"
#include "QwcConnectWidget.h"
#include "QwcFiletransferWidget.h"
#include "QwcAccountsWidget.h"
#include "QwcServerInfoWidget.h"
#include "QwcPrivateMessager.h"
#include "QwcUserInfoWidget.h"

class QwcSocket;

namespace Qwired {
    enum Event { ServerConnected, ServerDisconnected, ServerError, UserJoined, UserChangedNick,
                 UserChangedStatus, UserLeft, ChatReceived, MessageReceived, NewsPosted,
                 BroadcastReceived, TransferStarted, TransferFinished };
}


class QwcFileBrowserWidget;

class QwcSession :
        public QObject
{
    Q_OBJECT

public:
    QwcSession(QObject *parent = 0);
    ~QwcSession();

    QwcSocket* socket();

    bool eventFilter(QObject *watched, QEvent *event);




    // Main window widgets
    void initMainWindow();
    QPointer<QwcPrivateMessager> privateMessager;

    // Wired Socket
    void initWiredSocket();


    // Connection dialog widgets
    QPointer<QwcConnectionMainWindow> m_mainWindow;
    QPointer<QwcConnectWidget> m_connectWidget;
    QPointer<QwcChatWidget> m_publicChat;
    QPointer<QTabWidget> connectionTabWidget;
    QPointer<QStackedWidget> connectionStackedWidget;

    QPointer<QwcNewsWidget> m_newsWidget;
    QHash<int, QPointer<QwcChatWidget> > m_chatWidgets;
    QPointer<QwcServerInfoWidget> m_serverInfoWidget;
    QPointer<QwcPreferencesWidget> m_preferencesWidget;
    QPointer<QwcFiletransferWidget> m_transfersWidget;
    QPointer<QwcAccountsWidget> m_accountsWidget;
    QPointer<QwcFileBrowserWidget> m_fileBrowserWidget;
    /*! A list of QwcUserInfoWidgets, identified by the \a userId. */
    QHash<int,QPointer<QwcUserInfoWidget> > m_userInfoWidgets;

    QPointer<QMenu> m_systemTrayMenu;

    bool confirmDisconnection();

private:
    QPointer<QwcSocket> m_socket;
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


    // File browser
    void handleTransferComplete(const QwcTransferInfo &transfer);

    // Chat Window
    void onUserlistComplete(int chatId);

    void handlePrivateMessage(QwcUserInfo sender, QString text);

    void connectionWindowDestroyed(QObject *obj);
    void reloadPreferences();
    void onSocketPrivileges(QwcUserInfo);

    void handleFileInformation(QwcFileInfo file);

    void userJoined(int theChat, QwcUserInfo theUser);
    void userLeft(int theChat, QwcUserInfo theUser);
    void userChanged(QwcUserInfo theOld, QwcUserInfo theNew);
    void newsPosted(QString nickname, QDateTime time, QString post);

    void handleBroadcastMessage(QwcUserInfo theUser, QString theMessage);

public slots:

    void showMessagerForUser(const QwcUserInfo targetUser);

    void triggerEvent(QString event, QStringList parameters);
    void setTrayMenuAction(QMenu*);

    void do_handle_chat_message(int, int, QString, bool);

    // Acount buttons from connect window.
    void doActionAccounts();
    void doActionDisconnect();
    void doActionNews();
    void doActionMessages();
    void doActionServerInfo();
    void doActionNewConnection();
    void doActionBroadcast();
    void doActionFiles(QString initialPath="/");
    void doActionPublicChat();

    // Toolbar handlers
    void doActionPreferences();
    void doActionTransfers();


    void search_download_file(QString);
    void search_reveal_file(QString);



    // Connect window
    void onDoConnect(QString theHost, QString theLogin, QString thePassword);

    // Socket handlers
    void handleChatTopic(int chatId, QString nickname, QString login, QHostAddress userIp, QDateTime date, QString topic);
    void handleUserInformation(QwcUserInfo user);

    void doHandlePrivateChatInvitation(int theChatID, QwcUserInfo theUser);
    void doCreateNewChat(int theChatID);

    void handleServerInformation();
    void onLoginSuccessful();

    void setBannerView(const QPixmap banner);


};

#endif
