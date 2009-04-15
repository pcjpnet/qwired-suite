#ifndef QWCFILETRANSFERSOCKET_H
#define QWCFILETRANSFERSOCKET_H


#include "QwcTransferInfo.h"

#include <QThread>
#include <QTime>
#include <QPointer>
#include <QString>
#include <QList>
#include <QSslSocket>
#include <QFile>

class QwcTransferSocket : public QObject
{

    Q_OBJECT

public:
    void run();
    QwcTransferSocket(QObject *parent=0);
    ~QwcTransferSocket();

    QwcTransferInfo pTransfer;

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
    void fileTransferStarted(const QwcTransferInfo);
    void fileTransferDone(const QwcTransferInfo);
    void fileTransferFileDone(const QwcTransferInfo);
    void fileTransferError(const QwcTransferInfo);
    void fileTransferStatus(const QwcTransferInfo);
    void fileTransferSocketError(QAbstractSocket::SocketError);

public slots:
    void cancelTransfer();

};

#endif
