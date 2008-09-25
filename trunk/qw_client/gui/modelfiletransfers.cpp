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

 
#include "modelfiletransfers.h"


ModelFileTransfers::ModelFileTransfers(QObject *parent)
	: QAbstractListModel(parent)
{
// 	startTimer(1000);
}


ModelFileTransfers::~ModelFileTransfers()
{ }


int ModelFileTransfers::rowCount(const QModelIndex &) const
{
	if(!pSocket) return 0;
	return pSocket->pTransferSockets.count();
}


QVariant ModelFileTransfers::data(const QModelIndex & index, int role) const
{
	if(!index.isValid()) return QVariant();
	if(!pSocket) return QVariant();
	
	if(index.row()<pSocket->pTransferSockets.count()) {
		WiredTransferSocket *tmpSock = pSocket->pTransferSockets.value(index.row());
		if(!tmpSock) return QVariant();
		if (role==Qt::UserRole)
			return QVariant::fromValue(tmpSock->pTransfer);
	}
	return QVariant();
}


void ModelFileTransfers::setSocket(WiredSocket *theSocket)
{
	if(!theSocket) return;
	pSocket = theSocket;
	connect(pSocket, SIGNAL(fileTransferStatus(ClassWiredTransfer)),  this, SLOT(updateTransfers(ClassWiredTransfer)) );
	connect(pSocket, SIGNAL(fileTransferStarted(ClassWiredTransfer)), this, SLOT(reloadTransfers()) );
	connect(pSocket, SIGNAL(fileTransferDone(ClassWiredTransfer)), this, SLOT(reloadTransfers()) );
	connect(pSocket, SIGNAL(fileTransferError(ClassWiredTransfer)), this, SLOT(reloadTransfers()) );
}


// The status of an item has updated and should be redrawn
void ModelFileTransfers::updateTransfers(const ClassWiredTransfer theTransfer)
{
	QListIterator<QPointer<WiredTransferSocket> > i(pSocket->pTransferSockets);
	int tmpIdx=0;
	while(i.hasNext()) {
		QPointer<WiredTransferSocket> tmpP = i.next();
		if(tmpP && tmpP->pTransfer.pHash==theTransfer.pHash) {
			QModelIndex tmpIndex = index(tmpIdx,0);
			emit dataChanged(tmpIndex, tmpIndex);
		}
		tmpIdx++;
	}
}


void ModelFileTransfers::reloadTransfers()
{
	reset();
}

// void ModelFileTransfers::timerEvent(QTimerEvent *event)
// {
// 	reset();
// }


