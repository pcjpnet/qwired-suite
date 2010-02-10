#include "QwUser.h"

#include <QtCore/QCryptographicHash>

QwUser::QwUser()
{
    m_userId = -1;
    m_idleFlag = false;
    m_type = Qws::UserTypeAccount;

    // Default privs
    m_privileges = Qws::PrivilegeGetUserInfo | Qws::PrivilegePostNews
                   | Qws::PrivilegeDownload | Qws::PrivilegeUpload;

    m_downloadSpeedLimit = 0;
    m_uploadSpeedLimit = 0;
    m_downloadLimit = 0;
    m_uploadLimit = 0;
}


/*! Set the privileges flags from a Qwired(Server) privileges specification. The Qwired
    specification is based on single characters instead of a series of numbers, making it much
    easier to parse and write them even for human beings.
*/
void QwUser::setPrivilegesFromQwiredSpec(const QString privileges)
{
    Qws::Privileges newPrivs = Qws::PrivilegeNoPrivileges;
    if (privileges.contains("a", Qt::CaseSensitive)) { newPrivs |= Qws::PrivilegeAlterFiles; }
    if (privileges.contains("B", Qt::CaseSensitive)) { newPrivs |= Qws::PrivilegeBanUsers; }
    if (privileges.contains("b", Qt::CaseSensitive)) { newPrivs |= Qws::PrivilegeSendBroadcast; }
    if (privileges.contains("P", Qt::CaseSensitive)) { newPrivs |= Qws::PrivilegeCanNotBeKicked; }
    if (privileges.contains("t", Qt::CaseSensitive)) { newPrivs |= Qws::PrivilegeChangeChatTopic; }
    if (privileges.contains("C", Qt::CaseSensitive)) { newPrivs |= Qws::PrivilegeClearNews; }
    if (privileges.contains("A", Qt::CaseSensitive)) { newPrivs |= Qws::PrivilegeCreateAccounts; }
    if (privileges.contains("f", Qt::CaseSensitive)) { newPrivs |= Qws::PrivilegeCreateFolders; }
    if (privileges.contains("R", Qt::CaseSensitive)) { newPrivs |= Qws::PrivilegeDeleteAccounts; }
    if (privileges.contains("r", Qt::CaseSensitive)) { newPrivs |= Qws::PrivilegeDeleteFiles; }
    if (privileges.contains("d", Qt::CaseSensitive)) { newPrivs |= Qws::PrivilegeDownload; }
    if (privileges.contains("M", Qt::CaseSensitive)) { newPrivs |= Qws::PrivilegeEditAccounts; }
    if (privileges.contains("E", Qt::CaseSensitive)) { newPrivs |= Qws::PrivilegeElevatePrivileges; }
    if (privileges.contains("i", Qt::CaseSensitive)) { newPrivs |= Qws::PrivilegeGetUserInfo; }
    if (privileges.contains("K", Qt::CaseSensitive)) { newPrivs |= Qws::PrivilegeKickUsers; }
    if (privileges.contains("p", Qt::CaseSensitive)) { newPrivs |= Qws::PrivilegePostNews; }
    if (privileges.contains("u", Qt::CaseSensitive)) { newPrivs |= Qws::PrivilegeUpload; }
    if (privileges.contains("U", Qt::CaseSensitive)) { newPrivs |= Qws::PrivilegeUploadAnywhere; }
    if (privileges.contains("D", Qt::CaseSensitive)) { newPrivs |= Qws::PrivilegeViewDropboxes; }
    m_privileges = newPrivs;
    qDebug() << newPrivs;
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


int QwUser::userId() const
{ return m_userId; }

void QwUser::setUserId(int id)
{ m_userId = id; }

QString QwUser::loginName() const
{ return m_login; }

void QwUser::setLoginName(const QString &login)
{ m_login = login; }

QString QwUser::groupName() const
{ return m_group; }

void QwUser::setGroupName(const QString &group)
{ m_group = group; }

QString QwUser::nickname() const
{ return m_nickname; }

void QwUser::setNickname(const QString &name)
{ m_nickname = name; }

QString QwUser::status() const
{ return m_status; }

void QwUser::setStatus(const QString &status)
{ m_status = status; }

Qws::UserType QwUser::type() const
{ return m_type; }

void QwUser::setType(Qws::UserType type)
{ m_type = type; }

bool QwUser::isIdle() const
{ return m_idleFlag; }

void QwUser::setIdle(bool flag)
{ m_idleFlag = flag; }

bool QwUser::isAdmin() const
{ return m_privileges & Qws::PrivilegeKickUsers || m_privileges & Qws::PrivilegeBanUsers; }

QString QwUser::password() const
{ return m_password; }

void QwUser::setCryptedPassword(const QString &password)
{ m_password = password; }

void QwUser::setCleartextPassword(const QString &password)
{
    m_password = QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha1).toHex();
}

Qws::Privileges QwUser::privileges() const
{ return m_privileges; }


void QwUser::setPrivileges(Qws::Privileges privs)
{ m_privileges = privs; }

QDateTime QwUser::lastActivity() const
{ return m_lastActivity; }

void QwUser::setLastActivity(QDateTime when)
{ m_lastActivity = when; }

QDateTime QwUser::loginTime() const
{ return m_loginTime; }

void QwUser::setLoginTime(QDateTime when)
{ m_loginTime = when; }

QString QwUser::cipherName() const
{ return m_clientCipherName; }

void QwUser::setClientCipherName(const QString &name)
{ m_clientCipherName = name; }

QString QwUser::clientVersion() const
{ return m_clientVersion; }

void QwUser::setClientVersion(const QString &name)
{ m_clientVersion = name; }

QString QwUser::clientIpAddress() const
{ return m_clientIpAddress; }

void QwUser::setClientIpAddress(const QString &ip)
{ m_clientIpAddress = ip; }

QString QwUser::clientHostName() const
{ return m_clientHostName; }

void QwUser::setClientHostName(const QString &name)
{ m_clientHostName = name; }

int QwUser::clientCipherBits() const
{ return m_clientCipherBits; }

void QwUser::setClientCipherBits(int bits)
{ m_clientCipherBits = bits; }

QByteArray QwUser::iconData() const
{ return m_iconData; }

void QwUser::setIconData(const QByteArray &data)
{ m_iconData = data; }

int QwUser::downloadSpeedLimit() const
{ return m_downloadLimit; }

void QwUser::setDownloadSpeedLimit(int limit)
{ m_downloadSpeedLimit = limit; }

int QwUser::uploadSpeedLimit() const
{ return m_uploadSpeedLimit; }

void QwUser::setUploadSpeedLimit(int limit)
{ m_uploadSpeedLimit = limit; }

int QwUser::uploadLimit() const
{ return m_uploadLimit; }

void QwUser::setUploadLimit(int limit)
{ m_uploadLimit = limit; }

int QwUser::downloadLimit() const
{ return m_downloadLimit; }

void QwUser::setDownloadLimit(int limit)
{ m_downloadLimit = limit; }
