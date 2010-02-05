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
    friend class QwcSingleton;

public:
    QwcSession(QObject *parent = 0);
    ~QwcSession();

    QwcSocket* socket();



    QPointer<QMenu> m_systemTrayMenu;

    bool confirmDisconnection();

protected:
    QPointer<QwcSocket> m_socket;
    // Connection dialog widgets
    QPointer<QwcConnectionMainWindow> m_mainWindow;
    QPointer<QwcConnectWidget> m_connectWidget;
    QPointer<QwcChatWidget> m_publicChat;
    QPointer<QTabWidget> m_connectionTabWidget;
    QPointer<QStackedWidget> m_connectionStackedWidget;
    QPointer<QwcPrivateMessager> m_privateMessagerWidget;

    QPointer<QwcNewsWidget> m_newsWidget;
    QHash<int, QPointer<QwcChatWidget> > m_chatWidgets;
    QPointer<QwcServerInfoWidget> m_serverInfoWidget;
    QPointer<QwcPreferencesWidget> m_preferencesWidget;
    QPointer<QwcFiletransferWidget> m_transfersWidget;
    QPointer<QwcAccountsWidget> m_accountsWidget;
    QPointer<QwcFileBrowserWidget> m_fileBrowserWidget;

    void initializeSocket();
    void initializeMainWindow();
    bool eventFilter(QObject *watched, QEvent *event);

private slots:

    void reloadPreferences();

    // Protocol + Socket
    void connectionWindowDestroyed(QObject *object);
    void handleProtocolError(Qw::ProtocolError error);
    void handleSocketError(QAbstractSocket::SocketError error);
    void handleSocketChatInvitation(int chatId, QwcUserInfo inviter);

    // Main window widgets
    void handleMainWindowAction(QwcConnectionMainWindow::TriggeredAction action);
    void onTabBarCurrentChanged(int index);
    void onTabBarCloseRequested(int index);


    void handleSocketUserlistComplete(int chatId);

    void handleFileInformation(const QwFile &file);

    void userJoined(int theChat, QwcUserInfo theUser);
    void userLeft(int theChat, QwcUserInfo theUser);
    void userChanged(QwcUserInfo theOld, QwcUserInfo theNew);
    void newsPosted(QString nickname, QDateTime time, QString post);
    void handleBroadcastMessage(QwcUserInfo theUser, QString theMessage);
    void handleUserInformation(QwcUserInfo user);

    void showMessagerForUser(const QwcUserInfo &targetUser, const QString &message = QString());
    void triggerEvent(QString event, QStringList parameters);
    void handleSocketChatMessage(int chatId, int userId, const QString &text, bool isEmote);
    void createChatWidget(int chatId);



};

#endif
