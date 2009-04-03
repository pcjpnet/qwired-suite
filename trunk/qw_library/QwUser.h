#ifndef QWUSER_H
#define QWUSER_H

#include <QHostAddress>
#include <QDateTime>

#include "QwMessage.h"


namespace Qws {
    enum UserType { UserTypeAccount, UserTypeGroup };
}


class QwUser
{

public:
    QwUser();

    void setPrivilegesFromQwiredSpec(const QString privileges);
    QString privilegesFlagsAsQwiredSpec();

    QString cryptedPassword();

    // Account parameters (mainly database)
    QString name;
    QString pPassword;
    QString pGroupName;

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
    QByteArray pDownloads;
    QByteArray pUploads;

    // Privilege flags
    Qws::UserType userType;
    bool privGetUserInfo;
    bool privBroadcast;
    bool privPostNews;
    bool privClearNews;
    bool privDownload;
    bool privUpload;
    bool privUploadAnywhere;
    bool privCreateFolders;
    bool privAlterFiles;
    bool privDeleteFiles;
    bool privViewDropboxes;
    bool privCreateAccounts;
    bool privEditAccounts;
    bool privDeleteAccounts;
    bool privElevatePrivileges;
    bool privKickUsers;
    bool privBanUsers;
    bool privCannotBeKicked;
    int privDownloadSpeed;
    int privUploadSpeed;
    int privDownloadLimit;
    int privUploadLimit;
    bool privChangeTopic;

};

Q_DECLARE_METATYPE(QwUser);

#endif
