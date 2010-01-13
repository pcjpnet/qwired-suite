#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>

#include "QwsUser.h"

#include "QwMessage.h"

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
    message.appendArg(m_privileges & Qws::PrivilegeGetUserInfo ? 1 : 0);
    message.appendArg(m_privileges & Qws::PrivilegeSendBroadcast ? 1 : 0);
    message.appendArg(m_privileges & Qws::PrivilegePostNews ? 1 : 0);
    message.appendArg(m_privileges & Qws::PrivilegeClearNews ? 1 : 0);
    message.appendArg(m_privileges & Qws::PrivilegeDownload ? 1 : 0);
    message.appendArg(m_privileges & Qws::PrivilegeUpload ? 1 : 0);
    message.appendArg(m_privileges & Qws::PrivilegeUploadAnywhere ? 1 : 0);
    message.appendArg(m_privileges & Qws::PrivilegeCreateFolders ? 1 : 0);
    message.appendArg(m_privileges & Qws::PrivilegeAlterFiles ? 1 : 0);
    message.appendArg(m_privileges & Qws::PrivilegeDeleteFiles ? 1 : 0);
    message.appendArg(m_privileges & Qws::PrivilegeViewDropboxes ? 1 : 0);
    message.appendArg(m_privileges & Qws::PrivilegeCreateAccounts ? 1 : 0);
    message.appendArg(m_privileges & Qws::PrivilegeEditAccounts ? 1 : 0);
    message.appendArg(m_privileges & Qws::PrivilegeDeleteAccounts ? 1 : 0);
    message.appendArg(m_privileges & Qws::PrivilegeElevatePrivileges ? 1 : 0);
    message.appendArg(m_privileges & Qws::PrivilegeKickUsers ? 1 : 0);
    message.appendArg(m_privileges & Qws::PrivilegeBanUsers ? 1 : 0);
    message.appendArg(m_privileges & Qws::PrivilegeCanNotBeKicked ? 1 : 0);
    message.appendArg(privDownloadSpeed);
    message.appendArg(privUploadSpeed);
    message.appendArg(privDownloadLimit);
    message.appendArg(privUploadLimit);
    message.appendArg(m_privileges & Qws::PrivilegeChangeChatTopic ? 1 : 0);
}


/*! Complete a message specially formatted for the user listing responses (310)
    If \a emptyUserImage is true, the user image will not be transmitted, allowing a faster login
    on busy servers.
*/
void QwsUser::userListEntry(QwMessage &message, bool emptyUserImage) const
{
    message.appendArg(QString::number(pUserID));
    message.appendArg(QString::number(pIdle));
    if (m_privileges.testFlag(Qws::PrivilegeKickUsers)
        || m_privileges.testFlag(Qws::PrivilegeBanUsers)) {
        message.appendArg(1);
    } else {
        message.appendArg(0);
    }
    message.appendArg(QString::number(pIcon));
    message.appendArg(userNickname);
    message.appendArg(name);
    message.appendArg(userIpAddress);
    message.appendArg(userHostName);
    message.appendArg(userStatus);
    message.appendArg(emptyUserImage ? QByteArray() : pImage);
}


/* Complete a message specially formatted for the User Status Change messages.
*/
void QwsUser::userStatusEntry(QwMessage &message) const
{
    message.appendArg(QString::number(pUserID));
    message.appendArg(QString::number(pIdle));
    if (m_privileges.testFlag(Qws::PrivilegeKickUsers)
        || m_privileges.testFlag(Qws::PrivilegeBanUsers)) {
        message.appendArg(1);
    } else {
        message.appendArg(0);
    }
    message.appendArg(QString::number(pIcon)); // icon, unused since 1.1
    message.appendArg(userNickname);
    message.appendArg(userStatus);
}


