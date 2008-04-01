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
 
#include "classwireduser.h"
#include <QCryptographicHash>

ClassWiredUser::ClassWiredUser()
{
	pUserID = 0;
	pIdle = false;
	pAdmin = false;
	pIcon = 0;
	pAccountType = 0;

	// Default privs

	privGetUserInfo = true;
	privBroadcast = false;
	privPostNews = true;
	privClearNews = false;
	privDownload = false;
	privUpload = false;
	privUploadAnywhere = false;
	privCreateFolders = false;
	privAlterFiles = false;
	privDeleteFiles = false;
	privViewDropboxes = false;;
	privCreateAccounts = false;
	privEditAccounts = false;
	privDeleteAccounts = false;
	privElevatePrivileges = false;
	privKickUsers = false;
	privBanUsers = false;
	privCannotBeKicked = false;
	privDownloadSpeed = 0;
	privUploadSpeed = 0;
	privDownloadLimit = 0;
	privUploadLimit = 0;
	privChangeTopic = false;
}

ClassWiredUser::ClassWiredUser(QList<QByteArray> theParams) {
	// Create the object from an incoming user list record.
	pUserID = theParams.value(1,"").toInt();
	pIdle = theParams.value(2,"").toInt()==1;
	pAdmin = theParams.value(3,"").toInt()==1;
	pIcon = theParams.value(4,"").toInt();
	pNick = QString::fromUtf8(theParams.value(5,""));
	pLogin = QString::fromUtf8(theParams.value(6,""));
	pIP = theParams.value(7,"");
	pHost = theParams.value(8,"");
	pStatus = QString::fromUtf8(theParams.value(9,""));
	pImage = QByteArray::fromBase64(theParams.value(10,""));
	pAccountType = 0;
	//qDebug() << "ClassWiredUser: Created "<<pUserID<<pNick<<pIP<<pStatus<<pIdle<<pAdmin<<pIcon;
}

ClassWiredUser::~ClassWiredUser()
{ }

ClassWiredUser ClassWiredUser::fromUserInfo(QList<QByteArray> theParams) {
	// Fill parameters from a Get User Info response.
	
	ClassWiredUser usr;
	usr.pAccountType = 0;
	usr.pUserID = theParams.value(0,"").toInt();
	usr.pIdle = theParams.value(1,"").toInt()==1;
	usr.pAdmin = theParams.value(2,"").toInt()==1;
	usr.pIcon = theParams.value(3,"").toInt();
	usr.pNick = QString::fromUtf8(theParams.value(4,""));
	usr.pLogin = QString::fromUtf8(theParams.value(5,""));
	usr.pIP = theParams.value(6,"");
	usr.pHost = theParams.value(7,"");
	usr.pClientVersion = QString::fromUtf8(theParams.value(8,""));
	usr.pCipherName = theParams.value(9,"");
	usr.pCipherBits = theParams.value(10,"").toInt();
	usr.pLoginTime = QDateTime::fromString( theParams.value(11,""), Qt::ISODate);
	usr.pIdleTime = QDateTime::fromString( theParams.value(12,""), Qt::ISODate);
	usr.pDownloads = theParams.value(13,"");
	usr.pUploads = theParams.value(14,"");
	usr.pStatus = theParams.value(15,"");
	usr.pImage = QByteArray::fromBase64(theParams.value(16,""));
	return usr;
	
}

QByteArray ClassWiredUser::privilegesFlags() {
	QByteArray tmpPrivs;
	if((pAccountType==0 && pGroupName.isEmpty()) || pAccountType==1) {
		tmpPrivs += QByteArray::number(privGetUserInfo); tmpPrivs += 0x1C;
		tmpPrivs += QByteArray::number(privBroadcast); tmpPrivs += 0x1C;
		tmpPrivs += QByteArray::number(privPostNews); tmpPrivs += 0x1C;
		tmpPrivs += QByteArray::number(privClearNews); tmpPrivs += 0x1C;
		tmpPrivs += QByteArray::number(privDownload); tmpPrivs += 0x1C;
		tmpPrivs += QByteArray::number(privUpload); tmpPrivs += 0x1C;
		tmpPrivs += QByteArray::number(privUploadAnywhere); tmpPrivs += 0x1C;
		tmpPrivs += QByteArray::number(privCreateFolders); tmpPrivs += 0x1C;
		tmpPrivs += QByteArray::number(privAlterFiles); tmpPrivs += 0x1C;
		tmpPrivs += QByteArray::number(privDeleteFiles); tmpPrivs += 0x1C;
		tmpPrivs += QByteArray::number(privViewDropboxes); tmpPrivs += 0x1C;
		tmpPrivs += QByteArray::number(privCreateAccounts); tmpPrivs += 0x1C;
		tmpPrivs += QByteArray::number(privEditAccounts); tmpPrivs += 0x1C;
		tmpPrivs += QByteArray::number(privDeleteAccounts); tmpPrivs += 0x1C;
		tmpPrivs += QByteArray::number(privElevatePrivileges); tmpPrivs += 0x1C;
		tmpPrivs += QByteArray::number(privKickUsers); tmpPrivs += 0x1C;
		tmpPrivs += QByteArray::number(privBanUsers); tmpPrivs += 0x1C;
		tmpPrivs += QByteArray::number(privCannotBeKicked); tmpPrivs += 0x1C;
		tmpPrivs += QByteArray::number(privDownloadSpeed); tmpPrivs += 0x1C;
		tmpPrivs += QByteArray::number(privUploadSpeed); tmpPrivs += 0x1C;
		tmpPrivs += QByteArray::number(privDownloadLimit); tmpPrivs += 0x1C;
		tmpPrivs += QByteArray::number(privUploadLimit); tmpPrivs += 0x1C;
		tmpPrivs += QByteArray::number(privChangeTopic);
	} else {
		// Empty flags if a group is specified for a user or this is a group.
		tmpPrivs += QByteArray("0"); tmpPrivs += 0x1C;
		tmpPrivs += QByteArray("0"); tmpPrivs += 0x1C;
		tmpPrivs += QByteArray("0"); tmpPrivs += 0x1C;
		tmpPrivs += QByteArray("0"); tmpPrivs += 0x1C;
		tmpPrivs += QByteArray("0"); tmpPrivs += 0x1C;
		tmpPrivs += QByteArray("0"); tmpPrivs += 0x1C;
		tmpPrivs += QByteArray("0"); tmpPrivs += 0x1C;
		tmpPrivs += QByteArray("0"); tmpPrivs += 0x1C;
		tmpPrivs += QByteArray("0"); tmpPrivs += 0x1C;
		tmpPrivs += QByteArray("0"); tmpPrivs += 0x1C;
		tmpPrivs += QByteArray("0"); tmpPrivs += 0x1C;
		tmpPrivs += QByteArray("0"); tmpPrivs += 0x1C;
		tmpPrivs += QByteArray("0"); tmpPrivs += 0x1C;
		tmpPrivs += QByteArray("0"); tmpPrivs += 0x1C;
		tmpPrivs += QByteArray("0"); tmpPrivs += 0x1C;
		tmpPrivs += QByteArray("0"); tmpPrivs += 0x1C;
		tmpPrivs += QByteArray("0"); tmpPrivs += 0x1C;
		tmpPrivs += QByteArray("0"); tmpPrivs += 0x1C;
		tmpPrivs += QByteArray("0"); tmpPrivs += 0x1C;
		tmpPrivs += QByteArray("0"); tmpPrivs += 0x1C;
		tmpPrivs += QByteArray("0"); tmpPrivs += 0x1C;
		tmpPrivs += QByteArray("0"); tmpPrivs += 0x1C;
		tmpPrivs += QByteArray("0");
	}
	return tmpPrivs;
}


