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

	private slots:
		void on_socket_readyRead();
		void on_socket_sslErrors(const QList<QSslError> & errors);
		void do_handle_wiredmessage(QByteArray theData);
		void on_socket_error();
		
	private:
		/// Our function that formats a Wired message and sends it to the server.
		void sendWiredCommand(const QByteArray);

		/// This is our TCP buffer. Could possibly be optimized, but works for now.
		QByteArray pBuffer;
		QPointer<QSslSocket> pSocket;
		
		
    
};

#endif
