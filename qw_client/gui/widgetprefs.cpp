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

 
 
#include "widgetprefs.h"
#include "general/wiredcommon.h"
#include <QtCore>
#include <QtGui>

WidgetPrefs::WidgetPrefs(QWidget *parent)
 : QWidget(parent)
{
	setAttribute(Qt::WA_DeleteOnClose);
	setupUi(this);
	
	pNickChanged = false;
	pStatusChanged = false;
	pIconChanged = false;

	// Language menu
	fLanguage->addItem(tr("Automatic (default)"), QString("_auto_"));
	fLanguage->addItem("English", QString(""));
	fLanguage->addItem("French", QString("fr"));
	fLanguage->addItem("German", QString("de"));
	fLanguage->addItem("Italian", QString("it"));
	fLanguage->addItem("Portuguese", QString("pt"));
	fLanguage->addItem("Spanish", QString("es"));
    fLanguage->addItem("Japanese", QString("jp"));
	
	initEventsMenu();
	initPrefs();
	
}


WidgetPrefs::~WidgetPrefs() {
	savePrefs();
}


void WidgetPrefs::initEventsMenu() {
	QListWidgetItem *item;
	fEventsList->clear();

	item = new QListWidgetItem(QIcon(":/icons/icn_event.png"), tr("Server Connected"), fEventsList);
	item->setData(Qt::UserRole, QString("ServerConnected"));
	item = new QListWidgetItem(QIcon(":/icons/icn_event.png"), tr("Server Disconnected"), fEventsList);
	item->setData(Qt::UserRole, QString("ServerDisconnected"));
	item = new QListWidgetItem(QIcon(":/icons/icn_event.png"), tr("Server Error"), fEventsList);
	item->setData(Qt::UserRole, QString("ServerError"));
	item = new QListWidgetItem(QIcon(":/icons/icn_event.png"), tr("User Joined"), fEventsList);
	item->setData(Qt::UserRole, QString("UserJoined"));
	item = new QListWidgetItem(QIcon(":/icons/icn_event.png"), tr("User Changed Nick"), fEventsList);
	item->setData(Qt::UserRole, QString("UserChangedNick"));
	item = new QListWidgetItem(QIcon(":/icons/icn_event.png"), tr("User Changed Status"), fEventsList);
	item->setData(Qt::UserRole, QString("UserChangedStatus"));
	item = new QListWidgetItem(QIcon(":/icons/icn_event.png"), tr("User Left"), fEventsList);
	item->setData(Qt::UserRole, QString("UserLeft"));
	item = new QListWidgetItem(QIcon(":/icons/icn_event.png"), tr("Chat Received"), fEventsList);
	item->setData(Qt::UserRole, QString("ChatReceived"));
	item = new QListWidgetItem(QIcon(":/icons/icn_event.png"), tr("Message Received"), fEventsList);
	item->setData(Qt::UserRole, QString("MessageReceived"));
	item = new QListWidgetItem(QIcon(":/icons/icn_event.png"), tr("News Posted"), fEventsList);
	item->setData(Qt::UserRole, QString("NewsPosted"));
	item = new QListWidgetItem(QIcon(":/icons/icn_event.png"), tr("Broadcast Received"), fEventsList);
	item->setData(Qt::UserRole, QString("BroadcastReceived"));
	item = new QListWidgetItem(QIcon(":/icons/icn_event.png"), tr("Transfer Started"), fEventsList);
	item->setData(Qt::UserRole, QString("TransferStarted"));
	item = new QListWidgetItem(QIcon(":/icons/icn_event.png"), tr("Transfer Finished"), fEventsList);
	item->setData(Qt::UserRole, QString("TransferFinished"));
}


