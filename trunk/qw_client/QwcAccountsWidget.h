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
    QwcUserInfo pCurrentUser;
    bool newAccountMode;

    void enableGui(bool);
    void setPrivFlags(const QwcUserInfo);

private slots:
    void on_fList_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);
    void on_fBtnApply_clicked();
    void on_fBtnDelete_clicked();
    void on_fBtnNew_clicked();
    void on_fType_currentIndexChanged(int);
    void on_fGroup_currentIndexChanged(int);

public slots:
    void appendUserNames(QStringList);
    void appendGroupNames(QStringList);
    void loadUserSpec(QwcUserInfo);
    void loadGroupSpec(QwcUserInfo);

signals:
    void userSpecRequested(QString theName);
    void groupSpecRequested(QString theName);
    void saveUserSpec(QwcUserInfo);
    void saveGroupSpec(QwcUserInfo);
    void userDeleted(QString);
    void groupDeleted(QString);
    void createUser(QwcUserInfo);
    void editUser(QwcUserInfo);
    void createGroup(QwcUserInfo);
    void editGroup(QwcUserInfo);


};

#endif
