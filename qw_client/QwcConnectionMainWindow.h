#ifndef QWCCONNECTIONMAINWINDOW_H
#define QWCCONNECTIONMAINWINDOW_H

#include <QtCore/QDateTime>
#include <QtCore/QPointer>

#include "ui_QwcConnectionMainWindow.h"

class QwcSocket;
class QwcAboutWidget;

class QwcConnectionMainWindow :
        public QMainWindow,
        protected Ui_QwcConnectionMainWindow
{
    Q_OBJECT

public:
    /*! A list of 'actions' that a user may cause and which are handled somewhere else. */
    enum TriggeredAction {
        TriggeredActionNewConnection,
        TriggeredActionDisconnect,
        TriggeredActionChat,
        TriggeredActionAccounts,
        TriggeredActionNews,
        TriggeredActionMessages,
        TriggeredActionServerInformation,
        TriggeredActionFiles,
        TriggeredActionPreferences,
        TriggeredActionReconnect,
        TriggeredActionTransfers,
        TriggeredActionPostNews,
        TriggeredActionBroadcast
    };

    QwcConnectionMainWindow(QWidget *parent = 0);

    QPointer<QwcAboutWidget> pQwcAboutWidget;
    void setSocket(QwcSocket *socket);
    QwcSocket* socket();


protected:
    QwcSocket *m_socket;

signals:
    void actionTriggered(QwcConnectionMainWindow::TriggeredAction action);

public slots:
    void toggleVisible();
	
private slots:
    void handleSocketNewsPosted(QString nickname, QDateTime time, QString post);
    void handleSocketReceivedPrivileges();
    void handleSocketConnected();
    void handleSocketDisconnected();
    void handleSocketLoginSuccessful();

    void on_actionAbout_triggered(bool);
    void on_actionDonate_triggered();
    void on_actionNewConnection_triggered();
    void on_actionChat_triggered();
    void on_actionDisconnect_triggered();
    void on_actionReconnect_triggered();
    void on_actionServerInfo_triggered();
    void on_actionNews_triggered();
    void on_actionMessages_triggered();
    void on_actionFiles_triggered();
    void on_actionTransfers_triggered();
    void on_actionAccounts_triggered();
    void on_actionBroadcast_triggered();
    void on_actionPreferences_triggered();

};

#endif
