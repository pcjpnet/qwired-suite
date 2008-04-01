/***************************************************************************
 *   Copyright (C) 2007 by Bastian Bense   *
 *   bb@bense.de   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include "ssltcpserver.h"

SslTcpServer::SslTcpServer(QObject *parent)
 : QTcpServer(parent)
{
	QFile tmpCertFile("/home/bbense/Desktop/testct.pem");
	if(!tmpCertFile.open(QIODevice::ReadOnly))
		qDebug() << "Failed to open file.";
	
	QByteArray tmpCertData = tmpCertFile.readAll();
	
	pPrivateKey = QSslKey(tmpCertData, QSsl::Rsa);
	if(!pPrivateKey.isNull())
		qDebug() << "[ssltcpserver] Loaded private key:"<<pPrivateKey.length();
	pLocalCert = QSslCertificate(tmpCertData);
		if(!pLocalCert.isNull())
			qDebug() << "[ssltcpserver] Loaded cert:"<<pLocalCert.subjectInfo(QSslCertificate::CommonName);
}

SslTcpServer::~SslTcpServer()
{ }

void SslTcpServer::incomingConnection(int socketDescriptor) {
	qDebug() << "[ssltcpserver] New connection:"<<socketDescriptor;
	QSslSocket *conn = new QSslSocket(this);
	if(conn->setSocketDescriptor(socketDescriptor)) {
		qDebug() << "[ssltcpserver] Accepted connection, setting parameters and initing handshake.";
		conn->setProtocol(QSsl::TlsV1);
		conn->setPrivateKey(pPrivateKey);
		conn->setLocalCertificate(pLocalCert);
		conn->startServerEncryption();
		pPendingSockets.append(conn);
		emit newSslConnection();
	} else {
		qDebug() << "[ssltcpserver] Failed to set socket descriptor. Dropping.";
		delete conn;
	}
}

/**
 * Returns true if there are any waiting SSL sockets in the queue.
 */
bool SslTcpServer::hasPendingSslSocket() {
	return pPendingSockets.size()>0;
}

/**
 * Returns the oldest pending SSL socket and removes it from the queue.
 */
QSslSocket* SslTcpServer::nextPendingSslSocket() {
	if(!pPendingSockets.size())
		return 0;
	return pPendingSockets.takeFirst();
}


