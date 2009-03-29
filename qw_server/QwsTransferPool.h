#ifndef QWSTRANSFERPOOL_H
#define QWSTRANSFERPOOL_H

#include "QwsTransferInfo.h"
#include <QQueue>
#include <QMutex>

class QwsTransferPool
{

public:
    QwsTransferPool();

    bool hasTransferWithHash(const QByteArray hash);
    void appendTransferToQueue(const QwsTransferInfo transfer);
    QwsTransferInfo takeTransferFromQueueWithHash(const QByteArray hash);

    int deleteTransfersWithUserId(int userId);
    QList<QwsTransferInfo> findTransfersWithUserId(int userId);
    QwsTransferInfo firstTransferWithUserId(int userId);

private:
    /*! The mutex that protects the queue from getting corrupted. */
    QMutex queueMutex;
    /*! The server-wide list of transfers. */
    QQueue<QwsTransferInfo> transferQueue;

};


#endif // QWSTRANSFERPOOL_H
