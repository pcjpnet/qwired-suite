#ifndef QWCFILETRANSFERSOCKET_H
#define QWCFILETRANSFERSOCKET_H

#include <QObject>
#include <QtCore>
#include <QtNetwork>

#include "QwcFiletransferInfo.h"

/**
	@author Bastian Bense <bastibense@gmail.com>
 */
class QwcFiletransferSocket
	: public QThread
{
	
Q_OBJECT
		
public:
	void run();
    ~QwcFiletransferSocket();
    QwcFiletransferInfo pTransfer;
	
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
    void fileTransferStarted(const QwcFiletransferInfo);
    void fileTransferDone(const QwcFiletransferInfo);
    void fileTransferFileDone(const QwcFiletransferInfo);
    void fileTransferError(const QwcFiletransferInfo);
    void fileTransferStatus(const QwcFiletransferInfo);
	void fileTransferSocketError(QAbstractSocket::SocketError);
	
public slots:
    void cancelTransfer();

};

#endif
