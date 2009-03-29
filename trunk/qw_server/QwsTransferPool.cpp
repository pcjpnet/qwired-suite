#include "QwsTransferPool.h"


QwsTransferPool::QwsTransferPool()
{

}


/*! Remove and return the transfer identified by the hash \a hash from the queue. This method is
    thread-safe. It is assumed that the existence of a transfer is checked previously with the
    \a hasTransferWithHas() method, but will return a null-QwsTransferInfo object if the has is
    not found.
*/
QwsTransferInfo QwsTransferPool::takeTransferFromQueueWithHash(const QByteArray hash)
{
    QMutexLocker locker(&queueMutex);

    QMutableListIterator<QwsTransferInfo> i(transferQueue);
    while (i.hasNext()) {
        QwsTransferInfo item = i.next();
        if (item.hash == hash) {
            i.remove();
            return item;
        }
    }
    return QwsTransferInfo();
}


 /*! Check if the pool contains a transfer with the provided hash \a hash. Returns true if a
     transfer exists. This method is thread-safe. */
bool QwsTransferPool::hasTransferWithHash(const QByteArray hash)
{
    QMutexLocker locker(&queueMutex);

    QListIterator<QwsTransferInfo> i(transferQueue);
    while (i.hasNext()) {
        QwsTransferInfo item = i.next();
        if (item.hash == hash) {
            return true;
        }
    }
    return false;

}

/*! Append a new transfer to the queue and return the position of the added transfer within the
    queue. This method is thread-safe.
*/
void QwsTransferPool::appendTransferToQueue(const QwsTransferInfo transfer)
{
    QMutexLocker locker(&queueMutex);
    transferQueue.append(transfer);
}


/*! Delete all transfers in this queue which are owned by the session with the ID \a userId.
*/
int QwsTransferPool::deleteTransfersWithUserId(int userId)
{
    QMutexLocker locker(&queueMutex);
    int deleteCounter = 0;

    QMutableListIterator<QwsTransferInfo> i(transferQueue);
    while (i.hasNext()) {
        QwsTransferInfo item = i.next();
        if (item.targetUserId == userId) {
            i.remove();
            deleteCounter += 1;
        }
    }
    return deleteCounter;
}


/*! Returns the transfers with a user ID of \a userId.
*/
QList<QwsTransferInfo> QwsTransferPool::findTransfersWithUserId(int userId)
{
    QList<QwsTransferInfo> resultList;

    QListIterator<QwsTransferInfo> i(transferQueue);
    while (i.hasNext()) {
        QwsTransferInfo item = i.next();
        if (item.targetUserId == userId) {
            resultList.append(item);
        }
    }

    return resultList;
}


/*! Returns the first transfer info item for the user identified by \a userId.
*/
QwsTransferInfo QwsTransferPool::firstTransferWithUserId(int userId)
{
    QListIterator<QwsTransferInfo> i(transferQueue);
    while (i.hasNext()) {
        QwsTransferInfo item = i.next();
        if (item.targetUserId == userId) {
            return item;
        }
    }

}
