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
 
#ifndef WIDGETTRACKER_H
#define WIDGETTRACKER_H

#include <QtCore>
#include <QtGui>
#include "ui_WidgetTracker.h"
#include "../wired/wiredsocket.h"
#include "../wired/classtrackerserver.h"

/**
	@author Bastian Bense <bb@bense.de>
*/
class WidgetTracker : public QWidget, public Ui::WidgetTracker
{
Q_OBJECT
public:
    WidgetTracker(QWidget *parent = 0);
    ~WidgetTracker();
	QPointer<WiredSocket> pSocket;

private:
	QPointer<QStandardItemModel> pModel;
	QPointer<QSortFilterProxyModel> pSortModel;
	void updateTrackerList();

private slots:
	
	void on_fBtnRefresh_clicked();
	void trackerServersReceived(QList<ClassTrackerServer> theList);
	void handleSocketError(QAbstractSocket::SocketError);

};

#endif
