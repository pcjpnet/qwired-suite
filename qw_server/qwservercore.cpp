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
#include "qwservercore.h"


QWServerCore::QWServerCore(QObject *parent)
 : QObject(parent)
{
}


QWServerCore::~QWServerCore()
{
}

/**
 * Return a unique user identification number used for user list management.
 */
int QWServerCore::getUniqueUserId() {
	static int tmpUserId=0;
	return ++tmpUserId;
}

void QWServerCore::registerClient(WiredSocket *socket) {
	if(!socket) return;
	socket->setUserId(getUniqueUserId());
	qDebug() << "[core] registered client with id"<<socket->userId();
}

void QWServerCore::sendUserlist(const int id) {
	if(!pClients.contains(id)) return;
	WiredSocket *tmpSock = pClients[id];
	QHashIterator<int,WiredSocket*> i(pClients);
	while(i.hasNext()) {
		i.next();
		WiredSocket *item = i.value();
		tmpSock->sendUserlistItem(id, item->sessionUser());
	}
	tmpSock->sendUserlistDone(id);
}


