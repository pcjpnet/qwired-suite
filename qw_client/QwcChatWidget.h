#ifndef QWCCHATWIDGET_H
#define QWCCHATWIDGET_H

#include "ui_QwcChatWidget.h"
#include "QwcUserlistDelegate.h"
#include "QwcMessageStyle.h"

#include <QtCore/QPointer>

class QwcSession;

class QwcChatWidget :
        public QWidget,
        protected Ui_QwcChatWidget
{
    Q_OBJECT

public:
    QwcChatWidget(QWidget *parent = 0);
    ~QwcChatWidget();

    void setSession(QwcSession *session);
    bool eventFilter(QObject *watched, QEvent *event);

    QwcSession* session();

    void setChatId(int newId);
    int chatId() const;


    void loadChatStyle(const QString &path = QString());



signals:
    /*! This signal is emitted if a entry has been double-clicked in the user list. */
    void userDoubleClicked(const QwcUserInfo user);


public slots:
    void resetForm();
    void writeTextToChat(QwUser sender, QString text, bool emote);
    void writeEventToChat(QString eventText, QString eventType);
    void writeBroadcastToChat(QwcUserInfo sender, QString text);
    void setTopic(const QString &text, const QString &user, const QDateTime &dateTime);


private:
    QwcSession *m_session;
    int m_chatId;

    QPointer<QMenu> pInviteMenu;
    void updateInviteMenu();

    /*! The currently used message style for this chat. */
    QwcMessageStyle m_currentMessageStyle;

private slots:
    void handleExternalLinkClicked(const QUrl &url);

    void processChatInput();
    void on_fUsers_doubleClicked(const QModelIndex &index);
    void on_fBtnMsg_clicked();
    void on_fBtnKick_clicked();
    void on_fBtnBan_clicked();
    void on_fBtnInfo_clicked();
    void on_fBtnChat_clicked();
    void onUserlistSelectionChanged(const QItemSelection &current, const QItemSelection &previous);
    void inviteMenuTriggered(QAction *action);
    void reloadPreferences();

    void handleChatViewFrameSizeChanged(QSize size);

};

#endif
