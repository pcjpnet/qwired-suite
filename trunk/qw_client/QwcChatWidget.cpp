
#include "QwcSession.h"
#include "QwcChatWidget.h"
#include "QwcGlobals.h"
#include "QwcSocket.h"

#include <QtGui/QInputDialog>
#include <QtGui/QFileDialog>
#include <QtGui/QDesktopServices>

#include <QtCore/QProcess>

#include <QtWebKit/QWebFrame>





QwcChatWidget::QwcChatWidget(QWidget *parent) : QWidget (parent)
{
    m_session = NULL;
    m_chatId = Qwc::PUBLIC_CHAT;

    setAttribute(Qt::WA_DeleteOnClose);
    setupUi(this);
    fBtnInvite->setVisible(false);

    fChatInput->installEventFilter(this);

    fVSplitter->setStretchFactor(0,20);
    fVSplitter->setStretchFactor(1,1);
    fHSplitter->setStretchFactor(0,20);
    fHSplitter->setStretchFactor(1,1);

    QSettings settings;
    if(settings.contains("gui/chat_splitter_h"))
        fHSplitter->restoreState(settings.value("gui/chat_splitter_h").toByteArray());
    if(settings.contains("gui/chat_splitter_v"))
        fVSplitter->restoreState(settings.value("gui/chat_splitter_v").toByteArray());

    // Set up the chat view
    m_currentMessageStyle.loadStyle(":/styles/Default.QwiredMessageStyle");
    chatView->setHtml(m_currentMessageStyle.chatTemplate());
    chatView->page()->setLinkDelegationPolicy(QWebPage::DelegateExternalLinks);
    chatView->installEventFilter(this);
    connect(chatView->page(), SIGNAL(linkClicked(QUrl)),
            this, SLOT(handleExternalLinkClicked(QUrl)));
    connect(chatView->page()->mainFrame(), SIGNAL(contentsSizeChanged(QSize)),
            this, SLOT(handleChatViewFrameSizeChanged(QSize)));

    // Notification manager
    QwcSingleton *tmpS = &WSINGLETON::Instance();
    connect(tmpS, SIGNAL(applicationSettingsChanged()),
            this, SLOT(reloadPreferences()));

    reloadPreferences();
}


QwcChatWidget::~QwcChatWidget()
{
    // Leave the chat, if it is private
    if( m_chatId != 1 ) {
        m_session->m_chatWidgets.remove(m_chatId);
        m_session->socket()->leaveChat(m_chatId);
    }

    // Save the splitters
    QSettings s;
    s.setValue("gui/chat_splitter_h", fHSplitter->saveState());
    s.setValue("gui/chat_splitter_v", fVSplitter->saveState());
}


/*! Returns the session pointer for this chat widget.
*/
QwcSession* QwcChatWidget::session()
{ return m_session; }

void QwcChatWidget::setChatId(int newId)
{
    m_chatId = newId;
    fBtnChat->setVisible(newId == Qwc::PUBLIC_CHAT);
}

int QwcChatWidget::chatId() const
{ return m_chatId; }

void QwcChatWidget::setSession(QwcSession *session)
{
    Q_ASSERT(session);
    m_session = session;
    userListWidget->setSocket(session->socket());
}


/*! This is the chat widget's event filter. This can be used for things like tab auto-completition.
*/
bool QwcChatWidget::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == chatView) {
        if (event->type() == QEvent::ContextMenu) {
            return true;
        }
    }

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
                QwRoom currentRoom = m_session->socket()->rooms[m_chatId];
                QListIterator<int> i(currentRoom.pUsers);
                while (i.hasNext()) {
                    // Now run through all user IDs registered in this room and get the proper
                    // QwcUserInfo from the socket.
                    int itemUserId = i.next();
                    QwcUserInfo targetUser = m_session->socket()->users[itemUserId];
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

            } else if (keyEvent->key() == Qt::Key_Return
                       || keyEvent->key() == Qt::Key_Enter) {
                // Return/Enter was pressed - send chat
                processChatInput();
                return true;
            }
        }
    }

    return QWidget::eventFilter(watched, event);
}


/*! Write a status message/event to the chat.
*/
void QwcChatWidget::writeEventToChat(QString eventText, QString eventType)
{
    m_currentMessageStyle.addStatusMessage(chatView->page()->currentFrame(), eventText);
}


