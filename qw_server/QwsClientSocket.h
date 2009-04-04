/***************************************************************************
 * QWired - a free Wired client.
 * Copyright (C) 2008 Bastian Bense
 ***************************************************************************
 * This program is free software; you can redistribute it and/or modify it *
 * under the terms of the GNU General Public License as published by the   *
 * Free Software Foundation; either version 2 of the License, or           *
 * (at your option) any later version.                                     *
 *                                                                         *
 * This program is distributed in the hope that it will be useful, but     *
 * WITHOUT ANY WARRANTY; without even the implied warranty of              *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU        *
 * General Public License for more details.                                *
 *                                                                         *
 * You should have received a copy of the GNU General Public License along *
 * with this program; if not, write to the                                 *
 * Free Software Foundation, Inc.,                                         *
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA                   *
 ***************************************************************************/

 
#ifndef WIREDSOCKET_H
#define WIREDSOCKET_H

#include "QwsUser.h"
#include "QwsFile.h"
#include "QwSocket.h"
#include "QwRoom.h"

#include <QtCore>
#include <QtNetwork>


namespace Qws {
    enum SessionState { StateInactive /*! TCP-unknown, no handshake, not logged in */,
                        StateConnected /*! TCP connected, handshake OK, not logged in */,
                        StateActive /*! TCP connected, handshake OK, logged in (ready) */
                      };

    enum ProtocolError { ErrorComandFailed = 500,
                         ErrorCommandNotRecognized,
                         ErrorCommandNotImplemented,
                         ErrorSyntaxError,
                         ErrorLoginFailed = 510,
                         ErrorBanned,
                         ErrorClientNotFound,
                         ErrorAccountNotFound,
                         ErrorAccountExists,
                         ErrorCannotBeDisconnected,
                         ErrorPermissionDenied,
                         ErrorFileOrDirectoryNotFound = 520,
                         ErrorFileOrDirectoryExists,
                         ErrorChecksumMismatch,
                         ErrorQueueLimitExceeded
                        };
}



/*! \class QwsClientSocket
    \author Bastian Bense <bb@bense.de>

*/

Q_DECLARE_METATYPE(Qws::SessionState);


class QwsClientSocket : public QwSocket
{
    Q_OBJECT
    friend class QwsServerController;

public:
    QwsClientSocket(QObject *parent=0);
    ~QwsClientSocket();


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
    void sendError(const Qws::ProtocolError error);
    void sendServerInfo();
    void disconnectClient();
    void idleTimerTriggered();
    void resetIdleTimer();
    void resolveHostname();


private slots:

    // TCP/IP/SSL
    void handleHostLookupResult(QHostInfo hostInfo);
    void handleIncomingMessage(QwMessage message);

    // Protocol
    void handleMessageHELLO(QwMessage &message);
    void handleMessageCLIENT(QwMessage &message);
    void handleMessageNICK(QwMessage &message);
    void handleMessagePASS(QwMessage &message);
    void handleMessageUSER(QwMessage &message);
    void handleMessagePING(QwMessage &message);
    void handleMessageSTATUS(QwMessage &message);
    void handleMessageWHO(QwMessage &message);
    void handleMessageICON(QwMessage &message);
    void handleMessageBANNER(QwMessage &message);
    void handleMessageINFO(QwMessage &message);
    void handleMessagePRIVILEGES(QwMessage &message);

    // Communication
    void handleMessageSAY(QwMessage &message);
    void handleMessageME(QwMessage &message);
    void handleMessageMSG(QwMessage &message);
    void handleMessageBROADCAST(QwMessage &message);
    void handleMessageTOPIC(QwMessage &message);
    void handleMessagePRIVCHAT(QwMessage &message);
    void handleMessageINVITE(QwMessage &message);
    void handleMessageJOIN(QwMessage &message);
    void handleMessageDECLINE(QwMessage &message);
    void handleMessageLEAVE(QwMessage &message);

    // News
    void handleMessageNEWS(QwMessage &message);
    void handleMessagePOST(QwMessage &message);
    void handleMessageCLEARNEWS(QwMessage &message);

    // Administration
    void handleMessageKICK(QwMessage &message);
    void handleMessageBAN(QwMessage &message);
    void handleMessageUSERS(QwMessage &message);
    void handleMessageGROUPS(QwMessage &message);
    void handleMessageREADUSER(QwMessage &message);
    void handleMessageEDITUSER(QwMessage &message);
    void handleMessageCREATEUSER(QwMessage &message);
    void handleMessageDELETEUSER(QwMessage &message);
    void handleMessageREADGROUP(QwMessage &message);
    void handleMessageCREATEGROUP(QwMessage &message);
    void handleMessageEDITGROUP(QwMessage &message);
    void handleMessageDELETEGROUP(QwMessage &message);

    // Files
    void handleMessageLIST(QwMessage &message);
    void handleMessageLISTRECURSIVE(QwMessage &message);
    void handleMessageSTAT(QwMessage &message);
    void handleMessageFOLDER(QwMessage &message);
    void handleMessageDELETE(QwMessage &message);
    void handleMessageMOVE(QwMessage &message);
    void handleMessageGET(QwMessage &message);
    void handleMessagePUT(QwMessage &message);
    void handleMessageSEARCH(QwMessage &message);

    void on_socket_sslErrors(const QList<QSslError> & errors);
    void on_socket_error();


private:
    /*! Defines the current state of the session/socket. */
    Qws::SessionState sessionState;


    void deleteDirRecursive(QString pathToDir);

    /*! This is the pointer to our SSL socket. We use this as input/output to the client. */
    QPointer<QSslSocket> pSocket;

};

#endif
