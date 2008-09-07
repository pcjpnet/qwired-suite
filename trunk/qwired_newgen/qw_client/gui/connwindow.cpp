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
	tmpCloseBtn->setAutoRaise(true);
	connect( tmpCloseBtn, SIGNAL(clicked()), this, SLOT(onCloseButton()) );
	pTabWidget->setCornerWidget(tmpCloseBtn);
}

// Close button pressed, close the current tab.
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
	if(tmpChat && tmpChat->pChatID!=0) {
		pTabWidget->setTabIcon(index, QIcon(":/icons/tab-idle.png"));
	}
}

void ConnWindow::on_actionAbout_triggered(bool ) {
	QMessageBox::about(this,
		tr("Qwired %1").arg(QWIRED_VERSION),
		   tr("<h2>Qwired %1</h2><br><br><b>A free Wired client for Linux, Windows and Mac OS X written in C++ using the Qt4 framework.</b><br><br>Qwired is available under the terms of the GPL (General Public License) Version 2 or later. Please see source code for more information.<br><br>Copyright (c) 2008 Bastian Bense.<br><br>Web: <a href=\"http://qwired-client.sourceforge.net/\">http://qwired-client.sourceforge.net/</a>").arg(QWIRED_VERSION) );
}

ConnWindow::~ConnWindow() { }

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






