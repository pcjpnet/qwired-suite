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

/**
 * MiuSocket provides a low level implementation of the transaction-based
 * Miu protocol stack. If you want to implement a client or server, refer
 * to MiuClient and MiuServer instead.
 */
 
#ifndef MIUSOCKET_H
#define MIUSOCKET_H

#include <QSslSocket>
#include <QByteArray>

#include "MiuSocket.h"
#include "MiuTransaction.h"

const int kEOF = 0x04;
const int kFS = 0x1C;

class MiuSocket : public QObject {

Q_OBJECT

public:
	MiuSocket(QObject *parent = 0);
	~MiuSocket();
	void setMiuSocket(QSslSocket *socket);
	bool bufferHasMessage(QByteArray &buffer);
	void sendTransaction(const MiuTransaction &t);	


signals:
	void transactionReceived(const MiuTransaction &t);
	void clientDisconnected();

		
public slots:
	void disconnectClient();


private slots:
	void handleSocketError();
	void readDataFromSocket();
	void handleSocketSslError(const QList<QSslError> & errors);


private:
	QByteArray pBuffer;
	QPointer<QSslSocket> pSocket;
		
    
};

#endif
