#ifndef QWCACCOUNTSWIDGET_H
#define QWCACCOUNTSWIDGET_H

//#include <QWidget>
#include "ui_QwcAccountsWidget.h"
#include "QwcUserInfo.h"


class QwcAccountsWidget : public QWidget, public Ui_QwcAccountsWidget
{
    Q_OBJECT

public:
    QwcAccountsWidget(QWidget *parent = 0);

private:
    void setupEditWidgets();
    QwcUserInfo currentAccount;
    bool newAccountMode;


//    void enableGui(bool);


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


public slots:
    void appendUserNames(QStringList);
    void appendGroupNames(QStringList);
    void loadFromAccount(const QwcUserInfo account);

signals:
    void userSpecRequested(QString theName);
    void groupSpecRequested(QString theName);
    void saveUserSpec(QwcUserInfo);
    void saveGroupSpec(QwcUserInfo);


    void accountCreated(QwcUserInfo);
    void accountEdited(QwcUserInfo);
    void accountDeleted(QString);

    void groupCreated(QwcUserInfo);
    void groupEdited(QwcUserInfo);
    void groupDeleted(QString);

    /*! Emitted when the user wants to refresh the list of available groups and users. */
    void refreshedAccountsAndGroups();

};

#endif
