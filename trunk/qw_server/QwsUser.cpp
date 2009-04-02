#include <QtSql>

#include "QwsUser.h"

/*! \class QwsUser
    \date 2009-03-08
    \author Bastian Bense <bastibense@gmail.com>
    \brief This class defines a user for the Qwired Server.
*/

QwsUser::QwsUser()
{
    QwUser();
}


/*! This method generates a list of privilege flag fields for use in all commands that utilize the
    <privileges> list (see Wired documentation).
*/
void QwsUser::privilegesFlags(QwMessage &message) const
{
    message.appendArg(QString::number(privGetUserInfo));
    message.appendArg(QString::number(privBroadcast));
    message.appendArg(QString::number(privPostNews));
    message.appendArg(QString::number(privClearNews));
    message.appendArg(QString::number(privDownload));
    message.appendArg(QString::number(privUpload));
    message.appendArg(QString::number(privUploadAnywhere));
    message.appendArg(QString::number(privCreateFolders));
    message.appendArg(QString::number(privAlterFiles));
    message.appendArg(QString::number(privDeleteFiles));
    message.appendArg(QString::number(privViewDropboxes));
    message.appendArg(QString::number(privCreateAccounts));
    message.appendArg(QString::number(privEditAccounts));
    message.appendArg(QString::number(privDeleteAccounts));
    message.appendArg(QString::number(privElevatePrivileges));
    message.appendArg(QString::number(privKickUsers));
    message.appendArg(QString::number(privBanUsers));
    message.appendArg(QString::number(privCannotBeKicked));
    message.appendArg(QString::number(privDownloadSpeed));
    message.appendArg(QString::number(privUploadSpeed));
    message.appendArg(QString::number(privDownloadLimit));
    message.appendArg(QString::number(privUploadLimit));
    message.appendArg(QString::number(privChangeTopic));
}


/*! Complete a message specially formatted for the user listing responses (310)
*/
void QwsUser::userListEntry(QwMessage &message) const
{
    message.appendArg(QString::number(pUserID));
    message.appendArg(QString::number(pIdle));
    message.appendArg(QString::number(privKickUsers | privBanUsers));
    message.appendArg(QString::number(pIcon));
    message.appendArg(userNickname);
    message.appendArg(name);
    message.appendArg(userIpAddress);
    message.appendArg(userHostName);
    message.appendArg(userStatus);
    message.appendArg(pImage);
}


/* Complete a message specially formatted for the User Status Change messages.
*/
void QwsUser::userStatusEntry(QwMessage &message) const
{
    message.appendArg(QString::number(pUserID));
    message.appendArg(QString::number(pIdle));
    message.appendArg(QString::number(privKickUsers | privBanUsers));
    message.appendArg(QString::number(pIcon)); // icon, unused since 1.1
    message.appendArg(userNickname);
    message.appendArg(userStatus);
}


/*! This method generates (appends) the fields required for a INFO response to a client.
*/
void QwsUser::userInfoEntry(QwMessage &message) const
{
    message.appendArg(QString::number(pUserID));
    message.appendArg(pIdle ? "1" : "0");
    message.appendArg(privBanUsers || privKickUsers ? "1" : "0");
    message.appendArg(QString::number(pIcon));
    message.appendArg(userNickname);
    message.appendArg(name);
    message.appendArg(userIpAddress);
    message.appendArg(userHostName);

    message.appendArg(pClientVersion);
    message.appendArg(pCipherName);
    message.appendArg(QString::number(pCipherBits));
    message.appendArg(pLoginTime.toTimeSpec(Qt::UTC).toString(Qt::ISODate)+"+00:00");
    message.appendArg(pIdleTime.toTimeSpec(Qt::UTC).toString(Qt::ISODate)+"+00:00");
    message.appendArg(""); // downloads - filled in later
    message.appendArg(""); // uploads - filled in later
    message.appendArg(userStatus);
    message.appendArg(pImage);

}


