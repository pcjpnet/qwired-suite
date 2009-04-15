#ifndef QWCTRANSFERPOOL_H
#define QWCTRANSFERPOOL_H

#include "QwcTransferInfo.h"
#include <QQueue>

class QwcTransferPool
{

public:
    QwcTransferPool();

    bool hasTransferWithHash(const QByteArray hash);
    QwcTransferInfo findTransferWithHash(const QByteArray hash) const;
    void appendTransferToQueue(const QwcTransferInfo transfer);
    QwcTransferInfo takeTransferFromQueueWithHash(const QByteArray hash);
    QwcTransferInfo takeFirstFromQueue();

    QList<QwcTransferInfo> allTransfers() const;

private:
    /*! The server-wide list of transfers. */
    QQueue<QwcTransferInfo> transferQueue;

};


#endif // QWSTRANSFERPOOL_H
