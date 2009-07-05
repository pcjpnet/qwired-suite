#include "QwcFiletransferModel.h"


QwcFiletransferModel::QwcFiletransferModel(QObject *parent) : QAbstractListModel(parent)
{
}


/*! Return the number of active transfers. We address the active sockets first, then the inactive
    /queued ones.
*/
int QwcFiletransferModel::rowCount(const QModelIndex &) const
{
    if (socket.isNull()) { return 0; }
    return socket->allTransferSockets().count() + socket->allQueuedTransfers().count();
}


/*! Return the n'th row of the transfer list. Since we address the active transfers first, the index
    has to be corrected for the queued transfers (substract the number of active transfers).
*/
QVariant QwcFiletransferModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || socket.isNull()) { return QVariant(); }
    if (role != Qt::UserRole) { return QVariant(); }
    if (index.row() >= rowCount()) { return QVariant(); }

    if (index.row() < socket->allTransferSockets().count()) {
        // Active transfer
        QwcTransferSocket *transferSocket = socket->allTransferSockets().value(index.row());
        if (!transferSocket) { return QVariant(); }
        return QVariant::fromValue(transferSocket->transferInfo);

    } else if (index.row() >= socket->allTransferSockets().count() && index.row() < rowCount()) {
        // Queued transfer
        return QVariant::fromValue(socket->allQueuedTransfers()
                                   .value(index.row()-socket->allTransferSockets().count()));
    }

    return QVariant();
}


/*! Set the active socket for the transfer model.
*/
void QwcFiletransferModel::setSocket(QwcSocket *socket)
{
    if (!socket) { return; }
    this->socket = socket;
    connect(socket, SIGNAL(fileTransferStatus(QwcTransferInfo)),
            this, SLOT(updateTransfers(QwcTransferInfo)));
    connect(socket, SIGNAL(fileTransferQueueChanged(QwcTransferInfo)),
            this, SLOT(reloadTransfers()));
}



/*! Manually update the status of a single transfer using the model internal update mechanism.
*/
void QwcFiletransferModel::updateTransfers(const QwcTransferInfo &transfer)
{
    qDebug() << "update transfer";
    for (int i = 0; i < rowCount(); i++) {
        QModelIndex itemIndex = createIndex(i, 0);
        QwcTransferInfo existingTransfer = data(itemIndex, Qt::UserRole).value<QwcTransferInfo>();
        if (transfer.file.path == existingTransfer.file.path) {
            emit dataChanged(itemIndex, itemIndex);
            return;
        }
    }
}


/*! Reset the complete model and rebuild the list of transfers.
*/
void QwcFiletransferModel::reloadTransfers()
{
    reset();
}

