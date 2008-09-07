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


#include <QtCore>
#include "qwservercontroller.h"
#include "qwtransaction.h"


const QString QWSERVER_VERSION("1.0.0");

int main (int argc, char *argv[]) {
	QCoreApplication app(argc, argv);
	QStringList tmpCmdArgs = QCoreApplication::arguments();

	QWServerController *controller = new QWServerController();
	controller->reloadConfig();
	controller->reloadDatabase();

	if(int index=tmpCmdArgs.indexOf("-gui") > -1) {
		// Started in GUI mode. Wait for the GUI client to connnect and
		// provide commands.
		qDebug() << "QWServer: Starting in GUI interface mode.";
		controller->startGuiInterfaceServer();
	} else {
		// Normal mode - start server directly
		qDebug() << "QWServer: Starting normally.";
		controller->startServer();
	}
	
	
	
	return app.exec();
}



