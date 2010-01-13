#ifndef QWCPRIVATEMESSAGER_H
#define QWCPRIVATEMESSAGER_H

#include "QwcUserInfo.h"
#include "ui_QwcPrivateMessager.h"
#include <QWidget>
#include <QGraphicsScene>


/*! The session contains the QTextDocument and user information for a specific conversation (session)
    in the messenger. It is passed using the QVariant mechanism to provide the information for the
    delegate.
*/
class QwcPrivateMessagerSession
{

public:
    QwcPrivateMessagerSession();
    int unreadCount;
    /*! If true, the user has logged off or can't be messaged anymore. */
    bool inactive;
    QwcUserInfo userInfo;
    QTextDocument *document;

};

Q_DECLARE_METATYPE(QwcPrivateMessagerSession);



/*! This is the main messenger widget. It is responsible for the user interaction.
*/
class QwcPrivateMessager : public QWidget, private Ui_QwcPrivateMessager
{
    Q_OBJECT

public:
    QwcPrivateMessager(QWidget *parent=0);
    ~QwcPrivateMessager();
    bool eventFilter(QObject *watched, QEvent *event);

private:
    QGraphicsScene *graphicsScene;
    bool appendMessageToCurrentSession(QTextDocument *document, const QString message, const QColor messageColor=Qt::gray);

private slots:
    void on_fMessageList_currentRowChanged(int currentRow);
    void on_btnRemoveSession_clicked();
    void on_btnSaveSession_clicked();


public slots:
    void handleNewMessage(const QwcUserInfo &sender, const QString message);
    void handleUserChanged(QwcUserInfo previous, QwcUserInfo current);
    void handleUserLeft(int chatId, QwcUserInfo user);

signals:
    void enteredNewMessage(const int targetUserId, const QString message);
};

//In our darkest hour,
//In my deepest despair,
//Will you still care?
//Will you be there?
//
//In my trials
//And my tribulations
//Through our doubts
//And frustrations
//
//In my violence
//In my turbulence
//Through my fear
//And my connfesssions
//
//In my anguish and my pain
//Through my joy and my sorrow
//In the promise of another tomorrow
//I’ll never let you apart
//For you’re always in my heart

#endif // QWCPRIVATEMESSAGER_H
