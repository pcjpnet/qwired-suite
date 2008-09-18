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

 
#ifndef CONNWINDOW_H
#define CONNWINDOW_H

#include <QWidget>
#include <QPointer>
#include <QMainWindow>

#include "ui_WinMain.h"
#include "gui/widgetabout.h"
#include "wired/classwireduser.h"
#include "../general/wiredcommon.h"

/**
	@author Bastian Bense <bastibense@gmail.com>
 */
class ConnWindow
	: public QMainWindow, public Ui_MainWindow
{
Q_OBJECT

public:
	
	QPointer<WidgetAbout> pWidgetAbout;
	
	ConnWindow(QWidget *parent=0)
	{
		Q_UNUSED(parent)
		setupUi(this);
		setUnifiedTitleAndToolBarOnMac(true);
		setAttribute(Qt::WA_DeleteOnClose);
		setWindowIcon( QIcon(":/icons/qwired_logo_32.png") );
	};
	
	~ConnWindow()
	{ };
	
	
	
public slots:
	
	void toggleVisible()
	{
		if(this->isMinimized()) {
			this->showNormal();
		} else {
			if(this->isVisible()) this->hide();
			else this->show();
		}
	};

	
private slots:
	
	void on_actionAbout_triggered(bool)
	{
		if(!pWidgetAbout)
			pWidgetAbout = new WidgetAbout();
		pWidgetAbout->show();
	};
	


};

#endif
