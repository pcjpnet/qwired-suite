/*! \class QwcAccountsWidget
    \author Bastian Bense <bastibense@gmail.com>
    \date 2009-03-06

    This is the account administration widget for the Qwired client.
*/


#include "QwcAccountsWidget.h"
#include "QwcSocket.h"

#include <QtGui/QMessageBox>


QwcAccountsWidget::QwcAccountsWidget(QWidget *parent) : QWidget(parent)
{
    setAttribute(Qt::WA_DeleteOnClose);
    setupUi(this);
    m_socket = NULL;
    newAccountMode = false;
    fGroupBasic->setEnabled(false);
    fGroupFiles->setEnabled(false);
    fGroupAdmin->setEnabled(false);
    fGroupLimits->setEnabled(false);
    // Show the list by default
    stackedWidget->setCurrentIndex(1);
}


void QwcAccountsWidget::setSocket(QwcSocket *socket)
{
    if (m_socket) { disconnect(m_socket, 0, this, 0); }
    m_socket = socket;
    if (!socket) { return; }

    connect(m_socket, SIGNAL(receivedAccountList(QStringList)),
            this, SLOT(appendUserNames(QStringList)));
    connect(m_socket, SIGNAL(receivedAccountGroupList(QStringList)),
            this, SLOT(appendGroupNames(QStringList)));

    connect(m_socket, SIGNAL(userSpecReceived(QwcUserInfo)),
            this, SLOT(loadFromAccount(QwcUserInfo)));
    connect(m_socket, SIGNAL(groupSpecReceived(QwcUserInfo)),
            this, SLOT(loadFromAccount(QwcUserInfo)));

//    connect(m_accountsWidget, SIGNAL(userSpecRequested(QString)),
//            m_socket, SLOT(readUser(QString)));
//    connect(m_accountsWidget, SIGNAL(groupSpecRequested(QString)),
//            m_socket, SLOT(readGroup(QString)));
//
//
//    connect(m_accountsWidget, SIGNAL(accountCreated(QwcUserInfo)),
//            m_socket, SLOT(createUser(QwcUserInfo)));
//    connect(m_accountsWidget, SIGNAL(accountEdited(QwcUserInfo)),
//            m_socket, SLOT(editUser(QwcUserInfo)));
//    connect(m_accountsWidget, SIGNAL(accountDeleted(QString)),
//            m_socket, SLOT(deleteUser(QString)));
//
//    connect(m_accountsWidget, SIGNAL(groupCreated(QwcUserInfo)),
//            m_socket, SLOT(createGroup(QwcUserInfo)));
//    connect(m_accountsWidget, SIGNAL(groupEdited(QwcUserInfo)),
//            m_socket, SLOT(editGroup(QwcUserInfo)));
//    connect(m_accountsWidget, SIGNAL(groupDeleted(QString)),
//            m_socket, SLOT(deleteGroup(QString)));
//
//    connect(m_accountsWidget, SIGNAL(refreshedAccountsAndGroups()),
//            m_socket, SLOT(getGroups()));
//    connect(m_accountsWidget, SIGNAL(refreshedAccountsAndGroups()),
//            m_socket, SLOT(getUsers()));

    m_socket->getGroups();
    m_socket->getUsers();

}

QwcSocket* QwcAccountsWidget::socket()
{ return m_socket; }


/*! Enable/Disable the widgets of the editor form depending on the current editor mode. (e.g.
    disable some widgets when creating a new account).
*/
void QwcAccountsWidget::setupEditWidgets()
{
    btnEditDelete->setEnabled(!newAccountMode);
}


/*! The "New Account" button has been clicked.
*/
void QwcAccountsWidget::on_btnCreateAccount_clicked()
{
    currentAccount = QwcUserInfo();
    newAccountMode = true;
    loadFromAccount(currentAccount);
    setupEditWidgets();
    stackedWidget->setCurrentIndex(0);
}


void QwcAccountsWidget::appendUserNames(QStringList theUsers)
{
    QStringListIterator i(theUsers);
    while(i.hasNext()) {
        QString accountName = i.next();
        QListWidgetItem *newItem = new QListWidgetItem;
        newItem->setIcon(QIcon(":/icons/icon_account.png"));
        newItem->setText(accountName);
        newItem->setData(Qt::UserRole, Qws::UserTypeAccount);
        fList->addItem(newItem);
    }

}

