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
	QSettings settings;
	
	setupUi(this);
	fInput->installEventFilter(this);

	fSplitterV->setStretchFactor(0,5);
	fSplitterV->setStretchFactor(1,10);

	fSplitterV->restoreState(settings.value("gui/messages/splitterV").toByteArray());
	fSplitterH->restoreState(settings.value("gui/messages/splitterH").toByteArray());
}


WidgetSendPrivMsg::~WidgetSendPrivMsg()
{
	QSettings settings;
	settings.setValue("gui/messages/splitterV", fSplitterV->saveState());
	settings.setValue("gui/messages/splitterH", fSplitterH->saveState());
}

/**
 * The event filter for the input field.
 */
bool WidgetSendPrivMsg::eventFilter(QObject * object, QEvent * event) {
	if(object == fInput && QEvent::KeyPress) {
		QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
		if(keyEvent->key()==Qt::Key_Enter || keyEvent->key()==Qt::Key_Return){
			on_fInput_returnPressed();
			
			keyEvent->accept();
			return true;
		}
	}
	return false;
}
				
/**
 * The user has entered a new message and it should be sent to the target user.
 */
void WidgetSendPrivMsg::on_fInput_returnPressed() {
	if(!fUsers->currentRow() == -1) return;
	if(fInput->toPlainText().isEmpty()) return;
	int tmpId = fUsers->currentItem()->data(Qt::UserRole+1).toInt();

	qDebug() << "Sending new message to"<<tmpId;
	//emit newMessageEntered(tmpId, fInput->text());

	if(pSocket) pSocket->sendMessage(tmpId, fInput->toPlainText());
	addMessage(tmpId, fInput->toPlainText(), pSocket->sessionUser.pUserID, 0);
	fInput->clear();
}

/**
 * A new message has arrived. Handle the message here.
 */
void WidgetSendPrivMsg::handleNewMessage(const ClassWiredUser & user, const QString & text) {
	if(!pDocuments.contains(user.pUserID))
		initUserMessages(user);
	addMessage(user.pUserID, text, user.pUserID, 0);
}

/**
 * Reveal the existing user panel or create a new one.
 * @param user The user.
 */
void WidgetSendPrivMsg::initUserMessages(const ClassWiredUser & user) {
	if(pDocuments.contains(user.pUserID))
		return;
	
	// Not found, create a new entry
	QListWidgetItem *item = new QListWidgetItem();
	QTextDocument *tdoc = new QTextDocument(this);
	pDocuments[user.pUserID] = tdoc;
	item->setText(user.pNick);
	item->setIcon(user.iconAsPixmap());
	item->setData(Qt::UserRole+1, user.pUserID);
	item->setData(Qt::UserRole+2, 0); // lock flag
	fUsers->addItem(item);
}

void WidgetSendPrivMsg::on_fUsers_currentRowChanged(int currentRow) {
	fMsg->setEnabled(currentRow>-1);
	fInput->setEnabled(currentRow>-1);
	fBtnDelete->setEnabled(currentRow>-1);
	fBtnSave->setEnabled(currentRow>-1);
	
	if(currentRow==-1) { fMsg->clear();	return; }

 	int tmpId = fUsers->currentItem()->data(Qt::UserRole+1).toInt();
	int tmpLockFlag = fUsers->currentItem()->data(Qt::UserRole+2).toInt();
	QTextDocument *tdoc = pDocuments[tmpId];
	if(tdoc) {
		fMsg->setDocument(tdoc);
		fMsg->ensureCursorVisible();
		fInput->setEnabled(!tmpLockFlag);

		QTextCursor tc(fMsg->document());
		tc.movePosition(QTextCursor::End);
 		fMsg->setTextCursor(tc);
		fMsg->ensureCursorVisible();
		
		// Fix unread marker
		QFont font = fUsers->currentItem()->font();
		font.setBold(false);
		fUsers->currentItem()->setFont(font);

	}
	

}

void WidgetSendPrivMsg::showUserMessages(const int uid) {
	for(int i=0; i<fUsers->count(); i++) {
		if(fUsers->item(i)->data(Qt::UserRole+1).toInt() == uid) {
			fUsers->setCurrentRow(i);
			return;
		}
	}
}


void WidgetSendPrivMsg::addMessage(const int uid, const QString & text, const int senderId, const int msgType=0) {
	QTextDocument *tdoc = pDocuments[uid];
	if(tdoc) {
		ClassWiredUser user = pSocket->getUserByID(senderId);

		QPalette tmpPal;
		QColor baseColor = tmpPal.color(QPalette::Midlight);
		
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

		// Header
		if(msgType==0) {
			// Normal
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

		} else if(msgType==1) {
			// Status
			tc.insertBlock();
			
			QTextCharFormat charFormat = tc.charFormat();
			charFormat.setFontWeight(QFont::Bold);
			charFormat.setFontPointSize(10);
			charFormat.setForeground(Qt::gray);

			QTextBlockFormat tbf;
			tbf.setTopMargin(4);
			tbf.setAlignment(Qt::AlignHCenter);
			tc.setBlockFormat(tbf);
			tc.insertText(text, charFormat);

		}

		// Update the text browser
		if(fUsers->currentItem()) {
			int tmpId = fUsers->currentItem()->data(Qt::UserRole+1).toInt();
			if(tmpId == uid) { // Update the text browser
				fMsg->setTextCursor(tc);
				fMsg->ensureCursorVisible();
				qDebug() << "updating browser";
				return;
			}
		}

		// Update the font instead
		for(int i=0; i<fUsers->count(); i++) {
			QListWidgetItem *item = fUsers->item(i);
			if(!item) continue;
			if(item->data(Qt::UserRole+1).toInt() == uid) {
				QFont font = item->font();
				font.setBold(true);
				item->setFont(font);
				return;
		} }
		
		
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
			item->setData(Qt::UserRole+2, 1);
			addMessage(theUser.pUserID, tr("<<< %1 has disconnected >>>").arg(theUser.pNick), 0, 1);

			int tmpId = fUsers->currentItem()->data(Qt::UserRole+1).toInt();
			if(tmpId == theUser.pUserID)
				fInput->setEnabled(!fUsers->currentItem()->data(Qt::UserRole+2).toInt());
			
			
			return;
		}
	}
}

void WidgetSendPrivMsg::setInputFocus() {
	fInput->setFocus();
}

void WidgetSendPrivMsg::on_fBtnDelete_clicked(bool) {
	if(fUsers->currentItem()) {
		QListWidgetItem *item = fUsers->takeItem(fUsers->currentRow());
		pDocuments.remove(item->data(Qt::UserRole+1).toInt());
		delete item;
		fUsers->setCurrentRow(-1);
	}
}

void WidgetSendPrivMsg::on_fBtnSave_clicked(bool ) {
	QListWidgetItem *item = fUsers->currentItem();
	if(!item) return;
	QString tmpFile = QFileDialog::getSaveFileName(this, tr("Save Message History"), QDir::homePath(), "HTML (*.html)");
	if(!tmpFile.isEmpty()) {
		QFile file(tmpFile);
		if(file.open(QIODevice::WriteOnly)) {
			file.write(pDocuments[item->data(Qt::UserRole+1).toInt()]->toHtml("utf-8").toUtf8());
			file.close();
		}
	}


}



