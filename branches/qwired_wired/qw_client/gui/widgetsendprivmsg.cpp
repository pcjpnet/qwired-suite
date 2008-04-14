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

 
#include "widgetsendprivmsg.h"
#include <QMessageBox>
#include <QKeyEvent>

WidgetSendPrivMsg::WidgetSendPrivMsg ( QWidget *parent )
		: QWidget ( parent )
{
// 	setAttribute(Qt::WA_DeleteOnClose);
	setupUi ( this );
	//fBtnSend->setShortcut(QKeySequence(Qt::Key_Enter));
	pTargetID = 0;
	fMsg->installEventFilter(this);
}


WidgetSendPrivMsg::~WidgetSendPrivMsg()
{
}

void WidgetSendPrivMsg::on_fInput_returnPressed() {
	emit newMessage(pTargetID, fInput->text());
	addText(fInput->text(),0);
	fInput->clear();
}



// void WidgetSendPrivMsg::setTarget ( ClassWiredSession *theSession, int theUserID )
// {
// 	pSession = theSession;
// 	if ( pSession and pSession->pWiredSocket ) {
// 		pUser = pSession->pWiredSocket->getUserByID ( theUserID );
// 	}
// 	//fTitle->setText ( tr("Private Message To: %1").arg (pUser.pNick) );
// }

// void WidgetSendPrivMsg::on_fBtnSend_clicked()
// {
// 	if( fBtnSend->text()==tr("Dismiss") ) {
// 		this->close();
// 	} else {
// 		if( pSession and pSession->pWiredSocket ) {
// 			pSession->pWiredSocket->sendPrivateMessage(pUser.pUserID, fMsg->toPlainText());
// 			this->close();
// 		}
// 	}
// }

// void WidgetSendPrivMsg::on_fBtnCancel_clicked()
// {
// 	if( fBtnCancel->text()==tr("Reply") ) {
// 		fMsg->setReadOnly(false);
// 		fMsg->clear();
// 		fBtnSend->setText(tr("Send"));
// 		fBtnCancel->setText(tr("Cancel"));
// 		fBtnCancel->setShortcut(QKeySequence(""));
// 	} else {
// 		this->close();
// 	}
// 	
// }

// bool WidgetSendPrivMsg::eventFilter(QObject * obj, QEvent *event)
// {
// 	if (obj == fMsg) {
// 		if (event->type() == QEvent::KeyPress) {
// 			QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
// 			if(keyEvent->key() == Qt::Key_Enter) {
// 				fBtnSend->click();
// 				return true;
// 			}
// 		}
// 	}
// 	return false;
// }

void WidgetSendPrivMsg::addText(QString theText, int theWho=0) {
	QColor tmpColor;
	if(theWho==0)
		 tmpColor = Qt::darkGray;
	else tmpColor = Qt::black;

// 	QString tmpHTML = QString("<div style='background-color: %1;'>%2</div><br>")
// 			.arg(tmpColor)
// 			.arg(theText);

	QTextCursor tc = fMsg->textCursor();
	tc.movePosition(QTextCursor::End);
	fMsg->setTextCursor(tc);

	fMsg->setTextColor(tmpColor);
	fMsg->append(theText);
// 	tc.insertHtml(tmpHTML);
}


