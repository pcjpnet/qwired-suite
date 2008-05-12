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

/**
 * Read the configuration file from the disk and store it in our configuration hash for quick
 * parameter access during runtime.
 */
void QWServerController::reloadConfig() {
	QStringList tmpCmdArgs = QCoreApplication::arguments();

	// Override config file
	QString tmpCfgPath = "etc/wired.conf";
	if(int index=tmpCmdArgs.indexOf("-c") > -1)
		tmpCfgPath = tmpCmdArgs.value(index+1);
	
	qwLog(QString("Loading configuration from '%1'").arg(tmpCfgPath));
	
	QFile tmpConfigFile(tmpCfgPath);
	pConfigParams.clear();
	if(!tmpConfigFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
		qwLog(QString("Fatal: Unable to open configuration file: %1. Terminating.").arg(tmpConfigFile.errorString()));
		exit(1);
	} else {
		while(!tmpConfigFile.atEnd()) {
			QString tmpLine = tmpConfigFile.readLine();
			if(!tmpLine.startsWith("#") && !tmpLine.trimmed().isEmpty()) 
				pConfigParams[tmpLine.section("=",0,0).trimmed()] = tmpLine.section("=",1,-1).trimmed();
		}
	}


	// Banner
	if(pConfigParams.contains("banner")) {
		QFile banner(pConfigParams.value("banner"));
		if(!banner.open(QIODevice::ReadOnly)) {
			qwLog(QString("Warning: Unable to read server banner: %1").arg(banner.errorString()));
		} else {
			qwLog(QString("Loaded server banner from '%1'").arg(banner.fileName()));
			pCore->pBannerData = banner.readAll();
		}
	}
	
	// Banner
	if(pConfigParams.contains("motd")) {
		QFile banner(pConfigParams.value("motd"));
		if(!banner.open(QIODevice::ReadOnly)) {
			qwLog(QString("Warning: Unable to read MOTD file: %1").arg(banner.errorString()));
		} else {
			qwLog(QString("Loaded MOTD from '%1'").arg(banner.fileName()));
			pCore->pMotdData = banner.readAll();
		}
	}

	// Files directory
	QDir tmpFilesRoot(pConfigParams.value("files","files"));
	pCore->pFilesRoot = tmpFilesRoot.canonicalPath();
	qwLog(QString("Setting files root directory to '%1'.").arg(pCore->pFilesRoot));
	if(!tmpFilesRoot.exists()) {
		qwLog(QString("Fatal: Unable to access files directory '%1'. Terminating.").arg(pCore->pFilesRoot));
		exit(1);
	}
	
	
	
}

/**
 * Start the TCP listener in order to accept connections.
 */
void QWServerController::startServer() {
	int tmpPort = pConfigParams.value("port","2000").toInt();
	QString tmpAddress = pConfigParams.value("address","0.0.0.0");
	qwLog(QString("Starting server on  %2:%1...").arg(tmpPort).arg(tmpAddress));
	pTcpServer = new SslTcpServer(this);
	pTcpServer->setCertificateFromFile(pConfigParams.value("certificate","certificate"));
	connect(pTcpServer, SIGNAL(newSslConnection()), this, SLOT(acceptSslConnection()));
	if(!pTcpServer->listen( QHostAddress(tmpAddress), tmpPort)) {
		qwLog(QString("Fatal: Unable to listen on TCP port %1. %2. Terminating.").arg(tmpPort).arg(pTcpServer->errorString()));
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
	QStringList tmpCmdArgs = QCoreApplication::arguments();
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

		if(!tmpTables.contains("qw_conferences")) {
			qDebug() << "[controller] Creating missing qw_conferences table.";
			query.exec("CREATE TABLE qw_conferences (id INTEGER PRIMARY KEY, topic TEXT, password TEXT);");
			query.exec("INSERT INTO qw_accounts (password,topic) VALUES ('Administrators Chat', 'admin');");
		}

		// Import accounts from older configuration file
		if(int index=tmpCmdArgs.indexOf("-iu") > -1) {
			QFile tmpFile(tmpCmdArgs.value(index+1));
			if(tmpFile.open(QIODevice::ReadOnly)) {
				qwLog("Importing users from old-world file...");
				query.exec("DELETE FROM qw_accounts;");
				while(!tmpFile.atEnd()) {
					QString tmpLine = tmpFile.readLine();
					if(tmpLine.startsWith("#") || tmpLine.trimmed().isEmpty()) continue;
					QStringList tmpRec = tmpLine.split(":");
					query.prepare("INSERT INTO qw_accounts (login,password,groupname,privileges) VALUES (:login, :password, :groupname, :privileges);");
					query.bindValue(":login", tmpLine.section(":",0,0));
					query.bindValue(":password", tmpLine.section(":",1,1));
					query.bindValue(":groupname", tmpLine.section(":",2,2));
					query.bindValue(":privileges", tmpLine.section(":",3,-1).trimmed());
					if(!query.exec()) qwLog(QString("SQL error: %1").arg(query.lastError().text()));
					qwLog(QString("  Imported user %1").arg(tmpLine.section(":",0,0).trimmed()));
				}
			} else {
				qwLog(QString("Error: Could not open old-world users file: %1").arg(tmpFile.errorString()));
			}
			exit(0);
		}
		

		if(!tmpTables.contains("qw_groups")) {
			qDebug() << "[controller] Creating missing qw_groups table.";
			query.exec("CREATE TABLE qw_groups (id INTEGER PRIMARY KEY, groupname TEXT, privileges TEXT);");
			query.exec("INSERT INTO qw_groups (groupname,privileges) VALUES ('admins', '1:1:1:1:1:1:1:1:1:1:1:1:1:1:1:1:1:0:0:0:0:0:1');");
		}

		// Import accounts from older configuration file
		if(int index=tmpCmdArgs.indexOf("-ig") > -1) {
			QFile tmpFile(tmpCmdArgs.value(index+1));
			if(tmpFile.open(QIODevice::ReadOnly)) {
				qwLog("Importing groups from old-world file...");
				query.exec("DELETE FROM qw_groups;");
				while(!tmpFile.atEnd()) {
					QString tmpLine = tmpFile.readLine();
					if(tmpLine.startsWith("#") || tmpLine.trimmed().isEmpty()) continue;
					QStringList tmpRec = tmpLine.split(":");
					query.prepare("INSERT INTO qw_groups (groupname,privileges) VALUES (:groupname, :privileges);");
					query.bindValue(":groupname", tmpLine.section(":",0,0));
					query.bindValue(":privileges", tmpLine.section(":",1,-1).trimmed());
					if(!query.exec()) qwLog(QString("SQL error: %1").arg(query.lastError().text()));
					qwLog(QString("  Imported group %1").arg(tmpLine.section(":",0,0)));
				}
			} else {
				qwLog(QString("Error: Could not open old-world group file: %1").arg(tmpFile.errorString()));
			}
			exit(0);
		}
		
		qDebug() << "Tables:"<<db.tables();
	}
}

