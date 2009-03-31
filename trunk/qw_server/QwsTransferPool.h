#ifndef QWSTRANSFERPOOL_H
#define QWSTRANSFERPOOL_H

#include "QwsTransferInfo.h"
#include <QQueue>

class QwsTransferPool
{

public:
    QwsTransferPool();

    bool hasTransferWithHash(const QByteArray hash);
    void appendTransferToQueue(const QwsTransferInfo transfer);
    QwsTransferInfo takeTransferFromQueueWithHash(const QByteArray hash);

    int deleteTransfersWithUserId(int userId);
    QList<QwsTransferInfo> findTransfersWithUserId(int userId);
    QList<QwsTransferInfo> findWaitingTransfersWithUserId(int userId);
    QwsTransferInfo firstTransferWithUserId(int userId);

private:
    /*! The server-wide list of transfers. */
    QQueue<QwsTransferInfo> transferQueue;

};


#endif // QWSTRANSFERPOOL_H
