#ifndef QWCPRIVATEMESSAGER_H
#define QWCPRIVATEMESSAGER_H

#include "QwcUserInfo.h"
#include "ui_QwcPrivateMessager.h"
#include <QWidget>


class QwcPrivateMessagerSession
{

public:
    QwcPrivateMessagerSession();
    int unreadCount;
    QwcUserInfo userInfo;
    QTextDocument *document;

};
Q_DECLARE_METATYPE(QwcPrivateMessagerSession);


class QwcPrivateMessageListDelegate : public QItemDelegate
{
    Q_OBJECT

public:
    QwcPrivateMessageListDelegate(QObject *parent=0);
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;
};



class QwcPrivateMessager : public QWidget, private Ui_QwcPrivateMessager
{
    Q_OBJECT

public:
    QwcPrivateMessager(QWidget *parent=0);

private:
    QGraphicsScene *graphicsScene;
    QMap<int,QwcPrivateMessagerSession> messageSessions;

private slots:
    void on_fMessageList_currentRowChanged(int currentRow);

public slots:
    void handleNewMessage(const QwcUserInfo &sender, const QString message);



};

#endif // QWCPRIVATEMESSAGER_H
