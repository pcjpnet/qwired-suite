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
    unreadCount = 4;
}

// Item Delegate
//
QwcPrivateMessageListDelegate::QwcPrivateMessageListDelegate(QObject *parent) : QItemDelegate(parent)
{

}

void QwcPrivateMessageListDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if (!index.data(Qt::UserRole).canConvert<QwcPrivateMessagerSession>()) { return; }
    painter->setRenderHint(QPainter::Antialiasing);

    QwcPrivateMessagerSession session = index.data(Qt::UserRole).value<QwcPrivateMessagerSession>();

    QSize iconSize(option.rect.height()-4, option.rect.height()-4);
    QPixmap userIcon = session.userInfo.iconAsPixmap().scaled(iconSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    // Nice Background
    painter->save();
    painter->setClipRect(option.rect);
    painter->translate(0, option.rect.height()/2);
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
    painter->translate(iconSize.width()+8, option.rect.height()/2);

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

        //painter->setOpacity(0.8);
        painter->translate(option.rect.width()-iconSize.width()-8, option.rect.height()/2 - badgeRect.height()/2);
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
    if (option.state % QStyle::State_Selected) {
        painter->save();
        painter->setPen(Qt::NoPen);
        painter->setOpacity(0.3);
        painter->setBrush(option.palette.color(QPalette::Highlight));
        painter->drawRect(option.rect);
        painter->restore();
    }

}

QSize QwcPrivateMessageListDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{

    return QSize(200, 36);
};



// Messenger Widget
//
QwcPrivateMessager::QwcPrivateMessager(QWidget *parent) : QWidget(parent)
{
    setupUi(this);
    fMessageList->setItemDelegate(new QwcPrivateMessageListDelegate(fMessageList));



}


/*! Handle a new incoming message and add it to the list of messages and/or notify the user.
*/
void QwcPrivateMessager::handleNewMessage(const QwcUserInfo &sender, const QString message)
{
    qDebug() << "Handling message...";

    QTextDocument *targetDocument = NULL;

    if (messageSessions.contains(sender.pUserID)) {
        // There is a messageSession - we should add the new message and update the icon in the
        // conversation list.
        QListWidgetItem *currentItem = fMessageList->currentItem();
        if (!currentItem) { return; }

        QwcPrivateMessagerSession session = currentItem->data(Qt::UserRole).value<QwcPrivateMessagerSession>();
        targetDocument = session.document;

    } else {
        // There is no messageSession for the user yet. Create a new one.
        QwcPrivateMessagerSession session;
        targetDocument = new QTextDocument(this);
        session.document = targetDocument;
        session.userInfo = sender;

        QListWidgetItem *item = new QListWidgetItem(fMessageList);
        item->setText(sender.pNick);
        item->setData(Qt::UserRole, QVariant::fromValue(session));
    }

    // Add the new message to the log
    if (!targetDocument) { return; }
    QTextCursor cursor(targetDocument);
    cursor.movePosition(QTextCursor::End);
    cursor.insertHtml(QString("<b>%1:</b> %2").arg(sender.pNick).arg(message));

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
        int sessionId = fMessageList->currentItem()->data(Qt::UserRole).toInt();
        QTextDocument *targetDocument = messageSessions[sessionId].document;
        if (targetDocument) {
            fMessageView->setDocument(targetDocument);
        }
    }
}
