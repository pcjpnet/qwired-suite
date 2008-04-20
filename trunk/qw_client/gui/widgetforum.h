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

 
#ifndef WIDGETFORUM_H
#define WIDGETFORUM_H

#include "ui_WidgetForum.h"
#include "modeluserlist.h"
#include "delegateuserlist.h"

/**
	@author Bastian Bense <bastibense@gmail.com>
 */

class ClassWiredSession;

class WidgetForum : public QWidget, public Ui::WidgetForum
{
	Q_OBJECT
			
public:
	WidgetForum(QWidget *parent = 0);
	~WidgetForum();
	void setUserListModel(ModelUserList *model);
	QPointer<ClassWiredSession> pSession;
	
	int pChatID;
	
	bool pEmoticonsEnabled;
	
	
public slots:
	void writeToChat(QString theUser, QString theText, bool theEmote);
	void writeEventToChat(QString theMsg);
	
private:
	QColor pChatTextColor;
	QColor pChatTimeColor;
	QColor pChatEventColor;
	int pChatStyle;
	bool pChatShowTime;
	QFont pChatFont;
	QPointer<DelegateUserlist> userlistDelegate;
	QPointer<QMenu> pInviteMenu;
	void updateInviteMenu();
	
	
private slots:
	void on_fUsers_doubleClicked ( const QModelIndex & index );
	void on_fChatInput_returnPressed();
	void on_fBtnMsg_clicked();
	void on_fBtnKick_clicked();
	void on_fBtnBan_clicked();
	void on_fBtnInfo_clicked();
	void on_fBtnChat_clicked();
	void onUserlistSelectionChanged(const QItemSelection &current, const QItemSelection &previous);
	void inviteMenuTriggered(QAction *action);

	void reloadPrefs();
	

};

#endif