// Load preferences
void WidgetPrefs::initPrefs() {
	QSettings conf;
	
	// GENERAL TAB
	//
	fNickname->setText( conf.value("general/nickname", "Unnamed").toString() );
	fStatus->setText( conf.value("general/status", "Qwired Newbie").toString() );

	if(conf.contains("general/icon")) {
		QPixmap tmpIcon = conf.value("general/icon").value<QPixmap>();
		fIcon->setPixmap(tmpIcon);
	} else {
		QPixmap tmpIcon(":/icons/qwired_logo_32.png");
		fIcon->setPixmap(tmpIcon);
	}
	fLanguage->setCurrentIndex( fLanguage->findData( conf.value("general/language", "_auto_").toString() ) );

	
	
	// FAVOURITES TAB
	//
	fBookmarksList->clear();
	int tmpCnt = conf.beginReadArray("bookmarks");
	for(int i=0; i<tmpCnt; ++i) {
		conf.setArrayIndex(i);
		QListWidgetItem *item = new QListWidgetItem();
		item->setText(conf.value("name").toString());
		item->setData(Qt::UserRole, conf.value("address").toString());
		item->setData(Qt::UserRole+1, conf.value("login").toString());
		item->setData(Qt::UserRole+2, conf.value("password").toString());
		item->setData(Qt::UserRole+3, conf.value("autoconnect").toBool());
		item->setIcon(QIcon(":/icons/icn_trackerserver.png"));
		fBookmarksList->addItem(item);
	}
	conf.endArray();
	fBookmarksList->setCurrentRow(-1);
	
	// FILES TAB
	//
	fFilesFolder->setText(conf.value("files/download_dir", QDir::homePath()).toString());
	fFilesQueue->setChecked(conf.value("files/queue_local", false).toBool());
	fFilesEncrypt->setChecked(conf.value("files/encrypt", true).toBool());

	// INTERFACE TAB
	//
	QFont f;
	bool tmpBool;
	int tmpInt;

	// Interface, Chat
	f.fromString( conf.value("interface/chat/font", WiredSingleton::systemMonospaceFont().append(",11") ).toString() );
	fIfChatFont->setText( QString("%1, %2pt").arg(f.family()).arg(f.pointSize()) );
	fBtnIfChatText->setColor( WiredSingleton::colorFromPrefs("interface/chat/text/color", Qt::black) );
	fBtnIfChatBack->setColor( WiredSingleton::colorFromPrefs("interface/chat/back/color", Qt::white) );
	fBtnIfChatURLs->setColor( WiredSingleton::colorFromPrefs("interface/chat/urls/color", Qt::blue) );
	fBtnIfChatEvents->setColor( WiredSingleton::colorFromPrefs("interface/chat/events/color", Qt::red) );

	// Interface, news
	fBtnIfNewsText->setColor( WiredSingleton::colorFromPrefs("interface/news/text/color", Qt::black) );
	fBtnIfNewsBack->setColor( WiredSingleton::colorFromPrefs("interface/news/back/color", Qt::white) );
	fBtnIfNewsTitles->setColor( WiredSingleton::colorFromPrefs("interface/news/titles/color", Qt::gray) );
	f.fromString( conf.value("interface/news/font", WiredSingleton::systemMonospaceFont().append(",11") ).toString() );
	fIfNewsFont->setText( QString("%1, %2pt").arg(f.family()).arg(f.pointSize()) );

	// Userlist
	f.fromString( conf.value("interface/userlist/font", QApplication::font().toString() ).toString() );
	fIfUserlistFont->setText( QString("%1, %2pt").arg(f.family()).arg(f.pointSize()) );
	tmpBool = conf.value("interface/userlist/compact",false).toBool();
	fBtnIfUserlistCompactLarge->setChecked(!tmpBool);
	fBtnIfUserlistCompactSmall->setChecked(tmpBool);
	fBtnIfUserlistAlternateRow->setCheckState(Qt::Checked);

	

	// Fileslist
	f.fromString( conf.value("interface/fileslist/font", QApplication::font().toString() ).toString() );
	fIfFilesFont->setText( QString("%1, %2pt").arg(f.family()).arg(f.pointSize()) );
	tmpBool = conf.value("interface/fileslist/alternate_bg",false).toBool();
	fBtnIfFilesAlternateRow->setChecked(tmpBool);

	// Trackers
	int count = conf.beginReadArray("trackers");
	for(int i=0; i<count; i++) {
		conf.setArrayIndex(i);
		QListWidgetItem *item = new QListWidgetItem(fTrackerList);
		item->setText( conf.value("name").toString() );
		item->setIcon(QIcon(":/icons/icn_tracker.png"));
		item->setData( Qt::UserRole, conf.value("address").toString() );
	}
	conf.endArray();

	// === Chat ===
	// ============
	tmpInt = conf.value("interface/chat/style",0).toInt();
	fChatStyleQwired->setChecked(tmpInt==0);
	fChatStyleIRC->setChecked(tmpInt==1);

	fChatEmoticons->setChecked(conf.value("interface/chat/emoticons", true).toBool());
	fChatTimestampLine->setChecked(conf.value("interface/chat/show_time", false).toBool());
	fChatTimeColor->setColor( WiredSingleton::colorFromPrefs("interface/chat/time/color", Qt::darkGray) );
	
}




