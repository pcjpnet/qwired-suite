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


void QwcAccountsWidget::appendUserNames(QStringList theUsers)
{
    QStringListIterator i(theUsers);
    while(i.hasNext()) {
        QString tmpLogin = i.next();
        QListWidgetItem *newItem = new QListWidgetItem;
        newItem->setIcon( QIcon(":/icons/icon_account.png") );
        newItem->setText(tmpLogin);
        newItem->setData(Qt::UserRole, 0); // user
        fList->addItem(newItem);
    }

}

void QwcAccountsWidget::appendGroupNames(QStringList theGroups)
{
    QStringListIterator i(theGroups);
    fGroup->clear();
    fGroup->addItem(tr("No Group"), QString() );
    while(i.hasNext()) {
        QString tmpGroup = i.next();
        QListWidgetItem *newItem = new QListWidgetItem;
        newItem->setIcon( QIcon(":/icons/icon_accountgroup.png") );
        newItem->setText(tmpGroup);
        newItem->setData(Qt::UserRole, 1); // group
        fGroup->addItem(tmpGroup, tmpGroup);
        fList->addItem(newItem);
    }
}

void QwcAccountsWidget::on_fList_currentItemChanged(QListWidgetItem * current, QListWidgetItem * previous)
{

    Q_UNUSED(previous)
            if(current) {
        if( current->data(Qt::UserRole).toInt() == 0 ) {
            emit userSpecRequested(current->text());
        } else {
            emit groupSpecRequested(current->text());
        }
        enableGui(false);
    } else {
        enableGui(false);
        fGroupBasic->setEnabled(false);
        fGroupFiles->setEnabled(false);
        fGroupAdmin->setEnabled(false);
        fGroupLimits->setEnabled(false);
    }

}

void QwcAccountsWidget::on_fBtnApply_clicked()
{
    QwcUserInfo u;
    u.name = fName->text();
    u.pPassword = fPassword->text();
    u.userType = (Qws::UserType)fType->currentIndex(); // 0=user, 1=group
    if(fGroup->currentIndex()>0)
        u.pGroupName = fGroup->currentText();
    u.privGetUserInfo = fBasicGetUserInfo->isChecked();
    u.privPostNews = fBasicPostNews->isChecked();
    u.privBroadcast = fBasicBroadcast->isChecked();
    u.privClearNews = fBasicClearNews->isChecked();
    u.privChangeTopic = fBasicSetTopic->isChecked();
    u.privDownload = fFilesDownload->isChecked();
    u.privUpload = fFilesUpload->isChecked();
    u.privUploadAnywhere = fFilesUploadAnywhere->isChecked();
    u.privViewDropboxes = fFilesViewDropBoxes->isChecked();
    u.privCreateFolders = fFilesCreateFolders->isChecked();
    u.privAlterFiles = fFilesMoveChange->isChecked();
    u.privDeleteFiles = fFilesDelete->isChecked();
    u.privCreateAccounts = fUsersCreate->isChecked();
    u.privEditAccounts = fUsersEdit->isChecked();
    u.privDeleteAccounts = fUsersDelete->isChecked();
    u.privElevatePrivileges = fUsersElevate->isChecked();
    u.privKickUsers = fUsersKick->isChecked();
    u.privBanUsers = fUsersBan->isChecked();
    u.privCannotBeKicked = fUsersNoKick->isChecked();
    u.privDownloadLimit = fLimitDown->text().toInt();
    u.privUploadLimit = fLimitUp->text().toInt();
    if(u.userType == Qws::UserTypeAccount) {
        u.pPassword = fPassword->text();
        if(pCurrentUser.pPassword != u.pPassword)// password, changed - reencode
            u.pPassword = u.cryptedPassword();

    }

    if(newAccountMode) {
        QListWidgetItem *newItem = new QListWidgetItem;
        newItem->setText(u.name);
        if(fType->currentIndex()==0) {
            emit createUser(u);
            newItem->setIcon( QIcon(":/icons/icon_account.png") );
            newItem->setData(Qt::UserRole, 0); // user
        } else {
            emit createGroup(u);
            newItem->setIcon( QIcon(":/icons/icon_accountgroup.png") );
            newItem->setData(Qt::UserRole, 1); // group
        }
        fList->addItem(newItem);
        fBtnDelete->click();

    } else { // Update
        if(fType->currentIndex()==0) {
            emit editUser(u);
        } else {
            emit editGroup(u);
        }
        fBtnApply->setEnabled(false);
        fList->setCurrentRow(-1);
    }
}


void QwcAccountsWidget::on_fType_currentIndexChanged(int index)
{
    if(!newAccountMode) return;
    fPassword->setEnabled( index==0 );
    fGroup->setEnabled( index==0 );
    bool b = (index==0 && fGroup->currentIndex()==0) || (index==1);
    fGroupBasic->setEnabled(b);
    fGroupFiles->setEnabled(b);
    fGroupAdmin->setEnabled(b);
    fGroupLimits->setEnabled(b);
}

