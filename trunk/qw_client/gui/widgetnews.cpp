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


#include "../general/wiredcommon.h"
#include "widgetnews.h"
#include "dialognewspost.h"
#include <QtCore>

WidgetNews::WidgetNews(QWidget *parent)
 : QWidget(parent)
{
	setAttribute(Qt::WA_DeleteOnClose);
	setupUi(this);

	// Notification manager
	WiredSingleton *tmpS = &WSINGLETON::Instance();
	connect( tmpS, SIGNAL(prefsChanged()), this, SLOT(reloadPrefs()) );
	initPrefs();
}


WidgetNews::~WidgetNews()
{
}

/**
 * Set the responsible WiredSocket and connect the signals as needed.
 */
void WidgetNews::setSocket(WiredSocket * socket) {
	pSocket = socket;
	pSocket->getNewsGroups();
	connect(pSocket, SIGNAL(onNewsGroupListDone()),
			this, SLOT(onNewsGroupListDone()));
	connect(pSocket, SIGNAL(onNewsGroupListItem(const int, const QString, const int)),
			this, SLOT(onNewsGroupListItem(const int, const QString, const int)));
}

void WidgetNews::onNewsGroupListItem(const int nid, const QString name, const int count)
{
	QTreeWidgetItem *item = new QTreeWidgetItem(listGroups);
	item->setText(0, name);
	item->setText(1, QString::number(count));
	item->setData(0, Qt::UserRole, nid);
}

void WidgetNews::onNewsGroupListDone()
{
	
}




void WidgetNews::reloadPrefs() {
	initPrefs();
	fNews->clear();
	emit doRefreshNews();
}

void WidgetNews::initPrefs() {
	QPalette tmpP = fNews->palette();
	tmpP.setColor(QPalette::Base, WiredSingleton::colorFromPrefs("interface/news/back/color", Qt::white));
	fNews->setPalette(tmpP);
	pColorText =  WiredSingleton::colorFromPrefs("interface/news/text/color", Qt::black);
	pColorTitle = WiredSingleton::colorFromPrefs("interface/news/titles/color", Qt::gray);
	
	
// 	fBtnIfNewsText->setColor( getSafeColor("interface/news/text/color", Qt::black) );
// 	fBtnIfNewsBack->setColor( getSafeColor("interface/news/back/color", Qt::white) );
// 	fBtnIfNewsTitles->setColor( getSafeColor("interface/news/titles/color", Qt::gray) );
// 	f.fromString( conf.value("interface/news/font", WiredSingleton::systemMonospaceFont().append(",11") ).toString() );
// 	fIfNewsFont->setText( QString("%1, %2pt").arg(f.family()).arg(f.pointSize()) );
}



void WidgetNews::addNewsItem(QString theNick, QString theTime, QString thePost)
{
	fNews->setFontWeight(QFont::Bold);
	fNews->setTextColor(pColorTitle);
	fNews->textCursor().insertText(tr("From %1 (%2):\n").arg(theNick).arg(theTime));
	fNews->setFontWeight(QFont::Normal);
	fNews->setTextColor(pColorText);
	fNews->textCursor().insertText(tr("%1\n\n").arg(thePost));
}

void WidgetNews::addFreshNewsItem(QString theNick, QString theTime, QString thePost)
{
	QTextCursor tc = fNews->textCursor();
	tc.setPosition(0);
	fNews->setTextCursor(tc);
	
	fNews->setFontWeight(QFont::Bold);
	fNews->setTextColor(pColorTitle);
	fNews->textCursor().insertText(tr("From %1 (%2):\n").arg(theNick).arg(theTime));
	fNews->setFontWeight(QFont::Normal);
	fNews->setTextColor(pColorText);
	fNews->textCursor().insertText(tr("%1\n\n").arg(thePost));
}

void WidgetNews::doSendNews() {
	// Send the news text from the post window to the user.
	if( pWinPost!=0 ) {
		emit doPostNews(pWinPost->getPostText());
	}
}


void WidgetNews::on_fBtnRefresh_clicked(bool checked)
{
	Q_UNUSED(checked)
	fNews->clear();
	fProc->setVisible(true);
	emit doRefreshNews();
}

void WidgetNews::on_fBtnPost_clicked(bool checked)
{
	Q_UNUSED(checked)
	if( pWinPost==0 ) {
		pWinPost = new DialogNewsPost(this);
		connect( pWinPost, SIGNAL(accepted()), this, SLOT(doSendNews()) );
		pWinPost->show();
	} else {
		pWinPost->raise();
	
	}
}

void WidgetNews::on_fBtnDelete_clicked(bool) {
	fNews->clear();
	emit onDeleteNews();
}

void WidgetNews::onServerNewsDone()
{
	fProc->setVisible(false);
}





