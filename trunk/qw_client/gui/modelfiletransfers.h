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

 
#ifndef MODELFILETRANSFERS_H
#define MODELFILETRANSFERS_H

#include <QAbstractListModel>
#include "wired/wiredsocket.h"
#include "wired/wiredtransfersocket.h"

/**
	@author Bastian Bense <bastibense@gmail.com>
 */

class ModelFileTransfers : public QAbstractListModel
{
	
Q_OBJECT
		
public:
    ModelFileTransfers(QObject *parent = 0);
    ~ModelFileTransfers();
	
	QVariant data(const QModelIndex &index, int role=Qt::DisplayRole) const;
	int rowCount(const QModelIndex &parent=QModelIndex()) const;

	void setSocket(WiredSocket *theSocket);
	
	QPointer<WiredSocket> pSocket;
		
private slots:
	void updateTransfers(const ClassWiredTransfer theTransfer);
	void reloadTransfers();
};

#endif
