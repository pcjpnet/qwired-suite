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
 
#include "classwiredtransfer.h"
#include <QCryptographicHash>

ClassWiredTransfer::ClassWiredTransfer()
{
	pStatus = 0;
	pTotalSize = 0;
	pDoneSize = 0;
	pOffset = 0;
	pTransferType = 0;
	pQueuePosition = 0;
	pEncryptTransfer = true;
	pCurrentSpeed = 0;
}


ClassWiredTransfer::~ClassWiredTransfer()
{ }

/**
 * Calculate the checksum of the first megabyte of the file specified in pLocalPath.
 */
void ClassWiredTransfer::calcLocalChecksum() {
	QFile tmpFile(pLocalPath);
	if( tmpFile.open(QIODevice::ReadOnly) ) {
		// If file exists, calculate the hash.
		QByteArray tmpData = tmpFile.read(1048576); // 1MB for the hash
		QByteArray tmpHash = QCryptographicHash::hash(tmpData, QCryptographicHash::Sha1);
		pChecksum = tmpHash.toHex();
		qDebug() << "Hash of"<<pLocalPath<<"="<<pChecksum<<"size"<<tmpFile.size();
		tmpFile.close();
	}
	
	
}

ClassWiredTransfer::ClassWiredTransfer(const ClassWiredTransfer &orig) {
	pStatus = orig.pStatus;
	pLocalPath = orig.pLocalPath;
	pRemotePath = orig.pRemotePath;
	pHash = orig.pHash;
	pChecksum = orig.pChecksum;
	pTransferType = orig.pTransferType; 
	pOffset = orig.pOffset;
	pTotalSize = orig.pTotalSize;
	pDoneSize = orig.pDoneSize;
	pQueuePosition = orig.pQueuePosition;
    
	pEncryptTransfer = orig.pEncryptTransfer;
	pCurrentSpeed = orig.pCurrentSpeed;
}

QString ClassWiredTransfer::fileName() const {
	return pRemotePath.section("/", -1);
}