void QwcChatWidget::writeBroadcastToChat(QwcUserInfo sender, QString text)
{
    m_currentMessageStyle.addBroadcastMessage(chatView->page()->currentFrame(), sender, text);
}


void QwcChatWidget::setTopic(const QString &text, const QString &user, const QDateTime &dateTime)
{
    QwcMessageStyle::setTopicContents(chatView->page()->currentFrame(), text, user, dateTime);
}


/// Write some chat text to the chat log view.
void QwcChatWidget::writeTextToChat(QwUser sender, QString text, bool emote)
{
    QSettings settings;
    QString fixedString = m_currentMessageStyle.replaceEmoticons(text,
                            settings.value("interface/chat/emoticons", true).toBool());
    m_currentMessageStyle.addChatMessage(chatView->page()->currentFrame(),
                                         sender, fixedString, emote);
}



/*! Open any clicked URLs externally using the system's URL handler.
*/
void QwcChatWidget::handleExternalLinkClicked(const QUrl &url)
{
    QDesktopServices::openUrl(url);
}


/// Send the chat message to the server.
void QwcChatWidget::processChatInput()
{
    QString inputText = fChatInput->toPlainText();
    if(inputText.startsWith("/me ")) {
        // Write an emote to the current chat room
        m_session->socket()->writeToChat(m_chatId, inputText.mid(4), true);

    } else if(inputText.startsWith("/topic ")) {
        // Change the chat room topic
        m_session->socket()->setChatTopic(m_chatId, inputText.mid(7));

    } else if (inputText.startsWith("/status ")) {
        // Change the current user status
        m_session->socket()->setUserStatus(inputText.mid(8));

    } else if(inputText.startsWith("/nick ")) {
        m_session->socket()->setNickname(inputText.mid(6));

    } else if(inputText.startsWith("/clear")) {
        m_currentMessageStyle.clearChat(chatView->page()->mainFrame());

    } else if(inputText.startsWith("/exec ")) {
        // Execute a system command
        QString command = inputText.mid(6);
        if (!command.isEmpty()) {
            QProcess process;
            process.start(command);
            if(process.waitForFinished())
                m_session->socket()->writeToChat(m_chatId, process.readAllStandardOutput(), false);
        }

    } else if (inputText.startsWith("/save ")) {
        QString targetFile = QFileDialog::getSaveFileName(this,
                                                          tr("Save chat to file"),
                                                          QDir::home().filePath(inputText.mid(6)));
        if (targetFile.isEmpty()) { return; }
        QFile outFile(targetFile);
        if (!outFile.open(QIODevice::WriteOnly)) {
            //: The message that is displayed in the chat if saving the chat transcript failed (/save)
            writeEventToChat(tr("Error: Failed to write log to file."), "");
            return;
        }

        outFile.write(chatView->page()->mainFrame()->toHtml().toUtf8());
        //: The message that is displayed in the chat if the log was saved successfully (/save)
        writeEventToChat(tr("Chat transcript was saved to %1").arg(targetFile), "");


    } else if (inputText.startsWith("/caturday")) {
        // Surprise, surprise!
        m_session->socket()->setCaturdayMode(true);

    } else {
        // No explicit command - send to chat
        if (inputText.trimmed().isEmpty()) { return; }

        bool isEmote = qApp->keyboardModifiers() & Qt::AltModifier;
        m_session->socket()->writeToChat(m_chatId, inputText, isEmote);
    }

    fChatInput->clear();
}


void QwcChatWidget::on_fBtnKick_clicked()
{
    const QModelIndex tmpIdx = userListWidget->selectionModel()->currentIndex();
    if (!tmpIdx.isValid()) { return; }
    QwcUserInfo userInfo = tmpIdx.data(Qt::UserRole).value<QwcUserInfo>();

    bool ok;
    QString reason = QInputDialog::getText(this, tr("Kick"),
                        tr("You are about to disconnect '%1'.\nPlease enter a reason and press OK.")
                        .arg(userInfo.userNickname), QLineEdit::Normal, "", &ok);
    if (!ok) { return; }
    m_session->socket()->kickClient(userInfo.pUserID, reason);
}


