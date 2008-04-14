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

 
#ifndef WIDGETSENDPRIVMSG_H
#define WIDGETSENDPRIVMSG_H

#include <QWidget>
#include "ui_WidgetSendPrivMsg.h"
#include "../wired/classwireduser.h"
//#include "wsessionmanager.h"

/**
	@author Bastian Bense <bastibense@gmail.com>
 */
class WidgetSendPrivMsg : public QWidget, public Ui::WidgetSendPrivMsg
{
		Q_OBJECT
	public:
// 		void setTarget ( ClassWiredSession *theSession, int theUserID );
// 		QPointer<ClassWiredSession> pSession;
		WidgetSendPrivMsg ( QWidget *parent = 0 );
		~WidgetSendPrivMsg();
		int pTargetID;
		void addText(QString theText, int theWho); // theWho: 0=me, 1=them

	signals:
		void newMessage(int theUser, QString theText);
		
	private slots:
		void on_fInput_returnPressed();
// 		void on_fBtnSend_clicked();
// 		void on_fBtnCancel_clicked();
		
		
	private:
		ClassWiredUser pUser;
// 		bool eventFilter(QObject *obj, QEvent *event);

};

#endif
