#ifndef QWCPRIVATEMESSAGER_H
#define QWCPRIVATEMESSAGER_H

#include "QwcUserInfo.h"
#include "ui_QwcPrivateMessager.h"
#include <QWidget>


/*! The session contains the QTextDocument and user information for a specific conversation (session)
    in the messenger. It is passed using the QVariant mechanism to provide the information for the
    delegate.
*/
class QwcPrivateMessagerSession
{
public:
    QwcPrivateMessagerSession();
    int unreadCount;
    QwcUserInfo userInfo;
    QTextDocument *document;
};
Q_DECLARE_METATYPE(QwcPrivateMessagerSession);



/*! This delegate is responsible for drawing the user list in the messenger window.
*/
class QwcPrivateMessageListDelegate : public QItemDelegate
{
Q_OBJECT
public:
    QwcPrivateMessageListDelegate(QObject *parent=0);
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;
};



/*! This is the main messenger widget. It is responsible for the user interaction.
*/
class QwcPrivateMessager : public QWidget, private Ui_QwcPrivateMessager
{
Q_OBJECT
public:
    QwcPrivateMessager(QWidget *parent=0);
    bool eventFilter(QObject *watched, QEvent *event);

private:
    QGraphicsScene *graphicsScene;
    QMap<int,QwcPrivateMessagerSession> messageSessions;

private slots:
    void on_fMessageList_currentRowChanged(int currentRow);

public slots:
    void handleNewMessage(const QwcUserInfo &sender, const QString message);

signals:
    void enteredNewMessage(const int targetUserId, const QString message);
};

#endif // QWCPRIVATEMESSAGER_H
