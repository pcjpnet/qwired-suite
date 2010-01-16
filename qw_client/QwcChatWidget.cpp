
#include "QwcSocket.h"
#include "QwcChatWidget.h"
#include "QwcGlobals.h"
#include "QwcSocket.h"

#include <QtGui/QInputDialog>
#include <QtGui/QFileDialog>
#include <QtGui/QDesktopServices>

#include <QtCore/QProcess>

#include <QtWebKit/QWebFrame>


QwcChatWidget::QwcChatWidget(QWidget *parent) :
        QWidget(parent)
{
    setAttribute(Qt::WA_DeleteOnClose);

    m_socket = NULL;
    m_chatId = Qwc::PUBLIC_CHAT;

    setupUi(this);
    btnInvite->setVisible(false);

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
    if (m_chatId != Qwc::PUBLIC_CHAT) {
        m_socket->leaveChat(m_chatId);
    }

    // Save the splitters
    QSettings settings;
    settings.setValue("gui/chat_splitter_h", fHSplitter->saveState());
    settings.setValue("gui/chat_splitter_v", fVSplitter->saveState());
}


void QwcChatWidget::setSocket(QwcSocket *socket)
{
    if (m_socket) { disconnect(m_socket, 0, this, 0); }
    m_socket = socket;
    if (!socket) { return; }
    userListWidget->setSocket(m_socket);

    connect(m_socket, SIGNAL(onChatTopic(int,QString,QString,QHostAddress,QDateTime,QString)),
            this, SLOT(handleSocketChatTopic(int,QString,QString,QHostAddress,QDateTime,QString)));
    connect(m_socket, SIGNAL(broadcastMessage(QwcUserInfo,QString)),
            this, SLOT(handleSocketBroadcastMessage(QwcUserInfo,QString)));
    connect(m_socket, SIGNAL(receivedChatMessage(int,int,QString,bool)),
            this, SLOT(handleSocketChatMessage(int,int,QString,bool)));
    connect(m_socket, SIGNAL(privateChatDeclined(int,QwcUserInfo)),
            this, SLOT(handleSocketPrivateChatDeclined(int,QwcUserInfo)));
}

QwcSocket* QwcChatWidget::socket()
{ return m_socket; }


void QwcChatWidget::setChatId(int newId)
{
    m_chatId = newId;
    userListWidget->setChatId(newId);
    btnInvite->setVisible(newId != Qwc::PUBLIC_CHAT);
    m_socket->getUserlist(newId);
}

