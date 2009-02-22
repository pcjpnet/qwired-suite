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

 
#ifndef WIDGETPREFS_H
#define WIDGETPREFS_H

#include <QWidget>
#include "ui_WidgetPrefs.h"

/**
	@author Bastian Bense <bastibense@gmail.com>
 */
class WidgetPrefs
	: public QWidget, private Ui::WidgetPrefs
{

Q_OBJECT
		
public:
    WidgetPrefs(QWidget *parent = 0);
    ~WidgetPrefs();
	
private:
    void initPrefs();
	void initEventsMenu();
	bool pNickChanged;
	bool pStatusChanged;
	bool pIconChanged;
	
    
private slots:
    void savePrefs();

	// General
    void on_btnGeneralSetIcon_clicked();
    void on_btnGeneralDefaultIcon_clicked();
	void on_fNickname_editingFinished();
	void on_fStatus_editingFinished();

	// Interface, Chat
	void on_fBtnIfChatFont_clicked();
	void on_fBtnIfUserlistFont_clicked();
	void on_fBtnIfUserlistCompactLarge_toggled(bool checked);
	void on_fBtnIfUserlistCompactSmall_toggled(bool checked);
	void on_fBtnIfUserlistAlternateRow_stateChanged(int state);

	// Interface, News
	void on_fBtnIfNewsFont_clicked();
	
	// Files
	void on_btnFilesFolderSet_clicked();

	// Trackers
	void on_fBtnTrackerNew_clicked();
	void on_fBtnTrackerDelete_clicked();
	void on_fTrackerList_currentRowChanged(int);
	void on_fTrackerName_editingFinished();
	void on_fTrackerAddress_editingFinished();

	// Bookmarks
	void on_fBookmarksList_currentRowChanged(int);
	void on_btnBMAdd_clicked();
	void on_btnBMRemove_clicked();
	void on_fBMName_editingFinished();
	void on_fBMAddr_editingFinished();
	void on_fBMLogin_editingFinished();
	void on_fBMPassword_editingFinished();
	void on_fBMAutoConnect_stateChanged(int);

	// Events
	void on_fEventsList_currentRowChanged(int);
	void on_fEventsSysCmd_editingFinished();
	void on_fEventsSound_clicked();
	void on_fBtnOk_clicked();
	void on_fEventsPostChat_stateChanged(int state);
	void on_fEventsDisplayTrayMsg_stateChanged(int state);
	void on_fEventsSysCmdEnabled_stateChanged(int state);
};

#endif