void QwcAccountsWidget::on_fGroup_currentIndexChanged(int index)
{
    bool b = index==0;
    fGroupBasic->setEnabled(b);
    fGroupFiles->setEnabled(b);
    fGroupAdmin->setEnabled(b);
    fGroupLimits->setEnabled(b);
}



void QwcAccountsWidget::on_fBtnNew_clicked()
{
    // Switch to "new account" mode and lock down the GUI.
    newAccountMode = true;
    pCurrentUser = QwcUserInfo();
    setPrivFlags(pCurrentUser);

    fBtnApply->setText(tr("Create"));

    fList->setCurrentRow(-1);
    fList->setEnabled(false);
    enableGui(true);
    fGroup->setCurrentIndex(0);
    fType->setCurrentIndex(0);
    fGroup->setEnabled(true);
    fBtnNew->setEnabled(false);
    fBtnDelete->setEnabled(true);
    fBtnDelete->setText(tr("Cancel"));
    fGroupBasic->setEnabled(true);
    fGroupFiles->setEnabled(true);
    fGroupAdmin->setEnabled(true);
    fGroupLimits->setEnabled(true);


}



void QwcAccountsWidget::on_fBtnDelete_clicked()
{
    if(newAccountMode) {
        fBtnApply->setText(tr("Apply Changes"));
        fBtnDelete->setText(tr("Delete"));
        fBtnNew->setEnabled(true);
        newAccountMode = false;
        fList->setCurrentRow(-1);
        fList->setEnabled(true);
        enableGui(false);
        fGroup->setCurrentIndex(0);
        fType->setCurrentIndex(0);
        fGroup->setEnabled(false);
        fBtnDelete->setEnabled(false);
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
                emit userDeleted(tmpName);
            else emit groupDeleted(tmpName);
        }
    }
}

void QwcAccountsWidget::enableGui(bool e)
{
    fType->setEnabled(e);
    fName->setEnabled(e);
    fGroup->setEnabled(e);
    fPassword->setEnabled(e);;
    fBtnApply->setEnabled(e);
    fBtnDelete->setEnabled(e);
}

void QwcAccountsWidget::loadGroupSpec(QwcUserInfo u)
{
    fName->setText(u.name);
    enableGui(true);
    setPrivFlags(u);
}

void QwcAccountsWidget::loadUserSpec(QwcUserInfo u)
{
    fName->setText(u.name);
    enableGui(true);
    setPrivFlags(u);
}

void QwcAccountsWidget::setPrivFlags(QwcUserInfo u)
{
    pCurrentUser = u; // to recover the password hash later
    fGroupBasic->setEnabled(u.pGroupName.isEmpty());
    fGroupLimits->setEnabled(u.pGroupName.isEmpty());
    fGroupFiles->setEnabled(u.pGroupName.isEmpty());
    fGroupAdmin->setEnabled(u.pGroupName.isEmpty());
    fGroup->setEnabled(u.userType == Qws::UserTypeAccount);
    fType->setCurrentIndex(u.userType);
    fType->setEnabled(false);
    fName->setEnabled(false);
    fPassword->setEnabled(u.userType == Qws::UserTypeAccount);
    fPassword->setText(u.pPassword);

    fLimitDown->setText( QString::number(u.privDownloadLimit) );
    fLimitUp->setText( QString::number(u.privUploadLimit) );

    fBasicGetUserInfo->setChecked( u.privGetUserInfo );
    fBasicPostNews->setChecked( u.privPostNews );
    fBasicBroadcast->setChecked( u.privBroadcast );
    fBasicSetTopic->setChecked( u.privChangeTopic );
    fBasicClearNews->setChecked( u.privClearNews );

    fFilesDownload->setChecked( u.privDownload );
    fFilesUpload->setChecked( u.privUpload );
    fFilesUploadAnywhere->setChecked( u.privUploadAnywhere );
    fFilesViewDropBoxes->setChecked( u.privViewDropboxes );
    fFilesCreateFolders->setChecked( u.privCreateFolders );
    fFilesMoveChange->setChecked( u.privAlterFiles );
    fFilesDelete->setChecked( u.privDeleteFiles );

    fUsersCreate->setChecked( u.privCreateAccounts );
    fUsersEdit->setChecked( u.privEditAccounts );
    fUsersDelete->setChecked( u.privDeleteAccounts );
    fUsersElevate->setChecked( u.privElevatePrivileges );
    fUsersKick->setChecked( u.privKickUsers );
    fUsersBan->setChecked( u.privBanUsers );
    fUsersNoKick->setChecked( u.privCannotBeKicked );



    int tmpIdx = fGroup->findData(u.pGroupName);
    if(tmpIdx==-1) {
        fGroup->setCurrentIndex(0);
    } else {
        fGroup->setCurrentIndex(tmpIdx);
    }


}







