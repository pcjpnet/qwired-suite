
#include "QwcSession.h"
#include "QwcChatWidget.h"
#include "QwcGlobals.h"
#include <QInputDialog>
#include <QProcess>

QwcChatWidget::QwcChatWidget(QWidget *parent) : QWidget (parent)
{
    pSession = NULL;
    pChatID = 1;

    setAttribute(Qt::WA_DeleteOnClose);
    setupUi(this);
    fBtnInvite->setVisible(false);


    // We can filter the return key with this
    fChatInput->installEventFilter(this);

    // Set vertical splitter
    fVSplitter->setStretchFactor(0,20);
    fVSplitter->setStretchFactor(1,1);

    // Set horizontal splitter
    fHSplitter->setStretchFactor(0,20);
    fHSplitter->setStretchFactor(1,1);

    // Restore splitters from prefs
    QSettings settings;
    if(settings.contains("gui/chat_splitter_h"))
        fHSplitter->restoreState(settings.value("gui/chat_splitter_h").toByteArray());
    if(settings.contains("gui/chat_splitter_v"))
        fVSplitter->restoreState(settings.value("gui/chat_splitter_v").toByteArray());

    // Notification manager
    QwcSingleton *tmpS = &WSINGLETON::Instance();
    connect(tmpS, SIGNAL(prefsChanged()),
            this, SLOT(reloadPreferences()));

    reloadPreferences();


}


QwcChatWidget::~QwcChatWidget()
{
    // Leave the chat, if it is private
    if( pChatID!=1 ) {
        pSession->pChats.remove(pChatID);
        pSession->wiredSocket()->leaveChat(pChatID);
    }

    // Save the splitters
    QSettings s;
    s.setValue("gui/chat_splitter_h", fHSplitter->saveState());
    s.setValue("gui/chat_splitter_v", fVSplitter->saveState());
}


QwcSession* QwcChatWidget::session()
{
    return pSession;
}

void QwcChatWidget::setSession(QwcSession *session)
{
    pSession = session;
}


/*! This is the chat widget's event filter. This can be used for things like tab auto-completition.
*/
bool QwcChatWidget::eventFilter(QObject *watched, QEvent *event)
{
    // Check for tab-auto completition.
    if (watched == fChatInput) {
        if (event->type() == QEvent::KeyPress) {
            QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
            if (keyEvent->key() == Qt::Key_Tab) {
                QTextCursor textCursor = fChatInput->textCursor();
                textCursor.movePosition(QTextCursor::StartOfWord, QTextCursor::KeepAnchor);
                QString searchTerm = textCursor.selectedText().trimmed();

                if (searchTerm.isEmpty()) {
                    // We can't do much with an empty hint, so let's ignore that.
                    return true;
                }

                // Get information about the current room from the socket
                QwRoom currentRoom = pSession->wiredSocket()->rooms[pChatID];
                QListIterator<int> i(currentRoom.pUsers);
                while (i.hasNext()) {
                    // Now run through all user IDs registered in this room and get the proper
                    // QwcUserInfo from the socket.
                    int itemUserId = i.next();
                    QwcUserInfo targetUser = pSession->wiredSocket()->users[itemUserId];
                    // Check if the nickname starts with the entered seach terms and auto-complete.
                    if (targetUser.userNickname.startsWith(searchTerm, Qt::CaseInsensitive)) {
                        if (textCursor.selectionStart() == 0) {
                            // If the target nickname is entered first, we should add a colon.
                            textCursor.insertText(targetUser.userNickname + ": ");
                        } else {
                            // Otherwise the user might be using the target nickname in a
                            // conversation.
                            textCursor.insertText(targetUser.userNickname);
                        }
                        break;
                    }
                }
                return true;

            } else if (keyEvent->key() == Qt::Key_Return || keyEvent->key() == Qt::Key_Enter) {
                // Check if the pressed key was return or enter in order to send the chat text.
                postChatInputText();
                return true;
            }
        }
    }

    return false;
}


