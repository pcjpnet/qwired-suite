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
 
#ifndef WIREDTRANSFERSOCKET_H
#define WIREDTRANSFERSOCKET_H

#include <QObject>
#include <QtCore>
#include <QtNetwork>
#include "classwiredtransfer.h"

/**
	@author Bastian Bense <bastibense@gmail.com>
 */
class WiredTransferSocket
	: public QThread
{
	
Q_OBJECT
		
public:
	void run();
    ~WiredTransferSocket();
    ClassWiredTransfer pTransfer;
	
    QString pServerHost;
    int pServerPort;
    void setServer(QString theServer, int thePort);
    int pTransferLimit;
	bool isActive();

    
private:
	void sendHandshake();
	void uploadData();
	void downloadData();
	void calculateSpeed();
	QTime pTimer;
	QPointer<QSslSocket> pSocket;
    QPointer<QFile> pFile;
    
	qlonglong pLastDone; // amount of bytes transferrd since last measurement
	int pTimerID;
	void startTransfer();
	void killTransfer();
	
	bool pSendingFile;
	QList<int> pSpeedList;
	
signals:
    void fileTransferStarted(const ClassWiredTransfer);
    void fileTransferDone(const ClassWiredTransfer);
	void fileTransferFileDone(const ClassWiredTransfer);
    void fileTransferError(const ClassWiredTransfer);
    void fileTransferStatus(const ClassWiredTransfer);  
	void fileTransferSocketError(QAbstractSocket::SocketError);
	
public slots:
    void cancelTransfer();

};

#endif
