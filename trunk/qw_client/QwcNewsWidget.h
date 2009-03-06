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

 
#ifndef WIDGETNEWS_H
#define WIDGETNEWS_H

#include <QWidget>
#include <QPointer>
#include "ui_QwcNewsWidget.h"
#include "QwcNewsPostWidget.h"

/**
	@author Bastian Bense <bastibense@gmail.com>
 */

class QwcNewsWidget : public QWidget, private Ui_QwcNewsWidget
{
	Q_OBJECT

	private:
		QColor pColorTitle;
		QColor pColorText;
                int newsCounter;
		
	public:
        QwcNewsWidget( QWidget *parent = 0 );
        ~QwcNewsWidget();
        QPointer<QwcNewsPostWidget> pWinPost;
                int newsCount();
		
	signals:
		void doRefreshNews();
		void doPostNews(QString);
		void onDeleteNews();
		
	public slots:
		void addNewsItem(QString theNick, QString theTime, QString thePost);
		void addFreshNewsItem(QString theNick, QString theTime, QString thePost);
		void onServerNewsDone();
		void doSendNews();
                void dontSendNews();
                void setDisabledPostButton(bool b);
                void setDisabledClearButton(bool b);
                void clearTextArea();

	private slots:
		void on_fBtnRefresh_clicked(bool checked);
		void on_fBtnPost_clicked(bool checked);
		void on_fBtnDelete_clicked(bool checked);
		void reloadPreferences();
		void initPrefs();


};

#endif
