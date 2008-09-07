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

 
#include <QInputDialog>
#include "../general/classwiredsession.h"
#include "widgetforum.h"
#include "../general/wiredcommon.h"
#include "widgetsendprivmsg.h"

WidgetForum::WidgetForum ( QWidget *parent ) : QWidget ( parent ) {
	QSettings settings;
	
	setAttribute(Qt::WA_DeleteOnClose);
	setupUi(this);
	fBtnInvite->setVisible(false);
	pChatID = 0;

	fChatInput->installEventFilter(this);
	fSplitterV->setStretchFactor(0,20);
	fSplitterV->setStretchFactor(1,1);

	fSplitterH->setStretchFactor(0,10);
	fSplitterH->setStretchFactor(1,2);
	
	fSplitterV->restoreState(settings.value("gui/chat/splitterV").toByteArray());
	fSplitterH->restoreState(settings.value("gui/chat/splitterH").toByteArray());
	
	// Notification manager
	WiredSingleton *tmpS = &WSINGLETON::Instance();
	connect( tmpS, SIGNAL(prefsChanged()), this, SLOT(reloadPrefs()) );
	reloadPrefs();
}

WidgetForum::~WidgetForum() {
	// Leave the chat, if it is private
	if( pChatID!=0 ) {
		pSession->pChats.remove(pChatID);
		pSession->pWiredSocket->leaveChat(pChatID);
	} else {
		QSettings settings;
		settings.setValue("gui/chat/splitterV", fSplitterV->saveState());
		settings.setValue("gui/chat/splitterH", fSplitterH->saveState());
	}
}


/**
 * The event filter for the input field.
 */
bool WidgetForum::eventFilter(QObject * object, QEvent * event) {
	if(object == fChatInput && QEvent::KeyPress) {
		QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
		if((keyEvent->key()==Qt::Key_Enter || keyEvent->key()==Qt::Key_Return) && !(keyEvent->modifiers() & Qt::ShiftModifier) ){
			on_fChatInput_returnPressed();
			keyEvent->accept();
			return true;
		}
	}
	return false;
}

/**
 * Write a event to the chat
 * @param theMsg 
 */
void WidgetForum::writeEventToChat(QString theMsg) {
	QTextCursor tc = fChatLog->textCursor();
	tc.movePosition(QTextCursor::End);
	
	QTextCharFormat f;
	f.setFont(pChatFont);
	f.setForeground(pChatTimeColor);
	tc.setCharFormat(f);

	QString tmpTimestamp;
	if(pChatShowTime) {
		f.setForeground(pChatTimeColor);
		tc.setCharFormat(f);
		tmpTimestamp = QTime::currentTime().toString().append(" ");
		tc.insertText(tmpTimestamp);
	}

	f.setForeground(pChatEventColor);
	tc.setCharFormat(f);
	
	tc.insertText(QString("<<< %1 >>>\n").arg(theMsg));

	tc.movePosition(QTextCursor::End);
	fChatLog->ensureCursorVisible();
}


/**
 * Write some chat text to the chat log view.
 * @param theUser The name of the user.
 * @param theText The text of the chat message.
 * @param theEmote True if the user performed an action.
 */
