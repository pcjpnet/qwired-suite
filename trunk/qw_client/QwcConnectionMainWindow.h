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

    QPointer<QwcAboutWidget> pQwcAboutWidget;
    QwcConnectionMainWindow(QWidget *parent = 0);

    void setSocket(QwcSocket *socket);
    QwcSocket* socket();


protected:
    QwcSocket *m_socket;

signals:
    void actionTriggered(TriggeredAction action);

public slots:
    void toggleVisible();
	
private slots:
    void handleSocketNewsPosted(QString nickname, QDateTime time, QString post);

    void on_actionAbout_triggered(bool);
    void on_actionDonate_triggered();
    void on_actionNewConnection_triggered();
    void on_actionDisconnect_triggered();
    void on_actionReconnect_triggered();
    void on_actionServerInfo_triggered();
    void on_actionServerNews_triggered();
    void on_actionServerMessages_triggered();
    void on_actionServerFiles_triggered();
    void on_actionServerTransfers_triggered();
    void on_actionServerAccounts_triggered();
    void on_actionServerBroadcast_triggered();
    void on_actionServerPreferences_triggered();

};

#endif
