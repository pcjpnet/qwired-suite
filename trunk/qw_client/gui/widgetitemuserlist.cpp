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

 
#include "widgetitemuserlist.h"
#include <QMouseEvent>
#include <QtDebug>
#include <QApplication>

WidgetItemUserlist::WidgetItemUserlist(QWidget *parent) : QListView(parent)
{
}


WidgetItemUserlist::~WidgetItemUserlist()
{
}

void WidgetItemUserlist::mouseMoveEvent(QMouseEvent * event)
{
	Q_UNUSED(event)
			
	// Handle Mouse Drag+Drop
	QDrag *drag = new QDrag(this);
	QMimeData *mimeData = new QMimeData;
	QImage tmpVar =  this->model()->data(this->currentIndex(), Qt::DecorationRole).value<QImage>();
	if(!tmpVar.isNull())
		drag->setPixmap( QPixmap::fromImage(tmpVar) );
	mimeData->setData("application/x-userid", QByteArray("Hello world"));
	drag->setMimeData(mimeData);
	Qt::DropAction dropAction = drag->start();
	Q_UNUSED(dropAction)
}

void WidgetItemUserlist::dragEnterEvent(QDragEnterEvent * event)
{
	Q_UNUSED(event)
			
	// Handle drag+drop hovering
	const QMimeData *md = event->mimeData();
	if( md->hasFormat("application/x-userid") ) {
		qDebug() << md->data("application/x-userid");
		event->acceptProposedAction();
		
	}
}

void WidgetItemUserlist::dropEvent(QDropEvent * event)
{
	Q_UNUSED(event)
	// Handle drag+drop
	//qDebug() << "Dropped";
}
