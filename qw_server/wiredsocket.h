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
#include <QtCore>
#include <QtNetwork>


/**
	@author Bastian Bense <bastibense@gmail.com>
 */

namespace Wired {

}

const int kEOF = 0x04;
const int kFS = 0x1C;

class WiredSocket : public QObject
{
	Q_OBJECT

	public:
		WiredSocket(QObject *parent = 0);
		~WiredSocket();
		void setWiredSocket(QSslSocket *socket);

		ClassWiredUser sessionUser() { return pSessionUser; };
		int userId() { return pSessionUser.pUserID; }
		bool isLoggedIn() { return pSessionUser.pUserID>0; };
		
		
	signals:
		/**
		 * The client has sent the login and password and the server should now check if these are correct.
		 * Call acceptLogin() or rejectLogin() to accept or reject the connection.
		 * @param id The ID of the connection.
		 * @param login The login name.
		 * @param password The password.
		 */
		void clientDisconnected(const int id);
		void loginReceived(const int id, QString login, QString password);
		void receivedChat(const int id, const int chatId, const QString text);
		void requestedUserlist(const int id, int chat);
		void userStatusChanged(const ClassWiredUser user);
		
	public slots:
		void disconnectClient();
		void acceptLogin();
		void rejectLogin();
		
		void setServerInformation(QString serverName, QString serverDesc);
		void setUserId(int userId);

		void sendClientJoin(const int chatId, const ClassWiredUser user);
		void sendClientLeave(const int chatId, const int id);
		void sendUserlistItem(const int chatId, const ClassWiredUser item);
		void sendUserlistDone(const int chatId);
		void sendUserStatusChanged(const ClassWiredUser user);
		void sendChat(const int chatId, const int userId, const QString text);



		
	private slots:
		void readDataFromSocket();
		void on_socket_sslErrors(const QList<QSslError> & errors);
		void handleWiredMessage(QByteArray theData);
		void on_socket_error();


		// Protocol Stack
		void qwSendServerInformation();
		void qwSendLoginSuccessful(int userId);
		void qwSendCommandNotImplemented();
		
	private:
		// Session state parameters
		//


		QString pServerName;
		QString pServerDesc;
		
		ClassWiredUser pSessionUser;
		bool pHandshakeOK;
		
		/// Our function that formats a Wired message and sends it to the server.
		void sendWiredCommand(const QByteArray);

		/// This is our TCP buffer. Could possibly be optimized, but works for now.
		QByteArray pBuffer;
		QPointer<QSslSocket> pSocket;
		QList<QByteArray> splitMessageFields(QByteArray theData);
		
		
    
};

#endif
