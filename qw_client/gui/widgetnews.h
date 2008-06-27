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
#include "ui_WidgetNews.h"
#include "dialognewspost.h"
#include "wired/wiredsocket.h"

/**
	@author Bastian Bense <bastibense@gmail.com>
 */

class WidgetNews : public QWidget, private Ui::WidgetNews
{
	Q_OBJECT

	private:
		QColor pColorTitle;
		QColor pColorText;
		QPointer<WiredSocket> pSocket;
		
	public:
		WidgetNews( QWidget *parent = 0 );
		~WidgetNews();
		QPointer<DialogNewsPost> pWinPost;
		void setSocket(WiredSocket *socket);
		
	signals:
		void doRefreshNews();
		void doPostNews(QString);
		void onDeleteNews();
		
	public slots:
		void addNewsItem(QString theNick, QString theTime, QString thePost);
		void addFreshNewsItem(QString theNick, QString theTime, QString thePost);
		void onServerNewsDone();
		void doSendNews();

	private slots:
		void on_fBtnRefresh_clicked(bool checked);
		void on_fBtnPost_clicked(bool checked);
		void on_fBtnDelete_clicked(bool checked);
		void reloadPrefs();
		void initPrefs();


		// Socket slots
		void onNewsGroupListItem(const int nid, const QString name, const int count);
		void onNewsGroupListDone();

};

#endif