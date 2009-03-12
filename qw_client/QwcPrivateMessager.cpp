#include "QwcPrivateMessager.h"

/*! \class QwcPrivateMessager
    \author Bastian Bense <bastibense@gmail.com>
    \date 2009-03-06
    \brief This is the private message manager (messager) which allows users to send private messages
    to each other without having to initiate a private chat session.
*/

#include <QtDebug>
#include <QGraphicsItemAnimation>


// Session intialization
QwcPrivateMessagerSession::QwcPrivateMessagerSession()
{
    unreadCount = 0;
    document = NULL;
}

// Item Delegate
//
QwcPrivateMessageListDelegate::QwcPrivateMessageListDelegate(QObject *parent) : QItemDelegate(parent)
{
}

void QwcPrivateMessageListDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    painter->save();
    painter->translate(option.rect.topLeft());
    QRect itemRect(QPoint(0,0), option.rect.size());

    if (!index.data(Qt::UserRole).canConvert<QwcPrivateMessagerSession>()) { return; }
    painter->setRenderHint(QPainter::Antialiasing);

    QwcPrivateMessagerSession session = index.data(Qt::UserRole).value<QwcPrivateMessagerSession>();

    QSize iconSize(itemRect.height()-4, itemRect.height()-4);
    QPixmap userIcon = session.userInfo.iconAsPixmap().scaled(iconSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    // Nice Background
    painter->save();
    painter->setClipRect(itemRect);
    painter->translate(0, itemRect.height()/2);
    painter->scale(8, 8);
    painter->translate(0, -iconSize.height()*0.5);
    painter->setOpacity(0.2);
    painter->drawPixmap(0, 0, userIcon);
    painter->restore();

    // Draw the icon
    painter->drawPixmap(2, 2, userIcon);

    // Draw the user nickname and status
    QFont font = painter->font();

    // Nickname
    painter->save();
    painter->translate(iconSize.width()+8, itemRect.height()/2);

    font.setBold(true);
    font.setPixelSize(12);
    painter->setFont(font);
    painter->drawText(0, -2, session.userInfo.pNick);

    // Status
    font.setBold(false);
    font.setPixelSize(10);
    painter->setFont(font);
    painter->drawText(0, painter->fontMetrics().ascent()+1, session.userInfo.pStatus);
    painter->restore();

    // Unread Count
    if (session.unreadCount) {
        painter->save();
        QRect badgeRect(0, 0, iconSize.width(), iconSize.height()*0.5);
        font.setPixelSize(badgeRect.height()*0.8);
        font.setBold(true);
        painter->translate(itemRect.width()-iconSize.width()-8, itemRect.height()/2 - badgeRect.height()/2);
        painter->setFont(font);
        painter->setPen(QColor(Qt::red).darker(130));
        painter->setBrush(Qt::red);
        painter->drawRoundedRect(badgeRect, 3, 3);
        painter->setOpacity(1);
        painter->setPen(QPen(Qt::white));
        painter->drawText(badgeRect, Qt::AlignHCenter | Qt::AlignVCenter, QString::number(session.unreadCount));
        painter->restore();
    }

    // Option highlight
    if (option.state & QStyle::State_Selected) {
        painter->save();
        painter->setPen(Qt::NoPen);
        painter->setOpacity(0.3);
        painter->setBrush(option.palette.color(QPalette::Highlight));
        painter->drawRect(itemRect);
        painter->restore();
    }

    painter->restore();
}


QSize QwcPrivateMessageListDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    return QSize(160, 36);
};



// Messenger Widget
//
QwcPrivateMessager::QwcPrivateMessager(QWidget *parent) : QWidget(parent)
{
    setupUi(this);
    fMessageList->setItemDelegate(new QwcPrivateMessageListDelegate(fMessageList));
    fMessageInput->installEventFilter(this);
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
    appendMessageToCurrentSession(targetDocument, message, Qt::green);

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

    if (currentRow == -1) {
        // Nothing selected or list is empty now
        fMessageInput->clear();
        fMessageView->clear();
    } else {
        // Display the selected session
        QwcPrivateMessagerSession session = fMessageList->currentItem()->data(Qt::UserRole).value<QwcPrivateMessagerSession>();
        if (session.document) {
            session.unreadCount = 0;
            fMessageList->currentItem()->setData(Qt::UserRole, QVariant::fromValue(session));
            fMessageView->setDocument(session.document);
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
