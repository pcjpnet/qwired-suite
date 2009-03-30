#include "QwcFiletransferModel.h"


QwcFiletransferModel::QwcFiletransferModel(QObject *parent) : QAbstractListModel(parent)
{
}


QwcFiletransferModel::~QwcFiletransferModel()
{
}


int QwcFiletransferModel::rowCount(const QModelIndex &) const
{
    if(!pSocket) { return 0; }
    return pSocket->pTransferSockets.count();
}


QVariant QwcFiletransferModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || !pSocket) { return QVariant(); }
    if (role != Qt::UserRole) { return QVariant(); }
    if (index.row() < pSocket->pTransferSockets.count()) {
        QwcFiletransferSocket *tmpSock = pSocket->pTransferSockets.value(index.row());
        return QVariant::fromValue(tmpSock->pTransfer);
    }
    return QVariant();
}


void QwcFiletransferModel::setSocket(QwcSocket *theSocket)
{
    if (!theSocket) { return; }
    pSocket = theSocket;
    connect(pSocket, SIGNAL(fileTransferStatus(QwcFiletransferInfo)),  this, SLOT(updateTransfers(QwcFiletransferInfo)) );
    // 	connect(pSocket, SIGNAL(fileTransferFileDone(QwcFiletransferInfo)), this, SLOT(updateTransfers(QwcFiletransferInfo)) );
    connect(pSocket, SIGNAL(fileTransferStarted(QwcFiletransferInfo)), this, SLOT(reloadTransfers()) );
    connect(pSocket, SIGNAL(fileTransferDone(QwcFiletransferInfo)), this, SLOT(reloadTransfers()) );
    connect(pSocket, SIGNAL(fileTransferError(QwcFiletransferInfo)), this, SLOT(reloadTransfers()) );
}


// The status of an item has updated and should be redrawn
void QwcFiletransferModel::updateTransfers(const QwcFiletransferInfo theTransfer)
{
    QListIterator<QPointer<QwcFiletransferSocket> > i(pSocket->pTransferSockets);
    int tmpIdx = 0;
    while(i.hasNext()) {
        QPointer<QwcFiletransferSocket> tmpP = i.next();
        if(tmpP && tmpP->pTransfer.pHash==theTransfer.pHash) {
            QModelIndex tmpIndex = index(tmpIdx,0);
            emit dataChanged(tmpIndex, tmpIndex);
        }
        tmpIdx++;
    }
}


void QwcFiletransferModel::reloadTransfers()
{
    reset();
}

