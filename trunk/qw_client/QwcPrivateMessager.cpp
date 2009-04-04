#include "QwcPrivateMessager.h"
#include "QwcUserlistDelegate.h"

/*! \class QwcPrivateMessager
    \author Bastian Bense <bastibense@gmail.com>
    \date 2009-03-06
    \brief This is the private message manager (messager) which allows users to send private messages
    to each other without having to initiate a private chat session.
*/

#include <QtDebug>
#include <QSettings>
#include <QTextFrame>
#include <QFileDialog>
#include <QGraphicsItemAnimation>
#include <QMessageBox>


// Session intialization
QwcPrivateMessagerSession::QwcPrivateMessagerSession()
{
    inactive = false;
    unreadCount = 0;
    document = NULL;
}

// Messenger Widget
//
QwcPrivateMessager::QwcPrivateMessager(QWidget *parent) : QWidget(parent)
{
    setupUi(this);
    fMessageList->setItemDelegate(new QwcUserlistDelegate(fMessageList));
    fMessageInput->installEventFilter(this);

    // Set up the splitter
    splitterVertical->setStretchFactor(0, 4);
    splitterVertical->setStretchFactor(1, 10);

    // Restore the splitter
    QSettings settings;
    splitterVertical->restoreState(settings.value("messager/splitterVertical", splitterVertical->saveState()).toByteArray());
    splitterHorizontal->restoreState(settings.value("messager/splitterHorizontal", splitterHorizontal->saveState()).toByteArray());

}


QwcPrivateMessager::~QwcPrivateMessager()
{
    QSettings settings;
    settings.setValue("messager/splitterVertical", splitterVertical->saveState());
    settings.setValue("messager/splitterHorizontal", splitterHorizontal->saveState());
}


/*! This method is called automatically and we use it to strip the return and enter key from the
    message input field.
*/
bool QwcPrivateMessager::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == fMessageInput) {
        if (event->type() == QEvent::KeyPress) {
            QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
            if (!keyEvent) { return false; }
            if (keyEvent->key() == Qt::Key_Return) {

                // Check if the current item is a valid one
                QwcPrivateMessagerSession session = fMessageList->currentItem()->data(Qt::UserRole).value<QwcPrivateMessagerSession>();
                if (!session.document) { return false; }
                if (fMessageInput->toPlainText().isEmpty()) { return true; }

                // Append it to the view
                if (appendMessageToCurrentSession(session.document, fMessageInput->toPlainText(), Qt::blue)) {
                    // Scroll down
                    fMessageView->setTextCursor(fMessageView->document()->rootFrame()->lastCursorPosition());
                    fMessageView->ensureCursorVisible();
                    // Let other parts of the program know that the user entered a message
                    emit enteredNewMessage(session.userInfo.pUserID, fMessageInput->toPlainText());
                    fMessageInput->clear();
                    return true;
                }

            }
        }
    }

    return false;
}


/*! Handle a new incoming message and add it to the list of messages and/or notify the user.
*/
void QwcPrivateMessager::handleNewMessage(const QwcUserInfo &sender, const QString message)
{
    QListWidgetItem *targetListItem = NULL;
    QTextDocument *targetDocument = NULL;

    // Try to find an existing session
    for (int i=0; i<fMessageList->count(); i++) {
        QListWidgetItem *item = fMessageList->item(i);
        if (!item) { continue; }

        QwcPrivateMessagerSession itemSession = item->data(Qt::UserRole).value<QwcPrivateMessagerSession>();
        if (itemSession.userInfo.pUserID != sender.pUserID) { continue; }

        targetListItem = item;
        targetDocument = itemSession.document;

        // If the target session is not the current one, increment the unread counter to display
        // the event to the user.
        if (i != fMessageList->currentRow()) {
            itemSession.unreadCount += 1;
            item->setData(Qt::UserRole, QVariant::fromValue(itemSession));
        }
    }

    // Check if we need to create a new session
    if (!targetDocument) {
        QwcPrivateMessagerSession session;
        targetDocument = new QTextDocument(fMessageList);
        session.document = targetDocument;
        session.unreadCount = 1;
        session.userInfo = sender;

        targetListItem = new QListWidgetItem(fMessageList);
        targetListItem->setData(Qt::UserRole, QVariant::fromValue(session));
        fMessageList->setCurrentItem(targetListItem);
    }

    // Add the data
    if (!message.isNull()) {
        appendMessageToCurrentSession(targetDocument, message, Qt::green);
    } else {
        // Set the focus, because if the message is a null string, the user probably wants to
        // send a new message.
        fMessageInput->setFocus();
    }

    // If this is the current item, scroll down
    if (fMessageList->currentItem() == targetListItem) {
         fMessageView->setTextCursor(fMessageView->document()->rootFrame()->lastCursorPosition());
         fMessageView->ensureCursorVisible();
    }

}