/// Write an event to the chat
void QwcChatWidget::writeEventToChat(QString theMsg)
{
    QTextCursor tc = fChatLog->textCursor();
    tc.movePosition(QTextCursor::End);

    QTextCharFormat f;
    f.setFont(pChatFont);
    f.setForeground(pChatTimeColor);
    tc.setCharFormat(f);

    QString tmpTimestamp;
    if (pChatShowTime) {
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


/// Write some chat text to the chat log view.
void QwcChatWidget::writeToChat(QString theUser, QString theText, bool theEmote)
{

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
    if(pChatStyle==0) { // Wired style
        if(theEmote)
            tmpData = QString("%1 %2\n").arg(QString("*** %1").arg(theUser), tmpNameLength-tmpTimestamp.size()).arg(theText);
        else tmpData = QString("%1: %2\n").arg(theUser.left(tmpNameLength-tmpTimestamp.count() ), tmpNameLength-tmpTimestamp.size()).arg(theText);
        tc.insertText(tmpData);

    } else if(pChatStyle==1) { // IRC style
        if(theEmote)
            tmpData = QString("*** %1 %2\n").arg(theUser).arg(theText);
        else tmpData = QString("<%1> %2\n").arg(theUser).arg(theText);
        tc.insertText(tmpData);

    } else if(pChatStyle==2) { // Qwired style
        // 		QString nick = theUser;
        // 		nick.replace("<", "&lt;");
        // 		nick.replace(">", "&gt;");

        if(theEmote) {
            tc.insertHtml(QString("<b>&#9787; %1</b> %2<br>")
                          .arg(QString(theUser).replace("<", "&lt;").replace(">", "&gt;"))
                          .arg(QString(theText).replace("<", "&lt;").replace(">", "&gt;")));
        } else {
            tc.insertHtml(QString("<b>%1:</b> %2<br>")
                          .arg(QString(theUser).replace("<", "&lt;").replace(">", "&gt;"))
                          .arg(QString(theText).replace("<", "&lt;").replace(">", "&gt;")));
        }
        // 			tmpData = QString("*** %1 %2\n").arg(theUser).arg(theText);
        // 		else tmpData = QString("<%1> %2\n").arg(theUser).arg(theText);
    }

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
                tmpTC.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
            }
            QString tmpUrl = tmpTC.selectedText();
            tmpTC.insertHtml(QString("<a href=\"%1\"><font style=\"font-family: %2; font-size: %3pt\">%1</font></a>").arg(tmpUrl).arg(fChatLog->currentFont().family()).arg(QString::number(fChatLog->currentFont().pointSize())));
            tc = tmpTC;
        }
    }

    QTextCursor tc2 = fChatLog->textCursor();
    tc2.movePosition(QTextCursor::End);
    fChatLog->setTextCursor(tc2);
    fChatLog->ensureCursorVisible();


}


/// Send the chat message to the server.
void QwcChatWidget::postChatInputText()
{
    QString msg = fChatInput->toPlainText();
    if(msg.startsWith("/me ")) {
        pSession->wiredSocket()->sendChatToRoom(pChatID, msg.mid(4), true);
    } else if(msg.startsWith("/topic ")) {
        pSession->wiredSocket()->setChatTopic(pChatID, msg.mid(7));
    } else if(msg.startsWith("/status ")) {
        pSession->wiredSocket()->setUserStatus(msg.mid(8));
    } else if(msg.startsWith("/nick ")) {
        pSession->wiredSocket()->setNickname(msg.mid(6));
    } else if(msg.startsWith("/clear")) {
        fChatLog->clear();
    } else if(msg.startsWith("/exec ")) {
        QString tmpCmd = msg.mid(6);
        if(!tmpCmd.isEmpty()) {
            QProcess tmpProc;
            tmpProc.start(tmpCmd);
            if(tmpProc.waitForFinished())
                pSession->wiredSocket()->sendChatToRoom(pChatID, tmpProc.readAllStandardOutput(), false);
        }
    } else if(msg.startsWith("/caturday")) {
        //   Caturday mode   :3
        pSession->wiredSocket()->setCaturday(true);

    } else {
        pSession->wiredSocket()->sendChatToRoom(pChatID, msg, false);
    }
    fChatInput->clear();
}