void QwcAccountsWidget::appendGroupNames(QStringList theGroups)
{
    QStringListIterator i(theGroups);
    fGroup->clear();
    /*: The entry in the account editor group-popup menu when no group is selected for an account. */
    fGroup->addItem(tr("No Group"), QString() );
    while(i.hasNext()) {
        QString accountName = i.next();
        QListWidgetItem *newItem = new QListWidgetItem;
        newItem->setIcon(QIcon(":/icons/icon_accountgroup.png"));
        newItem->setText(accountName);
        newItem->setData(Qt::UserRole, Qws::UserTypeGroup);
        fGroup->addItem(accountName, accountName);
        fList->addItem(newItem);
    }
}


/*! The currently selected group/user in the list has changed.
*/
void QwcAccountsWidget::on_fList_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous)
{
    Q_UNUSED(previous);
    btnEditAccount->setEnabled(current != NULL);
    btnDeleteAccount->setEnabled(current != NULL);
}


/*! The save/create button has been clicked.
*/
void QwcAccountsWidget::on_fBtnApply_clicked()
{
    QwcUserInfo newAccount;
    newAccount.setLoginName(fName->text());
    newAccount.setType((Qws::UserType)fAccountType->currentIndex()); // 0=user, 1=group

    Qws::Privileges newPrivs = newAccount.privileges();
    if (fBasicGetUserInfo->isChecked()) { newPrivs |= Qws::PrivilegeGetUserInfo; }
    if (fBasicPostNews->isChecked()) { newPrivs |= Qws::PrivilegePostNews; }
    if (fBasicBroadcast->isChecked()) { newPrivs |= Qws::PrivilegeSendBroadcast; }
    if (fBasicClearNews->isChecked()) { newPrivs |= Qws::PrivilegeClearNews; }
    if (fBasicSetTopic->isChecked()) { newPrivs |= Qws::PrivilegeChangeChatTopic; }
    if (fFilesDownload->isChecked()) { newPrivs |= Qws::PrivilegeDownload; }
    if (fFilesUpload->isChecked()) { newPrivs |= Qws::PrivilegeUpload; }
    if (fFilesUploadAnywhere->isChecked()) { newPrivs |= Qws::PrivilegeUploadAnywhere; }
    if (fFilesViewDropBoxes->isChecked()) { newPrivs |= Qws::PrivilegeViewDropboxes; }
    if (fFilesCreateFolders->isChecked()) { newPrivs |= Qws::PrivilegeCreateFolders; }
    if (fFilesMoveChange->isChecked()) { newPrivs |= Qws::PrivilegeAlterFiles; }
    if (fFilesDelete->isChecked()) { newPrivs |= Qws::PrivilegeDeleteFiles; }
    if (fUsersCreate->isChecked()) { newPrivs |= Qws::PrivilegeCreateAccounts; }
    if (fUsersEdit->isChecked()) { newPrivs |= Qws::PrivilegeEditAccounts; }
    if (fUsersDelete->isChecked()) { newPrivs |= Qws::PrivilegeDeleteAccounts; }
    if (fUsersElevate->isChecked()) { newPrivs |= Qws::PrivilegeElevatePrivileges; }
    if (fUsersKick->isChecked()) { newPrivs |= Qws::PrivilegeKickUsers; }
    if (fUsersBan->isChecked()) { newPrivs |= Qws::PrivilegeBanUsers; }
    if (fUsersNoKick->isChecked()) { newPrivs |= Qws::PrivilegeCanNotBeKicked; }

    newAccount.privDownloadLimit = fLimitDown->text().toInt();
    newAccount.privUploadLimit = fLimitUp->text().toInt();

    if (newAccount.type() == Qws::UserTypeAccount) {
        newAccount.password() = fPassword->text();

        // Re-hash the password if it changed
        if (currentAccount.password() != newAccount.password()) {// password, changed - reencode
            newAccount.setCryptedPassword(newAccount.password());
        }

        // Set the group name
        if (fGroup->currentIndex() > 0) {
            newAccount.setGroupName(fGroup->currentText());
        }
    }

    if (newAccountMode) {
        // Create new account/group
        if (newAccount.type() == Qws::UserTypeAccount) {
            appendUserNames(QStringList() << newAccount.loginName());
            m_socket->createUser(newAccount);
        } else if (newAccount.type() == Qws::UserTypeGroup) {
            appendGroupNames(QStringList() << newAccount.loginName());
            m_socket->createGroup(newAccount);
        }
        btnEditDelete->click();

    } else {
        // Update existing account/group
        if(fAccountType->currentIndex()==0) {
            m_socket->editUser(newAccount);
        } else {
            m_socket->editGroup(newAccount);
        }
    }

    // Display the list again
    stackedWidget->setCurrentIndex(1);
}


