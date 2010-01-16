#ifndef QWCACCOUNTSWIDGET_H
#define QWCACCOUNTSWIDGET_H

#include "ui_QwcAccountsWidget.h"
#include "QwcUserInfo.h"

class QwcSocket;

class QwcAccountsWidget :
        public QWidget,
        protected Ui_QwcAccountsWidget
{
    Q_OBJECT

public:
    QwcAccountsWidget(QWidget *parent = 0);

    void setSocket(QwcSocket *socket);
    QwcSocket* socket();

private:
    QwcSocket *m_socket;
    void setupEditWidgets();
    QwcUserInfo currentAccount;
    bool newAccountMode;

private slots:
    void on_btnCreateAccount_clicked();

    void on_fList_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);
    void on_fBtnApply_clicked();
    void on_btnRefreshAccounts_clicked();
    void on_btnEditDelete_clicked();

    void on_fAccountType_currentIndexChanged(int);
    void on_fGroup_currentIndexChanged(int);

    void on_btnEditCancel_clicked();
    void on_btnEditAccount_clicked();
    void on_btnDeleteAccount_clicked();

    void appendUserNames(QStringList);
    void appendGroupNames(QStringList);
    void loadFromAccount(const QwcUserInfo account);

};

#endif