/*! The current row changed - display the message transcript for the selected session.
*/
void QwcPrivateMessager::on_fMessageList_currentRowChanged(int currentRow)
{
    btnRemoveSession->setEnabled(currentRow != -1);
    btnSaveSession->setEnabled(currentRow != -1);
    fMessageList->setEnabled(currentRow != -1);

    if (currentRow == -1) {
        // Nothing selected or list is empty now
        fMessageInput->clear();
        fMessageView->clear();
        fMessageInput->setEnabled(false);
    } else {
        // Display the selected session
        QwcPrivateMessagerSession session = fMessageList->currentItem()->data(Qt::UserRole).value<QwcPrivateMessagerSession>();
        if (session.document) {
            session.unreadCount = 0;
            fMessageList->currentItem()->setData(Qt::UserRole, QVariant::fromValue(session));
            fMessageView->setDocument(session.document);

            fMessageInput->setEnabled(!session.inactive);
        }
    }
}


/*! Append a new message to the current session and scroll to the end of the message protocol and
    returns true if the action was successful. The \a messageColor defines the color of the message
    box and should be provided as a full-color (no dimming required) color, as it is automatically
    adjusted for the border and background.
*/
bool QwcPrivateMessager::appendMessageToCurrentSession(QTextDocument *document, const QString message, const QColor messageColor)
{
    if (!document) { return false; }

    QTextCursor cursor = document->rootFrame()->lastCursorPosition();
    cursor.movePosition(QTextCursor::StartOfBlock);

    QTextFrameFormat frameFormat;
    frameFormat.setPadding(4);
    frameFormat.setBackground(messageColor.lighter(190));
    frameFormat.setMargin(0);
    frameFormat.setBorder(2);
    frameFormat.setBorderBrush(messageColor.lighter(150));
    frameFormat.setBorderStyle(QTextFrameFormat::BorderStyle_Outset);

    // Title
    QTextCharFormat backupCharFormat = cursor.charFormat();

    QTextCharFormat newCharFormat;
    newCharFormat.setFontPointSize(9);

    QTextBlockFormat headerFormat;
    headerFormat.setAlignment(Qt::AlignHCenter);
    cursor.insertBlock(headerFormat);
    cursor.setCharFormat(newCharFormat);
    cursor.insertText(QDateTime::currentDateTime().toString());

    QTextFrame *frame = cursor.insertFrame(frameFormat);
    cursor.setCharFormat(backupCharFormat);
    frame->firstCursorPosition().insertText(message);

    return true;
}


/*! Delete the current message session and remove it fromt he list.
*/
void QwcPrivateMessager::on_btnRemoveSession_clicked()
{
    if (fMessageList->currentRow() == -1) { return; }

    QwcPrivateMessagerSession session = fMessageList->currentItem()->data(Qt::UserRole).value<QwcPrivateMessagerSession>();
    if (session.document) {
        fMessageView->setDocument(0);
        fMessageList->takeItem(fMessageList->currentRow());
        session.document->deleteLater();
    }
}


/*! Save the contents of the message session to a HTML file.
*/
void QwcPrivateMessager::on_btnSaveSession_clicked()
{
    QTextDocument *document = fMessageView->document();
    if (!document) { return; }

    QString targetFile = QFileDialog::getSaveFileName(this, tr("Save message log as HTML file"), QDir::homePath(), tr("HTML file (*.html)"));
    if (targetFile.isEmpty()) { return; }

    QFile outputFile(targetFile);
    if (!outputFile.open(QIODevice::WriteOnly)) {
        QMessageBox::warning(this, tr("Unable to save message log"), tr("An error occourred while "
                    "attempting to write the message log file (%1)").arg(outputFile.errorString()));
        return;
    }

    outputFile.write(document->toHtml().toUtf8());
    outputFile.close();
}


/*! Something about a user changed - handle the change here and update the user list.
*/
void QwcPrivateMessager::handleUserChanged(QwcUserInfo previous, QwcUserInfo current)
{
    for (int i=0; i<fMessageList->count(); i++) {
        QListWidgetItem *item = fMessageList->item(i);
        if (!item) { continue; }

        QwcPrivateMessagerSession itemSession = item->data(Qt::UserRole).value<QwcPrivateMessagerSession>();
        if (itemSession.userInfo.pUserID != current.pUserID) { continue; }

        itemSession.userInfo = current;
        item->setData(Qt::UserRole, QVariant::fromValue(itemSession));
        fMessageList->update();
        return;
    }
}


/*! A user logged off or became invalid. We should disable some features such as sending messages
    and change the user's nickname to reflect the fact.
*/
void QwcPrivateMessager::handleUserLeft(int chatId, QwcUserInfo user)
{
    for (int i=0; i<fMessageList->count(); i++) {
        QListWidgetItem *item = fMessageList->item(i);
        if (!item) { continue; }

        QwcPrivateMessagerSession itemSession = item->data(Qt::UserRole).value<QwcPrivateMessagerSession>();
        if (itemSession.userInfo.pUserID != user.pUserID) { continue; }
        itemSession.inactive = true;
        itemSession.userInfo.userNickname = tr("%1 [user left]").arg(itemSession.userInfo.userNickname);
        item->setData(Qt::UserRole, QVariant::fromValue(itemSession));
        fMessageList->update();

        if (item == fMessageList->currentItem()) {
            fMessageInput->setEnabled(false);
        }

        return;
    }
}
