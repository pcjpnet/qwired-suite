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

/* This is the source code of the Qwired server. It is basically a Wired 1.1 server that also
   implements some nice extensions.
*/

#include <QtCore>
#include "QwsServerController.h"
#include "QwTrackerClientSocket.h"

const QString QWSERVER_VERSION("0.1.0");

int main (int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    QwTrackerClientSocket *trackerSocket = new QwTrackerClientSocket(0);
    trackerSocket->connectToTracker("wired.zankasoftware.com");

//    QwsServerController controller;
//    if (!controller.loadConfiguration()) {
//        return 1;
//    }
//    if (!controller.startServer()) {
//        return 1;
//    }




    return app.exec();
}



