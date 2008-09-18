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


#include "../general/wiredcommon.h"
#include "modeluserlist.h"
#include "wired/classwireduser.h"
#include <QtGui>
#include <QtCore>

ModelUserList::ModelUserList(QObject *parent)
 : QAbstractListModel(parent)
{
	chatID = 1;
	
	// Notification manager
	WiredSingleton *tmpS = &WSINGLETON::Instance();
	connect( tmpS, SIGNAL(prefsChanged()), this, SLOT(reloadPreferences()) );
}


ModelUserList::~ModelUserList()
{
}

Qt::ItemFlags ModelUserList::flags (const QModelIndex & index) const 
{

	if (index.isValid())
		return Qt::ItemIsSelectable | Qt::ItemIsDragEnabled;
	else
		return Qt::ItemIsSelectable | Qt::ItemIsDropEnabled;
}

// Qt::DropActions ModelUserList::supportedDropActions() const
// {
// 	return Qt::CopyAction;
// }

QStringList ModelUserList::mimeTypes() const
{
	qDebug() << "mime types";
	QStringList types;
	types << "application/x-userid";
	return types;
}

QMimeData *ModelUserList::mimeData(const QModelIndexList &indexes) const
{
	QMimeData *mimeData = new QMimeData();
	QModelIndex tmpIdx = indexes.first();
	QByteArray data = tmpIdx.data(Qt::UserRole).toByteArray();
	mimeData->setData("application/x-userid", data);
	return mimeData;
}

bool ModelUserList::dropMimeData(const QMimeData *data,
	Qt::DropAction action, int row, int column, const QModelIndex &parent)
{
	Q_UNUSED(row)
	Q_UNUSED(column)
	Q_UNUSED(parent)

			qDebug() << "drop mime data";
	if (action == Qt::IgnoreAction)
		return true;

	if (data->hasFormat("application/x-userid")) {
		QByteArray dat = data->data("application/x-userid");
		int tmpID = dat.toInt();
		wiredSocket->inviteClientToChat(chatID, tmpID);
		return true;
	}
	return false;
}

QVariant ModelUserList::data(const QModelIndex & index, int role) const
{
	if( !index.isValid() )
		return QVariant();
	if( index.row()>=wiredSocket->userCountByChat(chatID) )
		return QVariant();
	
	
	if(role == Qt::DisplayRole) {
		ClassWiredUser wu = wiredSocket->userByIndex(chatID, index.row());
		return wu.pNick;
	} else if( role == Qt::DecorationRole ) { // Nickname
		ClassWiredUser wu = wiredSocket->userByIndex(chatID, index.row());
		QImage img;
		if(!wu.pImage.isEmpty()) {
			img = QImage::fromData(wu.pImage);
			img = img.scaled(32,32,Qt::KeepAspectRatio);
		}
		return img;
	} else if( role == Qt::UserRole+4 ) { // Status text
		ClassWiredUser wu = wiredSocket->userByIndex(chatID, index.row());
		return wu.pStatus;
	} else if( role == Qt::UserRole+1 ) { // Admin Flag
		ClassWiredUser wu = wiredSocket->userByIndex(chatID, index.row());
		return wu.pAdmin;
	} else if( role == Qt::UserRole+2 ) { // Idle Flag
		ClassWiredUser wu = wiredSocket->userByIndex(chatID, index.row());
		return wu.pIdle;
	} else if( role == Qt::UserRole) { // User ID
		ClassWiredUser wu = wiredSocket->userByIndex(chatID, index.row());
		return wu.pUserID;
	} else {
		return QVariant();
	}
}

int ModelUserList::rowCount(const QModelIndex &) const {
	if( wiredSocket )
		return wiredSocket->userCountByChat(chatID);
	return 0;
}

void ModelUserList::setWiredSocket(WiredSocket * theSocket)
{
	wiredSocket = theSocket;
	if( wiredSocket ) {
		connect( wiredSocket, SIGNAL(onServerUserlistDone(int)), this, SLOT(onDataUpdate(int)) );
		connect( wiredSocket, SIGNAL(onServerUserChanged(const ClassWiredUser, const ClassWiredUser)),
			 this,          SLOT(onServerUserChanged(const ClassWiredUser, const ClassWiredUser)) );
		connect( wiredSocket, SIGNAL(onServerUserJoined(int, const ClassWiredUser)),
			 this,		SLOT(onServerUserJoined(int, const ClassWiredUser)) );
		connect( wiredSocket, SIGNAL(onServerUserLeft(int, const ClassWiredUser)),
			 this,		SLOT(onServerUserLeft(int, const ClassWiredUser)) );	
	}
}

// Data base been updated.
void ModelUserList::onDataUpdate(int theChatID) {
	if( wiredSocket and theChatID==chatID )
		reset();
}

void ModelUserList::onServerUserChanged(const ClassWiredUser, const ClassWiredUser theNew) {
	if(wiredSocket) {
		int tmpIdx = wiredSocket->userIndexByID(theNew.pUserID);
		emit dataChanged( createIndex(tmpIdx,0), createIndex(tmpIdx,0) ); 
	}
}

void ModelUserList::onServerUserJoined(int theChatID, const ClassWiredUser theUser)
{
	Q_UNUSED(theUser)
	// A user joined the server.
	if(wiredSocket && theChatID==chatID) {
		//int tmpIdx = wiredSocket->userIndexByID(theUser.pUserID);
		//insertRows(rowCount(), 1); 
		reset();
		
	}
}

// A user has left the chat
void ModelUserList::onServerUserLeft(int theChatID, const ClassWiredUser) {
	if(wiredSocket && (chatID==theChatID || chatID==1))
		reset();
}

bool ModelUserList::removeRows(int row, int count, const QModelIndex &) {
	beginRemoveRows(QModelIndex(), row, row+count-1);
	endRemoveRows();
	return true;
}

bool ModelUserList::insertRows(int row, int count, const QModelIndex &) {
	beginInsertRows(QModelIndex(), row, row+count-1);
	endInsertRows();
	return true;
}

void ModelUserList::reloadPreferences() {
	reset();
}




