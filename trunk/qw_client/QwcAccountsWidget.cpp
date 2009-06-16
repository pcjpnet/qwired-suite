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
        fBtnApply->setEnabled(false);
        fList->setCurrentRow(-1);
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

    if (item->data(Qt::UserRole) == Qws::UserTypeAccount) {
        emit userSpecRequested(item->text());
    } else if (item->data(Qt::UserRole) == Qws::UserTypeGroup) {
        emit groupSpecRequested(item->text());
    }

    // Switch to the editor form.
    stackedWidget->setCurrentIndex(0);
}


void QwcAccountsWidget::on_btnEditDelete_clicked()
{
    if(newAccountMode) {
        fBtnApply->setText(tr("Apply Changes"));
        btnEditDelete->setText(tr("Delete"));
//        fBtnNew->setEnabled(true);
        newAccountMode = false;
        fList->setCurrentRow(-1);
        fList->setEnabled(true);
//        enableGui(false);
        fGroup->setCurrentIndex(0);
        fAccountType->setCurrentIndex(0);
        fGroup->setEnabled(false);
        btnEditDelete->setEnabled(false);
        fGroupBasic->setEnabled(false);
        fGroupFiles->setEnabled(false);
        fGroupAdmin->setEnabled(false);
        fGroupLimits->setEnabled(false);

    } else {
        QMessageBox::StandardButton result = QMessageBox::question(this,
                    tr("Delete Account or Group"),
                    tr("Are you sure you want to delete the current account or group? This can not be undone."),
                    QMessageBox::Ok | QMessageBox::Cancel,
                    QMessageBox::Ok );
        if(result == QMessageBox::Ok) {
            QListWidgetItem *tmpItm = fList->takeItem( fList->currentRow() );
            QString tmpName = tmpItm->text();
            int tmpType = tmpItm->data(Qt::UserRole).toInt();
            delete tmpItm; tmpItm=0;
            fList->setCurrentRow(-1);
            if(tmpType==0)
                emit accountDeleted(tmpName);
            else emit groupDeleted(tmpName);
        }
    }
}


void QwcAccountsWidget::loadFromAccount(const QwcUserInfo account)
{
    currentAccount = account; // to recover the password hash later
    fGroupBasic->setEnabled(currentAccount.pGroupName.isEmpty());
    fGroupLimits->setEnabled(currentAccount.pGroupName.isEmpty());
    fGroupFiles->setEnabled(currentAccount.pGroupName.isEmpty());
    fGroupAdmin->setEnabled(currentAccount.pGroupName.isEmpty());
    fGroup->setEnabled(currentAccount.userType == Qws::UserTypeAccount);
    fAccountType->setCurrentIndex(currentAccount.userType);
    fAccountType->setEnabled(false);
    fName->setEnabled(false);
    fPassword->setEnabled(currentAccount.userType == Qws::UserTypeAccount);
    fPassword->setText(currentAccount.pPassword);

    fLimitDown->setText( QString::number(currentAccount.privDownloadLimit) );
    fLimitUp->setText( QString::number(currentAccount.privUploadLimit) );

    fBasicGetUserInfo->setChecked( currentAccount.privGetUserInfo );
    fBasicPostNews->setChecked( currentAccount.privPostNews );
    fBasicBroadcast->setChecked( currentAccount.privBroadcast );
    fBasicSetTopic->setChecked( currentAccount.privChangeTopic );
    fBasicClearNews->setChecked( currentAccount.privClearNews );

    fFilesDownload->setChecked( currentAccount.privDownload );
    fFilesUpload->setChecked( currentAccount.privUpload );
    fFilesUploadAnywhere->setChecked( currentAccount.privUploadAnywhere );
    fFilesViewDropBoxes->setChecked( currentAccount.privViewDropboxes );
    fFilesCreateFolders->setChecked( currentAccount.privCreateFolders );
    fFilesMoveChange->setChecked( currentAccount.privAlterFiles );
    fFilesDelete->setChecked( currentAccount.privDeleteFiles );

    fUsersCreate->setChecked( currentAccount.privCreateAccounts );
    fUsersEdit->setChecked( currentAccount.privEditAccounts );
    fUsersDelete->setChecked( currentAccount.privDeleteAccounts );
    fUsersElevate->setChecked( currentAccount.privElevatePrivileges );
    fUsersKick->setChecked( currentAccount.privKickUsers );
    fUsersBan->setChecked( currentAccount.privBanUsers );
    fUsersNoKick->setChecked( currentAccount.privCannotBeKicked );



    int tmpIdx = fGroup->findData(currentAccount.pGroupName);
    if(tmpIdx==-1) {
        fGroup->setCurrentIndex(0);
    } else {
        fGroup->setCurrentIndex(tmpIdx);
    }


}







