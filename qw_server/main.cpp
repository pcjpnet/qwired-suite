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
	

	QWTransaction t;
	t.type = 1001;
	t.error = 0x00;
	t.ttl = 0x00;
	t.flags = 0x0000;
	t.addObject("uid", "wow this rocks, really totally awesome");
	//t.addObject("text", "basdfasdfasfdsdf sda sad");
	//t.addObject("value", "basdfasdf");
	//t.addObject("bleh", "bfdasdfasdfasd :D");
	qDebug() << t.toData().toHex();

	QWTransaction c;
	c.parseFromData(t.toData());
	qDebug() << c.toData().toHex();

	QWServerController *controller = new QWServerController();
	controller->reloadConfig();
	controller->reloadDatabase();
	controller->startServer();
	
	return app.exec();
}