/**
 * Set the privileges of this user object to the ones provided by theParams. This is
 * used internally by WiredSocket to manage the privileges of a user and session.
 * @param theParams A list of flags representing the privileges.
 */
void ClassWiredUser::setFromPrivileges(const QList<QByteArray> theParams) {
	if( theParams.count()>=23 ) {
		privGetUserInfo = theParams.value(0).toInt();
		privBroadcast = theParams.value(1).toInt();
		privPostNews = theParams.value(2).toInt();
		privClearNews = theParams.value(3).toInt();
		privDownload = theParams.value(4).toInt();
		privUpload = theParams.value(5).toInt();
		privUploadAnywhere = theParams.value(6).toInt();
		privCreateFolders = theParams.value(7).toInt();
		privAlterFiles = theParams.value(8).toInt();
		privDeleteFiles = theParams.value(9).toInt();
		privViewDropboxes = theParams.value(10).toInt();
		privCreateAccounts = theParams.value(11).toInt();
		privEditAccounts = theParams.value(12).toInt();
		privDeleteAccounts = theParams.value(13).toInt();
		privElevatePrivileges = theParams.value(14).toInt();
		privKickUsers = theParams.value(15).toInt();
		privBanUsers = theParams.value(16).toInt();
		privCannotBeKicked = theParams.value(17).toInt();
		privDownloadSpeed = theParams.value(18).toInt();
		privUploadSpeed = theParams.value(19).toInt();
		privDownloadLimit = theParams.value(20).toInt();
		privUploadLimit = theParams.value(21).toInt();
		privChangeTopic = theParams.value(22).toInt();
		return;
	}
	qDebug() << "ClassWiredUser: Failed to set privileges from privileges block!";
}

/**
 * Return the SHA-1 crypt of the password in this user object. If the password is empty,
 * no crypting will be done.
 * @return The SHA-1 hash of the password.
 */
QString ClassWiredUser::cryptedPassword() {
	if( pPassword.isEmpty() )
		return QString("");
	QByteArray tmpDat = QCryptographicHash::hash(pPassword.toUtf8(), QCryptographicHash::Sha1);
	return QString::fromUtf8(tmpDat.toHex());
}

/**
 * Return a ByteArray specially formatted for the user listing responses (310)
 * @return 
 */
QByteArray ClassWiredUser::userListEntry() const {
	QByteArray ba;
	ba += QByteArray::number(pUserID); ba += 0x1C;
	ba += QByteArray::number(pIdle); ba += 0x1C;
	ba += QByteArray::number(pAdmin); ba += 0x1C;
	ba += QByteArray::number(0); ba += 0x1C; // icon, unused since 1.1
	ba += pNick.toUtf8(); ba += 0x1C;
	ba += pLogin.toUtf8(); ba += 0x1C;
	ba += pIP.toUtf8(); ba += 0x1C;
	ba += pHost.toUtf8(); ba += 0x1C;
	ba += pStatus.toUtf8(); ba += 0x1C;
	ba += pImage;
	
	return ba;
}

/**
 * Return a ByteArray specially formatted for the User Status Change messages.
 */
QByteArray ClassWiredUser::userStatusEntry() const {
	QByteArray ba;
	ba += QByteArray::number(pUserID); ba += 0x1C;
	ba += QByteArray::number(pIdle); ba += 0x1C;
	ba += QByteArray::number(pAdmin); ba += 0x1C;
	ba += QByteArray::number(0); ba += 0x1C; // icon, unused since 1.1
	ba += pNick.toUtf8(); ba += 0x1C;
	ba += pStatus.toUtf8();
	return ba;
}


