#ifndef QWUSER_H
#define QWUSER_H


#include <QtNetwork/QHostAddress>
#include <QtCore/QDateTime>
#include <QtCore/QString>
#include <QtCore/QMetaType>


namespace Qws {
    enum UserType {
        UserTypeAccount,
        UserTypeGroup
    };
    enum Privilege {
        PrivilegeNoPrivileges = 0,
        PrivilegeGetUserInfo = 1,
        PrivilegeSendBroadcast = 2,
        PrivilegePostNews = 4,
        PrivilegeClearNews = 8,
        PrivilegeDownload = 16,
        PrivilegeUpload = 32,
        PrivilegeUploadAnywhere = 64,
        PrivilegeCreateFolders = 128,
        PrivilegeAlterFiles = 256,
        PrivilegeDeleteFiles = 512,
        PrivilegeViewDropboxes = 1024,
        PrivilegeCreateAccounts = 2048,
        PrivilegeEditAccounts = 4096,
        PrivilegeDeleteAccounts = 8192,
        PrivilegeElevatePrivileges = 16384,
        PrivilegeKickUsers = 32768,
        PrivilegeBanUsers = 65536,
        PrivilegeCanNotBeKicked = 131072,
        PrivilegeChangeChatTopic = 262144
    };
    Q_DECLARE_FLAGS(Privileges, Privilege);
}
Q_DECLARE_OPERATORS_FOR_FLAGS(Qws::Privileges);



class QwUser
{

public:
    QwUser();

    bool isNull() const;

    int userId() const;
    void setUserId(int id);

    QString loginName() const;
    void setLoginName(const QString &login);

    QString groupName() const;
    void setGroupName(const QString &group);

    QString nickname() const;
    void setNickname(const QString &name);

    QString status() const;
    void setStatus(const QString &status);

    Qws::UserType type() const;
    void setType(Qws::UserType type);

    bool isIdle() const;
    void setIdle(bool flag);

    bool isAdmin() const;

    QString password() const;
    void setCryptedPassword(const QString &password);
    void setCleartextPassword(const QString &password);

    Qws::Privileges privileges() const;
    void setPrivileges(Qws::Privileges privs);

    QDateTime lastActivity() const;
    void setLastActivity(QDateTime when);

    QDateTime loginTime() const;
    void setLoginTime(QDateTime when);

    QString cipherName() const;
    void setClientCipherName(const QString &name);

    QString clientVersion() const;
    void setClientVersion(const QString &name);

    QString clientIpAddress() const;
    void setClientIpAddress(const QString &ip);

    QString clientHostName() const;
    void setClientHostName(const QString &name);

    int clientCipherBits() const;
    void setClientCipherBits(int bits);

    QByteArray iconData() const;
    void setIconData(const QByteArray &data);

    int downloadSpeedLimit() const;
    void setDownloadSpeedLimit(int limit);

    int uploadSpeedLimit() const;
    void setUploadSpeedLimit(int limit);

    int uploadLimit() const;
    void setUploadLimit(int limit);

    int downloadLimit() const;
    void setDownloadLimit(int limit);

protected:
    bool m_isNull;

    Qws::Privileges m_privileges;
    QString m_login;
    QString m_password;
    QString m_group;
    QString m_status;
    QString m_nickname;
    bool m_idleFlag;
    Qws::UserType m_type;
    int m_userId;
    QDateTime m_lastActivity;
    QDateTime m_loginTime;
    QByteArray m_iconData;

    int m_downloadSpeedLimit;
    int m_uploadSpeedLimit;
    int m_downloadLimit;
    int m_uploadLimit;

    QString m_clientIpAddress;
    QString m_clientHostName;
    int m_clientCipherBits;
    QString m_clientCipherName;
    QString m_clientVersion;
};

Q_DECLARE_METATYPE(QwUser);

#endif
