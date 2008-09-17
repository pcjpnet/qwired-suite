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

 
#ifndef MODELUSERLIST_H
#define MODELUSERLIST_H

#include <QAbstractListModel>
#include "wired/wiredsocket.h"

/**
This model manages the userlists found in the forum and private chats.

	@author Bastian Bense <bastibense@gmail.com>
*/
class ModelUserList : public QAbstractListModel
{

	Q_OBJECT

public:
	ModelUserList(QObject *parent = 0);
	~ModelUserList();
	int rowCount(const QModelIndex &parent=QModelIndex()) const;
	QVariant data(const QModelIndex &index, int role) const;
	bool insertRows ( int row, int count, const QModelIndex & parent = QModelIndex() );
	bool removeRows ( int row, int count, const QModelIndex & parent = QModelIndex() );
	
	void setChatID (int theValue) {chatID = theValue;}
	void setWiredSocket ( WiredSocket *theSocket );
	int chatID;
	Qt::ItemFlags flags (const QModelIndex & index) const;
 	QStringList mimeTypes() const;
	QMimeData* mimeData(const QModelIndexList &indexes) const;
	bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent);

private:
	QPointer<WiredSocket> wiredSocket;
	
private slots:
	void onDataUpdate(int theChatID);
	void onServerUserChanged(const ClassWiredUser theOld, const ClassWiredUser theNew);
	void onServerUserJoined(int theChatID, const ClassWiredUser theUser);
	void onServerUserLeft(int theChatID, const ClassWiredUser theUser);
	void reloadPreferences();

};

#endif
