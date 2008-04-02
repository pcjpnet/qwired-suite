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
#include "qwservercontroller.h"
#include "wiredsocket.h"

#include <QtSql>

QWServerController::QWServerController(QObject *parent)
 : QObject(parent)
{
	pCore = new QWServerCore(this);
}

QWServerController::~QWServerController()
{
	
}

void QWServerController::reloadConfig() {
	qwLog("Reloading configuration...");
	pCfServerPort = 2000;
}

void QWServerController::startServer() {
	qwLog("Starting server...");
	pTcpServer = new SslTcpServer(this);
	connect(pTcpServer, SIGNAL(newSslConnection()), this, SLOT(acceptSslConnection()));
	if(!pTcpServer->listen(QHostAddress::Any, pCfServerPort)) {
		qwLog(QString("Fatal: Unable to listen on TCP port %1. Terminating.").arg(pCfServerPort));
		exit(1);
	}
}

/**
 * Write a message to the log (and console)
 */
void QWServerController::qwLog(QString theMessage) {
	using namespace std;
	cout << QString("[%1] %2").arg(QDateTime::currentDateTime().toString()).arg(theMessage).toStdString() << endl;
}

/**
 * Accepts a new connection. Connected to the newConnect() signal of pTcpServer.
 */
void QWServerController::acceptSslConnection() {
	WiredSocket *wsock = new WiredSocket(this);
	pCore->registerClient(wsock);
	wsock->setWiredSocket(pTcpServer->nextPendingSslSocket());
}

/**
 * Load the database from the disk.
 */
void QWServerController::reloadDatabase() {
	qwLog("Loading database...");
	QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
	db.setDatabaseName("accounts.db");
	if(!db.open()) {
		qwLog(QString("Fatal: Could not open accounts.db. Reason: %1").arg(QSqlError(db.lastError()).text()));
		exit(1);
	} else {
		pCore->pDatabase = db;

		QSqlQuery query;
		QStringList tmpTables = db.tables();
		if(!tmpTables.contains("qw_news")) {
			qDebug() << "[controller] Creating missing qw_new table.";
			query.exec("CREATE TABLE qw_news (id INTEGER PRIMARY KEY, message TEXT, date TEXT, user TEXT, deleted INTEGER DEFAULT 0);");
		}

		if(!tmpTables.contains("qw_accounts")) {
			qDebug() << "[controller] Creating missing qw_accounts table.";
			query.exec("CREATE TABLE qw_accounts (id INTEGER PRIMARY KEY, login TEXT, password TEXT, groupname TEXT, privileges TEXT);");
			query.exec("INSERT INTO qw_accounts (login,password,groupname,privileges) VALUES ('admin', '', 'admins', '1:1:1:1:1:1:1:1:1:1:1:1:1:1:1:1:1:0:0:0:0:0:1');");
			query.exec("INSERT INTO qw_accounts (login,password,groupname,privileges) VALUES ('guest', '', '', '0:0:0:0:1:1:0:1:0:0:0:0:0:0:0:0:0:0:0:0:0:0:0');");
		}

		if(!tmpTables.contains("qw_groups")) {
			qDebug() << "[controller] Creating missing qw_groups table.";
			query.exec("CREATE TABLE qw_groups (id INTEGER PRIMARY KEY, groupname TEXT, privileges TEXT);");
			query.exec("INSERT INTO qw_groups (groupname,privileges) VALUES ('admins', '1:1:1:1:1:1:1:1:1:1:1:1:1:1:1:1:1:0:0:0:0:0:1');");
		}

		qDebug() << "Tables:"<<db.tables();
	}
}

