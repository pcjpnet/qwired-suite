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
#include "widgetaccounts.h"

WidgetAccounts::WidgetAccounts(QWidget *parent)
 : QWidget(parent)
{
	setAttribute(Qt::WA_DeleteOnClose);
	setupUi(this);
	pNewMode = false;
	fGroupBasic->setEnabled(false);
	fGroupFiles->setEnabled(false);
	fGroupAdmin->setEnabled(false);
	fGroupLimits->setEnabled(false);
// 	pCurrentUser = ClassWiredUser();
}


WidgetAccounts::~WidgetAccounts()
{
}

void WidgetAccounts::appendUserNames(QStringList theUsers) {
	QStringListIterator i(theUsers);
	while(i.hasNext()) {
		QString tmpLogin = i.next();
		QListWidgetItem *newItem = new QListWidgetItem;
		newItem->setIcon( QIcon(":/icons/icon_account.png") );
		newItem->setText(tmpLogin);
		newItem->setData(Qt::UserRole, 0); // user
		fList->addItem(newItem);
	}
	
}

void WidgetAccounts::appendGroupNames(QStringList theGroups) {
	QStringListIterator i(theGroups);
	fGroup->clear();
	fGroup->addItem(tr("No Group"), QString() );
	while(i.hasNext()) {
		QString tmpGroup = i.next();
		QListWidgetItem *newItem = new QListWidgetItem;
		newItem->setIcon( QIcon(":/icons/icon_accountgroup.png") );
		newItem->setText(tmpGroup);
		newItem->setData(Qt::UserRole, 1); // group
		fGroup->addItem(tmpGroup, tmpGroup);
		fList->addItem(newItem);
	}
}

void WidgetAccounts::on_fList_currentItemChanged(QListWidgetItem * current, QListWidgetItem * previous) {

	Q_UNUSED(previous)
	if(current) {
		if( current->data(Qt::UserRole).toInt() == 0 ) {
			emit userSpecRequested(current->text());
		} else {
			emit groupSpecRequested(current->text());
		}
		enableGui(false);
	} else {
		enableGui(false);
		fGroupBasic->setEnabled(false);
		fGroupFiles->setEnabled(false);
		fGroupAdmin->setEnabled(false);
		fGroupLimits->setEnabled(false);
	}
	
}

void WidgetAccounts::on_fBtnApply_clicked() {
	ClassWiredUser u;
	u.pLogin = fName->text();
	u.pPassword = fPassword->text();
	u.pAccountType = fType->currentIndex(); // 0=user, 1=group
	if(fGroup->currentIndex()>0)
		u.pGroupName = fGroup->currentText();
	u.privGetUserInfo = fBasicGetUserInfo->isChecked();
	u.privPostNews = fBasicPostNews->isChecked();
	u.privBroadcast = fBasicBroadcast->isChecked();
	u.privClearNews = fBasicClearNews->isChecked();
	u.privChangeTopic = fBasicSetTopic->isChecked();
	u.privDownload = fFilesDownload->isChecked();
	u.privUpload = fFilesUpload->isChecked();
	u.privUploadAnywhere = fFilesUploadAnywhere->isChecked();
	u.privViewDropboxes = fFilesViewDropBoxes->isChecked();
	u.privCreateFolders = fFilesCreateFolders->isChecked();
	u.privAlterFiles = fFilesMoveChange->isChecked();
	u.privDeleteFiles = fFilesDelete->isChecked();
	u.privCreateAccounts = fUsersCreate->isChecked();
	u.privEditAccounts = fUsersEdit->isChecked();
	u.privDeleteAccounts = fUsersDelete->isChecked();
	u.privElevatePrivileges = fUsersElevate->isChecked();
	u.privKickUsers = fUsersKick->isChecked();
	u.privBanUsers = fUsersBan->isChecked();
	u.privCannotBeKicked = fUsersNoKick->isChecked();
	u.privDownloadLimit = fLimitDown->text().toInt();
	u.privUploadLimit = fLimitUp->text().toInt();
	if(u.pAccountType==0) {
		u.pPassword = fPassword->text();
		if(pCurrentUser.pPassword != u.pPassword)// password, changed - reencode
			u.pPassword = u.cryptedPassword();
		
	}
	
	if(pNewMode) {
		QListWidgetItem *newItem = new QListWidgetItem;
		newItem->setText(u.pLogin);
		if(fType->currentIndex()==0) {
			emit createUser(u);
			newItem->setIcon( QIcon(":/icons/icon_account.png") );
			newItem->setData(Qt::UserRole, 0); // user
		} else {
			emit createGroup(u);
			newItem->setIcon( QIcon(":/icons/icon_accountgroup.png") );
			newItem->setData(Qt::UserRole, 1); // group
		}
		fList->addItem(newItem);
		fBtnDelete->click();
		
	} else { // Update
		if(fType->currentIndex()==0) {
			emit editUser(u);
		} else {
			emit editGroup(u);
		}
		fBtnApply->setEnabled(false);
		fList->setCurrentRow(-1);
	}
}


void WidgetAccounts::on_fType_currentIndexChanged(int index) {
	if(!pNewMode) return;
	fPassword->setEnabled( index==0 );
	fGroup->setEnabled( index==0 );
	bool b = (index==0 && fGroup->currentIndex()==0) || (index==1);
	fGroupBasic->setEnabled(b);
	fGroupFiles->setEnabled(b);
	fGroupAdmin->setEnabled(b);
	fGroupLimits->setEnabled(b);
}

