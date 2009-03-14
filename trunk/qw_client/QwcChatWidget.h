#ifndef QWCCHATWIDGET_H
#define QWCCHATWIDGET_H

#include "ui_QwcChatWidget.h"

#include "QwcUserlistModel.h"
#include "QwcUserlistDelegate.h"


class QwcSession;

class QwcChatWidget : public QWidget, public Ui_QwcChatWidget
{
    Q_OBJECT

public:
    QwcChatWidget(QWidget *parent = 0);
    ~QwcChatWidget();

    void setUserListModel(QwcUserlistModel *model);
    void setSession(QwcSession *session);

    QwcSession* session();
    int pChatID;
    bool pEmoticonsEnabled;

signals:
    /*! This signal is emitted if a entry has been double-clicked in the user list. */
    void userDoubleClicked(const QwcUserInfo user);


protected:
    bool eventFilter(QObject *watched, QEvent *event);


public slots:
    void resetForm();
    void writeToChat(QString theUser, QString theText, bool theEmote);
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

private slots:
    void postChatInputText();
    void on_fUsers_doubleClicked ( const QModelIndex & index );
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
