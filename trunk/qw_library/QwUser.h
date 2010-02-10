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

    void setPrivilegesFromQwiredSpec(const QString privileges);
    QString privilegesFlagsAsQwiredSpec();

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

    // Runtime user information


    int pIcon; // warning: unused in 1.1


    QByteArray pImage;
    QString userIpAddress;
    QString userHostName;

    // Extended user info (get info)
    QString pClientVersion;
    QString pCipherName;
    int pCipherBits;
    QDateTime pLoginTime;
    QDateTime pIdleTime;

    // Privilege flags

    int privDownloadSpeed;
    int privUploadSpeed;
    int privDownloadLimit;
    int privUploadLimit;


    Qws::Privileges privileges() const;
    void setPrivileges(Qws::Privileges privs);

protected:
    Qws::Privileges m_privileges;
    QString m_login;
    QString m_password;
    QString m_group;
    QString m_status;
    QString m_nickname;
    bool m_idleFlag;
    Qws::UserType m_type;
    int m_userId;
};

Q_DECLARE_METATYPE(QwUser);

#endif
