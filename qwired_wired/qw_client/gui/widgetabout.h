/***************************************************************************
 *   Copyright (C) 2007 by Bastian Bense   *
 *   bb@bense.de   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef WIDGETABOUT_H
#define WIDGETABOUT_H

#include <QObject>
#include <QDesktopServices>
#include <QUrl>
#include "ui_WidgetAbout.h"

/**
	@author Bastian Bense <bb@bense.de>
*/
class WidgetAbout : public QWidget, public Ui_WidgetAbout
{
Q_OBJECT
public:
    WidgetAbout()
	{
		setupUi(this);
		setAttribute(Qt::WA_DeleteOnClose);
	};

    ~WidgetAbout()
	{ };

private slots:
	void on_fBtnDonate_clicked()
	{
		QDesktopServices::openUrl(QUrl("http://qwired.neo.de/donate/"));
	};

	
	void on_fBtnGoToWebsite_clicked()
	{
		QDesktopServices::openUrl(QUrl("http://qwired.neo.de/"));
	};

};

#endif
