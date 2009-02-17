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
#include "WidgetNewsPost.h"
#include <QtCore>

WidgetNews::WidgetNews(QWidget *parent)
 : QWidget(parent)
{
	setAttribute(Qt::WA_DeleteOnClose);
	setupUi(this);

	// Notification manager
	WiredSingleton *tmpS = &WSINGLETON::Instance();
	connect( tmpS, SIGNAL(prefsChanged()), this, SLOT(reloadPreferences()) );
	initPrefs();
}


WidgetNews::~WidgetNews()
{
}

void WidgetNews::reloadPreferences() {
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
    QString timeString;
    QSettings conf;

    timeString.append(theTime.left(10));
    timeString = tr("%1/%2/%3 at %4 GTM %5").arg(timeString.mid(8,2), timeString.mid(5,2), timeString.left(4),theTime.mid(11,8), theTime.right(6));

    fNews->setFontFamily(conf.value("interface/news/font", WiredSingleton::systemMonospaceFont().append(",11") ).toString());
    fNews->setFontWeight(QFont::Bold);
    fNews->setTextColor(pColorTitle);
    fNews->textCursor().insertText(tr("From %1 (%2):\n").arg(theNick).arg(timeString));
    fNews->setFontWeight(QFont::Normal);
    fNews->setTextColor(pColorText);
    fNews->textCursor().insertText(tr("%1\n\n").arg(thePost));
}

void WidgetNews::addFreshNewsItem(QString theNick, QString theTime, QString thePost)
{
        QString timeString;
        QSettings conf;

        timeString.append(theTime.left(10));
        timeString = tr("%1/%2/%3 at %4 GTM %5").arg(timeString.mid(8,2), timeString.mid(5,2), timeString.left(4),theTime.mid(11,8), theTime.right(6));

	QTextCursor tc = fNews->textCursor();
	tc.setPosition(0);
	fNews->setTextCursor(tc);

        fNews->setFontFamily(conf.value("interface/news/font", WiredSingleton::systemMonospaceFont().append(",11") ).toString());
        fNews->setFontWeight(QFont::Bold);
	fNews->setTextColor(pColorTitle);
        fNews->textCursor().insertText(tr("From %1 (%2):\n").arg(theNick).arg(timeString));
	fNews->setFontWeight(QFont::Normal);
	fNews->setTextColor(pColorText);
	fNews->textCursor().insertText(tr("%1\n\n").arg(thePost));
}

void WidgetNews::doSendNews() {
	// Send the news text from the post window to the user.
	if( pWinPost!=0 ) {
                emit doPostNews(pWinPost->getPostText());
                pWinPost->close();
                pWinPost->deleteLater();
	}
}

void WidgetNews::dontSendNews() {
    if( pWinPost!=0 ) {
                pWinPost->close();
                pWinPost->deleteLater();
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
		pWinPost = new WidgetNewsPost(this);
		connect( pWinPost, SIGNAL(accepted()), this, SLOT(doSendNews()) );
                connect( pWinPost, SIGNAL(rejected()), this, SLOT(dontSendNews()));
		pWinPost->show();
	} else {
		pWinPost->raise();
	
	}
}

void WidgetNews::on_fBtnDelete_clicked(bool) {
        emit onDeleteNews();
}

void WidgetNews::clearTextArea() {
    fNews->clear();
}

void WidgetNews::onServerNewsDone()
{
	fProc->setVisible(false);
}


void WidgetNews::setDisabledPostButton(bool b)
{
    fBtnPost->setDisabled(b);
}

void WidgetNews::setDisabledClearButton(bool b)
{
    fBtnDelete->setDisabled(b);
}