/*! Set the current model for the ListView.
*/
void QwcChatWidget::setUserListModel(QwcUserlistModel *model)
{ 
    if (!model) { return; }

    // Set the model of the user list.
    fUsers->setModel(model);
    fUsers->setItemDelegate(new QwcUserlistDelegate(this));

    model->setChatID(pChatID);
    model->setWiredSocket(pSession->wiredSocket());

    // Connect the change event to this window
    connect(fUsers->selectionModel(), SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
             this, SLOT(onUserlistSelectionChanged(const QItemSelection &, const QItemSelection &)) );

    // Allow Drag+Drop only in private chats

    fUsers->setSelectionMode(QAbstractItemView::ExtendedSelection);
//    fUsers->setDragEnabled(true);
//    fUsers->setDropIndicatorShown(true);

    if (pChatID != 1) {
        fUsers->setAcceptDrops(true);
    }

    // Allow the user to use the "Invite" button if this is a private chat
    if(pChatID != 1) {
        fBtnInvite->setVisible(true);
        updateInviteMenu();
    }
}


void QwcChatWidget::on_fBtnKick_clicked()
{
    const QModelIndex tmpIdx = fUsers->selectionModel()->currentIndex();
    int tmpID = tmpIdx.data(Qt::UserRole).toInt();
    QString tmpNick = tmpIdx.data(Qt::DisplayRole).toString();

    bool ok;
    QString tmpReason = QInputDialog::getText(this, tr("Kick"),
                                              tr("You are about to disconnect '%1'.\nPlease enter a reason and press OK.").arg(tmpNick), QLineEdit::Normal, "", &ok);
    if (ok) {
        pSession->wiredSocket()->kickClient(tmpID, tmpReason);
    }
}


void QwcChatWidget::on_fBtnBan_clicked()
{
    const QModelIndex tmpIdx = fUsers->selectionModel()->currentIndex();
    int tmpID = tmpIdx.data(Qt::UserRole).toInt();
    QString tmpNick = tmpIdx.data(Qt::DisplayRole).toString();

    bool ok;
    QString tmpReason = QInputDialog::getText(this, tr("Kick"),
                                              tr("You are about to ban '%1'.\nPlease enter a reason and press OK.").arg(tmpNick), QLineEdit::Normal, "", &ok);
    if (ok) {
        pSession->wiredSocket()->banClient(tmpID, tmpReason);
    }
}


void QwcChatWidget::on_fBtnMsg_clicked()
{
    const QModelIndex tmpIdx = fUsers->selectionModel()->currentIndex();
    on_fUsers_doubleClicked(tmpIdx);
}


/*! A user (or more) have been double-clicked. We emit a signal for this event, so the session
    can comfortably handle this.
*/
void QwcChatWidget::on_fUsers_doubleClicked(const QModelIndex &index)
{
    if (!index.isValid()) { return; }
    if (!index.data(Qt::UserRole).canConvert<QwcUserInfo>()) { return; }

    QwcUserInfo targetUser = index.data(Qt::UserRole).value<QwcUserInfo>();
    emit userDoubleClicked(targetUser);
}

// User list item has changed/selection changed.
void QwcChatWidget::onUserlistSelectionChanged(const QItemSelection &, const QItemSelection &)
{
    fBtnInfo->setEnabled( fUsers->selectionModel()->hasSelection() && pSession->wiredSocket()->sessionUser.privGetUserInfo );
    fBtnBan->setEnabled( fUsers->selectionModel()->hasSelection() && pSession->wiredSocket()->sessionUser.privBanUsers );
    fBtnMsg->setEnabled( fUsers->selectionModel()->hasSelection() );
    fBtnKick->setEnabled( fUsers->selectionModel()->hasSelection() && pSession->wiredSocket()->sessionUser.privKickUsers );
    fBtnChat->setEnabled( fUsers->selectionModel()->hasSelection() );
}


