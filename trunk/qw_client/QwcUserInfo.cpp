#include "QwcUserInfo.h"
#include "QwMessage.h"
#include <QtGui/QPixmap>

QwcUserInfo::QwcUserInfo() :
        QwUser()
{

}



/*! Create a new user info object from objects as expected from the 308 Client Information message
    and return it.
*/
QwcUserInfo QwcUserInfo::fromMessage308(const QwMessage *message)
{
    int n = 0;
    QwcUserInfo user;
    user.setType(Qws::UserTypeAccount);

    user.setUserId(message->stringArg(n++).toInt());
    user.setIdle(message->stringArg(n++).toInt());
    if (message->stringArg(n++).toInt()) {
        user.setPrivileges(Qws::PrivilegeKickUsers);
    }
    user.pIcon = message->stringArg(n++).toInt();
    user.setNickname(message->stringArg(n++));
    user.setLoginName(message->stringArg(n++));
    user.userIpAddress = message->stringArg(n++);
    user.userHostName = message->stringArg(n++);
    user.pClientVersion =message->stringArg(n++);
    user.pCipherName = message->stringArg(n++);
    user.pCipherBits = message->stringArg(n++).toInt();
    user.pLoginTime = QDateTime::fromString(message->stringArg(n++), Qt::ISODate);
    user.pIdleTime = QDateTime::fromString(message->stringArg(n++), Qt::ISODate);

    user.parseTransfersFromData(message->stringArg(n++).toUtf8(), Qw::TransferTypeDownload);
    user.parseTransfersFromData(message->stringArg(n++).toUtf8(), Qw::TransferTypeUpload);
    user.setStatus(message->stringArg(n++));
    user.setImageFromData(QByteArray::fromBase64(message->stringArg(10).toAscii()));
    return user;
}


/*! Create a new user info object from objects as expected from the 310 User List message
    and return it.
*/
QwcUserInfo QwcUserInfo::fromMessage310(const QwMessage *message)
{
    QwcUserInfo user;
    user.setUserId(message->stringArg(1).toInt());
    user.setIdle(message->stringArg(2).toInt());
    if (message->stringArg(3).toInt()) {
        user.setPrivileges(Qws::PrivilegeKickUsers);
    }
    user.pIcon = message->stringArg(4).toInt();
    user.setNickname(message->stringArg(5));
    user.setLoginName(message->stringArg(6));
    user.userIpAddress = message->stringArg(7);
    user.userHostName = message->stringArg(8);
    user.setStatus(message->stringArg(9));
    user.setImageFromData(QByteArray::fromBase64(message->stringArg(10).toAscii()));
    return user;
}


QList<QwTransferInfo> & QwcUserInfo::transfers()
{ return m_transfers; }


const QList<QwTransferInfo> & QwcUserInfo::transfers() const
{ return m_transfers; }

/*! Parses a list of transfers from \a data and sets their type to \a type.
*/
void QwcUserInfo::parseTransfersFromData(const QByteArray &data, Qw::TransferType type)
{
    qDebug() << "Transfer:" << data;

    QList<QByteArray> records = data.split('\x1D');
    foreach (QByteArray record, records) {
        if (record.isEmpty()) { continue; }
        QList<QByteArray> fields = record.split('\x1E');
        QwTransferInfo transfer;
        transfer.type = type;
        transfer.file.setRemotePath(QString::fromUtf8(fields[0]));
        transfer.bytesTransferred = fields[1].toLongLong();
        transfer.file.setSize(fields[2].toLongLong());
        transfer.currentTransferSpeed = fields[3].toLongLong();
        m_transfers << transfer;
    }
}