/*! The refresh button has been clicked.
*/
void QwcAccountsWidget::on_btnRefreshAccounts_clicked()
{
    fList->clear();
    m_socket->getUsers();
    m_socket->getGroups();
}


/*! The type of the account/group object changed.
*/
void QwcAccountsWidget::on_fAccountType_currentIndexChanged(int index)
{
    if (!newAccountMode) { return; }

    fPassword->setEnabled(index == 0); // 0 = Account, 1 = Group
    fGroup->setEnabled(index == 0);

    // Enable privileges only if the object is a group or account, but not if the
    // object is an account with a group set.
    bool enablePrivileges = (index == 0 && fGroup->currentIndex() == 0) || (index == 1);
    fGroupBasic->setEnabled(enablePrivileges);
    fGroupFiles->setEnabled(enablePrivileges);
    fGroupAdmin->setEnabled(enablePrivileges);
    fGroupLimits->setEnabled(enablePrivileges);

}


/*! The account group has changed.
*/
void QwcAccountsWidget::on_fGroup_currentIndexChanged(int index)
{
    bool enablePrivileges = (index == 0);
    fGroupBasic->setEnabled(enablePrivileges);
    fGroupFiles->setEnabled(enablePrivileges);
    fGroupAdmin->setEnabled(enablePrivileges);
    fGroupLimits->setEnabled(enablePrivileges);
}


/*! The "Cancel" button of the account editor has been clicked.
*/
void QwcAccountsWidget::on_btnEditCancel_clicked()
{
    stackedWidget->setCurrentIndex(1);
}


/*! The "Edit User/Group" button has been clicked.
*/
void QwcAccountsWidget::on_btnEditAccount_clicked()
{
    QListWidgetItem *item = fList->currentItem();
    if (!item) { return; }

    newAccountMode = false;

    // Request the account data from the server
    if (item->data(Qt::UserRole) == Qws::UserTypeAccount) {
        m_socket->readUser(item->text());
    } else if (item->data(Qt::UserRole) == Qws::UserTypeGroup) {
        m_socket->readGroup(item->text());
    }

    // Switch to the editor form.
    stackedWidget->setCurrentIndex(0);

    // Now enable/disable the controls
    setupEditWidgets();
}



/*! The delete button next to the group/account list has been clicked.
*/
void QwcAccountsWidget::on_btnDeleteAccount_clicked()
{
    QListWidgetItem *item = fList->currentItem();
    if (!item) { return; }

    currentAccount = QwcUserInfo();
    currentAccount.setType((Qws::UserType)item->data(Qt::UserRole).toInt());
    currentAccount.setLoginName(item->text());

    // We use the existing delete mechanism to save some code
    on_btnEditDelete_clicked();
}


/*! The "Delete" button has been clicked.
*/
void QwcAccountsWidget::on_btnEditDelete_clicked()
{

    QMessageBox::StandardButton result = QMessageBox::question(this,
                 tr("Delete Account or Group"),
                 tr("Are you sure you want to delete the account/group \"%1\"?")
                    .arg(currentAccount.loginName()),
                 QMessageBox::Ok | QMessageBox::Cancel,
                 QMessageBox::Ok );

    if (result == QMessageBox::Ok) {
        // Remove from the main list
        QListWidgetItem *listItem = fList->takeItem( fList->currentRow() );
        delete listItem;
        if (currentAccount.type() == Qws::UserTypeAccount) {
            m_socket->deleteUser(currentAccount.loginName());
        } else if (currentAccount.type() == Qws::UserTypeGroup) {
            m_socket->deleteGroup(currentAccount.loginName());
            // Also delete from the popup-menu if this is a group
            fGroup->removeItem(fGroup->findText(currentAccount.loginName()));
        }

        // Go back to the list
        stackedWidget->setCurrentIndex(1);
    }

}


