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

 
#include "modelfilelist.h"
#include <QtGui>
#include <math.h>
#include "general/wiredcommon.h"

ModelFileList::ModelFileList(QObject *parent)
	: QStandardItemModel(parent)
{
	pTotalSize=0;
	pWaitingForList=false;

	QStringList tmpHeaders;
	tmpHeaders << tr("Name") << tr("Size") << tr("Modified");
	this->setHorizontalHeaderLabels(tmpHeaders);

}

ModelFileList::~ModelFileList()
{ }

//
// External signals
//

void ModelFileList::onServerFileListDone(QString thePath, qlonglong theFreeSpace) {
	if( thePath==pCurrentPath && pWaitingForList ) {
		pWaitingForList = false;
		pFreeSize = theFreeSpace;		
	}
}

void ModelFileList::onServerFileListItem(ClassWiredFile file) {
	if( file.path.startsWith(pCurrentPath) && pWaitingForList ) {
		// Add an item
		QList<QStandardItem*> tmpList;
		QStandardItem *item;

		QVariant tmpData;
		tmpData.setValue(file);
		
		item = new QStandardItem(); // name
		item->setText(file.fileName());
		item->setIcon(file.fileIcon());
		item->setData(file.fileName(), Qt::UserRole);
		item->setData(tmpData, Qt::UserRole+1);
		tmpList.append(item);

		item = new QStandardItem(); // size
		item->setText( file.type==0 ? ClassWiredFile::humanReadableSize(file.size) : QString::number(file.size) );
		item->setData(file.size, Qt::UserRole);
		item->setData(tmpData, Qt::UserRole+1);
		tmpList.append(item);

		item = new QStandardItem(); // changed
		item->setText(file.modified.toString(Qt::SystemLocaleDate));
		item->setData(file.modified.toTime_t(), Qt::UserRole);
		item->setData(tmpData, Qt::UserRole+1);
		tmpList.append(item);

		this->appendRow(tmpList);
		pTotalSize += file.size;
	}
}

void ModelFileList::clearList() {
	this->setRowCount(0);
	pTotalSize = 0;
}

bool ModelFileList::dropMimeData(const QMimeData *, Qt::DropAction, int, int, const QModelIndex &) {
 	return true;
}

Qt::DropActions ModelFileList::supportedDropActions() const {
 	return Qt::CopyAction;
}



