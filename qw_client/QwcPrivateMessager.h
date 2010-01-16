#ifndef QWCPRIVATEMESSAGER_H
#define QWCPRIVATEMESSAGER_H

#include "ui_QwcPrivateMessager.h"
#include "QwcUserInfo.h"

#include <QtGui/QWidget>

class QwcSocket;

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
class QwcPrivateMessager :
        public QWidget,
        protected Ui_QwcPrivateMessager
{
    Q_OBJECT

public:
    QwcPrivateMessager(QWidget *parent=0);
    ~QwcPrivateMessager();

    void setSocket(QwcSocket *socket);
    QwcSocket* socket();

public slots:
    void handleNewMessage(const QwcUserInfo &sender, const QString message);
    void handleUserChanged(QwcUserInfo previous, QwcUserInfo current);
    void handleUserLeft(int chatId, QwcUserInfo user);

private slots:
    void on_fMessageList_currentRowChanged(int currentRow);
    void on_btnRemoveSession_clicked();
    void on_btnSaveSession_clicked();


private:
    QwcSocket *m_socket;
    bool appendMessageToCurrentSession(QTextDocument *document, const QString message,
                                       const QColor messageColor = Qt::gray);

protected:
    bool eventFilter(QObject *watched, QEvent *event);

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
