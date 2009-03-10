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
                // Add to the document
                if (fMessageList->currentRow() == -1) { return false; }
                QwcPrivateMessagerSession session = fMessageList->currentItem()->data(Qt::UserRole).value<QwcPrivateMessagerSession>();
                if (session.document) {
                    qDebug() << "send message";

                    QTextCursor cursor = session.document->rootFrame()->lastCursorPosition();

                    cursor.movePosition(QTextCursor::StartOfBlock);



                    QTextFrameFormat frameFormat;
                    frameFormat.setPadding(6);
                    frameFormat.setBackground(QColor(Qt::gray).lighter());
                    frameFormat.setMargin(0);
                    frameFormat.setBorder(2);
                    frameFormat.setBorderBrush(QColor(Qt::gray));
                    frameFormat.setBackground(QColor(Qt::gray).lighter());

                    // Title
                    QTextBlockFormat headerFormat;
                    headerFormat.setAlignment(Qt::AlignHCenter);
                    cursor.insertBlock(headerFormat);
                    cursor.insertText(QDateTime::currentDateTime().toString());

                    QTextFrame *frame = cursor.insertFrame(frameFormat);

                    frame->firstCursorPosition().insertText(fMessageInput->toPlainText());


                    //cursor.insertBlock(blockFormat);

                    //QTextCharFormat format = cursor.charFormat();
                    //format.setBackground(Qt::lightGray);
                    //format.setFontWeight(QFont::Bold);
                    //cursor.setCharFormat(format);


                    //cursor.insertHtml(fMessageInput->toPlainText());

                    //cursor.insertHtml(QString("<br><div class=\"msg_out\">%1</div>").arg());
                    emit enteredNewMessage(session.userInfo.pUserID, fMessageInput->toPlainText());
                    fMessageInput->clear();


                    fMessageView->ensureCursorVisible();
                }
                return true;
            }
        }
    }

    return false;
}


/*! Handle a new incoming message and add it to the list of messages and/or notify the user.
*/
void QwcPrivateMessager::handleNewMessage(const QwcUserInfo &sender, const QString message)
{
    qDebug() << "Handling message...";
    QTextDocument *targetDocument = NULL;

    // Try to find an existing session
    for (int i=0; i<fMessageList->count(); i++) {
        QListWidgetItem *item = fMessageList->item(i);
        if (!item) { continue; }

        QwcPrivateMessagerSession itemSession = item->data(Qt::UserRole).value<QwcPrivateMessagerSession>();
        if (itemSession.userInfo.pUserID != sender.pUserID) { continue; }
        targetDocument = itemSession.document;

        if (i != fMessageList->currentRow()) {
            itemSession.unreadCount += 1;
            item->setData(Qt::UserRole, QVariant::fromValue(itemSession));
        }
    }

    // Check if we need to create a new session
    if (!targetDocument) {
        QwcPrivateMessagerSession session;
        targetDocument = new QTextDocument(fMessageList);
        targetDocument->setDefaultStyleSheet(".msg_in { background-color: #DDF; }  .msg_out { background-color: #FDD; }");
        session.document = targetDocument;
        session.unreadCount = 1;
        session.userInfo = sender;

        QListWidgetItem *item = new QListWidgetItem(fMessageList);
        item->setData(Qt::UserRole, QVariant::fromValue(session));
    }

    // Add the data
    if (targetDocument) {
        QTextCursor cursor = targetDocument->rootFrame()->lastCursorPosition();
        cursor.insertHtml(QString("<div class=\"msg_in\">%1</div>").arg(fMessageInput->toPlainText()));
        /*
QTextCursor cursor(targetDocument);
        cursor.movePosition(QTextCursor::End);

        QTextBlockFormat blockFormat;
        blockFormat.setTopMargin(4);
        blockFormat.setLeftMargin(4);
        blockFormat.setRightMargin(4);
        blockFormat.setBottomMargin(4);
        blockFormat.setBackground(QColor(Qt::gray).lighter(150));
        cursor.insertBlock(blockFormat);
        cursor.insertText(message);
        */
    }
}


/*! The current row changed - display the message transcript for the selected session.
*/
void QwcPrivateMessager::on_fMessageList_currentRowChanged(int currentRow)
{
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
