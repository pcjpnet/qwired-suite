/*! \class QwcAccountsWidget
    \author Bastian Bense <bastibense@gmail.com>
    \date 2009-03-06

    This is the account administration widget for the Qwired client.
*/


#include "QwcAccountsWidget.h"

#include <QMessageBox>


QwcAccountsWidget::QwcAccountsWidget(QWidget *parent) : QWidget(parent)
{
    setAttribute(Qt::WA_DeleteOnClose);
    setupUi(this);
    newAccountMode = false;
    fGroupBasic->setEnabled(false);
    fGroupFiles->setEnabled(false);
    fGroupAdmin->setEnabled(false);
    fGroupLimits->setEnabled(false);
}


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
    newAccount.name = fName->text();
    newAccount.userType = (Qws::UserType)fAccountType->currentIndex(); // 0=user, 1=group

    newAccount.privGetUserInfo = fBasicGetUserInfo->isChecked();
    newAccount.privPostNews = fBasicPostNews->isChecked();
    newAccount.privBroadcast = fBasicBroadcast->isChecked();
    newAccount.privClearNews = fBasicClearNews->isChecked();
    newAccount.privChangeTopic = fBasicSetTopic->isChecked();
    newAccount.privDownload = fFilesDownload->isChecked();
    newAccount.privUpload = fFilesUpload->isChecked();
    newAccount.privUploadAnywhere = fFilesUploadAnywhere->isChecked();
    newAccount.privViewDropboxes = fFilesViewDropBoxes->isChecked();
    newAccount.privCreateFolders = fFilesCreateFolders->isChecked();
    newAccount.privAlterFiles = fFilesMoveChange->isChecked();
    newAccount.privDeleteFiles = fFilesDelete->isChecked();
    newAccount.privCreateAccounts = fUsersCreate->isChecked();
    newAccount.privEditAccounts = fUsersEdit->isChecked();
    newAccount.privDeleteAccounts = fUsersDelete->isChecked();
    newAccount.privElevatePrivileges = fUsersElevate->isChecked();
    newAccount.privKickUsers = fUsersKick->isChecked();
    newAccount.privBanUsers = fUsersBan->isChecked();
    newAccount.privCannotBeKicked = fUsersNoKick->isChecked();
    newAccount.privDownloadLimit = fLimitDown->text().toInt();
    newAccount.privUploadLimit = fLimitUp->text().toInt();

    if (newAccount.userType == Qws::UserTypeAccount) {
        newAccount.pPassword = fPassword->text();

        // Re-hash the password if it changed
        if (currentAccount.pPassword != newAccount.pPassword) {// password, changed - reencode
            newAccount.pPassword = newAccount.cryptedPassword();
        }

        // Set the group name
        if (fGroup->currentIndex() > 0) {
            newAccount.pGroupName = fGroup->currentText();
        }
    }

    if (newAccountMode) {
        // Create new account/group
        if (newAccount.userType == Qws::UserTypeAccount) {
            appendUserNames(QStringList() << newAccount.name);
            emit accountCreated(newAccount);
        } else if (newAccount.userType == Qws::UserTypeGroup) {
            appendGroupNames(QStringList() << newAccount.name);
            emit groupCreated(newAccount);
        }
        btnEditDelete->click();

    } else {
        // Update existing account/group
        if(fAccountType->currentIndex()==0) {
            emit accountEdited(newAccount);
        } else {
            emit groupEdited(newAccount);
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
    emit refreshedAccountsAndGroups();
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
        emit userSpecRequested(item->text());
    } else if (item->data(Qt::UserRole) == Qws::UserTypeGroup) {
        emit groupSpecRequested(item->text());
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
    currentAccount.userType = (Qws::UserType)item->data(Qt::UserRole).toInt();
    currentAccount.name = item->text();

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
                    .arg(currentAccount.name),
                 QMessageBox::Ok | QMessageBox::Cancel,
                 QMessageBox::Ok );

    if (result == QMessageBox::Ok) {
        // Remove from the main list
        QListWidgetItem *listItem = fList->takeItem( fList->currentRow() );
        delete listItem;
        if (currentAccount.userType == Qws::UserTypeAccount) {
            emit accountDeleted(currentAccount.name);
        } else if (currentAccount.userType == Qws::UserTypeGroup) {
            emit groupDeleted(currentAccount.name);
            // Also delete from the popup-menu if this is a group
            fGroup->removeItem(fGroup->findText(currentAccount.name));
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
    fName->setText(account.name);

    fPassword->setEnabled(account.userType == Qws::UserTypeAccount);
    fGroup->setEnabled(currentAccount.userType == Qws::UserTypeAccount);

    if (account.userType == Qws::UserTypeAccount) {
        // Select the correct group for an account
        int groupIndex = fGroup->findData(currentAccount.pGroupName);
        fGroup->setCurrentIndex(groupIndex == -1 ? 0 : groupIndex);
        fPassword->setText(account.pPassword);

    } else if (account.userType == Qws::UserTypeGroup) {
        // Select no group when editing groups
        fGroup->setCurrentIndex(0);
    }

    fAccountType->setCurrentIndex(account.userType);

    // Privileges
    fGroupBasic->setEnabled(currentAccount.pGroupName.isEmpty());
    fGroupLimits->setEnabled(currentAccount.pGroupName.isEmpty());
    fGroupFiles->setEnabled(currentAccount.pGroupName.isEmpty());
    fGroupAdmin->setEnabled(currentAccount.pGroupName.isEmpty());

    // Basic Privileges
    fBasicGetUserInfo->setChecked(currentAccount.privGetUserInfo);
    fBasicPostNews->setChecked(currentAccount.privPostNews);
    fBasicBroadcast->setChecked(currentAccount.privBroadcast);
    fBasicSetTopic->setChecked(currentAccount.privChangeTopic);
    fBasicClearNews->setChecked(currentAccount.privClearNews);

    // Transfer/Files Privileges
    fFilesDownload->setChecked(currentAccount.privDownload);
    fFilesUpload->setChecked(currentAccount.privUpload);
    fFilesUploadAnywhere->setChecked(currentAccount.privUploadAnywhere);
    fFilesViewDropBoxes->setChecked(currentAccount.privViewDropboxes);
    fFilesCreateFolders->setChecked(currentAccount.privCreateFolders);
    fFilesMoveChange->setChecked(currentAccount.privAlterFiles);
    fFilesDelete->setChecked(currentAccount.privDeleteFiles);

    // Administration Privileges
    fUsersCreate->setChecked(currentAccount.privCreateAccounts);
    fUsersEdit->setChecked(currentAccount.privEditAccounts);
    fUsersDelete->setChecked(currentAccount.privDeleteAccounts);
    fUsersElevate->setChecked(currentAccount.privElevatePrivileges);
    fUsersKick->setChecked(currentAccount.privKickUsers);
    fUsersBan->setChecked(currentAccount.privBanUsers);
    fUsersNoKick->setChecked(currentAccount.privCannotBeKicked);

    // Transfer limits
    fLimitDown->setText(QString::number(currentAccount.privDownloadLimit));
    fLimitUp->setText(QString::number(currentAccount.privUploadLimit));

}







