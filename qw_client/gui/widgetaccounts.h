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
#ifndef WIDGETACCOUNTS_H
#define WIDGETACCOUNTS_H

#include <QWidget>
#include <ui_WidgetAccounts.h>
#include "../wired/classwireduser.h"

/**
	@author Bastian Bense <bb@bense.de>
*/
class WidgetAccounts : public QWidget, public Ui::WidgetAccounts
{
Q_OBJECT
public:
    WidgetAccounts(QWidget *parent = 0);
    ~WidgetAccounts();

private:
 	ClassWiredUser pCurrentUser;
	bool pNewMode;
	
	void enableGui(bool);
	void setPrivFlags(const ClassWiredUser);
		
private slots:
	void on_fList_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);
	void on_fBtnApply_clicked();
	void on_fBtnDelete_clicked();
	void on_fBtnNew_clicked();
	void on_fType_currentIndexChanged(int);
	void on_fGroup_currentIndexChanged(int);
	
public slots:
	void appendUserNames(QStringList);
	void appendGroupNames(QStringList);
	void loadUserSpec(ClassWiredUser);
	void loadGroupSpec(ClassWiredUser);
	
signals:
	void userSpecRequested(QString theName);
	void groupSpecRequested(QString theName);
	void saveUserSpec(ClassWiredUser);
	void saveGroupSpec(ClassWiredUser);
	void userDeleted(QString);
	void groupDeleted(QString);
	void createUser(ClassWiredUser);
	void editUser(ClassWiredUser);
	void createGroup(ClassWiredUser);
	void editGroup(ClassWiredUser);


};

#endif
