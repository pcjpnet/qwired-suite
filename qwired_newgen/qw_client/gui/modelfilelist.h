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

 
#ifndef MODELFILELIST_H
#define MODELFILELIST_H

#include "wired/classwiredfile.h"
#include <QStandardItemModel>

/**
	@author Bastian Bense <bastibense@gmail.com>
 */
class ModelFileList : public QStandardItemModel {
Q_OBJECT
public:
	ModelFileList(QObject *parent = 0);
	~ModelFileList();

//     QList<ClassWiredFile> pFiles;
//     
//     int rowCount(const QModelIndex &parent=QModelIndex()) const;
//     int columnCount ( const QModelIndex & parent = QModelIndex() ) const;
//     QVariant data(const QModelIndex &index, int role=Qt::DisplayRole) const;
//     QVariant headerData(int section, Qt::Orientation orientation, int role=Qt::DisplayRole) const;
//

	
	bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent);
	Qt::DropActions supportedDropActions() const;
	
	qulonglong pTotalSize;
	qulonglong pFreeSize;
	QString pCurrentPath;
	bool pWaitingForList;
    
    
public slots:
     void onServerFileListItem(ClassWiredFile file);
     void onServerFileListDone(QString thePath, qlonglong theFreeSpace);
     void clearList();

};

#endif
