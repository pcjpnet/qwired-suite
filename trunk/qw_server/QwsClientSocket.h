#ifndef WIREDSOCKET_H
#define WIREDSOCKET_H

#include "QwGlobals.h"
#include "QwsUser.h"
#include "QwsFile.h"
#include "QwSocket.h"
#include "QwRoom.h"
#include "QwServerInfo.h"

#include <QtCore>
#include <QtNetwork>

namespace Qws {
    enum SessionState{
        /*! TCP-unknown, no handshake, not logged in */
        StateInactive,
        /*! TCP connected, handshake OK, not logged in */
        StateConnected,
        /*! TCP connected, handshake OK, logged in (ready) */
        StateActive
    };
}



/*! \class QwsClientSocket
    \author Bastian Bense <bb@bense.de>
    \brief This class allows direct communication to a Qwired client and implementes the
           server-side part of the protocol.
*/

Q_DECLARE_METATYPE(Qws::SessionState);


class QwsClientSocket
        : public QwSocket
{
    Q_OBJECT

    friend class QwsServerController;

public:
    QwsClientSocket(QObject *parent=0);
    ~QwsClientSocket();

    /*! A pointer to the information about this server. */
    QwServerInfo *serverInfo;

    /*! This timer is responsible for automatically setting the user as "inactive" after a certain
        period of time. */
    QTimer *pIdleTimer;
    /*! This is an instace representing the user account and information for this connection. */
    QwsUser user;
    /*! Defines the root path of the files directory for this connection. */
    QString filesRootPath;
		
		
signals:
    /*! This signal is emitted when a message must be sent to all users in a specific room, but
        there is no further action from QwsServerController required. */
    void broadcastedMessage(const QwMessage message, const int roomId, const bool sendToSelf);
    /*! This signal is emitted when the client issues a WHO command. */
    void requestedUserlist(const int roomId);
    /*! This signal is emitted whenever the session state changes. */
    void sessionStateChanged(const Qws::SessionState state);
    /*! This signal is emitted when there is chat text received from the client for a room. */
    void requestedChatRelay(const int roomId, const QString text, const bool isEmote);
    /*! This signal is emitted when the client wants to send a private message to another user. */
    void requestedMessageRelay(const int userId, const QString text);
    /*! This signal is emitted when the user information changes. This includes status, idle state,
        nickname, icon, image and the admin flag. */
    void userStatusChanged();
    /*! This signal is emitted if the user wants to change the topic of a chat. */
    void requestedRoomTopicChange(const int roomId, const QString topic);
    /*! This signal is emitted if the client needs to be sent the topic of a room. (After login) */
    void requestedRoomTopic(const int roomId);
    /*! This signal is emitted when the client requests information about a client. */
    void receivedMessageINFO(const int userId);
    /*! This signal is emitted when the client wants to create a new chat room. */
    void requestedNewRoom();
    /*! This signal is emitted when the client invites a user to a room. */
    void requestedUserInviteToRoom(const int userId, const int roomId);
    /*! This signal is emitted when a client decides to join a room by invitation. */
    void receivedMessageJOIN(const int roomId);
    /*! This signal is emitted when a client declines to join an invited room. */
    void receivedMessageDECLINE(const int roomId);
    /*! This signal is emitted when a client leaves a chat room. */
    void receivedMessageLEAVE(const int roomId);
    /*! This signal is emitted when the client kicked/banned another user. */
    void receivedMessageBAN_KICK(const int userId, const QString reason, const bool isBan);
    /*! This signal is emitted when the client modified a user account and it needs to be reloaded. */
    void modifiedUserAccount(const QString name);
    /*! This signal is emitted when the client modified a group and it needs to be reloaded. */
    void modifiedUserGroup(const QString name);
    /*! This signal is emitted when the client requests (GET) a file from the server. */
    void receivedMessageGET(const QwsFile file);
    /*! This signal is emitted when the client requests (PUT) a upload to the server. */
    void receivedMessagePUT(const QwsFile file);
		

public slots:
    void sendError(const Qw::ProtocolError error);
    void sendServerInfo();
    void disconnectClient();
    void idleTimerTriggered();
    void resetIdleTimer();
    void resolveHostname();


private slots:

    // TCP/IP/SSL
    void handleHostLookupResult(QHostInfo hostInfo);

    // Protocol
    void handleMessageHELLO(const QwMessage &message);
    void handleMessageCLIENT(const QwMessage &message);
    void handleMessageNICK(const QwMessage &message);
    void handleMessagePASS(const QwMessage &message);
    void handleMessageUSER(const QwMessage &message);
    void handleMessagePING(const QwMessage &message);
    void handleMessageSTATUS(const QwMessage &message);
    void handleMessageWHO(const QwMessage &message);
    void handleMessageICON(const QwMessage &message);
    void handleMessageBANNER(const QwMessage &message);
    void handleMessageINFO(const QwMessage &message);
    void handleMessagePRIVILEGES(const QwMessage &message);

    // Communication
    void handleMessageSAY(const QwMessage &message);
    void handleMessageME(const QwMessage &message);
    void handleMessageMSG(const QwMessage &message);
    void handleMessageBROADCAST(const QwMessage &message);
    void handleMessageTOPIC(const QwMessage &message);
    void handleMessagePRIVCHAT(const QwMessage &message);
    void handleMessageINVITE(const QwMessage &message);
    void handleMessageJOIN(const QwMessage &message);
    void handleMessageDECLINE(const QwMessage &message);
    void handleMessageLEAVE(const QwMessage &message);

    // News
    void handleMessageNEWS(const QwMessage &message);
    void handleMessagePOST(const QwMessage &message);
    void handleMessageCLEARNEWS(const QwMessage &message);

    // Administration
    void handleMessageKICK(const QwMessage &message);
    void handleMessageBAN(const QwMessage &message);
    void handleMessageUSERS(const QwMessage &message);
    void handleMessageGROUPS(const QwMessage &message);
    void handleMessageREADUSER(const QwMessage &message);
    void handleMessageEDITUSER(const QwMessage &message);
    void handleMessageCREATEUSER(const QwMessage &message);
    void handleMessageDELETEUSER(const QwMessage &message);
    void handleMessageREADGROUP(const QwMessage &message);
    void handleMessageCREATEGROUP(const QwMessage &message);
    void handleMessageEDITGROUP(const QwMessage &message);
    void handleMessageDELETEGROUP(const QwMessage &message);

    // Files
    void handleMessageLIST(const QwMessage &message);
    void handleMessageLISTRECURSIVE(const QwMessage &message);
    void handleMessageSTAT(const QwMessage &message);
    void handleMessageFOLDER(const QwMessage &message);
    void handleMessageDELETE(const QwMessage &message);
    void handleMessageMOVE(const QwMessage &message);
    void handleMessageGET(const QwMessage &message);
    void handleMessagePUT(const QwMessage &message);
    void handleMessageSEARCH(const QwMessage &message);
    void handleMessageCOMMENT(const QwMessage &message);
    void handleMessageTYPE(const QwMessage &message);

    void on_socket_sslErrors(const QList<QSslError> & errors);
    void on_socket_error();


protected:
    /*! Re-implemented from QwSocket */
    void handleMessageReceived(const QwMessage &message);

    /*! Defines the current state of the session/socket. */
    Qws::SessionState m_sessionState;


    void deleteDirRecursive(QString pathToDir);

    /*! This is the pointer to our SSL socket. We use this as input/output to the client. */
    QPointer<QSslSocket> pSocket;

};

#endif
