#ifndef QWCSESSION_H
#define QWCSESSION_H

class QwcSocket;
class QwcPrivateMessager;
class QwcConnectWidget;
class QwcChatWidget;
class QwcNewsWidget;
class QwcServerInfoWidget;
class QwcPreferencesWidget;
class QwcFiletransferWidget;
class QwcAccountsWidget;
class QwcUserInfoWidget;

#include "QwGlobals.h"
#include "QwcUserInfo.h"
#include "QwcFileInfo.h"
#include "QwcTransferInfo.h"
#include "QwcConnectionMainWindow.h"

#include <QtCore/QObject>
#include <QtCore/QPointer>

#include <QtGui/QTabWidget>
#include <QtGui/QStackedWidget>
#include <QtGui/QMenu>

#include <QtNetwork/QAbstractSocket>

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


    // Wired Socket
    void initWiredSocket();


    // Connection dialog widgets
    QPointer<QwcConnectionMainWindow> m_mainWindow;
    QPointer<QwcConnectWidget> m_connectWidget;
    QPointer<QwcChatWidget> m_publicChat;
    QPointer<QTabWidget> connectionTabWidget;
    QPointer<QStackedWidget> connectionStackedWidget;
    QPointer<QwcPrivateMessager> m_privateMessagerWidget;

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

private slots:
    // Protocol + Socket
    void connectionWindowDestroyed(QObject *object);
    void handleProtocolError(Qw::ProtocolError error);
    void handleSocketError(QAbstractSocket::SocketError error);


    // Main window widgets
    void onTabBarCloseButtonClicked();
    void onTabBarCurrentChanged(int index);
    void onTabBarCloseRequested(int index);

    void handleMainWindowAction(QwcConnectionMainWindow::TriggeredAction action);


    // Chat Window
    void handleSocketUserlistComplete(int chatId);

    void handlePrivateMessage(QwcUserInfo sender, QString text);


    void reloadPreferences();


    void handleFileInformation(QwcFileInfo file);

    void userJoined(int theChat, QwcUserInfo theUser);
    void userLeft(int theChat, QwcUserInfo theUser);
    void userChanged(QwcUserInfo theOld, QwcUserInfo theNew);
    void newsPosted(QString nickname, QDateTime time, QString post);

    void handleBroadcastMessage(QwcUserInfo theUser, QString theMessage);

public slots:

    void showMessagerForUser(const QwcUserInfo targetUser);

    void triggerEvent(QString event, QStringList parameters);

    void do_handle_chat_message(int, int, QString, bool);

    // Socket handlers
    void handleChatTopic(int chatId, QString nickname, QString login, QHostAddress userIp, QDateTime date, QString topic);
    void handleUserInformation(QwcUserInfo user);

    void doHandlePrivateChatInvitation(int theChatID, QwcUserInfo theUser);
    void createChatWidget(int chatId);



};

#endif
