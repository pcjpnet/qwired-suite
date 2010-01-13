#ifndef QWCUSERSWIDGET_H
#define QWCUSERSWIDGET_H

#include "QwcUserInfo.h"

#include <QtGui/QListWidget>

class QwcSocket;

class QwcUsersWidget :
        public QListWidget
{
    Q_OBJECT

public:
    explicit QwcUsersWidget(QWidget *parent = 0);

    void setSocket(QwcSocket *socket);
    QwcSocket* socket();

    int chatId() const;
    void setChatId(int id);

    QList<QwcUserInfo> selectedUsers() const;

    void updateUsers();

private slots:
    void handleReceivedUserlist(int chatId);
    void handleUserJoined(int chatId, const QwcUserInfo &user);
    void handleUserLeft(int chatId, const QwcUserInfo &user);
    void handleUserChanged(const QwcUserInfo &oldUser, const QwcUserInfo &newUser);


protected:
    QwcSocket *m_socket;
    int m_chatId;


};

#endif
