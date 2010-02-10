#ifndef QWUSER_H
#define QWUSER_H


#include <QtNetwork/QHostAddress>
#include <QtCore/QDateTime>
#include <QtCore/QString>
#include <QtCore/QMetaType>


namespace Qws {
    enum UserType { UserTypeAccount,
                    UserTypeGroup };

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

    QString cryptedPassword();

    // Account parameters (mainly database)
    QString login;
    QString password;
    QString group;

    // Runtime user information
    bool pIdle;
    bool pAdmin;
    int pUserID;
    int pIcon; // warning: unused in 1.1
    QString userNickname;
    QString userStatus;
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
    Qws::UserType userType;
    int privDownloadSpeed;
    int privUploadSpeed;
    int privDownloadLimit;
    int privUploadLimit;


    Qws::Privileges privileges() const;
    void setPrivileges(Qws::Privileges privs);

protected:
    Qws::Privileges m_privileges;

};

Q_DECLARE_METATYPE(QwUser);

#endif