void QwcChatWidget::on_fBtnChat_clicked()
{
    const QModelIndex tmpIdx = fUsers->selectionModel()->currentIndex();
    if( tmpIdx.isValid() ) {
        int tmpID = tmpIdx.data(Qt::UserRole).toInt();
        pSession->wiredSocket()->createChatWithClient(tmpID);
    }
}


void QwcChatWidget::on_fBtnInfo_clicked()
{
    const QModelIndex tmpIdx = fUsers->selectionModel()->currentIndex();
    if (tmpIdx.isValid()) {
        int tmpID = tmpIdx.data(Qt::UserRole).toInt();
        pSession->wiredSocket()->getClientInfo(tmpID);
    }
}


//
// INVITE MENU
//
void QwcChatWidget::updateInviteMenu()
{
    if (!pInviteMenu) {
        pInviteMenu = new QMenu(fBtnInvite);
        fBtnInvite->setMenu(pInviteMenu);
        connect(pInviteMenu, SIGNAL(triggered(QAction*)),
                this, SLOT(inviteMenuTriggered(QAction*)));
    }

    pInviteMenu->clear();
    QHashIterator<int, QwcUserInfo> i(pSession->wiredSocket()->users);
    while (i.hasNext()) {
        i.next();
        const QwcUserInfo &item = i.value();
        if (item.pUserID == pSession->wiredSocket()->sessionUser.pUserID) { continue; }
        QAction *tmpAct = pInviteMenu->addAction(item.userNickname);
        tmpAct->setIcon(QPixmap::fromImage(item.userImage));
        tmpAct->setData(item.pUserID);
    }
}


void QwcChatWidget::inviteMenuTriggered(QAction *action)
{
    pSession->wiredSocket()->inviteClientToChat(pChatID, action->data().toInt());
}


void QwcChatWidget::reloadPreferences()
{
    QPalette p;
    QSettings conf;

    pEmoticonsEnabled = conf.value("interface/chat/emoticons",true).toBool();
    pChatFont.fromString( conf.value("interface/chat/font", QwcSingleton::systemMonospaceFont().append(",11")).toString() );

    // Chat log
    p = fChatLog->palette();
    p.setColor(QPalette::Base, QwcSingleton::colorFromPrefs( "interface/chat/back/color", Qt::white) );
    fChatLog->setTextColor( QwcSingleton::colorFromPrefs( "interface/chat/text/color", Qt::black) );
    fChatLog->setPalette(p);

    // Fix existing text
    pChatTextColor = QwcSingleton::colorFromPrefs( "interface/chat/text/color", Qt::black);

    QTextCursor tc(fChatLog->textCursor());
    tc.movePosition(QTextCursor::Start);
    tc.movePosition(QTextCursor::End, QTextCursor::KeepAnchor);

    QTextCharFormat cf = fChatLog->currentCharFormat();
    cf.setFont(pChatFont);
    tc.mergeCharFormat(cf);

    // Input box
    p = fChatInput->palette();
    p.setColor(QPalette::Base, QwcSingleton::colorFromPrefs( "interface/chat/back/color", Qt::white) );
    p.setColor(QPalette::Text, QwcSingleton::colorFromPrefs( "interface/chat/text/color", Qt::black) );
    fChatInput->setPalette(p);
    fChatInput->setFont(pChatFont);

    pChatStyle = conf.value("interface/chat/style",0).toInt();
    pChatShowTime = conf.value("interface/chat/show_time",false).toBool();
    pChatTimeColor =  QwcSingleton::colorFromPrefs( "interface/chat/time/color", Qt::darkGray);
    pChatEventColor = QwcSingleton::colorFromPrefs("interface/chat/events/color", Qt::red);


}

/// Reset the form.
void QwcChatWidget::resetForm()
{
    fChatLog->clear();
    fChatInput->clear();
    fTopic->clear();
}








