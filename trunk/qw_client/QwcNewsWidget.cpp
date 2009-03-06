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


#include "QwcGlobals.h"
#include "QwcNewsWidget.h"
#include "QwcNewsPostWidget.h"
#include <QtCore>
#include <QMessageBox>

QwcNewsWidget::QwcNewsWidget(QWidget *parent)
 : QWidget(parent)
{
	setAttribute(Qt::WA_DeleteOnClose);
	setupUi(this);

        newsCounter = 0;

	// Notification manager
    QwcSingleton *tmpS = &WSINGLETON::Instance();
	connect( tmpS, SIGNAL(prefsChanged()), this, SLOT(reloadPreferences()) );
	initPrefs();
}


QwcNewsWidget::~QwcNewsWidget()
{
}

void QwcNewsWidget::reloadPreferences() {
	initPrefs();
	fNews->clear();
        newsCounter = 0;
	emit doRefreshNews();
}

void QwcNewsWidget::initPrefs() {
	QPalette tmpP = fNews->palette();
    tmpP.setColor(QPalette::Base, QwcSingleton::colorFromPrefs("interface/news/back/color", Qt::white));
	fNews->setPalette(tmpP);
    pColorText =  QwcSingleton::colorFromPrefs("interface/news/text/color", Qt::black);
    pColorTitle = QwcSingleton::colorFromPrefs("interface/news/titles/color", Qt::gray);
	
	
// 	fBtnIfNewsText->setColor( getSafeColor("interface/news/text/color", Qt::black) );
// 	fBtnIfNewsBack->setColor( getSafeColor("interface/news/back/color", Qt::white) );
// 	fBtnIfNewsTitles->setColor( getSafeColor("interface/news/titles/color", Qt::gray) );
// 	f.fromString( conf.value("interface/news/font", QwcSingleton::systemMonospaceFont().append(",11") ).toString() );
// 	fIfNewsFont->setText( QString("%1, %2pt").arg(f.family()).arg(f.pointSize()) );
}



void QwcNewsWidget::addNewsItem(QString theNick, QString theTime, QString thePost)
{
    QString timeString;
    QSettings conf;

    timeString.append(theTime.left(10));
    timeString = tr("%1/%2/%3 at %4 GTM %5").arg(timeString.mid(8,2), timeString.mid(5,2), timeString.left(4),theTime.mid(11,8), theTime.right(6));

    fNews->setFontFamily(conf.value("interface/news/font", QwcSingleton::systemMonospaceFont().append(",11") ).toString());
    fNews->setFontWeight(QFont::Bold);
    fNews->setTextColor(pColorTitle);
    fNews->textCursor().insertText(tr("From %1 (%2):\n").arg(theNick).arg(timeString));
    fNews->setFontWeight(QFont::Normal);
    fNews->setTextColor(pColorText);
    fNews->textCursor().insertText(tr("%1\n\n").arg(thePost));

    newsCounter++;
}

void QwcNewsWidget::addFreshNewsItem(QString theNick, QString theTime, QString thePost)
{
    QString timeString;
    QSettings conf;

    timeString.append(theTime.left(10));
    timeString = tr("%1/%2/%3 at %4 GTM %5").arg(timeString.mid(8,2), timeString.mid(5,2), timeString.left(4),theTime.mid(11,8), theTime.right(6));

    QTextCursor tc = fNews->textCursor();
    tc.setPosition(0);
    fNews->setTextCursor(tc);

    fNews->setFontFamily(conf.value("interface/news/font", QwcSingleton::systemMonospaceFont().append(",11") ).toString());
    fNews->setFontWeight(QFont::Bold);
    fNews->setTextColor(pColorTitle);
    fNews->textCursor().insertText(tr("From %1 (%2):\n").arg(theNick).arg(timeString));
    fNews->setFontWeight(QFont::Normal);
    fNews->setTextColor(pColorText);
    fNews->textCursor().insertText(tr("%1\n\n").arg(thePost));

    newsCounter++;
}

void QwcNewsWidget::doSendNews() {
	// Send the news text from the post window to the user.
	if( pWinPost!=0 ) {
                emit doPostNews(pWinPost->getPostText());
                pWinPost->close();
                pWinPost->deleteLater();
	}
}

void QwcNewsWidget::dontSendNews() {
    if( pWinPost!=0 ) {
                pWinPost->close();
                pWinPost->deleteLater();
        }
}


void QwcNewsWidget::on_fBtnRefresh_clicked(bool checked)
{
	Q_UNUSED(checked)
	fNews->clear();
	fProc->setVisible(true);
        newsCounter = 0;
	emit doRefreshNews();
}

void QwcNewsWidget::on_fBtnPost_clicked(bool checked)
{
	Q_UNUSED(checked)
	if( pWinPost==0 ) {
        pWinPost = new QwcNewsPostWidget(this);
		connect( pWinPost, SIGNAL(accepted()), this, SLOT(doSendNews()) );
                connect( pWinPost, SIGNAL(rejected()), this, SLOT(dontSendNews()));
		pWinPost->show();
	} else {
		pWinPost->raise();
	
	}
}

void QwcNewsWidget::on_fBtnDelete_clicked(bool) {
        if(QMessageBox::question (this, tr("Clear news"), tr("Are you sure you want to clear all news items?\n\nThis cannot be undone."), QMessageBox::Yes, QMessageBox::No ) == QMessageBox::Yes)
        {
            newsCounter = 0;
            emit onDeleteNews();
        }
}

void QwcNewsWidget::clearTextArea() {
    fNews->clear();
}

void QwcNewsWidget::onServerNewsDone()
{
	fProc->setVisible(false);
}


void QwcNewsWidget::setDisabledPostButton(bool b)
{
    fBtnPost->setDisabled(b);
}

void QwcNewsWidget::setDisabledClearButton(bool b)
{
    fBtnDelete->setDisabled(b);
}

int QwcNewsWidget::newsCount()
{
    return newsCounter;
}


