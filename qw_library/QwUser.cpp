#include "QwUser.h"

#include <QCryptographicHash>

QwUser::QwUser()
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
        privDownload = true;
        privUpload = true;
	privUploadAnywhere = false;
	privCreateFolders = false;
	privAlterFiles = false;
	privDeleteFiles = false;
	privViewDropboxes = false;;
	privCreateAccounts = false;
	privEditAccounts = false;
	privDeleteAccounts = false;
	privElevatePrivileges = false;
        privKickUsers = false;
        privBanUsers = false;
	privCannotBeKicked = false;
	privDownloadSpeed = 0;
	privUploadSpeed = 0;
	privDownloadLimit = 0;
	privUploadLimit = 0;
	privChangeTopic = false;
}


/*! Set the privileges flags from a Qwired(Server) privileges specification. The Qwired
    specification is based on single characters instead of a series of numbers, making it much
    easier to parse and write them even for human beings.
*/
void QwUser::setPrivilegesFromQwiredSpec(const QString privileges)
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
QString QwUser::privilegesFlagsAsQwiredSpec()
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


/*! This method returns the user's cleartext password as a SHA-1 hash.
*/
QString QwUser::cryptedPassword()
{
    if (pPassword.isEmpty()) {
        return QString();
    }
    QByteArray hashedBinaryData = QCryptographicHash::hash(pPassword.toUtf8(), QCryptographicHash::Sha1);
    return QString::fromUtf8(hashedBinaryData.toHex());
}



