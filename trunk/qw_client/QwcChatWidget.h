#ifndef QWCCHATWIDGET_H
#define QWCCHATWIDGET_H

#include "ui_QwcChatWidget.h"
#include "QwcUserlistDelegate.h"
#include "QwcMessageStyle.h"

#include <QtCore/QPointer>

class QwcSocket;

class QwcChatWidget :
        public QWidget,
        protected Ui_QwcChatWidget
{
    Q_OBJECT

public:
    QwcChatWidget(QWidget *parent = 0);
    ~QwcChatWidget();

    void setSocket(QwcSocket *socket);
    QwcSocket* socket();

    bool eventFilter(QObject *watched, QEvent *event);


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
    void handleSocketBroadcastMessage(QwcUserInfo sender, QString text);
    void handleSocketChatMessage(int chatId, int userId, const QString &text, bool isEmote);

private:
    QwcSocket *m_socket;
    int m_chatId;

    QPointer<QMenu> pInviteMenu;
    void updateInviteMenu();

    /*! The currently used message style for this chat. */
    QwcMessageStyle m_currentMessageStyle;

private slots:
    void handleSocketChatTopic(int chatId, const QString &nickname, const QString &login, QHostAddress ip,
                               QDateTime dateTime, const QString &topic);

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