int QwcChatWidget::chatId() const
{ return m_chatId; }


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
                QwRoom currentRoom = m_socket->rooms[m_chatId];
                QListIterator<int> i(currentRoom.pUsers);
                while (i.hasNext()) {
                    // Now run through all user IDs registered in this room and get the proper
                    // QwcUserInfo from the socket.
                    int itemUserId = i.next();
                    QwcUserInfo targetUser = m_socket->users[itemUserId];
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
void QwcChatWidget::writeEventToChat(QString eventText)
{
    m_currentMessageStyle.addStatusMessage(chatView->page()->currentFrame(), eventText);
}


void QwcChatWidget::handleSocketBroadcastMessage(QwcUserInfo sender, QString text)
{
    m_currentMessageStyle.addBroadcastMessage(chatView->page()->currentFrame(), sender, text);
}


void QwcChatWidget::handleSocketChatMessage(int chatId, int userId, const QString &text, bool isEmote)
{
    Q_ASSERT(m_socket);
    if (chatId != m_chatId) { return; }
    QwcUserInfo sender = m_socket->users[userId];
    writeTextToChat(sender, text, isEmote);
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

void QwcChatWidget::handleSocketChatTopic(int chatId, const QString &nickname, const QString &login,
                                          QHostAddress ip, QDateTime dateTime, const QString &topic)
{
    if (chatId != m_chatId) { return; }
    QwcMessageStyle::setTopicContents(chatView->page()->currentFrame(), topic, nickname, dateTime);
}


void QwcChatWidget::handleSocketPrivateChatDeclined(int chatId, const QwcUserInfo &user)
{
    if (chatId != m_chatId) { return; }

    writeEventToChat(tr("%1 declined the invitation to a private chat").arg(user.userNickname));
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
        m_socket->writeToChat(m_chatId, inputText.mid(4), true);

    } else if(inputText.startsWith("/topic ")) {
        // Change the chat room topic
        m_socket->setChatTopic(m_chatId, inputText.mid(7));

    } else if (inputText.startsWith("/status ")) {
        // Change the current user status
        m_socket->setUserStatus(inputText.mid(8));

    } else if(inputText.startsWith("/nick ")) {
        m_socket->setNickname(inputText.mid(6));

    } else if(inputText.startsWith("/clear")) {
        m_currentMessageStyle.clearChat(chatView->page()->mainFrame());

    } else if(inputText.startsWith("/exec ")) {
        // Execute a system command
        QString command = inputText.mid(6);
        if (!command.isEmpty()) {
            QProcess process;
            process.start(command);
            if(process.waitForFinished())
                m_socket->writeToChat(m_chatId, process.readAllStandardOutput(), false);
        }

    } else if (inputText.startsWith("/save ")) {
        QString targetFile = QFileDialog::getSaveFileName(this,
                                                          tr("Save chat to file"),
                                                          QDir::home().filePath(inputText.mid(6)));
        if (targetFile.isEmpty()) { return; }
        QFile outFile(targetFile);
        if (!outFile.open(QIODevice::WriteOnly)) {
            //: The message that is displayed in the chat if saving the chat transcript failed (/save)
            writeEventToChat(tr("Error: Failed to write log to file."));
            return;
        }

        outFile.write(chatView->page()->mainFrame()->toHtml().toUtf8());
        //: The message that is displayed in the chat if the log was saved successfully (/save)
        writeEventToChat(tr("Chat transcript was saved to %1").arg(targetFile));


    } else if (inputText.startsWith("/caturday")) {
        // Surprise, surprise!
        m_socket->setCaturdayMode(true);

    } else {
        // No explicit command - send to chat
        if (inputText.trimmed().isEmpty()) { return; }

        bool isEmote = qApp->keyboardModifiers() & Qt::AltModifier;
        m_socket->writeToChat(m_chatId, inputText, isEmote);
    }

    fChatInput->clear();
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


/*! Workaround helper for scrolling in the chat log.
*/
void QwcChatWidget::handleChatViewFrameSizeChanged(QSize size)
{
    chatView->page()->mainFrame()->setScrollPosition(QPoint(0, size.height()));
}

/*! A user (or more) have been double-clicked. We emit a signal for this event, so the session
    can comfortably handle this.
*/
void QwcChatWidget::on_userListWidget_itemDoubleClicked()
{
    if (!userListWidget->selectedUsers().count()) { return; }
    QwcUserInfo target = userListWidget->selectedUsers().first();
    emit requestedNewPrivateMessage(target, QString());
}


/*! The selection of the user list has changed.
*/
void QwcChatWidget::on_userListWidget_itemSelectionChanged()
{
    bool hasSelection = userListWidget->selectionModel()->hasSelection();
    btnInformation->setEnabled(hasSelection && m_socket->sessionUser().privileges() & Qws::PrivilegeGetUserInfo);
    btnBan->setEnabled(hasSelection && m_socket->sessionUser().privileges() & Qws::PrivilegeBanUsers);
    btnMessage->setEnabled(hasSelection && m_socket->sessionUser().privileges());
    btnKick->setEnabled(hasSelection && m_socket->sessionUser().privileges() & Qws::PrivilegeKickUsers);
    btnChat->setEnabled(hasSelection);
}

void QwcChatWidget::on_btnInvite_clicked()
{
    QMenu menu;

    QwRoom publicRoom = m_socket->rooms[Qwc::PUBLIC_CHAT];
    foreach (int id, publicRoom.pUsers) {
        QAction *action = menu.addAction(m_socket->users[id].userNickname);
        action->setIcon(QPixmap::fromImage(m_socket->users[id].userImage()));
        action->setData(id);
    }

    QAction *result = menu.exec();
    if (!result) { return; }
    m_socket->inviteClientToChat(m_chatId, result->data().toInt());

    writeEventToChat(tr("Invited %1 to join this chat").arg(result->text()));
}

void QwcChatWidget::on_btnMessage_clicked()
{
    if (!userListWidget->selectedUsers().count()) { return; }
    QwcUserInfo target = userListWidget->selectedUsers().first();
    emit requestedNewPrivateMessage(target, QString());
}


void QwcChatWidget::on_btnKick_clicked()
{
    if (!userListWidget->selectedUsers().count()) { return; }
    QwcUserInfo target = userListWidget->selectedUsers().first();

    bool ok;
    QString reason = QInputDialog::getText(this, tr("Kick User"),
                        tr("You are about to disconnect '%1'.\nPlease enter a reason and press OK.")
                        .arg(target.userNickname), QLineEdit::Normal, "", &ok);
    if (!ok) { return; }
    m_socket->kickClient(target.pUserID, reason);
}


void QwcChatWidget::on_btnBan_clicked()
{
    if (!userListWidget->selectedUsers().count()) { return; }
    QwcUserInfo target = userListWidget->selectedUsers().first();

    bool ok;
    QString reason = QInputDialog::getText(this, tr("Kick"),
                                           tr("You are about to ban '%1'.\nPlease enter a reason and press OK.")
                                           .arg(target.userNickname), QLineEdit::Normal, "", &ok);
    if (!ok) { return; }
    m_socket->banClient(target.pUserID, reason);
}


void QwcChatWidget::on_btnInformation_clicked()
{
    if (!userListWidget->selectedUsers().count()) { return; }
    QwcUserInfo target = userListWidget->selectedUsers().first();
    m_socket->getClientInformation(target.pUserID);
}


void QwcChatWidget::on_btnChat_clicked()
{
    if (!userListWidget->selectedUsers().count()) { return; }
    QwcUserInfo target = userListWidget->selectedUsers().first();
    m_socket->createChatWithClient(target.pUserID);
}



