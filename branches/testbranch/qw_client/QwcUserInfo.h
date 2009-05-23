#ifndef QWCUSERINFO_H
#define QWCUSERINFO_H

#include "QwUser.h"
#include <QImage>
#include <QCryptographicHash>

class QwcUserInfo : public QwUser
{

public:
    QImage userImage;

    /*! Create a new user info object from objects as expected from the 308 Client Information message
        and return it.
    */
    static QwcUserInfo fromMessage308(const QwMessage &message)
    {
        int n = 0;
        QwcUserInfo user;
        user.userType = Qws::UserTypeAccount;
        user.pUserID = message.getStringArgument(n++).toInt();
        user.pIdle = message.getStringArgument(n++).toInt();
        user.pAdmin = message.getStringArgument(n++).toInt();
        user.pIcon = message.getStringArgument(n++).toInt();
        user.userNickname = message.getStringArgument(n++);
        user.name = message.getStringArgument(n++);
        user.userIpAddress = message.getStringArgument(n++);
        user.userHostName = message.getStringArgument(n++);
        user.pClientVersion =message.getStringArgument(n++);
        user.pCipherName = message.getStringArgument(n++);
        user.pCipherBits = message.getStringArgument(n++).toInt();
        user.pLoginTime = QDateTime::fromString(message.getStringArgument(n++), Qt::ISODate);
        user.pIdleTime = QDateTime::fromString(message.getStringArgument(n++), Qt::ISODate);
        user.pDownloads = message.getStringArgument(n++).toUtf8();
        user.pUploads = message.getStringArgument(n++).toUtf8();
        user.userStatus = message.getStringArgument(n++);
        user.setImageFromData(QByteArray::fromBase64(message.getStringArgument(10).toAscii()));
        return user;
    };



    /*! Create a new user info object from objects as expected from the 310 User List message
        and return it.
    */
    static QwcUserInfo fromMessage310(const QwMessage &message)
    {
        QwcUserInfo user;
        user.pUserID = message.getStringArgument(1).toInt();
        user.pIdle = message.getStringArgument(2).toInt();
        user.pAdmin = message.getStringArgument(3).toInt();
        user.pIcon = message.getStringArgument(4).toInt();
        user.userNickname = message.getStringArgument(5);
        user.name = message.getStringArgument(6);
        user.userIpAddress = message.getStringArgument(7);
        user.userHostName = message.getStringArgument(8);
        user.userStatus = message.getStringArgument(9);
        user.setImageFromData(QByteArray::fromBase64(message.getStringArgument(10).toAscii()));
        return user;


    };


    /*! Set the user privileges from a 602 privileges message.
    */
    void setPrivilegesFromMessage602(const QwMessage &message)
    {
        int n = 0;
        privGetUserInfo = message.getStringArgument(n++).toInt();
        privBroadcast = message.getStringArgument(n++).toInt();
        privPostNews = message.getStringArgument(n++).toInt();
        privClearNews = message.getStringArgument(n++).toInt();
        privDownload = message.getStringArgument(n++).toInt();
        privUpload = message.getStringArgument(n++).toInt();
        privUploadAnywhere = message.getStringArgument(n++).toInt();
        privCreateFolders = message.getStringArgument(n++).toInt();
        privAlterFiles = message.getStringArgument(n++).toInt();
        privDeleteFiles = message.getStringArgument(n++).toInt();
        privViewDropboxes = message.getStringArgument(n++).toInt();
        privCreateAccounts = message.getStringArgument(n++).toInt();
        privEditAccounts = message.getStringArgument(n++).toInt();
        privDeleteAccounts = message.getStringArgument(n++).toInt();
        privElevatePrivileges = message.getStringArgument(n++).toInt();
        privKickUsers = message.getStringArgument(n++).toInt();
        privBanUsers = message.getStringArgument(n++).toInt();
        privCannotBeKicked = message.getStringArgument(n++).toInt();
        privDownloadSpeed = message.getStringArgument(n++).toInt();
        privUploadSpeed = message.getStringArgument(n++).toInt();
        privDownloadLimit = message.getStringArgument(n++).toInt();
        privUploadLimit = message.getStringArgument(n++).toInt();
        privChangeTopic = message.getStringArgument(n++).toInt();
    };


    /*! Append privileges flags to the end of \a message to be used in CREATExy-type messages.
    */
    void appendPrivilegesFlags(QwMessage &message)
    {

        // Don't set any flags if the user is part of a group.
        if (userType == Qws::UserTypeAccount && !pGroupName.isEmpty()) {
            for (int i=0; i<23; i++) { message.appendArg("0"); }
        }

        message.appendArg(QString::number(privGetUserInfo));
        message.appendArg(QString::number(privBroadcast));
        message.appendArg(QString::number(privPostNews));
        message.appendArg(QString::number(privClearNews));
        message.appendArg(QString::number(privDownload));
        message.appendArg(QString::number(privUpload));
        message.appendArg(QString::number(privUploadAnywhere));
        message.appendArg(QString::number(privCreateFolders));;
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

    QString cryptedPassword()
    {
        if( pPassword.isEmpty() )
            return QString("");
        QByteArray tmpDat = QCryptographicHash::hash(pPassword.toUtf8(), QCryptographicHash::Sha1);
        return QString::fromUtf8(tmpDat.toHex());
    };


    /*! Set the user image from PNG data and return true if the task was successful.
    */
    bool setImageFromData(const QByteArray data)
    {
        return userImage.loadFromData(data);
    };

};

Q_DECLARE_METATYPE(QwcUserInfo);

#endif
