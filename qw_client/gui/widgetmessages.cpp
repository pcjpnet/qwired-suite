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
#include "widgetmessages.h"
#include <QtCore>

WidgetMessages::WidgetMessages(QWidget *parent)
 : QWidget(parent)
{
	setAttribute(Qt::WA_DeleteOnClose);
	setupUi(this);

	// Notification manager
	WiredSingleton *tmpS = &WSINGLETON::Instance();
	connect( tmpS, SIGNAL(prefsChanged()), this, SLOT(reloadPreferences()) );
	initPrefs();
}


WidgetMessages::~WidgetMessages()
{
}

void WidgetMessages::reloadPreferences() {
	initPrefs();
}

void WidgetMessages::initPrefs() {
	
}









