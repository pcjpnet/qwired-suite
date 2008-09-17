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

 
#include "connwindow.h"
#include "ui_WinMain.h"
#include "../general/wiredcommon.h"
#include "../general/classwiredsession.h"

ConnWindow::ConnWindow(QWidget *parent) : QMainWindow(parent) {
	setupUi(this);
	setUnifiedTitleAndToolBarOnMac(true);
	setAttribute(Qt::WA_DeleteOnClose);
	
	setWindowIcon( QIcon(":/icons/qwired_logo_32.png") );
	pTabWidget = new QTabWidget(this);
	pTabWidget->clear();
	pTabWidget->setVisible(false);
	connect(pTabWidget, SIGNAL(currentChanged(int)), this, SLOT(onTab_currentChanged(int)) );
	
	QToolButton *tmpCloseBtn = new QToolButton(this);
	tmpCloseBtn->setIcon( QIcon(":/icons/icn_close.png") );
	tmpCloseBtn->setShortcut(QKeySequence("ctrl+w"));
	tmpCloseBtn->setEnabled(false);
	connect( tmpCloseBtn, SIGNAL(clicked()), this, SLOT(onCloseButton()) );
	pTabWidget->setCornerWidget(tmpCloseBtn);
}


ConnWindow::~ConnWindow()
{ }

/// Close button pressed, close the current tab.
void ConnWindow::onCloseButton() {
	int tmpIdx = pTabWidget->currentIndex();
	if( tmpIdx>0 ) {
		QWidget *tmpWid = pTabWidget->currentWidget();
		tmpWid->close();
		pTabWidget->removeTab(tmpIdx);
	}
}

// Tab changed, let's see if we need to hide an icon.
void ConnWindow::onTab_currentChanged(int index) {
	
	QWidget *tmpBtn = pTabWidget->cornerWidget();
	tmpBtn->setEnabled( index>0 );
	
	// Icon removal for private chats
	QWidget *tmpWid = pTabWidget->widget(index);
	WidgetForum *tmpChat = qobject_cast<WidgetForum*>(tmpWid);
	if(tmpChat && tmpChat->pChatID!=1) {
		pTabWidget->setTabIcon(index, QIcon(":/icons/tab-idle.png"));
	}
}


/// Show the about box.
void ConnWindow::on_actionAbout_triggered(bool ) {
	if(!pWidgetAbout)
		pWidgetAbout = new WidgetAbout();
	pWidgetAbout->show();
}




/**
 * Toggle window visibility (implemented for TrayIcon usage)
 */
void ConnWindow::toggleVisible() {
	if(this->isMinimized()) {
		this->showNormal();
	} else {
		if(this->isVisible())
			this->hide();
		else 
			this->show();
	}
	
}






