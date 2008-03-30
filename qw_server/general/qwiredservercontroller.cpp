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
#include "../general/qwiredservercontroller.h"

QwiredServerController::QwiredServerController(QObject *parent)
 : QObject(parent)
{ }

QwiredServerController::~QwiredServerController()
{ }

void QwiredServerController::reloadConfig() {
	qwLog("Reloading configuration...");
	pCfServerPort = 2000;
}

void QwiredServerController::startServer() {
	qwLog("Starting server...");
	pTcpServer = new QTcpServer(this);
	connect(pTcpServer, SIGNAL(newConnection()), this, SLOT(acceptClient()));
	if(!pTcpServer->listen(QHostAddress::Any, pCfServerPort)) {
		qwLog(QString("Fatal: Unable to listen on TCP port %1. Terminating.").arg(pCfServerPort));
		exit(1);
	}
}

/**
 * Write a message to the log (and console)
 */
void QwiredServerController::qwLog(QString theMessage) {
	using namespace std;
	cout << QString("[%1] %2").arg(QDateTime::currentDateTime().toString()).arg(theMessage).toStdString() << endl;
}

/**
 * Accepts a new connection. Connected to the newConnect() signal of pTcpServer.
 */
void QwiredServerController::acceptClient() {
	qDebug() << "Accepting connection...";
}


