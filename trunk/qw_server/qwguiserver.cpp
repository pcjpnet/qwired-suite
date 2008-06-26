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
#include "qwguiserver.h"

QWGuiServer::QWGuiServer(QObject *parent)
 : QObject(parent)
{
	tcpSocket = NULL;
	tcpServer = new QTcpServer(this);
	connect(tcpServer, SIGNAL(newConnection()), this, SLOT(acceptConnection()));
	
}


QWGuiServer::~QWGuiServer()
{
}

void QWGuiServer::startServer()
{
	tcpServer->listen(QHostAddress::LocalHost, 2016);
	
}

void QWGuiServer::acceptConnection()
{
	if(!tcpSocket) {
		tcpSocket = tcpServer->nextPendingConnection();
		connect(tcpSocket, SIGNAL(disconnected()), this, SLOT(clearConnection()));
		connect(tcpSocket, SIGNAL(readyRead()), this, SLOT(readData()));
		tcpSocket->write(QByteArray("QWS_PROTOCOL 1\n"));
	} else {
		// Already connected, drop this connection.
		QTcpSocket *socket = tcpServer->nextPendingConnection();
		socket->disconnectFromHost();
		socket->deleteLater();
	}
}

void QWGuiServer::clearConnection()
{
	qDebug() << "QWGuiServer: GUI disconnected, cleaning up.";
	if(!tcpSocket) return;
	tcpSocket->disconnectFromHost();
	tcpSocket->deleteLater();
	
}

void QWGuiServer::readData()
{
	tcpBuffer += tcpSocket->readAll();
	while(tcpBuffer.contains("\n")) {
		int tmpPos = tcpBuffer.indexOf("\n");
		QByteArray command( tcpBuffer.left(tmpPos) );
		tcpBuffer.remove(0, tmpPos+1);
		tcpSocket->write(command + QByteArray("\n"));
	}
}


