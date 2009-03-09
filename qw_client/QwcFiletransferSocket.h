#ifndef QWCFILETRANSFERSOCKET_H
#define QWCFILETRANSFERSOCKET_H


#include "QwcFiletransferInfo.h"

#include <QThread>
#include <QTime>
#include <QPointer>
#include <QString>
#include <QList>
#include <QSslSocket>

class QwcFiletransferSocket : public QThread
{

    Q_OBJECT

public:
    void run();
    QwcFiletransferSocket(QObject *parent=0);
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