/*! This method loads the account data from the database. In order for this to work, the \a name
    property has to be set to a existing user name. If a group is to be loaded from the database,
    the \a userType property has to be set to \a Qws::UserTypeGroup.
    If something goes wrong, or the account is not found in the database, this method returns false.
    Otherwise the data is loaded from the database and all properties of the class instance are set.
*/
bool QwsUser::loadFromDatabase()
{
    qDebug() << this << "Reading properties for account/group" << name;

    QSqlQuery query;
    if (userType == Qws::UserTypeAccount) {
        // Load Information about a User Account
        query.prepare("SELECT id, acc_secret, acc_privileges, acc_group, acc_maxupload, acc_maxdownload, "
                      "acc_speedupload, acc_speeddownload "
                      "FROM qws_accounts WHERE acc_name=:_name LIMIT 1;");
        query.bindValue(":_name", this->name);
        if (!query.exec()) {
            qDebug() << this << "Unable to find user account:" << query.lastError().text();
            return false;
        } else {
            query.first();
            if (!query.isValid()) {
                return false;
            }
            this->pPassword = query.value(1).toString();
            this->pGroupName = query.value(3).toString();

            if (pGroupName.isEmpty()) {
                // No group specified - load user privileges
                this->privUploadLimit = query.value(4).toInt();
                this->privDownloadLimit = query.value(5).toInt();
                this->privUploadSpeed = query.value(6).toInt();
                this->privDownloadSpeed = query.value(7).toInt();
                this->setPrivilegesFromQwiredSpec(query.value(2).toString());
            } else {
                // Group specified - load group data
                QwsUser targetGroup;
                targetGroup.userType = Qws::UserTypeGroup;
                targetGroup.name = pGroupName;
                if (!targetGroup.loadFromDatabase()) {
                    qDebug() << this << "Unable to load group for user.";
                    return false;
                } else {
                    // Load privileges from group
                    qDebug() << this << "Loading privileges and settings from group" << pGroupName;
                    this->setPrivilegesFromQwiredSpec(targetGroup.privilegesFlagsAsQwiredSpec());
                    this->privUploadLimit = targetGroup.privUploadLimit;
                    this->privDownloadLimit = targetGroup.privDownloadLimit;
                    this->privUploadSpeed = targetGroup.privUploadSpeed;
                    this->privDownloadSpeed = targetGroup.privDownloadSpeed;
                }

            }

        }

    } else if (userType == Qws::UserTypeGroup) {
        // Load information about a group
        query.prepare("SELECT id, group_privs FROM qws_groups WHERE group_name=:_name LIMIT 1");
        query.bindValue(":_name", this->name);
        if (!query.exec()) {
            qDebug() << this << "Unable to find user account:" << query.lastError().text();
            return false;
        } else {
            query.first();
            if (!query.isValid()) {
                return false;
            }
            this->setPrivilegesFromQwiredSpec(query.value(1).toString());
        }
    }
    return true;
}


/*! This method writes the current account to the database. The account (or group) is created, if it
    does not exist yet.
*/
bool QwsUser::writeToDatabase()
{
    QSqlQuery query;
    if (userType == Qws::UserTypeAccount) {
        // Load Information about a User Account
        query.prepare("UPDATE qws_accounts "
                      "SET acc_secret=:_secret, acc_privileges=:_privileges, acc_group=:_group, "
                      "acc_maxdownload=:_maxdown, acc_maxupload=:_maxup, "
                      "acc_speeddownload=:_speeddownload, acc_speedupload=:_speedupload "
                      "WHERE acc_name=:_name");
        query.bindValue(":_name", this->name);
        query.bindValue(":_secret", this->pPassword);
        query.bindValue(":_group", this->pGroupName);
        query.bindValue(":_maxdown", this->privDownloadLimit);
        query.bindValue(":_maxup", this->privUploadLimit);
        query.bindValue(":_speeddownload", this->privDownloadSpeed);
        query.bindValue(":_speedupload", this->privUploadSpeed);
        query.bindValue(":_privileges", this->privilegesFlagsAsQwiredSpec());
        if (!query.exec()) {
            qDebug() << this << "Unable to write user account:" << query.lastError().text();
            return false;
        } else {
            if (!query.numRowsAffected()) {
                // Account does not exist yet
                return false;
            }
        }

    } else if (userType == Qws::UserTypeGroup) {
        // Write a group
        query.prepare("UPDATE qws_groups SET group_privs=:_privs WHERE group_name=:_name");
        query.bindValue(":_name", this->name);
        query.bindValue(":_privs", this->privilegesFlagsAsQwiredSpec());
        if (!query.exec()) {
            qDebug() << this << "Unable to edit user group:" << query.lastError().text();
            return false;
        } else {
            if (query.numRowsAffected() <= 0) {
                return false;
            }
        }
    }
    return true;
}


