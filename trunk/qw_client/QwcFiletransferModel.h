#ifndef QWCFILETRANSFERMODEL_H
#define QWCFILETRANSFERMODEL_H

#include <QAbstractListModel>

#include "QwcSocket.h"
#include "QwcTransferSocket.h"

#include <QPointer>

class QwcFiletransferModel : public QAbstractListModel
{
    Q_OBJECT
		
public:
    QwcFiletransferModel(QObject *parent = 0);

    QVariant data(const QModelIndex &index, int role=Qt::DisplayRole) const;
    int rowCount(const QModelIndex &parent=QModelIndex()) const;

    void setSocket(QwcSocket *socket);


private:
    /*! The connection socket context for this model. */
    QPointer<QwcSocket> socket;


private slots:
    void updateTransfers(const QwcTransferInfo &transfer);
    void reloadTransfers();
};

#endif
