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
        m_privileges = Qws::PrivilegeGetUserInfo | Qws::PrivilegePostNews
                       | Qws::PrivilegeDownload | Qws::PrivilegeUpload;

	privDownloadSpeed = 0;
	privUploadSpeed = 0;
	privDownloadLimit = 0;
	privUploadLimit = 0;
}


/*! Set the privileges flags from a Qwired(Server) privileges specification. The Qwired
    specification is based on single characters instead of a series of numbers, making it much
    easier to parse and write them even for human beings.
*/
void QwUser::setPrivilegesFromQwiredSpec(const QString privileges)
{
    Qws::Privileges newPrivs = Qws::PrivilegeNoPrivileges;
    if (privileges.contains("a", Qt::CaseSensitive)) { m_privileges = m_privileges | Qws::PrivilegeAlterFiles; }
    if (privileges.contains("B", Qt::CaseSensitive)) { m_privileges = m_privileges | Qws::PrivilegeBanUsers; }
    if (privileges.contains("b", Qt::CaseSensitive)) { m_privileges = m_privileges | Qws::PrivilegeSendBroadcast; }
    if (privileges.contains("P", Qt::CaseSensitive)) { m_privileges = m_privileges | Qws::PrivilegeCanNotBeKicked; }
    if (privileges.contains("t", Qt::CaseSensitive)) { m_privileges = m_privileges | Qws::PrivilegeChangeChatTopic; }
    if (privileges.contains("C", Qt::CaseSensitive)) { m_privileges = m_privileges | Qws::PrivilegeClearNews; }
    if (privileges.contains("A", Qt::CaseSensitive)) { m_privileges = m_privileges | Qws::PrivilegeCreateAccounts; }
    if (privileges.contains("f", Qt::CaseSensitive)) { m_privileges = m_privileges | Qws::PrivilegeCreateFolders; }
    if (privileges.contains("R", Qt::CaseSensitive)) { m_privileges = m_privileges | Qws::PrivilegeDeleteAccounts; }
    if (privileges.contains("r", Qt::CaseSensitive)) { m_privileges = m_privileges | Qws::PrivilegeDeleteFiles; }
    if (privileges.contains("d", Qt::CaseSensitive)) { m_privileges = m_privileges | Qws::PrivilegeDownload; }
    if (privileges.contains("M", Qt::CaseSensitive)) { m_privileges = m_privileges | Qws::PrivilegeEditAccounts; }
    if (privileges.contains("E", Qt::CaseSensitive)) { m_privileges = m_privileges | Qws::PrivilegeElevatePrivileges; }
    if (privileges.contains("i", Qt::CaseSensitive)) { m_privileges = m_privileges | Qws::PrivilegeGetUserInfo; }
    if (privileges.contains("K", Qt::CaseSensitive)) { m_privileges = m_privileges | Qws::PrivilegeKickUsers; }
    if (privileges.contains("p", Qt::CaseSensitive)) { m_privileges = m_privileges | Qws::PrivilegePostNews; }
    if (privileges.contains("u", Qt::CaseSensitive)) { m_privileges = m_privileges | Qws::PrivilegeUpload; }
    if (privileges.contains("U", Qt::CaseSensitive)) { m_privileges = m_privileges | Qws::PrivilegeUploadAnywhere; }
    if (privileges.contains("D", Qt::CaseSensitive)) { m_privileges = m_privileges | Qws::PrivilegeViewDropboxes; }
    m_privileges = newPrivs;
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
    if (m_privileges.testFlag(Qws::PrivilegeAlterFiles)) { privList.append("a"); }
    if (m_privileges.testFlag(Qws::PrivilegeBanUsers)) { privList.append("B"); }
    if (m_privileges.testFlag(Qws::PrivilegeSendBroadcast)) { privList.append("b"); }
    if (m_privileges.testFlag(Qws::PrivilegeCanNotBeKicked)) { privList.append("P"); }
    if (m_privileges.testFlag(Qws::PrivilegeChangeChatTopic)) { privList.append("t"); }
    if (m_privileges.testFlag(Qws::PrivilegeClearNews)) { privList.append("C"); }
    if (m_privileges.testFlag(Qws::PrivilegeCreateAccounts)) { privList.append("A"); }
    if (m_privileges.testFlag(Qws::PrivilegeCreateFolders)) { privList.append("f"); }
    if (m_privileges.testFlag(Qws::PrivilegeDeleteAccounts)) { privList.append("R"); }
    if (m_privileges.testFlag(Qws::PrivilegeDeleteFiles)) { privList.append("r"); }
    if (m_privileges.testFlag(Qws::PrivilegeDownload)) { privList.append("d"); }
    if (m_privileges.testFlag(Qws::PrivilegeEditAccounts)) {  privList.append("M"); }
    if (m_privileges.testFlag(Qws::PrivilegeElevatePrivileges)) { privList.append("E"); }
    if (m_privileges.testFlag(Qws::PrivilegeGetUserInfo)) { privList.append("i"); }
    if (m_privileges.testFlag(Qws::PrivilegeKickUsers)) { privList.append("K"); }
    if (m_privileges.testFlag(Qws::PrivilegePostNews)) { privList.append("p"); }
    if (m_privileges.testFlag(Qws::PrivilegeUpload)) { privList.append("u"); }
    if (m_privileges.testFlag(Qws::PrivilegeUploadAnywhere)) { privList.append("U"); }
    if (m_privileges.testFlag(Qws::PrivilegeViewDropboxes)) { privList.append("D"); }
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



Qws::Privileges QwUser::privileges() const
{ return m_privileges; }


void QwUser::setPrivileges(Qws::Privileges privs)
{ m_privileges = privs; }


