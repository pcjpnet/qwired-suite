#include "QwcTransferPool.h"


QwcTransferPool::QwcTransferPool()
{

}


/*! Remove and return the transfer identified by the hash \a hash from the queue. This method is
    thread-safe. It is assumed that the existence of a transfer is checked previously with the
    \a hasTransferWithHas() method, but will return a null-QwcTransferInfo object if the has is
    not found.
*/
QwcTransferInfo QwcTransferPool::takeTransferFromQueueWithHash(const QByteArray hash)
{
    QMutableListIterator<QwcTransferInfo> i(transferQueue);
    while (i.hasNext()) {
        QwcTransferInfo item = i.next();
        if (item.hash == hash) {
            i.remove();
            return item;
        }
    }
    return QwcTransferInfo();
}


/*! Find a transfer info with the given hash and return it. If no hash is found, a default-
    constructed value is returned. Please use hasTransferWithHash() to check if a hash exists.
*/
QwcTransferInfo QwcTransferPool::findTransferWithHash(const QByteArray hash) const
{
    QListIterator<QwcTransferInfo> i(transferQueue);
    while (i.hasNext()) {
        QwcTransferInfo item = i.next();
        if (item.hash == hash) {
            return item;
        }
    }
    return QwcTransferInfo();
}


 /*! Check if the pool contains a transfer with the provided hash \a hash. Returns true if a
     transfer exists. This method is thread-safe. */
bool QwcTransferPool::hasTransferWithHash(const QByteArray hash)
{
    QListIterator<QwcTransferInfo> i(transferQueue);
    while (i.hasNext()) {
        QwcTransferInfo item = i.next();
        if (item.hash == hash) {
            return true;
        }
    }
    return false;

}


/*! Append a new transfer to the queue and return the position of the added transfer within the
    queue. If a transfer with the same hash already exists, it is replaced.
*/
void QwcTransferPool::appendTransferToQueue(const QwcTransferInfo transfer)
{
    // Discard the old hash if it exists
    takeTransferFromQueueWithHash(transfer.hash.toAscii());
    // Prepend the new one (not append, because it would be at the end of the queue)
    transferQueue.prepend(transfer);
}


/*! Returns all transfer entries from the queue as a QList.
*/
QList<QwcTransferInfo> QwcTransferPool::allTransfers() const
{
    return transferQueue;
}


/*! Take the first queued transfer from the queue and return it.
*/
QwcTransferInfo QwcTransferPool::takeFirstFromQueue()
{
    return transferQueue.takeFirst();
}