/*! This function deletes a group or user from the database. The properties \a name and \a userType
    have to be set in order for this to work.
    This method returns true on success.
*/
bool QwsUser::deleteFromDatabase()
{
    QSqlQuery query;
    if (userType == Qws::UserTypeAccount) {
        query.prepare("DELETE FROM qws_accounts WHERE acc_name=:_name");
        query.bindValue(":_name", this->name);
        if (!query.exec()) {
            qDebug() << this << "Unable to delete user account:" << query.lastError().text();
            return false;
        } else {
            if (query.numRowsAffected() <= 0) {
                return false;
            }
        }


    } else if (userType == Qws::UserTypeGroup) {
        query.prepare("DELETE FROM qws_groups WHERE group_name=:_name");
        query.bindValue(":_name", this->name);
        if (!query.exec()) {
            qDebug() << this << "Unable to delete user group:" << query.lastError().text();
            return false;
        } else {
            if (query.numRowsAffected() <= 0) {
                return false;
            }
        }
    }
    return true;
}


/*! This method appends the required privileges flags for a response to the READUSER command (1.1).
    The user name and other fields are not appended to the message, though.
    The syntax for the privileges is also used on groups.
*/
void QwsUser::appendPrivilegeFlagsForREADUSER(QwMessage &message)
{
    message.appendArg(QString::number(privGetUserInfo));
    message.appendArg(QString::number(privBroadcast));
    message.appendArg(QString::number(privPostNews));
    message.appendArg(QString::number(privClearNews));
    message.appendArg(QString::number(privDownload));
    message.appendArg(QString::number(privUpload));
    message.appendArg(QString::number(privUploadAnywhere));
    message.appendArg(QString::number(privCreateFolders));
    message.appendArg(QString::number(privAlterFiles));
    message.appendArg(QString::number(privDeleteFiles));
    message.appendArg(QString::number(privViewDropboxes));
    message.appendArg(QString::number(privCreateAccounts));
    message.appendArg(QString::number(privEditAccounts));
    message.appendArg(QString::number(privDeleteAccounts));
    message.appendArg(QString::number(privElevatePrivileges));
    message.appendArg(QString::number(privKickUsers));
    message.appendArg(QString::number(privBanUsers));
    message.appendArg(QString::number(privCannotBeKicked));
    message.appendArg(QString::number(privDownloadSpeed));
    message.appendArg(QString::number(privUploadSpeed));
    message.appendArg(QString::number(privDownloadLimit)); // wired 1.1
    message.appendArg(QString::number(privUploadLimit)); // wired 1.1
    message.appendArg(QString::number(privChangeTopic)); // wired 1.1
}


/*! This method sets the user privilege flags from the contents of the EDITUSER message.
*/
void QwsUser::setPrivilegesFromEDITUSER(QwMessage &message, int fieldOffset)
{
    // For EDITUSER we should skip 3 fields
    // For EDITGROUP we should skip 1 field
    this->privGetUserInfo = message.getStringArgument(fieldOffset++).toInt();
    this->privBroadcast = message.getStringArgument(fieldOffset++).toInt();
    this->privPostNews = message.getStringArgument(fieldOffset++).toInt();
    this->privClearNews = message.getStringArgument(fieldOffset++).toInt();
    this->privDownload = message.getStringArgument(fieldOffset++).toInt();
    this->privUpload = message.getStringArgument(fieldOffset++).toInt();
    this->privUploadAnywhere = message.getStringArgument(fieldOffset++).toInt();
    this->privCreateFolders = message.getStringArgument(fieldOffset++).toInt();
    this->privAlterFiles = message.getStringArgument(fieldOffset++).toInt();
    this->privDeleteFiles = message.getStringArgument(fieldOffset++).toInt();
    this->privViewDropboxes = message.getStringArgument(fieldOffset++).toInt();
    this->privCreateAccounts = message.getStringArgument(fieldOffset++).toInt();
    this->privEditAccounts = message.getStringArgument(fieldOffset++).toInt();
    this->privDeleteAccounts = message.getStringArgument(fieldOffset++).toInt();
    this->privElevatePrivileges = message.getStringArgument(fieldOffset++).toInt();
    this->privKickUsers = message.getStringArgument(fieldOffset++).toInt();
    this->privBanUsers = message.getStringArgument(fieldOffset++).toInt();
    this->privCannotBeKicked = message.getStringArgument(fieldOffset++).toInt();
    this->privDownloadSpeed = message.getStringArgument(fieldOffset++).toInt();
    this->privUploadSpeed = message.getStringArgument(fieldOffset++).toInt();
    this->privDownloadLimit = message.getStringArgument(fieldOffset++).toInt(); // wired 1.1
    this->privUploadLimit = message.getStringArgument(fieldOffset++).toInt(); // wired 1.1
    this->privChangeTopic = message.getStringArgument(fieldOffset++).toInt(); // wired 1.1

}