/*! This method generates (appends) the fields required for a INFO response to a client.
*/
void QwsUser::userInfoEntry(QwMessage &message) const
{
    message.appendArg(QString::number(pUserID));
    message.appendArg(pIdle ? 1 : 0);
    if (m_privileges.testFlag(Qws::PrivilegeBanUsers)
        || m_privileges.testFlag(Qws::PrivilegeBanUsers)) {
        message.appendArg(1);
    } else {
        message.appendArg(0);
    }
    message.appendArg(QString::number(pIcon));
    message.appendArg(userNickname);
    message.appendArg(name);
    message.appendArg(userIpAddress);
    message.appendArg(userHostName);

    message.appendArg(pClientVersion);
    message.appendArg(pCipherName);
    message.appendArg(QString::number(pCipherBits));
    message.appendArg(pLoginTime.toUTC().toString(Qt::ISODate)+"+00:00");
    message.appendArg(pIdleTime.toUTC().toString(Qt::ISODate)+"+00:00");
    message.appendArg(QString()); // downloads - filled in later
    message.appendArg(QString()); // uploads - filled in later
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
    message.appendArg(m_privileges & Qws::PrivilegeGetUserInfo ? 1 : 0);
    message.appendArg(m_privileges & Qws::PrivilegeSendBroadcast ? 1 : 0);
    message.appendArg(m_privileges & Qws::PrivilegePostNews ? 1 : 0);
    message.appendArg(m_privileges & Qws::PrivilegeClearNews ? 1 : 0);
    message.appendArg(m_privileges & Qws::PrivilegeDownload ? 1 : 0);
    message.appendArg(m_privileges & Qws::PrivilegeUpload ? 1 : 0);
    message.appendArg(m_privileges & Qws::PrivilegeUploadAnywhere ? 1 : 0);
    message.appendArg(m_privileges & Qws::PrivilegeCreateFolders ? 1 : 0);
    message.appendArg(m_privileges & Qws::PrivilegeAlterFiles ? 1 : 0);
    message.appendArg(m_privileges & Qws::PrivilegeDeleteFiles ? 1 : 0);
    message.appendArg(m_privileges & Qws::PrivilegeViewDropboxes ? 1 : 0);
    message.appendArg(m_privileges & Qws::PrivilegeCreateAccounts ? 1 : 0);
    message.appendArg(m_privileges & Qws::PrivilegeEditAccounts ? 1 : 0);
    message.appendArg(m_privileges & Qws::PrivilegeDeleteAccounts ? 1 : 0);
    message.appendArg(m_privileges & Qws::PrivilegeElevatePrivileges ? 1 : 0);
    message.appendArg(m_privileges & Qws::PrivilegeKickUsers ? 1 : 0);
    message.appendArg(m_privileges & Qws::PrivilegeBanUsers ? 1 : 0);
    message.appendArg(m_privileges & Qws::PrivilegeCanNotBeKicked ? 1 : 0);
    message.appendArg(privDownloadSpeed);
    message.appendArg(privUploadSpeed);
    message.appendArg(privDownloadLimit); // wired 1.1
    message.appendArg(privUploadLimit); // wired 1.1
    message.appendArg(m_privileges & Qws::PrivilegeChangeChatTopic ? 1 : 0); // wired 1.1
}


/*! This method sets the user privilege flags from the contents of the EDITUSER message.
*/
void QwsUser::setPrivilegesFromEDITUSER(QwMessage &message, int fieldOffset)
{
    // About fieldOffset:
    //    For EDITUSER we should skip 3 fields
    //    For EDITGROUP we should skip 1 field

    Qws::Privileges newPrivs;
    if (message.stringArg(fieldOffset++).toInt()) { newPrivs |= Qws::PrivilegeGetUserInfo; }
    if (message.stringArg(fieldOffset++).toInt()) { newPrivs |= Qws::PrivilegeSendBroadcast; }
    if (message.stringArg(fieldOffset++).toInt()) { newPrivs |= Qws::PrivilegePostNews; }
    if (message.stringArg(fieldOffset++).toInt()) { newPrivs |= Qws::PrivilegeClearNews; }
    if (message.stringArg(fieldOffset++).toInt()) { newPrivs |= Qws::PrivilegeDownload; }
    if (message.stringArg(fieldOffset++).toInt()) { newPrivs |= Qws::PrivilegeUpload; }
    if (message.stringArg(fieldOffset++).toInt()) { newPrivs |= Qws::PrivilegeUploadAnywhere; }
    if (message.stringArg(fieldOffset++).toInt()) { newPrivs |= Qws::PrivilegeCreateFolders; }
    if (message.stringArg(fieldOffset++).toInt()) { newPrivs |= Qws::PrivilegeAlterFiles; }
    if (message.stringArg(fieldOffset++).toInt()) { newPrivs |= Qws::PrivilegeDeleteFiles; }
    if (message.stringArg(fieldOffset++).toInt()) { newPrivs |= Qws::PrivilegeViewDropboxes; }
    if (message.stringArg(fieldOffset++).toInt()) { newPrivs |= Qws::PrivilegeCreateAccounts; }
    if (message.stringArg(fieldOffset++).toInt()) { newPrivs |= Qws::PrivilegeEditAccounts; }
    if (message.stringArg(fieldOffset++).toInt()) { newPrivs |= Qws::PrivilegeDeleteAccounts; }
    if (message.stringArg(fieldOffset++).toInt()) { newPrivs |= Qws::PrivilegeElevatePrivileges; }
    if (message.stringArg(fieldOffset++).toInt()) { newPrivs |= Qws::PrivilegeKickUsers; }
    if (message.stringArg(fieldOffset++).toInt()) { newPrivs |= Qws::PrivilegeBanUsers; }
    if (message.stringArg(fieldOffset++).toInt()) { newPrivs |= Qws::PrivilegeCanNotBeKicked; }
    this->privDownloadSpeed = message.stringArg(fieldOffset++).toInt();
    this->privUploadSpeed = message.stringArg(fieldOffset++).toInt();
    this->privDownloadLimit = message.stringArg(fieldOffset++).toInt(); // wired 1.1
    this->privUploadLimit = message.stringArg(fieldOffset++).toInt(); // wired 1.1
    if (message.stringArg(fieldOffset++).toInt()) { newPrivs |= Qws::PrivilegeChangeChatTopic; }
    m_privileges = newPrivs;
}