void WidgetPrefs::savePrefs() {
	qDebug() << "WidgetPrefs: Saving prefs...";
	QSettings conf;
	
	// GENERAL TAB
	conf.setValue("general/nickname", fNickname->text());
	conf.setValue("general/status", fStatus->text());
	conf.setValue("general/language", fLanguage->itemData(fLanguage->currentIndex()).toString() );
	
	// INTERFACE, chat
	conf.setValue("interface/chat/text/color", fBtnIfChatText->selectedColor() );
	conf.setValue("interface/chat/back/color", fBtnIfChatBack->selectedColor() );
	conf.setValue("interface/chat/urls/color", fBtnIfChatURLs->selectedColor() );
	conf.setValue("interface/chat/events/color", fBtnIfChatEvents->selectedColor() );
	conf.setValue("interface/chat/emoticons", fChatEmoticons->checkState()==Qt::Checked);

	// Interface, news
	conf.setValue("interface/news/text/color", fBtnIfNewsText->selectedColor() );
	conf.setValue("interface/news/back/color", fBtnIfNewsBack->selectedColor() );
	conf.setValue("interface/news/titles/color", fBtnIfNewsTitles->selectedColor() );

	// Fileslist
	conf.setValue("interface/fileslist/alternate_bg", fBtnIfFilesAlternateRow->checkState()==Qt::Checked);
	
	// FILES TAB
	conf.setValue("files/download_dir", fFilesFolder->text());
	conf.setValue("files/queue_local", fFilesQueue->isChecked());
	conf.setValue("files/encrypt", fFilesEncrypt->checkState()==Qt::Checked );

	// Trackers
	conf.beginWriteArray("trackers");
	for(int i=0; i<fTrackerList->count(); i++) {
		conf.setArrayIndex(i);
		QListWidgetItem *item = fTrackerList->item(i);
		if(item) {
			conf.setValue("name", item->text());
			conf.setValue("address", item->data(Qt::UserRole).toString() );
		}
	}
	conf.endArray();

	// FAVOURITES TAB
	//
	conf.beginWriteArray("bookmarks");
	for(int i=0; i<fBookmarksList->count(); i++) {
		conf.setArrayIndex(i);
		QListWidgetItem *item = fBookmarksList->item(i);
		if(item) {
			conf.setValue("name", item->text());
			conf.setValue("address", item->data(Qt::UserRole).toString() );
			conf.setValue("login", item->data(Qt::UserRole+1).toString() );
			conf.setValue("password", item->data(Qt::UserRole+2).toString() );
			conf.setValue("autoconnect", item->data(Qt::UserRole+3).toBool() );
		}
	}
	conf.endArray();
	

	// === Chat ===
	if(fChatStyleQwired->isChecked()) {
		conf.setValue("interface/chat/style", 0);
	} else if(fChatStyleIRC->isChecked()) {
		conf.setValue("interface/chat/style", 1);
	}
	conf.setValue("interface/chat/show_time", fChatTimestampLine->isChecked());
	conf.setValue("interface/chat/time/color", fChatTimeColor->selectedColor() );
			
	// Notify everyone
	WiredSingleton *tmpS = &WSINGLETON::Instance();
	tmpS->notifyPrefsChanged();
	this->close();
}



