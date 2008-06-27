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
#ifndef WIDGETCONFERENCELIST_H3
#define WIDGETCONFERENCELIST_H

#include <QWidget>
#include "ui_WidgetConferenceList.h"
#include "../wired/wiredsocket.h"

/**
	@author Bastian Bense <bb@bense.de>
*/
class WidgetConferenceList : public QWidget, public Ui_WidgetConferenceList
{
Q_OBJECT
public:
    WidgetConferenceList(QWidget *parent = 0);
    ~WidgetConferenceList();
	QPointer<WiredSocket> pSocket;

};

#endif