void WidgetForum::writeToChat(QString theUser, QString theText, bool theEmote) {
	
	QTextCursor tc = fChatLog->textCursor();
	tc.movePosition(QTextCursor::End);

	QTextCharFormat f;
	f.setFont(pChatFont);
	f.setForeground(pChatTextColor);
	tc.setCharFormat(f);

	int tmpStart = tc.position();
	QString tmpData;

	QString tmpTimestamp;
	if(pChatShowTime) {
		f.setForeground(pChatTimeColor);
		tc.setCharFormat(f);
		tmpTimestamp = QTime::currentTime().toString().append(" ");
		tc.insertText(tmpTimestamp);
	}

	f.setForeground(pChatTextColor);
	tc.setCharFormat(f);

	const int tmpNameLength = 20;
	if(pChatStyle==0) { // Qwired style
		if(theEmote)
			 tmpData = QString("%1 %2\n").arg(QString("*** %1").arg(theUser), tmpNameLength-tmpTimestamp.size()).arg(theText);
		else tmpData = QString("%1: %2\n").arg(theUser.left(tmpNameLength-tmpTimestamp.count() ), tmpNameLength-tmpTimestamp.size()).arg(theText);
	} else if(pChatStyle==1) { // IRC style
		if(theEmote)
			 tmpData = QString("*** %1 %2\n").arg(theUser).arg(theText);
		else tmpData = QString("<%1> %2\n").arg(theUser).arg(theText);
	}
	tc.insertText(tmpData);
	tc.setPosition(tmpStart);

	if(pEmoticonsEnabled) {
		QHash<QString,QString> pEmotes;
		pEmotes[":):)"] = ":/icons/emotes/face-smile-big.png";
		pEmotes[":D"] = ":/icons/emotes/face-grin.png";
		pEmotes[":-D"] = ":/icons/emotes/face-grin.png";
		pEmotes[":O"] = ":/icons/emotes/face-surprise.png";
		pEmotes[";)"] = ":/icons/emotes/face-wink.png";
		pEmotes[";-)"] = ":/icons/emotes/face-wink.png";
		pEmotes[":)"] = ":/icons/emotes/face-smile.png";
		pEmotes[":-)"] = ":/icons/emotes/face-smile.png";
		pEmotes["^^"] = ":/icons/emotes/face-smile.png";
		pEmotes["^.^"] = ":/icons/emotes/face-cat.png";
		pEmotes[":>"] = ":/icons/emotes/face-smile.png";
		pEmotes[":("] = ":/icons/emotes/face-sad.png";
		pEmotes[":-("] = ":/icons/emotes/face-sad.png";
		pEmotes[";("] = ":/icons/emotes/face-crying.png";
		pEmotes[";-("] = ":/icons/emotes/face-crying.png";
		pEmotes[":*"] = ":/icons/emotes/face-kiss.png";
		pEmotes[":-*"] = ":/icons/emotes/face-kiss.png";
		pEmotes["(angel)"] = ":/icons/emotes/face-angel.png";
		pEmotes["(devil)"] = ":/icons/emotes/face-devilish.png";
		pEmotes["(nerd)"] = ":/icons/emotes/face-glasses.png";
		pEmotes["(monkey)"] = ":/icons/emotes/face-monkey.png";
		pEmotes[":|"] = ":/icons/emotes/face-plain.png";
		pEmotes[":-|"] = ":/icons/emotes/face-plain.png";
		pEmotes[":/"] = ":/icons/emotes/face-plain.png";
		pEmotes[":-/"] = ":/icons/emotes/face-plain.png";
		pEmotes[":-\\"] = ":/icons/emotes/face-plain.png";
		pEmotes[":\\"] = ":/icons/emotes/face-plain.png";
		pEmotes["(meow)"] = ":/icons/emotes/face-cat.png";

		
		// Smileys
		QHashIterator<QString,QString> i(pEmotes);
		while(i.hasNext()) {
			i.next();
			while(true) {
				QString tmpSearch = QString(" ").append(i.key());
				QTextCursor tmpTC = fChatLog->document()->find(tmpSearch, tc);
				if(tmpTC.isNull()) break;
				tmpTC.insertText(" ");
				tmpTC.insertImage( i.value() );
			}
		}
	}

	// Hyperlinks
	QStringList tmpProtos;
	tmpProtos << "http://" << "ftp://" << "wired://" << "skype:" << "callto:";
	QStringListIterator ia(tmpProtos);
	while(ia.hasNext()) {
		QString tmpProto = ia.next();
		while(true) {
			QTextCursor tmpTC = fChatLog->document()->find(tmpProto, tc);
			if(tmpTC.isNull()) break;
			QTextCursor tmpSpace = fChatLog->document()->find(" ", tmpTC);
			if(!tmpSpace.isNull()) {
				tmpTC.setPosition(tmpSpace.position()-1, QTextCursor::KeepAnchor);
			} else {
				tmpTC.movePosition(QTextCursor::EndOfLine, QTextCursor::KeepAnchor);
			}
			QString tmpUrl = tmpTC.selectedText();
			tmpTC.insertHtml(QString("<a href=\"%1\">%1</a>").arg(tmpUrl));
			tc = tmpTC;
		}
	}
	
	QTextCursor tc2 = fChatLog->textCursor();
	tc2.movePosition(QTextCursor::End);
	fChatLog->setTextCursor(tc2);
	fChatLog->ensureCursorVisible();
	
	
}