void WidgetPrefs::on_btnGeneralSetIcon_clicked() {
	QSettings settings;
	QString tmpFile = QFileDialog::getOpenFileName(this, tr("Select Icon"), QDir::homePath(), tr("Images (*.png *.xpm *.jpg)") );
	if( !tmpFile.isEmpty() ) {
		QPixmap tmpImg(tmpFile);
		if( !tmpImg.isNull() ) {
			tmpImg = tmpImg.scaled(32, 32, Qt::KeepAspectRatio);
			fIcon->setPixmap(tmpImg);
			settings.setValue("general/icon", tmpImg);
		} else {
			// File open error
			QMessageBox::warning(this, tr("Invalid File"), tr("Could not open the selected image."));
		}
	}
	
}

// Reset the icon
void WidgetPrefs::on_btnGeneralDefaultIcon_clicked() {
	QSettings s;
	QPixmap tmpIcon(":/icons/qwired_logo_32.png");
	s.setValue("general/icon", tmpIcon);
	fIcon->setPixmap(tmpIcon);
}

// Files
///////////

void WidgetPrefs::on_btnFilesFolderSet_clicked()
{
	// Select a file as default download directory.
	QString tmpDir = QFileDialog::getExistingDirectory(this, tr("Select Download Directory"),
			QDir::homePath(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks );
	if(!tmpDir.isEmpty()) {
		QSettings settings;
		settings.setValue("files/download_dir", tmpDir);
		fFilesFolder->setText(tmpDir);
	}

}

void WidgetPrefs::on_fNickname_editingFinished() {
	pNickChanged = true;
}

void WidgetPrefs::on_fStatus_editingFinished() {
	pStatusChanged = true;
}

void WidgetPrefs::on_fBtnOk_clicked() {
	savePrefs();	
}

void WidgetPrefs::on_fBtnIfChatFont_clicked() {
	QSettings s;
	bool ok;
	QFont f;
	f.setStyleHint(QFont::TypeWriter);
	f.fromString( s.value("interface/chat/font", WiredSingleton::systemMonospaceFont().append(",11")).toString() );
	f = QFontDialog::getFont(&ok, f, this);
	if(ok) {
		s.setValue("interface/chat/font", f.toString());
		fIfChatFont->setText( QString("%1, %2pt").arg(f.family()).arg(f.pointSize()) );
	}
}

void WidgetPrefs::on_fBtnIfUserlistFont_clicked() {
	QSettings s;
	bool ok;
	QFont f;
	f.fromString( s.value("interface/userlist/font", QApplication::font().toString() ).toString() );
	f = QFontDialog::getFont(&ok, f, this);
	if(ok) {
		s.setValue("interface/userlist/font", f.toString());
		fIfUserlistFont->setText( QString("%1, %2pt").arg(f.family()).arg(f.pointSize()) );
	}
}

void WidgetPrefs::on_fBtnIfUserlistCompactLarge_toggled(bool checked) {
	if(!checked) return;
	QSettings s;
	s.setValue("interface/userlist/compact",false);
	fBtnIfUserlistCompactSmall->setChecked(false);
}

void WidgetPrefs::on_fBtnIfUserlistCompactSmall_toggled(bool checked) {
	if(!checked) return;
	QSettings s;
	s.setValue("interface/userlist/compact",true);
	fBtnIfUserlistCompactLarge->setChecked(false);
}

void WidgetPrefs::on_fBtnIfUserlistAlternateRow_stateChanged(int state) {
	QSettings s;
	s.setValue("interface/userlist/alternate_bg", state==Qt::Checked);
}


void WidgetPrefs::on_fBtnIfNewsFont_clicked() {
	QSettings s;
	bool ok;
	QFont f;
	f.fromString( s.value("interface/news/font", WiredSingleton::systemMonospaceFont().append(",11") ).toString() );
	f = QFontDialog::getFont(&ok, f, this);
	if(ok) {
		s.setValue("interface/news/font", f.toString());
		fIfNewsFont->setText( QString("%1, %2pt").arg(f.family()).arg(f.pointSize()) );
	}
}


//
// Trackers
//

void WidgetPrefs::on_fTrackerName_editingFinished() {
	QListWidgetItem *item = fTrackerList->currentItem();
	if(!item) return;
	item->setText( fTrackerName->text() );
}

void WidgetPrefs::on_fTrackerAddress_editingFinished() {
	QListWidgetItem *item = fTrackerList->currentItem();
	if(!item) return;
	item->setData(Qt::UserRole, fTrackerAddress->text() );
}

void WidgetPrefs::on_fBtnTrackerNew_clicked() {
	QListWidgetItem *item = new QListWidgetItem(fTrackerList);
	item->setText(tr("New Tracker"));
	item->setIcon(QIcon(":/icons/icn_tracker.png"));
	item->setData(Qt::UserRole, tr("my.tracker.com"));
	fTrackerList->setCurrentItem(item);
}

void WidgetPrefs::on_fTrackerList_currentRowChanged(int current) {
	if(current==-1) { // Disable gui
		fTrackerGroup->setEnabled(false);
		fBtnTrackerDelete->setEnabled(false);
	} else {
		QListWidgetItem *item = fTrackerList->item(current);
		fTrackerGroup->setEnabled(true);
		fTrackerName->setText( item->text() );
		fTrackerAddress->setText( item->data(Qt::UserRole).toString() );
		fBtnTrackerDelete->setEnabled(true);
	}
}

void WidgetPrefs::on_fBtnTrackerDelete_clicked() {
	int tmpRow = fTrackerList->currentRow();
	if(tmpRow>-1) {
		QListWidgetItem *item = fTrackerList->takeItem(tmpRow);
		delete item;
	}
	fTrackerList->setCurrentRow(-1);
}



// BOOKMARKS
//
void WidgetPrefs::on_btnBMAdd_clicked() {
	QListWidgetItem *item = new QListWidgetItem(fBookmarksList);
	item->setText(tr("New Bookmark"));
	item->setIcon(QIcon(":/icons/icn_trackerserver.png"));
	fBookmarksList->setCurrentRow( fBookmarksList->count()-1);
}

void WidgetPrefs::on_btnBMRemove_clicked() {
	int tmpRow = fBookmarksList->currentRow();
	if(tmpRow>-1) {
		QListWidgetItem *item = fBookmarksList->takeItem(tmpRow);
		delete item;
	}
	fBookmarksList->setCurrentRow(-1);
}

void WidgetPrefs::on_fBookmarksList_currentRowChanged(int current) {
	if(current==-1) { // Disable gui
		fBMGroup->setEnabled(false);
		btnBMRemove->setEnabled(false);
	} else {
		QListWidgetItem *item = fBookmarksList->item(current);
		fBMGroup->setEnabled(true);
		btnBMRemove->setEnabled(true);
		fBMName->setText( item->text() );
		fBMAddr->setText( item->data(Qt::UserRole).toString() );
		fBMLogin->setText( item->data(Qt::UserRole+1).toString() );
		fBMPassword->setText( item->data(Qt::UserRole+2).toString() );
		fBMAutoConnect->setChecked( item->data(Qt::UserRole+3).toBool() );
	}
}

void WidgetPrefs::on_fBMName_editingFinished() {
	QListWidgetItem *item = fBookmarksList->currentItem();
	if(!item) return;
	item->setText(fBMName->text());
}

void WidgetPrefs::on_fBMAddr_editingFinished() {
	QListWidgetItem *item = fBookmarksList->currentItem();
	if(!item) return;
	item->setData(Qt::UserRole, fBMAddr->text());
}

void WidgetPrefs::on_fBMLogin_editingFinished() {
	QListWidgetItem *item = fBookmarksList->currentItem();
	if(!item) return;
	item->setData(Qt::UserRole+1, fBMLogin->text());
}

void WidgetPrefs::on_fBMPassword_editingFinished() {
	QListWidgetItem *item = fBookmarksList->currentItem();
	if(!item) return;
	item->setData(Qt::UserRole+2, fBMPassword->text());
}

void WidgetPrefs::on_fBMAutoConnect_stateChanged(int state) {
	QListWidgetItem *item = fBookmarksList->currentItem();
	if(!item) return;
	item->setData(Qt::UserRole+3, state==Qt::Checked);
}

void WidgetPrefs::on_fEventsList_currentRowChanged(int current) {
	QSettings conf;

	fEventsSound->setEnabled(current!=-1);
	fEventsPostChat->setEnabled(current!=-1);
	fEventsDisplayTrayMsg->setEnabled(current!=-1);
	fEventsSysCmdEnabled->setEnabled(current!=-1);
	fEventsSysCmd->setEnabled(current!=-1);
	
	if(current==-1) return;
	
	QListWidgetItem *item = fEventsList->item(current);
	if(!item) return;
	QString tmpKey = item->data(Qt::UserRole).toString();

	fEventsSound->setText( conf.value(QString("events/%1/sound").arg(tmpKey), tr("(none)") ).toString());
	fEventsPostChat->setChecked( conf.value(QString("events/%1/chat").arg(tmpKey) ).toBool());
	fEventsDisplayTrayMsg->setChecked( conf.value(QString("events/%1/traymsg").arg(tmpKey) ).toBool());
	fEventsSysCmdEnabled->setChecked( conf.value(QString("events/%1/syscmd_enabled").arg(tmpKey)).toBool() );
	fEventsSysCmd->setText( conf.value(QString("events/%1/syscmd").arg(tmpKey)).toString() );
	
}

void WidgetPrefs::on_fEventsDisplayTrayMsg_stateChanged(int state) {
	QListWidgetItem *item = fEventsList->item(fEventsList->currentRow());
	QString tmpKey = item->data(Qt::UserRole).toString();
	QSettings conf;
	conf.setValue(QString("events/%1/traymsg").arg(tmpKey), state==Qt::Checked);
}


void WidgetPrefs::on_fEventsSysCmd_editingFinished() {
	QListWidgetItem *item = fEventsList->item(fEventsList->currentRow());
	QString tmpKey = item->data(Qt::UserRole).toString();
	QSettings conf;
	conf.setValue(QString("events/%1/syscmd").arg(tmpKey), fEventsSysCmd->text());
}

void WidgetPrefs::on_fEventsSysCmdEnabled_stateChanged(int state) {
	QListWidgetItem *item = fEventsList->item(fEventsList->currentRow());
	QString tmpKey = item->data(Qt::UserRole).toString();
	QSettings conf;
	conf.setValue(QString("events/%1/syscmd_enabled").arg(tmpKey), state==Qt::Checked);
}

void WidgetPrefs::on_fEventsPostChat_stateChanged(int state) {
	QListWidgetItem *item = fEventsList->item(fEventsList->currentRow());
	QString tmpKey = item->data(Qt::UserRole).toString();
	QSettings conf;
	conf.setValue(QString("events/%1/chat").arg(tmpKey), state==Qt::Checked);
}


void WidgetPrefs::on_fEventsSound_clicked() {
	QListWidgetItem *item = fEventsList->item(fEventsList->currentRow());
	QString tmpKey = item->data(Qt::UserRole).toString();
	QSettings conf;
	QString tmpPath = QFileDialog::getOpenFileName(this,
			tr("Select WAV Sound File"),
			   conf.value(QString("events/%1/sound").arg(tmpKey), QDir::homePath()).toString(),
			   "Sound files (*.*)" );
	if(tmpPath.isEmpty()) {
		conf.remove(QString("events/%1/sound").arg(tmpKey));
		fEventsSound->setText(tr("(none)"));
	} else {
		conf.setValue(QString("events/%1/sound").arg(tmpKey), tmpPath);
		fEventsSound->setText(tmpPath);
	}
}




