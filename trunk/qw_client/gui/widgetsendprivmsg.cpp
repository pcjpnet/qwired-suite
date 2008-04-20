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
	setupUi(this);
	fMsg->installEventFilter(this);
}


WidgetSendPrivMsg::~WidgetSendPrivMsg()
{
}

/**
 * The user has entered a new message and it should be sent to the target user.
 */
void WidgetSendPrivMsg::on_fInput_returnPressed() {
	if(!fUsers->currentRow() == -1) return;
	if(fInput->text().isEmpty()) return;
	int tmpId = fUsers->currentItem()->data(Qt::UserRole+1).toInt();

	qDebug() << "Sending new message to"<<tmpId;
	//emit newMessageEntered(tmpId, fInput->text());

	if(pSocket) pSocket->sendMessage(tmpId, fInput->text());
	addMessage(tmpId, fInput->text(), pSocket->sessionUser.pUserID);
	fInput->clear();
}

/**
 * A new message has arrived. Handle the message here.
 * @param user 
 * @param text 
 */
void WidgetSendPrivMsg::handleNewMessage(const ClassWiredUser & user, const QString & text) {
	qDebug() << "Handling new message from"<<user.pUserID;

	QTextDocument *doc = 0;
	if(!pDocuments.contains(user.pUserID))
		showUserMessages(user);

	int tmpId = fUsers->currentItem()->data(Qt::UserRole+1).toInt();
	addMessage(tmpId, text, tmpId);
	
	
}

/**
 * Reveal the existing user panel or create a new one.
 * @param user The user.
 */
void WidgetSendPrivMsg::showUserMessages(const ClassWiredUser & user) {
	for(int i=0; i<fUsers->count(); i++) {
		QListWidgetItem *item = fUsers->item(i);
		if(!item) continue;
		if(item->data(Qt::UserRole+1).toInt() == user.pUserID) {
			fUsers->setCurrentRow(i);
			return;
		}
	}
	
	// Not found, create a new entry
	QListWidgetItem *item = new QListWidgetItem();

	QTextDocument *tdoc = new QTextDocument(this);
	tdoc->clear();
	pDocuments[user.pUserID] = tdoc;
	
	item->setText(user.pNick);
	item->setIcon(user.iconAsPixmap());
	item->setData(Qt::UserRole+1, user.pUserID);

	fUsers->addItem(item);
	fUsers->setCurrentItem(item);
	
	
}

void WidgetSendPrivMsg::on_fUsers_currentRowChanged(int currentRow) {
	fMsg->setEnabled(currentRow>-1);
	fInput->setEnabled(currentRow>-1);
	
	if(currentRow==-1) {
		fMsg->clear();
		return;
	}
 	int tmpId = fUsers->currentItem()->data(Qt::UserRole+1).toInt();

	QTextDocument *tdoc = pDocuments[tmpId];
	if(tdoc) {
		fMsg->setDocument(tdoc);
	}
	
	//QTextCursor tc = fMsg->textCursor();
	//tc.movePosition(QTextCursor::End);
	//fMsg->setTextCursor(tc);
	//fMsg->ensureCursorVisible();
}

void WidgetSendPrivMsg::addMessage(const int uid, const QString & text, const int senderId) {
	QTextDocument *tdoc = pDocuments[uid];
	if(tdoc) {
		ClassWiredUser user = pSocket->getUserByID(senderId);

		QPalette tmpPal;
		QColor baseColor = tmpPal.color(QPalette::Midlight);
		//QColor baseColor = QColor(0xAA, 0xAA, 0xAA);
		
		QTextFrameFormat fformatHeader;
		fformatHeader.setBorder(1);
		fformatHeader.setPadding(2);
		fformatHeader.setMargin(4);
		fformatHeader.setTopMargin(6);
		fformatHeader.setBorderStyle(QTextFrameFormat::BorderStyle_Solid);
		fformatHeader.setBorderBrush(baseColor.darker());
		fformatHeader.setBackground(baseColor);

		QTextCursor tc(pDocuments[uid]);
		QTextFrame *topFrame = tc.currentFrame();
		
		//tc.movePosition(QTextCursor::End);

		tc.setPosition(topFrame->lastPosition());
		if(tc.position()==0)
			tc.setPosition(topFrame->firstPosition());
		qDebug() << "Pos:"<<tc.position();
		// Header

		
		QTextFrame *headerFrame = tc.insertFrame(fformatHeader);
		
		QTextCharFormat charFormat = tc.charFormat();
		charFormat.setFontWeight(QFont::Bold);
		tc.insertText(QString("%1").arg(user.pNick), charFormat);
		charFormat.setFontWeight(QFont::Normal);
		charFormat.setFontPointSize(10);
		tc.insertText(QString(" (%1)").arg(QDateTime::currentDateTime().toString()), charFormat);
		
		tc.setPosition(topFrame->lastPosition());

		QTextBlockFormat tbf;
		tbf.setLeftMargin(4);
		tbf.setRightMargin(4);
		tbf.setTopMargin(4);
		tbf.setBottomMargin(8);
		tc.setBlockFormat(tbf);
		tc.insertText(QString("%1").arg(text.trimmed()));

		int tmpId = fUsers->currentItem()->data(Qt::UserRole+1).toInt();
		if(tmpId == uid) {
			fMsg->setTextCursor(tc);
			fMsg->ensureCursorVisible();
		}
		
	}
}

void WidgetSendPrivMsg::setSocket(WiredSocket * socket) {
	pSocket = socket;
	connect(socket, SIGNAL(onServerUserChanged(ClassWiredUser, ClassWiredUser)),
			this, SLOT(handleUserChanged(ClassWiredUser, ClassWiredUser)) );
	connect(socket, SIGNAL(onServerUserLeft(int,ClassWiredUser)),
			this, SLOT(handleUserLeft(int,ClassWiredUser)) );

}

/**
 * A user changed. Reflect the changes in the user list respectively.
 */
void WidgetSendPrivMsg::handleUserChanged(const ClassWiredUser oldUser, const ClassWiredUser newUser) {
	if(!pDocuments.contains(newUser.pUserID))
		return;

	for(int i=0; i<fUsers->count(); i++) {
		QListWidgetItem *item = fUsers->item(i);
		if(!item) continue;
		if(item->data(Qt::UserRole+1).toInt() == newUser.pUserID) {
			item->setIcon(newUser.iconAsPixmap() );
			item->setText(newUser.pNick);
			return;
		}
	}

}

void WidgetSendPrivMsg::handleUserLeft(int theChatID, const ClassWiredUser theUser) {
	if(!pDocuments.contains(theUser.pUserID))
		return;

	for(int i=0; i<fUsers->count(); i++) {
		QListWidgetItem *item = fUsers->item(i);
		if(!item) continue;
		if(item->data(Qt::UserRole+1).toInt() == theUser.pUserID) {
			item->setText(QString("%1 (%2)").arg(item->text()).arg(tr("disconnected")));
			return;
		}
	}
}