// Send the chat message to the server.
void WidgetForum::on_fChatInput_returnPressed() {

	QString msg = fChatInput->toPlainText().trimmed();
	if(msg.startsWith("/me ")) {
		pSession->pWiredSocket->sendChat(pChatID, msg.mid(4), true);
	} else if(msg.startsWith("/topic ")) {
		pSession->pWiredSocket->setConferenceOptions(pChatID, msg.mid(7), "");
	} else if(msg.startsWith("/status ")) {
		pSession->pWiredSocket->setUserStatus(msg.mid(8));
	} else if(msg.startsWith("/nick ")) {
		pSession->pWiredSocket->setNickname(msg.mid(6));
	} else if(msg.startsWith("/clear")) {
		fChatLog->clear();
	} else if(msg.startsWith("/exec ")) {
		QString tmpCmd = msg.mid(6);
		if(!tmpCmd.isEmpty()) {
			QProcess tmpProc;
			tmpProc.start(tmpCmd);
			if(tmpProc.waitForFinished())
				pSession->pWiredSocket->sendChat(pChatID, tmpProc.readAllStandardOutput(), false);
		}
	} else if(msg.startsWith("/caturday")) {
		//   Caturday mode   :3
		pSession->pWiredSocket->setCaturday(true);
		
	} else if(!msg.isEmpty()) {
		pSession->pWiredSocket->sendChat(pChatID, msg, false);
	}
	fChatInput->clear();
}

void WidgetForum::setUserListModel( ModelUserList * model ) { 
	// Set the model of the user list.
	fUsers->setModel(model);
	userlistDelegate = new DelegateUserlist (this);
	fUsers->setItemDelegate(userlistDelegate);
	model->setChatID(pChatID);
	model->setWiredSocket(pSession->pWiredSocket);
	
	// Connect the change event to this window
	connect( fUsers->selectionModel(), SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
		 this, SLOT(onUserlistSelectionChanged(const QItemSelection &, const QItemSelection &)) );
	
	// Allow Drag+Drop only in private chats
	
	fUsers->setSelectionMode(QAbstractItemView::ExtendedSelection);
	fUsers->setDragEnabled(true);
	fUsers->setDropIndicatorShown(true);
	
	if( pChatID!=0 ) {
		fUsers->setAcceptDrops(true);
	}

	// Allow the user to use the "Invite" button if this is a private chat
	if(pChatID!=0) {
		fBtnInvite->setVisible(true);
		updateInviteMenu();

	}
}

void WidgetForum::on_fBtnKick_clicked() {
	const QModelIndex tmpIdx = fUsers->selectionModel()->currentIndex();
	int tmpID = tmpIdx.data(Qt::UserRole).toInt();
	QString tmpNick = tmpIdx.data(Qt::DisplayRole).toString();
	
	bool ok;
	QString tmpReason = QInputDialog::getText(this, tr("Kick"),
			tr("You are about to disconnect '%1'.\nPlease enter a reason and press OK.").arg(tmpNick), QLineEdit::Normal, "", &ok);
	if (ok) {
		pSession->pWiredSocket->kickClient(tmpID, tmpReason);
	}
}

void WidgetForum::on_fBtnBan_clicked() {
	const QModelIndex tmpIdx = fUsers->selectionModel()->currentIndex();
	int tmpID = tmpIdx.data(Qt::UserRole).toInt();
	QString tmpNick = tmpIdx.data(Qt::DisplayRole).toString();
	
	bool ok;
	QString tmpReason = QInputDialog::getText(this, tr("Kick"),
			tr("You are about to ban '%1'.\nPlease enter a reason and press OK.").arg(tmpNick), QLineEdit::Normal, "", &ok);
	if (ok) {
		pSession->pWiredSocket->banClient(tmpID, tmpReason);
	}
}

void WidgetForum::on_fBtnMsg_clicked() {
	const QModelIndex tmpIdx = fUsers->selectionModel()->currentIndex();
	on_fUsers_doubleClicked(tmpIdx);
}

// User list item double-clicked. Show a new private message dialog.
void WidgetForum::on_fUsers_doubleClicked (const QModelIndex &index) {
	int tmpUserID = index.data( Qt::UserRole ).toInt();
	ClassWiredUser tmpUsr = pSession->pWiredSocket->getUserByID(tmpUserID);

	pSession->showMessages();
	pSession->pWinMessages->initUserMessages(tmpUsr);
	pSession->pWinMessages->showUserMessages(tmpUsr.pUserID);
	pSession->pWinMessages->setInputFocus();

}