void QwcChatWidget::on_fBtnBan_clicked()
{
    const QModelIndex tmpIdx = userListWidget->selectionModel()->currentIndex();
    if (!tmpIdx.isValid()) { return; }
    QwcUserInfo userInfo = tmpIdx.data(Qt::UserRole).value<QwcUserInfo>();

    bool ok;
    QString reason = QInputDialog::getText(this, tr("Kick"),
                                           tr("You are about to ban '%1'.\nPlease enter a reason and press OK.")
                                           .arg(userInfo.userNickname), QLineEdit::Normal, "", &ok);
    if (!ok) { return; }
    m_session->socket()->banClient(userInfo.pUserID, reason);
}


void QwcChatWidget::on_fBtnMsg_clicked()
{
    const QModelIndex tmpIdx = userListWidget->selectionModel()->currentIndex();
    on_fUsers_doubleClicked(tmpIdx);
}


/*! A user (or more) have been double-clicked. We emit a signal for this event, so the session
    can comfortably handle this.
*/
void QwcChatWidget::on_fUsers_doubleClicked(const QModelIndex &index)
{
    if (!index.isValid()) { return; }
    QwcUserInfo userInfo = index.data(Qt::UserRole).value<QwcUserInfo>();
    emit userDoubleClicked(userInfo);
}


/*! The selection of the user list has changed.
*/
void QwcChatWidget::onUserlistSelectionChanged(const QItemSelection &, const QItemSelection &)
{
    fBtnInfo->setEnabled(userListWidget->selectionModel()->hasSelection()
                         && m_session->socket()->sessionUser.privileges().testFlag(Qws::PrivilegeGetUserInfo));

    fBtnBan->setEnabled(userListWidget->selectionModel()->hasSelection()
                        && m_session->socket()->sessionUser.privileges().testFlag(Qws::PrivilegeBanUsers));
    fBtnMsg->setEnabled(userListWidget->selectionModel()->hasSelection());
    fBtnKick->setEnabled(userListWidget->selectionModel()->hasSelection()
                         && m_session->socket()->sessionUser.privileges().testFlag(Qws::PrivilegeKickUsers));
    fBtnChat->setEnabled(userListWidget->selectionModel()->hasSelection());
}


/*! The privat-chat button has been clicked.
*/
void QwcChatWidget::on_fBtnChat_clicked()
{
    const QModelIndex tmpIdx = userListWidget->selectionModel()->currentIndex();
    if (!tmpIdx.isValid()) { return; }
    QwcUserInfo userInfo = tmpIdx.data(Qt::UserRole).value<QwcUserInfo>();
    m_session->socket()->createChatWithClient(userInfo.pUserID);
}


/*! The get-info button has been clicked.
*/
void QwcChatWidget::on_fBtnInfo_clicked()
{
    const QModelIndex tmpIdx = userListWidget->selectionModel()->currentIndex();
    if (!tmpIdx.isValid()) { return; }
    QwcUserInfo userInfo = tmpIdx.data(Qt::UserRole).value<QwcUserInfo>();
    m_session->socket()->getClientInformation(userInfo.pUserID);

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
    QHashIterator<int, QwcUserInfo> i(m_session->socket()->users);
    while (i.hasNext()) {
        i.next();
        const QwcUserInfo &item = i.value();
        if (item.pUserID == m_session->socket()->sessionUser.pUserID) { continue; }
        QAction *tmpAct = pInviteMenu->addAction(item.userNickname);
        tmpAct->setIcon(QPixmap::fromImage(item.userImage()));
        tmpAct->setData(item.pUserID);
    }
}


void QwcChatWidget::inviteMenuTriggered(QAction *action)
{
    m_session->socket()->inviteClientToChat(m_chatId, action->data().toInt());
}


void QwcChatWidget::reloadPreferences()
{
    QSettings settings;


    // Chat font
    // userCss settings property can be used to override the style alltogether, but must be edited
    // manually in the settings file.
    QFont chatFont;
    chatFont.fromString(settings.value("interface/chat/font", QFont().toString()).toString());
    m_currentMessageStyle.setStyleOverride(chatView->page()->mainFrame(),
                                           QString("body { %1 } \n%2")
                                           .arg(QwcMessageStyle::cssFromFont(chatFont))
                                           .arg(settings.value("interface/chat/userCss").toString()));
    fChatInput->setFont(chatFont);
}


void QwcChatWidget::resetForm()
{
    fChatInput->clear();
}


void QwcChatWidget::handleChatViewFrameSizeChanged(QSize size)
{
    chatView->page()->mainFrame()->setScrollPosition(QPoint(0, size.height()));
}






