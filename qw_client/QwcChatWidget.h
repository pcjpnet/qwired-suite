#ifndef QWCCHATWIDGET_H
#define QWCCHATWIDGET_H

#include "ui_QwcChatWidget.h"

#include "QwcUserlistModel.h"
#include "QwcUserlistDelegate.h"

#include "QwcMessageStyle.h"

class QwcSession;

class QwcChatWidget : public QWidget, public Ui_QwcChatWidget
{
    Q_OBJECT

public:
    QwcChatWidget(QWidget *parent = 0);
    ~QwcChatWidget();

    void setUserListModel(QwcUserlistModel *model);
    void setSession(QwcSession *session);

    bool eventFilter(QObject *watched, QEvent *event);

    QwcSession* session();
    int pChatID;
    bool pEmoticonsEnabled;

    void loadChatStyle(const QString &path = QString());



signals:
    /*! This signal is emitted if a entry has been double-clicked in the user list. */
    void userDoubleClicked(const QwcUserInfo user);


public slots:
    void resetForm();
    void writeToChat(QwUser &sender, QString theText, bool theEmote);
    void writeEventToChat(QString theMsg);


private:
    QwcSession *pSession;
    QColor pChatTextColor;
    QColor pChatTimeColor;
    QColor pChatEventColor;
    int pChatStyle;
    bool pChatShowTime;
    QFont pChatFont;
    QPointer<QMenu> pInviteMenu;
    void updateInviteMenu();

    /*! The ID of the user who posted chat as the last person. */
    int m_lastUserChatId;

    /*! The data of the chat style that is currently used. */
    QwcMessageStyle currentChatStyle;

private slots:
    void postChatInputText();
    void on_fUsers_doubleClicked(const QModelIndex &index);
    void on_fBtnMsg_clicked();
    void on_fBtnKick_clicked();
    void on_fBtnBan_clicked();
    void on_fBtnInfo_clicked();
    void on_fBtnChat_clicked();
    void onUserlistSelectionChanged(const QItemSelection &current, const QItemSelection &previous);
    void inviteMenuTriggered(QAction *action);
    void reloadPreferences();

};

#endif
