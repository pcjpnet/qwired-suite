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
#ifndef QWCLASSPRIVATECHAT_H
#define QWCLASSPRIVATECHAT_H

#include <QList>
#include <QString>
#include <QDateTime>
#include "classwireduser.h"

/**
	@author Bastian Bense <bb@bense.de>
*/
class QWClassPrivateChat
{
public:
    QWClassPrivateChat();
    ~QWClassPrivateChat();

	int pChatId;
	QList<int> pUsers;
	QList<int> pInvitedUsers;
	QString pTopic;
	ClassWiredUser pTopicSetter;
	QDateTime pTopicDate;

};

#endif
