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

 
#include "widgetconnect.h"
#include "../general/wiredcommon.h"

WidgetConnect::WidgetConnect(QWidget *parent) : QWidget(parent) {
	setupUi(this);
	fProgress->setVisible(false);
	loadBookmarks();

	// Notification manager
	WiredSingleton *tmpS = &WSINGLETON::Instance();
	connect( tmpS, SIGNAL(prefsChanged()), this, SLOT(loadBookmarks()) );
	
}


WidgetConnect::~WidgetConnect()
{ }

void WidgetConnect::on_btnConnect_clicked() {
	fProgress->setVisible(true);
	emit onConnnectReady(fAddress->text(), fLogin->text(), fPassword->text());
}

void WidgetConnect::setProgressBar(int value, int max) {
	fProgress->setVisible(value+max>0);
	fProgress->setMaximum(max);
	fProgress->setValue(value);
}

void WidgetConnect::loadBookmarks() {
	QSettings conf;
	QMenu *menu = fBtnFavourites->menu();
	if(!menu) {
		menu = new QMenu(fBtnFavourites);
		connect( menu, SIGNAL(triggered(QAction*)), this, SLOT(bookmarkSelected(QAction*)) );
		fBtnFavourites->setMenu(menu);
	}
	menu->clear();
	int tmpCnt = conf.beginReadArray("bookmarks");
	for(int i=0; i<tmpCnt; ++i) {
		conf.setArrayIndex(i);
		QStringList tmpData;
		tmpData << conf.value("address").toString() << conf.value("login").toString() << conf.value("password").toString();
		QAction *action = new QAction(menu);
		action->setText( conf.value("name").toString() );
		action->setIcon(QIcon(":/icons/icn_trackerserver.png"));
		action->setData(tmpData);
		menu->addAction(action);
	}
	conf.endArray();
}

void WidgetConnect::bookmarkSelected(QAction *action) {
	QStringList tmpList = action->data().value<QStringList>();
	fAddress->setText(tmpList.at(0));
	fLogin->setText(tmpList.at(1));
	fPassword->setText(tmpList.at(2));
	btnConnect->click();
}