void WidgetAccounts::on_fGroup_currentIndexChanged(int index) {
	bool b = index==0;
	fGroupBasic->setEnabled(b);
	fGroupFiles->setEnabled(b);
	fGroupAdmin->setEnabled(b);
	fGroupLimits->setEnabled(b);
}



void WidgetAccounts::on_fBtnNew_clicked() {
	// Switch to "new account" mode and lock down the GUI.
	pNewMode = true;
 	pCurrentUser = ClassWiredUser();
	setPrivFlags(pCurrentUser);
	
	fBtnApply->setText(tr("Create"));
	
	fList->setCurrentRow(-1);
	fList->setEnabled(false);
	enableGui(true);
	fGroup->setCurrentIndex(0);
	fType->setCurrentIndex(0);
	fGroup->setEnabled(true);
	fBtnNew->setEnabled(false);
	fBtnDelete->setEnabled(true);
	fBtnDelete->setText(tr("Cancel"));
	fGroupBasic->setEnabled(true);
	fGroupFiles->setEnabled(true);
	fGroupAdmin->setEnabled(true);
	fGroupLimits->setEnabled(true);

	
}



void WidgetAccounts::on_fBtnDelete_clicked() {
	if(pNewMode) {
		fBtnApply->setText(tr("Apply Changes"));
		fBtnDelete->setText(tr("Delete"));
		fBtnNew->setEnabled(true);
		pNewMode = false;
		fList->setCurrentRow(-1);
		fList->setEnabled(true);
		enableGui(false);
		fGroup->setCurrentIndex(0);
		fType->setCurrentIndex(0);
		fGroup->setEnabled(false);
		fBtnDelete->setEnabled(false);
		fGroupBasic->setEnabled(false);
		fGroupFiles->setEnabled(false);
		fGroupAdmin->setEnabled(false);
		fGroupLimits->setEnabled(false);
		
	} else {
		QMessageBox::StandardButton result = QMessageBox::question(this,
			tr("Delete Account or Group"),
			tr("Are you sure you want to delete the current account or group? This can not be undone."),
			QMessageBox::Ok | QMessageBox::Cancel,
			QMessageBox::Ok );
		if(result == QMessageBox::Ok) {
			QListWidgetItem *tmpItm = fList->takeItem( fList->currentRow() );
			QString tmpName = tmpItm->text();
			int tmpType = tmpItm->data(Qt::UserRole).toInt();
			delete tmpItm; tmpItm=0;
			fList->setCurrentRow(-1);
			if(tmpType==0)
					emit userDeleted(tmpName);
				else emit groupDeleted(tmpName);
		}
	}
}

void WidgetAccounts::enableGui(bool e) {
	fType->setEnabled(e);
	fName->setEnabled(e);
	fGroup->setEnabled(e);
	fPassword->setEnabled(e);;
	fBtnApply->setEnabled(e);
	fBtnDelete->setEnabled(e);
}

void WidgetAccounts::loadGroupSpec(ClassWiredUser u) {
	fName->setText(u.pLogin);
	enableGui(true);
	setPrivFlags(u);
}

void WidgetAccounts::loadUserSpec(ClassWiredUser u) {
	fName->setText(u.pLogin);
	enableGui(true);
	setPrivFlags(u);
}

void WidgetAccounts::setPrivFlags(ClassWiredUser u){
 	pCurrentUser = u; // to recover the password hash later
	fGroupBasic->setEnabled(u.pGroupName.isEmpty());
	fGroupLimits->setEnabled(u.pGroupName.isEmpty());
	fGroupFiles->setEnabled(u.pGroupName.isEmpty());
	fGroupAdmin->setEnabled(u.pGroupName.isEmpty());
	fGroup->setEnabled(u.pAccountType==0);
	fType->setCurrentIndex(u.pAccountType);
	fType->setEnabled(false);
	fName->setEnabled(false);
	fPassword->setEnabled(u.pAccountType==0);
	fPassword->setText(u.pPassword);
	
	fLimitDown->setText( QString::number(u.privDownloadLimit) );
	fLimitUp->setText( QString::number(u.privUploadLimit) );
	
	fBasicGetUserInfo->setChecked( u.privGetUserInfo );
	fBasicPostNews->setChecked( u.privPostNews );
	fBasicBroadcast->setChecked( u.privBroadcast );
	fBasicSetTopic->setChecked( u.privChangeTopic );
	fBasicClearNews->setChecked( u.privClearNews );

	fFilesDownload->setChecked( u.privDownload );
	fFilesUpload->setChecked( u.privUpload );
	fFilesUploadAnywhere->setChecked( u.privUploadAnywhere );
	fFilesViewDropBoxes->setChecked( u.privViewDropboxes );
	fFilesCreateFolders->setChecked( u.privCreateFolders );
	fFilesMoveChange->setChecked( u.privAlterFiles );
	fFilesDelete->setChecked( u.privDeleteFiles );

	fUsersCreate->setChecked( u.privCreateAccounts );
	fUsersEdit->setChecked( u.privEditAccounts );
	fUsersDelete->setChecked( u.privDeleteAccounts );
	fUsersElevate->setChecked( u.privElevatePrivileges );
	fUsersKick->setChecked( u.privKickUsers );
	fUsersBan->setChecked( u.privBanUsers );
	fUsersNoKick->setChecked( u.privCannotBeKicked );



	int tmpIdx = fGroup->findData(u.pGroupName);
	if(tmpIdx==-1) {
		fGroup->setCurrentIndex(0);
	} else {
		fGroup->setCurrentIndex(tmpIdx);
	}

	
}







