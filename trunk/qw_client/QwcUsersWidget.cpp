#include "QwRoom.h"
#include "QwcUsersWidget.h"
#include "QwcSocket.h"
#include "QwcUserInfo.h"

#include "QwcUserlistDelegate.h"
#include <QtGui/QListWidgetItem>

QwcUsersWidget::QwcUsersWidget(QWidget *parent) :
    QListWidget(parent)
{
    m_socket = NULL;
    m_chatId = 1; // default public chat


}

void QwcUsersWidget::setSocket(QwcSocket *socket)
{
    if (m_socket) {
        // Disconnect all signals
        disconnect(m_socket, 0, this, 0);
    }

    m_socket = socket;
    clear();
    if (socket) {
        connect(m_socket, SIGNAL(receivedUserlist(int)),
                this, SLOT(handleReceivedUserlist(int)));
        connect(m_socket, SIGNAL(userJoinedRoom(int,QwcUserInfo)),
                this, SLOT(handleUserJoined(int,QwcUserInfo)));
        connect(m_socket, SIGNAL(userLeftRoom(int,QwcUserInfo)),
                this, SLOT(handleUserLeft(int,QwcUserInfo)));
        connect(m_socket, SIGNAL(userChanged(QwcUserInfo,QwcUserInfo)),
                this, SLOT(handleUserChanged(QwcUserInfo,QwcUserInfo)));
        updateUsers();
    }


    // Setup the delegate
    QwcUserlistDelegate *delegate = new QwcUserlistDelegate(this);
    delegate->setSocket(m_socket);
    setItemDelegate(delegate);
}


QwcSocket* QwcUsersWidget::socket()
{ return m_socket; }


int QwcUsersWidget::chatId() const
{ return m_chatId; }


void QwcUsersWidget::setChatId(int id)
{
    m_chatId = id;
    updateUsers();
}


QList<QwcUserInfo> QwcUsersWidget::selectedUsers() const
{
    QList<QwcUserInfo> foundUsers;
    foreach (QListWidgetItem *item, selectedItems()) {
        if (!item) { continue; }
        QwcUserInfo userInfo = m_socket->users[item->data(Qt::UserRole).toInt()];
        foundUsers << userInfo;
    }
    return foundUsers;
}


void QwcUsersWidget::updateUsers()
{
    clear();
    QwRoom targetRoom = m_socket->rooms[m_chatId];

    foreach (int userId, targetRoom.pUsers) {
        QwcUserInfo userInfo = m_socket->users[userId];
        QListWidgetItem *item = new QListWidgetItem(this);
        item->setData(Qt::UserRole, userInfo.pUserID);
    }
}


void QwcUsersWidget::handleReceivedUserlist(int chatId)
{
    if (chatId != m_chatId) { return; }
    updateUsers();
}


void QwcUsersWidget::handleUserJoined(int chatId, const QwcUserInfo &user)
{
    if (chatId != m_chatId) { return; }
    QListWidgetItem *item = new QListWidgetItem(this);
    item->setData(Qt::UserRole, user.pUserID);
}


void QwcUsersWidget::handleUserLeft(int chatId, const QwcUserInfo &user)
{
    if (chatId != m_chatId) { return; }
    for (int i = 0; i < this->count(); i++) {
        QListWidgetItem *item = this->item(i);
        if (!item) { continue; }
        if (item->data(Qt::UserRole).toInt() == user.pUserID) {
            this->takeItem(i);
            delete item;
            return;
        }
    }
}


void QwcUsersWidget::handleUserChanged(const QwcUserInfo &oldUser, const QwcUserInfo &newUser)
{
    Q_UNUSED(newUser);
    for (int i = 0; i < this->count(); i++) {
        QListWidgetItem *item = this->item(i);
        if (!item) { continue; }
        if (item->data(Qt::UserRole).toInt() == oldUser.pUserID) {
            update(indexFromItem(item));
            return;
        }
    }
}