// User list item has changed/selection changed.
void WidgetForum::onUserlistSelectionChanged(const QItemSelection &, const QItemSelection &) {
	fBtnInfo->setEnabled( fUsers->selectionModel()->hasSelection() && pSession->pWiredSocket->sessionUser.privGetUserInfo );
	fBtnBan->setEnabled( fUsers->selectionModel()->hasSelection() && pSession->pWiredSocket->sessionUser.privBanUsers );
	fBtnMsg->setEnabled( fUsers->selectionModel()->hasSelection() );
	fBtnKick->setEnabled( fUsers->selectionModel()->hasSelection() && pSession->pWiredSocket->sessionUser.privKickUsers );
	fBtnChat->setEnabled( fUsers->selectionModel()->hasSelection() );
}

void WidgetForum::on_fBtnChat_clicked() {
	const QModelIndex tmpIdx = fUsers->selectionModel()->currentIndex();
	if( tmpIdx.isValid() ) {
		int tmpID = tmpIdx.data(Qt::UserRole).toInt();
		pSession->pWiredSocket->createChatWithClient(tmpID);
	}
}

void WidgetForum::on_fBtnInfo_clicked()
{
	const QModelIndex tmpIdx = fUsers->selectionModel()->currentIndex();
	if( tmpIdx.isValid() ) {
		int tmpID = tmpIdx.data(Qt::UserRole).toInt();
		pSession->pWiredSocket->getClientInfo(tmpID);
	}
}


//
// INVITE MENU
//
void WidgetForum::updateInviteMenu() {
	if(!pInviteMenu) {
		pInviteMenu = new QMenu(fBtnInvite);
		fBtnInvite->setMenu(pInviteMenu);
		connect(pInviteMenu, SIGNAL(triggered(QAction*)), this, SLOT(inviteMenuTriggered(QAction*)));
	}
	
	pInviteMenu->clear();
	
	QList<ClassWiredUser> &tmpList = pSession->pWiredSocket->pUsers[1];
	QListIterator<ClassWiredUser> i(tmpList);
	while(i.hasNext()) {
		ClassWiredUser tmpUsr = i.next();
		if(tmpUsr.pUserID!=pSession->pWiredSocket->sessionUser.pUserID) {
			QAction *tmpAct = pInviteMenu->addAction(tmpUsr.pNick);
			tmpAct->setIcon(tmpUsr.iconAsPixmap());
			tmpAct->setData(tmpUsr.pUserID);
		}
	}
}

void WidgetForum::inviteMenuTriggered(QAction * action) {
	pSession->pWiredSocket->inviteClientToChat(pChatID, action->data().toInt());
}

void WidgetForum::reloadPrefs() {
	QPalette p;
	QSettings conf;

	pEmoticonsEnabled = conf.value("interface/chat/emoticons",true).toBool();
	pChatFont.fromString( conf.value("interface/chat/font", WiredSingleton::systemMonospaceFont().append(",11")).toString() );
	
	// Chat log
	p = fChatLog->palette();
	p.setColor(QPalette::Base, WiredSingleton::colorFromPrefs( "interface/chat/back/color", Qt::white) );
	fChatLog->setTextColor( WiredSingleton::colorFromPrefs( "interface/chat/text/color", Qt::black) );
	fChatLog->setPalette(p);

	// Fix existing text
	pChatTextColor = WiredSingleton::colorFromPrefs( "interface/chat/text/color", Qt::black);

	QTextCursor tc(fChatLog->textCursor());
	tc.movePosition(QTextCursor::Start);
	tc.movePosition(QTextCursor::End, QTextCursor::KeepAnchor);
	
	QTextCharFormat cf = fChatLog->currentCharFormat();
	cf.setFont(pChatFont);
	tc.mergeCharFormat(cf);

	// Input box
	p = fChatInput->palette();
	p.setColor(QPalette::Base, WiredSingleton::colorFromPrefs( "interface/chat/back/color", Qt::white) );
	p.setColor(QPalette::Text, WiredSingleton::colorFromPrefs( "interface/chat/text/color", Qt::black) );
	fChatInput->setPalette(p);
	fChatInput->setFont(pChatFont);

	pChatStyle = conf.value("interface/chat/style",0).toInt();
	pChatShowTime = conf.value("interface/chat/show_time",false).toBool();
	pChatTimeColor =  WiredSingleton::colorFromPrefs( "interface/chat/time/color", Qt::darkGray);
	pChatEventColor = WiredSingleton::colorFromPrefs("interface/chat/events/color", Qt::red);

	
}






