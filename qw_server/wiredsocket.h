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

#include "classwireduser.h"
#include "classwiredfile.h"

#include <QtCore>
#include <QtNetwork>

#include "QwSocket.h"
#include "QwsRoom.h"

#include "classwireduser.h"

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



/*! \class WiredSocket
    \author Bastian Bense <bb@bense.de>

*/

const int kEOF = 0x04;
const int kFS = 0x1C;

class WiredSocket : public QwSocket
{
    friend class QwsServerController;

    Q_OBJECT

public:
    WiredSocket(QObject *parent = 0);
    ~WiredSocket();

    ClassWiredUser sessionUser() { return pSessionUser; };
    int userId() { return pSessionUser.pUserID; }
    bool pLoggedIn;
    bool isLoggedIn() { return pLoggedIn; };
    ClassWiredUser pSessionUser;
    QTimer* pIdleTimer;

    ClassWiredUser user;
		
		
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
    void requestedClientInfo(const int userId);
    /*! This signal is emitted when the client wants to create a new chat room. */
    void requestedNewRoom();
    /*! This signal is emitted when the client invites a user to a room. */
    void requestedUserInviteToRoom(const int userId, const int roomId);
    /*! This signal is emitted when a client decides to join a room by invitation. */
    void requestedUserJoinRoom(const int roomId);


		void clearedNews(const int id);
		void clientDisconnected(const int id);
		void createdUser(const int id, const ClassWiredUser user);
		void createdGroup(const int id, const ClassWiredUser group);
		void editedUser(const int id, const ClassWiredUser user);
		void editedGroup(const int id, const ClassWiredUser group);
		void deletedUser(const int id, const QString name);
		void deletedGroup(const int id, const QString name);
		void declinedPrivateChat(const int id, const int chatId);
		void handshakeComplete(const int id);
		void invitedUserToChat(const int id, const int userId, const int chatId);
		void joinedPrivateChat(const int id, const int chatId);
		void loginReceived(const int id, QString login, QString password);
		void newsPosted(const int id, const QString news);
		void privateChatLeft(const int id, const int chatId);
		void receivedClientInfo(const int id, const QString info);


                void requestedAccountsList(const int id);
		void requestedBanner(const int id);
		void requestedFileList(const int id, const QString path);
		void requestedFileStat(const int id, const QString path);
		void requestedGroupsList(const int id);
		void requestedNews(const int id);
		void requestedPrivateChat(const int id);
		void requestedReadUser(const int id, const QString name);
		void requestedReadGroup(const int id, const QString name);
		void requestedUserInfo(const int id, const int userId);

		void topicChanged(const ClassWiredUser user, const int chatId, const QString topic);
		void userImageChanged(const ClassWiredUser user);
		void userKicked(const int id, const int userId, const QString reason, const bool banned);

		

		
public slots:
    void sendUserlistItem(const int roomId, const ClassWiredUser &item);
    void sendUserlistDone(const int roomId);
    void sendClientJoin(const int chatId, const ClassWiredUser &user);
    void sendChat(const int chatId, const int userId, const QString text, const bool isEmote);
    void sendError(const Qws::ProtocolError error);
    void sendServerInfo();



		void disconnectClient();
		
		void setClientInfo(const QString info);

		void sendAccountListing(const QString name);
		void sendAccountListingDone();
		void sendGroupListing(const QString name);
		void sendGroupListingDone();
		void sendUserSpec(const ClassWiredUser account);
		void sendGroupSpec(const ClassWiredUser group);

		void sendFileListing(const ClassWiredFile file);
		void sendFileListingDone(const QString path, const int free);
		void sendFileStat(const ClassWiredFile file);
		

                void sendChatTopic(const QwsRoom chat);
		void sendClientKicked(const int killerId, const int victimId, const QString reason, const bool banned);
		void sendClientDeclinedChat(const int chatId, const int userId);

		void sendClientLeave(const int chatId, const int id);
		void sendInviteToChat(const int chatId, const int id);

		void sendPrivateMessage(const int userId, const QString text);
		void sendPrivateChatCreated(const int chatId);
		void sendPrivileges();

		
		void sendUserInfo(const ClassWiredUser user);
		
		void idleTimerTriggered();



		
	private slots:

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

                // Communication
                void handleMessageSAY(QwMessage &message);
                void handleMessageME(QwMessage &message);
                void handleMessageMSG(QwMessage &message);
                void handleMessageBROADCAST(QwMessage &message);
                void handleMessageTOPIC(QwMessage &message);
                void handleMessagePRIVCHAT(QwMessage &message);
                void handleMessageINVITE(QwMessage &message);
                void handleMessageJOIN(QwMessage &message);

                // News
                void handleMessageNEWS(QwMessage &message);
                void handleMessagePOST(QwMessage &message);
                void handleMessageCLEARNEWS(QwMessage &message);

		void on_socket_sslErrors(const QList<QSslError> & errors);
		void handleWiredMessage(QByteArray theData);
		void on_socket_error();


		
private:
    /*! Defines the current state of the session/socket. */
    Qws::SessionState sessionState;


    void resetIdleTimer();

    /// Our function that formats a Wired message and sends it to the server.
    void sendWiredCommand(const QByteArray);
    void sendWiredCommandBuffer(const QByteArray);

    /// This is our TCP buffer. Could possibly be optimized, but works for now.
    QByteArray pBuffer;

    /// This is the send TCP buffer. It is used when sending lists and to prevent the event loop
    /// from fireing command handling during list sending.
    QByteArray pSendBuffer;

    QPointer<QSslSocket> pSocket;
		
		
    
};

#endif