/*! Load the data of a requested account/group specification into the editor form.
*/
void QwcAccountsWidget::loadFromAccount(const QwcUserInfo account)
{
    currentAccount = account;

    // Disable the name/type fields when editing an existing group/account
    fAccountType->setEnabled(newAccountMode);
    fName->setEnabled(newAccountMode);
    fName->setText(account.loginName());

    fPassword->setEnabled(account.type() == Qws::UserTypeAccount);
    fGroup->setEnabled(currentAccount.type() == Qws::UserTypeAccount);

    if (account.type() == Qws::UserTypeAccount) {
        // Select the correct group for an account
        int groupIndex = fGroup->findData(currentAccount.groupName());
        fGroup->setCurrentIndex(groupIndex == -1 ? 0 : groupIndex);
        fPassword->setText(account.password());

    } else if (account.type() == Qws::UserTypeGroup) {
        // Select no group when editing groups
        fGroup->setCurrentIndex(0);
    }

    fAccountType->setCurrentIndex(account.type());

    // Privileges
    fGroupBasic->setEnabled(currentAccount.groupName().isEmpty());
    fGroupLimits->setEnabled(currentAccount.groupName().isEmpty());
    fGroupFiles->setEnabled(currentAccount.groupName().isEmpty());
    fGroupAdmin->setEnabled(currentAccount.groupName().isEmpty());

    // Basic Privileges
    fBasicGetUserInfo->setChecked(currentAccount.privileges() & Qws::PrivilegeGetUserInfo);
    fBasicPostNews->setChecked(currentAccount.privileges() & Qws::PrivilegePostNews);
    fBasicBroadcast->setChecked(currentAccount.privileges() & Qws::PrivilegeSendBroadcast);
    fBasicSetTopic->setChecked(currentAccount.privileges() & Qws::PrivilegeChangeChatTopic);
    fBasicClearNews->setChecked(currentAccount.privileges() & Qws::PrivilegeClearNews);

    // Transfer/Files Privileges
    fFilesDownload->setChecked(currentAccount.privileges() & Qws::PrivilegeDownload);
    fFilesUpload->setChecked(currentAccount.privileges() & Qws::PrivilegeUpload);
    fFilesUploadAnywhere->setChecked(currentAccount.privileges() & Qws::PrivilegeUploadAnywhere);
    fFilesViewDropBoxes->setChecked(currentAccount.privileges() & Qws::PrivilegeViewDropboxes);
    fFilesCreateFolders->setChecked(currentAccount.privileges() & Qws::PrivilegeCreateFolders);
    fFilesMoveChange->setChecked(currentAccount.privileges() & Qws::PrivilegeAlterFiles);
    fFilesDelete->setChecked(currentAccount.privileges() & Qws::PrivilegeDeleteFiles);

    // Administration Privileges
    fUsersCreate->setChecked(currentAccount.privileges() & Qws::PrivilegeCreateAccounts);
    fUsersEdit->setChecked(currentAccount.privileges() & Qws::PrivilegeEditAccounts);
    fUsersDelete->setChecked(currentAccount.privileges() & Qws::PrivilegeDeleteAccounts);
    fUsersElevate->setChecked(currentAccount.privileges() & Qws::PrivilegeElevatePrivileges);
    fUsersKick->setChecked(currentAccount.privileges() & Qws::PrivilegeKickUsers);
    fUsersBan->setChecked(currentAccount.privileges() & Qws::PrivilegeBanUsers);
    fUsersNoKick->setChecked(currentAccount.privileges() & Qws::PrivilegeCanNotBeKicked);

    // Transfer limits
    fLimitDown->setText(QString::number(currentAccount.privDownloadLimit));
    fLimitUp->setText(QString::number(currentAccount.privUploadLimit));

}