/*! Set the user privileges from a 602 privileges message.
*/
void QwcUserInfo::setPrivilegesFromMessage602(const QwMessage *message)
{
    int offset = 0;
    Qws::Privileges privileges;
    if (message->stringArg(offset++).toInt()) { privileges |= Qws::PrivilegeGetUserInfo; }
    if (message->stringArg(offset++).toInt()) { privileges |= Qws::PrivilegeSendBroadcast; }
    if (message->stringArg(offset++).toInt()) { privileges |= Qws::PrivilegePostNews; }
    if (message->stringArg(offset++).toInt()) { privileges |= Qws::PrivilegeClearNews; }
    if (message->stringArg(offset++).toInt()) { privileges |= Qws::PrivilegeDownload; }
    if (message->stringArg(offset++).toInt()) { privileges |= Qws::PrivilegeUpload; }
    if (message->stringArg(offset++).toInt()) { privileges |= Qws::PrivilegeUploadAnywhere; }
    if (message->stringArg(offset++).toInt()) { privileges |= Qws::PrivilegeCreateFolders; }
    if (message->stringArg(offset++).toInt()) { privileges |= Qws::PrivilegeAlterFiles; }
    if (message->stringArg(offset++).toInt()) { privileges |= Qws::PrivilegeDeleteFiles; }
    if (message->stringArg(offset++).toInt()) { privileges |= Qws::PrivilegeViewDropboxes; }
    if (message->stringArg(offset++).toInt()) { privileges |= Qws::PrivilegeCreateAccounts; }
    if (message->stringArg(offset++).toInt()) { privileges |= Qws::PrivilegeEditAccounts; }
    if (message->stringArg(offset++).toInt()) { privileges |= Qws::PrivilegeDeleteAccounts; }
    if (message->stringArg(offset++).toInt()) { privileges |= Qws::PrivilegeElevatePrivileges; }
    if (message->stringArg(offset++).toInt()) { privileges |= Qws::PrivilegeKickUsers; }
    if (message->stringArg(offset++).toInt()) { privileges |= Qws::PrivilegeBanUsers; }
    if (message->stringArg(offset++).toInt()) { privileges |= Qws::PrivilegeCanNotBeKicked; }
    privDownloadSpeed = message->stringArg(offset++).toInt();
    privUploadSpeed = message->stringArg(offset++).toInt();
    privDownloadLimit = message->stringArg(offset++).toInt();
    privUploadLimit = message->stringArg(offset++).toInt();
    if (message->stringArg(offset++).toInt()) { privileges |= Qws::PrivilegeChangeChatTopic; }

    m_privileges = privileges;

}


/*! Append privileges flags to the end of \a message to be used in CREATExy-type messages.
*/
void QwcUserInfo::appendPrivilegesFlags(QwMessage *message) const
{
    // Don't set any flags if the user is part of a group.
    if (type() == Qws::UserTypeAccount && !groupName().isEmpty()) {
        for (int i = 0; i < 23; i++) {
            message->appendArg(0);
        }
    }

    message->appendArg(m_privileges & Qws::PrivilegeGetUserInfo ? 1 : 0);
    message->appendArg(m_privileges & Qws::PrivilegeSendBroadcast ? 1 : 0);
    message->appendArg(m_privileges & Qws::PrivilegePostNews ? 1 : 0);
    message->appendArg(m_privileges & Qws::PrivilegeClearNews ? 1 : 0);
    message->appendArg(m_privileges & Qws::PrivilegeDownload ? 1 : 0);
    message->appendArg(m_privileges & Qws::PrivilegeUpload ? 1 : 0);
    message->appendArg(m_privileges & Qws::PrivilegeUploadAnywhere ? 1 : 0);
    message->appendArg(m_privileges & Qws::PrivilegeCreateFolders ? 1 : 0);
    message->appendArg(m_privileges & Qws::PrivilegeAlterFiles ? 1 : 0);
    message->appendArg(m_privileges & Qws::PrivilegeDeleteFiles ? 1 : 0);
    message->appendArg(m_privileges & Qws::PrivilegeViewDropboxes ? 1 : 0);
    message->appendArg(m_privileges & Qws::PrivilegeCreateAccounts ? 1 : 0);
    message->appendArg(m_privileges & Qws::PrivilegeEditAccounts ? 1 : 0);
    message->appendArg(m_privileges & Qws::PrivilegeDeleteAccounts ? 1 : 0);
    message->appendArg(m_privileges & Qws::PrivilegeElevatePrivileges ? 1 : 0);
    message->appendArg(m_privileges & Qws::PrivilegeKickUsers ? 1 : 0);
    message->appendArg(m_privileges & Qws::PrivilegeBanUsers ? 1 : 0);
    message->appendArg(m_privileges & Qws::PrivilegeCanNotBeKicked ? 1 : 0);
    message->appendArg(privDownloadSpeed);
    message->appendArg(privUploadSpeed);
    message->appendArg(privDownloadLimit); // wired 1.1
    message->appendArg(privUploadLimit); // wired 1.1
    message->appendArg(m_privileges & Qws::PrivilegeChangeChatTopic ? 1 : 0); // wired 1.1

}




/*! Set the user image from PNG data and return true if the task was successful.
*/
bool QwcUserInfo::setImageFromData(const QByteArray data)
{ return m_userIcon.loadFromData(data); }

/*! Returns the user image as QImage.
*/
const QImage & QwcUserInfo::userImage() const
{ return m_userIcon; }

/*! Set the user image to \a image.
*/
void QwcUserInfo::setUserImage(const QImage &image)
{ m_userIcon = image; }
