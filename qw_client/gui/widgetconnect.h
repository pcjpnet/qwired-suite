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

 
#ifndef WIDGETCONNECT_H
#define WIDGETCONNECT_H

#include <QtCore>
#include <QtGui>
#include "ui_WidgetConnect.h"
#include "../general/wiredcommon.h"


/**
	@author Bastian Bense <bastibense@gmail.com>
 */
class WidgetConnect : public QWidget, public Ui::WidgetConnect
{
	
Q_OBJECT

private:
	int pReconnectTimerId;

		
public:
	
    WidgetConnect(QWidget *parent = 0)
	{
		Q_UNUSED(parent)
		setupUi(this);
		fContainer->setCurrentIndex(0);
		loadBookmarks();
		pReconnectTimerId = -1;

		// Notification manager
		WiredSingleton *tmpS = &WSINGLETON::Instance();
		connect( tmpS, SIGNAL(prefsChanged()), this, SLOT(loadBookmarks()) );
	};

	
    ~WidgetConnect()
	{ };


private slots:
	
    void on_btnConnect_clicked()
	{
		fContainer->setCurrentIndex(1);
		fStatus->setText(tr("Connecting..."));
		emit onConnnectReady(fAddress->text(), fLogin->text(), fPassword->text());
	};

	
	void on_fBtnCancel_clicked()
	{
		emit onConnectAborted();
		resetForm();
	}

	
	void bookmarkSelected(QAction *action)
	{
		QStringList tmpList = action->data().value<QStringList>();
		fAddress->setText(tmpList.at(0));
		fLogin->setText(tmpList.at(1));
		fPassword->setText(tmpList.at(2));
		btnConnect->click();
	};

	
	void loadBookmarks()
	{
		QSettings conf;
		QMenu *menu = fBtnFavourites->menu();
		if(!menu) {
			menu = new QMenu(fBtnFavourites);
			connect( menu, SIGNAL(triggered(QAction*)),
					 this, SLOT(bookmarkSelected(QAction*)) );
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

		
	};


protected:
	/// The reconnect timer will attempt to reconnect to the server.
	void timerEvent(QTimerEvent *)
	{
		qDebug() << this <<": Attempting to auto-reconnect.";
		killTimer(pReconnectTimerId);
		pReconnectTimerId = -1;
		on_btnConnect_clicked(); 
	}
	

public slots:

	void stopReconnecting()
	{
		fContainer->setCurrentIndex(0);
		if(pReconnectTimerId>-1)
			this->killTimer(pReconnectTimerId);
		
	}

	void startReconnecting()
	{
		fContainer->setCurrentIndex(1);
		fStatus->setText(tr("Waiting for reconnect..."));
		
		qDebug() << this << ": Starting reconnect timer.";
		pReconnectTimerId = this->startTimer(5*1000);
		fProgress->setMaximum(0);
	}
	
	void resetForm()
	{
		fContainer->setCurrentIndex(0);
		//fProgress->setVisible(false);
	};
	
    void setProgressBar(int value, int max)
	{
		//fProgress->setVisible(value+max>0);
		fProgress->setMaximum(max);
		fProgress->setValue(value);
	};


	/// Set the text of the status bar text label.
	void setStatus(const QString &status)
	{
		fStatus->setText(status);
	}

	
signals:
	/// The user clicked 'connect' and is ready to wait for a connection.
    void onConnnectReady(QString theHost, QString theLogin, QString thePassword);
	/// The user clicked the cancel button.
	void onConnectAborted();
    
};

#endif
