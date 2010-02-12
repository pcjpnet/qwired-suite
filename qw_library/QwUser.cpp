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

    m_isNull = true;
}


bool QwUser::isNull() const
{ return m_isNull; }

int QwUser::userId() const
{ return m_userId; }

void QwUser::setUserId(int id)
{
    m_userId = id;
    m_isNull = false;
}

QString QwUser::loginName() const
{ return m_login; }

void QwUser::setLoginName(const QString &login)
{
    m_login = login;
    m_isNull = false;
}

QString QwUser::groupName() const
{ return m_group; }

void QwUser::setGroupName(const QString &group)
{
    m_group = group;
    m_isNull = false;
}

QString QwUser::nickname() const
{ return m_nickname; }

void QwUser::setNickname(const QString &name)
{
    m_nickname = name;
    m_isNull = false;
}

QString QwUser::status() const
{ return m_status; }

void QwUser::setStatus(const QString &status)
{
    m_status = status;
    m_isNull = false;
}

Qws::UserType QwUser::type() const
{ return m_type; }

void QwUser::setType(Qws::UserType type)
{
    m_type = type;
    m_isNull = false;
}

bool QwUser::isIdle() const
{ return m_idleFlag; }

void QwUser::setIdle(bool flag)
{
    m_idleFlag = flag;
    m_isNull = false;
}

bool QwUser::isAdmin() const
{ return m_privileges & Qws::PrivilegeKickUsers || m_privileges & Qws::PrivilegeBanUsers; }

QString QwUser::password() const
{ return m_password; }

void QwUser::setCryptedPassword(const QString &password)
{
    m_password = password;
    m_isNull = false;
}

void QwUser::setCleartextPassword(const QString &password)
{
    m_password = QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha1).toHex();
    m_isNull = false;
}

Qws::Privileges QwUser::privileges() const
{ return m_privileges; }


void QwUser::setPrivileges(Qws::Privileges privs)
{
    m_privileges = privs;
    m_isNull = false;
}

QDateTime QwUser::lastActivity() const
{ return m_lastActivity; }

void QwUser::setLastActivity(QDateTime when)
{
    m_lastActivity = when;
    m_isNull = false;
}

QDateTime QwUser::loginTime() const
{ return m_loginTime; }

void QwUser::setLoginTime(QDateTime when)
{
    m_loginTime = when;
    m_isNull = false;
}

QString QwUser::cipherName() const
{ return m_clientCipherName; }

void QwUser::setClientCipherName(const QString &name)
{
    m_clientCipherName = name;
    m_isNull = false;
}

QString QwUser::clientVersion() const
{ return m_clientVersion; }

void QwUser::setClientVersion(const QString &name)
{
    m_clientVersion = name;
    m_isNull = false;
}

QString QwUser::clientIpAddress() const
{ return m_clientIpAddress; }

void QwUser::setClientIpAddress(const QString &ip)
{
    m_clientIpAddress = ip;
    m_isNull = false;
}

QString QwUser::clientHostName() const
{ return m_clientHostName; }

void QwUser::setClientHostName(const QString &name)
{
    m_clientHostName = name;
    m_isNull = false;
}

int QwUser::clientCipherBits() const
{ return m_clientCipherBits; }

void QwUser::setClientCipherBits(int bits)
{
    m_clientCipherBits = bits;
    m_isNull = false;
}

QByteArray QwUser::iconData() const
{ return m_iconData; }

void QwUser::setIconData(const QByteArray &data)
{
    m_iconData = data;
    m_isNull = false;
}

int QwUser::downloadSpeedLimit() const
{ return m_downloadSpeedLimit; }

void QwUser::setDownloadSpeedLimit(int limit)
{
    m_downloadSpeedLimit = limit;
    m_isNull = false;
}

int QwUser::uploadSpeedLimit() const
{ return m_uploadSpeedLimit; }

void QwUser::setUploadSpeedLimit(int limit)
{
    m_uploadSpeedLimit = limit;
    m_isNull = false;
}

int QwUser::uploadLimit() const
{ return m_uploadLimit; }

void QwUser::setUploadLimit(int limit)
{
    m_uploadLimit = limit;
    m_isNull = false;
}

int QwUser::downloadLimit() const
{ return m_downloadLimit; }

void QwUser::setDownloadLimit(int limit)
{
    m_downloadLimit = limit;
    m_isNull = false;
}
