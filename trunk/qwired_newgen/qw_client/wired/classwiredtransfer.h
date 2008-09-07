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

 
#ifndef WIREDTRANSFER_H
#define WIREDTRANSFER_H

#include <QtCore>

/**
	@author Bastian Bense <bastibense@gmail.com>
 */
class ClassWiredTransfer{
public:
    ClassWiredTransfer();
    ~ClassWiredTransfer();
    ClassWiredTransfer(const ClassWiredTransfer &);
	
    int pStatus; // 0=waiting for stat, 1=queued/waiting, 2=running
    QString pLocalPath; // path to file on local disk
    QString pRemotePath; // path to file on server
    QString pHash; // hash for the file transfer
    QString pChecksum; // checksum for the file
    int pTransferType; // 0=download, 1=upload
    qlonglong pOffset; // first byte for transfer
    qlonglong pTotalSize; // total length of file
    qlonglong pDoneSize; // transferred size of data
    int pQueuePosition; // position within the queue
    
	bool pEncryptTransfer; // if true, transfer will be encrypted
	int pCurrentSpeed; // the current speed, updated during transfer, otherwise 0
    void calcLocalChecksum();
	QString fileName() const;
    
    
};

Q_DECLARE_METATYPE(ClassWiredTransfer)


#endif
