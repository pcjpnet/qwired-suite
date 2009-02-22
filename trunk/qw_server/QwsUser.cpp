/***************************************************************************
 * QWired - a free Wired client.
 * Copyright (C) 2008 Bastian Bense
 ***************************************************************************
 * This program is free software; you can redistribute it and/or modify it *
 * under the terms of the GNU General Public License as published by the   *
 * Free Software Foundation; either version 2 of the License, or           *
 * (at your option) any later version.                                     *
 *                                                                         *
 * This program is distributed in the hope that it will be useful, but     *
 * WITHOUT ANY WARRANTY; without even the implied warranty of              *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU        *
 * General Public License for more details.                                *
 *                                                                         *
 * You should have received a copy of the GNU General Public License along *
 * with this program; if not, write to the                                 *
 * Free Software Foundation, Inc.,                                         *
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA                   *
 ***************************************************************************/
 
#include "QwsUser.h"

#include <QtSql>
#include <QCryptographicHash>

QwsUser::QwsUser()
{
	pUserID = 0;
	pIdle = false;
	pAdmin = false;
	pIcon = 0;
        userType = Qws::UserTypeAccount;

	// Default privs
	privGetUserInfo = true;
	privBroadcast = false;
	privPostNews = true;
	privClearNews = false;
	privDownload = false;
	privUpload = false;
	privUploadAnywhere = false;
	privCreateFolders = false;
	privAlterFiles = false;
	privDeleteFiles = false;
	privViewDropboxes = false;;
	privCreateAccounts = false;
	privEditAccounts = false;
	privDeleteAccounts = false;
	privElevatePrivileges = false;
	privKickUsers = true;
	privBanUsers = true;
	privCannotBeKicked = false;
	privDownloadSpeed = 0;
	privUploadSpeed = 0;
	privDownloadLimit = 0;
	privUploadLimit = 0;
	privChangeTopic = false;
}


QwsUser::~QwsUser()
{ }


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
        query.prepare("SELECT id, acc_secret, acc_privileges, acc_group, acc_maxupload, acc_maxdownload "
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
            this->setPrivilegesFromQwiredSpec(query.value(2).toString());
            this->pGroupName = query.value(3).toString();
            this->privUploadLimit = query.value(4).toInt();
            this->privDownloadLimit = query.value(5).toInt();
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
                      "SET acc_secret=:_secret, acc_privileges=:_privileges, acc_group=:_group, acc_maxdownload=:_maxdown, acc_maxupload=:_maxup "
                      "WHERE acc_name=:_name");
        query.bindValue(":_name", this->name);
        query.bindValue(":_secret", this->pPassword);
        query.bindValue(":_group", this->pGroupName);
        query.bindValue(":_maxdown", this->privDownloadLimit);
        query.bindValue(":_maxup", this->privUploadLimit);
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


/*! Set the privileges flags from a Qwired(Server) privileges specification. The Qwired
    specification is based on single characters instead of a series of numbers, making it much
    easier to parse and write them even for human beings.
*/
void QwsUser::setPrivilegesFromQwiredSpec(const QString privileges)
{
    this->privAlterFiles = privileges.contains("a", Qt::CaseSensitive);
    this->privBanUsers = privileges.contains("B", Qt::CaseSensitive);
    this->privBroadcast = privileges.contains("b", Qt::CaseSensitive);
    this->privCannotBeKicked = privileges.contains("P", Qt::CaseSensitive);
    this->privChangeTopic = privileges.contains("t", Qt::CaseSensitive);
    this->privClearNews = privileges.contains("C", Qt::CaseSensitive);
    this->privCreateAccounts = privileges.contains("A", Qt::CaseSensitive);
    this->privCreateFolders = privileges.contains("f", Qt::CaseSensitive);
    this->privDeleteAccounts = privileges.contains("R", Qt::CaseSensitive);
    this->privDeleteFiles = privileges.contains("r", Qt::CaseSensitive);
    this->privDownload = privileges.contains("d", Qt::CaseSensitive);
    this->privEditAccounts = privileges.contains("M", Qt::CaseSensitive);
    this->privElevatePrivileges = privileges.contains("E", Qt::CaseSensitive);
    this->privGetUserInfo = privileges.contains("i", Qt::CaseSensitive);
    this->privKickUsers = privileges.contains("K", Qt::CaseSensitive);
    this->privPostNews = privileges.contains("p", Qt::CaseSensitive);
    this->privUpload = privileges.contains("u", Qt::CaseSensitive);
    this->privUploadAnywhere = privileges.contains("U", Qt::CaseSensitive);
    this->privViewDropboxes = privileges.contains("D", Qt::CaseSensitive);
}


/*! This method returns the privileges flags as a series of Qwired Specification flags.
    Please note that this does not include the numeric speed limits, as those are stored
    independently from the normal flags.
    I saved some nerves by not following the coding guidelines for brackets here, I hope you
    forgive me. :)
*/
QString QwsUser::privilegesFlagsAsQwiredSpec()
{
    QString privList;
    if (this->privAlterFiles) privList.append("a");
    if (this->privBanUsers) privList.append("B");
    if (this->privBroadcast) privList.append("b");
    if (this->privCannotBeKicked) privList.append("P");
    if (this->privChangeTopic) privList.append("t");
    if (this->privClearNews) privList.append("C");
    if (this->privCreateAccounts) privList.append("A");
    if (this->privCreateFolders) privList.append("f");
    if (this->privDeleteAccounts) privList.append("R");
    if (this->privDeleteFiles) privList.append("r");
    if (this->privDownload) privList.append("d");
    if (this->privEditAccounts) privList.append("M");
    if (this->privElevatePrivileges) privList.append("E");
    if (this->privGetUserInfo) privList.append("i");
    if (this->privKickUsers) privList.append("K");
    if (this->privPostNews) privList.append("p");
    if (this->privUpload) privList.append("u");
    if (this->privUploadAnywhere) privList.append("U");
    if (this->privViewDropboxes) privList.append("D");
    return privList;
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
    this->privDownloadLimit = message.getStringArgument(fieldOffset++).toInt();
    this->privUploadLimit = message.getStringArgument(fieldOffset++).toInt();
    this->privChangeTopic = message.getStringArgument(fieldOffset++).toInt();
}



/*! This method generates (appends) the fields required for a INFO response to a client.
*/
void QwsUser::userInfoEntry(QwMessage &message) const
{
    message.appendArg(QString::number(pUserID));
    message.appendArg(pIdle ? "1" : "0");
    message.appendArg(pAdmin ? "1" : "0");
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
    message.appendArg(""); // downloads
    message.appendArg(""); // uploads
    message.appendArg(userStatus);
    message.appendArg(pImage);

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


/*! This method returns the user's cleartext password as a SHA-1 hash.
*/
QString QwsUser::cryptedPassword()
{
    if (pPassword.isEmpty()) {
        return QString();
    }
    QByteArray hashedBinaryData = QCryptographicHash::hash(pPassword.toUtf8(), QCryptographicHash::Sha1);
    return QString::fromUtf8(hashedBinaryData.toHex());
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


