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

 
#ifndef WIDGETCONNECT_H
#define WIDGETCONNECT_H

#include <QtCore>
#include <QtGui>
#include "ui_WidgetConnect.h"



/**
	@author Bastian Bense <bastibense@gmail.com>
 */
class WidgetConnect : public QWidget, public Ui::WidgetConnect
{
	
Q_OBJECT
		
public:
    WidgetConnect(QWidget *parent = 0);
    ~WidgetConnect();


private slots:
    void on_btnConnect_clicked();
	void bookmarkSelected(QAction *action);
	void loadBookmarks();

public slots:
    void setProgressBar(int value, int max);
    
signals:
    void onConnnectReady(QString theHost, QString theLogin, QString thePassword);
    
};

#endif
