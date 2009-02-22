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


#ifndef CLASSWIREDUSER_H
#define CLASSWIREDUSER_H

#include <QHostAddress>
#include <QDateTime>

#include "QwMessage.h"

/**
        @author Bastian Bense <bastibense@gmail.com>
 */
class ClassWiredUser
{
public:
    ClassWiredUser();
    ClassWiredUser ( QList<QByteArray> theParams );
    static ClassWiredUser fromUserInfo( QList<QByteArray> theParams );
    ~ClassWiredUser();



    void setFromPrivileges(QList<QByteArray> theParams);
    void setPrivilegesFromAccount(const QString privileges);

    void privilegesFlags(QwMessage &message) const;
    void userListEntry(QwMessage &message) const;
    void userStatusEntry(QwMessage &message) const;
    void userInfoEntry(QwMessage &message) const;

    void setPrivilegesFromQwiredSpec(const QString privileges);
    QString privilegesFlagsAsQwiredSpec();
    void appendPrivilegeFlagsForREADUSER(QwMessage &message);
    void setPrivilegesFromEDITUSER(QwMessage &message, int fieldOffset);

    QString cryptedPassword();

    int pUserID;
    bool pIdle;
    bool pAdmin;
    int pIcon; /* unused in 1.1 */
    QString userNickname; // Nickname of user, or name of group.
    QString userLogin;
    QString userIpAddress;
    QString userHostName;
    QString userStatus;
    QByteArray pImage;

    // Session specific
    QString pPassword;

    // Extended user info (get info)
    QString pClientVersion;
    QString pCipherName;
    int pCipherBits;
    QDateTime pLoginTime;
    QDateTime pIdleTime;
    QByteArray pDownloads;
    QByteArray pUploads;

    QString pGroupName; // Only in 600 User Specification

    // Privileges management
    int pAccountType; // 0 = user, 1 = group
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

#endif
